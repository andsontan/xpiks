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

#ifndef COMMANDMANAGER_H
#define COMMANDMANAGER_H

#include <QList>
#include "commandbase.h"

namespace Encryption {
    class SecretsManager;
}

namespace UndoRedo {
    class UndoRedoManager;
}

namespace Models {
    class ArtworksRepository;
    class ArtItemsModel;
    class ArtItemInfo;
    class CombinedArtworksModel;
    class IptcProvider;
    class ArtworkUploader;
    class UploadInfoRepository;
    class WarningsManager;
    class UploadInfo;
    class ArtworkMetadata;
}

namespace Commands {
    class CommandManager
    {
    public:
        CommandManager() {}
        ~CommandManager() {}

    public:
        void InjectDependency(Models::ArtworksRepository *artworkRepository);
        void InjectDependency(Models::ArtItemsModel *artItemsModel);
        void InjectDependency(Models::CombinedArtworksModel *combinedArtworksModel);
        void InjectDependency(Models::IptcProvider *iptcProvider);
        void InjectDependency(Models::ArtworkUploader *artworkUploader);
        void InjectDependency(Models::UploadInfoRepository *uploadInfoRepository);
        void InjectDependency(Models::WarningsManager *warningsManager);
        void InjectDependency(Encryption::SecretsManager *secretsManager);
        void InjectDependency(UndoRedo::UndoRedoManager *undoRedoManager);

    public:
        CommandResult *processCommand(CommandBase *command) const;

    public:
        void connectEntitiesSignalsSlots() const;

    public:
        void recodePasswords(const QString &oldMasterPassword,
                                const QString &newMasterPassword,
                                const QList<Models::UploadInfo*> &uploadInfos) const;

        void combineArtworks(const QList<Models::ArtItemInfo*> &artworks) const;
        void setArtworksForIPTCProcessing(const QList<Models::ArtworkMetadata*> &artworks) const;
        void setArtworksForUpload(const QList<Models::ArtworkMetadata*> &artworks) const;

    public:
        // methods for getters
        Models::ArtworksRepository *getArtworkRepository() const { return m_ArtworkRepository; }
        Models::ArtItemsModel *getArtItemsModel() const { return m_ArtItemsModel; }
        const Encryption::SecretsManager *getSecretsManager() const { return m_SecretsManager; }
        const Models::UploadInfoRepository *getUploadInfoRepository() const { return m_UploadInfoRepository; }

    private:
        Models::ArtworksRepository *m_ArtworkRepository;
        Models::ArtItemsModel *m_ArtItemsModel;
        Models::CombinedArtworksModel *m_CombinedArtworksModel;
        Models::IptcProvider *m_IptcProvider;
        Models::ArtworkUploader *m_ArtworkUploader;
        Models::UploadInfoRepository *m_UploadInfoRepository;
        Models::WarningsManager *m_WarningsManager;
        Encryption::SecretsManager *m_SecretsManager;
        UndoRedo::UndoRedoManager *m_UndoRedoManager;
    };
}

#endif // COMMANDMANAGER_H
