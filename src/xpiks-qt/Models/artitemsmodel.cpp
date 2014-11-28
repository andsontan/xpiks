#include <QFileDialog>
#include "artitemsmodel.h"

namespace Models {
    ArtItemsModel::ArtItemsModel(QObject *parent) {
    }

    void ArtItemsModel::setDirectoryList(const QStringList &copy) {
        if (m_DirectoryList != copy) {
            m_DirectoryList = copy;
            emit directoryListChanged();
        }
    }

    void ArtItemsModel::removeDirectory(int index)
    {
        const QString &directory = m_DirectoryList.at(index);
        m_DirectorySet.remove(directory);
        m_DirectoryList.removeAt(index);
        emit directoryListChanged();
    }

    int ArtItemsModel::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return m_ArtworkList.count();
    }

    QVariant ArtItemsModel::data(const QModelIndex &index, int role) const {
        if (index.row() < 0 || index.row() >= m_ArtworkList.count())
            return QVariant();

        const ArtworkMetadata &metadata = m_ArtworkList[index.row()];
        switch (role) {
        case ImageDescriptionRole:
            return metadata.GetImageDescription();
        case ImageFilenameRole:
            return metadata.GetImageFileName();
        case KeywordsRole:
            return QVariant::fromValue(metadata.GetKeywords());
        default:
            return QVariant();
        }
    }

    void ArtItemsModel::addDirectoryButtonClicked()
    {
        QFileDialog dialog;
        dialog.setFileMode(QFileDialog::Directory);
        dialog.setOption(QFileDialog::ShowDirsOnly);

        QString filename = dialog.getExistingDirectory();
        addDirectory(filename);
    }

    void ArtItemsModel::addDirectory(const QString &directory)
    {
        if (!directory.isEmpty() && !m_DirectorySet.contains(directory))
        {
            m_DirectorySet.insert(directory);
            m_DirectoryList.append(directory);
            emit directoryListChanged();
        }
    }

    QHash<int, QByteArray> ArtItemsModel::roleNames() const {
        QHash<int, QByteArray> roles;
        roles[ImageDescriptionRole] = "description";
        roles[ImageFilenameRole] = "filename";
        roles[KeywordsRole] = "keywords";
        return roles;
    }
}
