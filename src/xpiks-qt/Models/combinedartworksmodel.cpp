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
#include "artiteminfo.h"
#include "../SpellCheck/spellcheckiteminfo.h"
#include "../Common/defines.h"

namespace Models {
    CombinedArtworksModel::CombinedArtworksModel(QObject *parent) :
        AbstractListModel(parent),
        Common::BaseEntity(),
        m_CommonKeywordsModel(this),
        m_EditFlags(0),
        m_AreKeywordsModified(false),
        m_IsDescriptionModified(false),
        m_IsTitleModified(false)
    {
        QObject::connect(&m_CommonKeywordsModel, SIGNAL(spellCheckErrorsChanged()),
                         this, SLOT(spellCheckErrorsChangedHandler()));
    }

    CombinedArtworksModel::~CombinedArtworksModel() { qDeleteAll(m_ArtworksList); }

    void CombinedArtworksModel::initArtworks(const QVector<ArtItemInfo *> &artworks) {
        int innerLength = m_ArtworksList.length();
        int start = innerLength == 0 ? 0 : innerLength - 1;
        int paramLength = artworks.length();
        if (paramLength > 0) {
            beginInsertRows(QModelIndex(), start, start + paramLength - 1);
            m_ArtworksList << artworks;
            endInsertRows();
        }

        if (paramLength == 1) {
            enableAllFields();
        }
    }

    void CombinedArtworksModel::recombineArtworks() {
        LOG_DEBUG << m_ArtworksList.length() << "artwork(s)";
        if (m_ArtworksList.length() == 1) {
            assignFromOneArtwork();
            ArtworkMetadata *metadata = m_ArtworksList.first()->getOrigin();
            m_CommonKeywordsModel.setSpellCheckInfo(metadata->getSpellCheckInfo());
            m_CommonKeywordsModel.setSpellStatuses(metadata->getSpellStatuses());
        } else {
            assignFromManyArtworks();
            m_CommonKeywordsModel.setSpellCheckInfo(&m_SpellCheckInfo);
        }

        m_CommandManager->submitItemForSpellCheck(&m_CommonKeywordsModel);
    }

    void CombinedArtworksModel::acceptSuggestedKeywords(const QStringList &keywords)  {
        foreach (const QString &keyword, keywords) {
            this->appendKeyword(keyword);
        }
    }

    void CombinedArtworksModel::setChangeDescription(bool value)  {
        if (Common::HasFlag(m_EditFlags, Common::EditDesctiption) != value) {
            Common::ApplyFlag(m_EditFlags, value, Common::EditDesctiption);
            emit changeDescriptionChanged();
        }
    }

    void CombinedArtworksModel::setChangeTitle(bool value) {
        if (Common::HasFlag(m_EditFlags, Common::EditTitle) != value) {
            Common::ApplyFlag(m_EditFlags, value, Common::EditTitle);
            emit changeTitleChanged();
        }
    }

    void CombinedArtworksModel::setChangeKeywords(bool value){
        if (Common::HasFlag(m_EditFlags, Common::EditKeywords) != value) {
            Common::ApplyFlag(m_EditFlags, value, Common::EditKeywords);
            emit changeKeywordsChanged();
        }
    }

    void CombinedArtworksModel::setAppendKeywords(bool value) {
        if (Common::HasFlag(m_EditFlags, Common::AppendKeywords) != value) {
            Common::ApplyFlag(m_EditFlags, value, Common::AppendKeywords);
            emit appendKeywordsChanged();
        }
    }

    int CombinedArtworksModel::getSelectedArtworksCount() const {
        int selectedCount = 0;
        int count = m_ArtworksList.length();
        for (int i = 0; i < count; ++i) {
            if (m_ArtworksList.at(i)->isSelected()) {
                selectedCount++;
            }
        }

        return selectedCount;
    }

#if defined(TESTS)
        const QStringList &CombinedArtworksModel::getKeywords() const {
            return m_CommonKeywordsModel.getKeywords();
        }
#endif

    void CombinedArtworksModel::editKeyword(int index, const QString &replacement) {
        if (m_CommonKeywordsModel.editKeyword(index, replacement)) {
            m_AreKeywordsModified = true;
            m_CommandManager->submitKeywordForSpellCheck(&m_CommonKeywordsModel, index);
        }
    }

    QString CombinedArtworksModel::removeKeywordAt(int keywordIndex) {
        QString keyword;
        if (m_CommonKeywordsModel.takeKeywordAt(keywordIndex, keyword)) {
            emit keywordsCountChanged();
            m_AreKeywordsModified = true;
        }

        return keyword;
    }

    void CombinedArtworksModel::removeLastKeyword() {
        QString keyword;
        if (m_CommonKeywordsModel.takeLastKeyword(keyword)) {
            emit keywordsCountChanged();
            m_AreKeywordsModified = true;
        }
    }

    void CombinedArtworksModel::appendKeyword(const QString &keyword) {
        if (m_CommonKeywordsModel.appendKeyword(keyword)) {
            emit keywordsCountChanged();
            m_AreKeywordsModified = true;

            m_CommandManager->submitKeywordForSpellCheck(&m_CommonKeywordsModel, m_CommonKeywordsModel.rowCount() - 1);
        }
    }

