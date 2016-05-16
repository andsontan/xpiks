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

#ifndef EXIV2WRITINGWORKER_H
#define EXIV2WRITINGWORKER_H

#include <QObject>
#include <QVector>

namespace Models {
    class ArtworkMetadata;
}

namespace MetadataIO {
    class Exiv2WritingWorker : public QObject
    {
        Q_OBJECT
    public:
        explicit Exiv2WritingWorker(int index, QVector<Models::ArtworkMetadata *> itemsToWrite, QObject *parent = 0);

    public:
        int getWorkerIndex() const { return m_WorkerIndex; }
        void dismiss() { emit stopped(); }

    public slots:
        void process();
        void cancel();

    signals:
        void stopped();
        void finished(bool anyError);

    private:
        void writeMetadata(Models::ArtworkMetadata *artwork);

    private:
        QVector<Models::ArtworkMetadata*> m_ItemsToWrite;
        int m_WorkerIndex;
        volatile bool m_Stopped;
    };
}

#endif // EXIV2WRITINGWORKER_H
