/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * Xpiks is distributed under the GNU General Public License, version 3.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QCoreApplication>
#include <QStringList>
#include <QDirIterator>
#include <QImageReader>
#include <QList>
#include <QHash>
#include <QSet>
#include <memory>
#include "artitemsmodel.h"
#include "metadataelement.h"
#include "../Helpers/indiceshelper.h"
#include "../Commands/addartworkscommand.h"
#include "../Commands/removeartworkscommand.h"
#include "../Commands/pastekeywordscommand.h"
#include "../Suggestion/keywordssuggestor.h"
#include "../Commands/commandmanager.h"
#include "artworksrepository.h"
#include "../Models/settingsmodel.h"
#include "../SpellCheck/spellcheckiteminfo.h"
#include "../Common/flags.h"
#include "../Commands/combinededitcommand.h"
#include "../Common/defines.h"
#include "../QMLExtensions/colorsmodel.h"
#include "imageartwork.h"
#include "../Commands/expandpresetcommand.h"
#include "../Helpers/constants.h"
#include "../Helpers/stringhelper.h"
#include "../QuickBuffer/quickbuffer.h"

namespace Models {
    ArtItemsModel::ArtItemsModel(QObject *parent):
        AbstractListModel(parent),
        Common::BaseEntity(),
        // all items before 1024 are reserved for internal models
        m_LastID(1024)
    {}

    ArtItemsModel::~ArtItemsModel() {
        for (auto *artwork: m_ArtworkList) {
            if (artwork->release()) {
                delete artwork;
            } else {
                m_FinalizationList.push_back(artwork);
            }
        }

#ifndef INTEGRATION_TESTS
        qDeleteAll(m_FinalizationList);
#endif
    }

    ArtworkMetadata *ArtItemsModel::createMetadata(const QString &filepath, qint64 directoryID) {
        int id = m_LastID++;
        return new ImageArtwork(filepath, id, directoryID);
    }

    void ArtItemsModel::deleteAllItems() {
        LOG_DEBUG << "#";
        // should be called only from beforeDestruction() !
        // will not cause sync issues on shutdown if no items
        beginResetModel();
        std::deque<ArtworkMetadata *> artworksToDestroy;
        artworksToDestroy.swap(m_ArtworkList);

        size_t size = artworksToDestroy.size();
        for (size_t i = 0; i < size; ++i) {
            ArtworkMetadata *metadata = artworksToDestroy.at(i);
            if (metadata->release()) {
                LOG_INTEGRATION_TESTS << "Destroying metadata for real";
                m_CommandManager->disconnectArtworkSignals(metadata);
                metadata->deepDisconnect();
#ifdef QT_DEBUG
                m_DestroyedList.push_back(metadata);
#endif
                metadata->deleteLater();
            } else {
                LOG_WARNING << "Metadata at index" << i << "is locked. Postponing destruction...";

                metadata->disconnect();
                auto *metadataModel = metadata->getBasicModel();
                metadataModel->disconnect();
                metadataModel->clearModel();

                m_FinalizationList.push_back(metadata);
            }
        }

        m_ArtworkList.clear();
        endResetModel();
    }

    int ArtItemsModel::getModifiedArtworksCount() {
        int modifiedCount = 0;
        size_t size = m_ArtworkList.size();

        for (size_t i = 0; i < size; ++i) {
            if (m_ArtworkList.at(i)->isModified()) {
                modifiedCount++;
            }
        }

        return modifiedCount;
    }

    void ArtItemsModel::updateItems(const QVector<int> &indices, const QVector<int> &roles) {
        QVector<QPair<int, int> > rangesToUpdate;
        QVector<int> sortedIndices(indices);
        qSort(sortedIndices);
        Helpers::indicesToRanges(sortedIndices, rangesToUpdate);
        AbstractListModel::updateItemsInRanges(rangesToUpdate, roles);
    }

    void ArtItemsModel::forceUnselectAllItems() const {
        size_t count = m_ArtworkList.size();

        for (size_t i = 0; i < count; ++i) {
            m_ArtworkList.at(i)->resetSelected();
        }
    }

    void ArtItemsModel::updateAllItems() {
        updateItemsInRanges(QVector<QPair<int, int> >() << qMakePair(0, (int)getArtworksCount() - 1));
    }

    void ArtItemsModel::removeArtworksDirectory(int index) {
        LOG_INFO << "Remove artworks directory at" << index;
        const QString &directory = m_CommandManager->getArtworksRepository()->getDirectory(index);
        LOG_CORE_TESTS << "Removing directory:" << directory;

        QDir dir(directory);
        QString directoryAbsolutePath = dir.absolutePath();

        QVector<int> indicesToRemove;
        size_t size = m_ArtworkList.size();
        indicesToRemove.reserve((int)size);

        for (size_t i = 0; i < size; ++i) {
            ArtworkMetadata *metadata = m_ArtworkList.at(i);
            if (metadata->isInDirectory(directoryAbsolutePath)) {
                indicesToRemove.append((int)i);
            }
        }

        doRemoveItemsAtIndices(indicesToRemove);

        emit modifiedArtworksCountChanged();
    }

    bool ArtItemsModel::removeUnavailableItems() {
        LOG_DEBUG << "#";
        QVector<int> indicesToRemove;
        QVector<QPair<int, int> > rangesToRemove;

        size_t size = m_ArtworkList.size();
        for (size_t i = 0; i < size; ++i) {
            if (m_ArtworkList.at(i)->isUnavailable()) {
                indicesToRemove.append((int)i);
                emit fileWithIndexUnavailable((int)i);
            }
        }

        Helpers::indicesToRanges(indicesToRemove, rangesToRemove);
        doRemoveItemsInRanges(rangesToRemove);

        return !indicesToRemove.empty();
    }

    void ArtItemsModel::generateAboutToBeRemoved() {
        size_t size = m_ArtworkList.size();

        for (size_t i = 0; i < size; ++i) {
            ArtworkMetadata *metadata = m_ArtworkList.at(i);

            if (metadata->isUnavailable()) {
                Common::BasicKeywordsModel *keywordsModel = metadata->getBasicModel();
                keywordsModel->notifyAboutToBeRemoved();
            }
        }
    }

