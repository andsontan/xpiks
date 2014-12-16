#ifndef COMBINEDARTWORKSMODEL_H
#define COMBINEDARTWORKSMODEL_H

#include <QAbstractListModel>
#include <QStringList>
#include <QString>
#include <QList>

namespace Models {
    class CombinedArtworksModel : public QAbstractListModel
    {
        Q_OBJECT
        Q_PROPERTY(QStringList keywords READ getKeywords WRITE setKeywords NOTIFY keywordsChanged)
        Q_PROPERTY(QString description READ getDescription WRITE setDescription NOTIFY descriptionChanged)
    public:
        CombinedArtworksModel(QObject *parent = 0);
        ~CombinedArtworksModel() {}

    public:
        void initKeywords(const QStringList &ek) { std::copy(ek.begin(), ek.end(), std::back_inserter(m_CommonKeywords)); emit keywordsChanged(); }
        void initDescription(const QString &description) { setDescription(description); }
        void initImages(const QStringList &fp) {
            beginInsertRows(QModelIndex(), 0, fp.length() - 1);
            std::copy(fp.begin(), fp.end(), std::back_inserter(m_ImagesFilenames));
            endInsertRows();
        }

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
