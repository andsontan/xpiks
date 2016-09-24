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

#ifndef GETTYQUERYENGINE_H
#define GETTYQUERYENGINE_H

#include <QString>
#include "suggestionqueryenginebase.h"

namespace Models {
    class SettingsModel;
}

namespace Suggestion {
    class GettyQueryEngine : public SuggestionQueryEngineBase
    {
        Q_OBJECT
    public:
        GettyQueryEngine(int engineID, Models::SettingsModel *settingsModel);

    public:
        virtual void submitQuery(const QStringList &queryKeywords);
        virtual QString getName() const { return tr("iStock"); }

    private slots:
        void requestFinishedHandler(bool success);

    private:
        void parseResponse(const QJsonObject &jsonObject, int count,
                           std::vector<std::shared_ptr<SuggestionArtwork> > &suggestionArtworks);
        void parseUrl(const QJsonValue &previewObject, QString &url);
        void parseKeywords(const QJsonValue &keywordsObject, QStringList &keywords);
        QUrl buildQuery(const QStringList &queryKeywords) const;

    private:
        QString m_GettyImagesAPIKey;
    };
}

#endif // GETTYQUERYENGINE_H
