#ifndef ARTWORKSPROCESSOR_H
#define ARTWORKSPROCESSOR_H

#include <QObject>
#include "artworkmetadata.h"

namespace Models {
    class ArtworksProcessor : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(bool inProgress READ getInProgress WRITE setInProgress NOTIFY inProgressChanged)
        Q_PROPERTY(bool isError READ getIsError WRITE setIsError NOTIFY isErrorChanged)
        Q_PROPERTY(int percent READ getPercent NOTIFY percentChanged)

    public:
        ArtworksProcessor() :
            m_ProcessedArtworksCount(0),
            m_ArtworksCount(0),
            m_IsInProgress(false),
            m_IsError(false)
        {}

        ~ArtworksProcessor() {}

    protected:
        void incProgress() { m_ProcessedArtworksCount++; updateProgress(); }

    public:
        bool getInProgress() const { return m_IsInProgress; }
        void setInProgress(bool value) { m_IsInProgress = value; emit inProgressChanged(); }
        int getPercent() const { return m_ArtworksCount == 0 ? 0 : (m_ProcessedArtworksCount * 100 / m_ArtworksCount); }
        void updateProgress() { emit percentChanged(); }
        bool getIsError() const { return m_IsError; }
        void setIsError(bool value) { m_IsError = value; emit isErrorChanged(); }

    public:
        Q_INVOKABLE void resetModel();

    signals:
        void inProgressChanged();
        void isErrorChanged();
        void percentChanged();

    public:
        void setArtworks(const QList<ArtworkMetadata*> &artworkList) { resetArtworks(); addArtworks(artworkList); }
        void addArtworks(const QList<ArtworkMetadata*> &artworkList) { m_ArtworkList.append(artworkList); }
        void resetArtworks() { m_ArtworkList.clear(); }

    protected:
        const QList<ArtworkMetadata*> getArtworkList() const { return m_ArtworkList; }
        void beginProcessing();
        void endProcessing();
        void endAfterFirstError();

    private:
        QList<ArtworkMetadata*> m_ArtworkList;
        volatile int m_ProcessedArtworksCount;
        volatile int m_ArtworksCount;
        bool m_IsInProgress;
        bool m_IsError;
    };
}
#endif // ARTWORKSPROCESSOR_H
