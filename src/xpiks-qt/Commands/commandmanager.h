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

#ifndef COMMANDMANAGER_H
#define COMMANDMANAGER_H

#include <QStringList>
#include <QList>
#include "../UndoRedo/historyitem.h"
#include "commandbase.h"
#include "../Conectivity/analyticsuserevent.h"

namespace Encryption {
    class SecretsManager;
}

namespace UndoRedo {
    class UndoRedoManager;
}

namespace Models {
    class ArtworksRepository;
    class ArtItemsModel;
    class FilteredArtItemsProxyModel;
    class ArtItemInfo;
    class CombinedArtworksModel;
    class IptcProvider;
    class ArtworkUploader;
    class UploadInfoRepository;
    class WarningsManager;
    class UploadInfo;
    class ArtworkMetadata;
    class ZipArchiver;
    class SettingsModel;
    class RecentDirectoriesModel;
    class LogsModel;
}

namespace Suggestion {
    class KeywordsSuggestor;
    class LocalLibrary;
}

namespace MetadataIO {
    class BackupSaverService;
    class MetadataIOCoordinator;
}

namespace Helpers {
    class UpdateService;
}

namespace SpellCheck {
    class SpellCheckerService;
    class SpellCheckSuggestionModel;
    class ISpellCheckable;
}

namespace Conectivity {
    class TelemetryService;
}

namespace Commands {
    class CommandManager
    {
    public:
        CommandManager():
            m_ArtworksRepository(NULL),
            m_ArtItemsModel(NULL),
            m_FilteredItemsModel(NULL),
            m_CombinedArtworksModel(NULL),
            m_IptcProvider(NULL),
            m_ArtworkUploader(NULL),
            m_UploadInfoRepository(NULL),
            m_WarningsManager(NULL),
            m_SecretsManager(NULL),
            m_UndoRedoManager(NULL),
            m_ZipArchiver(NULL),
            m_KeywordsSuggestor(NULL),
            m_RecentDirectories(NULL),
            m_MetadataSaverService(NULL),
            m_TelemetryService(NULL),
            m_UpdateService(NULL),
            m_LogsModel(NULL),
            m_LocalLibrary(NULL),
            m_MetadataIOCoordinator(NULL),
            m_AfterInitCalled(false)
        { }

        virtual ~CommandManager() {}

    public:
        void InjectDependency(Models::ArtworksRepository *artworkRepository);
        void InjectDependency(Models::ArtItemsModel *artItemsModel);
        void InjectDependency(Models::FilteredArtItemsProxyModel *filteredItemsModel);
        void InjectDependency(Models::CombinedArtworksModel *combinedArtworksModel);
        void InjectDependency(Models::IptcProvider *iptcProvider);
        void InjectDependency(Models::ArtworkUploader *artworkUploader);
        void InjectDependency(Models::UploadInfoRepository *uploadInfoRepository);
        void InjectDependency(Models::WarningsManager *warningsManager);
        void InjectDependency(Encryption::SecretsManager *secretsManager);
        void InjectDependency(UndoRedo::UndoRedoManager *undoRedoManager);
        void InjectDependency(Models::ZipArchiver *zipArchiver);
        void InjectDependency(Suggestion::KeywordsSuggestor *keywordsSuggestor);
        void InjectDependency(Models::SettingsModel *settingsModel);
        void InjectDependency(Models::RecentDirectoriesModel *recentDirectories);
        void InjectDependency(SpellCheck::SpellCheckerService *spellCheckerService);
        void InjectDependency(SpellCheck::SpellCheckSuggestionModel *spellCheckSuggestionModel);
        void InjectDependency(MetadataIO::BackupSaverService *backupSaverService);
        void InjectDependency(Conectivity::TelemetryService *telemetryService);
        void InjectDependency(Helpers::UpdateService *updateService);
        void InjectDependency(Models::LogsModel *logsModel);
        void InjectDependency(MetadataIO::MetadataIOCoordinator *metadataIOCoordinator);
        void InjectDependency(Suggestion::LocalLibrary *localLibrary);

    public:
        virtual CommandResult *processCommand(CommandBase *command)
#ifndef TESTS
        const
#endif
        ;
        void recordHistoryItem(UndoRedo::HistoryItem *historyItem) const;

    public:
        void connectEntitiesSignalsSlots() const;

