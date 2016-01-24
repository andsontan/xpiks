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

#ifndef METADATAWRITINGWORKER_H
#define METADATAWRITINGWORKER_H

#include <QObject>
#include <QVector>
#include <QProcess>

namespace Models {
    class ArtworkMetadata;
    class SettingsModel;
}

namespace MetadataIO {
    class MetadataWritingWorker : public QObject
    {
        Q_OBJECT
    public:
        explicit MetadataWritingWorker(const QVector<Models::ArtworkMetadata*> &itemsToWrite,
                                       Models::SettingsModel *settingsModel,
                                       bool useBackups);
        virtual ~MetadataWritingWorker();

    signals:
        void stopped();
        void finished(bool success);

    public slots:
        void process();
        //void cancel();

    private slots:
        void innerProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

    private:
        void initWorker();
        QStringList createArgumentsList(const QString &jsonFilePath);

    private:
        QVector<Models::ArtworkMetadata*> m_ItemsToWrite;
        QProcess *m_ExiftoolProcess;
        Models::SettingsModel *m_SettingsModel;
        bool m_UseBackups;
    };
}

#endif // METADATAWRITINGWORKER_H
