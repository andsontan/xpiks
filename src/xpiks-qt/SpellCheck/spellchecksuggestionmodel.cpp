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

#include "spellchecksuggestionmodel.h"
#include <QQmlEngine>
#include <QHash>
#include <QString>
#include "spellsuggestionsitem.h"
#include "../Models/artworkmetadata.h"
#include "spellcheckerservice.h"
#include "ispellcheckable.h"
#include "../Commands/commandmanager.h"
#include "../Common/flags.h"
#include "../Common/defines.h"
#include "../Common/basickeywordsmodel.h"

namespace SpellCheck {

    QVector<SpellSuggestionsItem *> combineSuggestionRequests(const QVector<SpellSuggestionsItem *> &items) {
        QHash<QString, QVector<SpellSuggestionsItem*> > dict;

        int size = items.size();
        for (int i = 0; i < size; ++i) {
            SpellSuggestionsItem *item = items.at(i);
            const QString &word = item->getWord();
            if (!dict.contains(word)) {
                dict.insert(word, QVector<SpellSuggestionsItem*>());
            }

            dict[word].append(item);
        }

        QVector<SpellSuggestionsItem *> result;
        result.reserve(size);

        QHashIterator<QString, QVector<SpellSuggestionsItem*> > i(dict);
        while (i.hasNext()) {
            i.next();

            const QVector<SpellSuggestionsItem*> &vector = i.value();

            if (vector.size() > 1) {
                result.append(new CombinedSpellSuggestions(i.key(), vector));
            } else {
                result.append(vector.first());
            }
        }

        return result;
    }

    SpellCheckSuggestionModel::SpellCheckSuggestionModel():
        QAbstractListModel(),
        Common::BaseEntity(),
        m_CurrentItem(NULL),
        m_ItemIndex(-1)
    {
    }

    SpellCheckSuggestionModel::~SpellCheckSuggestionModel() {
        qDeleteAll(m_SuggestionsList);
    }

    QObject *SpellCheckSuggestionModel::getSuggestionItself(int index) const {
        SpellSuggestionsItem *item = NULL;

        if (0 <= index && index < m_SuggestionsList.length()) {
            item = m_SuggestionsList.at(index);
            QQmlEngine::setObjectOwnership(item, QQmlEngine::CppOwnership);
        }

        return item;
    }

    void SpellCheckSuggestionModel::clearModel() {
        beginResetModel();
        qDeleteAll(m_SuggestionsList);
        m_SuggestionsList.clear();
        endResetModel();
        m_ItemIndex = -1;
    }

    void SpellCheckSuggestionModel::submitCorrections() const {
        LOG_DEBUG << "#";
        bool anyChanged = false;

        QVector<SpellSuggestionsItem *> failedItems;

        foreach (SpellSuggestionsItem *item, m_SuggestionsList) {
            if (item->anyReplacementSelected()) {
                item->replaceToSuggested(m_CurrentItem);

                if (!item->getReplacementSucceeded()) {
                    failedItems.append(item);
                } else {
                    anyChanged = true;
                }
            }
        }

        if (processFailedReplacements(failedItems)) {
            anyChanged = true;
        }

        if (anyChanged) {
            m_CurrentItem->afterReplaceCallback();
            m_CommandManager->submitItemForSpellCheck(m_CurrentItem);
        }

        if (m_ItemIndex != -1) {
            m_CommandManager->updateArtworks(QVector<int>() << m_ItemIndex);
        }
    }

    void SpellCheckSuggestionModel::resetAllSuggestions() {
        int size = m_SuggestionsList.length();

        for (int i = 0; i < size; ++i) {
            SpellSuggestionsItem *item = m_SuggestionsList.at(i);
            item->setReplacementIndex(-1);
        }
    }

