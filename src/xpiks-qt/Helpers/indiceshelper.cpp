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
#include <cmath>
#include <algorithm>

namespace Models {
    class ArtworkMetadata;
}

namespace Helpers {
    template<class T>
    int splitIntoChunks(const QVector<T> &items, int chunksCount, QVector<QVector<T> > &chunks) {
        int size = items.size();

        if (items.isEmpty() || (chunksCount == 0) || (chunksCount > size)) { return 0; }

        if (chunksCount == 1) {
            chunks << items;
            return 1;
        }

        int chunkSize = ceil((size + 0.0) / chunksCount);

        int left = chunkSize;
        while (left < size) {
            chunks.push_back(items.mid(left - chunkSize, chunkSize));
            left += chunkSize;
        }

        if (left - chunkSize < size) {
            chunks.push_back(items.mid(left - chunkSize, chunkSize));
        }

        return chunks.length();
    }

    template
    int splitIntoChunks<Models::ArtworkMetadata*>(const QVector<Models::ArtworkMetadata*> &items,
    int chunksCount, QVector<QVector<Models::ArtworkMetadata*> > &chunks);

#ifdef CORE_TESTS
    template
    int splitIntoChunks<int>(const QVector<int> &items, int chunksCount, QVector<QVector<int> > &chunks);
#endif

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

    bool segmentsOverlap(const std::pair<int, int> &a, const std::pair<int, int> &b) {
        if (a.first <= b.first) {
            return b.first <= a.second;
        } else {
            return a.first <= b.second;
        }
    }

    std::pair<int, int> unionOverlappingSegments(const std::pair<int, int> &a, const std::pair<int, int> &b) {
        Q_ASSERT(segmentsOverlap(a, b));
        return std::make_pair(std::min(a.first, b.first), std::max(a.second, b.second));
    }

    RangesVector unionRanges(RangesVector &ranges) {
        if (ranges.size() <= 1) { return ranges; }

        std::sort(std::begin(ranges), std::end(ranges),
                  [](const std::pair<int, int> &a, const std::pair<int, int> &b) {
            return (a.first == b.first) ? (a.second < b.second) : (a.first < b.first);
        });

        auto prev = ranges.at(0);
        size_t size = ranges.size();

        RangesVector results;
        results.reserve(size);

        for (size_t i = 1; i < size; ++i) {
            auto &curr = ranges.at(i);

            if (segmentsOverlap(prev, curr)) {
                prev = unionOverlappingSegments(prev, curr);
            } else {
                results.emplace_back(prev);
                prev = curr;
            }
        }

        results.emplace_back(prev);

        return results;
    }
}
