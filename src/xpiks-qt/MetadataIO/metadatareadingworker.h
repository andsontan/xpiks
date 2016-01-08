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

namespace Models {
    class ArtworkMetadata;
    class SettingsModel;
}

namespace MetadataIO {
    class BackupSaverService;

    struct ImportDataResult {
        QString FilePath;
        QString Title;
        QString Description;
        QStringList Keywords;
        QSize Size;
    };

    class MetadataReadingWorker : public QObject
    {
        Q_OBJECT
    public:
        explicit MetadataReadingWorker(const QVector<Models::ArtworkMetadata *> &itemsToRead,
                                       Models::SettingsModel *settingsModel,
                                       bool ignoreBackups);
        virtual ~MetadataReadingWorker();

    signals:
        void stopped();
        void finished(bool success);

    public slots:
        void process();
        //void cancel();

    private slots:
        void innerProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

    public:
        const QHash<QString, ImportDataResult> &getImportResult() const { return m_ImportResult; }
        const QVector<Models::ArtworkMetadata *> &getArtworksToImport() const { return m_ItemsToRead; }
        bool getIgnoreBackups() const { return m_IgnoreBackups; }
        void shutdown() { emit stopped(); }

    private:
        void initWorker();
        QStringList createArgumentsList();
        void parseExiftoolOutput(const QByteArray &output);

    private:
        QVector<Models::ArtworkMetadata *> m_ItemsToRead;
        QHash<QString, ImportDataResult> m_ImportResult;
        QProcess *m_ExiftoolProcess;
        Models::SettingsModel *m_SettingsModel;
        volatile bool m_IgnoreBackups;
    };
}

#endif // METADATAREADINGWORKER_H
