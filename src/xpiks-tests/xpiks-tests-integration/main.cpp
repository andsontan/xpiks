#include <iostream>
#include <QDebug>
#include <QCoreApplication>

#include "../../xpiks-qt/SpellCheck/spellchecksuggestionmodel.h"
#include "../../xpiks-qt/Models/filteredartitemsproxymodel.h"
#include "../../xpiks-qt/QMLExtensions/imagecachingservice.h"
#include "../../xpiks-qt/MetadataIO/metadataiocoordinator.h"
#include "../../xpiks-qt/AutoComplete/autocompleteservice.h"
#include "../../xpiks-qt/Models/deletekeywordsviewmodel.h"
#include "../../xpiks-qt/Conectivity/analyticsuserevent.h"
#include "../../xpiks-qt/SpellCheck/spellcheckerservice.h"
#include "../../xpiks-qt/AutoComplete/autocompletemodel.h"
#include "../../xpiks-qt/Models/recentdirectoriesmodel.h"
#include "../../xpiks-qt/MetadataIO/backupsaverservice.h"
#include "../../xpiks-qt/Suggestion/keywordssuggestor.h"
#include "../../xpiks-qt/Models/combinedartworksmodel.h"
#include "../../xpiks-qt/Conectivity/telemetryservice.h"
#include "../../xpiks-qt/Helpers/globalimageprovider.h"
#include "../../xpiks-qt/Models/uploadinforepository.h"
#include "../../xpiks-qt/Conectivity/ftpcoordinator.h"
#include "../../xpiks-qt/Models/findandreplacemodel.h"
#include "../../xpiks-qt/Conectivity/curlinithelper.h"
#include "../../xpiks-qt/MetadataIO/exiv2inithelper.h"
#include "../../xpiks-qt/Helpers/helpersqmlwrapper.h"
#include "../../xpiks-qt/Conectivity/updateservice.h"
#include "../../xpiks-qt/Encryption/secretsmanager.h"
#include "../../xpiks-qt/Models/artworksrepository.h"
#include "../../xpiks-qt/QMLExtensions/colorsmodel.h"
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
#include "../../xpiks-qt/PresetKeywords/presetkeywordsmodel.h"
#include "../../xpiks-qt/PresetKeywords/presetkeywordsmodelconfig.h"

#include "integrationtestbase.h"
#include "addfilesbasictest.h"
#include "autoattachvectorstest.h"
#include "savefilebasictest.h"
#include "spellcheckmultireplacetest.h"
#include "spellcheckcombinedmodeltest.h"
#include "zipartworkstest.h"
#include "spellcheckundotest.h"
#include "autocompletebasictest.h"
#include "spellingproduceswarningstest.h"
#include "undoaddwithvectorstest.h"
#include "readlegacysavedtest.h"
#include "clearmetadatatest.h"
#include "savewithemptytitletest.h"
#include "jsonmerge_tests.h"
#include "combinededitfixspellingtest.h"
#include "findandreplacemodeltest.h"
#include "addtouserdictionarytest.h"
#include "autodetachvectortest.h"
#include "removefromuserdictionarytest.h"
#include "artworkuploaderbasictest.h"
#include "plaintextedittest.h"
#include "fixspellingmarksmodifiedtest.h"
#include "preset_tests.h"

#if defined(WITH_LOGS)
#undef WITH_LOGS
#endif

#if defined(WITH_PLUGINS)
#undef WITH_PLUGINS
#endif

int main(int argc, char *argv[]) {
    std::cout << "Started integration tests" << std::endl;

    // will call curl_global_init and cleanup
    Conectivity::CurlInitHelper curlInitHelper;
    Q_UNUSED(curlInitHelper);

    MetadataIO::Exiv2InitHelper exiv2InitHelper;
    Q_UNUSED(exiv2InitHelper);

    QCoreApplication app(argc, argv);

    std::cout << "Initialized application" << std::endl;

    qSetMessagePattern("%{time hh:mm:ss.zzz} %{type} T#%{threadid} %{function} - %{message}");
    qRegisterMetaType<Common::SpellCheckFlags>("Common::SpellCheckFlags");

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
        std::cerr << "AppDataPath is empty!" << std::endl;
    }

    QMLExtensions::ColorsModel colorsModel;
    Models::LogsModel logsModel(&colorsModel);
    logsModel.startLogging();

    localLibrary.loadLibraryAsync();

    Models::ArtworksRepository artworkRepository;
    Models::ArtItemsModel artItemsModel;
    Models::CombinedArtworksModel combinedArtworksModel;
    Models::UploadInfoRepository uploadInfoRepository;
    KeywordsPreset::PresetKeywordsModel presetsModel;
    KeywordsPreset::PresetKeywordsModelConfig presetsModelConfig;
    Warnings::WarningsService warningsService;
    Models::SettingsModel settingsModel;
    settingsModel.readAllValues();
    Encryption::SecretsManager secretsManager;
    UndoRedo::UndoRedoManager undoRedoManager;
    Models::ZipArchiver zipArchiver;
    Suggestion::KeywordsSuggestor keywordsSuggestor(&localLibrary);
    Models::FilteredArtItemsProxyModel filteredArtItemsModel;
    filteredArtItemsModel.setSourceModel(&artItemsModel);
    Models::RecentDirectoriesModel recentDirectorieModel;
    Conectivity::FtpCoordinator *ftpCoordinator = new Conectivity::FtpCoordinator(settingsModel.getMaxParallelUploads());
    Models::ArtworkUploader artworkUploader(ftpCoordinator);
    SpellCheck::SpellCheckerService spellCheckerService;
    SpellCheck::SpellCheckSuggestionModel spellCheckSuggestionModel;
    MetadataIO::BackupSaverService metadataSaverService;
    Warnings::WarningsModel warningsModel;
    warningsModel.setSourceModel(&artItemsModel);
    Models::LanguagesModel languagesModel;
    AutoComplete::AutoCompleteModel autoCompleteModel;
    AutoComplete::AutoCompleteService autoCompleteService(&autoCompleteModel);
    QMLExtensions::ImageCachingService imageCachingService;
    Models::FindAndReplaceModel findAndReplaceModel(&colorsModel);
    Models::DeleteKeywordsViewModel deleteKeywordsModel;

    Conectivity::UpdateService updateService(&settingsModel);

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
    commandManager.InjectDependency(&colorsModel);
    commandManager.InjectDependency(&autoCompleteService);
    commandManager.InjectDependency(&imageCachingService);
    commandManager.InjectDependency(&findAndReplaceModel);
    commandManager.InjectDependency(&deleteKeywordsModel);
    commandManager.InjectDependency(&presetsModel);
    commandManager.InjectDependency(&presetsModelConfig);

    commandManager.ensureDependenciesInjected();

    keywordsSuggestor.initSuggestionEngines();

    secretsManager.setMasterPasswordHash(appSettings.value(Constants::MASTER_PASSWORD_HASH, "").toString());
    uploadInfoRepository.initFromString(appSettings.value(Constants::UPLOAD_HOSTS, "").toString());
    recentDirectorieModel.deserializeFromSettings(appSettings.value(Constants::RECENT_DIRECTORIES, "").toString());

