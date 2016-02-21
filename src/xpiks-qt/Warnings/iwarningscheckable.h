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

#ifndef IWARNINGSCHECKABLE
#define IWARNINGSCHECKABLE

#include <QSize>
#include <QStringList>
#include "../Common/ibasicartwork.h"

namespace Warnings {
    class IWarningsCheckable : public virtual Common::IBasicArtwork {
    public:
        virtual ~IWarningsCheckable() {}

        virtual QSize getImageSize() const = 0;
        virtual qint64 getFileSize() const = 0;
        virtual int getKeywordsCount() const = 0;
        virtual bool hasTitleSpellError() const = 0;
        virtual bool hasDescriptionSpellError() const = 0;
        virtual bool hasKeywordsSpellError() const = 0;
        virtual QStringList getDescriptionWords() const = 0;
        virtual QStringList getTitleWords() const = 0;
        virtual void setWarningsFlags(int warningsFlags) = 0;
        virtual void addWarningsFlags(int warningsFlags) = 0;
        virtual void dropWarningsFlags(int flagsToDrop) = 0;
    };
}

#endif // IWARNINGSCHECKABLE

