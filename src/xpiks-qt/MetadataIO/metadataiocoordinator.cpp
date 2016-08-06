/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2016 Taras Kushnir <kushnirTV@gmail.com>
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

#include "metadataiocoordinator.h"
#include <QHash>
#include <QFutureWatcher>
#include <QtConcurrent>
#include <QFileInfo>
#include <QProcess>
#include <QImageReader>
#include "metadatareadingworker.h"
#include "metadatawritingworker.h"
#include "backupsaverservice.h"
#include "../Models/artworkmetadata.h"
#include "../Models/settingsmodel.h"
#include "../Commands/commandmanager.h"
#include "../Suggestion/locallibrary.h"
#include "saverworkerjobitem.h"
#include "../Models/settingsmodel.h"
#include "../Common/defines.h"
#include "../Models/imageartwork.h"
#include "readingorchestrator.h"
#include "writingorchestrator.h"

namespace MetadataIO {
    bool tryGetExiftoolVersion(const QString &path, QString &version) {
        QProcess process;
        process.start(path, QStringList() << "-ver");
        bool success = process.waitForFinished(3000);

        int exitCode = process.exitCode();
        QProcess::ExitStatus exitStatus = process.exitStatus();

        success = success &&
                (exitCode == 0) &&
                (exitStatus == QProcess::NormalExit);

        if (success) {
            version = QString::fromUtf8(process.readAll());
        }

        return success;
    }

    MetadataIOCoordinator::MetadataIOCoordinator():
        Common::BaseEntity(),
        m_ReadingWorker(NULL),
        m_WritingWorker(NULL),
        m_ProcessingItemsCount(0),
        m_IsImportInProgress(false),
        m_CanProcessResults(false),
        m_IgnoreBackupsAtImport(false),
        m_HasErrors(false),
        m_ExiftoolNotFound(false)
    {
        m_ExiftoolDiscoveryFuture = new QFutureWatcher<void>(this);
        QObject::connect(m_ExiftoolDiscoveryFuture, SIGNAL(finished()),
                         this, SLOT(exiftoolDiscoveryFinished()));

        LOG_INFO << "Supported image formats:" << QImageReader::supportedImageFormats();
    }

    void MetadataIOCoordinator::readingWorkerFinished(bool success) {
        LOG_INFO << "Success:" << success;
        setHasErrors(!success);

        const QVector<Models::ArtworkMetadata*> &itemsToRead = m_ReadingWorker->getItemsToRead();
        m_CommandManager->generatePreviews(itemsToRead);

        if (m_CanProcessResults) {
            readingFinishedHandler(m_IgnoreBackupsAtImport);
        } else {
            LOG_INFO << "Can't process results. Waiting for user interaction...";
        }

        m_IsImportInProgress = false;
    }

    void MetadataIOCoordinator::writingWorkerFinished(bool success) {
        LOG_INFO << success;
        setHasErrors(!success);
        const QVector<Models::ArtworkMetadata*> &artworksToWrite = m_WritingWorker->getItemsToWrite();
        m_CommandManager->addToLibrary(artworksToWrite);
        emit metadataWritingFinished();
    }

    void MetadataIOCoordinator::exiftoolDiscoveryFinished() {
        if (!m_ExiftoolNotFound && !m_RecommendedExiftoolPath.isEmpty()) {
            LOG_DEBUG << "Recommended exiftool path is" << m_RecommendedExiftoolPath;

            Models::SettingsModel *settingsModel = m_CommandManager->getSettingsModel();
            QString existingExiftoolPath = settingsModel->getExifToolPath();

            if (existingExiftoolPath != m_RecommendedExiftoolPath) {
                LOG_INFO << "Setting exiftool path to recommended";
                settingsModel->setExifToolPath(m_RecommendedExiftoolPath);
                settingsModel->saveExiftool();
            }
        }
    }

