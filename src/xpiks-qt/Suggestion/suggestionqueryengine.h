/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2015 Taras Kushnir <kushnirTV@gmail.com>
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

#ifndef SUGGESTIONQUERYENGINE_H
#define SUGGESTIONQUERYENGINE_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QStringList>
#include <QJsonArray>

namespace Suggestion {
    class KeywordsSuggestor;
    class LocalLibrary;
    class SuggestionArtwork;

    class SuggestionQueryEngine : public QObject
    {
        Q_OBJECT
    public:
        SuggestionQueryEngine(KeywordsSuggestor *keywordsSuggestor);

    public:
        void submitQuery(const QStringList &queryKeywords);
        void submitLocalQuery(LocalLibrary *localLibrary, const QStringList &queryKeywords);
        void cancelQueries();

    private slots:
        void replyReceived(QNetworkReply *networkReply);
        void artworksFound(QVector<SuggestionArtwork *> *suggestions);

    signals:
        void searchResultsRetrieved(const QVector<SuggestionArtwork*> &artworks);
        void cancelAllQueries();

    private:
        void parseResponse(const QJsonArray &jsonArray, QVector<SuggestionArtwork *> &suggestionArtworks);
        QUrl buildQuery(const QStringList &queryKeywords) const;

    private:
        QNetworkAccessManager m_NetworkManager;
        KeywordsSuggestor *m_Suggestor;
        QString m_ClientId;
        QString m_ClientSecret;
    };
}

#endif // SUGGESTIONQUERYENGINE_H
