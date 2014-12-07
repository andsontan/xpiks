#ifndef ARTWORKSDIRECTORIES_H
#define ARTWORKSDIRECTORIES_H

#include <QAbstractListModel>
#include <QStringList>
#include <QPair>
#include <QSet>

namespace Models {
    class ArtworksRepository : public QAbstractListModel {
        Q_OBJECT
    public:
        ArtworksRepository(QObject *parent = 0);
        ArtworksRepository(const ArtworksRepository &copy):
            m_DirectoriesList(copy.m_DirectoriesList), m_DirectoriesHash(copy.m_DirectoriesHash),
        m_FilesSet(copy.m_FilesSet) {}
        ~ArtworksRepository() {}

    public:
        enum ArtworksDirectoriesRoles {
            PathRole = Qt::UserRole + 1,
            UsedImagesCountRole
        };

    public:
        void updateCountsForExistingDirectories();

    public:
        bool beginAccountingFiles(const QStringList &items);
        void endAccountingFiles(bool filesWereAccounted);
    public:
        int getNewDirectoriesCount(const QStringList &items) const;
        int getNewFilesCount(const QStringList &items) const;

    public:
        bool accountFile(const QString &filepath);
        void removeFile(const QString &filepath);
        void removeDirectory(const QString &directory);
        void removeDirectory(int index);
        void eraseFile(const QString &filepath);

        const QString &getDirectory(int index) const { return m_DirectoriesList[index]; }

    public:
        int rowCount(const QModelIndex & parent = QModelIndex()) const;
        QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    protected:
        QHash<int, QByteArray> roleNames() const;
    private:
        QStringList m_DirectoriesList;
        QHash<QString, int> m_DirectoriesHash;
        QSet<QString> m_FilesSet;
    };
}

#endif // ARTWORKSDIRECTORIES_H