    void ArtItemsModel::removeKeywordAt(int metadataIndex, int keywordIndex) {
        LOG_INFO << "metadata index" << metadataIndex << "| keyword index" << keywordIndex;
        if (0 <= metadataIndex && metadataIndex < getArtworksCount()) {
            ArtworkMetadata *metadata = m_ArtworkList.at(metadataIndex);
            QString removed;

            if (metadata->removeKeywordAt(keywordIndex, removed)) {
                QModelIndex index = this->index(metadataIndex);
                emit dataChanged(index, index, QVector<int>() << IsModifiedRole << KeywordsCountRole);
                m_CommandManager->submitKeywordsForWarningsCheck(metadata);
                metadata->requestBackup();
            }
        }
    }

    void ArtItemsModel::removeLastKeyword(int metadataIndex) {
        LOG_INFO << "index" << metadataIndex;
        if (0 <= metadataIndex && metadataIndex < getArtworksCount()) {
            ArtworkMetadata *metadata = m_ArtworkList.at(metadataIndex);
            QString removed;

            if (metadata->removeLastKeyword(removed)) {
                QModelIndex index = this->index(metadataIndex);
                emit dataChanged(index, index, QVector<int>() << IsModifiedRole << KeywordsCountRole);
                m_CommandManager->submitKeywordsForWarningsCheck(metadata);
                metadata->requestBackup();
            }
        }
    }

    void ArtItemsModel::appendKeyword(int metadataIndex, const QString &keyword) {
        LOG_INFO << "metadata index" << metadataIndex << "| keyword" << keyword;
        if (0 <= metadataIndex && metadataIndex < getArtworksCount()) {
            ArtworkMetadata *metadata = m_ArtworkList.at(metadataIndex);
            if (metadata->appendKeyword(keyword)) {
                QModelIndex index = this->index(metadataIndex);
                emit dataChanged(index, index, QVector<int>() << IsModifiedRole << KeywordsCountRole);
                auto *keywordsModel = metadata->getBasicModel();

                m_CommandManager->submitKeywordForSpellCheck(keywordsModel, keywordsModel->getKeywordsCount() - 1);
                m_CommandManager->submitKeywordsForWarningsCheck(metadata);
                metadata->requestBackup();
            }
        }
    }

    void ArtItemsModel::pasteKeywords(int metadataIndex, const QStringList &keywords) {
        LOG_INFO << "item index" << metadataIndex << "|" << keywords;
        if (metadataIndex >= 0
            && metadataIndex < getArtworksCount()
            && !keywords.empty()) {
            std::vector<MetadataElement> metadataElements;
            QVector<int> selectedIndices;

            // TODO: to be changed in future to the dialog
            // getSelectedItemsIndices(selectedIndices);
            // if (!metadata->getIsSelected()) {
            selectedIndices.append(metadataIndex);
            // }

            metadataElements.reserve(selectedIndices.length());

            bool onlyOneKeyword = keywords.length() == 1;

            if (onlyOneKeyword) {
                LOG_INFO << "Pasting only one keyword. Leaving it in the edit box.";
                return;
            }

            foreach(int index, selectedIndices) {
                ArtworkMetadata *metadata = m_ArtworkList.at(index);

                metadataElements.emplace_back(metadata, index);
            }

            std::shared_ptr<Commands::PasteKeywordsCommand> pasteCommand(new Commands::PasteKeywordsCommand(metadataElements, keywords));
            m_CommandManager->processCommand(pasteCommand);
        }
    }

    void ArtItemsModel::addSuggestedKeywords(int metadataIndex, const QStringList &keywords) {
        LOG_DEBUG << "item index" << metadataIndex;
        if (metadataIndex >= 0
            && metadataIndex < getArtworksCount()
            && !keywords.empty()) {
            std::vector<MetadataElement> metadataElements;

            ArtworkMetadata *metadata = m_ArtworkList.at(metadataIndex);
            metadataElements.emplace_back(metadata, metadataIndex);

            std::shared_ptr<Commands::PasteKeywordsCommand> pasteCommand(new Commands::PasteKeywordsCommand(metadataElements, keywords));
            m_CommandManager->processCommand(pasteCommand);
        }
    }

    void ArtItemsModel::suggestCorrections(int metadataIndex) {
        if (0 <= metadataIndex && metadataIndex < getArtworksCount()) {
            using namespace Common;
            auto flags = 0;
            Common::SetFlag(flags, SuggestionFlags::Description);
            Common::SetFlag(flags, SuggestionFlags::Title);
            Common::SetFlag(flags, SuggestionFlags::Keywords);
            ArtworkMetadata *metadata = m_ArtworkList.at(metadataIndex);
            m_CommandManager->setupSpellCheckSuggestions(metadata, metadataIndex, (SuggestionFlags)flags);
        }
    }

    void ArtItemsModel::backupItem(int metadataIndex) {
        if (0 <= metadataIndex && metadataIndex < getArtworksCount()) {
            ArtworkMetadata *metadata = m_ArtworkList.at(metadataIndex);
            m_CommandManager->saveArtworkBackup(metadata);
        }
    }

    int ArtItemsModel::dropFiles(const QList<QUrl> &urls) {
        LOG_INFO << "Dropped" << urls.count() << "items(s)";
        QList<QUrl> directories, files;
        directories.reserve(urls.count()/2);
        files.reserve(urls.count());

        foreach(const QUrl &url, urls) {
            bool isDirectory = QDir(url.toLocalFile()).exists();

            if (isDirectory) {
                directories.append(url);
            } else {
                files.append(url);
            }
        }

        QStringList filesToImport;
        filesToImport.reserve(files.size() + directories.size() * 10);

        foreach(const QUrl &fileUrl, files) {
            filesToImport.append(fileUrl.toLocalFile());
        }

        foreach(const QUrl &dirUrl, directories) {
            doAddDirectory(dirUrl.toLocalFile(), filesToImport);
        }

        int importedCount = addFiles(filesToImport);
        return importedCount;
    }

