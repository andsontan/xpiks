#include "iptcprovider.h"
#include <QtConcurrent>
#include <QDebug>
#include "../Helpers/exiftoolwrapper.h"

namespace Models {
    IptcProvider::IptcProvider():
        ArtworksProcessor()
    {
        m_MetadataWriter = new QFutureWatcher<ArtworkMetadata*>(this);
        connect(m_MetadataWriter, SIGNAL(resultReadyAt(int)), SLOT(metadataExported(int)));
        connect(m_MetadataWriter, SIGNAL(finished()), SLOT(allFinished()));

        m_MetadataReader = new QFutureWatcher<ArtworkMetadata*>(this);
        connect(m_MetadataReader, SIGNAL(resultReadyAt(int)), SLOT(metadataImported(int)));
        connect(m_MetadataReader, SIGNAL(finished()), SLOT(allFinished()));
    }

    void IptcProvider::metadataImported(int index)
    {
        qDebug() << "Metadata exported at " << index;
        ArtworkMetadata *metadata = m_MetadataReader->resultAt(index);
        metadataImportedHandler(metadata);
    }

    void IptcProvider::metadataExported(int index) {
        qDebug() << "Metadata exported at " << index;
        ArtworkMetadata *metadata = m_MetadataWriter->resultAt(index);
        metadataExportedHandler(metadata);
    }

    void IptcProvider::allFinished() {
        endProcessing();
    }

    void IptcProvider::metadataImportedHandler(ArtworkMetadata *metadata)
    {
        incProgress();

        // TODO: handle bad results
        if (NULL != metadata) {
            qDebug() << metadata->getFilepath();
        }
    }

    void IptcProvider::metadataExportedHandler(ArtworkMetadata *metadata)
    {
        incProgress();

        // TODO: handle bad results
        if (NULL != metadata) {
            qDebug() << metadata->getFilepath();
        }
    }

    void IptcProvider::doReadMetadata(const QList<ArtworkMetadata *> &artworkList)
    {
        int artworksCount = artworkList.length();
        if (artworksCount == 0) {
            return;
        }

        beginProcessing();

        ArtworkMetadata *firstMetadata = artworkList.first();
        if (!readArtworkMetadata(firstMetadata)) {
            setIsError(true);
            incProgress();
            allFinished();
            return;
        }
        else {
            metadataImportedHandler(firstMetadata);
        }

        m_MetadataWriter->setFuture(QtConcurrent::mapped(artworkList.begin() + 1, artworkList.end(), readArtworkMetadata));
    }

    void IptcProvider::doWriteMetadata(const QList<ArtworkMetadata *> &artworkList) {
        int artworksCount = artworkList.length();
        if (artworksCount == 0) {
            return;
        }

        beginProcessing();

        ArtworkMetadata *firstMetadata = artworkList.first();
        if (!writeArtworkMetadata(firstMetadata)) {
            endAfterFirstError();
            return;
        }
        else {
            metadataExportedHandler(firstMetadata);
        }

        m_MetadataWriter->setFuture(QtConcurrent::mapped(artworkList.begin() + 1, artworkList.end(), writeArtworkMetadata));
    }
}