#if defined(APPVEYOR)
    settingsModel.setExifToolPath("c:/projects/xpiks-deps/windows-3rd-party-bin/exiftool.exe");
#endif

    commandManager.connectEntitiesSignalsSlots();    
    commandManager.afterConstructionCallback();

    int result = 0;

    QVector<IntegrationTestBase*> integrationTests;

    integrationTests.append(new AddFilesBasicTest(&commandManager));
    integrationTests.append(new AutoAttachVectorsTest(&commandManager));
    integrationTests.append(new SaveFileBasicTest(&commandManager));
    integrationTests.append(new SpellCheckMultireplaceTest(&commandManager));
    integrationTests.append(new SpellCheckCombinedModelTest(&commandManager));
    integrationTests.append(new ZipArtworksTest(&commandManager));
    integrationTests.append(new SpellCheckUndoTest(&commandManager));
    integrationTests.append(new AutoCompleteBasicTest(&commandManager));
    integrationTests.append(new SpellingProducesWarningsTest(&commandManager));
    integrationTests.append(new UndoAddWithVectorsTest(&commandManager));
    integrationTests.append(new ReadLegacySavedTest(&commandManager));
    integrationTests.append(new ClearMetadataTest(&commandManager));
    integrationTests.append(new SaveWithEmptyTitleTest(&commandManager));
    integrationTests.append(new JsonMergeTests(&commandManager));
    integrationTests.append(new CombinedEditFixSpellingTest(&commandManager));
    integrationTests.append(new FindAndReplaceModelTest(&commandManager));
    integrationTests.append(new AddToUserDictionaryTest(&commandManager));
    integrationTests.append(new AutoDetachVectorTest(&commandManager));
    integrationTests.append(new RemoveFromUserDictionaryTest(&commandManager));
    integrationTests.append(new ArtworkUploaderBasicTest(&commandManager));
    integrationTests.append(new PlainTextEditTest(&commandManager));
    integrationTests.append(new FixSpellingMarksModifiedTest(&commandManager));
    integrationTests.append(new PresetTests(&commandManager));


    qDebug("\n");
    int succeededTestsCount = 0, failedTestsCount = 0;
    QStringList failedTests;

    foreach (IntegrationTestBase *test, integrationTests) {
        QThread::msleep(500);

        qDebug("---------------------------------------------------------");
        qInfo("Running test: %s", test->testName().toStdString().c_str());

        try {
            test->setup();
            int testResult = test->doTest();
            test->teardown();

            result += testResult;
            if (testResult == 0) {
                succeededTestsCount++;
                qInfo("Test %s PASSED", test->testName().toStdString().c_str());
            } else {
                failedTestsCount++;
                qInfo("Test %s FAILED", test->testName().toStdString().c_str());
                failedTests.append(test->testName());
            }
        }
        catch (...) {
            qInfo("Test %s CRASHED", test->testName().toStdString().c_str());
            result += 1;
        }

        qDebug("\n");
    }

    qDeleteAll(integrationTests);

    commandManager.beforeDestructionCallback();

    qInfo() << "--------------------------";
    qInfo() << "Integration Tests Results:" << succeededTestsCount << "succeeded," << failedTestsCount << "failed";

    if (!failedTests.empty()) {
        qInfo() << "FAILED TESTS:" << failedTests.join(", ");
    }

    qInfo() << "--------------------------";

    // for the logs to appear
    app.processEvents();

    QThread::sleep(1);

    return result;
}