    void MetadataIOCoordinator::readMetadataExifTool(const QVector<Models::ArtworkMetadata *> &artworksToRead,
                                             const QVector<QPair<int, int> > &rangesToUpdate) {
        MetadataReadingWorker *readingWorker = new MetadataReadingWorker(artworksToRead,
                                                    m_CommandManager->getSettingsModel(),
                                                    rangesToUpdate);
        QThread *thread = new QThread();
        readingWorker->moveToThread(thread);

        QObject::connect(thread, SIGNAL(started()), readingWorker, SLOT(process()));
        QObject::connect(readingWorker, SIGNAL(stopped()), thread, SLOT(quit()));

        QObject::connect(readingWorker, SIGNAL(stopped()), readingWorker, SLOT(deleteLater()));
        QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

        QObject::connect(readingWorker, SIGNAL(finished(bool)), this, SLOT(readingWorkerFinished(bool)));
        QObject::connect(this, SIGNAL(metadataReadingFinished()), readingWorker, SIGNAL(stopped()));
        QObject::connect(this, SIGNAL(discardReadingSignal()), readingWorker, SLOT(cancel()));

        initializeImport(artworksToRead.count());

        LOG_DEBUG << "Starting thread...";
        thread->start();

        m_ReadingWorker = readingWorker;
    }

#ifndef CORE_TESTS
    void MetadataIOCoordinator::readMetadataExiv2(const QVector<Models::ArtworkMetadata *> &artworksToRead,
                                                  const QVector<QPair<int, int> > &rangesToUpdate) {
        ReadingOrchestrator *readingOrchestrator = new ReadingOrchestrator(artworksToRead, rangesToUpdate);

        QObject::connect(readingOrchestrator, SIGNAL(allFinished(bool)), this, SLOT(readingWorkerFinished(bool)));
        QObject::connect(this, SIGNAL(metadataReadingFinished()), readingOrchestrator, SLOT(dismiss()));
        QObject::connect(this, SIGNAL(discardReadingSignal()), readingOrchestrator, SLOT(dismiss()));

        initializeImport(artworksToRead.count());
        m_ReadingWorker = readingOrchestrator;

        readingOrchestrator->startReading();
    }
#endif

    void MetadataIOCoordinator::writeMetadataExifTool(const QVector<Models::ArtworkMetadata *> &artworksToWrite, bool useBackups) {
        MetadataWritingWorker *writingWorker = new MetadataWritingWorker(artworksToWrite,
                                                    m_CommandManager->getSettingsModel(),
                                                    useBackups);
        QThread *thread = new QThread();
        writingWorker->moveToThread(thread);

        QObject::connect(thread, SIGNAL(started()), writingWorker, SLOT(process()));
        QObject::connect(writingWorker, SIGNAL(stopped()), thread, SLOT(quit()));

        QObject::connect(writingWorker, SIGNAL(stopped()), writingWorker, SLOT(deleteLater()));
        QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

        QObject::connect(writingWorker, SIGNAL(finished(bool)), this, SLOT(writingWorkerFinished(bool)));
        QObject::connect(this, SIGNAL(metadataWritingFinished()), writingWorker, SIGNAL(stopped()));
        setProcessingItemsCount(artworksToWrite.length());

        LOG_DEBUG << "Starting thread...";
        m_WritingWorker = writingWorker;
        thread->start();
    }

#ifndef CORE_TESTS
    void MetadataIOCoordinator::writeMetadataExiv2(const QVector<Models::ArtworkMetadata *> &artworksToWrite) {
        WritingOrchestrator *writingOrchestrator = new WritingOrchestrator(artworksToWrite);

        QObject::connect(writingOrchestrator, SIGNAL(allFinished(bool)), this, SLOT(writingWorkerFinished(bool)));
        QObject::connect(this, SIGNAL(metadataWritingFinished()), writingOrchestrator, SLOT(dismiss()));

        m_WritingWorker = writingOrchestrator;

        writingOrchestrator->startWriting();
    }
#endif

    void MetadataIOCoordinator::autoDiscoverExiftool() {
        Models::SettingsModel *settingsModel = m_CommandManager->getSettingsModel();
        QString existingExiftoolPath = settingsModel->getExifToolPath();
        m_ExiftoolDiscoveryFuture->setFuture(QtConcurrent::run(this,
                                                               &MetadataIOCoordinator::tryToLaunchExiftool,
                                                               existingExiftoolPath));
    }

    void MetadataIOCoordinator::discardReading() {
        emit discardReadingSignal();
        LOG_DEBUG << "Reading results discarded";
    }

    void MetadataIOCoordinator::continueReading(bool ignoreBackups) {
        m_CanProcessResults = true;

        LOG_DEBUG << "Is in progress:" << m_IsImportInProgress;

        if (!m_IsImportInProgress) {
            readingFinishedHandler(ignoreBackups);
        } else {
            m_IgnoreBackupsAtImport = ignoreBackups;
        }
    }

    void MetadataIOCoordinator::continueWithoutReading() {
        LOG_DEBUG << "Setting technical data";

        const QHash<QString, ImportDataResult> &importResult = m_ReadingWorker->getImportResult();
        const QVector<Models::ArtworkMetadata*> &itemsToRead = m_ReadingWorker->getItemsToRead();

        int size = itemsToRead.size();
        for (int i = 0; i < size; ++i) {
            Models::ArtworkMetadata *metadata = itemsToRead.at(i);
            const QString &filepath = metadata->getFilepath();

            if (importResult.contains(filepath)) {
                const ImportDataResult &importResultItem = importResult.value(filepath);

                Models::ImageArtwork *image = dynamic_cast<Models::ImageArtwork*>(metadata);
                if (image != NULL) {
                    image->setImageSize(importResultItem.ImageSize);
                    image->setDateTimeOriginal(importResultItem.DateTimeOriginal);
                }

                metadata->setFileSize(importResultItem.FileSize);
            }
        }
    }

