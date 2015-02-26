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

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtDebug>
#include <QQmlContext>
#include <QtQml>
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QSettings>
#include <QTextStream>
#include <QTranslator>
#include <QStandardPaths>
#include "Helpers/globalimageprovider.h"
#include "Models/uploadinforepository.h"
#include "Helpers/clipboardhelper.h"
#include "Models/artworkuploader.h"
#include "Models/warningsmanager.h"
#include "Models/artitemsmodel.h"
#include "Models/iptcprovider.h"
#include "Models/logsmodel.h"
#include "Helpers/appsettings.h"
#include "Helpers/constants.h"
#include "Helpers/logger.h"

#ifdef QT_NO_DEBUG

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
            .arg(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz"))
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
    QCoreApplication::setApplicationVersion("1.0 beta.6 - " + appVersion.left(10));
}

int main(int argc, char *argv[]) {    
    initQSettings();

    Helpers::LogsManager logsManager;

    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    if (!appDataPath.isEmpty()) {
        QDir logFileDir(appDataPath);
        QString logFilePath = logFileDir.filePath(Constants::LOG_FILENAME);

        logsManager.initLogger(logFilePath);
    }

#ifdef QT_NO_DEBUG
    QString logFileDir = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
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
    Models::ArtworkUploader artworkUploader;
    Models::UploadInfoRepository uploadInfoRepository;
    Models::LogsModel logsModel;
    Models::WarningsManager warningsManager;
    Helpers::AppSettings appSettings;
    Encryption::SecretsManager secretsManager;

    // injecting dependencies
    artworkUploader.setUploadInfoRepository(&uploadInfoRepository);
    artworkUploader.setSecretsManager(&secretsManager);
    artItemsModel.setArtworksRepository(&artworkRepository);
    artItemsModel.setCombinedArtworksModel(&combinedArtworksModel);
    artItemsModel.setIptcProvider(&iptcProvider);
    artItemsModel.setArtworkUploader(&artworkUploader);
    uploadInfoRepository.setSecretsManager(&secretsManager);
    logsModel.setLogsManager(&logsManager);

    // other initializations
    secretsManager.setMasterPasswordHash(appSettings.value(Constants::MASTER_PASSWORD_HASH, "").toString());
    uploadInfoRepository.initFromString(appSettings.value(Constants::UPLOAD_HOSTS, "").toString());

    QObject::connect(&secretsManager, SIGNAL(beforeMasterPasswordChange(QString,QString)),
                     &uploadInfoRepository, SLOT(onBeforeMasterPasswordChanged(QString,QString)));

    QObject::connect(&artItemsModel, SIGNAL(needCheckItemsForWarnings(QList<ArtworkMetadata*>)),
                     &warningsManager, SLOT(onCheckWarnings(QList<ArtworkMetadata*>)));

    qmlRegisterType<Helpers::ClipboardHelper>("xpiks", 1, 0, "ClipboardHelper");

    QQmlApplicationEngine engine;
    Helpers::GlobalImageProvider *globalProvider = new Helpers::GlobalImageProvider(QQmlImageProviderBase::Image);

    warningsManager.setImageProvider(globalProvider);

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

    engine.addImageProvider("global", globalProvider);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
