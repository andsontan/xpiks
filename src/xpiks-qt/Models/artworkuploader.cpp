#include "artworkuploader.h"
#include <QtConcurrent>
#include <QDebug>
#include "../Helpers/curlwrapper.h"

namespace Models {
    ArtworkUploader::ArtworkUploader() :
        ArtworksProcessor()
    {
        m_ArtworksUploader = new QFutureWatcher<UploadPair>(this);
        connect(m_ArtworksUploader, SIGNAL(resultReadyAt(int)), SLOT(artworkUploaded(int)));
        connect(m_ArtworksUploader, SIGNAL(finished()), SLOT(allFinished()));
    }

    void ArtworkUploader::artworkUploaded(int index)
    {
        qDebug() << "Artwork uploaded at " << index;
        ArtworkMetadata *metadata = m_ArtworksUploader->resultAt(index).first;
        artworkUploadedHandler(metadata);
    }

    void ArtworkUploader::allFinished()
    {
        endProcessing();
    }

    void ArtworkUploader::artworkUploadedHandler(ArtworkMetadata *metadata)
    {
        incProgress();

        // TODO: handle bad results
        if (NULL != metadata) {
            qDebug() << metadata->getFilepath();
        }
        else {
            setIsError(true);
        }
    }

    void ArtworkUploader::doUploadArtworks(const QList<ArtworkMetadata *> &artworkList)
    {
        int artworksCount = artworkList.length();
        if (artworksCount == 0) {
            return;
        }

        beginProcessing();

        QList<UploadPair> pairs;
        foreach(ArtworkMetadata *metadata, artworkList) {
            pairs.append(qMakePair(metadata, &m_FtpCredentials));
        }

        m_ArtworksUploader->setFuture(QtConcurrent::mapped(pairs, uploadViaCurl));
    }
}
