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

#include "fotoliaqueryengine.h"
#include <QObject>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QUrlQuery>
#include <QJsonObject>
#include <QJsonDocument>
#include "../Encryption/aes-qt.h"
#include "../Common/defines.h"
#include "suggestionartwork.h"

namespace Suggestion {
    FotoliaQueryEngine::FotoliaQueryEngine(int engineID):
        SuggestionQueryEngineBase(engineID),
        m_NetworkManager(this)
    {
        m_FotoliaAPIKey = "ad2954b4ee1e9686fbf8446f85e0c26edfae6003f51f49ca5559aed915879e733bbaf2003b3575bc0b96e682a30a69907c612865ec8f4ec2522131108a4a9f24467f1f83befc3d80201e5f906c761341";

        QObject::connect(&m_NetworkManager, SIGNAL(finished(QNetworkReply*)),
                         this, SLOT(replyReceived(QNetworkReply*)));
    }

    void FotoliaQueryEngine::submitQuery(const QStringList &queryKeywords) {
        LOG_INFO << queryKeywords;

        QString decodedAPIKey = Encryption::decodeText(m_FotoliaAPIKey, "MasterPassword");

        QUrl url = buildQuery(decodedAPIKey, queryKeywords);
        QNetworkRequest request(url);

        QNetworkReply *reply = m_NetworkManager.get(request);
        QObject::connect(this, SIGNAL(cancelAllQueries()),
                         reply, SLOT(abort()));
    }

    void FotoliaQueryEngine::replyReceived(QNetworkReply *networkReply) {
        LOG_DEBUG << "#";
        if (networkReply->error() == QNetworkReply::NoError) {
            QJsonDocument document = QJsonDocument::fromJson(networkReply->readAll());
            QJsonObject jsonObject = document.object();
            QJsonValue nbResultsValue = jsonObject["nb_results"];

            if (!nbResultsValue.isUndefined()) {
                int resultsNumber = nbResultsValue.toInt();
                std::vector<std::shared_ptr<SuggestionArtwork> > suggestionArtworks;
                parseResponse(jsonObject, resultsNumber, suggestionArtworks);
                setResults(suggestionArtworks);
                emit resultsAvailable();
            }
        } else {
            LOG_WARNING << "error:" << networkReply->errorString();
            emit errorReceived(networkReply->errorString());
        }

        networkReply->deleteLater();
    }

    void FotoliaQueryEngine::parseResponse(const QJsonObject &jsonObject, int count,
                                           std::vector<std::shared_ptr<SuggestionArtwork> > &suggestionArtworks) {
        LOG_DEBUG << "#";
        for (int i = 0; i < count; ++i) {
            QJsonValue item = jsonObject[QString::number(i)];
            if (!item.isObject()) { continue; }
            QJsonObject object = item.toObject();

            QJsonValue url = object["thumbnail_url"];
            if (!url.isString()) { continue; }

            QJsonValue keywords = object["keywords"];
            if (!keywords.isString()) { continue; }

            QStringList keywordsList = keywords.toString().split(',');

            suggestionArtworks.emplace_back(new SuggestionArtwork(url.toString(), keywordsList, false));
        }
    }

    QUrl FotoliaQueryEngine::buildQuery(const QString &apiKey, const QStringList &queryKeywords) const {
        QUrlQuery urlQuery;

        urlQuery.addQueryItem("search_parameters[language_id]", "2");
        urlQuery.addQueryItem("search_parameters[thumbnail_size]", "160");
        urlQuery.addQueryItem("search_parameters[limit]", "100");
        urlQuery.addQueryItem("search_parameters[order]", "nb_downloads");
        urlQuery.addQueryItem("search_parameters[words]", queryKeywords.join(' '));
        urlQuery.addQueryItem("result_columns[0]", "nb_results");
        urlQuery.addQueryItem("result_columns[1]", "title");
        urlQuery.addQueryItem("result_columns[2]", "keywords");
        urlQuery.addQueryItem("result_columns[3]", "thumbnail_url");

        QUrl url;
        url.setUrl(QLatin1String("http://api.fotolia.com/Rest/1/search/getSearchResults"));
        url.setUserName(apiKey);
        //url.setPassword("");
        url.setQuery(urlQuery);
        return url;
    }
}