    void ArtItemsModel::setSelectedItemsSaved(const QVector<int> &selectedIndices) {
        LOG_INFO << "Setting selected" << selectedIndices.length() << "item(s) saved";
        foreach(int index, selectedIndices) {
            m_ArtworkList.at(index)->resetModified();
        }

        QVector<QPair<int, int> > rangesToUpdate;
        Helpers::indicesToRanges(selectedIndices, rangesToUpdate);
        AbstractListModel::updateItemsInRanges(rangesToUpdate, QVector<int>() << IsModifiedRole);

        updateModifiedCount();
        emit artworksChanged(false);
    }

    void ArtItemsModel::removeSelectedArtworks(QVector<int> &selectedIndices) {
        doRemoveItemsAtIndices(selectedIndices);
    }

    void ArtItemsModel::updateSelectedArtworks(const QVector<int> &selectedIndices) {
        QVector<QPair<int, int> > rangesToUpdate;
        Helpers::indicesToRanges(selectedIndices, rangesToUpdate);
        QVector<int> roles;
        fillStandardRoles(roles);
        AbstractListModel::updateItemsInRanges(rangesToUpdate, roles);

        emit artworksChanged(false);
    }

    void ArtItemsModel::saveSelectedArtworks(const QVector<int> &selectedIndices, bool overwriteAll, bool useBackups) {
        QVector<ArtworkMetadata *> modifiedSelectedArtworks;
        int count = selectedIndices.count();
        modifiedSelectedArtworks.reserve(count/2);

        for (int i = 0; i < count; ++i) {
            int index = selectedIndices.at(i);
            ArtworkMetadata *metadata = getArtwork(index);
            if (metadata != NULL && metadata->isSelected()) {
                if (metadata->isModified() || overwriteAll) {
                    modifiedSelectedArtworks.append(metadata);
                }
            }
        }

        m_CommandManager->writeMetadata(modifiedSelectedArtworks, useBackups);
        m_CommandManager->saveArtworksBackups(modifiedSelectedArtworks);
    }

    ArtworkMetadata *ArtItemsModel::getArtworkMetadata(int index) const {
        ArtworkMetadata *item = NULL;

        if (0 <= index && index < getArtworksCount()) {
            item = m_ArtworkList.at(index);
            QQmlEngine::setObjectOwnership(item, QQmlEngine::CppOwnership);
        }

        return item;
    }

    Common::BasicMetadataModel *ArtItemsModel::getBasicModel(int index) const {
        Common::BasicMetadataModel *keywordsModel = NULL;

        if (0 <= index && index < getArtworksCount()) {
            keywordsModel = m_ArtworkList.at(index)->getBasicModel();
            QQmlEngine::setObjectOwnership(keywordsModel, QQmlEngine::CppOwnership);
        }

        return keywordsModel;
    }

    QSize ArtItemsModel::retrieveImageSize(int metadataIndex) const {
        if (metadataIndex < 0 || metadataIndex >= getArtworksCount()) {
            return QSize();
        }

        ArtworkMetadata *metadata = m_ArtworkList.at(metadataIndex);
        ImageArtwork *image = dynamic_cast<ImageArtwork *>(metadata);

        if (image == NULL) {
            return QSize();
        }

        QSize size;

        if (image->isInitialized()) {
            size = image->getImageSize();
        } else {
            QImageReader reader(image->getFilepath());
            size = reader.size();
            // metadata->setSize(size);
        }

        return size;
    }

    QString ArtItemsModel::retrieveFileSize(int metadataIndex) const {
        if (metadataIndex < 0 || metadataIndex >= getArtworksCount()) {
            return QLatin1String("-");
        }

        ArtworkMetadata *metadata = m_ArtworkList.at(metadataIndex);

        double size = 0;

        if (metadata->isInitialized()) {
            size = metadata->getFileSize();
        } else {
            QFileInfo fi(metadata->getFilepath());
            size = fi.size(); // in bytes
        }

        size /= 1024.0*1024.0;

        QString sizeDescription;
        if (size >= 1) {
            sizeDescription = QString::number(size, 'f', 2) + QLatin1String(" MB");
        } else {
            size *= 1024;
            sizeDescription = QString::number(size, 'f', 2) + QLatin1String(" KB");
        }

        return sizeDescription;
    }

    QString ArtItemsModel::getArtworkFilepath(int metadataIndex) const {
        if (metadataIndex < 0 || metadataIndex >= getArtworksCount()) {
            return QLatin1String("");
        }

        ArtworkMetadata *metadata = m_ArtworkList.at(metadataIndex);
        return metadata->getFilepath();
    }

    QString ArtItemsModel::getArtworkDateTaken(int metadataIndex) const {
        if (metadataIndex < 0 || metadataIndex >= getArtworksCount()) {
            return QLatin1String("");
        }

        ArtworkMetadata *metadata = m_ArtworkList.at(metadataIndex);
        ImageArtwork *image = dynamic_cast<ImageArtwork *>(metadata);
        if (image != NULL) {
            return image->getDateTaken();
        } else {
            return QLatin1String("");
        }
    }

    QString ArtItemsModel::getAttachedVectorPath(int metadataIndex) const {
        if (metadataIndex < 0 || metadataIndex >= getArtworksCount()) {
            return QLatin1String("");
        }

        ArtworkMetadata *metadata = m_ArtworkList.at(metadataIndex);
        ImageArtwork *image = dynamic_cast<ImageArtwork *>(metadata);
        if (image != NULL) {
            return image->getAttachedVectorPath();
        } else {
            return QLatin1String("");
        }
    }

    int ArtItemsModel::addRecentDirectory(const QString &directory) {
        LOG_INFO << directory;
        int filesAdded = addDirectories(QStringList() << directory);
        return filesAdded;
    }

    int ArtItemsModel::addRecentFile(const QString &file) {
        LOG_INFO << file;
        int filesAdded = addFiles(QStringList() << file);
        return filesAdded;
    }

