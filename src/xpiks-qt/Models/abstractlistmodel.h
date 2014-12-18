#ifndef ABSTRACTLISTMODEL
#define ABSTRACTLISTMODEL

#include <QAbstractListModel>
#include <QVector>
#include <QList>

namespace Models {
    class AbstractListModel : public QAbstractListModel {
    protected:
        void removeItemsAtIndices(const QList<QPair<int, int> > &ranges) {
            int removedCount = 0;
            int rangesCount = ranges.count();
            for (int i = 0; i < rangesCount; ++i) {
                int startRow = ranges[i].first - removedCount;
                int endRow = ranges[i].second - removedCount;

                beginRemoveRows(QModelIndex(), startRow, endRow);
                int count = endRow - startRow + 1;
                for (int j = 0; j < count; ++j) { removeInnerItem(startRow); }
                endRemoveRows();

                removedCount += (endRow - startRow + 1);
            }
        }

        void updateItemsAtIndices(const QList<QPair<int, int> > &ranges, QVector<int> roles) {
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
    };
}
#endif // ABSTRACTLISTMODEL

