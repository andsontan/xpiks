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

#ifndef IMETADATAOPERATOR_H
#define IMETADATAOPERATOR_H

#include <QString>

namespace Common {
    class IMetadataOperator {
    public:
        virtual ~IMetadataOperator() {}

        virtual bool editKeyword(int index, const QString &replacement) = 0;
        virtual bool removeKeywordAt(int index, QString &removedKeyword) = 0;
        virtual bool removeLastKeyword(QString &removedKeyword) = 0;
        virtual bool appendKeyword(const QString &keyword) = 0;
        virtual int appendKeywords(const QStringList &keywordsList) = 0;
        virtual bool clearKeywords() = 0;
        virtual QString getKeywordsString() = 0;
        virtual void setKeywords(const QStringList &keywords) = 0;
        virtual bool setDescription(const QString &value) = 0;
        virtual bool setTitle(const QString &value) = 0;
    };
}

#endif // IMETADATAOPERATOR_H