    void SpellCheckSuggestionModel::setupModel(Common::BasicKeywordsModel *item, int index, int flags) {
        Q_ASSERT(item != NULL);
        LOG_INFO << "flags =" << flags;
        QVector<SpellSuggestionsItem*> requests;

        if (Common::HasFlag(flags, Common::CorrectKeywords)) {
            QVector<SpellSuggestionsItem*> keywordsSuggestionsRequests = item->createKeywordsSuggestionsList();
            requests << keywordsSuggestionsRequests;
        }

        if (Common::HasFlag(flags, Common::CorrectTitle)) {
            QVector<SpellSuggestionsItem*> titleSuggestionsRequests = item->createTitleSuggestionsList();
            requests << titleSuggestionsRequests;
        }

        if (Common::HasFlag(flags, Common::CorrectDescription)) {
            QVector<SpellSuggestionsItem*> descriptionSuggestionsRequests = item->createDescriptionSuggestionsList();
            requests << descriptionSuggestionsRequests;
        }

        QVector<SpellSuggestionsItem*> combinedRequests = combineSuggestionRequests(requests);
        LOG_INFO << combinedRequests.length() << "combined request(s)";

        QVector<SpellSuggestionsItem*> executedRequests = setupSuggestions(combinedRequests);
        LOG_INFO << executedRequests.length() << "executed request(s)";

#if defined(CORE_TESTS) || defined(INTEGRATION_TESTS)
        foreach(SpellSuggestionsItem *item, executedRequests) {
            LOG_INFO << item->toDebugString();
        }
#endif

        beginResetModel();
        m_CurrentItem = item;
        qDeleteAll(m_SuggestionsList);
        m_SuggestionsList.clear();
        m_SuggestionsList << executedRequests;
        endResetModel();

        m_ItemIndex = index;
    }

    bool SpellCheckSuggestionModel::processFailedReplacements(const QVector<SpellSuggestionsItem *> &failedReplacements) const {
        LOG_INFO << failedReplacements.size() << "failed items";

        QVector<KeywordSpellSuggestions *> candidatesToRemove;
        int size = failedReplacements.size();

        for (int i = 0; i < size; ++i) {
            SpellSuggestionsItem *item = failedReplacements.at(i);
            KeywordSpellSuggestions *keywordsItem = dynamic_cast<KeywordSpellSuggestions*>(item);

            if (keywordsItem != NULL) {
                if (keywordsItem->isPotentialDuplicate()) {
                    candidatesToRemove.append(keywordsItem);
                }
            } else {
                CombinedSpellSuggestions *combinedItem = dynamic_cast<CombinedSpellSuggestions*>(item);
                if (combinedItem != NULL) {
                    QVector<KeywordSpellSuggestions*> keywordsItems = combinedItem->getKeywordsDuplicateSuggestions();
                    candidatesToRemove += keywordsItems;
                }
            }
        }

        bool anyReplaced = m_CurrentItem->processFailedKeywordReplacements(candidatesToRemove);
        return anyReplaced;
    }

    QVector<SpellSuggestionsItem *> SpellCheckSuggestionModel::setupSuggestions(const QVector<SpellSuggestionsItem *> &items) {
        LOG_INFO << items.length() << "item(s)";
        SpellCheckerService *service = m_CommandManager->getSpellCheckerService();
        // another vector for requests with available suggestions
        QVector<SpellSuggestionsItem*> executedRequests;
        executedRequests.reserve(items.length());

        foreach (SpellSuggestionsItem* item, items) {
            QStringList suggestions = service->suggestCorrections(item->getWord());
            if (!suggestions.isEmpty()) {
                item->setSuggestions(suggestions);
                executedRequests.append(item);
            } else {
                delete item;
            }
        }

        return executedRequests;
    }

    int SpellCheckSuggestionModel::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return m_SuggestionsList.length();
    }

    QVariant SpellCheckSuggestionModel::data(const QModelIndex &index, int role) const {
        int row = index.row();
        if (row < 0 || row >= m_SuggestionsList.length()) { return QVariant(); }

        SpellSuggestionsItem *item = m_SuggestionsList.at(row);

        switch (role) {
        case WordRole:
            return item->getWord();
        case ReplacementIndexRole:
            return item->getReplacementIndex();
        case ReplacementOriginRole:
            return item->getReplacementOrigin();
        default:
            return QVariant();
        }
    }

    QHash<int, QByteArray> SpellCheckSuggestionModel::roleNames() const {
        QHash<int, QByteArray> roles;
        roles[WordRole] = "word";
        roles[ReplacementIndexRole] = "replacementindex";
        roles[ReplacementOriginRole] = "replacementorigin";
        return roles;
    }
}