    void MetadataIOCoordinator::initializeImport(int itemsCount) {
        m_CanProcessResults = false;
        m_IgnoreBackupsAtImport = false;
        m_IsImportInProgress = true;
        setProcessingItemsCount(itemsCount);
    }

    void MetadataIOCoordinator::readingFinishedHandler(bool ignoreBackups) {
        Q_ASSERT(m_CanProcessResults);
        m_CanProcessResults = false;

        const QHash<QString, ImportDataResult> &importResult = m_ReadingWorker->getImportResult();
        const QVector<Models::ArtworkMetadata*> &itemsToRead = m_ReadingWorker->getItemsToRead();

        LOG_DEBUG  << "Setting imported metadata...";
        int size = itemsToRead.size();
        for (int i = 0; i < size; ++i) {
            Models::ArtworkMetadata *metadata = itemsToRead.at(i);
            const QString &filepath = metadata->getFilepath();

            if (importResult.contains(filepath)) {
                const ImportDataResult &importResultItem = importResult.value(filepath);
                metadata->initialize(importResultItem.Title,
                                     importResultItem.Description,
                                     importResultItem.Keywords);

                Models::ImageArtwork *image = dynamic_cast<Models::ImageArtwork*>(metadata);
                if (image != NULL) {
                    image->setImageSize(importResultItem.ImageSize);
                    image->setDateTimeOriginal(importResultItem.DateTimeOriginal);
                }

                metadata->setFileSize(importResultItem.FileSize);
            }
        }

        afterImportHandler(itemsToRead, ignoreBackups);

        LOG_DEBUG << "Metadata import finished";
        emit metadataReadingFinished();
    }

    void MetadataIOCoordinator::afterImportHandler(const QVector<Models::ArtworkMetadata*> &itemsToRead, bool ignoreBackups) {
        Models::SettingsModel *settingsModel = m_CommandManager->getSettingsModel();
        const QHash<QString, ImportDataResult> &importResult = m_ReadingWorker->getImportResult();
        const QVector<QPair<int, int> > &rangesToUpdate = m_ReadingWorker->getRangesToUpdate();

        if (!ignoreBackups && settingsModel->getSaveBackups()) {
            LOG_DEBUG << "Restoring the backups...";
            int size = itemsToRead.size();
            for (int i = 0; i < size; ++i) {
                Models::ArtworkMetadata *metadata = itemsToRead.at(i);
                const QString &filepath = metadata->getFilepath();

                if (importResult.contains(filepath)) {
                    const ImportDataResult &importResultItem = importResult.value(filepath);
                    MetadataSavingCopy copy(importResultItem.BackupDict);
                    copy.saveToMetadata(metadata);
                }
            }
        } else {
            LOG_DEBUG << "Skipped restoring the backups";
        }

        if (!getHasErrors()) {
            m_CommandManager->addToLibrary(itemsToRead);
        }

        m_CommandManager->updateArtworks(rangesToUpdate);
        m_CommandManager->submitForSpellCheck(itemsToRead);
        m_CommandManager->submitForWarningsCheck(itemsToRead);
    }

    void MetadataIOCoordinator::tryToLaunchExiftool(const QString &settingsExiftoolPath) {
        LOG_DEBUG << "Default path is" << settingsExiftoolPath;
        // SHOULD BE UNDER DEFINE OS X
        QStringList possiblePaths;
        QString exiftoolPath;
        QString exiftoolVersion;

        possiblePaths << settingsExiftoolPath << "/usr/bin/exiftool" << "/usr/local/bin/exiftool";

        foreach (const QString &path, possiblePaths) {
            LOG_DEBUG << "Trying path" << path;
            QFileInfo fi(path);
            if (fi.exists()) {
                if (tryGetExiftoolVersion(path, exiftoolVersion)) {
                    LOG_INFO << "Exiftool version:" << exiftoolVersion;
                    exiftoolPath = path;
                    break;
                }
            }
        }

        if (exiftoolPath.isEmpty()) {
            const QString path = "exiftool";
            LOG_DEBUG << "Trying ordinary exiftool in PATH";
            if (tryGetExiftoolVersion(path, exiftoolVersion)) {
                LOG_INFO << "Exiftool version:" << exiftoolVersion;
                exiftoolPath = path;
            }
        }

        setExiftoolNotFound(exiftoolPath.isEmpty());
        m_RecommendedExiftoolPath = exiftoolPath;
    }
}

