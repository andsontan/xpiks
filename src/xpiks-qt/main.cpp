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

#include <iostream>

#include <QUrl>
#include <QDir>
#include <QtQml>
#include <QFile>
#include <QUuid>
#include <QScreen>
#include <QtDebug>
#include <QDateTime>
#include <QSettings>
#include <QTextStream>
#include <QQmlContext>
#include <QApplication>
#include <QQuickWindow>
#include <QStandardPaths>
#include <QQmlApplicationEngine>
#include <QDesktopWidget>
// -------------------------------------
#include "SpellCheck/spellchecksuggestionmodel.h"
#include "SpellCheck/userdicteditmodel.h"
#include "QMLExtensions/cachingimageprovider.h"
#include "Models/filteredartitemsproxymodel.h"
#include "QMLExtensions/imagecachingservice.h"
#include "MetadataIO/metadataiocoordinator.h"
#include "AutoComplete/autocompleteservice.h"
#include "Conectivity/analyticsuserevent.h"
#include "SpellCheck/spellcheckerservice.h"
#include "AutoComplete/autocompletemodel.h"
#include "Models/deletekeywordsviewmodel.h"
#include "Translation/translationmanager.h"
#include "Translation/translationservice.h"
#include "Models/recentdirectoriesmodel.h"
#include "Models/recentfilesmodel.h"
#include "MetadataIO/backupsaverservice.h"
#include "QMLExtensions/triangleelement.h"
#include "Suggestion/keywordssuggestor.h"
#include "Models/combinedartworksmodel.h"
#include "Conectivity/telemetryservice.h"
#include "QMLExtensions/folderelement.h"
#include "Helpers/globalimageprovider.h"
#include "Models/uploadinforepository.h"
#include "Conectivity/ftpcoordinator.h"
#include "Conectivity/curlinithelper.h"
#include "Helpers/helpersqmlwrapper.h"
#include "Encryption/secretsmanager.h"
#include "Models/artworksrepository.h"
#include "QMLExtensions/colorsmodel.h"
#include "Conectivity/updateservice.h"
#include "Models/artworkproxymodel.h"
#include "Warnings/warningsservice.h"
#include "UndoRedo/undoredomanager.h"
#include "QuickBuffer/quickbuffer.h"
#include "Helpers/clipboardhelper.h"
#include "Commands/commandmanager.h"
#include "Suggestion/locallibrary.h"
#include "QMLExtensions/tabsmodel.h"
#include "Models/artworkuploader.h"
#include "Warnings/warningsmodel.h"
#include "Plugins/pluginmanager.h"
#include "Helpers/loggingworker.h"
#include "Models/languagesmodel.h"
#include "Models/artitemsmodel.h"
#include "Models/settingsmodel.h"
#include "Models/ziparchiver.h"
#include "Helpers/constants.h"
#include "Helpers/runguard.h"
#include "Models/logsmodel.h"
#include "Models/uimanager.h"
#include "Helpers/logger.h"
#include "Common/version.h"
#include "Common/defines.h"
#include "Models/proxysettings.h"
#include "MetadataIO/exiv2inithelper.h"
#include "Models/findandreplacemodel.h"
#include "Models/previewmetadataelement.h"
#include "KeywordsPresets/presetkeywordsmodel.h"
#include "KeywordsPresets/presetkeywordsmodelconfig.h"

void myMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    Q_UNUSED(context);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
    QString logLine = qFormatLogMessage(type, context, msg);
#else
    QString msgType;
    switch (type) {
        case QtDebugMsg:
            msgType = "debug";
            break;
        case QtWarningMsg:
            msgType = "warning";
            break;
        case QtCriticalMsg:
            msgType = "critical";
            break;
        case QtFatalMsg:
            msgType = "fatal";
            break;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 5, 1))
        case QtInfoMsg:
            msgType = "info";
            break;
#endif
    }

    // %{time hh:mm:ss.zzz} %{type} T#%{threadid} %{function} - %{message}
    QString time = QDateTime::currentDateTimeUtc().toString("hh:mm:ss.zzz");
    QString logLine = QString("%1 %2 T#%3 %4 - %5")
                          .arg(time).arg(msgType)
                          .arg(0).arg(context.function)
                          .arg(msg);
