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

#include "gettyqueryengine.h"
#include <QObject>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QUrlQuery>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include "../Encryption/aes-qt.h"
#include "../Common/defines.h"
#include "suggestionartwork.h"

namespace Suggestion {
    GettyQueryEngine::GettyQueryEngine():
        m_NetworkManager(this)
    {
        m_GettyImagesAPIKey = QLatin1String("17a45639c3bf88f7a6d549759af398090c3f420e53a61a06d7a2a2b153c89fc9470b2365dae8c6d92203287dc6f69f55b230835a8fb2a70b24e806771b750690");

        QObject::connect(&m_NetworkManager, SIGNAL(finished(QNetworkReply*)),
                         this, SLOT(replyReceived(QNetworkReply*)));
    }

    void GettyQueryEngine::submitQuery(const QStringList &queryKeywords) {
        LOG_INFO << queryKeywords;
        QUrl url = buildQuery(queryKeywords);
        QNetworkRequest request(url);


        QString decodedAPIKey = Encryption::decodeText(m_GettyImagesAPIKey, "MasterPassword");
        LOG_INFO << decodedAPIKey;
        request.setRawHeader("Api-Key", decodedAPIKey.toLocal8Bit());

        QNetworkReply *reply = m_NetworkManager.get(request);
        QObject::connect(this, SIGNAL(cancelAllQueries()),
                         reply, SLOT(abort()));
    }

    void GettyQueryEngine::replyReceived(QNetworkReply *networkReply) {
        LOG_DEBUG << "#";
        if (networkReply->error() == QNetworkReply::NoError) {
            QJsonDocument document = QJsonDocument::fromJson(networkReply->readAll());
            QJsonObject jsonObject = document.object();
            QJsonValue nbResultsValue = jsonObject["result_count"];

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

    void GettyQueryEngine::parseResponse(const QJsonObject &jsonObject, int count,
                                         std::vector<std::shared_ptr<SuggestionArtwork> > &suggestionArtworks) {
        LOG_DEBUG << "#";
        Q_UNUSED(count);
        if (!jsonObject.contains("images")) { return; }
        QJsonValue imagesValue = jsonObject["images"];

        if (imagesValue.isArray()) {
            QJsonArray imagesArray = imagesValue.toArray();
            int size = imagesArray.size();
            for (int i = 0; i < size; ++i) {
                QJsonValue element = imagesArray.at(i);
                if (!element.isObject()) { continue; }

                QJsonObject item = element.toObject();

                QString url;
                QStringList keywords;

                if (item.contains("display_sizes")) {
                    parseUrl(item["display_sizes"], url);
                }

                if (item.contains("keywords")) {
                    parseKeywords(item["keywords"], keywords);
                }

                suggestionArtworks.emplace_back(new SuggestionArtwork(url, keywords, false));
            }
        }
    }

    void GettyQueryEngine::parseUrl(const QJsonValue &previewObject, QString &url) {
        if (!previewObject.isArray()) { return; }

        QJsonArray previews = previewObject.toArray();
        int size = previews.size();

        if (size > 0) {
            QJsonValue element = previews.first();
            if (element.isObject()) {
                QJsonObject firstItem = element.toObject();
                if (firstItem.contains("uri") && firstItem.contains("is_watermarked")) {
                    QJsonValue value = firstItem["uri"];
                    if (value.isString()) {
                        url = value.toString();
                    }
                }
            }
        }
    }

    void GettyQueryEngine::parseKeywords(const QJsonValue &keywordsObject, QStringList &keywords) {
        if (keywordsObject.isArray()) {
            QJsonArray keywordsArray = keywordsObject.toArray();

            int size = keywordsArray.size();
            keywords.reserve(size);

            for (int i = 0; i < size; ++i) {
                QJsonValue element = keywordsArray.at(i);
                if (!element.isObject()) { continue; }

                QJsonObject keywordItemObject = element.toObject();
                if (keywordItemObject.contains("text")) {
                    QJsonValue textValue = keywordItemObject["text"];
                    if (textValue.isString()) {
                        keywords.append(textValue.toString());
                    }
                }
            }
        }
    }

    QUrl GettyQueryEngine::buildQuery(const QStringList &queryKeywords) const {
        QUrlQuery urlQuery;

        urlQuery.addQueryItem("fields", "keywords,preview,title");
        urlQuery.addQueryItem("phrase", queryKeywords.join(' '));
        urlQuery.addQueryItem("page", "1");
        urlQuery.addQueryItem("page_size", "100");
        urlQuery.addQueryItem("sort_order", "most_popular");

        QUrl url;
        url.setUrl(QLatin1String("https://api.gettyimages.com:443/v3/search/images"));
        url.setQuery(urlQuery);
        return url;
    }
}
