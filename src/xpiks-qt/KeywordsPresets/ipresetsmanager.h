/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
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

#ifndef IPRESETSMANAGER_H
#define IPRESETSMANAGER_H

#include <QString>
#include <QVector>
#include <QPair>

namespace KeywordsPresets {
    class IPresetsManager {
    public:
        virtual ~IPresetsManager() {}

        virtual bool tryGetPreset(int presetIndex, QStringList &keywords) = 0;
        virtual bool tryFindSinglePresetByName(const QString &name, bool strictMatch, int &index) = 0;
        virtual void findPresetsByName(const QString &name, QVector<QPair<int, QString> > &results) = 0;
        virtual void findOrRegisterPreset(const QString &name, const QStringList &keywords, int &index) = 0;
        virtual void requestBackup() = 0;
    };
}

#endif // IPRESETSMANAGER_H
