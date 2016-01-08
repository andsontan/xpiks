/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2015 Taras Kushnir <kushnirTV@gmail.com>
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
#include <QThread>
#include <QDebug>
#include <QHash>
#include "metadatareadingworker.h"
#include "metadatawritingworker.h"
#include "backupsaverservice.h"
#include "../Models/artworkmetadata.h"
#include "../Models/settingsmodel.h"
#include "../Commands/commandmanager.h"
#include "../Suggestion/locallibrary.h"
#include "saverworkerjobitem.h"

namespace MetadataIO {
    MetadataIOCoordinator::MetadataIOCoordinator():
        Common::BaseEntity(),
        m_ReadingWorker(NULL),
        m_WritingWorker(NULL),
        m_ProcessingItemsCount(0),
        m_IsImportInProgress(false),
        m_CanProcessResults(false),
        m_IgnoreBackupsAtImport(false)
    {
    }

    void MetadataIOCoordinator::readingWorkerFinished(bool success) {
        qDebug() << "Metadata reading finished with status" << success;

        if (m_CanProcessResults) {
            readingFinishedHandler(m_IgnoreBackupsAtImport);
        }

        setHasErrors(!success);
        m_IsImportInProgress = false;
    }

    void MetadataIOCoordinator::writingWorkerFinished(bool success) {
        setHasErrors(!success);
        emit metadataWritingFinished();
    }

    void MetadataIOCoordinator::readMetadata(const QVector<Models::ArtworkMetadata *> &artworksToRead) {
        m_ReadingWorker = new MetadataReadingWorker(artworksToRead,
                                                    m_CommandManager->getSettingsModel());
        QThread *thread = new QThread();
        m_ReadingWorker->moveToThread(thread);

        QObject::connect(thread, SIGNAL(started()), m_ReadingWorker, SLOT(process()));
        QObject::connect(m_ReadingWorker, SIGNAL(stopped()), thread, SLOT(quit()));

        QObject::connect(m_ReadingWorker, SIGNAL(stopped()), m_ReadingWorker, SLOT(deleteLater()));
        QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

        QObject::connect(m_ReadingWorker, SIGNAL(finished(bool)), this, SLOT(readingWorkerFinished(bool)));
        QObject::connect(this, SIGNAL(metadataReadingFinished()), m_ReadingWorker, SIGNAL(stopped()));
        QObject::connect(this, SIGNAL(discardReadingSignal()), m_ReadingWorker, SLOT(cancel()));

        m_CanProcessResults = false;
        m_IgnoreBackupsAtImport = false;
        m_IsImportInProgress = true;
        setProcessingItemsCount(artworksToRead.length());

        qDebug() << "Starting metadata reading thread";
        thread->start();
    }

    void MetadataIOCoordinator::writeMetadata(const QVector<Models::ArtworkMetadata *> &artworksToWrite, bool useBackups) {
        m_WritingWorker = new MetadataWritingWorker(artworksToWrite,
                                                    m_CommandManager->getSettingsModel(),
                                                    useBackups);
        QThread *thread = new QThread();
        m_WritingWorker->moveToThread(thread);

        QObject::connect(thread, SIGNAL(started()), m_WritingWorker, SLOT(process()));
        QObject::connect(m_WritingWorker, SIGNAL(stopped()), thread, SLOT(quit()));

        QObject::connect(m_WritingWorker, SIGNAL(stopped()), m_WritingWorker, SLOT(deleteLater()));
        QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

        QObject::connect(m_WritingWorker, SIGNAL(finished(bool)), this, SLOT(writingWorkerFinished(bool)));
        QObject::connect(this, SIGNAL(metadataWritingFinished()), m_WritingWorker, SIGNAL(stopped()));
        setProcessingItemsCount(artworksToWrite.length());

        qDebug() << "Starting metadata writing thread";
        thread->start();
    }

    void MetadataIOCoordinator::discardReading() {
        emit discardReadingSignal();
        qDebug() << "Reading results discarded";
    }

    void MetadataIOCoordinator::readMetadata(bool ignoreBackups) {
        m_CanProcessResults = true;

        if (!m_IsImportInProgress) {
            readingFinishedHandler(ignoreBackups);
        } else {
            m_IgnoreBackupsAtImport = ignoreBackups;
        }
    }

    void MetadataIOCoordinator::readingFinishedHandler(bool ignoreBackups) {
        Q_ASSERT(m_CanProcessResults);
        m_CanProcessResults = false;

        const QHash<QString, ImportDataResult> &importResult = m_ReadingWorker->getImportResult();
        const QVector<Models::ArtworkMetadata*> &itemsToRead = m_ReadingWorker->getArtworksToImport();

        qDebug() << "Setting imported metadata...";
        int size = itemsToRead.size();
        for (int i = 0; i < size; ++i) {
            Models::ArtworkMetadata *metadata = itemsToRead.at(i);
            const QString &filepath = metadata->getFilepath();

            if (importResult.contains(filepath)) {
                const ImportDataResult &importResultItem = importResult.value(filepath);
                metadata->initialize(importResultItem.Title,
                                     importResultItem.Description,
                                     importResultItem.Keywords);
            }
        }

        afterImportHandler(itemsToRead, ignoreBackups);

        qDebug() << "Metadata import finished";
        emit metadataReadingFinished();
    }

    void MetadataIOCoordinator::afterImportHandler(const QVector<Models::ArtworkMetadata*> &itemsToRead, bool ignoreBackups) {
        Models::SettingsModel *settingsModel = m_CommandManager->getSettingsModel();
        const QHash<QString, ImportDataResult> &importResult = m_ReadingWorker->getImportResult();

        if (!ignoreBackups && settingsModel->getSaveBackups()) {
            qDebug() << "Restoring the backups...";
            int size = itemsToRead.size();
            for (int i = 0; i < size; ++i) {
                Models::ArtworkMetadata *metadata = itemsToRead.at(i);
                const QString &filepath = metadata->getFilepath();

                if (importResult.contains(filepath)) {
                    const ImportDataResult &importResultItem = importResult.value(filepath);
                    MetadataSavingCopy copy(metadata, importResultItem.BackupDict);
                    copy.saveToMetadata();
                }
            }
        } else {
            qDebug() << "Skipped restoring the backups";
        }

        m_CommandManager->addToLibrary(itemsToRead);
        m_CommandManager->saveLocalLibraryAsync();
        m_CommandManager->submitForSpellCheck(itemsToRead);
    }
}

