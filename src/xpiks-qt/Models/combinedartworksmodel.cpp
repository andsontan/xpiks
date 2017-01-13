/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2016 Taras Kushnir <kushnirTV@gmail.com>
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

#include "combinedartworksmodel.h"
#include "../Helpers/indiceshelper.h"
#include "../Commands/combinededitcommand.h"
#include "../Commands/commandmanager.h"
#include "../Commands/commandbase.h"
#include "../Suggestion/keywordssuggestor.h"
#include "artworkmetadata.h"
#include "metadataelement.h"
#include "../SpellCheck/spellcheckiteminfo.h"
#include "../Common/defines.h"
#include "../QMLExtensions/colorsmodel.h"

namespace Models {
    CombinedArtworksModel::CombinedArtworksModel(QObject *parent):
        ArtworksViewModel(parent),
        ArtworkProxyBase(),
        m_CommonKeywordsModel(m_HoldPlaceholder, this),
        m_EditFlags(Common::CombinedEditFlags::None),
        m_ModifiedFlags(0) {
        m_CommonKeywordsModel.setSpellCheckInfo(&m_SpellCheckInfo);

        QObject::connect(&m_CommonKeywordsModel, SIGNAL(spellCheckErrorsChanged()),
                         this, SLOT(spellCheckErrorsChangedHandler()));

        QObject::connect(&m_CommonKeywordsModel, SIGNAL(completionsAvailable()),
                         this, SIGNAL(completionsAvailable()));

        QObject::connect(&m_CommonKeywordsModel, SIGNAL(afterSpellingErrorsFixed()),
                         this, SLOT(spellCheckErrorsFixedHandler()));
    }

    void CombinedArtworksModel::setArtworks(std::vector<MetadataElement> &artworks) {
        ArtworksViewModel::setArtworks(artworks);

        recombineArtworks();

        if (getArtworksCount() == 1) {
            enableAllFields();
        }
    }

    void CombinedArtworksModel::recombineArtworks() {
        LOG_INFO << getArtworksCount() << "artwork(s)";

        LOG_DEBUG << "Before recombine description:" << getDescription();
        LOG_DEBUG << "Before recombine title:" << getTitle();
        LOG_DEBUG << "Before recombine keywords:" << getKeywordsString();

        if (isEmpty()) {
            return;
        }

        if (getArtworksCount() == 1) {
            assignFromOneArtwork();
        } else {
            assignFromManyArtworks();
        }

        LOG_DEBUG << "After recombine description:" << getDescription();
        LOG_DEBUG << "After recombine title:" << getTitle();
        LOG_DEBUG << "After recombine keywords:" << getKeywordsString();

        m_CommandManager->submitItemForSpellCheck(&m_CommonKeywordsModel);
    }

    void CombinedArtworksModel::acceptSuggestedKeywords(const QStringList &keywords) {
        LOG_INFO << keywords.size() << "keyword(s)";
        foreach(const QString &keyword, keywords) {
            this->appendKeyword(keyword);
        }
    }

    void CombinedArtworksModel::setChangeDescription(bool value) {
        LOG_INFO << value;
        auto flag = Common::CombinedEditFlags::EditDesctiption;
        if (Common::HasFlag(m_EditFlags, flag) != value) {
            Common::ApplyFlag(m_EditFlags, value, flag);
            emit changeDescriptionChanged();
        }
    }

    void CombinedArtworksModel::setChangeTitle(bool value) {
        LOG_INFO << value;
        auto flag = Common::CombinedEditFlags::EditTitle;
        if (Common::HasFlag(m_EditFlags, flag) != value) {
            Common::ApplyFlag(m_EditFlags, value, flag);
            emit changeTitleChanged();
        }
    }

    void CombinedArtworksModel::setChangeKeywords(bool value) {
        LOG_INFO << value;
        auto flag = Common::CombinedEditFlags::EditKeywords;
        if (Common::HasFlag(m_EditFlags, flag) != value) {
            Common::ApplyFlag(m_EditFlags, value, flag);
            emit changeKeywordsChanged();
        }
    }

