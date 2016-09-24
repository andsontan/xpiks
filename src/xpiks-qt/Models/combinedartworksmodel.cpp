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
        LOG_INFO << "index:" << index << "replacement:" << replacement;
        if (m_CommonKeywordsModel.editKeyword(index, replacement)) {
            setKeywordsModified(true);
            m_CommandManager->submitKeywordForSpellCheck(&m_CommonKeywordsModel, index);
        } else {
            LOG_INFO << "Failed to edit to" << replacement;
        }
    }

    QString CombinedArtworksModel::removeKeywordAt(int keywordIndex) {
        LOG_INFO << "keyword index:" << keywordIndex;
        QString keyword;
        if (m_CommonKeywordsModel.takeKeywordAt(keywordIndex, keyword)) {
            emit keywordsCountChanged();
            setKeywordsModified(true);
            LOG_INFO << "Removed keyword:" << keyword << "keywords count:" << getKeywordsCount();
        }

        return keyword;
    }

    void CombinedArtworksModel::removeLastKeyword() {
        LOG_DEBUG << "#";
        QString keyword;
        if (m_CommonKeywordsModel.takeLastKeyword(keyword)) {
            emit keywordsCountChanged();
            setKeywordsModified(true);
            LOG_INFO << "Removed keyword:" << keyword << "keywords count:" << getKeywordsCount();
        }
    }

    void CombinedArtworksModel::appendKeyword(const QString &keyword) {
        LOG_INFO << keyword;
        if (m_CommonKeywordsModel.appendKeyword(keyword)) {
            emit keywordsCountChanged();
            setKeywordsModified(true);

            m_CommandManager->submitKeywordForSpellCheck(&m_CommonKeywordsModel, m_CommonKeywordsModel.rowCount() - 1);
        } else {
            LOG_INFO << "Failed to append:" << keyword;
        }
    }

    void CombinedArtworksModel::pasteKeywords(const QStringList &keywords) {
        LOG_INFO << keywords.length() << "keyword(s)" << "|" << keywords;
        int appendedCount = m_CommonKeywordsModel.appendKeywords(keywords);
        LOG_INFO << "Appended" << appendedCount << "keywords";

        if (appendedCount > 0) {
            emit keywordsCountChanged();
            setKeywordsModified(true);

            m_CommandManager->submitItemForSpellCheck(&m_CommonKeywordsModel, Common::SpellCheckFlags::Keywords);
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
        LOG_DEBUG << "#";
        if (m_CommonKeywordsModel.clearKeywords()) {
            setKeywordsModified(true);
            emit keywordsCountChanged();
        }
    }

    void CombinedArtworksModel::suggestCorrections() {
        LOG_DEBUG << "#";
        m_CommandManager->setupSpellCheckSuggestions(&m_CommonKeywordsModel, -1, Common::SuggestionFlags::All);
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
        LOG_DEBUG << "#";
        if (!m_CommonKeywordsModel.getDescription().trimmed().isEmpty()) {
            m_CommandManager->submitItemForSpellCheck(&m_CommonKeywordsModel, Common::SpellCheckFlags::Description);
        } else {
            m_CommonKeywordsModel.notifySpellCheckResults(Common::SpellCheckFlags::Description);
        }
    }

    void CombinedArtworksModel::spellCheckTitle() {
        LOG_DEBUG << "#";
        if (!m_CommonKeywordsModel.getTitle().trimmed().isEmpty()) {
            m_CommandManager->submitItemForSpellCheck(&m_CommonKeywordsModel, Common::SpellCheckFlags::Title);
        } else {
            m_CommonKeywordsModel.notifySpellCheckResults(Common::SpellCheckFlags::Title);
        }
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

    bool CombinedArtworksModel::hasTitleWordSpellError(const QString &word) {
        return m_CommonKeywordsModel.hasTitleWordSpellError(word);
    }

    bool CombinedArtworksModel::hasDescriptionWordSpellError(const QString &word) {
        return m_CommonKeywordsModel.hasDescriptionWordSpellError(word);
    }

    void CombinedArtworksModel::plainTextEdit(const QString &rawKeywords) {
        QStringList keywords = rawKeywords.trimmed().split(QChar(','), QString::SkipEmptyParts);

        m_CommonKeywordsModel.setKeywords(keywords);
        setKeywordsModified(true);
        emit keywordsCountChanged();
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
