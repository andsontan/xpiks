#ifndef INDICESHELPER_H
#define INDICESHELPER_H

#include <QList>
#include <QPair>

namespace Helpers
{
    void indicesToRanges(const QList<int> &indices, QList<QPair<int, int> > &pairs);
}

#endif // INDICESHELPER_H
