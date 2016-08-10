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

#ifndef ABSTRACTLISTMODEL
#define ABSTRACTLISTMODEL

#include <QAbstractListModel>

#include <QVector>
#include <QList>
#include "../Helpers/indiceshelper.h"
#include "../Common/defines.h"

namespace Common {
    class AbstractListModel : public QAbstractListModel {
        Q_OBJECT
    public:
        AbstractListModel(QObject *parent = 0) : QAbstractListModel(parent) {}
        virtual ~AbstractListModel() {}

    public:
        virtual void removeItemsAtIndices(const QVector<QPair<int, int> > &ranges) {
            int rangesLength = Helpers::getRangesLength(ranges);
            LOG_INFO << "Ranges length:" << rangesLength;
            doRemoveItemsAtIndices(ranges, rangesLength);
        }

    protected:
        void updateItemsInRanges(const QVector<QPair<int, int> > &ranges, const QVector<int> &roles) {
            int rangesCount = ranges.count();
            for (int i = 0; i < rangesCount; ++i) {
                int startRow = ranges[i].first;
                int endRow = ranges[i].second;

                QModelIndex topLeft = index(startRow);
                QModelIndex bottomRight = index(endRow);
                emit dataChanged(topLeft, bottomRight, roles);
            }
        }

        virtual void removeInnerItem(int row) = 0;

        virtual bool shouldRemoveInRanges(int rangesLength) const { return rangesLength > 50; }

        virtual void removeInnerItemRange(int startRow, int endRow) {
            int count = endRow - startRow + 1;
            for (int j = 0; j < count; ++j) { removeInnerItem(startRow); }
        }

        void doRemoveItemsAtIndices(const QVector<QPair<int, int> > &ranges, int rangesLength) {
            int removedCount = 0;
            int rangesCount = ranges.count();

            bool willResetModel = shouldRemoveInRanges(rangesLength);

            if (willResetModel) {
                beginResetModel();
            }

            for (int i = 0; i < rangesCount; ++i) {
                int startRow = ranges[i].first - removedCount;
                int endRow = ranges[i].second - removedCount;

                if (!willResetModel) {
                    beginRemoveRows(QModelIndex(), startRow, endRow);
                }

                removeInnerItemRange(startRow, endRow);
                removedCount += (endRow - startRow + 1);

                if (!willResetModel) {
                    endRemoveRows();
                }
            }

            if (willResetModel) {
                endResetModel();
            }
        }
    };
}
#endif // ABSTRACTLISTMODEL

