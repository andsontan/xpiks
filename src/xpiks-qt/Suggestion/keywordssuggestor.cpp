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

#include <QHash>
#include <QString>
#include <QMultiMap>
#include "keywordssuggestor.h"
#include "suggestionartwork.h"
#include "../Commands/commandmanager.h"
#include "../Common/defines.h"
#include "suggestionqueryenginebase.h"
#include "shutterstockqueryengine.h"
#include "locallibraryqueryengine.h"
#include "fotoliaqueryengine.h"
#include "gettyqueryengine.h"

namespace Suggestion {
    KeywordsSuggestor::KeywordsSuggestor(LocalLibrary *library, QObject *parent):
        QAbstractListModel(parent),
        Common::BaseEntity(),
        m_LocalLibrary(library),
        m_SuggestedKeywords(m_HoldPlaceholder, this),
        m_AllOtherKeywords(m_HoldPlaceholder, this),
        m_SelectedArtworksCount(0),
        m_SelectedSourceIndex(0),
        m_IsInProgress(false)
    {
        setLastErrorString(tr("No results found"));
        qsrand(QTime::currentTime().msec());
    }

    KeywordsSuggestor::~KeywordsSuggestor() {
         qDeleteAll(m_QueryEngines);
    }

    void KeywordsSuggestor::initSuggestionEngines() {
        Q_ASSERT(m_CommandManager != NULL);
        auto *settingsModel = m_CommandManager->getSettingsModel();

        int id = 0;
        m_QueryEngines.append(new ShutterstockQueryEngine(id++, settingsModel));
        m_QueryEngines.append(new GettyQueryEngine(id++, settingsModel));
        m_QueryEngines.append(new FotoliaQueryEngine(id++, settingsModel));
        m_QueryEngines.append(new LocalLibraryQueryEngine(id++, m_LocalLibrary));

        int length = m_QueryEngines.length();
        for (int i = 0; i < length; ++i) {
            SuggestionQueryEngineBase *engine = m_QueryEngines.at(i);
            m_QueryEnginesNames.append(engine->getName());

            QObject::connect(engine, SIGNAL(resultsAvailable()),
                             this, SLOT(resultsAvailableHandler()));

            QObject::connect(engine, SIGNAL(errorReceived(QString)),
                             this, SLOT(errorsReceivedHandler(QString)));
        }
    }

    void KeywordsSuggestor::setSuggestedArtworks(std::vector<std::shared_ptr<SuggestionArtwork> > &suggestedArtworks) {
        LOG_INFO << suggestedArtworks.size() << "item(s)";
        m_SelectedArtworksCount = 0;
        m_KeywordsHash.clear();
        m_SuggestedKeywords.clearKeywords();
        m_AllOtherKeywords.clearKeywords();
        beginResetModel();
        m_Suggestions = std::move(suggestedArtworks);
        endResetModel();
        unsetInProgress();
        emit suggestionArrived();
        emit suggestedKeywordsCountChanged();
        emit otherKeywordsCountChanged();
        emit selectedArtworksCountChanged();
    }

    void KeywordsSuggestor::clear() {
        LOG_DEBUG << "#";
        m_SelectedArtworksCount = 0;
        m_KeywordsHash.clear();
        m_SuggestedKeywords.clearKeywords();
        m_AllOtherKeywords.clearKeywords();
        beginResetModel();
        m_Suggestions.clear();
        endResetModel();
        unsetInProgress();
        emit suggestedKeywordsCountChanged();
        emit otherKeywordsCountChanged();
        emit selectedArtworksCountChanged();
    }

    void KeywordsSuggestor::setSelectedSourceIndex(int value) {
        if (!m_IsInProgress && (value != m_SelectedSourceIndex)) {
            if (0 <= value &&
                    value < m_QueryEngines.length()) {
                LOG_INFO << "Selected query source index:" << value;
                m_SelectedSourceIndex = value;
                emit selectedSourceIndexChanged();
            }
        }
    }

