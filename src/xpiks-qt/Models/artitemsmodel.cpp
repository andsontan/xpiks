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

    void ArtItemsModel::setArtworkSources(const QStringList &copy) {
        if (m_ArtworksSources != copy) {
            m_ArtworksSources = copy;
            emit artworkSourcesChanged();
        }
    }

    void ArtItemsModel::removeArtworkSource(int index)
    {
        const QString &directory = m_ArtworksSources.at(index);
        m_ArtworksSourcesSet.remove(directory);
        m_ArtworksSources.removeAt(index);
        emit artworkSourcesChanged();
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
        if (!directory.isEmpty() && !m_ArtworksSourcesSet.contains(directory))
        {
            m_ArtworksSourcesSet.insert(directory);
            m_ArtworksSources.append(directory);
            emit artworkSourcesChanged();
        }
    }

    void ArtItemsModel::addFiles(const QStringList &filenames)
    {
        beginInsertRows(QModelIndex(), rowCount(), rowCount());

        int count = filenames.count();
        for (int i = 0; i < count; ++i) {
            const QString &filename = filenames[i];
            if (!m_ArtworksSourcesSet.contains(filename))
            {
                // TODO: grab keywords here
                ArtworkMetadata *metadata = new ArtworkMetadata("my description", filenames[i], "test1,test2");
                m_ArtworkList.append(metadata);
                m_ArtworksSourcesSet.insert(filename);

                // TODO: create tree-like structure here
                m_ArtworksSources.append(filename);
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
