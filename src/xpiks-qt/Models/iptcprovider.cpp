/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014 Taras Kushnir <kushnirTV@gmail.com>
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

namespace Models {
    IptcProvider::IptcProvider():
        ArtworksProcessor()
    {
        m_MetadataWriter = new QFutureWatcher<ExportPair>(this);
        connect(m_MetadataWriter, SIGNAL(resultReadyAt(int)), SLOT(metadataExported(int)));
        connect(m_MetadataWriter, SIGNAL(finished()), SLOT(allFinished()));

        m_MetadataReader = new QFutureWatcher<ArtworkMetadata*>(this);
        connect(m_MetadataReader, SIGNAL(resultReadyAt(int)), SLOT(metadataImported(int)));
        connect(m_MetadataReader, SIGNAL(finished()), SLOT(allFinished()));
    }

    void IptcProvider::metadataImported(int index)
    {
        qDebug() << "Metadata exported at " << index;
        ArtworkMetadata *metadata = m_MetadataReader->resultAt(index);
        metadataImportedHandler(metadata);
    }

    void IptcProvider::metadataExported(int index) {
        qDebug() << "Metadata exported at " << index;
        ExportPair pair = m_MetadataWriter->resultAt(index);
        ArtworkMetadata *metadata = pair.first;
        metadataExportedHandler(metadata);
    }

    void IptcProvider::allFinished() {
        endProcessing();
    }

    void IptcProvider::metadataImportedHandler(ArtworkMetadata *metadata)
    {
        incProgress();

        // TODO: handle bad results
        if (NULL != metadata) {
            qDebug() << metadata->getFilepath();
        }
    }

    void IptcProvider::metadataExportedHandler(ArtworkMetadata *metadata)
    {
        incProgress();

        // TODO: handle bad results
        if (NULL != metadata) {
            qDebug() << metadata->getFilepath();
        }
    }

    void IptcProvider::doReadMetadata(const QList<ArtworkMetadata *> &artworkList)
    {
        int artworksCount = artworkList.length();
        if (artworksCount == 0) {
            return;
        }

        beginProcessing();

        ArtworkMetadata *firstMetadata = artworkList.first();
        if (!readArtworkMetadata(firstMetadata)) {
            setIsError(true);
            incProgress();
            allFinished();
            return;
        }
        else {
            metadataImportedHandler(firstMetadata);
        }

        m_MetadataReader->setFuture(QtConcurrent::mapped(artworkList.begin() + 1, artworkList.end(), readArtworkMetadata));
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

        m_MetadataWriter->setFuture(QtConcurrent::mapped(pairs, writeArtworkMetadata));
    }

    void IptcProvider::cancelProcessing()
    {
        m_MetadataWriter->cancel();
        m_MetadataReader->cancel();
    }
}
