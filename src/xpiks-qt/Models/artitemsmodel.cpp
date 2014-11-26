#include <QFileDialog>
#include "artitemsmodel.h"

namespace Models {
    ArtItemsModel::ArtItemsModel(QObject *parent) {
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

        QString filename = dialog.getOpenFileName();
    }

    void ArtItemsModel::addDirectory(const QString &directory)
    {
        // TODO: implement this
    }

    QHash<int, QByteArray> ArtItemsModel::roleNames() const {
        QHash<int, QByteArray> roles;
        roles[ImageDescriptionRole] = "description";
        roles[ImageFilenameRole] = "filename";
        roles[KeywordsRole] = "keywords";
        return roles;
    }
}