    void KeywordsSuggestor::resultsAvailableHandler() {
        Q_ASSERT(0 <= m_SelectedSourceIndex && m_SelectedSourceIndex < m_QueryEngines.length());
        SuggestionQueryEngineBase *currentEngine = m_QueryEngines.at(m_SelectedSourceIndex);
        SuggestionQueryEngineBase *senderEngine = qobject_cast<SuggestionQueryEngineBase *>(sender());

        if ((senderEngine == nullptr) || (senderEngine->getID() != currentEngine->getID())) {
            qInfo() << "Received results from another engine:" << senderEngine->getID() << "current:" << currentEngine->getID();
        }

        unsetInProgress();
        auto &results = currentEngine->getLastResults();
        setSuggestedArtworks(results);
    }

    void KeywordsSuggestor::errorsReceivedHandler(const QString &error) {
        unsetInProgress();
        setLastErrorString(error);
    }

    void KeywordsSuggestor::onLanguageChanged() {
        m_QueryEnginesNames.clear();
        int length = m_QueryEngines.length();

        for (int i = 0; i < length; ++i) {
            SuggestionQueryEngineBase *engine = m_QueryEngines.at(i);
            m_QueryEnginesNames.append(engine->getName());
        }

        setLastErrorString(tr("No results found"));
    }

    QString KeywordsSuggestor::removeSuggestedKeywordAt(int keywordIndex) {
        LOG_INFO << "Index:" << keywordIndex;

        QString keyword;
        if (m_SuggestedKeywords.takeKeywordAt(keywordIndex, keyword)) {
            emit suggestedKeywordsCountChanged();
            LOG_INFO << "Removed:" << keyword;
        }

        return keyword;
    }

    QString KeywordsSuggestor::removeOtherKeywordAt(int keywordIndex) {
        LOG_INFO << "Index:" << keywordIndex;

        QString keyword;
        if (m_AllOtherKeywords.takeKeywordAt(keywordIndex, keyword)) {
            emit otherKeywordsCountChanged();
            LOG_INFO << "Removed:" << keyword;
        }
        return keyword;
    }

    void KeywordsSuggestor::setArtworkSelected(int index, bool newState) {
        if (index < 0 || (size_t)index >= m_Suggestions.size()) {
            return;
        }

        auto &suggestionArtwork = m_Suggestions.at(index);
        suggestionArtwork->setIsSelected(newState);

        int sign = newState ? +1 : -1;
        accountKeywords(suggestionArtwork->getKeywordsSet(), sign);
        m_SelectedArtworksCount += sign;
        emit selectedArtworksCountChanged();

        QModelIndex qIndex = this->index(index);
        emit dataChanged(qIndex, qIndex, QVector<int>() << IsSelectedRole);
        updateSuggestedKeywords();
    }

    void KeywordsSuggestor::searchArtworks(const QString &searchTerm) {
        LOG_DEBUG << searchTerm;

        if (!m_IsInProgress && !searchTerm.trimmed().isEmpty()) {
            setInProgress();

            QStringList searchTerms = searchTerm.split(QChar::Space, QString::SkipEmptyParts);

            SuggestionQueryEngineBase *engine = m_QueryEngines.at(m_SelectedSourceIndex);
            engine->submitQuery(searchTerms);

            if (dynamic_cast<LocalLibraryQueryEngine*>(engine) == NULL) {
                m_CommandManager->reportUserAction(Conectivity::UserAction::SuggestionRemote);
            } else {
                m_CommandManager->reportUserAction(Conectivity::UserAction::SuggestionLocal);
            }
        }
    }

    void KeywordsSuggestor::cancelSearch() {
        LOG_DEBUG << "#";
        SuggestionQueryEngineBase *engine = m_QueryEngines.at(m_SelectedSourceIndex);
        engine->cancelQueries();
    }

