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

#include <QFutureWatcher>
#include "artworksprocessor.h"
#include "uploadinfo.h"

namespace Models {
    class ArtworkUploader : public ArtworksProcessor
    {
        Q_OBJECT
    public:
         ArtworkUploader();
         ~ArtworkUploader() { delete m_ArtworksUploader; }

    public:
         Q_PROPERTY(QString host READ getHost WRITE setHost NOTIFY hostChanged)
         Q_PROPERTY(QString username READ getUsername WRITE setUsername NOTIFY usernameChanged)
         Q_PROPERTY(QString password READ getPassword WRITE setPassword NOTIFY passwordChanged)
         Q_PROPERTY(bool includeEPS READ getIncludeEPS WRITE setIncludeEPS NOTIFY includeEPSChanged)

    signals:
         void hostChanged(QString);
         void usernameChanged(QString);
         void passwordChanged(QString);
         void includeEPSChanged(bool);

    public:
         const QString &getHost() const { return m_UploadInfo.getHost(); }
         const QString &getUsername() const { return m_UploadInfo.getUsername(); }
         const QString &getPassword() const { return m_UploadInfo.getPassword(); }
         bool getIncludeEPS() const { return m_UploadInfo.getIncludeEPS(); }

         void setHost(const QString &value) { m_UploadInfo.setHost(value); }
         void setUsername(const QString &value) { m_UploadInfo.setUsername(value); }
         void setPassword(const QString &value) { m_UploadInfo.setPassword(value); }
         void setIncludeEPS(bool value) { m_UploadInfo.setIncludeEPS(value); }

     public slots:
         void artworkUploaded(int);
         void allFinished();

     private:
         void artworkUploadedHandler(ArtworkMetadata *metadata);

     public:
         Q_INVOKABLE void uploadArtworks() { doUploadArtworks(getArtworkList()); }

     private:
         void doUploadArtworks(const QList<ArtworkMetadata*> &artworkList);

    protected:
        void cancelProcessing();

     private:
         QFutureWatcher<QPair<Models::ArtworkMetadata*, Models::UploadInfo*> > *m_ArtworksUploader;
         UploadInfo m_UploadInfo;
    };
}

#endif // ARTWORKUPLOADER_H
