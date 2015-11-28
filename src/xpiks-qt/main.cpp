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

#include <iostream>

#include <QDir>
#include <QtQml>
#include <QFile>
#include <QtDebug>
#include <QDateTime>
#include <QSettings>
#include <QTextStream>
#include <QTranslator>
#include <QQmlContext>
#include <QApplication>
#include <QStandardPaths>
#include <QQmlApplicationEngine>
//-------------------------------------
#include "SpellCheck/spellchecksuggestionmodel.h"
#include "Models/filteredartitemsproxymodel.h"
#include "Suggestion/suggestionqueryengine.h"
#include "SpellCheck/spellcheckerservice.h"
#include "Models/recentdirectoriesmodel.h"
#include "Suggestion/keywordssuggestor.h"
#include "Models/combinedartworksmodel.h"
#include "Helpers/globalimageprovider.h"
#include "Models/uploadinforepository.h"
#include "Helpers/helpersqmlwrapper.h"
#include "Encryption/secretsmanager.h"
#include "Models/artworksrepository.h"
#include "Helpers/settingsprovider.h"
#include "UndoRedo/undoredomanager.h"
#include "Helpers/clipboardhelper.h"
#include "Commands/commandmanager.h"
#include "Suggestion/locallibrary.h"
#include "Models/artworkuploader.h"
#include "Models/warningsmanager.h"
#include "Helpers/loggingworker.h"
#include "Helpers/updateservice.h"
#include "Models/artitemsmodel.h"
#include "Models/settingsmodel.h"
#include "Models/iptcprovider.h"
#include "Helpers/appsettings.h"
#include "Models/ziparchiver.h"
#include "Helpers/constants.h"
#include "Helpers/runguard.h"
#include "Models/logsmodel.h"
#include "Helpers/logger.h"
#include "Common/version.h"
#include "Common/defines.h"

#ifdef WITH_LOGS

void myMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    Q_UNUSED(context);

    QString txt;
    switch (type) {
    case QtDebugMsg:
        txt = QString("Debug: %1").arg(msg);
        break;
    case QtWarningMsg:
        txt = QString("Warning: %1").arg(msg);
        break;
    case QtCriticalMsg:
        txt = QString("Critical: %1").arg(msg);
        break;
    case QtFatalMsg:
        txt = QString("Fatal: %1").arg(msg);
        break;
    }

    QString logLine = QString("%1 - %2")
            .arg(QDateTime::currentDateTimeUtc().toString("dd.MM.yyyy hh:mm:ss.zzz"))
            .arg(txt);

    Helpers::Logger &logger = Helpers::Logger::getInstance();
    logger.log(logLine);

    if (type == QtFatalMsg) {
        abort();
    }
}

#endif

#define STRINGIZE_(x) #x
#define STRINGIZE(x) STRINGIZE_(x)

void initQSettings() {
    QCoreApplication::setOrganizationName(Constants::ORGANIZATION_NAME);
    QCoreApplication::setOrganizationDomain(Constants::ORGANIZATION_DOMAIN);
    QCoreApplication::setApplicationName(Constants::APPLICATION_NAME);
    QString appVersion(STRINGIZE(BUILDNUMBER));
    QCoreApplication::setApplicationVersion(STRINGIZE(XPIKS_VERSION)" "STRINGIZE(XPIKS_VERSION_SUFFIX)" - " + appVersion.left(10));
}

