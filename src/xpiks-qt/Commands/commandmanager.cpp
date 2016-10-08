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
#include "../Common/defines.h"
#include "../Models/artworksrepository.h"
#include "../Models/artitemsmodel.h"
#include "../Models/combinedartworksmodel.h"
#include "../Models/artworkuploader.h"
#include "../Models/uploadinforepository.h"
#include "../Models/uploadinfo.h"
#include "../Models/artworkmetadata.h"
#include "../Encryption/secretsmanager.h"
#include "../UndoRedo/undoredomanager.h"
#include "../Models/ziparchiver.h"
#include "../Suggestion/keywordssuggestor.h"
#include "../Commands/addartworkscommand.h"
#include "../Models/filteredartitemsproxymodel.h"
#include "../Models/recentdirectoriesmodel.h"
#include "../Models/metadataelement.h"
#include "../SpellCheck/spellcheckerservice.h"
#include "../Models/settingsmodel.h"
#include "../SpellCheck/spellchecksuggestionmodel.h"
#include "../SpellCheck/ispellcheckable.h"
#include "../MetadataIO/backupsaverservice.h"
#include "../Conectivity/telemetryservice.h"
#include "../Conectivity/updateservice.h"
#include "../Models/logsmodel.h"
#include "../Encryption/aes-qt.h"
#include "../MetadataIO/metadataiocoordinator.h"
#include "../Suggestion/locallibrary.h"
#include "../Plugins/pluginmanager.h"
#include "../Warnings/warningsservice.h"
#include "../Models/languagesmodel.h"
#include "../AutoComplete/autocompleteservice.h"
#include "../Helpers/deletelogshelper.h"
#include "../QMLExtensions/imagecachingservice.h"
#include "../Models/findandreplacemodel.h"
#include "../Models/deletekeywordsviewmodel.h"
#include "../Helpers/helpersqmlwrapper.h"

void Commands::CommandManager::InjectDependency(Models::ArtworksRepository *artworkRepository) {
    Q_ASSERT(artworkRepository != NULL); m_ArtworksRepository = artworkRepository;
    m_ArtworksRepository->setCommandManager(this);
}

void Commands::CommandManager::InjectDependency(Models::ArtItemsModel *artItemsModel) {
    Q_ASSERT(artItemsModel != NULL); m_ArtItemsModel = artItemsModel;
    m_ArtItemsModel->setCommandManager(this);
    // do not add to availabilityListeners
}

void Commands::CommandManager::InjectDependency(Models::FilteredArtItemsProxyModel *filteredItemsModel) {
    Q_ASSERT(filteredItemsModel != NULL); m_FilteredItemsModel = filteredItemsModel;
    m_FilteredItemsModel->setCommandManager(this);
}

void Commands::CommandManager::InjectDependency(Models::CombinedArtworksModel *combinedArtworksModel) {
    Q_ASSERT(combinedArtworksModel != NULL); m_CombinedArtworksModel = combinedArtworksModel;
    m_CombinedArtworksModel->setCommandManager(this);
    m_AvailabilityListeners.append(combinedArtworksModel);
}

void Commands::CommandManager::InjectDependency(Models::ArtworkUploader *artworkUploader) {
    Q_ASSERT(artworkUploader != NULL); m_ArtworkUploader = artworkUploader;
    m_ArtworkUploader->setCommandManager(this);
    m_AvailabilityListeners.append(m_ArtworkUploader);
}

void Commands::CommandManager::InjectDependency(Models::UploadInfoRepository *uploadInfoRepository) {
    Q_ASSERT(uploadInfoRepository != NULL); m_UploadInfoRepository = uploadInfoRepository;
    m_UploadInfoRepository->setCommandManager(this);
}

