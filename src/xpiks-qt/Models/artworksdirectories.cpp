#include "artworksdirectories.h"
#include <QFileInfo>
#include <QRegExp>

namespace Models {
    ArtworksDirectories::ArtworksDirectories(QObject *parent) {
    }

    void ArtworksDirectories::accountFile(const QString &filepath) {
        QFileInfo fi(filepath);

        if (fi.exists()) {
            const QString absolutePath = fi.absolutePath();

            beginInsertRows(QModelIndex(), rowCount(), rowCount());

            int occurances = 0;
            if (!m_DirectoriesHash.contains(absolutePath)) {
                m_DirectoriesList.append(absolutePath);
            }
            else {
                occurances = m_DirectoriesHash[absolutePath];
            }

            m_DirectoriesHash[absolutePath] = occurances + 1;

            endInsertRows();
        }
    }

    void ArtworksDirectories::removeFile(const QString &filepath) {
        QFileInfo fi(filepath);
        const QString absolutePath = fi.absolutePath();

        if (m_DirectoriesHash.contains(absolutePath)) {
            int occurances;
            occurances = m_DirectoriesHash[absolutePath] - 1;

            if (occurances > 0) {
                m_DirectoriesHash[absolutePath] = occurances;
            }
            else {
                removeDirectory(absolutePath);
            }
        }
    }

    void ArtworksDirectories::removeDirectory(const QString &directory)
    {
        if (m_DirectoriesHash.contains(directory)) {
            m_DirectoriesHash.remove(directory);
            int index = m_DirectoriesList.indexOf(QRegExp(directory));

            beginRemoveRows(QModelIndex(), index, index);
            m_DirectoriesList.removeAt(index);
            endRemoveRows();
        }
    }

    int ArtworksDirectories::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return m_DirectoriesList.count();
    }

    QVariant ArtworksDirectories::data(const QModelIndex &index, int role) const {
        if (index.row() < 0 || index.row() >= m_DirectoriesList.count())
            return QVariant();

        const QString &directory = m_DirectoriesList[index.row()];

        switch (role) {
        case PathRole:
            return directory;
        case UsedImagesCountRole:
            return m_DirectoriesHash[directory];
        default:
            return QVariant();
        }
    }

    QHash<int, QByteArray> ArtworksDirectories::roleNames() const {
        QHash<int, QByteArray> roles;
        roles[PathRole] = "path";
        roles[UsedImagesCountRole] = "usedimagescount";
        return roles;
    }
}