    void CombinedArtworksModel::pasteKeywords(const QStringList &keywords) {
        LOG_DEBUG << keywords.length() << "keyword(s)";
        if (m_CommonKeywordsModel.appendKeywords(keywords) > 0) {
            emit keywordsCountChanged();
            m_AreKeywordsModified = true;

            m_CommandManager->submitItemForSpellCheck(&m_CommonKeywordsModel);
        }
    }

    void CombinedArtworksModel::setArtworksSelected(int index, bool newState) {
        if (index < 0 || index >= m_ArtworksList.length()) {
            return;
        }

        m_ArtworksList.at(index)->setSelected(newState);
        QModelIndex qIndex = this->index(index);
        emit dataChanged(qIndex, qIndex, QVector<int>() << IsSelectedRole);
        emit selectedArtworksCountChanged();
    }

    void CombinedArtworksModel::removeSelectedArtworks() {
        int count = m_ArtworksList.length();
        QVector<int> indicesToRemove;
        indicesToRemove.reserve(count);

        for (int i = 0; i < count; ++i) {
            ArtItemInfo *item = m_ArtworksList[i];
            if (item->isSelected()) {
                indicesToRemove.append(i);
            }
        }

        QVector<QPair<int, int> > rangesToRemove;
        Helpers::indicesToRanges(indicesToRemove, rangesToRemove);
        removeItemsAtIndices(rangesToRemove);

        recombineArtworks();
        emit artworksCountChanged();
    }

    void CombinedArtworksModel::saveEdits() const {
        bool needToSave = false;

        needToSave = m_ArtworksList.length() > 1;
        needToSave = needToSave || (getChangeKeywords() && m_AreKeywordsModified);
        needToSave = needToSave || (getChangeTitle() && m_IsTitleModified);
        needToSave = needToSave || (getChangeDescription() && m_IsDescriptionModified);

        if (needToSave) {
            processCombinedEditCommand();
        }
    }

    void CombinedArtworksModel::resetModelData() {
        beginResetModel();
        qDeleteAll(m_ArtworksList);
        m_ArtworksList.clear();
        endResetModel();

        m_AreKeywordsModified = false;
        m_IsDescriptionModified = false;
        m_IsTitleModified = false;

        // TEMPORARY (enable everything on initial launch) --
        m_EditFlags = 0;
        enableAllFields();
        // TEMPORARY (enable everything on initial launch) --

        m_CommonKeywordsModel.setSpellCheckInfo(NULL);
        initDescription("");
        initTitle("");
        initKeywords(QStringList());
    }

    void CombinedArtworksModel::clearKeywords() {
        m_CommonKeywordsModel.clearKeywords();
    }

    void CombinedArtworksModel::suggestCorrections() {
        m_CommandManager->setupSpellCheckSuggestions(&m_CommonKeywordsModel, -1, Common::CorrectAll);
    }

    void CombinedArtworksModel::initDescriptionHighlighting(QQuickTextDocument *document) {
        SpellCheck::SpellCheckItemInfo *info = m_CommonKeywordsModel.getSpellCheckInfo();
        if (info == NULL) {
            Q_ASSERT(false);
            // OneItem edits will use artwork's spellcheckinfo
            // combined edit will use this one
            info = &m_SpellCheckInfo;
        }

        info->createHighlighterForDescription(document->textDocument(), &m_CommonKeywordsModel);
        m_CommonKeywordsModel.notifySpellCheckResults(Common::SpellCheckDescription);
    }

    void CombinedArtworksModel::initTitleHighlighting(QQuickTextDocument *document) {
        SpellCheck::SpellCheckItemInfo *info = m_CommonKeywordsModel.getSpellCheckInfo();
        if (info == NULL) {
            Q_ASSERT(false);
            // OneItem edits will use artwork's spellcheckinfo
            // combined edit will use this one
            info = &m_SpellCheckInfo;
        }

        info->createHighlighterForTitle(document->textDocument(), &m_CommonKeywordsModel);
        m_CommonKeywordsModel.notifySpellCheckResults(Common::SpellCheckTitle);
    }

    void CombinedArtworksModel::spellCheckDescription() {
        if (!m_CommonKeywordsModel.getDescription().trimmed().isEmpty()) {
            m_CommandManager->submitItemForSpellCheck(&m_CommonKeywordsModel, Common::SpellCheckDescription);
        }
    }

    void CombinedArtworksModel::spellCheckTitle() {
        if (!m_CommonKeywordsModel.getTitle().trimmed().isEmpty()) {
            m_CommandManager->submitItemForSpellCheck(&m_CommonKeywordsModel, Common::SpellCheckTitle);
        }
    }

