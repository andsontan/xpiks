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

#ifndef IPTCPROVIDER_H
#define IPTCPROVIDER_H

#include <QFutureWatcher>
#include <QPair>
#include "artworksprocessor.h"
#include "exportinfo.h"

namespace Models {
    class ArtworkMetadata;
    class ImportDataResult;
    class ExportInfo;
}

namespace Suggestion {
    class LocalLibrary;
}

typedef QPair<Models::ArtworkMetadata*, Models::ImportDataResult*> ImportPair;
typedef QPair<Models::ArtworkMetadata*, Models::ExportInfo*> ExportPair;

namespace Models {
    class IptcProvider : public ArtworksProcessor
    {
        Q_OBJECT
        Q_PROPERTY(bool mustSaveOriginal READ getMustSaveOriginal WRITE setMustSaveOriginal NOTIFY mustSaveOriginalChanged)
        Q_PROPERTY(bool ignoreAutosave READ getIgnoreAutosave WRITE setIgnoreAutosave NOTIFY ignoreAutosaveChanged)

   public:
        IptcProvider();
        virtual ~IptcProvider() { delete m_MetadataWriter; delete m_MetadataReader; }

    public slots:
        void metadataImported(int);
        void metadataExported(int);
        void allFinishedReading();
        void allFinishedWriting();

    private:
        void metadataImportedHandler(ImportPair importPair);
        void metadataExportedHandler(ArtworkMetadata *metadata);

    public:
        void setLocalLibrary(Suggestion::LocalLibrary *localLibrary) { m_LocalLibrary = localLibrary; }

        bool getMustSaveOriginal() const { return m_ExportInfo.getMustSaveOriginal(); }
        void setMustSaveOriginal(bool value) {
            if (value != m_ExportInfo.getMustSaveOriginal()) {
                m_ExportInfo.setMustSaveOriginal(value);
                emit mustSaveOriginalChanged();
            }
        }

        bool getIgnoreAutosave() const { return m_IgnoreAutosave; }
        void setIgnoreAutosave(bool value) {
            if (m_IgnoreAutosave != value) {
                m_IgnoreAutosave = value;
                emit ignoreAutosaveChanged();
            }
        }

    signals:
        void mustSaveOriginalChanged();
        void ignoreAutosaveChanged();

    public:
        Q_INVOKABLE void importMetadata() { doReadMetadata(getArtworkList()); }
        Q_INVOKABLE void exportMetadata() { doWriteMetadata(getArtworkList()); }

    private:
        void doReadMetadata(const QList<ArtworkMetadata*> &artworkList);
        void doWriteMetadata(const QList<ArtworkMetadata*> &artworkList);

    protected:
        void cancelProcessing();

    private:
        QFutureWatcher<QPair<Models::ArtworkMetadata*, Models::ImportDataResult*> > *m_MetadataReader;
        QFutureWatcher<QPair<ArtworkMetadata*, ExportInfo*> > *m_MetadataWriter;
        Suggestion::LocalLibrary *m_LocalLibrary;
        ExportInfo m_ExportInfo;
        bool m_IgnoreAutosave;
    };
}

#endif // IPTCPROVIDER_H
