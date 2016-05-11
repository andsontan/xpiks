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

#ifndef READINGORCHESTRATOR_H
#define READINGORCHESTRATOR_H

#include <QObject>
#include <QVector>
#include <QAtomicInt>
#include <QMutex>
#include "importdataresult.h"

namespace Models {
    class ArtworkMetadata;
}

namespace MetadataIO {
    class ReadingOrchestrator : public QObject
    {
        Q_OBJECT
    public:
        explicit ReadingOrchestrator(const QVector<Models::ArtworkMetadata *> &itemsToRead, QObject *parent = 0);

    public:
        const QHash<QString, ImportDataResult> &getImportResult() const { return m_ImportResult; }

    public:
        void startReading();

    signals:
        void allStarted();
        void allFinished(bool anyError);

    private slots:
        void onWorkerFinished(bool anyError);

    private:
        QVector<QVector<Models::ArtworkMetadata *> > m_ItemsToRead;
        QMutex m_ImportMutex;
        QHash<QString, ImportDataResult> m_ImportResult;
        int m_ThreadsCount;
        QAtomicInt m_FinishedCount;
        volatile bool m_AnyError;
    };
}

#endif // READINGORCHESTRATOR_H
