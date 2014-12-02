#include <QFileDialog>
#include <QStringList>
#include <QDirIterator>
#include "artitemsmodel.h"

namespace Models {
    ArtItemsModel::ArtItemsModel(QObject *parent) {
    }

    ArtItemsModel::~ArtItemsModel() {
        int count = m_ArtworkList.count();
        for (int i = 0; i < count; ++i) {
            ArtworkMetadata *metadata = m_ArtworkList[i];
            delete metadata;
        }

        m_ArtworkList.clear();
    }

    void ArtItemsModel::setArtwork(const QStringList &copy) {
        if (m_ArtworksDirectories != copy) {
            m_ArtworksDirectories = copy;
            emit artworksDirectoriesChanged();
        }
    }

    void ArtItemsModel::removeArtworksDirectory(int index)
    {
        const QString &directory = m_ArtworksDirectories.at(index);
        m_ArtworksDirectoriesSet.remove(directory);
        m_ArtworksDirectories.removeAt(index);
        emit artworksDirectoriesChanged();
    }

    int ArtItemsModel::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return m_ArtworkList.count();
    }

    QVariant ArtItemsModel::data(const QModelIndex &index, int role) const {
        if (index.row() < 0 || index.row() >= m_ArtworkList.count())
            return QVariant();

        ArtworkMetadata *metadata = m_ArtworkList[index.row()];
        switch (role) {
        case ImageDescriptionRole:
            return metadata->GetImageDescription();
        case ImageFilenameRole:
            return metadata->GetImageFileName();
        case KeywordsRole:
            return QVariant::fromValue(metadata->GetKeywords());
        default:
            return QVariant();
        }
    }

    void ArtItemsModel::addDirectoryButtonClicked()
    {
        QFileDialog dialog;
        dialog.setFileMode(QFileDialog::Directory);
        dialog.setOption(QFileDialog::ShowDirsOnly);

        QString directory = dialog.getExistingDirectory();
        addDirectory(directory);
    }

    void ArtItemsModel::addFilesButtonClicked()
    {
        QFileDialog dialog;

        QStringList filenames = dialog.getOpenFileNames();
        addFiles(filenames);
    }

    void ArtItemsModel::addDirectory(const QString &directory)
    {
        if (!directory.isEmpty() && !m_ArtworksDirectoriesSet.contains(directory))
        {
            m_ArtworksDirectoriesSet.insert(directory);
            m_ArtworksDirectories.append(directory);
            emit artworksDirectoriesChanged();
        }
    }

    void ArtItemsModel::addFiles(const QStringList &filenames)
    {
        beginInsertRows(QModelIndex(), rowCount(), rowCount());

        int count = filenames.count();
        for (int i = 0; i < count; ++i) {
            const QString &filename = filenames[i];
            if (!m_ArtworksDirectoriesSet.contains(filename))
            {
                // TODO: grab keywords here
                ArtworkMetadata *metadata = new ArtworkMetadata("my description", filenames[i], "test1,test2");
                m_ArtworkList.append(metadata);
                m_ArtworksDirectoriesSet.insert(filename);

                // TODO: create tree-like structure here
                m_Artworks(filename);
            }
        }

        endInsertRows();
    }

    QHash<int, QByteArray> ArtItemsModel::roleNames() const {
        QHash<int, QByteArray> roles;
        roles[ImageDescriptionRole] = "description";
        roles[ImageFilenameRole] = "filename";
        roles[KeywordsRole] = "keywords";
        return roles;
    }
}
