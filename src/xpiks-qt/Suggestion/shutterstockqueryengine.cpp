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

#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QByteArray>
#include <QString>
#include "shutterstockqueryengine.h"
#include "suggestionartwork.h"
#include "keywordssuggestor.h"
#include "../Encryption/aes-qt.h"
#include "libraryqueryworker.h"
#include "locallibrary.h"
#include "../Common/defines.h"

namespace Suggestion {
    ShutterstockQueryEngine::ShutterstockQueryEngine():
        m_NetworkManager(this)
    {
        m_ClientId = "28a2a9b917961a0cbc343c81b2dd0f6618377f9210aa3182e5cc9f5588f914d918ede1533c9e06b91769c89e80909743";
        m_ClientSecret = "5092d9a967c2f19b57aac29bc09ac3b9e6ae5baec1a371331b73ff24f1625d95c4f3fef90bdacfbe9b0b3803b48c269192bc55f14bb9c2b5a16d650cd641b746eb384fcf9dbd53a96f1f81215921b04409f3635ecf846ffdf01ee04ba76624c9";

        QObject::connect(&m_NetworkManager, SIGNAL(finished(QNetworkReply*)),
                         this, SLOT(replyReceived(QNetworkReply*)));
    }

    void ShutterstockQueryEngine::submitQuery(const QStringList &queryKeywords) {
        LOG_INFO << queryKeywords;
        QUrl url = buildQuery(queryKeywords);
        QNetworkRequest request(url);

        QString decodedClientId = Encryption::decodeText(m_ClientId, "MasterPassword");
        QString decodedClientSecret = Encryption::decodeText(m_ClientSecret, "MasterPassword");

        QString authStr = QString("%1:%2").arg(decodedClientId).arg(decodedClientSecret);
        QString headerData = "Basic " + QString::fromLatin1(authStr.toLocal8Bit().toBase64());
        request.setRawHeader("Authorization", headerData.toLocal8Bit());

        QNetworkReply *reply = m_NetworkManager.get(request);
        QObject::connect(this, SIGNAL(cancelAllQueries()),
                         reply, SLOT(abort()));
    }

    void ShutterstockQueryEngine::replyReceived(QNetworkReply *networkReply) {
        LOG_DEBUG << "#";
        if (networkReply->error() == QNetworkReply::NoError) {
            QJsonDocument document = QJsonDocument::fromJson(networkReply->readAll());
            QJsonObject jsonObject = document.object();
            QJsonValue dataValue = jsonObject["data"];

            if (dataValue.isArray()) {
                std::vector<std::shared_ptr<SuggestionArtwork> > suggestionArtworks;
                parseResponse(dataValue.toArray(), suggestionArtworks);
                setResults(suggestionArtworks);
                emit resultsAvailable();
            }
        } else {
            LOG_WARNING << "error:" << networkReply->errorString();
            emit errorReceived(networkReply->errorString());
        }

        networkReply->deleteLater();
    }

    void ShutterstockQueryEngine::parseResponse(const QJsonArray &jsonArray,
                                                std::vector<std::shared_ptr<SuggestionArtwork> > &suggestionArtworks) {
        LOG_DEBUG << "#";
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

                        suggestionArtworks.emplace_back(new SuggestionArtwork(url, keywordsList, false));
                    }
                }
            }
        }
    }

    QUrl ShutterstockQueryEngine::buildQuery(const QStringList &queryKeywords) const {
        QUrlQuery urlQuery;

        urlQuery.addQueryItem("language", "en");
        urlQuery.addQueryItem("view", "full");
        urlQuery.addQueryItem("per_page", "100");
        urlQuery.addQueryItem("sort", "popular");
        urlQuery.addQueryItem("query", queryKeywords.join(' '));

        QUrl url;
        url.setUrl(QLatin1String("https://api.shutterstock.com/v2/images/search"));
        url.setQuery(urlQuery);
        return url;
    }
}

