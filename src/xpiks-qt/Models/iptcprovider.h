#ifndef IPTCPROVIDER_H
#define IPTCPROVIDER_H

#include <QFutureWatcher>
#include <QObject>
#include "artworkmetadata.h"

namespace Models {
    class IptcProvider : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(bool inProgress READ getInProgress WRITE setInProgress NOTIFY inProgressChanged)
        Q_PROPERTY(bool isError READ getIsError WRITE setIsError NOTIFY isErrorChanged)
        Q_PROPERTY(int percent READ getPercent NOTIFY percentChanged)
    public:
        IptcProvider();
        ~IptcProvider() { delete m_MetadataWriter; delete m_MetadataReader; }

    public slots:
        void metadataImported(int);
        void metadataExported(int);
        void allFinished();

    private:
        void metadataImported(ArtworkMetadata *metadata);
        void metadataExported(ArtworkMetadata *metadata);
        void incProgress() { m_ProcessedArtworksCount++; updateProgress(); }

    public:
        bool getInProgress() const { return m_IsInProgress; }
        void setInProgress(bool value) { m_IsInProgress = value; emit inProgressChanged(); }
        int getPercent() const { return m_ArtworksCount == 0 ? 0 : (m_ProcessedArtworksCount * 100 / m_ArtworksCount); }
        void updateProgress() { emit percentChanged(); }
        bool getIsError() const { return m_IsError; }
        void setIsError(bool value) { m_IsError = value; emit isErrorChanged(); }

    signals:
        void inProgressChanged();
        void isErrorChanged();
        void percentChanged();

    public:
        void setArtworks(const QList<ArtworkMetadata*> &artworkList) { resetArtworks(); addArtworks(artworkList); }
        void addArtworks(const QList<ArtworkMetadata*> &artworkList) { m_ArtworkList.append(artworkList); }
        void resetArtworks() { m_ArtworkList.clear(); }

    public:
        Q_INVOKABLE void importMetadata() { doReadMetadata(m_ArtworkList); }
        Q_INVOKABLE void exportMetadata() { doWriteMetadata(m_ArtworkList); }
        Q_INVOKABLE void resetModel();

    private:
        void doReadMetadata(const QList<ArtworkMetadata*> &artworkList);
        void doWriteMetadata(const QList<ArtworkMetadata*> &artworkList);

    private:
        QList<ArtworkMetadata*> m_ArtworkList;
        QFutureWatcher<ArtworkMetadata*> *m_MetadataReader;
        QFutureWatcher<ArtworkMetadata*> *m_MetadataWriter;
        volatile int m_ProcessedArtworksCount;
        volatile int m_ArtworksCount;
        bool m_IsInProgress;
        bool m_IsError;
    };
}

#endif // IPTCPROVIDER_H
