#include "artworksdirectories.h"
#include <QFileInfo>

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
