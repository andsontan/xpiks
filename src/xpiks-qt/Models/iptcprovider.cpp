#include "iptcprovider.h"
#include <QtConcurrent>
#include <QDebug>
#include "../Helpers/patcher.h"

namespace Models {
    IptcProvider::IptcProvider():
        m_ArtworksPatching(0),
        m_ProcessedArtworksCount(0),
        m_ArtworksCount(0),
        m_IsInProgress(false),
        m_IsError(false)
    {
        m_ArtworksPatching = new QFutureWatcher<ArtworkMetadata*>(this);
        connect(m_ArtworksPatching, SIGNAL(resultReadyAt(int)), SLOT(artworkPatched(int)));
        connect(m_ArtworksPatching, SIGNAL(finished()), SLOT(allFinished()));
    }

    void IptcProvider::artworkPatched(int index) {
        qDebug() << "Result ready at " << index;
        ArtworkMetadata *metadata = m_ArtworksPatching->resultAt(index);
        artworkPatched(metadata);
    }

    void IptcProvider::allFinished() {
        setInProgress(false);
    }

    void IptcProvider::artworkPatched(ArtworkMetadata *metadata)
    {
        incProgress();

        // TODO: handle bad results
        if (NULL != metadata) {
            qDebug() << metadata->getImageFileName();
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

    void IptcProvider::doPatchArtworks(const QList<ArtworkMetadata *> &artworkList) {
        int artworksCount = artworkList.length();
        if (artworksCount == 0) {
            return;
        }

        m_ArtworksCount = artworksCount;
        m_ProcessedArtworksCount = 0;
        setInProgress(true);

        ArtworkMetadata *firstMetadata = artworkList.first();
        if (!patchArtwork(firstMetadata)) {
            setIsError(true);
            incProgress();
            allFinished();
            return;
        }
        else {
            artworkPatched(firstMetadata);
        }

        m_ArtworksPatching->setFuture(QtConcurrent::mapped(artworkList.begin() + 1, artworkList.end(), patchArtwork));
    }
}
