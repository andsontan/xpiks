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

#ifndef WARNINGSINFO_H
#define WARNINGSINFO_H

#include <QStringList>
#include <QString>

namespace Models {
    class WarningsInfo
    {
    public:
        WarningsInfo(const QString &filePath) :
            m_Filepath(filePath)
        {}

        ~WarningsInfo() {}

    public:
        void addWarning(const QString &warning) { m_WarningsList.append(warning); }

    public:
        const QString &getFilePath() const { return m_Filepath; }
        const QStringList &getWarnings() const { return m_WarningsList; }

    private:
        QStringList m_WarningsList;
        QString m_Filepath;
    };
}

#endif // WARNINGSINFO_H
