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

#include "commandmanager.h"
#include "../Models/artworksrepository.h"
#include "../Models/artitemsmodel.h"
#include "../Models/combinedartworksmodel.h"
#include "../Models/artworkuploader.h"
#include "../Models/uploadinforepository.h"
#include "../Models/warningsmanager.h"
#include "../Models/uploadinfo.h"
#include "../Models/artworkmetadata.h"
#include "../Encryption/secretsmanager.h"
#include "../UndoRedo/undoredomanager.h"
#include "../Models/ziparchiver.h"
#include "../Suggestion/keywordssuggestor.h"
#include "../Commands/addartworkscommand.h"
#include "../Models/filteredartitemsproxymodel.h"
#include "../Models/recentdirectoriesmodel.h"
#include "../Models/artiteminfo.h"
#include "../SpellCheck/spellcheckerservice.h"
#include "../Models/settingsmodel.h"
#include "../SpellCheck/spellchecksuggestionmodel.h"
#include "../SpellCheck/ispellcheckable.h"
#include "../MetadataIO/backupsaverservice.h"
#include "../Conectivity/telemetryservice.h"
#include "../Helpers/updateservice.h"
#include "../Models/logsmodel.h"
#include "../Encryption/aes-qt.h"
#include "../MetadataIO/metadataiocoordinator.h"
#include "../Suggestion/locallibrary.h"
#include "../Plugins/pluginmanager.h"

void Commands::CommandManager::InjectDependency(Models::ArtworksRepository *artworkRepository) {
    Q_ASSERT(artworkRepository != NULL); m_ArtworksRepository = artworkRepository;
    m_ArtworksRepository->setCommandManager(this);
}

void Commands::CommandManager::InjectDependency(Models::ArtItemsModel *artItemsModel) {
    Q_ASSERT(artItemsModel != NULL); m_ArtItemsModel = artItemsModel;
    m_ArtItemsModel->setCommandManager(this);
}

void Commands::CommandManager::InjectDependency(Models::FilteredArtItemsProxyModel *filteredItemsModel) {
    Q_ASSERT(filteredItemsModel != NULL); m_FilteredItemsModel = filteredItemsModel;
    m_FilteredItemsModel->setCommandManager(this);
}

void Commands::CommandManager::InjectDependency(Models::CombinedArtworksModel *combinedArtworksModel) {
    Q_ASSERT(combinedArtworksModel != NULL); m_CombinedArtworksModel = combinedArtworksModel;
    m_CombinedArtworksModel->setCommandManager(this);
}

void Commands::CommandManager::InjectDependency(Models::ArtworkUploader *artworkUploader) {
    Q_ASSERT(artworkUploader != NULL); m_ArtworkUploader = artworkUploader;
    m_ArtworkUploader->setCommandManager(this);
}

void Commands::CommandManager::InjectDependency(Models::UploadInfoRepository *uploadInfoRepository) {
    Q_ASSERT(uploadInfoRepository != NULL); m_UploadInfoRepository = uploadInfoRepository;
    m_UploadInfoRepository->setCommandManager(this);
}

void Commands::CommandManager::InjectDependency(Models::WarningsManager *warningsManager) {
    Q_ASSERT(warningsManager != NULL); m_WarningsManager = warningsManager;
    m_WarningsManager->setCommandManager(this);
}

void Commands::CommandManager::InjectDependency(Encryption::SecretsManager *secretsManager) {
    Q_ASSERT(secretsManager != NULL); m_SecretsManager = secretsManager;
    m_SecretsManager->setCommandManager(this);
}

void Commands::CommandManager::InjectDependency(UndoRedo::UndoRedoManager *undoRedoManager) {
    Q_ASSERT(undoRedoManager != NULL); m_UndoRedoManager = undoRedoManager;
    m_UndoRedoManager->setCommandManager(this);
}

void Commands::CommandManager::InjectDependency(Models::ZipArchiver *zipArchiver) {
    Q_ASSERT(zipArchiver != NULL); m_ZipArchiver = zipArchiver;
    m_ZipArchiver->setCommandManager(this);
}

void Commands::CommandManager::InjectDependency(Suggestion::KeywordsSuggestor *keywordsSuggestor) {
    Q_ASSERT(keywordsSuggestor != NULL); m_KeywordsSuggestor = keywordsSuggestor;
    m_KeywordsSuggestor->setCommandManager(this);
}

void Commands::CommandManager::InjectDependency(Models::SettingsModel *settingsModel) {
    Q_ASSERT(settingsModel != NULL); m_SettingsModel = settingsModel;
    m_SettingsModel->setCommandManager(this);
}

void Commands::CommandManager::InjectDependency(Models::RecentDirectoriesModel *recentDirectories) {
    Q_ASSERT(recentDirectories != NULL); m_RecentDirectories = recentDirectories;
}

