#include <QFileDialog>
#include <QStringList>
#include <QDirIterator>
#include "artitemsmodel.h"

namespace Models {

    ArtItemsModel::~ArtItemsModel() {
        int count = m_ArtworkList.count();
        for (int i = 0; i < count; ++i) {
            ArtworkMetadata *metadata = m_ArtworkList[i];
            delete metadata;
        }

        m_ArtworkList.clear();

        // being freed in gui
        //delete m_ArtworksDirectories;
    }

    void ArtItemsModel::removeArtworksDirectory(int index)
    {
        m_ArtworksRepository->removeDirectory(index);
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
        QDir dir(directory);

        dir.setFilter(QDir::NoDotAndDotDot | QDir::Files);
        dir.setNameFilters(QStringList() << "*.jpg" << "*.JPG");

        QStringList items = dir.entryList();
        for (int i = 0; i < items.size(); ++i) {
            items[i] = dir.filePath(items[i]);
        }

        if (items.count() > 0) {
            addFiles(items);
        }
    }

    void ArtItemsModel::addFiles(const QStringList &filenames)
    {
        int count = filenames.count();
        const int newFilesCount = m_ArtworksRepository->getNewFilesCount(filenames);

        if (newFilesCount > 0) {
            m_ArtworksRepository->beginAccountingFiles(filenames);
            beginInsertRows(QModelIndex(), rowCount(), rowCount() + newFilesCount - 1);
        }

        for (int i = 0; i < count; ++i) {
            const QString &filename = filenames[i];
            if (m_ArtworksRepository->accountFile(filename))
            {
                // TODO: grab keywords here
                ArtworkMetadata *metadata = new ArtworkMetadata("my description", filenames[i], "test1,test2");
                m_ArtworkList.append(metadata);
            }
        }

        if (newFilesCount) {
            endInsertRows();
            m_ArtworksRepository->endAccountingFiles();

            m_ArtworksRepository->updateCounts();
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
