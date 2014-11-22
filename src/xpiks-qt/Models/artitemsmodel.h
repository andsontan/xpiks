#include <QAbstractListModel>
#include "imagemetadata.h"

#ifndef ARTSITEMSMODEL_H
#define ARTSITEMSMODEL_H

namespace Models
{
    class ArtItemsModel : QAbstractListModel
    {
        Q_OBJECT
    public:
        ArtItemsModel();

    public:


    private:
        QList<ImageMetadata> m_ArtworkList;
    };
}

#endif // ARTSITEMSMODEL_H
