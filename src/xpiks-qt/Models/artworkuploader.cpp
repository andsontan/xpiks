/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014 Taras Kushnir <kushnirTV@gmail.com>
 *
 * Xpiks is distributed under the GNU General Public License, version 3.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
        UploadPair pair = m_ArtworksUploader->resultAt(index);
        UploadInfo *info = pair.second;
        artworkUploadedHandler(info);
        delete pair.first;
    }

    void ArtworkUploader::allFinished()
    {
        endProcessing();
    }

    void ArtworkUploader::artworkUploadedHandler(UploadInfo *info)
    {
        incProgress();

        // TODO: handle bad results
        if (NULL != info) {
            qDebug() << info->getHost();
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

        QStringList *filesList = new QStringList();
        foreach(ArtworkMetadata *metadata, artworkList) {
            QString filepath = metadata->getFilepath();
            filesList->append(filepath);

            if (m_IncludeEPS) {
                QString epsFilepath = filepath.replace(QRegExp("(.*)[.]jpg", Qt::CaseInsensitive), "\\1.eps");
                qDebug() << epsFilepath;
                filesList->append(epsFilepath);
            }
        }

        QList<UploadPair> pairs;
        UploadInfo *info = m_InfoRepository->getUploadInfos().first();
        pairs.append(qMakePair(filesList, info));

        m_ArtworksUploader->setFuture(QtConcurrent::mapped(pairs, uploadViaCurl));
    }

    void ArtworkUploader::cancelProcessing()
    {
        m_ArtworksUploader->cancel();
    }
}
