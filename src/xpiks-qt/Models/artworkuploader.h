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

#ifndef ARTWORKUPLOADER_H
#define ARTWORKUPLOADER_H

#include <QAbstractListModel>
#include <QStringList>
#include <QFutureWatcher>
#include "artworksprocessor.h"
#include "uploadinfo.h"
#include "uploadinforepository.h"
#include "../Helpers/uploaditem.h"
#include "../Encryption/secretsmanager.h"

namespace Models {
    class ArtworkUploader : public ArtworksProcessor
    {
        Q_OBJECT
    public:
         ArtworkUploader();
         ~ArtworkUploader() { delete m_ArtworksUploader; }

         void setUploadInfoRepository(UploadInfoRepository *infoRepository) { Q_ASSERT(infoRepository != NULL); m_InfoRepository = infoRepository; }
         void setSecretsManager(Encryption::SecretsManager *secretsManager) { Q_ASSERT(secretsManager != NULL); m_SecretsManager = secretsManager; }

    public:
         Q_PROPERTY(bool includeEPS READ getIncludeEPS WRITE setIncludeEPS NOTIFY includeEPSChanged)

    signals:
         void includeEPSChanged(bool);

    public:
         bool getIncludeEPS() const { return m_IncludeEPS; }
         void setIncludeEPS(bool value) { m_IncludeEPS = value; }

     public slots:
         void artworkUploaded(int);
         void allFinished();

     private:
         void artworkUploadedHandler(Helpers::UploadItem *item);

     public:
         Q_INVOKABLE void uploadArtworks() { doUploadArtworks(getArtworkList()); }

     private:
         void doUploadArtworks(const QList<ArtworkMetadata*> &artworkList);
         QStringList *getAllFilepathes() const;

    protected:
        void cancelProcessing();

     private:
         QFutureWatcher<Helpers::UploadItem> *m_ArtworksUploader;
         UploadInfoRepository *m_InfoRepository;
         Encryption::SecretsManager *m_SecretsManager;
         QStringList *m_ActiveUploads;
         bool m_IncludeEPS;
    };
}

#endif // ARTWORKUPLOADER_H
