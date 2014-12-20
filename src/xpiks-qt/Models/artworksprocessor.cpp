#include "artworksprocessor.h"

namespace Models {
    void ArtworksProcessor::resetModel()
    {
        setInProgress(false);
        setIsError(false);
        m_ArtworksCount = 0;
        m_ProcessedArtworksCount = 0;
        updateProgress();
    }

    void ArtworksProcessor::beginProcessing()
    {
        m_ArtworksCount = m_ArtworkList.length();
        m_ProcessedArtworksCount = 0;
        setInProgress(true);
    }

    void ArtworksProcessor::endProcessing()
    {
        setInProgress(false);
    }

    void ArtworksProcessor::endAfterFirstError()
    {
        setIsError(true);
        incProgress();
        endProcessing();
    }
}
