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
