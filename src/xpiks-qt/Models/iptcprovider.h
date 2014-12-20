#ifndef IPTCPROVIDER_H
#define IPTCPROVIDER_H

#include <QFutureWatcher>
#include "artworksprocessor.h"
#include "artworkmetadata.h"

namespace Models {
    class IptcProvider : public ArtworksProcessor
    {
        Q_OBJECT
   public:
        IptcProvider();
        ~IptcProvider() { delete m_MetadataWriter; delete m_MetadataReader; }

    public slots:
        void metadataImported(int);
        void metadataExported(int);
        void allFinished();

    private:
        void metadataImportedHandler(ArtworkMetadata *metadata);
        void metadataExportedHandler(ArtworkMetadata *metadata);

    public:
        Q_INVOKABLE void importMetadata() { doReadMetadata(getArtworkList()); }
        Q_INVOKABLE void exportMetadata() { doWriteMetadata(getArtworkList()); }

    private:
        void doReadMetadata(const QList<ArtworkMetadata*> &artworkList);
        void doWriteMetadata(const QList<ArtworkMetadata*> &artworkList);

    private:
        QFutureWatcher<ArtworkMetadata*> *m_MetadataReader;
        QFutureWatcher<ArtworkMetadata*> *m_MetadataWriter;
    };
}

#endif // IPTCPROVIDER_H