    void CombinedArtworksModel::assignFromSelected() {
        int selectedCount = 0;
        int count = m_ArtworksList.length();
        ArtItemInfo *firstSelected = NULL;
        for (int i = 0; i < count; ++i) {
            if (m_ArtworksList.at(i)->isSelected()) {
                selectedCount++;

                if (firstSelected == NULL) {
                    firstSelected = m_ArtworksList.at(i);
                }
            }
        }

        if (selectedCount == 1) {
            Q_ASSERT(firstSelected != NULL);
            LOG_DEBUG << "Assigning fields";
            ArtworkMetadata *metadata = firstSelected->getOrigin();
            setTitle(metadata->getTitle());
            setDescription(metadata->getDescription());
            setKeywords(metadata->getKeywords());
        } else {
            LOG_WARNING << "Method called with" << count << "items selected";
        }
    }

    void CombinedArtworksModel::processCombinedEditCommand() const {
        Commands::CombinedEditCommand *combinedEditCommand = new Commands::CombinedEditCommand(
                    m_EditFlags,
                    m_ArtworksList,
                    m_CommonKeywordsModel.getDescription(),
                    m_CommonKeywordsModel.getTitle(),
                    m_CommonKeywordsModel.getKeywords());

        Commands::ICommandResult *result = m_CommandManager->processCommand(combinedEditCommand);
        Commands::CombinedEditCommandResult *combinedResult = static_cast<Commands::CombinedEditCommandResult*>(result);
        m_CommandManager->updateArtworks(combinedResult->m_IndicesToUpdate);

        delete combinedResult;
    }

    void CombinedArtworksModel::enableAllFields(){
        setChangeDescription(true);
        setChangeTitle(true);
        setChangeKeywords(true);
    }

    void CombinedArtworksModel::assignFromOneArtwork() {
        Q_ASSERT(m_ArtworksList.length() == 1);
        ArtItemInfo *info = m_ArtworksList[0];
        ArtworkMetadata *metadata = info->getOrigin();

        initDescription(metadata->getDescription());
        initTitle(metadata->getTitle());

        if (!m_AreKeywordsModified) {
            initKeywords(metadata->getKeywords());
        }
    }

    void CombinedArtworksModel::assignFromManyArtworks() {
        bool anyItemsProcessed = false;
        bool descriptionsDiffer = false;
        bool titleDiffer = false;
        QString description;
        QString title;
        QSet<QString> commonKeywords;

        int artworksCount = m_ArtworksList.length();
        for (int i = 0; i < artworksCount; ++i) {
            ArtItemInfo *info = m_ArtworksList[i];
            ArtworkMetadata *metadata = info->getOrigin();

            if (!anyItemsProcessed) {
                description = metadata->getDescription();
                title = metadata->getTitle();
                commonKeywords.unite(metadata->getKeywordsSet());
                anyItemsProcessed = true;
                continue;
            }

            const QString &currDescription = metadata->getDescription();
            const QString &currTitle = metadata->getTitle();
            descriptionsDiffer = descriptionsDiffer || description != currDescription;
            titleDiffer = titleDiffer || title != currTitle;
            commonKeywords.intersect(metadata->getKeywordsSet());
        }

        if (artworksCount > 0) {
            if (descriptionsDiffer) {
                description = "";
            }

            if (titleDiffer) {
                title = "";
            }

            initDescription(description);
            initTitle(title);

            if (!m_AreKeywordsModified) {
                initKeywords(commonKeywords.toList());
            }
        }
    }

    void CombinedArtworksModel::spellCheckErrorsChangedHandler() {
        emit descriptionChanged();
        emit titleChanged();
    }

    int CombinedArtworksModel::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return m_ArtworksList.count();
    }

    QVariant CombinedArtworksModel::data(const QModelIndex &index, int role) const {
        if (index.row() < 0 || index.row() >= m_ArtworksList.count())
            return QVariant();

        ArtItemInfo *artItemInfo = m_ArtworksList.at(index.row());

        switch (role) {
        case PathRole:
            return artItemInfo->getOrigin()->getFilepath();
        case IsSelectedRole:
            return artItemInfo->isSelected();
        default:
            return QVariant();
        }
    }

    QHash<int, QByteArray> CombinedArtworksModel::roleNames() const {
        QHash<int, QByteArray> roles;
        roles[PathRole] = "path";
        roles[IsSelectedRole] = "isselected";
        return roles;
    }

    void CombinedArtworksModel::removeInnerItem(int row) {
        ArtItemInfo *info = m_ArtworksList[row];
        delete info;
        m_ArtworksList.removeAt(row);
    }


    void CombinedArtworksModel::removeUnavailableItems(){
        QVector<int> indicesToRemove;
        for (int i= 0; i<m_ArtworksList.size(); i++){
            ArtItemInfo* ArtItemInfoElement=m_ArtworksList[i];
            if (ArtItemInfoElement->getOrigin()->getIsRemoved()){
                emit artworkDeleted(i); // for single ArtItem Dialog
                indicesToRemove.append(i);
            }
         }
        QVector<QPair<int, int> > rangesToRemove;
        Helpers::indicesToRanges(indicesToRemove, rangesToRemove);
        removeItemsAtIndices(rangesToRemove);
       // recombineArtworks(); the application crashes when it is called
        emit artworksCountChanged();
        if (!m_ArtworksList.size())
            emit closeWindow();
    }




}
