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

#ifndef IBASICARTWORK
#define IBASICARTWORK

#include "isafepointer.h"
#include <QtGlobal>

namespace Common {
    class IBasicArtwork : public ISafePointer {
    public:
        virtual ~IBasicArtwork() {}

        virtual const QSet<QString> &getKeywordsSet() const = 0;
        virtual const QStringList &getKeywords() const = 0;
        virtual bool isEmpty() const = 0;

        virtual const QString &getDescription() const = 0;
        virtual const QString &getTitle() const = 0;
        virtual const QString &getFilepath() const = 0;

        virtual qint64 getItemID() const { return -1; }
    };
}

#endif // IBASICARTWORK

