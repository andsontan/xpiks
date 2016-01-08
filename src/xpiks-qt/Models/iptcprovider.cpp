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

#include "iptcprovider.h"
#include <QtConcurrent>
#include <QDebug>
#include "../Helpers/exiftoolwrapper.h"
#include "artworkmetadata.h"
#include "settingsmodel.h"
#include "../Commands/commandmanager.h"
#include "../Suggestion/locallibrary.h"
#include "../Common/defines.h"

namespace Models {
    IptcProvider::IptcProvider():
        ArtworksProcessor(),
        m_IgnoreAutosave(false)
    {
        m_MetadataWriter = new QFutureWatcher<ExportPair>(this);
        connect(m_MetadataWriter, SIGNAL(resultReadyAt(int)), SLOT(metadataExported(int)));
        connect(m_MetadataWriter, SIGNAL(finished()), SLOT(allFinishedWriting()));
    }

    void IptcProvider::metadataExported(int index) {
        qInfo() << "Metadata exported at " << index;
        ExportPair pair = m_MetadataWriter->resultAt(index);
        ArtworkMetadata *metadata = pair.first;
        metadataExportedHandler(metadata);
    }

    void IptcProvider::allFinishedWriting() {
        endProcessing();
        qDebug() << "Metadata writing finished (with Error = " << getIsError() << ")";
    }

    void IptcProvider::metadataExportedHandler(ArtworkMetadata *metadata)
    {
        incProgress();

        if (NULL != metadata) {
            qDebug() << "Metadata saved" << metadata->getFilepath();
        } else {
            setIsError(true);
        }
    }

    void IptcProvider::cleanupLibrary() const {
        if (m_LocalLibrary != NULL) {
            m_LocalLibrary->cleanupLocalLibraryAsync();
        }
    }

    void IptcProvider::doReadMetadata(const QVector<ArtworkMetadata *> &artworkList)
    {
        m_CommandManager->readMetadata(artworkList, m_IgnoreAutosave);
        /*
        int artworksCount = artworkList.length();

        qDebug() << "Reading metadata from" << artworksCount << "item(s)";

        if (artworksCount == 0) {
            return;
        }

        QVector<ImportPair> pairs;
        pairs.reserve(artworkList.length());
        foreach(ArtworkMetadata *metadata, artworkList) {
            pairs.append(qMakePair(metadata, new Models::ImportDataResult()));
        }

        beginProcessing();

        if (pairs.length() > 0) {
            restrictMaxThreads();
            m_MetadataReader->setFuture(QtConcurrent::mapped(pairs, readArtworkMetadata));
        }
        else {
            endProcessing();
        }
        */
    }

    void IptcProvider::doWriteMetadata(const QVector<ArtworkMetadata *> &artworkList) {
        int artworksCount = artworkList.length();
        if (artworksCount == 0) {
            return;
        }

        m_LocalLibrary->saveLibraryAsync();

        beginProcessing();

        QVector<ExportPair> pairs;
        pairs.reserve(artworkList.length());
        foreach(ArtworkMetadata *metadata, artworkList) {
            pairs.append(qMakePair(metadata, &m_ExportInfo));
        }

        ExportPair firstPair = pairs.takeFirst();

        if (!writeArtworkMetadata(firstPair).first) {
            endAfterFirstError();
            return;
        }
        else {
            metadataExportedHandler(firstPair.first);
        }

        if (pairs.length() > 0) {
            restrictMaxThreads();
            m_MetadataWriter->setFuture(QtConcurrent::mapped(pairs, writeArtworkMetadata));
        }
        else {
            endProcessing();
        }
    }

    void IptcProvider::cancelProcessing()
    {
        m_MetadataWriter->cancel();
    }
}
