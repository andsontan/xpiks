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

#ifndef WARNINGSSETTINGSMODEL_H
#define WARNINGSSETTINGSMODEL_H

#include <QJsonDocument>
#include <QJsonArray>
#include "../Models/abstractconfigupdatermodel.h"

namespace AutoComplete {
    class WarningsSettingsModel:
        public Models::AbstractConfigUpdaterModel
    {
    Q_OBJECT

    public:
        WarningsSettingsModel();
        void initializeConfigs();

        const QString &getAllowedFilenameCharacters() const { return m_AllowedFilenameCharacters; }
        double getMinMegapixels() const { return m_MinMegapixels; }
        int getMaxKeywordsCount() const { return m_MaxKeywordsCount; }
        int getMaxDescriptionLength() const { return m_MaxDescriptionLength; }

        // AbstractConfigUpdaterModel interface

    protected:
        virtual void processRemoteConfig(const QJsonDocument &remoteDocument, bool overwriteLocal);
        virtual bool parseConfig(const QJsonDocument &document);

        // CompareValuesJson interface

    public:
        virtual int operator ()(const QJsonObject &val1, const QJsonObject &val2);

    private:
        QString m_AllowedFilenameCharacters;
        double m_MinMegapixels;
        int m_MaxKeywordsCount;
        int m_MaxDescriptionLength;
    };
}
#endif // WARNINGSSETTINGSMODEL_H