void Commands::CommandManager::InjectDependency(SpellCheck::SpellCheckerService *spellCheckerService) {
    Q_ASSERT(spellCheckerService != NULL); m_SpellCheckerService = spellCheckerService;
}

void Commands::CommandManager::InjectDependency(SpellCheck::SpellCheckSuggestionModel *spellCheckSuggestionModel) {
    Q_ASSERT(spellCheckSuggestionModel != NULL); m_SpellCheckSuggestionModel = spellCheckSuggestionModel;
    m_SpellCheckSuggestionModel->setCommandManager(this);
}

void Commands::CommandManager::InjectDependency(MetadataIO::BackupSaverService *backupSaverService) {
    Q_ASSERT(backupSaverService != NULL); m_MetadataSaverService = backupSaverService;
}

void Commands::CommandManager::InjectDependency(Conectivity::TelemetryService *telemetryService) {
    Q_ASSERT(telemetryService != NULL); m_TelemetryService = telemetryService;
}

void Commands::CommandManager::InjectDependency(Helpers::UpdateService *updateService) {
    Q_ASSERT(updateService != NULL); m_UpdateService = updateService;
}

void Commands::CommandManager::InjectDependency(Models::LogsModel *logsModel) {
    Q_ASSERT(logsModel != NULL); m_LogsModel = logsModel;
}

void Commands::CommandManager::InjectDependency(MetadataIO::MetadataIOCoordinator *metadataIOCoordinator) {
    Q_ASSERT(metadataIOCoordinator != NULL); m_MetadataIOCoordinator = metadataIOCoordinator;
    m_MetadataIOCoordinator->setCommandManager(this);
}

void Commands::CommandManager::InjectDependency(Suggestion::LocalLibrary *localLibrary) {
    Q_ASSERT(localLibrary != NULL); m_LocalLibrary = localLibrary;
}

void Commands::CommandManager::InjectDependency(Plugins::PluginManager *pluginManager) {
    Q_ASSERT(pluginManager != NULL); m_PluginManager = pluginManager;
    m_PluginManager->setCommandManager(this);
}

Commands::ICommandResult *Commands::CommandManager::processCommand(ICommandBase *command)
#ifndef TESTS
const
#endif
{
    Commands::ICommandResult *result = command->execute(this);
    delete command;
    return result;
}

void Commands::CommandManager::recordHistoryItem(UndoRedo::IHistoryItem *historyItem) const {
    if (m_UndoRedoManager) {
        m_UndoRedoManager->recordHistoryItem(historyItem);
    }
}

void Commands::CommandManager::connectEntitiesSignalsSlots() const {
    QObject::connect(m_SecretsManager, SIGNAL(beforeMasterPasswordChange(QString,QString)),
                     m_UploadInfoRepository, SLOT(onBeforeMasterPasswordChanged(QString,QString)));

    QObject::connect(m_SecretsManager, SIGNAL(afterMasterPasswordReset()),
                     m_UploadInfoRepository, SLOT(onAfterMasterPasswordReset()));

    QObject::connect(m_ArtItemsModel, SIGNAL(needCheckItemsForWarnings(QVector<ArtItemInfo*>)),
                     m_WarningsManager, SLOT(onCheckWarnings(QVector<ArtItemInfo*>)));

    QObject::connect(m_FilteredItemsModel, SIGNAL(needCheckItemsForWarnings(QVector<ArtItemInfo*>)),
                     m_WarningsManager, SLOT(onCheckWarnings(QVector<ArtItemInfo*>)));

    QObject::connect(m_ArtItemsModel, SIGNAL(selectedArtworkRemoved()),
                     m_FilteredItemsModel, SLOT(onSelectedArtworksRemoved()));

    QObject::connect(m_SettingsModel, SIGNAL(userStatisticChanged(bool)),
                     m_TelemetryService, SLOT(changeReporting(bool)));

    QObject::connect(m_SpellCheckerService, SIGNAL(serviceAvailable(bool)),
                     m_FilteredItemsModel, SLOT(onSpellCheckerAvailable(bool)));
}

void Commands::CommandManager::ensureDependenciesInjected() {
    Q_ASSERT(m_ArtworksRepository != NULL);
    Q_ASSERT(m_ArtItemsModel != NULL);
    Q_ASSERT(m_FilteredItemsModel != NULL);
    Q_ASSERT(m_CombinedArtworksModel != NULL);
    Q_ASSERT(m_ArtworkUploader != NULL);
    Q_ASSERT(m_UploadInfoRepository != NULL);
    Q_ASSERT(m_WarningsManager != NULL);
    Q_ASSERT(m_SecretsManager != NULL);
    Q_ASSERT(m_UndoRedoManager != NULL);
    Q_ASSERT(m_ZipArchiver != NULL);
    Q_ASSERT(m_KeywordsSuggestor != NULL);
    Q_ASSERT(m_SettingsModel != NULL);
    Q_ASSERT(m_RecentDirectories != NULL);
    Q_ASSERT(m_SpellCheckerService != NULL);
    Q_ASSERT(m_SpellCheckSuggestionModel != NULL);
    Q_ASSERT(m_MetadataSaverService != NULL);
    Q_ASSERT(m_TelemetryService != NULL);
    Q_ASSERT(m_UpdateService != NULL);
    Q_ASSERT(m_LogsModel != NULL);
    Q_ASSERT(m_LocalLibrary != NULL);
    Q_ASSERT(m_MetadataIOCoordinator != NULL);
    Q_ASSERT(m_PluginManager != NULL);
}