    void ArtItemsModel::initDescriptionHighlighting(int metadataIndex, QQuickTextDocument *document) {
        if (0 <= metadataIndex && metadataIndex < getArtworksCount()) {
            ArtworkMetadata *metadata = m_ArtworkList.at(metadataIndex);
            auto *metadataModel = metadata->getBasicModel();
            SpellCheck::SpellCheckItemInfo *info = metadataModel->getSpellCheckInfo();
            QMLExtensions::ColorsModel *colorsModel = m_CommandManager->getColorsModel();
            info->createHighlighterForDescription(document->textDocument(), colorsModel, metadataModel);
            metadataModel->notifyDescriptionSpellCheck();
        }
    }

    void ArtItemsModel::initTitleHighlighting(int metadataIndex, QQuickTextDocument *document) {
        if (0 <= metadataIndex && metadataIndex < getArtworksCount()) {
            ArtworkMetadata *metadata = m_ArtworkList.at(metadataIndex);
            auto *metadataModel = metadata->getBasicModel();
            SpellCheck::SpellCheckItemInfo *info = metadataModel->getSpellCheckInfo();
            QMLExtensions::ColorsModel *colorsModel = m_CommandManager->getColorsModel();
            info->createHighlighterForTitle(document->textDocument(), colorsModel, metadataModel);
            metadataModel->notifyTitleSpellCheck();
        }
    }

    void ArtItemsModel::editKeyword(int metadataIndex, int keywordIndex, const QString &replacement) {
        LOG_INFO << "metadata index:" << metadataIndex;
        if (0 <= metadataIndex && metadataIndex < getArtworksCount()) {
            ArtworkMetadata *metadata = m_ArtworkList.at(metadataIndex);
            if (metadata->editKeyword(keywordIndex, replacement)) {
                auto *keywordsModel = metadata->getBasicModel();
                m_CommandManager->submitKeywordForSpellCheck(keywordsModel, keywordIndex);
                m_CommandManager->submitKeywordsForWarningsCheck(metadata);
                metadata->requestBackup();
            }
        }
    }

    void ArtItemsModel::plainTextEdit(int metadataIndex, const QString &rawKeywords, bool spaceIsSeparator) {
        LOG_DEBUG << "Plain text edit for item" << metadataIndex;
        if (0 <= metadataIndex && metadataIndex < getArtworksCount()) {
            ArtworkMetadata *metadata = m_ArtworkList.at(metadataIndex);

            QVector<QChar> separators;
            separators << QChar(',');
            if (spaceIsSeparator) { separators << QChar(' '); }
            QStringList keywords;
            Helpers::splitKeywords(rawKeywords.trimmed(), separators, keywords);

            std::vector<MetadataElement> items;
            items.emplace_back(metadata, metadataIndex);

            Common::CombinedEditFlags flags = Common::CombinedEditFlags::None;
            Common::SetFlag(flags, Common::CombinedEditFlags::EditKeywords);
            std::shared_ptr<Commands::CombinedEditCommand> combinedEditCommand(new Commands::CombinedEditCommand(
                    flags,
                    items,
                    "", "",
                    keywords));

            m_CommandManager->processCommand(combinedEditCommand);
            updateItemAtIndex(metadataIndex);
        }
    }

    void ArtItemsModel::detachVectorsFromSelected(const QVector<int> &selectedIndices) {
        QVector<int> indicesToUpdate;
        indicesToUpdate.reserve(selectedIndices.length());
        Models::ArtworksRepository *artworksRepository = m_CommandManager->getArtworksRepository();

        foreach(int index, selectedIndices) {
            ArtworkMetadata *metadata = m_ArtworkList.at(index);
            ImageArtwork *image = dynamic_cast<ImageArtwork *>(metadata);

            if (image != NULL) {
                const QString vectorPath = image->getAttachedVectorPath();
                image->detachVector();
                artworksRepository->removeVector(vectorPath);
                indicesToUpdate.append(index);
            }
        }

        LOG_INFO << indicesToUpdate.length() << "item(s) affected";

        if (!indicesToUpdate.isEmpty()) {
            QVector<QPair<int, int> > rangesToUpdate;
            Helpers::indicesToRanges(indicesToUpdate, rangesToUpdate);
            AbstractListModel::updateItemsInRanges(rangesToUpdate, QVector<int>() << HasVectorAttachedRole);
        }
    }

    void ArtItemsModel::expandPreset(int metadataIndex, int keywordIndex, int presetIndex) {
        LOG_INFO << "item" << metadataIndex << "keyword" << keywordIndex << "preset" << presetIndex;

        if (0 <= metadataIndex && metadataIndex < getArtworksCount()) {
            ArtworkMetadata *metadata = m_ArtworkList.at(metadataIndex);
            std::shared_ptr<Commands::ExpandPresetCommand> expandPresetCommand(new Commands::ExpandPresetCommand(MetadataElement(metadata, metadataIndex), presetIndex, keywordIndex));
            std::shared_ptr<Commands::ICommandResult> result = m_CommandManager->processCommand(expandPresetCommand);
            Q_UNUSED(result);
        }
    }

    void ArtItemsModel::expandLastAsPreset(int metadataIndex) {
        LOG_INFO << "item" << metadataIndex;

        if (0 <= metadataIndex && metadataIndex < getArtworksCount()) {
            ArtworkMetadata *metadata = m_ArtworkList.at(metadataIndex);
            auto *basicModel = metadata->getBasicModel();
            int keywordIndex = basicModel->getKeywordsCount() - 1;
            QString lastKeyword = basicModel->retrieveKeyword(keywordIndex);

            auto *presetsModel = m_CommandManager->getPresetsModel();
            int presetIndex = -1;
            if (presetsModel->tryFindSinglePresetByName(lastKeyword, false, presetIndex)) {
                std::shared_ptr<Commands::ExpandPresetCommand> expandPresetCommand(new Commands::ExpandPresetCommand(MetadataElement(metadata, metadataIndex), presetIndex, keywordIndex));
                std::shared_ptr<Commands::ICommandResult> result = m_CommandManager->processCommand(expandPresetCommand);
                Q_UNUSED(result);
            }
        }
    }

