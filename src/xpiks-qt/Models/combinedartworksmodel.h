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
        Q_PROPERTY(QString title READ getTitle WRITE setTitle NOTIFY titleChanged)
    public:
        CombinedArtworksModel(QObject *parent = 0) :
            AbstractListModel(parent)
        {}

        ~CombinedArtworksModel() { qDeleteAll(m_ArtworksList); }

    public:
        void initArtworks(const QList<ArtItemInfo*> &artworks);

    private:
        void initKeywords(const QStringList &ek) { m_CommonKeywords.clear(); std::copy(ek.begin(), ek.end(), std::back_inserter(m_CommonKeywords)); emit keywordsChanged(); }
        void initDescription(const QString &description) { setDescription(description); }
        void initTitle(const QString &title) { setTitle(title); }

    public:
        void recombineArtworks();

    public:
        Q_INVOKABLE void resetModelData();

    public:
        const QStringList &getKeywords() const { return m_CommonKeywords; }
        void setKeywords(const QStringList& keywords) { m_CommonKeywords = keywords; emit keywordsChanged(); }

    public:
        const QString &getDescription() const { return m_ArtworkDescription; }
        void setDescription(const QString &description) { m_ArtworkDescription = description; emit descriptionChanged(); }
        const QString &getTitle() const { return m_ArtworkTitle; }
        void setTitle(const QString &title) { m_ArtworkTitle = title; emit titleChanged(); }

    signals:
        void keywordsChanged();
        void descriptionChanged();
        void titleChanged();

    public:
        Q_INVOKABLE void removeKeywordAt(int keywordIndex);
        Q_INVOKABLE void removeLastKeyword() { removeKeywordAt(m_CommonKeywords.length() - 1); }
        Q_INVOKABLE void appendKeyword(const QString &keyword);
        Q_INVOKABLE void selectArtwork(int index);
        Q_INVOKABLE void deselectArtwork(int index);
        Q_INVOKABLE void removeSelectedArtworks();
        Q_INVOKABLE int getSelectedArtworksCount() const;
        Q_INVOKABLE int getArtworksCount() const { return m_ArtworksList.length(); }
        Q_INVOKABLE void saveSetKeywords();
        Q_INVOKABLE void saveAddKeywords();

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
        QString m_ArtworkDescription;
        QString m_ArtworkTitle;
    };
}

#endif // COMBINEDARTWORKSMODEL_H
