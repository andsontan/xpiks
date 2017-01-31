/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
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

#ifndef EXIV2READINGWORKER_H
#define EXIV2READINGWORKER_H

#include <QObject>
#include <QVector>
#include <QHash>
#include "importdataresult.h"

namespace Models {
    class ArtworkMetadata;
}

namespace MetadataIO {
    class Exiv2ReadingWorker : public QObject
    {
        Q_OBJECT
    public:
        explicit Exiv2ReadingWorker(int index, QVector<Models::ArtworkMetadata *> itemsToRead, QObject *parent = 0);
        virtual ~Exiv2ReadingWorker();

    public:
        const QHash<QString, ImportDataResult> &getImportResult() const { return m_ImportResult; }
        int getWorkerIndex() const { return m_WorkerIndex; }

    public:
        void dismiss() { emit stopped(); }

    public slots:
        void process();
        void cancel();

    signals:
        void stopped();
        void finished(bool anyError);

    private:
        bool readMetadata(Models::ArtworkMetadata *artwork, ImportDataResult &importResult);

    private:
        QVector<Models::ArtworkMetadata *> m_ItemsToRead;
        QHash<QString, ImportDataResult> m_ImportResult;
        int m_WorkerIndex;
        volatile bool m_Stopped;
    };
}

#endif // EXIV2READINGWORKER_H