    void CombinedArtworksModel::setAppendKeywords(bool value) {
        LOG_INFO << value;
        auto flag = Common::CombinedEditFlags::AppendKeywords;
        if (Common::HasFlag(m_EditFlags, flag) != value) {
            Common::ApplyFlag(m_EditFlags, value, flag);
            emit appendKeywordsChanged();
        }
    }

#ifdef CORE_TESTS
    QStringList CombinedArtworksModel::getKeywords() {
        return m_CommonKeywordsModel.getKeywords();
    }

#endif

    void CombinedArtworksModel::editKeyword(int index, const QString &replacement) {
        if (doEditKeyword(index, replacement)) {
            setKeywordsModified(true);
        }
    }

    QString CombinedArtworksModel::removeKeywordAt(int keywordIndex) {
        QString keyword;
        if (doRemoveKeywordAt(keywordIndex, keyword)) {
            setKeywordsModified(true);
        }
        return keyword;
    }

    void CombinedArtworksModel::removeLastKeyword() {
        QString keyword;
        if (doRemoveLastKeyword(keyword)) {
            setKeywordsModified(true);
        }
    }

    void CombinedArtworksModel::appendKeyword(const QString &keyword) {
        if (doAppendKeyword(keyword)) {
            setKeywordsModified(true);
        }
    }

    void CombinedArtworksModel::pasteKeywords(const QStringList &keywords) {
        if (doAppendKeywords(keywords) > 0) {
            setKeywordsModified(true);
        }
    }

    void CombinedArtworksModel::saveEdits() {
        LOG_INFO << "edit flags:" << (int)m_EditFlags << "modified flags:" << m_ModifiedFlags;
        bool needToSave = false;

        if (getChangeTitle() ||
            getChangeDescription() ||
            getChangeKeywords()) {
            needToSave = getArtworksCount() > 1;
            needToSave = needToSave || (getChangeKeywords() && areKeywordsModified());
            needToSave = needToSave || isSpellingFixed();
            needToSave = needToSave || (getChangeTitle() && isTitleModified());
            needToSave = needToSave || (getChangeDescription() && isDescriptionModified());
        }

        if (needToSave) {
            processCombinedEditCommand();
        } else {
            LOG_DEBUG << "nothing to save";
        }
    }

    void CombinedArtworksModel::clearKeywords() {
        if (doClearKeywords()) {
            setKeywordsModified(true);
        }
    }

    void CombinedArtworksModel::suggestCorrections() {
        doSuggestCorrections();
    }

    void CombinedArtworksModel::initDescriptionHighlighting(QQuickTextDocument *document) {
        SpellCheck::SpellCheckItemInfo *info = m_CommonKeywordsModel.getSpellCheckInfo();

        if (info == NULL) {
            Q_ASSERT(false);
            // OneItem edits will use artwork's spellcheckinfo
            // combined edit will use this one
            info = &m_SpellCheckInfo;
        }

        QMLExtensions::ColorsModel *colorsModel = m_CommandManager->getColorsModel();
        info->createHighlighterForDescription(document->textDocument(), colorsModel, &m_CommonKeywordsModel);
        m_CommonKeywordsModel.notifyDescriptionSpellCheck();
    }

    void CombinedArtworksModel::initTitleHighlighting(QQuickTextDocument *document) {
        SpellCheck::SpellCheckItemInfo *info = m_CommonKeywordsModel.getSpellCheckInfo();

        if (info == NULL) {
            Q_ASSERT(false);
            // OneItem edits will use artwork's spellcheckinfo
            // combined edit will use this one
            info = &m_SpellCheckInfo;
        }

        QMLExtensions::ColorsModel *colorsModel = m_CommandManager->getColorsModel();
        info->createHighlighterForTitle(document->textDocument(), colorsModel, &m_CommonKeywordsModel);
        m_CommonKeywordsModel.notifyTitleSpellCheck();
    }

    void CombinedArtworksModel::spellCheckDescription() {
        doSpellCheckDescription();
    }