void Commands::CommandManager::recodePasswords(const QString &oldMasterPassword,
                                                  const QString &newMasterPassword,
                                                  const QVector<Models::UploadInfo *> &uploadInfos) const {
    if (m_SecretsManager) {
        qDebug() << "CommandManager::recodePasswords #" << uploadInfos.length() << "item(s)";
        foreach (Models::UploadInfo *info, uploadInfos) {
            if (info->hasPassword()) {
                QString newPassword = m_SecretsManager->recodePassword(
                            info->getPassword(), oldMasterPassword, newMasterPassword);
                info->setPassword(newPassword);
            }
        }
    }
}

void Commands::CommandManager::combineArtwork(Models::ArtItemInfo *itemInfo) const {
    qDebug() << "CommandManager::combineArtwork #" << "one item with index" << itemInfo->getOriginalIndex();
    if (m_CombinedArtworksModel) {
        m_CombinedArtworksModel->resetModelData();
        m_CombinedArtworksModel->initArtworks(QVector<Models::ArtItemInfo*>() << itemInfo);
        m_CombinedArtworksModel->recombineArtworks();

        submitItemForSpellCheck(itemInfo->getOrigin());
    }
}

void Commands::CommandManager::combineArtworks(const QVector<Models::ArtItemInfo *> &artworks) const {
    qDebug() << "CommandManager::combineArtworks #" << artworks.length() << "artworks";
    if (m_CombinedArtworksModel) {
        m_CombinedArtworksModel->resetModelData();
        m_CombinedArtworksModel->initArtworks(artworks);
        m_CombinedArtworksModel->recombineArtworks();
    }
}


void Commands::CommandManager::setArtworksForUpload(const QVector<Models::ArtworkMetadata *> &artworks) const {
    if (m_ArtworkUploader) {
        m_ArtworkUploader->setArtworks(artworks);
    }
}

void Commands::CommandManager::setArtworksForZipping(const QVector<Models::ArtworkMetadata *> &artworks) const {
    if (m_ZipArchiver) {
        m_ZipArchiver->setArtworks(artworks);
    }
}

/*virtual*/
void Commands::CommandManager::connectArtworkSignals(Models::ArtworkMetadata *metadata) const {
    if (m_ArtItemsModel) {
        QObject::connect(metadata, SIGNAL(modifiedChanged(bool)),
                         m_ArtItemsModel, SLOT(itemModifiedChanged(bool)));
    }
    
    if (m_FilteredItemsModel) {
        QObject::connect(metadata, SIGNAL(selectedChanged(bool)),
                         m_FilteredItemsModel, SLOT(itemSelectedChanged(bool)));
    }

    if (m_ArtworksRepository) {
        QObject::connect(metadata, SIGNAL(fileSelectedChanged(QString,bool)),
                         m_ArtworksRepository, SLOT(fileSelectedChanged(QString,bool)));
    }
}

void Commands::CommandManager::readMetadata(const QVector<Models::ArtworkMetadata *> &artworks,
                                            const QVector<QPair<int, int> > &rangesToUpdate) const {
    if (m_MetadataIOCoordinator) {
        m_MetadataIOCoordinator->readMetadata(artworks, rangesToUpdate);
    }
}

void Commands::CommandManager::writeMetadata(const QVector<Models::ArtworkMetadata *> &artworks, bool useBackups) const {
    if (m_MetadataIOCoordinator) {
        m_MetadataIOCoordinator->writeMetadata(artworks, useBackups);
    }
}

void Commands::CommandManager::addToLibrary(const QVector<Models::ArtworkMetadata *> &artworks) const {
    if (m_LocalLibrary) {
        m_LocalLibrary->addToLibrary(artworks);
    }
}

void Commands::CommandManager::updateArtworks(const QVector<int> &indices) const {
    if (m_ArtItemsModel) {
        m_ArtItemsModel->updateItemsAtIndices(indices);
    }
}

void Commands::CommandManager::updateArtworks(const QVector<QPair<int, int> > &rangesToUpdate) const {
    if (m_ArtItemsModel) {
        m_ArtItemsModel->updateItemsInRanges(rangesToUpdate);
    }
}