    void ArtItemsModel::addPreset(int metadataIndex, int presetIndex) {
        LOG_INFO << "item" << metadataIndex << "preset" << presetIndex;

        if (0 <= metadataIndex && metadataIndex < rowCount()) {
            ArtworkMetadata *metadata = m_ArtworkList.at(metadataIndex);
            std::shared_ptr<Commands::ExpandPresetCommand> expandPresetCommand(new Commands::ExpandPresetCommand(MetadataElement(metadata, metadataIndex), presetIndex));
            std::shared_ptr<Commands::ICommandResult> result = m_CommandManager->processCommand(expandPresetCommand);
            Q_UNUSED(result);
        }
    }

    void ArtItemsModel::initSuggestion(int metadataIndex) {
        LOG_INFO << "item" << metadataIndex;
#ifndef CORE_TESTS
        if (0 <= metadataIndex && metadataIndex < rowCount()) {
            auto *suggestor = m_CommandManager->getKeywordsSuggestor();
            ArtworkMetadata *metadata = m_ArtworkList.at(metadataIndex);
            suggestor->setExistingKeywords(metadata->getKeywordsSet());
        }
#endif
    }

    void ArtItemsModel::fillFromQuickBuffer(int metadataIndex) {
        LOG_INFO << "item" << metadataIndex;

        if (0 <= metadataIndex && metadataIndex < rowCount()) {
            ArtworkMetadata *metadata = m_ArtworkList.at(metadataIndex);
            auto *quickBuffer = m_CommandManager->getQuickBuffer();

            std::vector<MetadataElement> items;
            items.emplace_back(metadata, metadataIndex);

            Common::CombinedEditFlags flags = Common::CombinedEditFlags::None;

            QString title = quickBuffer->getTitle();
            QString description = quickBuffer->getDescription();
            QStringList keywords = quickBuffer->getKeywords();

            if (!title.isEmpty()) { Common::SetFlag(flags, Common::CombinedEditFlags::EditTitle); }
            if (!description.isEmpty()) { Common::SetFlag(flags, Common::CombinedEditFlags::EditDescription); }
            if (!keywords.empty()) { Common::SetFlag(flags, Common::CombinedEditFlags::EditKeywords); }

            std::shared_ptr<Commands::CombinedEditCommand> combinedEditCommand(new Commands::CombinedEditCommand(
                                                                                   flags, items, description, title, keywords));

            m_CommandManager->processCommand(combinedEditCommand);
            updateItemAtIndex(metadataIndex);
        }
    }

