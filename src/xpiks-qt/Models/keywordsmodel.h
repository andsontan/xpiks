#ifndef KEYWORDSMODEL_H
#define KEYWORDSMODEL_H

#include <QAbstractListModel>
#include <QStringList>
#include <QString>
#include <QSet>

namespace Models {
    class KeywordsModel : public QAbstractListModel
    {
        Q_OBJECT
    public:
        KeywordsModel(QObject *parent = 0);

    public:
        void parseKeywords(const QString& rawKeywords);

    public:
        enum KeywordsRoles {
            KeywordRole = Qt::UserRole + 1
        };

    public:
        int rowCount(const QModelIndex & parent = QModelIndex()) const;
        QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    protected:
        QHash<int, QByteArray> roleNames() const;

    private:
        QSet<QString> m_KeywordsSet;
        QStringList m_KeywordsList;
    };
}

#endif // KEYWORDSMODEL_H
