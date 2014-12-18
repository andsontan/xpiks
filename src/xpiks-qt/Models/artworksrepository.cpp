#include "artworksrepository.h"
#include <QSet>
#include <QFileInfo>
#include <QRegExp>
#include "../Helpers/indiceshelper.h"

namespace Models {
    ArtworksRepository::ArtworksRepository(QObject *parent) {
    }

    void ArtworksRepository::updateCountsForExistingDirectories()
    {
        emit dataChanged(index(0), index(rowCount() - 1), QVector<int>() << UsedImagesCountRole);
    }

    void ArtworksRepository::cleanupEmptyDirectories()
    {
        int count = m_DirectoriesList.length();
        QList<int> indicesToRemove;
        for (int i = 0; i < count; ++i) {
            const QString &directory = m_DirectoriesList[i];
            if (m_DirectoriesHash[directory] == 0) {
                indicesToRemove.append(i);
            }
        }

        QList<QPair<int, int> > rangesToRemove;
        Helpers::indicesToRanges(indicesToRemove, rangesToRemove);
        removeItemsAtIndices(rangesToRemove);
    }

    bool ArtworksRepository::beginAccountingFiles(const QStringList &items)
    {
        int count = getNewDirectoriesCount(items);
        bool shouldAccountFiles = count > 0;
        if (shouldAccountFiles) {
            beginInsertRows(QModelIndex(), rowCount(), rowCount() + count - 1);
        }

        return shouldAccountFiles;
    }

    void ArtworksRepository::endAccountingFiles(bool filesWereAccounted)
    {
        if (filesWereAccounted) {
            endInsertRows();
        }
    }

    int ArtworksRepository::getNewDirectoriesCount(const QStringList &items) const
    {
        QSet<QString> filteredFiles;

        foreach (const QString &filepath, items) {
            if (!m_FilesSet.contains(filepath)) {
                filteredFiles.insert(filepath);
            }
        }

        QSet<QString> filteredDirectories;

        foreach (const QString &filepath, filteredFiles) {
            QFileInfo fi(filepath);
            if (fi.exists()) {
                filteredDirectories.insert(fi.absolutePath());
            }
        }

        int count = 0;
        foreach (const QString &directory, filteredDirectories) {
            if (!m_DirectoriesHash.contains(directory)) {
                count++;
            }
        }

        return count;
    }

    int ArtworksRepository::getNewFilesCount(const QStringList &items) const
    {
        int count = 0;
        QSet<QString> itemsSet = QSet<QString>::fromList(items);

        foreach (const QString &filepath, itemsSet) {
            if (!m_FilesSet.contains(filepath)) {
                count++;
            }
        }

        return count;
    }

    bool ArtworksRepository::accountFile(const QString &filepath) {
        bool wasModified = false;

        QFileInfo fi(filepath);

        if (fi.exists() && !m_FilesSet.contains(filepath)) {
            const QString absolutePath = fi.absolutePath();

            int occurances = 0;
            if (!m_DirectoriesHash.contains(absolutePath)) {
                m_DirectoriesList.append(absolutePath);
            }
            else {
                occurances = m_DirectoriesHash[absolutePath];
            }

            m_FilesSet.insert(filepath);
            m_DirectoriesHash[absolutePath] = occurances + 1;
            wasModified = true;
        }

        return wasModified;
    }

    void ArtworksRepository::removeFile(const QString &filepath) {
        QFileInfo fi(filepath);
        const QString absolutePath = fi.absolutePath();

        if (m_DirectoriesHash.contains(absolutePath)) {
            int occurances = m_DirectoriesHash[absolutePath] - 1;

            m_DirectoriesHash[absolutePath] = occurances;
        }

        m_FilesSet.remove(filepath);
    }

    int ArtworksRepository::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return m_DirectoriesList.count();
    }

    QVariant ArtworksRepository::data(const QModelIndex &index, int role) const {
        if (index.row() < 0 || index.row() >= m_DirectoriesList.count())
            return QVariant();

        const QString &directory = m_DirectoriesList.at(index.row());

        switch (role) {
        case PathRole:
            return QString(directory);
        case UsedImagesCountRole:
            return QVariant(m_DirectoriesHash[directory]);
        default:
            return QVariant();
        }
    }

    QHash<int, QByteArray> ArtworksRepository::roleNames() const {
        QHash<int, QByteArray> roles;
        roles[PathRole] = "path";
        roles[UsedImagesCountRole] = "usedimagescount";
        return roles;
    }
}
