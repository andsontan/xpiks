#include <iostream>
#include <QDebug>

#include "../../xpiks-qt/SpellCheck/spellchecksuggestionmodel.h"
#include "../../xpiks-qt/Models/filteredartitemsproxymodel.h"
#include "../../xpiks-qt/Suggestion/suggestionqueryengine.h"
#include "../../xpiks-qt/MetadataIO/metadataiocoordinator.h"
#include "../../xpiks-qt/Conectivity/analyticsuserevent.h"
#include "../../xpiks-qt/SpellCheck/spellcheckerservice.h"
#include "../../xpiks-qt/Models/recentdirectoriesmodel.h"
#include "../../xpiks-qt/MetadataIO/backupsaverservice.h"
#include "../../xpiks-qt/Suggestion/keywordssuggestor.h"
#include "../../xpiks-qt/Models/combinedartworksmodel.h"
#include "../../xpiks-qt/Conectivity/telemetryservice.h"
#include "../../xpiks-qt/Helpers/globalimageprovider.h"
#include "../../xpiks-qt/Models/uploadinforepository.h"
#include "../../xpiks-qt/Conectivity/ftpcoordinator.h"
#include "../../xpiks-qt/Helpers/helpersqmlwrapper.h"
#include "../../xpiks-qt/Encryption/secretsmanager.h"
#include "../../xpiks-qt/Models/artworksrepository.h"
#include "../../xpiks-qt/Warnings/warningsservice.h"
#include "../../xpiks-qt/UndoRedo/undoredomanager.h"
#include "../../xpiks-qt/Helpers/clipboardhelper.h"
#include "../../xpiks-qt/Commands/commandmanager.h"
#include "../../xpiks-qt/Suggestion/locallibrary.h"
#include "../../xpiks-qt/Models/artworkuploader.h"
#include "../../xpiks-qt/Warnings/warningsmodel.h"
#include "../../xpiks-qt/Plugins/pluginmanager.h"
#include "../../xpiks-qt/Helpers/loggingworker.h"
#include "../../xpiks-qt/Models/languagesmodel.h"
#include "../../xpiks-qt/Helpers/updateservice.h"
#include "../../xpiks-qt/Models/artitemsmodel.h"
#include "../../xpiks-qt/Models/settingsmodel.h"
#include "../../xpiks-qt/Helpers/appsettings.h"
#include "../../xpiks-qt/Models/ziparchiver.h"
#include "../../xpiks-qt/Helpers/constants.h"
#include "../../xpiks-qt/Helpers/runguard.h"
#include "../../xpiks-qt/Models/logsmodel.h"
#include "../../xpiks-qt/Helpers/logger.h"
#include "../../xpiks-qt/Common/version.h"
#include "../../xpiks-qt/Common/defines.h"

#include "integrationtestbase.h"
#include "addfilesbasictest.h"

#if defined(WITH_LOGS)
#undef WITH_LOGS
#endif

#if defined(WITH_PLUGINS)
#undef WITH_PLUGINS
#endif

