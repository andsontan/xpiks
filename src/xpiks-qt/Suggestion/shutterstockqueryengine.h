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

#ifndef SUGGESTIONQUERYENGINE_H
#define SUGGESTIONQUERYENGINE_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QStringList>
#include <QJsonArray>
#include "suggestionqueryenginebase.h"

namespace Suggestion {
    class KeywordsSuggestor;
    class LocalLibrary;
    class SuggestionArtwork;

    class ShutterstockQueryEngine : public SuggestionQueryEngineBase
    {
        Q_OBJECT
    public:
        ShutterstockQueryEngine(int engineID);

    public:
        virtual void submitQuery(const QStringList &queryKeywords);
        virtual QString getName() const { return tr("Shutterstock"); }

    private slots:
        void replyReceived(QNetworkReply *networkReply);

    private:
        void parseResponse(const QJsonArray &jsonArray,
                           std::vector<std::shared_ptr<SuggestionArtwork> > &suggestionArtworks);
        QUrl buildQuery(const QStringList &queryKeywords) const;

    private:
        QNetworkAccessManager m_NetworkManager;
        QString m_ClientId;
        QString m_ClientSecret;
    };
}

#endif // SUGGESTIONQUERYENGINE_H
