#ifndef ARTITEMSMODELMOCK_H
#define ARTITEMSMODELMOCK_H

#include <QVector>
#include <QPair>
#include "../xpiks-qt/Models/artitemsmodel.h"
#include "../xpiks-qt/Models/artworkmetadata.h"
#include "artworkmetadatamock.h"

namespace Mocks {
    class ArtItemsModelMock : public Models::ArtItemsModel
    {
    public:
        ArtItemsModelMock() {}

    public:
        virtual Models::ArtworkMetadata *createMetadata(const QString &filepath) {
            ArtworkMetadataMock *metadata = new ArtworkMetadataMock(filepath);
            metadata->initialize("Test title", "Test description",
                                 QStringList() << "keyword1" << "keyword2" << "keyword3");
            return metadata;
        }

        void removeAll() { this->removeArtworks(QVector<QPair<int, int> >() << qMakePair(0, rowCount() - 1)); }

        virtual void updateItemsAtIndices(const QVector<int> &indices) {  Q_UNUSED(indices); /* DO NOTHING */ }
        virtual void updateItemsInRanges(const QVector<QPair<int, int> > &ranges) { Q_UNUSED(ranges); /* DO NOTHING */ }
    };
}

#endif // ARTITEMSMODELMOCK_H