int main(int argc, char *argv[]) {
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    qSetMessagePattern("%{time hh:mm:ss.zzz} %{type} T#%{threadid} %{function} - %{message}");

    Helpers::AppSettings appSettings;
    Suggestion::LocalLibrary localLibrary;

#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
#else
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
#endif
    if (!appDataPath.isEmpty()) {
        QDir appDataDir(appDataPath);

        QString libraryFilePath = appDataDir.filePath(Constants::LIBRARY_FILENAME);
        localLibrary.setLibraryPath(libraryFilePath);
    } else {
        std::cerr << "AppDataPath is empty!";
    }

    Models::LogsModel logsModel;
    logsModel.startLogging();

    localLibrary.loadLibraryAsync();

    Models::ArtworksRepository artworkRepository;
    Models::ArtItemsModel artItemsModel;
    Models::CombinedArtworksModel combinedArtworksModel;
    Models::UploadInfoRepository uploadInfoRepository;
    Warnings::WarningsService warningsService;
    Models::SettingsModel settingsModel;
    settingsModel.readAllValues();
    Encryption::SecretsManager secretsManager;
    UndoRedo::UndoRedoManager undoRedoManager;
    Models::ZipArchiver zipArchiver;
    Suggestion::KeywordsSuggestor keywordsSuggestor;
    keywordsSuggestor.setLocalLibrary(&localLibrary);
    Models::FilteredArtItemsProxyModel filteredArtItemsModel;
    filteredArtItemsModel.setSourceModel(&artItemsModel);
    Models::RecentDirectoriesModel recentDirectorieModel;
    Conectivity::FtpCoordinator *ftpCoordinator = new Conectivity::FtpCoordinator(
                settingsModel.getMaxParallelUploads(), settingsModel.getUploadTimeout());
    Models::ArtworkUploader artworkUploader(ftpCoordinator);
    SpellCheck::SpellCheckerService spellCheckerService;
    SpellCheck::SpellCheckSuggestionModel spellCheckSuggestionModel;
    MetadataIO::BackupSaverService metadataSaverService;
    Warnings::WarningsModel warningsModel;
    warningsModel.setSourceModel(&artItemsModel);
    Models::LanguagesModel languagesModel;

    Helpers::UpdateService updateService(false);

    MetadataIO::MetadataIOCoordinator metadataIOCoordinator;
    Conectivity::TelemetryService telemetryService("1234567890", false);
    Plugins::PluginManager pluginManager;

    Commands::CommandManager commandManager;
    commandManager.InjectDependency(&artworkRepository);
    commandManager.InjectDependency(&artItemsModel);
    commandManager.InjectDependency(&filteredArtItemsModel);
    commandManager.InjectDependency(&combinedArtworksModel);
    commandManager.InjectDependency(&artworkUploader);
    commandManager.InjectDependency(&uploadInfoRepository);
    commandManager.InjectDependency(&warningsService);
    commandManager.InjectDependency(&secretsManager);
    commandManager.InjectDependency(&undoRedoManager);
    commandManager.InjectDependency(&zipArchiver);
    commandManager.InjectDependency(&keywordsSuggestor);
    commandManager.InjectDependency(&settingsModel);
    commandManager.InjectDependency(&recentDirectorieModel);
    commandManager.InjectDependency(&spellCheckerService);
    commandManager.InjectDependency(&spellCheckSuggestionModel);
    commandManager.InjectDependency(&metadataSaverService);
    commandManager.InjectDependency(&telemetryService);
    commandManager.InjectDependency(&updateService);
    commandManager.InjectDependency(&logsModel);
    commandManager.InjectDependency(&localLibrary);
    commandManager.InjectDependency(&metadataIOCoordinator);
    commandManager.InjectDependency(&pluginManager);
    commandManager.InjectDependency(&languagesModel);

    commandManager.ensureDependenciesInjected();

    secretsManager.setMasterPasswordHash(appSettings.value(Constants::MASTER_PASSWORD_HASH, "").toString());
    uploadInfoRepository.initFromString(appSettings.value(Constants::UPLOAD_HOSTS, "").toString());
    recentDirectorieModel.deserializeFromSettings(appSettings.value(Constants::RECENT_DIRECTORIES, "").toString());

    commandManager.connectEntitiesSignalsSlots();

    int result = 0;

    QVector<IntegrationTestBase*> integrationTests;

    integrationTests.append(new AddFilesBasicTest(&commandManager));

    foreach (IntegrationTestBase *test, integrationTests) {
        try {
            qInfo("Running test: %s", test->testName().toStdString().c_str());
            test->setup();
            result += test->doTest();
            test->teardown();
        }
        catch (...) {
            qWarning() << "Test crashed!";
            result += 1;
        }
    }

    qDeleteAll(integrationTests);

    return result;
}

