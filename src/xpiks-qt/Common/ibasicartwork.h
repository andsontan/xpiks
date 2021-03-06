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

#ifndef IBASICARTWORK
#define IBASICARTWORK

#include <QtGlobal>
#include <QStringList>
#include <QSet>
#include <QString>

namespace Common {
    class IBasicArtwork {
    public:
        virtual ~IBasicArtwork() {}

        virtual QSet<QString> getKeywordsSet() = 0;
        virtual QStringList getKeywords() = 0;
        virtual bool isEmpty() = 0;

        virtual QString getDescription() = 0;
        virtual QString getTitle() = 0;
        virtual const QString &getFilepath() const = 0;

        virtual qint64 getItemID() const { return -1; }
    };
}

#endif // IBASICARTWORK