    void CombinedArtworksModel::spellCheckTitle() {
        doSpellCheckTitle();
    }

    void CombinedArtworksModel::assignFromSelected() {
        int selectedCount = 0;
        int firstSelectedIndex = -1;

        processArtworks([](const MetadataElement &item) { return item.isSelected(); },
                        [&selectedCount, &firstSelectedIndex](int index, ArtworkMetadata *) {
            selectedCount++;

            if (firstSelectedIndex == -1) {
                firstSelectedIndex = index;
            }
        });

        if (selectedCount == 1) {
            Q_ASSERT(firstSelectedIndex != -1);
            LOG_DEBUG << "Assigning fields";
            ArtworkMetadata *metadata = getArtworkMetadata(firstSelectedIndex);
            setTitle(metadata->getTitle());
            setDescription(metadata->getDescription());
            setKeywords(metadata->getKeywords());
        } else {
            LOG_WARNING << "Method called with" << getArtworksCount() << "items selected";
        }
    }

    void CombinedArtworksModel::plainTextEdit(const QString &rawKeywords) {
        doPlainTextEdit(rawKeywords);
        setKeywordsModified(true);
    }

    bool CombinedArtworksModel::hasTitleWordSpellError(const QString &word) {
        return getHasTitleWordSpellError(word);
    }

    bool CombinedArtworksModel::hasDescriptionWordSpellError(const QString &word) {
        return getHasDescriptionWordSpellError(word);
    }

    void CombinedArtworksModel::replaceFromPreset(int keywordsIndex, int presetIndex) {
        auto *presetsModel = m_CommandManager->getPresetsModel();
        QStringList keywords;

        if (!presetsModel->tryGetPreset(presetIndex, keywords)) {
            return;
        }

        m_CommonKeywordsModel.replaceFromPreset(keywordsIndex, keywords);
        emit keywordsCountChanged();
        setKeywordsModified(true);
        m_CommandManager->submitItemForSpellCheck(&m_CommonKeywordsModel, Common::SpellCheckFlags::Keywords);
    }

    void CombinedArtworksModel::appendFromPreset(int presetIndex) {
        auto *presetsModel = m_CommandManager->getPresetsModel();
        QStringList keywords;

        if (presetsModel->tryGetPreset(presetIndex, keywords)) {
            m_CommonKeywordsModel.addFromPreset(keywords);
            emit keywordsCountChanged();
            setKeywordsModified(true);
            m_CommandManager->submitItemForSpellCheck(&m_CommonKeywordsModel, Common::SpellCheckFlags::Keywords);
        }
    }

    void CombinedArtworksModel::processCombinedEditCommand() {
        auto &artworksList = getArtworksList();

        std::shared_ptr<Commands::CombinedEditCommand> combinedEditCommand(new Commands::CombinedEditCommand(
                m_EditFlags,
                artworksList,
                m_CommonKeywordsModel.getDescription(),
                m_CommonKeywordsModel.getTitle(),
                m_CommonKeywordsModel.getKeywords()));

        m_CommandManager->processCommand(combinedEditCommand);
    }

    void CombinedArtworksModel::enableAllFields() {
        setChangeDescription(true);
        setChangeTitle(true);
        setChangeKeywords(true);
    }

    void CombinedArtworksModel::assignFromOneArtwork() {
        LOG_DEBUG << "#";
        Q_ASSERT(getArtworksCount() == 1);
        ArtworkMetadata *metadata = getArtworkMetadata(0);

        if (!isDescriptionModified()) {
            initDescription(metadata->getDescription());
        }

        if (!isTitleModified()) {
            initTitle(metadata->getTitle());
        }

        if (!areKeywordsModified()) {
            initKeywords(metadata->getKeywords());
        }
    }

