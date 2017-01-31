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

#ifndef METADATAREADINGWORKER_H
#define METADATAREADINGWORKER_H

#include <QObject>
#include <QVector>
#include <QHash>
#include <QString>
#include <QProcess>
#include <QStringList>
#include <QByteArray>
#include <QHash>
#include <QSize>
#include "importdataresult.h"
#include "imetadatareader.h"

namespace Models {
    class ArtworkMetadata;
    class SettingsModel;
}

namespace MetadataIO {
    class BackupSaverService;

    class MetadataReadingWorker : public QObject, public IMetadataReader
    {
        Q_OBJECT
    public:
        explicit MetadataReadingWorker(const QVector<Models::ArtworkMetadata *> &itemsToRead,
                                       Models::SettingsModel *settingsModel, const QVector<QPair<int, int> > &rangesToUpdate);
        virtual ~MetadataReadingWorker();

    signals:
        void stopped();
        void finished(bool success);

    public slots:
        void process();
        void cancel();

    private slots:
        void innerProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

    public:
        virtual const QHash<QString, ImportDataResult> &getImportResult() const override { return m_ImportResult; }
        virtual const QVector<Models::ArtworkMetadata *> &getItemsToRead() const override { return m_ItemsToRead; }
        virtual const QVector<QPair<int, int> > &getRangesToUpdate() const override { return m_RangesToUpdate; }

    private:
        void initWorker();
        QStringList createArgumentsList();
        void parseExiftoolOutput(const QByteArray &output);
        void readBackupsAndSizes(bool exiftoolSuccess);
        void readSizes();

    private:
        QVector<Models::ArtworkMetadata *> m_ItemsToRead;
        QHash<QString, ImportDataResult> m_ImportResult;
        QProcess *m_ExiftoolProcess;
        QVector<QPair<int, int> > m_RangesToUpdate;
        Models::SettingsModel *m_SettingsModel;
    };
}

#endif // METADATAREADINGWORKER_H
