#ifndef COMBINEDARTWORKSMODEL_H
#define COMBINEDARTWORKSMODEL_H

#include <QAbstractListModel>
#include <QStringList>
#include <QString>
#include <QList>
#include "artiteminfo.h"
#include "abstractlistmodel.h"

namespace Models {
    class CombinedArtworksModel : public AbstractListModel
    {
        Q_OBJECT
        Q_PROPERTY(QStringList keywords READ getKeywords WRITE setKeywords NOTIFY keywordsChanged)
        Q_PROPERTY(QString description READ getDescription WRITE setDescription NOTIFY descriptionChanged)
    public:
        CombinedArtworksModel(QObject *parent = 0);
        ~CombinedArtworksModel() { qDeleteAll(m_ArtworksList); }

    public:
        void initArtworks(const QList<ArtItemInfo*> &fp) {
            beginInsertRows(QModelIndex(), 0, fp.length() - 1);
            std::copy(fp.begin(), fp.end(), std::back_inserter(m_ArtworksList));
            endInsertRows();
        }

    private:
        void initKeywords(const QStringList &ek) { m_CommonKeywords.clear(); std::copy(ek.begin(), ek.end(), std::back_inserter(m_CommonKeywords)); emit keywordsChanged(); }
        void initDescription(const QString &description) { setDescription(description); }

    public:
        void recombineArtworks();

    public:
        Q_INVOKABLE void resetModelData();

    public:
        const QStringList &getKeywords() const { return m_CommonKeywords; }
        void setKeywords(const QStringList& keywords) { m_CommonKeywords = keywords; emit keywordsChanged(); }

    public:
        const QString &getDescription() const { return m_ImageDescription; }
        void setDescription(const QString &description) { m_ImageDescription = description; emit descriptionChanged(); }

    signals:
        void keywordsChanged();
        void descriptionChanged();

    public:
        Q_INVOKABLE void removeKeywordAt(int keywordIndex);
        Q_INVOKABLE void removeLastKeyword() { removeKeywordAt(m_CommonKeywords.length() - 1); }
        Q_INVOKABLE void appendKeyword(const QString &keyword);
        Q_INVOKABLE void selectArtwork(int index);
        Q_INVOKABLE void removeSelectedArtworks();
        Q_INVOKABLE int getSelectedArtworksCount();

    public:
        enum CombinedArtworksModelRoles {
            PathRole = Qt::UserRole + 1,
            IsSelectedRole
        };

    public:
        int rowCount(const QModelIndex & parent = QModelIndex()) const;
        QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    protected:
        QHash<int, QByteArray> roleNames() const;

    protected:
        void removeInnerItem(int row);

    private:
        QList<ArtItemInfo*> m_ArtworksList;
        QStringList m_CommonKeywords;
        QString m_ImageDescription;
    };
}

#endif // COMBINEDARTWORKSMODEL_H
