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
#include "ftpcredentials.h"

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

    signals:
         void hostChanged(QString);
         void usernameChanged(QString);
         void passwordChanged(QString);

    public:
         const QString &getHost() const { return m_FtpCredentials.getHost(); }
         const QString &getUsername() const { return m_FtpCredentials.getUsername(); }
         const QString &getPassword() const { return m_FtpCredentials.getPassword(); }

         void setHost(const QString &value) { m_FtpCredentials.setHost(value); }
         void setUsername(const QString &value) { m_FtpCredentials.setUsername(value); }
         void setPassword(const QString &value) { m_FtpCredentials.setPassword(value); }

     public slots:
         void artworkUploaded(int);
         void allFinished();

     private:
         void artworkUploadedHandler(ArtworkMetadata *metadata);

     public:
         Q_INVOKABLE void uploadArtworks() { doUploadArtworks(getArtworkList()); }

     private:
         void doUploadArtworks(const QList<ArtworkMetadata*> &artworkList);

     private:
         QFutureWatcher<QPair<Models::ArtworkMetadata*, Models::FtpCredentials*> > *m_ArtworksUploader;
         FtpCredentials m_FtpCredentials;
    };
}

#endif // ARTWORKUPLOADER_H
