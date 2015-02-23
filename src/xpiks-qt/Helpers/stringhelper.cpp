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

#include "stringhelper.h"

namespace Helpers {
    QString getLastNLines(const QString &line, int N) {
        QString result;

        QVector<QStringRef> items = line.splitRef(QRegExp("[\r\n]"),QString::SkipEmptyParts);

        int length = items.length();

        if (length > 0) {
            int startIndex = length - N;
            if (startIndex < 0) { startIndex = 0; }
            result = items[startIndex].toString();
            for (int i = startIndex + 1; i < length; ++i) {
                // TODO: use QStringBuilder
                result += "\n" + items[i].toString();
            }
        } else {
            result = line;
        }

        return result;
    }
}