#endif

    Helpers::Logger &logger = Helpers::Logger::getInstance();
    logger.log(logLine);

    if (type == QtFatalMsg) {
        abort();
    }
}

#define STRINGIZE_(x) #x
#define STRINGIZE(x) STRINGIZE_(x)

void initQSettings() {
    QCoreApplication::setOrganizationName(Constants::ORGANIZATION_NAME);
    QCoreApplication::setOrganizationDomain(Constants::ORGANIZATION_DOMAIN);
    QCoreApplication::setApplicationName(Constants::APPLICATION_NAME);
    QString appVersion(STRINGIZE(BUILDNUMBER));
    QCoreApplication::setApplicationVersion(XPIKS_VERSION_STRING " " STRINGIZE(XPIKS_VERSION_SUFFIX) " - " +
                                            appVersion.left(10));
}

void ensureUserIdExists(Models::SettingsModel *settings) {
    if (settings->getUserAgentId().isEmpty()) {
        QUuid uuid = QUuid::createUuid();
        settings->setUserAgentId(uuid.toString());
    }
}

static const char *setHighDpiEnvironmentVariable() {
    const char *envVarName = 0;
    static const char ENV_VAR_QT_DEVICE_PIXEL_RATIO[] = "QT_DEVICE_PIXEL_RATIO";

#ifdef Q_OS_WIN
    bool isWindows = true;
#else
    bool isWindows = false;
#endif

#if (QT_VERSION < QT_VERSION_CHECK(5, 6, 0))
    if (isWindows
        && !qEnvironmentVariableIsSet(ENV_VAR_QT_DEVICE_PIXEL_RATIO)) {
        envVarName = ENV_VAR_QT_DEVICE_PIXEL_RATIO;
        qputenv(envVarName, "auto");
    }

#else
    if (isWindows
        && !qEnvironmentVariableIsSet(ENV_VAR_QT_DEVICE_PIXEL_RATIO)     // legacy in 5.6, but still functional
        && !qEnvironmentVariableIsSet("QT_AUTO_SCREEN_SCALE_FACTOR")
        && !qEnvironmentVariableIsSet("QT_SCALE_FACTOR")
        && !qEnvironmentVariableIsSet("QT_SCREEN_SCALE_FACTORS")) {
        QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    }

#endif // < Qt 5.6
    return envVarName;
}

QString getRunGuardName() {
#ifdef QT_NO_DEBUG
    QString runGuardName = "xpiks_";
#else
    QString runGuardName = "xpiks-debug_";
#endif

    QString username = QString::fromLocal8Bit(qgetenv("USER"));
    if (username.isEmpty()) {
        username = QString::fromLocal8Bit(qgetenv("USERNAME"));
    }

    username.remove(QChar::Space);

    return (runGuardName + username);
}

