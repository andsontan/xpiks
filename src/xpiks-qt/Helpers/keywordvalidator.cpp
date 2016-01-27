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

#include "keywordvalidator.h"
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
        return keyword.simplified();
    }
}
