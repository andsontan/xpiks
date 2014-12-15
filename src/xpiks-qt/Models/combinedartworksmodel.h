#ifndef COMBINEDARTWORKSMODEL_H
#define COMBINEDARTWORKSMODEL_H

#include <QAbstractListModel>
#include <QStringList>
#include <QString>
#include <QList>

namespace Models {
    class CombinedArtworksModel : QAbstractListModel
    {
        Q_OBJECT
    public:
        CombinedArtworksModel(QObject *parent = 0);
        ~CombinedArtworksModel() {}

    public:
        void initKeywords(const QStringList &ek) { std::copy(ek.begin(), ek.end(), std::back_inserter(m_CommonKeywords)); }
        void initDescription(const QString &description) { m_ImageDescription = description; }
        void initImages(const QStringList &fp) { std::copy(fp.begin(), fp.end(), std::back_inserter(m_ImagesFilenames)); }

    public:
        Q_INVOKABLE void setDescription(const QString &description);
        Q_INVOKABLE void removeKeywordAt(int keywordIndex);
        Q_INVOKABLE void removeLastKeyword() { removeKeywordAt(m_CommonKeywords.length() - 1); }
        Q_INVOKABLE void appendKeyword(const QString &keyword);

    public:
        enum CombinedArtworksModelRoles {
            PathRole = Qt::UserRole + 1
        };

    public:
        int rowCount(const QModelIndex & parent = QModelIndex()) const;
        QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    protected:
        QHash<int, QByteArray> roleNames() const;

    private:
        QStringList m_ImagesFilenames;
        QStringList m_CommonKeywords;
        QString m_ImageDescription;
    };
}

#endif // COMBINEDARTWORKSMODEL_H