int main(int argc, char *argv[]) {
    const QString runGuardName = getRunGuardName();
    Helpers::RunGuard guard(runGuardName);
    if (!guard.tryToRun()) {
        std::cerr << "Xpiks is already running";
        return -1;
    }

    // will call curl_global_init and cleanup
    Conectivity::CurlInitHelper curlInitHelper;
    Q_UNUSED(curlInitHelper);

    // will init thread-unsafe XMP toolkit
    MetadataIO::Exiv2InitHelper exiv2InitHelper;
    Q_UNUSED(exiv2InitHelper);

    const char *highDpiEnvironmentVariable = setHighDpiEnvironmentVariable();
    qRegisterMetaTypeStreamOperators<Models::ProxySettings>("ProxySettings");
    qRegisterMetaTypeStreamOperators<Suggestion::LocalArtworkData>("LocalArtworkData");
    qRegisterMetaType<Common::SpellCheckFlags>("Common::SpellCheckFlags");
    initQSettings();
    Models::SettingsModel settingsModel;
    settingsModel.initializeConfigs();
    ensureUserIdExists(&settingsModel);

    Suggestion::LocalLibrary localLibrary;

    QString appDataPath = XPIKS_USERDATA_PATH;
    if (!appDataPath.isEmpty()) {
        QDir appDataDir(appDataPath);

        QString libraryFilePath = appDataDir.filePath(Constants::LIBRARY_FILENAME);
        localLibrary.setLibraryPath(libraryFilePath);
    } else {
        std::cerr << "AppDataPath is empty!";
    }

#ifdef WITH_LOGS
    const QString &logFileDir = QDir::cleanPath(appDataPath + QDir::separator() + "logs");
    if (!logFileDir.isEmpty()) {
        QDir dir(logFileDir);
        if (!dir.exists()) {
            bool created = QDir().mkpath(logFileDir);
            Q_UNUSED(created);
        }

        QString time = QDateTime::currentDateTimeUtc().toString("ddMMyyyy-hhmmss-zzz");
        QString logFilename = QString("xpiks-qt-%1.log").arg(time);

        QString logFilePath = dir.filePath(logFilename);

        Helpers::Logger &logger = Helpers::Logger::getInstance();
        logger.setLogFilePath(logFilePath);
    }

#endif

    QMLExtensions::ColorsModel colorsModel;
    Models::LogsModel logsModel(&colorsModel);
    logsModel.startLogging();

#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
    qSetMessagePattern("%{time hh:mm:ss.zzz} %{type} T#%{threadid} %{function} - %{message}");
#endif

    qInstallMessageHandler(myMessageHandler);

    LOG_INFO << "Log started. Today is" << QDateTime::currentDateTimeUtc().toString("dd.MM.yyyy");
    LOG_INFO << "Xpiks" << XPIKS_VERSION_STRING << "-" << STRINGIZE(BUILDNUMBER);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
    LOG_INFO << QSysInfo::productType() << QSysInfo::productVersion() << QSysInfo::currentCpuArchitecture();
#else
#ifdef Q_OS_WIN
    LOG_INFO << QLatin1String("Windows Qt<5.4");
#elsif Q_OS_DARWIN
    LOG_INFO << QLatin1String("OS X Qt<5.4");
#else
    LOG_INFO << QLatin1String("LINUX Qt<5.4");
#endif
#endif

    QApplication app(argc, argv);

    LOG_INFO << "Working directory of Xpiks is:" << QDir::currentPath();
    LOG_DEBUG << "Extra files search locations:" << QStandardPaths::standardLocations(XPIKS_DATA_LOCATION_TYPE);

    if (highDpiEnvironmentVariable) {
        qunsetenv(highDpiEnvironmentVariable);
    }

    localLibrary.loadLibraryAsync();

    QString userId = settingsModel.getUserAgentId();
    userId.remove(QRegExp("[{}-]."));

    Models::ArtworksRepository artworkRepository;
    Models::ArtItemsModel artItemsModel;
    Models::CombinedArtworksModel combinedArtworksModel;
    Models::UploadInfoRepository uploadInfoRepository;
    KeywordsPresets::PresetKeywordsModel presetsModel;
    KeywordsPresets::PresetKeywordsModelConfig presetsModelConfig;
    KeywordsPresets::FilteredPresetKeywordsModel filteredPresetsModel;
    filteredPresetsModel.setSourceModel(&presetsModel);
    Warnings::WarningsService warningsService;
    settingsModel.readAllValues();
    Encryption::SecretsManager secretsManager;
    UndoRedo::UndoRedoManager undoRedoManager;
    Models::ZipArchiver zipArchiver;
    Suggestion::KeywordsSuggestor keywordsSuggestor(&localLibrary);
    Models::FilteredArtItemsProxyModel filteredArtItemsModel;
    filteredArtItemsModel.setSourceModel(&artItemsModel);
    Models::RecentDirectoriesModel recentDirectorieModel;
    Models::RecentFilesModel recentFileModel;
    Conectivity::FtpCoordinator *ftpCoordinator = new Conectivity::FtpCoordinator(settingsModel.getMaxParallelUploads());
    Models::ArtworkUploader artworkUploader(ftpCoordinator);
    SpellCheck::SpellCheckerService spellCheckerService(&settingsModel);
    SpellCheck::SpellCheckSuggestionModel spellCheckSuggestionModel;
    SpellCheck::UserDictEditModel userDictEditModel;
    MetadataIO::BackupSaverService metadataSaverService;
    Warnings::WarningsModel warningsModel;
    warningsModel.setSourceModel(&artItemsModel);
    warningsModel.setWarningsSettingsModel(warningsService.getWarningsSettingsModel());
    Models::LanguagesModel languagesModel;
    AutoComplete::AutoCompleteModel autoCompleteModel;
    AutoComplete::AutoCompleteService autoCompleteService(&autoCompleteModel);
    QMLExtensions::ImageCachingService imageCachingService;
    Models::FindAndReplaceModel replaceModel(&colorsModel);
    Models::DeleteKeywordsViewModel deleteKeywordsModel;
    Models::ArtworkProxyModel artworkProxyModel;
    Translation::TranslationManager translationManager;
    Translation::TranslationService translationService(translationManager);
    Models::UIManager uiManager;
    QuickBuffer::QuickBuffer quickBuffer;

    Conectivity::UpdateService updateService(&settingsModel);

    MetadataIO::MetadataIOCoordinator metadataIOCoordinator;

#if defined(QT_NO_DEBUG) && !defined(TELEMETRY_DISABLED)
    const bool telemetryEnabled = settingsModel.getIsTelemetryEnabled();
#else
    const bool telemetryEnabled = false;
#endif
    Conectivity::TelemetryService telemetryService(userId, telemetryEnabled);

    Plugins::PluginManager pluginManager;
    Plugins::PluginsWithActionsModel pluginsWithActions;
    pluginsWithActions.setSourceModel(&pluginManager);

    Helpers::HelpersQmlWrapper helpersQmlWrapper;

    LOG_INFO << "Models created";

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
    commandManager.InjectDependency(&recentFileModel);
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
    commandManager.InjectDependency(&replaceModel);
    commandManager.InjectDependency(&deleteKeywordsModel);
    commandManager.InjectDependency(&helpersQmlWrapper);
    commandManager.InjectDependency(&presetsModel);
    commandManager.InjectDependency(&presetsModelConfig);
    commandManager.InjectDependency(&translationManager);
    commandManager.InjectDependency(&translationService);
    commandManager.InjectDependency(&uiManager);
    commandManager.InjectDependency(&artworkProxyModel);
    commandManager.InjectDependency(&quickBuffer);

    userDictEditModel.setCommandManager(&commandManager);
    autoCompleteModel.setCommandManager(&commandManager);

    commandManager.ensureDependenciesInjected();

    keywordsSuggestor.initSuggestionEngines();

    // other initializations
    secretsManager.setMasterPasswordHash(settingsModel.getMasterPasswordHash());
    uploadInfoRepository.initFromString(settingsModel.getUploadHosts());
    recentDirectorieModel.deserializeFromSettings(settingsModel.getRecentDirectories());
    recentFileModel.deserializeFromSettings(settingsModel.getRecentFiles());

    commandManager.connectEntitiesSignalsSlots();

    languagesModel.initFirstLanguage();
    languagesModel.loadLanguages();

    telemetryService.setInterfaceLanguage(languagesModel.getCurrentLanguage());
    colorsModel.applyTheme(settingsModel.getSelectedThemeIndex());

    qmlRegisterType<Helpers::ClipboardHelper>("xpiks", 1, 0, "ClipboardHelper");
    qmlRegisterType<QMLExtensions::TriangleElement>("xpiks", 1, 0, "TriangleElement");
    qmlRegisterType<QMLExtensions::FolderElement>("xpiks", 1, 0, "FolderElement");

    QQmlApplicationEngine engine;
    Helpers::GlobalImageProvider *globalProvider = new Helpers::GlobalImageProvider(QQmlImageProviderBase::Image);
    QMLExtensions::CachingImageProvider *cachingProvider = new QMLExtensions::CachingImageProvider(QQmlImageProviderBase::Image);
    cachingProvider->setImageCachingService(&imageCachingService);

    QQmlContext *rootContext = engine.rootContext();
    rootContext->setContextProperty("artItemsModel", &artItemsModel);
    rootContext->setContextProperty("artworkRepository", &artworkRepository);
    rootContext->setContextProperty("combinedArtworks", &combinedArtworksModel);
    rootContext->setContextProperty("secretsManager", &secretsManager);
    rootContext->setContextProperty("undoRedoManager", &undoRedoManager);
    rootContext->setContextProperty("keywordsSuggestor", &keywordsSuggestor);
    rootContext->setContextProperty("settingsModel", &settingsModel);
    rootContext->setContextProperty("filteredArtItemsModel", &filteredArtItemsModel);
    rootContext->setContextProperty("helpersWrapper", &helpersQmlWrapper);
    rootContext->setContextProperty("recentDirectories", &recentDirectorieModel);
    rootContext->setContextProperty("recentFiles", &recentFileModel);
    rootContext->setContextProperty("metadataIOCoordinator", &metadataIOCoordinator);
    rootContext->setContextProperty("pluginManager", &pluginManager);
    rootContext->setContextProperty("pluginsWithActions", &pluginsWithActions);
    rootContext->setContextProperty("warningsModel", &warningsModel);
    rootContext->setContextProperty("languagesModel", &languagesModel);
    rootContext->setContextProperty("i18", &languagesModel);
    rootContext->setContextProperty("Colors", &colorsModel);
    rootContext->setContextProperty("acSource", &autoCompleteModel);
    rootContext->setContextProperty("replaceModel", &replaceModel);
    rootContext->setContextProperty("presetsModel", &presetsModel);
    rootContext->setContextProperty("filteredPresetsModel", &filteredPresetsModel);
    rootContext->setContextProperty("artworkProxy", &artworkProxyModel);
    rootContext->setContextProperty("translationManager", &translationManager);
    rootContext->setContextProperty("uiManager", &uiManager);
    rootContext->setContextProperty("quickBuffer", &quickBuffer);
    rootContext->setContextProperty("userDictEditModel", &userDictEditModel);

    rootContext->setContextProperty("tabsModel", uiManager.getTabsModel());
    rootContext->setContextProperty("activeTabs", uiManager.getActiveTabs());
    rootContext->setContextProperty("inactiveTabs", uiManager.getInactiveTabs());

#ifdef QT_DEBUG
    QVariant isDebug(true);
#else
    QVariant isDebug(false);
#endif
    rootContext->setContextProperty("debug", isDebug);
    rootContext->setContextProperty("debugTabs", isDebug);

    engine.addImageProvider("global", globalProvider);
    engine.addImageProvider("cached", cachingProvider);

    uiManager.addSystemTab("qrc:/CollapserTabs/FilesFoldersIcon.qml", "qrc:/CollapserTabs/FilesFoldersTab.qml");
    uiManager.addSystemTab("qrc:/CollapserTabs/QuickBufferIcon.qml", "qrc:/CollapserTabs/QuickBufferTab.qml");
    uiManager.addSystemTab("qrc:/CollapserTabs/TranslatorIcon.qml", "qrc:/CollapserTabs/TranslatorTab.qml");
    uiManager.initializeSystemTabs();

    LOG_DEBUG << "About to load main view...";
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    LOG_DEBUG << "Main view loaded";

    auto *uiProvider = pluginManager.getUIProvider();
    uiProvider->setQmlEngine(&engine);
    QQuickWindow *window = qobject_cast<QQuickWindow *>(engine.rootObjects().at(0));
    imageCachingService.setScale(window->effectiveDevicePixelRatio());

    QScreen *screen = window->screen();
    QObject::connect(window, SIGNAL(screenChanged(QScreen*)), &imageCachingService, SLOT(screenChangedHandler(QScreen*)));
    QObject::connect(screen, SIGNAL(logicalDotsPerInchChanged(qreal)), &imageCachingService, SLOT(dpiChanged(qreal)));
    QObject::connect(screen, SIGNAL(physicalDotsPerInchChanged(qreal)), &imageCachingService, SLOT(dpiChanged(qreal)));

    uiProvider->setRoot(window->contentItem());
    uiProvider->setUIManager(&uiManager);

#ifdef QT_DEBUG
    if (argc > 1) {
        QList<QUrl> pathes;
        for (int i = 1; i < argc; ++i) {
            pathes.append(QUrl::fromLocalFile(QString::fromUtf8(argv[i])));
        }

        commandManager.addInitialArtworks(pathes);
    }
#endif

    commandManager.afterConstructionCallback();

    return app.exec();
}
