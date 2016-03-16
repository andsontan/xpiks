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

#ifndef ZIPARCHIVER_H
#define ZIPARCHIVER_H

#include <QFutureWatcher>
#include <QPair>
#include <QVector>
#include "artworksprocessor.h"

class QStringList;
class QString;

namespace Models {
    class ZipArchiver : public ArtworksProcessor
    {
        Q_OBJECT
    public:
        ZipArchiver();
        virtual ~ZipArchiver() { delete m_ArchiveCreator; }

    public:
        virtual int getItemsCount() const;

    public slots:
        void archiveCreated(int);
        void allFinished();

    public:
        Q_INVOKABLE void archiveArtworks();
        virtual void cancelProcessing() { /*BUMP*/ }

    private:
        void fillFilenamesHash(QHash<QString, QStringList> &hash);

    private:
        QFutureWatcher<QStringList> *m_ArchiveCreator;
    };
}

#endif // ZIPARCHIVER_H
