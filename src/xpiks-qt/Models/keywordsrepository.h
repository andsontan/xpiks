#include <QAbstractListModel>
#include <QList>

#ifndef KEYWORDSREPOSITORY_H
#define KEYWORDSREPOSITORY_H

namespace Models
{
    class KeywordsRepository : public QAbstractListModel
    {
    public:
        KeywordsRepository();

    private:
        QList<KeywordsSource> m_KeywordsSources;
    };
}

#endif // KEYWORDSREPOSITORY_H
