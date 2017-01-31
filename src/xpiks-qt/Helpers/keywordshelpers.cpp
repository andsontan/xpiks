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

#include "keywordshelpers.h"
#include <QRegularExpression>

#define ALLOWED_SYMBOLS "@#%&!?"

namespace Helpers {
    bool isValidKeyword(const QString &keyword) {
        bool isValid = false;
        int length = keyword.length();

        if (2 <= length && length < 30) {
            isValid = true;
        }

        if (length == 1) {
            QChar symbol = keyword.at(0);
            if (symbol.isLetterOrNumber() ||
                    QString(ALLOWED_SYMBOLS).contains(symbol) ||
                    symbol.category() == QChar::Symbol_Currency) {
                isValid = true;
            }
        }

        return isValid;
    }

    QString doSanitizeKeyword(const QString &keyword) {
        QString allowed = QString(ALLOWED_SYMBOLS);
        int start = 0;
        int length = keyword.length();
        QChar c;
        while (start < length) {
            c = keyword.at(start);
            if (c.isLetterOrNumber() ||
                    allowed.contains(c) ||
                    c.category() == QChar::Symbol_Currency) {
                break;
            } else {
                start++;
            }
        }

        int end = length - 1;
        while (end >= 0) {
            c = keyword.at(end);
            if (c.isLetterOrNumber() ||
                    allowed.contains(c) ||
                    c.category() == QChar::Symbol_Currency) {
                break;
            } else {
                end--;
            }
        }

        QString result;
        if (start <= end) {
            result = keyword.mid(start, end - start + 1).simplified();
        }

        return result;
    }
}
