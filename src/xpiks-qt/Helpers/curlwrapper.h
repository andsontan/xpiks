#ifndef CURLWRAPPER
#define CURLWRAPPER

#include <QStringList>
#include <QProcess>
#include <QString>
#include <QPair>
#include "externaltoolsprovider.h"
#include "../Models/ftpcredentials.h"
#include "../Models/artworkmetadata.h"

typedef QPair<Models::ArtworkMetadata*, Models::FtpCredentials*> UploadPair;

UploadPair uploadViaCurl(UploadPair pair) {
    Models::ArtworkMetadata *metadata = pair.first, *resultMetadata = NULL;
    Models::FtpCredentials *ftp = pair.second;

    const QString curlPath = Helpers::ExternalToolsProvider::getCurlPath();

    QStringList arguments;
    arguments << "-T" << metadata->getFilepath();
    arguments << ftp->getHost() << "--user" << QString("%1:%2").arg(ftp->getUsername(), ftp->getPassword());

    QProcess process;
    process.start(curlPath, arguments);
    // wait each for 10 minutes
    // TODO: move to config
    if (process.waitForFinished(100000) &&
            process.exitStatus() == QProcess::NormalExit &&
            process.exitCode() == 0) {
        resultMetadata = metadata;
    }

    return qMakePair(resultMetadata, ftp);
}

#endif // CURLWRAPPER