void Commands::CommandManager::InjectDependency(Warnings::WarningsService *warningsService) {
    Q_ASSERT(warningsService != NULL); m_WarningsService = warningsService;
    m_WarningsService->setCommandManager(this);
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
    m_AvailabilityListeners.append(zipArchiver);
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

void Commands::CommandManager::InjectDependency(Conectivity::UpdateService *updateService) {
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

void Commands::CommandManager::InjectDependency(Models::LanguagesModel *languagesModel) {
    Q_ASSERT(languagesModel != NULL); m_LanguagesModel = languagesModel;
    m_LanguagesModel->setCommandManager(this);
}

void Commands::CommandManager::InjectDependency(QMLExtensions::ColorsModel *colorsModel) {
    Q_ASSERT(colorsModel != NULL); m_ColorsModel = colorsModel;
}

void Commands::CommandManager::InjectDependency(AutoComplete::AutoCompleteService *autoCompleteService) {
    Q_ASSERT(autoCompleteService != NULL); m_AutoCompleteService = autoCompleteService;
}

void Commands::CommandManager::InjectDependency(QMLExtensions::ImageCachingService *imageCachingService) {
    Q_ASSERT(imageCachingService != NULL); m_ImageCachingService = imageCachingService;
}

void Commands::CommandManager::InjectDependency(Models::FindAndReplaceModel *findAndReplaceModel) {
    Q_ASSERT(findAndReplaceModel != NULL); m_FindAndReplaceModel = findAndReplaceModel;
    m_FindAndReplaceModel->setCommandManager(this);
}

void Commands::CommandManager::InjectDependency(Models::DeleteKeywordsViewModel *deleteKeywordsViewModel) {
    Q_ASSERT(deleteKeywordsViewModel != NULL); m_DeleteKeywordsViewModel = deleteKeywordsViewModel;
    m_DeleteKeywordsViewModel->setCommandManager(this);
}

void Commands::CommandManager::InjectDependency(Helpers::HelpersQmlWrapper *helpersQmlWrapper) {
    Q_ASSERT(helpersQmlWrapper != NULL); m_HelpersQmlWrapper = helpersQmlWrapper;
    m_HelpersQmlWrapper->setCommandManager(this);
}

std::shared_ptr<Commands::ICommandResult> Commands::CommandManager::processCommand(const std::shared_ptr<ICommandBase> &command)
#ifndef CORE_TESTS
const
#endif
{
    std::shared_ptr<Commands::ICommandResult> result = command->execute(this);

    result->afterExecCallback(this);
    return result;
}

void Commands::CommandManager::addWarningsService(Common::IServiceBase<Common::IBasicArtwork, Common::WarningsCheckFlags> *service) {
    if (service != NULL) {
        // TODO: check if we don't have such checker
        m_WarningsCheckers.append(service);
    }
}

void Commands::CommandManager::recordHistoryItem(std::unique_ptr<UndoRedo::IHistoryItem> &historyItem) const {
    if (m_UndoRedoManager) {
        m_UndoRedoManager->recordHistoryItem(historyItem);
    }
}

void Commands::CommandManager::connectEntitiesSignalsSlots() const {
    if (m_SecretsManager != NULL && m_UploadInfoRepository != NULL) {
        QObject::connect(m_SecretsManager, SIGNAL(beforeMasterPasswordChange(QString, QString)),
                         m_UploadInfoRepository, SLOT(onBeforeMasterPasswordChanged(QString, QString)));

        QObject::connect(m_SecretsManager, SIGNAL(afterMasterPasswordReset()),
                         m_UploadInfoRepository, SLOT(onAfterMasterPasswordReset()));
    }

    if (m_ArtItemsModel != NULL && m_FilteredItemsModel != NULL) {
        QObject::connect(m_ArtItemsModel, SIGNAL(selectedArtworksRemoved(int)),
                         m_FilteredItemsModel, SLOT(onSelectedArtworksRemoved(int)));
    }

    if (m_SettingsModel != NULL && m_TelemetryService != NULL) {
        QObject::connect(m_SettingsModel, SIGNAL(userStatisticsChanged(bool)),
                         m_TelemetryService, SLOT(changeReporting(bool)));
    }

    if (m_SpellCheckerService != NULL && m_FilteredItemsModel != NULL) {
        QObject::connect(m_SpellCheckerService, SIGNAL(serviceAvailable(bool)),
                         m_FilteredItemsModel, SLOT(onSpellCheckerAvailable(bool)));
    }

    if (m_ArtworksRepository != NULL && m_ArtItemsModel != NULL) {
        QObject::connect(m_ArtworksRepository, SIGNAL(filesUnavailable()),
                         m_ArtItemsModel, SLOT(onFilesUnavailableHandler()));
    }

    if (m_ArtItemsModel != NULL && m_UndoRedoManager != NULL) {
        QObject::connect(m_UndoRedoManager, SIGNAL(undoStackEmpty()),
                         m_ArtItemsModel, SLOT(onUndoStackEmpty()));
    }

    if (m_LanguagesModel != NULL && m_KeywordsSuggestor != NULL) {
        QObject::connect(m_LanguagesModel, SIGNAL(languageChanged()),
                         m_KeywordsSuggestor, SLOT(onLanguageChanged()));
    }

    if (m_SpellCheckerService != NULL && m_ArtItemsModel != NULL) {
        QObject::connect(m_SpellCheckerService, SIGNAL(userDictUpdate(QStringList)),
                         m_ArtItemsModel, SLOT(userDictUpdateHandler(QStringList)));
        QObject::connect(m_SpellCheckerService, SIGNAL(userDictCleared()),
                         m_ArtItemsModel, SLOT(userDictClearedHandler()));
    }

    if (m_SpellCheckerService != NULL && m_CombinedArtworksModel != NULL) {
        QObject::connect(m_SpellCheckerService, SIGNAL(userDictUpdate(QStringList)),
                         m_CombinedArtworksModel, SLOT(userDictUpdateHandler(QStringList)));
        QObject::connect(m_SpellCheckerService, SIGNAL(userDictCleared()),
                         m_CombinedArtworksModel, SLOT(userDictClearedHandler()));
    }

    if (m_HelpersQmlWrapper != NULL && m_UpdateService != NULL) {
        QObject::connect(m_UpdateService, SIGNAL(updateAvailable(QString)),
                         m_HelpersQmlWrapper, SIGNAL(updateAvailable(QString)));

        QObject::connect(m_UpdateService, SIGNAL(updateDownloaded(QString)),
                         m_HelpersQmlWrapper, SLOT(updateIsDownloaded(QString)));
    }
}

void Commands::CommandManager::ensureDependenciesInjected() {
    Q_ASSERT(m_ArtworksRepository != NULL);
    Q_ASSERT(m_ArtItemsModel != NULL);
    Q_ASSERT(m_FilteredItemsModel != NULL);
    Q_ASSERT(m_CombinedArtworksModel != NULL);
    Q_ASSERT(m_ArtworkUploader != NULL);
    Q_ASSERT(m_UploadInfoRepository != NULL);
    Q_ASSERT(m_WarningsService != NULL);
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
    Q_ASSERT(m_LanguagesModel != NULL);
    Q_ASSERT(m_ColorsModel != NULL);
    Q_ASSERT(m_AutoCompleteService != NULL);
    Q_ASSERT(m_ImageCachingService != NULL);
    Q_ASSERT(m_FindAndReplaceModel != NULL);
    Q_ASSERT(m_DeleteKeywordsViewModel != NULL);

#ifndef INTEGRATION_TESTS
    Q_ASSERT(m_HelpersQmlWrapper != NULL);
#endif
}

void Commands::CommandManager::recodePasswords(const QString &oldMasterPassword,
                                               const QString &newMasterPassword,
                                               const std::vector<std::shared_ptr<Models::UploadInfo> > &uploadInfos) const {
    if (m_SecretsManager) {
        LOG_INFO << uploadInfos.size() << "item(s)";

        for (auto &info: uploadInfos) {
            if (info->hasPassword()) {
                QString newPassword = m_SecretsManager->recodePassword(
                    info->getPassword(), oldMasterPassword, newMasterPassword);
                info->setPassword(newPassword);
            }
        }
    }
}

void Commands::CommandManager::combineArtwork(Models::ArtworkMetadata *metadata, int index) const {
    LOG_INFO << "one item with index" << index;
    if (m_CombinedArtworksModel) {
        std::vector<Models::MetadataElement> items;
        items.emplace_back(metadata, index);

        m_CombinedArtworksModel->resetModel();
        m_CombinedArtworksModel->setArtworks(items);
    }
}

void Commands::CommandManager::combineArtworks(std::vector<Models::MetadataElement> &artworks) const {
    LOG_INFO << artworks.size() << "artworks";
    if (m_CombinedArtworksModel) {
        m_CombinedArtworksModel->resetModel();
        m_CombinedArtworksModel->setArtworks(artworks);
    }
}

void Commands::CommandManager::deleteKeywordsFromArtworks(std::vector<Models::MetadataElement> &artworks) const {
    LOG_INFO << artworks.size() << "artworks";
    if (m_DeleteKeywordsViewModel != NULL) {
        m_DeleteKeywordsViewModel->setArtworks(artworks);
    }
}

void Commands::CommandManager::setArtworksForUpload(const QVector<Models::ArtworkMetadata *> &artworks) const {
    LOG_INFO << artworks.size() << "artworks";
    if (m_ArtworkUploader) {
        m_ArtworkUploader->setArtworks(artworks);
    }
}

void Commands::CommandManager::setArtworksForZipping(const QVector<Models::ArtworkMetadata *> &artworks) const {
    LOG_INFO << artworks.size() << "artworks";
    if (m_ZipArchiver) {
        m_ZipArchiver->setArtworks(artworks);
    }
}

/*virtual*/
void Commands::CommandManager::connectArtworkSignals(Models::ArtworkMetadata *metadata) const {
    if (m_ArtItemsModel) {
        LOG_INTEGRATION_TESTS << "Connecting to ArtItemsModel...";

        QObject::connect(metadata, SIGNAL(modifiedChanged(bool)),
                         m_ArtItemsModel, SLOT(itemModifiedChanged(bool)));

        QObject::connect(metadata, SIGNAL(spellCheckErrorsChanged()),
                         m_ArtItemsModel, SLOT(spellCheckErrorsChanged()));

        QObject::connect(metadata, SIGNAL(backupRequired()),
                         m_ArtItemsModel, SLOT(artworkBackupRequested()));
    }

    if (m_FilteredItemsModel) {
        LOG_INTEGRATION_TESTS << "Connecting to FilteredItemsModel...";

        QObject::connect(metadata, SIGNAL(selectedChanged(bool)),
                         m_FilteredItemsModel, SLOT(itemSelectedChanged(bool)));
    }

    if (m_ArtworksRepository) {
        LOG_INTEGRATION_TESTS << "Connecting to ArtworksRepository...";
        // QObject::connect(metadata, SIGNAL(fileSelectedChanged(QString,bool)),
        //                 m_ArtworksRepository, SLOT(fileSelectedChanged(QString,bool)));
    }
}

void Commands::CommandManager::readMetadata(const QVector<Models::ArtworkMetadata *> &artworks,
                                            const QVector<QPair<int, int> > &rangesToUpdate) const {
#ifndef CORE_TESTS
    if (m_MetadataIOCoordinator) {
        if ((m_SettingsModel != NULL) && !m_SettingsModel->getUseExifTool()) {
            m_MetadataIOCoordinator->readMetadataExiv2(artworks, rangesToUpdate);
        } else {
            // fallback
            m_MetadataIOCoordinator->readMetadataExifTool(artworks, rangesToUpdate);
        }
    }

#else
    Q_UNUSED(artworks);
    Q_UNUSED(rangesToUpdate);
#endif
}

void Commands::CommandManager::writeMetadata(const QVector<Models::ArtworkMetadata *> &artworks, bool useBackups) const {
#ifndef CORE_TESTS
    if (m_MetadataIOCoordinator) {
        if ((m_SettingsModel != NULL) && !m_SettingsModel->getUseExifTool()) {
            m_MetadataIOCoordinator->writeMetadataExiv2(artworks);
        } else {
            // fallback
            m_MetadataIOCoordinator->writeMetadataExifTool(artworks, useBackups);
        }
    }

#else
    Q_UNUSED(artworks);
    Q_UNUSED(useBackups);
#endif
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

void Commands::CommandManager::autoDiscoverExiftool() const {
    if (m_MetadataIOCoordinator) {
        m_MetadataIOCoordinator->autoDiscoverExiftool();
    }
}

#ifdef QT_DEBUG
void Commands::CommandManager::openInitialFiles() {
    if (m_InitialFilesToOpen.isEmpty()) {
        return;
    }

    m_ArtItemsModel->dropFiles(m_InitialFilesToOpen);
}

void Commands::CommandManager::addInitialArtworks(const QList<QUrl> &filePaths) {
    m_InitialFilesToOpen = filePaths;
}

#endif

void Commands::CommandManager::generatePreviews(const QVector<Models::ArtworkMetadata *> &items) const {
#ifndef CORE_TESTS
    if (m_SettingsModel != NULL &&
        m_SettingsModel->getAutoCacheImages() &&
        m_ImageCachingService != NULL) {
        m_ImageCachingService->generatePreviews(items);
    }

#else
    Q_UNUSED(items);
#endif
}

void Commands::CommandManager::submitKeywordForSpellCheck(Common::BasicKeywordsModel *item, int keywordIndex) const {
    Q_ASSERT(item != NULL);
    if ((m_SettingsModel != NULL) && m_SettingsModel->getUseSpellCheck() && (m_SpellCheckerService != NULL)) {
        m_SpellCheckerService->submitKeyword(item, keywordIndex);
    }
}

void Commands::CommandManager::submitForSpellCheck(const QVector<Models::ArtworkMetadata *> &items) const {
    if ((m_SettingsModel != NULL) &&
        m_SettingsModel->getUseSpellCheck() &&
        (m_SpellCheckerService != NULL) &&
        !items.isEmpty()) {
        QVector<Common::BasicKeywordsModel *> itemsToSubmit;
        int count = items.length();
        itemsToSubmit.reserve(count);

        for (int i = 0; i < count; ++i) {
            Models::ArtworkMetadata *metadata = items.at(i);
            itemsToSubmit << metadata->getKeywordsModel();
        }

        this->submitForSpellCheck(itemsToSubmit);
    }
}

void Commands::CommandManager::submitForSpellCheck(const QVector<Common::BasicKeywordsModel *> &items) const {
    if ((m_SettingsModel != NULL) && m_SettingsModel->getUseSpellCheck() && m_SpellCheckerService != NULL) {
        m_SpellCheckerService->submitItems(items);
    }
}

void Commands::CommandManager::submitItemForSpellCheck(Common::BasicKeywordsModel *item, Common::SpellCheckFlags flags) const {
    Q_ASSERT(item != NULL);
    if ((m_SettingsModel != NULL) && m_SettingsModel->getUseSpellCheck() && (m_SpellCheckerService != NULL)) {
        m_SpellCheckerService->submitItem(item, flags);
    }
}

void Commands::CommandManager::setupSpellCheckSuggestions(Common::BasicKeywordsModel *item, int index, Common::SuggestionFlags flags) {
    Q_ASSERT(item != NULL);
    if (m_SpellCheckSuggestionModel) {
        m_SpellCheckSuggestionModel->setupModel(item, index, flags);
        reportUserAction(Conectivity::UserAction::SpellSuggestions);
    }
}

void Commands::CommandManager::submitForSpellCheck(const QVector<Common::BasicKeywordsModel *> &items,
                                                   const QStringList &wordsToCheck) const {
    if ((m_SettingsModel != NULL) && m_SettingsModel->getUseSpellCheck() && m_SpellCheckerService != NULL) {
        m_SpellCheckerService->submitItems(items, wordsToCheck);
    }
}

void Commands::CommandManager::submitKeywordsForWarningsCheck(Models::ArtworkMetadata *item) const {
    Q_ASSERT(item != NULL);
    this->submitForWarningsCheck(item, Common::WarningsCheckFlags::Keywords);
}

void Commands::CommandManager::submitForWarningsCheck(Models::ArtworkMetadata *item, Common::WarningsCheckFlags flags) const {
    Q_ASSERT(item != NULL);

    if (m_WarningsService != NULL) {
        m_WarningsService->submitItem(item, flags);
    }

    int count = m_WarningsCheckers.length();

    LOG_INTEGRATION_TESTS << count << "checkers available";

    for (int i = 0; i < count; ++i) {
        Common::IServiceBase<Common::IBasicArtwork, Common::WarningsCheckFlags> *checker = m_WarningsCheckers.at(i);
        if (checker->isAvailable()) {
            checker->submitItem(item, flags);
        }
    }
}

void Commands::CommandManager::submitForWarningsCheck(const QVector<Models::ArtworkMetadata *> &items) const {
    if (m_WarningsService != NULL) {
        m_WarningsService->submitItems(items);
    }

    if (!m_WarningsCheckers.isEmpty()) {
        QVector<Common::IBasicArtwork *> itemsToSubmit;
        int count = items.length();
        itemsToSubmit.reserve(count);

        for (int i = 0; i < count; ++i) {
            itemsToSubmit << items.at(i);
        }

        this->submitForWarningsCheck(itemsToSubmit);
    }
}

void Commands::CommandManager::submitForWarningsCheck(const QVector<Common::IBasicArtwork *> &items) const {
    int count = m_WarningsCheckers.length();

    for (int i = 0; i < count; ++i) {
        Common::IServiceBase<Common::IBasicArtwork, Common::WarningsCheckFlags> *checker = m_WarningsCheckers.at(i);
        if (checker->isAvailable()) {
            checker->submitItems(items);
        }
    }
}

void Commands::CommandManager::saveArtworkBackup(Models::ArtworkMetadata *metadata) const {
    Q_ASSERT(metadata != NULL);
    if ((m_SettingsModel != NULL) && m_SettingsModel->getSaveBackups() && m_MetadataSaverService != NULL) {
        m_MetadataSaverService->saveArtwork(metadata);
    }
}

void Commands::CommandManager::saveArtworksBackups(const QVector<Models::ArtworkMetadata *> &artworks) const {
    if ((m_SettingsModel != NULL) && m_SettingsModel->getSaveBackups() && m_MetadataSaverService != NULL) {
        m_MetadataSaverService->saveArtworks(artworks);
    }
}

void Commands::CommandManager::reportUserAction(Conectivity::UserAction userAction) const {
#ifndef CORE_TESTS
    if (m_TelemetryService) {
        m_TelemetryService->reportAction(userAction);
    }
#else
    Q_UNUSED(userAction);
#endif
}

void Commands::CommandManager::cleanupLocalLibraryAsync() const {
    if (m_LocalLibrary) {
        m_LocalLibrary->cleanupLocalLibraryAsync();
    }
}

void Commands::CommandManager::afterConstructionCallback() {
    if (m_AfterInitCalled) {
        LOG_WARNING << "Attempt to call afterConstructionCallback() second time";
        return;
    }

    m_AfterInitCalled = true;

#ifndef CORE_TESTS
    m_ImageCachingService->startService();
#endif
    m_SpellCheckerService->startService();
    m_WarningsService->startService();
    m_MetadataSaverService->startSaving();
    m_AutoCompleteService->startService();

    QCoreApplication::processEvents();

#ifndef CORE_TESTS
    const QString reportingEndpoint =
        QLatin1String(
            "cc39a47f60e1ed812e2403b33678dd1c529f1cc43f66494998ec478a4d13496269a3dfa01f882941766dba246c76b12b2a0308e20afd84371c41cf513260f8eb8b71f8c472cafb1abf712c071938ec0791bbf769ab9625c3b64827f511fa3fbb");
    QString endpoint = Encryption::decodeText(reportingEndpoint, "reporting");
    m_TelemetryService->setEndpoint(endpoint);

#ifdef WITH_PLUGINS
    m_PluginManager->loadPlugins();
#endif

    m_TelemetryService->startReporting();
    m_UpdateService->startChecking();
    m_ArtworkUploader->initializeStocksList();
#endif

#ifdef Q_OS_MAC
    if (m_SettingsModel->getUseExifTool()) {
        QCoreApplication::processEvents();
        m_MetadataIOCoordinator->autoDiscoverExiftool();
    }

#endif

#ifdef QT_DEBUG
    openInitialFiles();
#endif

#if !defined(CORE_TESTS) && !defined(INTEGRATION_TESTS)
    Helpers::performCleanLogs();
#endif

#if !defined(CORE_TESTS)
    m_WarningsService->initWarningsSettings();
#endif
}

void Commands::CommandManager::beforeDestructionCallback() const {
    LOG_DEBUG << "Shutting down...";
    if (!m_AfterInitCalled) {
        return;
    }

    m_ArtworksRepository->stopListeningToUnavailableFiles();

    m_ArtItemsModel->disconnect();
    m_ArtItemsModel->deleteAllItems();
    m_FilteredItemsModel->disconnect();

#ifndef CORE_TESTS
    m_ImageCachingService->stopService();
#endif
    m_SpellCheckerService->stopService();
    m_WarningsService->stopService();
    m_MetadataSaverService->stopSaving();
    m_AutoCompleteService->stopService();

#ifdef WITH_PLUGINS
    m_PluginManager->unloadPlugins();
#endif

#ifndef CORE_TESTS
    // we have a second for important stuff
    m_TelemetryService->reportAction(Conectivity::UserAction::Close);
    m_TelemetryService->stopReporting();

    m_LogsModel->stopLogging();
#endif
}

void Commands::CommandManager::requestCloseApplication() const {
    if (m_HelpersQmlWrapper != NULL) {
        m_HelpersQmlWrapper->requestCloseApplication();
    }
}

void Commands::CommandManager::restartSpellChecking() {
    if (m_SpellCheckerService) {
        m_SpellCheckerService->restartWorker();
    }
}

#ifndef CORE_TESTS
void Commands::CommandManager::autoCompleteKeyword(const QString &keyword, QObject *notifyObject) const {
    if ((m_SettingsModel != NULL) && m_SettingsModel->getUseAutoComplete() && (m_AutoCompleteService != NULL)) {
        m_AutoCompleteService->findKeywordCompletions(keyword, notifyObject);
    }
}

#endif

void Commands::CommandManager::removeUnavailableFiles() {
    LOG_DEBUG << "#";

    m_CombinedArtworksModel->generateAboutToBeRemoved();
    m_ArtItemsModel->generateAboutToBeRemoved();
    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

    int size = m_AvailabilityListeners.size();
    for (int i = 0; i < size; ++i) {
        m_AvailabilityListeners[i]->removeUnavailableItems();
    }

    m_ArtItemsModel->removeUnavailableItems();

#ifndef CORE_TESTS
    m_UndoRedoManager->discardLastAction();
#endif

    if (m_ArtworksRepository->canPurgeUnavailableFiles()) {
        m_ArtworksRepository->purgeUnavailableFiles();
    } else {
        LOG_INFO << "Unavailable files purging postponed";
    }
}

#ifdef INTEGRATION_TESTS
void Commands::CommandManager::cleanup() {
    m_CombinedArtworksModel->resetModel();
    m_ZipArchiver->resetArtworks();
    m_ZipArchiver->resetModel();
    m_ArtworkUploader->resetArtworks();
    m_ArtworkUploader->resetModel();
    m_ArtworksRepository->resetEverything();
    m_ArtItemsModel->deleteAllItems();
    m_SettingsModel->resetToDefault();
    m_SpellCheckerService->clearUserDictionary();
}

#endif
