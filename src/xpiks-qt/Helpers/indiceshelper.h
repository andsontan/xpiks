#ifndef INDICESHELPER_H
#define INDICESHELPER_H

#include <QList>
#include <QPair>

namespace Helpers
{
    void indicesToRanges(const QList<int> &indices, QList<QPair<int, int> > &pairs) {
        if (indices.empty()) { return; }

        int currentStart = indices[0];
        int indicesCount = indices.count();

        for (int i = 1; i < indicesCount; ++i) {
            if (indices[i] - indices[i - 1] > 1) {
                pairs.append(qMakePair(currentStart, indices[i - 1]));
                currentStart = indices[i];
            }
        }

        pairs.append(qMakePair(currentStart, indices[indicesCount - 1]));
    }
}

#endif // INDICESHELPER_H
