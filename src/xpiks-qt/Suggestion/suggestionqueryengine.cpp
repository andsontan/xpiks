/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014 Taras Kushnir <kushnirTV@gmail.com>
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

#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include "suggestionqueryengine.h"
#include "suggestionartwork.h"
#include "keywordssuggestor.h"

namespace Suggestion {
    SuggestionQueryEngine::SuggestionQueryEngine(KeywordsSuggestor *keywordsSuggestor):
                    QObject(keywordsSuggestor)
    {
        m_NetworkManager = new QNetworkAccessManager(this);
        QObject::connect(m_NetworkManager, SIGNAL(finished(QNetworkReply*)),
                         this, SLOT(replyReceived(QNetworkReply*)));
    }

    void SuggestionQueryEngine::submitQuery(const QStringList &queryKeywords) {
        QString queryString = buildQuery(queryKeywords);
        m_NetworkManager->get(QNetworkRequest(QUrl(queryString)));
    }

    void SuggestionQueryEngine::replyReceived(QNetworkReply *networkReply) {
        if (networkReply->error() == QNetworkReply::NoError) {
            QJsonDocument document = QJsonDocument::fromJson(networkReply->readAll());
            QJsonObject jsonObject = document.object();
            QJsonValue dataValue = jsonObject["data"];

            if (dataValue.isArray()) {
                QList<SuggestionArtwork*> suggestionArtworks;
                parseResponse(dataValue.toArray(), suggestionArtworks);
                m_Suggestor->setSuggestedArtworks(suggestionArtworks);
            }
        }

        networkReply->deleteLater();
    }

    void SuggestionQueryEngine::parseResponse(const QJsonArray &jsonArray, QList<SuggestionArtwork*> &suggestionArtworks) {
        foreach (const QJsonValue &value, jsonArray) {
            QJsonObject imageResult = value.toObject();

            if (imageResult.contains("assets")) {
                QJsonObject assetsObject = imageResult["assets"].toObject();
                if (assetsObject.contains("large_thumb")) {
                    QJsonObject thumb = assetsObject["large_thumb"].toObject();
                    if (thumb.contains("url")) {
                        QString url = thumb["url"].toString();
                        QStringList keywordsList;

                        if (imageResult["keywords"].isArray()) {
                            foreach (const QJsonValue &keyword, imageResult["keywords"].toArray()) {
                                keywordsList.append(keyword.toString());
                            }
                        }

                        SuggestionArtwork *artwork = new SuggestionArtwork(url, keywordsList);
                        suggestionArtworks.append(artwork);
                    }
                }
            }
        }
    }

    QString SuggestionQueryEngine::buildQuery(const QStringList &queryKeywords) const {
        QUrlQuery urlQuery;
        const QString clientId = "9b9d014b29e6416f98c2";
        const QString clientSecret = "4be62c75cee7aa716e996d9e1336c3caa80e072c";

        QString queryString = QString("https://%1:%2@api.shutterstock.com/v2/images/search")
                .arg(clientId)
                .arg(clientSecret);

        urlQuery.setQuery(queryString);
        urlQuery.addQueryItem("language", "en");
        urlQuery.addQueryItem("view", "full");
        urlQuery.addQueryItem("per_page", "75");
        urlQuery.addQueryItem("query", queryKeywords.join(' '));

        QString request = urlQuery.query();
        return request;
    }
}

