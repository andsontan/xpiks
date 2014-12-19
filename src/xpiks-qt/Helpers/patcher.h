#ifndef PATCHER
#define PATCHER

#include <QProcess>
#include <QString>
#include "externaltoolsprovider.h"
#include "../Models/artworkmetadata.h"

// returns NULL if patching wasn't successfull
Models::ArtworkMetadata *patchArtwork(Models::ArtworkMetadata *metadata) {
    QStringList arguments;
    arguments << "-Headline=" + metadata->getImageDescription();
    arguments << "-Keywords=" + metadata->getKeywordsString();
    arguments << "-overwrite_original";
    arguments << metadata->getImageFileName();

    const QString exiftoolPath = Helpers::ExternalToolsProvider::getExifToolPath();

    QProcess process;
    process.start(exiftoolPath, arguments);
    if (!process.waitForFinished()) {
        return NULL;
    }

    Models::ArtworkMetadata *resultMetadata = NULL;
    if (process.exitStatus() == QProcess::NormalExit) {
        resultMetadata = metadata;
    }

    return resultMetadata;
}

#endif // PATCHER

