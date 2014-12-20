#ifndef PATCHER
#define PATCHER

#include <QByteArray>
#include <QProcess>
#include <QRegExp>
#include <QString>
#include "externaltoolsprovider.h"
#include "../Models/artworkmetadata.h"

// returns NULL if patching wasn't successfull
Models::ArtworkMetadata *writeArtworkMetadata(Models::ArtworkMetadata *metadata) {
    const QString &author = metadata->getAuthor();
    const QString &title = metadata->getTitle();
    const QString &description = metadata->getDescription();
    const QString &keywords = metadata->getKeywordsString();

    QStringList arguments;
    arguments << QString("-EXIF:Artist=\"%1\"").arg(author);
    arguments << QString("-IPTC:By-line=\"%1\"").arg(author);
    arguments << QString("-XMP:Author=\"%1\"").arg(author);
    arguments << QString("-XMP:Creator=\"%1\"").arg(author);
    arguments << QString("-IPTC:ObjectName=\"%1\"").arg(title);
    arguments << QString("-XMP:Title=\"%1\"").arg(title);
    arguments << QString("-EXIF:ImageDescription=\"%1\"").arg(description);
    arguments << QString("-IPTC:Caption-Abstract=\"%1\"").arg(description);
    arguments << QString("-XMP:Caption=\"%1\"").arg(description);
    arguments << QString("-XMP:Description=\"%1\"").arg(description);
    arguments << QString("-IPTC:Keywords=\"%1\"").arg(keywords);
    arguments << QString("-XMP:Keywords=\"%1\"").arg(keywords);
    arguments << QString("-XMP:Subject=\"%1\"").arg(keywords);
    arguments << "-overwrite_original" << "-quiet" << "-ignoreMinorErrors";
    arguments << metadata->getArtworkFilepath();

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

void grabMetadata(const QStringList &items, Models::ArtworkMetadata *metadata);

Models::ArtworkMetadata *readArtworkMetadata(Models::ArtworkMetadata *metadata) {
    const QString exiftoolPath = Helpers::ExternalToolsProvider::getExifToolPath();

    QStringList arguments;
    arguments << "-s" << "-e" << "-n" << "-EXIF:all" << "-IPTC:all" << "-XMP:all";
    arguments << metadata->getArtworkFilepath();

    QProcess process;
    process.start(exiftoolPath, arguments);
    if (!process.waitForFinished()) {
        return NULL;
    }

    QByteArray stdoutByteArray = process.readAll();
    QString stdout(stdoutByteArray);
    QStringList items = stdout.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
    grabMetadata(items, metadata);

    return metadata;
}

void grabMetadata(const QStringList &items, Models::ArtworkMetadata *metadata) {
    bool authorSet = false, titleSet = false, descriptionSet = false, keywordsSet = false;

    QRegExp authorRegExp("^Artist\\s|^By-line\\s|^Creator\\s");
    QRegExp titleRegExp("^ObjectName\\s|^Title\\s");
    QRegExp descriptionRegExp("^ImageDescription\\s|^Caption-Abstract\\s|^Description\\s");
    QRegExp keywordsRegExp("^Keywords\\s|^Subject\\s");

    QString author, title, description, keywords;

    foreach (const QString &item, items) {
        QStringList parts = item.split(':', QString::SkipEmptyParts);
        if (parts.empty()) {
            continue;
        }

        const QString &first = parts.first();

        if (!authorSet && first.contains(authorRegExp)) {
            author = parts.at(1).trimmed();
            authorSet = true;
        }
        else if (!titleSet && first.contains(titleRegExp)) {
            title = parts.at(1).trimmed();
            titleSet = true;
        }
        else if (!descriptionSet && first.contains(descriptionRegExp)) {
            description = parts.at(1).trimmed();
            descriptionSet = true;
        }
        else if (!keywordsSet && first.contains(keywordsRegExp)) {
            keywords = parts.at(1).trimmed();
            keywordsSet = true;
        }
    }

    metadata->initialize(author, title, description, keywords);
}

#endif // PATCHER

