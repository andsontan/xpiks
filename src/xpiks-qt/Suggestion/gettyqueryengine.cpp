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
#include <QUrl>
#include <QUrlQuery>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include "../Encryption/aes-qt.h"
#include "../Conectivity/simplecurlrequest.h"
#include "../Models/settingsmodel.h"
#include "../Common/defines.h"
#include "suggestionartwork.h"

namespace Suggestion {
    GettyQueryEngine::GettyQueryEngine(int engineID, Models::SettingsModel *settingsModel):
        SuggestionQueryEngineBase(engineID, settingsModel)
    {
        m_GettyImagesAPIKey = QLatin1String("17a45639c3bf88f7a6d549759af398090c3f420e53a61a06d7a2a2b153c89fc9470b2365dae8c6d92203287dc6f69f55b230835a8fb2a70b24e806771b750690");
    }

    void GettyQueryEngine::submitQuery(const QStringList &queryKeywords, QueryResultsType resultsType) {
        LOG_INFO << queryKeywords;
        QUrl url = buildQuery(queryKeywords, resultsType);

        QString decodedAPIKey = Encryption::decodeText(m_GettyImagesAPIKey, "MasterPassword");

        auto *settings = getSettingsModel();
        auto *proxySettings = settings->retrieveProxySettings();

        QString resourceUrl = QString::fromLocal8Bit(url.toEncoded());
        Conectivity::SimpleCurlRequest *request = new Conectivity::SimpleCurlRequest(resourceUrl);
        request->setRawHeaders(QStringList() << "Api-Key: " + decodedAPIKey);
        request->setProxySettings(proxySettings);

        QThread *thread = new QThread();

        request->moveToThread(thread);

        QObject::connect(thread, SIGNAL(started()), request, SLOT(process()));
        QObject::connect(request, SIGNAL(stopped()), thread, SLOT(quit()));

        QObject::connect(request, SIGNAL(stopped()), request, SLOT(deleteLater()));
        QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

        QObject::connect(request, SIGNAL(requestFinished(bool)), this, SLOT(requestFinishedHandler(bool)));

        thread->start();
    }

    void GettyQueryEngine::requestFinishedHandler(bool success) {
        LOG_INFO << "success:" << success;

        Conectivity::SimpleCurlRequest *request = qobject_cast<Conectivity::SimpleCurlRequest *>(sender());

        if (success) {
            QJsonParseError error;

            QJsonDocument document = QJsonDocument::fromJson(request->getResponseData(), &error);
            if (error.error == QJsonParseError::NoError) {
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
                LOG_WARNING << "parsing error:" << error.errorString();
                emit errorReceived(tr("Can't parse the response"));
            }
        } else {
            LOG_WARNING << "error:" << request->getErrorString();
            emit errorReceived(request->getErrorString());
        }

        request->dispose();
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

                // TODO: parse external url from istock
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

    QUrl GettyQueryEngine::buildQuery(const QStringList &queryKeywords, QueryResultsType resultsType) const {
        QUrlQuery urlQuery;

        urlQuery.addQueryItem("fields", "keywords,preview,title,id");
        urlQuery.addQueryItem("phrase", queryKeywords.join(' '));
        urlQuery.addQueryItem("page", "1");
        urlQuery.addQueryItem("page_size", "100");
        urlQuery.addQueryItem("sort_order", "most_popular");

        if (resultsType != QueryResultsType::AllImages) {
            urlQuery.addQueryItem("graphical_styles", resultsTypeToString(resultsType));
        }

        QUrl url;
        url.setUrl(QLatin1String("https://api.gettyimages.com:443/v3/search/images"));
        url.setQuery(urlQuery);
        return url;
    }

    QString GettyQueryEngine::resultsTypeToString(QueryResultsType resultsType) const {
        switch (resultsType) {
        case QueryResultsType::Photos: return QLatin1String("photography");
        case QueryResultsType::Vectors: return QLatin1String("illustration");
        case QueryResultsType::Illustrations: return QLatin1String("fine_art");
        default: return QString();
        }
    }
}
