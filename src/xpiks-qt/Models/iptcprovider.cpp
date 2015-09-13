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
#include "../Helpers/tempmetadatadb.h"
#include "../Helpers/exiftoolwrapper.h"
#include "artworkmetadata.h"

namespace Models {
    IptcProvider::IptcProvider():
        ArtworksProcessor(),
        m_IgnoreAutosave(false)
    {
        m_MetadataWriter = new QFutureWatcher<ExportPair>(this);
        connect(m_MetadataWriter, SIGNAL(resultReadyAt(int)), SLOT(metadataExported(int)));
        connect(m_MetadataWriter, SIGNAL(finished()), SLOT(allFinished()));

        m_MetadataReader = new QFutureWatcher<ImportPair>(this);
        connect(m_MetadataReader, SIGNAL(resultReadyAt(int)), SLOT(metadataImported(int)));
        connect(m_MetadataReader, SIGNAL(finished()), SLOT(allFinished()));
    }

    void IptcProvider::metadataImported(int index)
    {
        qDebug() << "Metadata imported at " << index;
        ImportPair importPair = m_MetadataReader->resultAt(index);
        metadataImportedHandler(importPair);
    }

    void IptcProvider::metadataExported(int index) {
        qDebug() << "Metadata exported at " << index;
        ExportPair pair = m_MetadataWriter->resultAt(index);
        ArtworkMetadata *metadata = pair.first;
        metadataExportedHandler(metadata);
    }

    void IptcProvider::allFinished() {
        endProcessing();
        qDebug() << "Metadata processing finished (with Error = " << getIsError() << ")";
    }

    void IptcProvider::metadataImportedHandler(ImportPair importPair)
    {
        ArtworkMetadata *metadata = importPair.first;
        Models::ImportDataResult *importData = importPair.second;

        if (metadata != NULL && importData != NULL) {
            metadata->initialize(importData->Title, importData->Description, importData->Keywords);

            if (!m_IgnoreAutosave &&
                    (importData->Description.isEmpty() || importData->Keywords.isEmpty())) {
                Helpers::TempMetadataDb(metadata).load();
            }
        }

        incProgress();

        if (NULL != metadata) {
            qDebug() << metadata->getFilepath();
        } else {
            setIsError(true);
        }

        if (importData != NULL) {
            delete importData;
        }
    }

    void IptcProvider::metadataExportedHandler(ArtworkMetadata *metadata)
    {
        incProgress();

        if (NULL != metadata) {
            qDebug() << metadata->getFilepath();
        } else {
            setIsError(true);
        }
    }

    void IptcProvider::doReadMetadata(const QList<ArtworkMetadata *> &artworkList)
    {
        int artworksCount = artworkList.length();
        if (artworksCount == 0) {
            return;
        }

        QList<ImportPair> pairs;
        foreach(ArtworkMetadata *metadata, artworkList) {
            pairs.append(qMakePair(metadata, new Models::ImportDataResult()));
        }

        beginProcessing();

        ImportPair firstPair = pairs.takeFirst();
        if (!(readArtworkMetadata(firstPair).first)) {
            endAfterFirstError();
            return;
        }
        else {
            metadataImportedHandler(firstPair);
        }

        if (pairs.length() > 0) {
            restrictMaxThreads();
            m_MetadataReader->setFuture(QtConcurrent::mapped(pairs, readArtworkMetadata));
        }
        else {
            endProcessing();
        }
    }

    void IptcProvider::doWriteMetadata(const QList<ArtworkMetadata *> &artworkList) {
        int artworksCount = artworkList.length();
        if (artworksCount == 0) {
            return;
        }

        beginProcessing();

        QList<ExportPair> pairs;
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
        m_MetadataReader->cancel();
    }
}