int main(int argc, char *argv[]) {
    Helpers::RunGuard guard("xpiks");
    if (!guard.tryToRun()) {
        std::cerr << "Xpiks is already running";
        return -1;
    }

    initQSettings();

    Suggestion::LocalLibrary localLibrary;

    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    if (!appDataPath.isEmpty()) {
        QDir appDataDir(appDataPath);

        QString logFilePath = appDataDir.filePath(Constants::LOG_FILENAME);
        Helpers::Logger &logger = Helpers::Logger::getInstance();
        logger.setLogFilePath(logFilePath);

        QString libraryFilePath = appDataDir.filePath(Constants::LIBRARY_FILENAME);
        localLibrary.setLibraryPath(libraryFilePath);
    }

    Models::LogsModel logsModel;
    logsModel.startLogging();

#ifdef WITH_LOGS
    QString logFileDir = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    if (!logFileDir.isEmpty()) {
        QDir dir(logFileDir);
        if (!dir.exists()) {
            bool created = QDir().mkpath(logFileDir);
            Q_UNUSED(created);
        }
    }

    qInstallMessageHandler(myMessageHandler);
    qDebug() << "Log started";
#endif
    QApplication app(argc, argv);

    localLibrary.loadLibraryAsync();

    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),
                      QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&qtTranslator);

    QTranslator myappTranslator;
    myappTranslator.load("xpiks_" + QLocale::system().name());
    app.installTranslator(&myappTranslator);

    Models::ArtworksRepository artworkRepository;
    Models::ArtItemsModel artItemsModel;
    Models::CombinedArtworksModel combinedArtworksModel;
    Models::IptcProvider iptcProvider;
    iptcProvider.setLocalLibrary(&localLibrary);
    Models::UploadInfoRepository uploadInfoRepository;
    Models::WarningsManager warningsManager;
    Helpers::AppSettings appSettings;
    Models::SettingsModel settingsModel;
    Encryption::SecretsManager secretsManager;
    UndoRedo::UndoRedoManager undoRedoManager;
    Models::ZipArchiver zipArchiver;
    Suggestion::KeywordsSuggestor keywordsSuggestor;
    keywordsSuggestor.setLocalLibrary(&localLibrary);
    Models::FilteredArtItemsProxyModel filteredArtItemsModel;
    filteredArtItemsModel.setSourceModel(&artItemsModel);
    Models::RecentDirectoriesModel recentDirectorieModel;
    Models::ArtworkUploader artworkUploader(settingsModel.getMaxParallelUploads());
    SpellCheck::SpellCheckerService spellCheckerService;
    SpellCheck::SpellCheckSuggestionModel spellCheckSuggestionModel;

    Commands::CommandManager commandManager;
    commandManager.InjectDependency(&artworkRepository);
    commandManager.InjectDependency(&artItemsModel);
    commandManager.InjectDependency(&filteredArtItemsModel);
    commandManager.InjectDependency(&combinedArtworksModel);
    commandManager.InjectDependency(&iptcProvider);
    commandManager.InjectDependency(&artworkUploader);
    commandManager.InjectDependency(&uploadInfoRepository);
    commandManager.InjectDependency(&warningsManager);
    commandManager.InjectDependency(&secretsManager);
    commandManager.InjectDependency(&undoRedoManager);
    commandManager.InjectDependency(&zipArchiver);
    commandManager.InjectDependency(&keywordsSuggestor);
    commandManager.InjectDependency(&settingsModel);
    commandManager.InjectDependency(&recentDirectorieModel);
    commandManager.InjectDependency(&spellCheckerService);
    commandManager.InjectDependency(&spellCheckSuggestionModel);

    // other initializations
    secretsManager.setMasterPasswordHash(appSettings.value(Constants::MASTER_PASSWORD_HASH, "").toString());
    uploadInfoRepository.initFromString(appSettings.value(Constants::UPLOAD_HOSTS, "").toString());
    recentDirectorieModel.deserializeFromSettings(appSettings.value(Constants::RECENT_DIRECTORIES, "").toString());

    Helpers::SettingsProvider::getInstance().setSettingsModelInstance(&settingsModel);

    commandManager.connectEntitiesSignalsSlots();

    qmlRegisterType<Helpers::ClipboardHelper>("xpiks", 1, 0, "ClipboardHelper");

    QQmlApplicationEngine engine;
    Helpers::GlobalImageProvider *globalProvider = new Helpers::GlobalImageProvider(QQmlImageProviderBase::Image);

    warningsManager.setImageProvider(globalProvider);

    Helpers::HelpersQmlWrapper helpersQmlWrapper;
    Helpers::UpdateService updateService;

    QQmlContext *rootContext = engine.rootContext();
    rootContext->setContextProperty("artItemsModel", &artItemsModel);
    rootContext->setContextProperty("artworkRepository", &artworkRepository);
    rootContext->setContextProperty("combinedArtworks", &combinedArtworksModel);
    rootContext->setContextProperty("appSettings", &appSettings);
    rootContext->setContextProperty("iptcProvider", &iptcProvider);
    rootContext->setContextProperty("artworkUploader", &artworkUploader);
    rootContext->setContextProperty("uploadInfos", &uploadInfoRepository);
    rootContext->setContextProperty("logsModel", &logsModel);
    rootContext->setContextProperty("warningsManager", &warningsManager);
    rootContext->setContextProperty("secretsManager", &secretsManager);
    rootContext->setContextProperty("undoRedoManager", &undoRedoManager);
    rootContext->setContextProperty("zipArchiver", &zipArchiver);
    rootContext->setContextProperty("keywordsSuggestor", &keywordsSuggestor);
    rootContext->setContextProperty("settingsModel", &settingsModel);
    rootContext->setContextProperty("filteredArtItemsModel", &filteredArtItemsModel);
    rootContext->setContextProperty("helpersWrapper", &helpersQmlWrapper);
    rootContext->setContextProperty("recentDirectories", &recentDirectorieModel);
    rootContext->setContextProperty("updateService", &updateService);
    rootContext->setContextProperty("spellCheckerService", &spellCheckerService);
    rootContext->setContextProperty("spellCheckSuggestionModel", &spellCheckSuggestionModel);

    engine.addImageProvider("global", globalProvider);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    spellCheckerService.startChecking();

#ifdef QT_DEBUG
    if (argc > 1) {
        QStringList pathes;
        for (int i = 1; i < argc; ++i) {
            pathes.append(QString(argv[i]));
        }
        commandManager.addInitialArtworks(pathes);
    }
#endif

    updateService.checkForUpdates();


    return app.exec();
}