    public:
        void recodePasswords(const QString &oldMasterPassword,
                                const QString &newMasterPassword,
                                const QVector<Models::UploadInfo*> &uploadInfos) const;

        void combineArtwork(Models::ArtItemInfo* itemInfo) const;
        void combineArtworks(const QVector<Models::ArtItemInfo*> &artworks) const;
        void setArtworksForIPTCProcessing(const QVector<Models::ArtworkMetadata *> &artworks) const;
        void setArtworksForUpload(const QVector<Models::ArtworkMetadata*> &artworks) const;
        void setArtworksForZipping(const QVector<Models::ArtworkMetadata*> &artworks) const;
        virtual void connectArtworkSignals(Models::ArtworkMetadata *metadata) const;
        void readMetadata(const QVector<Models::ArtworkMetadata*> &artworks) const;
        void addToLibrary(const QVector<Models::ArtworkMetadata*> &artworks) const;
        void updateArtworks(const QVector<int> &indices) const;
        void addToRecentDirectories(const QString &path) const;
#ifdef QT_DEBUG
        void addInitialArtworks(const QStringList &artworksFilepathes);
#endif
        void submitForSpellCheck(SpellCheck::ISpellCheckable *item, int keywordIndex) const;
        void submitForSpellCheck(const QVector<Models::ArtworkMetadata*> &items) const;
        void submitForSpellCheck(const QVector<SpellCheck::ISpellCheckable *> &items) const;
        void submitForSpellCheck(SpellCheck::ISpellCheckable *item) const;
        void setupSpellCheckSuggestions(SpellCheck::ISpellCheckable *item, int index, int flags);
        void saveMetadata(Models::ArtworkMetadata *metadata) const;
        void reportUserAction(Conectivity::UserAction userAction) const;
        void saveLocalLibraryAsync() const;
        void afterConstructionCallback();
        void beforeDestructionCallback() const;

    public:
        // methods for getters
        virtual Models::ArtworksRepository *getArtworksRepository() const { return m_ArtworksRepository; }
        virtual Models::ArtItemsModel *getArtItemsModel() const { return m_ArtItemsModel; }
        virtual const Encryption::SecretsManager *getSecretsManager() const { return m_SecretsManager; }
        virtual Models::UploadInfoRepository *getUploadInfoRepository() { return m_UploadInfoRepository; }
        virtual Suggestion::KeywordsSuggestor *getKeywordsSuggestor() const { return m_KeywordsSuggestor; }
        virtual Models::SettingsModel *getSettingsModel() const { return m_SettingsModel; }
        virtual SpellCheck::SpellCheckerService *getSpellCheckerService() const { return m_SpellCheckerService; }
        virtual MetadataIO::BackupSaverService *getBackupSaverService() const { return m_MetadataSaverService; }

    private:
        Models::ArtworksRepository *m_ArtworksRepository;
        Models::ArtItemsModel *m_ArtItemsModel;
        Models::FilteredArtItemsProxyModel *m_FilteredItemsModel;
        Models::CombinedArtworksModel *m_CombinedArtworksModel;
        Models::IptcProvider *m_IptcProvider;
        Models::ArtworkUploader *m_ArtworkUploader;
        Models::UploadInfoRepository *m_UploadInfoRepository;
        Models::WarningsManager *m_WarningsManager;
        Encryption::SecretsManager *m_SecretsManager;
        UndoRedo::UndoRedoManager *m_UndoRedoManager;
        Models::ZipArchiver *m_ZipArchiver;
        Suggestion::KeywordsSuggestor *m_KeywordsSuggestor;
        Models::SettingsModel *m_SettingsModel;
        Models::RecentDirectoriesModel *m_RecentDirectories;
        SpellCheck::SpellCheckerService *m_SpellCheckerService;
        SpellCheck::SpellCheckSuggestionModel *m_SpellCheckSuggestionModel;
        MetadataIO::BackupSaverService *m_MetadataSaverService;
        Conectivity::TelemetryService *m_TelemetryService;
        Helpers::UpdateService *m_UpdateService;
        Models::LogsModel *m_LogsModel;
        Suggestion::LocalLibrary *m_LocalLibrary;
        MetadataIO::MetadataIOCoordinator *m_MetadataIOCoordinator;
        volatile bool m_AfterInitCalled;
    };
}

#endif // COMMANDMANAGER_H
