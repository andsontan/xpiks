#include <QAbstractListModel>
#include <QList>
#include "keywordsfile.h"

#ifndef KEYWORDSREPOSITORY_H
#define KEYWORDSREPOSITORY_H

namespace Models
{
    class KeywordsRepository : public QAbstractListModel
    {
    public:
        KeywordsRepository();

    private:
        QList<KeywordsFile> m_KeywordsSources;
    };
}

#endif // KEYWORDSREPOSITORY_H
