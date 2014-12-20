#include "iptcprovider.h"
#include <QtConcurrent>
#include <QDebug>
#include "../Helpers/exiftoolwrapper.h"

namespace Models {
    IptcProvider::IptcProvider():
        m_MetadataWriter(0),
        m_ProcessedArtworksCount(0),
        m_ArtworksCount(0),
        m_IsInProgress(false),
        m_IsError(false)
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
        metadataImported(metadata);
    }

    void IptcProvider::metadataExported(int index) {
        qDebug() << "Metadata exported at " << index;
        ArtworkMetadata *metadata = m_MetadataWriter->resultAt(index);
        metadataExported(metadata);
    }

    void IptcProvider::allFinished() {
        setInProgress(false);
    }

    void IptcProvider::metadataImported(ArtworkMetadata *metadata)
    {
        incProgress();

        // TODO: handle bad results
        if (NULL != metadata) {
            qDebug() << metadata->getArtworkFilepath();
        }
    }

    void IptcProvider::metadataExported(ArtworkMetadata *metadata)
    {
        incProgress();

        // TODO: handle bad results
        if (NULL != metadata) {
            qDebug() << metadata->getArtworkFilepath();
        }
    }

    void IptcProvider::resetModel()
    {
        setInProgress(false);
        setIsError(false);
        m_ArtworksCount = 0;
        m_ProcessedArtworksCount = 0;
        updateProgress();
    }

    void IptcProvider::doReadMetadata(const QList<ArtworkMetadata *> &artworkList)
    {
        int artworksCount = artworkList.length();
        if (artworksCount == 0) {
            return;
        }

        m_ArtworksCount = artworksCount;
        m_ProcessedArtworksCount = 0;
        setInProgress(true);

        ArtworkMetadata *firstMetadata = artworkList.first();
        if (!readArtworkMetadata(firstMetadata)) {
            setIsError(true);
            incProgress();
            allFinished();
            return;
        }
        else {
            metadataImported(firstMetadata);
        }

        m_MetadataWriter->setFuture(QtConcurrent::mapped(artworkList.begin() + 1, artworkList.end(), readArtworkMetadata));
    }

    void IptcProvider::doWriteMetadata(const QList<ArtworkMetadata *> &artworkList) {
        int artworksCount = artworkList.length();
        if (artworksCount == 0) {
            return;
        }

        m_ArtworksCount = artworksCount;
        m_ProcessedArtworksCount = 0;
        setInProgress(true);

        ArtworkMetadata *firstMetadata = artworkList.first();
        if (!writeArtworkMetadata(firstMetadata)) {
            setIsError(true);
            incProgress();
            allFinished();
            return;
        }
        else {
            metadataExported(firstMetadata);
        }

        m_MetadataWriter->setFuture(QtConcurrent::mapped(artworkList.begin() + 1, artworkList.end(), writeArtworkMetadata));
    }
}
