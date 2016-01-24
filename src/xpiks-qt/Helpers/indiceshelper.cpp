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

#include "indiceshelper.h"

namespace Helpers {
    void indicesToRanges(const QVector<int> &indices, QVector<QPair<int, int> > &ranges) {
        if (indices.empty()) { return; }

        int currentStart = indices[0];
        int indicesCount = indices.count();
        ranges.reserve(indicesCount/3);

        for (int i = 1; i < indicesCount; ++i) {
            if (indices[i] - indices[i - 1] > 1) {
                ranges.append(qMakePair(currentStart, indices[i - 1]));
                currentStart = indices[i];
            }
        }

        ranges.append(qMakePair(currentStart, indices[indicesCount - 1]));
    }

    int getRangesLength(const QVector<QPair<int, int> > &ranges) {
        int sum = 0;

        int length = ranges.length();
        for (int i = 0; i < length; ++i) {
            const QPair<int, int> &pair = ranges[i];
            sum += pair.second - pair.first + 1;
        }

        return sum;
    }
}