    int KeywordsSuggestor::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return (int)m_Suggestions.size();
    }

    QVariant KeywordsSuggestor::data(const QModelIndex &index, int role) const {
        int row = index.row();
        if (row < 0 || (size_t)row >= m_Suggestions.size()) { return QVariant(); }

        auto &suggestionArtwork = m_Suggestions.at(row);

        switch (role) {
        case UrlRole:
            return suggestionArtwork->getUrl();
        case IsSelectedRole:
            return suggestionArtwork->getIsSelected();
        default:
            return QVariant();
        }
    }

    QHash<int, QByteArray> KeywordsSuggestor::roleNames() const {
        QHash<int, QByteArray> roles;
        roles[UrlRole] = "url";
        roles[IsSelectedRole] = "isselected";
        return roles;
    }

    void KeywordsSuggestor::accountKeywords(const QSet<QString> &keywords, int sign) {
        foreach(const QString &keyword, keywords) {
            if (m_KeywordsHash.contains(keyword)) {
                m_KeywordsHash[keyword] += sign;
            } else {
                m_KeywordsHash.insert(keyword, 1);
            }
        }
    }

    QSet<QString> KeywordsSuggestor::getSelectedArtworksKeywords() const {
        QSet<QString> allKeywords;
        size_t size = m_Suggestions.size();

        for (size_t i = 0; i < size; ++i) {
            auto &artwork = m_Suggestions.at(i);

            if (artwork->getIsSelected()) {
                const QSet<QString> &currentKeywords = artwork->getKeywordsSet();
                allKeywords.unite(currentKeywords);
            }
        }

        return allKeywords;
    }

    void KeywordsSuggestor::updateSuggestedKeywords() {
        QStringList suggestedKeywords, otherKeywords;
        QMultiMap<int, QString> selectedKeywords;
        int lowerThreshold, upperThreshold;
        calculateBounds(lowerThreshold, upperThreshold);

        QHash<QString, int>::const_iterator hashIt = m_KeywordsHash.constBegin();
        QHash<QString, int>::const_iterator hashItEnd = m_KeywordsHash.constEnd();

        for (; hashIt != hashItEnd; ++hashIt) {
            selectedKeywords.insert(hashIt.value(), hashIt.key());
        }

        QMultiMap<int, QString>::const_iterator it = selectedKeywords.constEnd();
        QMultiMap<int, QString>::const_iterator itBegin = selectedKeywords.constBegin();

        int maxSuggested = 35 + (qrand() % 10);
        int maxUpperBound = 40 + (qrand() % 5);
        int maxOthers = 35 + (qrand() % 10);

        suggestedKeywords.reserve(maxSuggested);
        otherKeywords.reserve(maxOthers);

        bool canAddToSuggested, canAddToOthers;

        while (it != itBegin) {
            --it;

            int frequency = it.key();

            if (frequency == 0) { continue; }

            int suggestedCount = suggestedKeywords.length();

            canAddToSuggested = (frequency >= upperThreshold) && (suggestedCount <= maxUpperBound);
            canAddToOthers = frequency >= lowerThreshold;

            const QString &frequentKeyword = it.value();

            if (canAddToSuggested ||
                    (canAddToOthers && (suggestedCount <= maxSuggested))) {
                suggestedKeywords.append(frequentKeyword);
            } else if (canAddToOthers || (otherKeywords.length() <= maxOthers)) {
                otherKeywords.append(frequentKeyword);

                if (otherKeywords.length() > maxOthers) {
                    break;
                }
            }
        }

        m_SuggestedKeywords.setKeywords(suggestedKeywords);
        m_AllOtherKeywords.setKeywords(otherKeywords);

        emit suggestedKeywordsCountChanged();
        emit otherKeywordsCountChanged();
    }

    void KeywordsSuggestor::calculateBounds(int &lowerBound, int &upperBound) const {
        if (m_SelectedArtworksCount <= 2) {
            lowerBound = 1;
            upperBound = qMax(m_SelectedArtworksCount, 1);
        } else if (m_SelectedArtworksCount <= 4) {
            lowerBound = 2;
            upperBound = 3;
        } else if (m_SelectedArtworksCount <= 5) {
            lowerBound = 2;
            upperBound = 3;
        } else if (m_SelectedArtworksCount <= 9) {
            upperBound = m_SelectedArtworksCount / 2;
            lowerBound = upperBound - 1;
        } else if (m_SelectedArtworksCount <= 15) {
            upperBound = m_SelectedArtworksCount / 2 - 1;
            lowerBound = upperBound - 2;
        } else {
            upperBound = m_SelectedArtworksCount / 2;
            lowerBound = upperBound - 2;
        }
    }
}