void Commands::CommandManager::addToRecentDirectories(const QString &path) const {
    if (m_RecentDirectories) {
        m_RecentDirectories->pushDirectory(path);
    }
}

#ifdef QT_DEBUG
void Commands::CommandManager::addInitialArtworks(const QStringList &artworksFilepathes)
{
    Commands::AddArtworksCommand *command = new Commands::AddArtworksCommand(artworksFilepathes);
    ICommandResult *result = this->processCommand(command);
    delete result;
}
#endif

void Commands::CommandManager::submitKeywordForSpellCheck(SpellCheck::ISpellCheckable *item, int keywordIndex) const {
    if ((m_SettingsModel != NULL) && m_SettingsModel->getUseSpellCheck() && (m_SpellCheckerService != NULL)) {
        m_SpellCheckerService->submitKeyword(item, keywordIndex);
    }
}

void Commands::CommandManager::submitForSpellCheck(const QVector<Models::ArtworkMetadata *> &items) const {
    if ((m_SettingsModel != NULL) &&
            m_SettingsModel->getUseSpellCheck() &&
            (m_SpellCheckerService != NULL) &&
            !items.isEmpty()) {
        QVector<SpellCheck::ISpellCheckable*> itemsToSubmit;
        int count = items.length();
        itemsToSubmit.reserve(count);

        for (int i = 0; i < count; ++i) {
            itemsToSubmit << items.at(i);
        }

        this->submitForSpellCheck(itemsToSubmit);
    }
}

void Commands::CommandManager::submitForSpellCheck(const QVector<SpellCheck::ISpellCheckable *> &items) const {
    if (m_SettingsModel->getUseSpellCheck() && m_SpellCheckerService != NULL) {
        m_SpellCheckerService->submitItems(items);
    }
}

void Commands::CommandManager::submitItemForSpellCheck(SpellCheck::ISpellCheckable *item, int flags) const {
    if ((m_SettingsModel != NULL) && m_SettingsModel->getUseSpellCheck() && (m_SpellCheckerService != NULL)) {
        m_SpellCheckerService->submitItem(item, flags);
    }
}

void Commands::CommandManager::setupSpellCheckSuggestions(SpellCheck::ISpellCheckable *item, int index, int flags) {
    if (m_SpellCheckSuggestionModel) {
        m_SpellCheckSuggestionModel->setupModel(item, index, flags);
        reportUserAction(Conectivity::UserActionSpellSuggestions);
    }
}

void Commands::CommandManager::saveMetadata(Models::ArtworkMetadata *metadata) const {
    if (m_SettingsModel->getSaveBackups() && m_MetadataSaverService != NULL) {
        m_MetadataSaverService->saveArtwork(metadata);
    }
}

void Commands::CommandManager::reportUserAction(Conectivity::UserAction userAction) const {
    if (m_TelemetryService) {
        m_TelemetryService->reportAction(userAction);
    }
}

void Commands::CommandManager::cleanupLocalLibraryAsync() const {
    if (m_LocalLibrary) {
        m_LocalLibrary->cleanupLocalLibraryAsync();
    }
}

void Commands::CommandManager::afterConstructionCallback()  {
    if (m_AfterInitCalled) {
        qWarning() << "CommandManager::afterConstructionCallback #" << "Attempt to call afterConstructionCallback() second time";
        return;
    }

    m_AfterInitCalled = true;

    m_SpellCheckerService->startChecking();
    m_MetadataSaverService->startSaving();

    const QString reportingEndpoint = QLatin1String("cc39a47f60e1ed812e2403b33678dd1c529f1cc43f66494998ec478a4d13496269a3dfa01f882941766dba246c76b12b2a0308e20afd84371c41cf513260f8eb8b71f8c472cafb1abf712c071938ec0791bbf769ab9625c3b64827f511fa3fbb");
    QString endpoint = Encryption::decodeText(reportingEndpoint, "reporting");
    m_TelemetryService->setEndpoint(endpoint);

    m_PluginManager->loadPlugins();

#if !defined(Q_OS_LINUX)
    m_UpdateService->startChecking();
#endif
}

void Commands::CommandManager::beforeDestructionCallback() const {
    qDebug() << "CommandManager::beforeDestructionCallback #";
    if (!m_AfterInitCalled) { return; }

    // we have a second for important stuff
    m_TelemetryService->reportAction(Conectivity::UserActionClose);
    m_SpellCheckerService->stopChecking();
    m_MetadataSaverService->stopSaving();
    m_PluginManager->unloadPlugins();
#ifndef TESTS
    m_LogsModel->stopLogging();
#endif
}

void Commands::CommandManager::restartSpellChecking() {
    if (m_SpellCheckerService) {
        m_SpellCheckerService->restartWorker();
    }
}