    void CombinedArtworksModel::assignFromManyArtworks() {
        LOG_DEBUG << "#";

        bool anyItemsProcessed = false;
        bool descriptionsDiffer = false;
        bool titleDiffer = false;
        QString description, title;
        QSet<QString> commonKeywords, unitedKeywords;
        QStringList firstItemKeywords;
        int firstItemKeywordsCount = 0;

        processArtworks([](const MetadataElement &) { return true; },
                        [&](int, ArtworkMetadata *metadata) {
            if (!anyItemsProcessed) {
                description = metadata->getDescription();
                title = metadata->getTitle();
                firstItemKeywords = metadata->getKeywords();
                // preserve case with List to Set convertion
                auto firstSet = firstItemKeywords.toSet();
                commonKeywords.unite(firstSet);
                firstItemKeywordsCount = firstSet.count();
                anyItemsProcessed = true;
                return;
            }

            QString currDescription = metadata->getDescription();
            QString currTitle = metadata->getTitle();
            descriptionsDiffer = descriptionsDiffer || description != currDescription;
            titleDiffer = titleDiffer || title != currTitle;
            // preserve case with List to Set convertion
            auto currentSet = metadata->getKeywords().toSet();
            commonKeywords.intersect(currentSet);

            // used to detect if all items have same keywords
            if ((currentSet.count() == firstItemKeywordsCount) &&
                (unitedKeywords.count() <= firstItemKeywordsCount)) {
                unitedKeywords.unite(currentSet);
            }
        });

        if (!isEmpty()) {
            if (descriptionsDiffer) {
                description = "";
            }

            if (titleDiffer) {
                title = "";
            }

            initDescription(description);
            initTitle(title);

            if (!areKeywordsModified()) {
                if (unitedKeywords.subtract(commonKeywords).isEmpty()) {
                    // all keywords are the same
                    initKeywords(firstItemKeywords);
                } else {
                    initKeywords(commonKeywords.toList());
                }
            }
        }
    }

    void CombinedArtworksModel::spellCheckErrorsChangedHandler() {
        emit descriptionChanged();
        emit titleChanged();
    }

    void CombinedArtworksModel::spellCheckErrorsFixedHandler() {
        setSpellingFixed(true);
        // for possible squeezing after replace
        emit keywordsCountChanged();
    }

    bool CombinedArtworksModel::doRemoveSelectedArtworks() {
        LOG_DEBUG << "#";
        bool anyRemoved = ArtworksViewModel::doRemoveSelectedArtworks();
        if (anyRemoved) {
            if (!isEmpty()) {
                recombineArtworks();
            }
        }

        return anyRemoved;
    }

    void CombinedArtworksModel::doResetModel() {
        LOG_DEBUG << "#";
        ArtworksViewModel::doResetModel();

        m_SpellCheckInfo.clear();

        // TEMPORARY (enable everything on initial launch) --
        m_ModifiedFlags = 0;
        m_EditFlags = Common::CombinedEditFlags::None;
        enableAllFields();
        // TEMPORARY (enable everything on initial launch) --

        initDescription("");
        initTitle("");
        initKeywords(QStringList());
    }

    bool CombinedArtworksModel::removeUnavailableItems() {
        LOG_DEBUG << "#";
        bool anyRemoved = ArtworksViewModel::removeUnavailableItems();
        if (anyRemoved) {
            if (!isEmpty()) {
                recombineArtworks();
            }
        }

        return anyRemoved;
    }

    void CombinedArtworksModel::generateAboutToBeRemoved() {
        LOG_DEBUG << "#";
        m_CommonKeywordsModel.notifyAboutToBeRemoved();
    }

    void CombinedArtworksModel::userDictUpdateHandler(const QStringList &keywords) {
        LOG_DEBUG << "#";
        Q_ASSERT(!keywords.isEmpty());

        SpellCheck::SpellCheckItemInfo *info = m_CommonKeywordsModel.getSpellCheckInfo();
        info->removeWordsFromErrors(keywords);

        m_CommandManager->submitForSpellCheck(QVector<Common::BasicKeywordsModel *>() << (&m_CommonKeywordsModel), keywords);
    }

    void CombinedArtworksModel::userDictClearedHandler() {
        LOG_DEBUG << "#";
        m_CommandManager->submitItemForSpellCheck(&m_CommonKeywordsModel);
    }
}