    int ArtItemsModel::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return (int)getArtworksCount();
    }

    QVariant ArtItemsModel::data(const QModelIndex &index, int role) const {
        int row = index.row();

        if (row < 0 || row >= getArtworksCount()) {
            return QVariant();
        }

        ArtworkMetadata *metadata = m_ArtworkList.at(row);
        switch (role) {
            case ArtworkDescriptionRole:
                return metadata->getDescription();
            case ArtworkFilenameRole:
                return metadata->getFilepath();
            case ArtworkTitleRole:
                return metadata->getTitle();
            case KeywordsStringRole: {
                Common::BasicKeywordsModel *keywordsModel = metadata->getBasicModel();
                return keywordsModel->getKeywordsString();
            }
            case IsModifiedRole:
                return metadata->isModified();
            case IsSelectedRole:
                return metadata->isSelected();
            case KeywordsCountRole: {
                Common::BasicKeywordsModel *keywordsModel = metadata->getBasicModel();
                return keywordsModel->getKeywordsCount();
            }
            case HasVectorAttachedRole: {
                ImageArtwork *image = dynamic_cast<ImageArtwork *>(metadata);
                return (image != NULL) && image->hasVectorAttached();
            }
            case BaseFilenameRole:
                return metadata->getBaseFilename();
            default:
                return QVariant();
        }
    }

    Qt::ItemFlags ArtItemsModel::flags(const QModelIndex &index) const {
        int row = index.row();

        if (row < 0 || row >= getArtworksCount()) {
            return Qt::ItemIsEnabled;
        }

        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
    }

    bool ArtItemsModel::setData(const QModelIndex &index, const QVariant &value, int role) {
        int row = index.row();

        if (row < 0 || row >= getArtworksCount()) {
            return false;
        }

        ArtworkMetadata *metadata = m_ArtworkList.at(row);
        if (metadata->isLockedForEditing()) { return false; }

        int roleToUpdate = 0;
        bool needToUpdate = false;
        switch (role) {
            case EditArtworkDescriptionRole:
            LOG_FOR_DEBUG << "item" << row << "description" << value;
                needToUpdate = metadata->setDescription(value.toString());
                roleToUpdate = ArtworkDescriptionRole;
                break;
            case EditArtworkTitleRole:
            LOG_FOR_DEBUG << "item" << row << "title" << value;
                needToUpdate = metadata->setTitle(value.toString());
                roleToUpdate = ArtworkTitleRole;
                break;
            case EditIsSelectedRole:
            LOG_FOR_DEBUG << "item" << row << "isSelected" << value;
                needToUpdate = metadata->setIsSelected(value.toBool());
                roleToUpdate = IsSelectedRole;
                break;
            default:
                return false;
        }

        if (needToUpdate) {
            emit dataChanged(index, index, QVector<int>() << IsModifiedRole << roleToUpdate);

            if (role == EditArtworkDescriptionRole ||
                role == EditArtworkTitleRole) {
                if (metadata->isInitialized()) {
                    metadata->requestBackup();
                }
            }
        }

        return true;
    }

    int ArtItemsModel::addLocalArtworks(const QList<QUrl> &artworksPaths) {
        LOG_DEBUG << artworksPaths;
        QStringList fileList;
        fileList.reserve(artworksPaths.length());

        foreach(const QUrl &url, artworksPaths) {
            fileList.append(url.toLocalFile());
        }

        int filesAddedCount = addFiles(fileList);
        return filesAddedCount;
    }

    int ArtItemsModel::addLocalDirectories(const QList<QUrl> &directories) {
        LOG_DEBUG << directories;
        QStringList directoriesList;
        directoriesList.reserve(directories.length());

        foreach(const QUrl &url, directories) {
            if (url.isLocalFile()) {
                directoriesList.append(url.toLocalFile());
            } else {
                directoriesList.append(url.path());
            }
        }

        int addedFilesCount = addDirectories(directoriesList);
        return addedFilesCount;
    }

    void ArtItemsModel::spellCheckErrorsChanged() {
        LOG_INTEGRATION_TESTS << "#";
        ArtworkMetadata *item = qobject_cast<ArtworkMetadata *>(sender());

#ifdef QT_DEBUG
        bool found = false;
        for (auto *existing: m_ArtworkList) {
            if (existing == item) {
                found = true;
                break;
            }
        }

        if (!found) {
            for (auto *existing: m_FinalizationList) {
                if (existing == item) {
                    found = true;
                    break;
                }
            }
        }

        Q_ASSERT(found);
#endif

#ifndef QT_DEBUG
        if (item != NULL)
#endif
        {
            m_CommandManager->submitForWarningsCheck(item, Common::WarningsCheckFlags::Spelling);
        }
    }

    void ArtItemsModel::removeItemsAtIndices(const QVector<QPair<int, int> > &ranges) {
        AbstractListModel::removeItemsAtIndices(ranges);
        emit artworksChanged(true);
    }

    void ArtItemsModel::beginAccountingFiles(int filesCount) {
        int rowsCount = rowCount();

        beginInsertRows(QModelIndex(), rowsCount, rowsCount + filesCount - 1);
    }

    void ArtItemsModel::beginAccountingFiles(int start, int end) {
        beginInsertRows(QModelIndex(), start, end);
    }

    void ArtItemsModel::endAccountingFiles() {
        endInsertRows();
    }

    void ArtItemsModel::beginAccountingManyFiles() {
        beginResetModel();
    }

    void ArtItemsModel::endAccountingManyFiles() {
        endResetModel();
    }

    void ArtItemsModel::insertArtwork(int index, ArtworkMetadata *metadata) {
        Q_ASSERT(index >= 0 && index <= getArtworksCount());
        Q_ASSERT(metadata != NULL);
        m_ArtworkList.insert(m_ArtworkList.begin() + index, metadata);
    }

    void ArtItemsModel::appendMetadata(ArtworkMetadata *metadata) {
        Q_ASSERT(metadata != NULL);
        m_ArtworkList.push_back(metadata);
    }

    void ArtItemsModel::removeArtworks(const QVector<QPair<int, int> > &ranges) {
        LOG_DEBUG << ranges.count() << "range(s)";
        doRemoveItemsInRanges(ranges);
    }

    ArtworkMetadata *ArtItemsModel::getArtwork(int index) const {
        ArtworkMetadata *result = NULL;

        if (0 <= index && index < getArtworksCount()) {
            result = m_ArtworkList.at(index);
        }

        return result;
    }

    void ArtItemsModel::raiseArtworksAdded(int imagesCount, int vectorsCount) {
        emit artworksAdded(imagesCount, vectorsCount);
        QCoreApplication::processEvents(QEventLoop::AllEvents);

        LOG_INFO << imagesCount << "images" << vectorsCount << "vectors";
    }

    void ArtItemsModel::updateItemsAtIndices(const QVector<int> &indices) {
        QVector<int> sortedIndices(indices);
        qSort(sortedIndices);
        QVector<QPair<int, int> > ranges;
        Helpers::indicesToRanges(sortedIndices, ranges);
        updateItemsInRanges(ranges);
    }

    void ArtItemsModel::updateItemsInRanges(const QVector<QPair<int, int> > &ranges) {
        QVector<int> roles;
        fillStandardRoles(roles);
        AbstractListModel::updateItemsInRanges(ranges, roles);
    }

    void ArtItemsModel::setAllItemsSelected(bool selected) {
        LOG_DEBUG << selected;
        size_t length = getArtworksCount();

        for (size_t i = 0; i < length; ++i) {
            ArtworkMetadata *metadata = m_ArtworkList.at(i);
            metadata->setIsSelected(selected);
        }

        if (length > 0) {
            QModelIndex startIndex = index(0);
            QModelIndex endIndex = index((int)length - 1);
            emit dataChanged(startIndex, endIndex, QVector<int>() << IsSelectedRole);
        }
    }

    int ArtItemsModel::attachVectors(const QHash<QString, QHash<QString, QString> > &vectorsPaths, QVector<int> &indicesToUpdate) const {
        LOG_DEBUG << "#";
        if (vectorsPaths.isEmpty()) { return 0; }

        int attachedVectors = 0;
        QString defaultPath;
        Models::ArtworksRepository *artworksRepository = m_CommandManager->getArtworksRepository();

        size_t size = getArtworksCount();
        indicesToUpdate.reserve((int)size);

        for (size_t i = 0; i < size; ++i) {
            ArtworkMetadata *metadata = m_ArtworkList.at(i);
            ImageArtwork *image = dynamic_cast<ImageArtwork *>(metadata);
            if (image == NULL) {
                continue;
            }

            const QString &filepath = image->getFilepath();
            QFileInfo fi(filepath);

            const QString &directory = fi.absolutePath();
            if (vectorsPaths.contains(directory)) {
                const QHash<QString, QString> &innerHash = vectorsPaths[directory];

                const QString &filename = fi.baseName().toLower();

                QString vectorsPath = innerHash.value(filename, defaultPath);
                if (!vectorsPath.isEmpty()) {
                    image->attachVector(vectorsPath);
                    artworksRepository->accountVector(vectorsPath);
                    indicesToUpdate.append((int)i);
                    attachedVectors++;
                }
            }
        }

        LOG_INFO << "Found matches to" << attachedVectors << "file(s)";

        return attachedVectors;
    }

    Common::IBasicArtwork *ArtItemsModel::getBasicArtwork(int index) const {
        Common::IBasicArtwork *result = NULL;

        if (0 <= index && index < getArtworksCount()) {
            result = m_ArtworkList.at(index);
        }

        return result;
    }

    void ArtItemsModel::updateItemAtIndex(int metadataIndex) {
        QVector<int> roles;
        fillStandardRoles(roles);
        QModelIndex topLeft = this->index(metadataIndex);
        QModelIndex bottomRight = this->index(metadataIndex);
        emit dataChanged(topLeft, bottomRight, roles);
    }

    int ArtItemsModel::addDirectories(const QStringList &directories) {
        LOG_INFO << directories;
        int filesCount = 0;
        QStringList files;

        foreach(const QString &directory, directories) {
            doAddDirectory(directory, files);
        }

        if (files.count() > 0) {
            filesCount = addFiles(files);
        }

        return filesCount;
    }

    void ArtItemsModel::doAddDirectory(const QString &directory, QStringList &filesList) {
        QDir dir(directory);

        dir.setFilter(QDir::NoDotAndDotDot | QDir::Files);

        QFileInfoList items = dir.entryInfoList();
        int size = items.size();
        filesList.reserve(filesList.size() + size);

        for (int i = 0; i < size; ++i) {
            QString filepath = items.at(i).absoluteFilePath();
            filesList.append(filepath);
        }

        LOG_INFO << filesList.length() << "file(s) found";
    }

    int ArtItemsModel::addFiles(const QStringList &rawFilenames) {
        LOG_INFO << rawFilenames.length() << "file(s)";
        QStringList filenames, vectors;
        filenames.reserve(rawFilenames.length());
        vectors.reserve(rawFilenames.length());

        QSet<QString> knownImageSuffixes;
        knownImageSuffixes << "jpg" << "jpeg" << "tiff" << "tif";

        foreach(const QString &filepath, rawFilenames) {
            QFileInfo fi(filepath);
            QString suffix = fi.suffix().toLower();

            if (knownImageSuffixes.contains(suffix)) {
                filenames.append(filepath);
            } else if (suffix == QLatin1String("png")) {
                LOG_WARNING << "PNG is unsupported file format";
            } else {
                if (suffix == QLatin1String("eps") ||
                    suffix == QLatin1String("ai")) {
                    vectors.append(filepath);
                } else if (suffix != QLatin1String(Constants::METADATA_BACKUP_SUFFIX)) {
                    LOG_WARNING << "Unsupported extension of file" << filepath;
                }
            }
        }

        Models::SettingsModel *settingsModel = m_CommandManager->getSettingsModel();
        bool autoFindVectors = settingsModel->getAutoFindVectors();

        std::shared_ptr<Commands::AddArtworksCommand> addArtworksCommand(new Commands::AddArtworksCommand(filenames, vectors, autoFindVectors));
        std::shared_ptr<Commands::ICommandResult> result = m_CommandManager->processCommand(addArtworksCommand);
        std::shared_ptr<Commands::AddArtworksCommandResult> addArtworksResult = std::dynamic_pointer_cast<Commands::AddArtworksCommandResult>(result);

        int newFilesCount = addArtworksResult->m_NewFilesAdded;
        return newFilesCount;
    }

    void ArtItemsModel::doCombineArtwork(int index) {
        LOG_DEBUG << "index" << index;
        if (0 <= index && index < getArtworksCount()) {
            ArtworkMetadata *metadata = m_ArtworkList.at(index);
            // QModelIndex qmIndex = this->index(index);
            // emit dataChanged(qmIndex, qmIndex, QVector<int>() << IsSelectedRole);

            m_CommandManager->combineArtwork(metadata, index);
        }
    }

    QHash<int, QByteArray> ArtItemsModel::roleNames() const {
        QHash<int, QByteArray> roles;
        roles[ArtworkDescriptionRole] = "description";
        roles[EditArtworkDescriptionRole] = "editdescription";
        roles[ArtworkTitleRole] = "title";
        roles[EditArtworkTitleRole] = "edittitle";
        roles[ArtworkFilenameRole] = "filename";
        roles[KeywordsStringRole] = "keywordsstring";
        roles[IsModifiedRole] = "ismodified";
        roles[IsSelectedRole] = "isselected";
        roles[EditIsSelectedRole] = "editisselected";
        roles[KeywordsCountRole] = "keywordscount";
        roles[HasVectorAttachedRole] = "hasvectorattached";
        roles[BaseFilenameRole] = "basefilename";
        return roles;
    }

    bool ArtItemsModel::shouldRemoveInRanges(int rangesLength) const {
#ifdef QT_DEBUG
        return rangesLength > 10;
#else
        return rangesLength > 50;
#endif
    }

    void ArtItemsModel::removeInnerItem(int row) {
        Q_ASSERT(row >= 0 && row < getArtworksCount());
        ArtworkMetadata *metadata = m_ArtworkList.at(row);
        m_ArtworkList.erase(m_ArtworkList.begin() + row);
        ArtworksRepository *artworksRepository = m_CommandManager->getArtworksRepository();
        artworksRepository->removeFile(metadata->getFilepath(), metadata->getDirectoryID());

        ImageArtwork *image = dynamic_cast<ImageArtwork*>(metadata);
        if ((image != nullptr) && image->hasVectorAttached()) {
            artworksRepository->removeVector(image->getAttachedVectorPath());
        }

        if (metadata->isSelected()) {
            emit selectedArtworksRemoved(1);
        }

        LOG_INFO << "File removed:" << metadata->getFilepath();
        destroyInnerItem(metadata);
    }

    void ArtItemsModel::removeInnerItemRange(int start, int end) {
        Q_ASSERT(start >= 0 && start < getArtworksCount());
        Q_ASSERT(end >= 0 && end < getArtworksCount());
        Q_ASSERT(start <= end);

        ArtworksRepository *artworkRepository = m_CommandManager->getArtworksRepository();

        std::deque<ArtworkMetadata *>::iterator itBegin = m_ArtworkList.begin() + start;
        std::deque<ArtworkMetadata *>::iterator itEnd = m_ArtworkList.begin() + (end + 1);

        std::vector<ArtworkMetadata *> itemsToDelete(itBegin, itEnd);
        m_ArtworkList.erase(itBegin, itEnd);

        int selectedItems = 0;

        std::vector<ArtworkMetadata *>::iterator it = itemsToDelete.begin();
        std::vector<ArtworkMetadata *>::iterator itemsEnd = itemsToDelete.end();
        for (; it < itemsEnd; it++) {
            ArtworkMetadata *metadata = *it;

            artworkRepository->removeFile(metadata->getFilepath(), metadata->getDirectoryID());
            if (metadata->isSelected()) {
                selectedItems++;
            }

            LOG_INFO << "File removed:" << metadata->getFilepath();
            destroyInnerItem(metadata);
        }

        emit selectedArtworksRemoved(selectedItems);
    }

    void ArtItemsModel::destroyInnerItem(ArtworkMetadata *metadata) {
        if (metadata->release()) {
            m_CommandManager->disconnectArtworkSignals(metadata);
            metadata->deepDisconnect();
#ifdef QT_DEBUG
            m_DestroyedList.push_back(metadata);
#endif
            metadata->deleteLater();
        } else {
            LOG_DEBUG << "Metadata is locked. Postponing destruction...";

            metadata->disconnect();
            auto *metadataModel = metadata->getBasicModel();
            metadataModel->disconnect();
            metadataModel->clearModel();

            m_FinalizationList.push_back(metadata);
        }
    }

    void ArtItemsModel::doRemoveItemsAtIndices(QVector<int> &indicesToRemove) {
        qSort(indicesToRemove);
        QVector<QPair<int, int> > rangesToRemove;
        Helpers::indicesToRanges(indicesToRemove, rangesToRemove);
        doRemoveItemsInRanges(rangesToRemove);
    }

    void ArtItemsModel::doRemoveItemsInRanges(const QVector<QPair<int, int> > &rangesToRemove) {
        std::shared_ptr<Commands::RemoveArtworksCommand> removeArtworksCommand(new Commands::RemoveArtworksCommand(rangesToRemove));

        m_CommandManager->processCommand(removeArtworksCommand);
    }

    void ArtItemsModel::getSelectedItemsIndices(QVector<int> &indices) {
        size_t size = m_ArtworkList.size();

        indices.reserve((int)size / 3);
        for (size_t i = 0; i < size; ++i) {
            if (m_ArtworkList.at(i)->isSelected()) {
                indices.append((int)i);
            }
        }
    }

    void ArtItemsModel::fillStandardRoles(QVector<int> &roles) const {
        roles << ArtworkDescriptionRole << IsModifiedRole <<
            ArtworkTitleRole << KeywordsCountRole << HasVectorAttachedRole;
    }

    void ArtItemsModel::onFilesUnavailableHandler() {
        LOG_DEBUG << "#";
        Models::ArtworksRepository *artworksRepository = m_CommandManager->getArtworksRepository();
        size_t count = getArtworksCount();

        bool anyArtworkUnavailable = false;
        bool anyVectorUnavailable = false;

        for (size_t i = 0; i < count; ++i) {
            ArtworkMetadata *artwork = m_ArtworkList.at(i);
            ImageArtwork *image = dynamic_cast<ImageArtwork *>(artwork);
            const QString &path = artwork->getFilepath();

            if (artworksRepository->isFileUnavailable(path)) {
                artwork->setUnavailable();
                anyArtworkUnavailable = true;
            } else if (image != NULL && image->hasVectorAttached()) {
                const QString &vectorPath = image->getAttachedVectorPath();
                if (artworksRepository->isFileUnavailable(vectorPath)) {
                    image->detachVector();
                    anyVectorUnavailable = true;
                }
            }
        }

        if (anyArtworkUnavailable) {
            emit unavailableArtworksFound();
        } else if (anyVectorUnavailable) {
            emit unavailableVectorsFound();
        }
    }

    void ArtItemsModel::artworkBackupRequested() {
        LOG_DEBUG << "#";
        ArtworkMetadata *metadata = qobject_cast<ArtworkMetadata *>(sender());
        if (metadata != NULL) {
            m_CommandManager->saveArtworkBackup(metadata);
        }
    }

    void ArtItemsModel::onUndoStackEmpty() {
        LOG_DEBUG << "#";
        if (m_ArtworkList.empty()) {
            if (!m_FinalizationList.empty()) {
                LOG_DEBUG << "Clearing the finalization list";
                for (auto *item: m_FinalizationList) {
                    item->deepDisconnect();
                    item->deleteLater();
                }
                m_FinalizationList.clear();
            }
        }
    }

    void ArtItemsModel::userDictUpdateHandler(const QStringList &keywords, bool overwritten) {
        LOG_DEBUG << "#";
        LOG_INTEGRATION_TESTS << keywords;
        size_t size = m_ArtworkList.size();

        Q_ASSERT(!keywords.isEmpty());

        QVector<Common::BasicKeywordsModel *> itemsToCheck;
        itemsToCheck.reserve((int)size);

        for (size_t i = 0; i < size; i++) {
            ArtworkMetadata *metadata = m_ArtworkList.at(i);
            auto *metadataModel = metadata->getBasicModel();
            SpellCheck::SpellCheckItemInfo *info = metadataModel->getSpellCheckInfo();
            if(!overwritten) {
                info->removeWordsFromErrors(keywords);
            } else {
                info->clear();
            }

            itemsToCheck.append(metadataModel);
        }

        if(!overwritten) {
            m_CommandManager->submitForSpellCheck(itemsToCheck, keywords);
        } 
		else {
            m_CommandManager->submitForSpellCheck(itemsToCheck);
        }
    }

    void ArtItemsModel::userDictClearedHandler() {
        size_t size = m_ArtworkList.size();
        QVector<Common::BasicKeywordsModel *> itemsToCheck;
        itemsToCheck.reserve((int)size);

        for (size_t i = 0; i < size; i++) {
            ArtworkMetadata *metadata = m_ArtworkList.at(i);
            auto *keywordsModel = metadata->getBasicModel();
            itemsToCheck.append(keywordsModel);
        }

        m_CommandManager->submitForSpellCheck(itemsToCheck);
    }
}
