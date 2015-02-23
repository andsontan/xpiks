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
        ArtworksProcessor(),
        m_IncludeEPS(false)
    {
        m_ArtworksUploader = new QFutureWatcher<Helpers::UploadItem>(this);
        connect(m_ArtworksUploader, SIGNAL(resultReadyAt(int)), SLOT(artworkUploaded(int)));
        connect(m_ArtworksUploader, SIGNAL(finished()), SLOT(allFinished()));

        m_TestingCredentialWatcher = new QFutureWatcher<bool>(this);
        connect(m_TestingCredentialWatcher, SIGNAL(finished()), SLOT(credentialsTestingFinished()));
    }

    void ArtworkUploader::artworkUploaded(int index)
    {
        qDebug() << "Artwork uploaded at " << index;
        Helpers::UploadItem item = m_ArtworksUploader->resultAt(index);
        artworkUploadedHandler(&item);
    }

    void ArtworkUploader::allFinished()
    {
        endProcessing();
        delete m_ActiveUploads;
    }

    void ArtworkUploader::credentialsTestingFinished()
    {
        bool result = m_TestingCredentialWatcher->result();
        emit credentialsChecked(result);
    }

    void ArtworkUploader::artworkUploadedHandler(Helpers::UploadItem *item)
    {
        incProgress();

        // TODO: handle bad results
        if (item->m_Success) {
            qDebug() << item->m_UploadInfo->getHost();
        }
        else {
            setIsError(true);
        }
    }

    void ArtworkUploader::checkCredentials(const QString &host, const QString &username, const QString &password) const
    {
        m_TestingCredentialWatcher->setFuture(QtConcurrent::run(isConnectionValid, host, username, password));
    }

    void ArtworkUploader::doUploadArtworks(const QList<ArtworkMetadata *> &artworkList)
    {
        int artworksCount = artworkList.length();
        if (artworksCount == 0) {
            return;
        }

        beginProcessing();
        QList<Helpers::UploadItem> uploadItems;

        Q_ASSERT(m_InfoRepository != NULL);
        Q_ASSERT(m_SecretsManager != NULL);
        const QList<UploadInfo *> &infos = m_InfoRepository->getUploadInfos();
        m_ActiveUploads = getAllFilepathes();

        foreach (UploadInfo *info, infos) {
            if (info->getIsSelected()) {
                uploadItems.append(Helpers::UploadItem(info, m_ActiveUploads, m_SecretsManager));
            }
        }

        qDebug() << "Uploading " << uploadItems.length() << " items...";

        if (uploadItems.length() > 0) {
            m_ArtworksUploader->setFuture(QtConcurrent::mapped(uploadItems, uploadViaCurl));
        }
        else {
            endProcessing();
        }
    }

    QStringList *ArtworkUploader::getAllFilepathes() const
    {
        QStringList *filesList = new QStringList();
        const QList<ArtworkMetadata*> &artworksList = this->getArtworkList();

        foreach(ArtworkMetadata *metadata, artworksList) {
            QString filepath = metadata->getFilepath();
            filesList->append(filepath);

            if (m_IncludeEPS) {
                QString epsFilepath = filepath.replace(QRegExp("(.*)[.]jpg", Qt::CaseInsensitive), "\\1.eps");
                if (QFileInfo(epsFilepath).exists()) {
                    filesList->append(epsFilepath);
                }
            }
        }

        return filesList;
    }

    void ArtworkUploader::cancelProcessing()
    {
        m_ArtworksUploader->cancel();
    }
}
