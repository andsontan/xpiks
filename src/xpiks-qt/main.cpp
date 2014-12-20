#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtDebug>
#include <QQmlContext>
#include <QtQml>
#include <QFile>
#include <QSettings>
#include <QTextStream>
#include "Helpers/globalimageprovider.h"
#include "Helpers/clipboardhelper.h"
#include "Models/artworkuploader.h"
#include "Models/artitemsmodel.h"
#include "Models/iptcprovider.h"
#include "Helpers/appsettings.h"
#include "Helpers/constants.h"

#ifdef QT_NO_DEBUG

void myMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
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

    QFile outFile("xpiks.log");
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << txt << endl;

    if (type == QtFatalMsg) {
        abort();
    }
}

#endif

void initQSettings() {
    QCoreApplication::setOrganizationName(Constants::ORGANIZATION_NAME);
    QCoreApplication::setOrganizationDomain(Constants::ORGANIZATION_DOMAIN);
    QCoreApplication::setApplicationName(Constants::APPLICATION_NAME);
}

int main(int argc, char *argv[]) {
    initQSettings();

    QApplication app(argc, argv);

#ifdef QT_NO_DEBUG
    qInstallMessageHandler(myMessageHandler);
#endif

    Models::ArtworksRepository artworkRepository;
    Models::ArtItemsModel artItemsModel;
    Models::CombinedArtworksModel combinedArtworksModel;
    Models::IptcProvider iptcProvider;
    Models::ArtworkUploader artworkUploader;

    artItemsModel.setArtworksRepository(&artworkRepository);
    artItemsModel.setCombinedArtworksModel(&combinedArtworksModel);
    artItemsModel.setIptcProvider(&iptcProvider);
    artItemsModel.setArtworkUploader(&artworkUploader);

    Helpers::AppSettings appSettings;

    qmlRegisterType<Helpers::ClipboardHelper>("xpiks", 1, 0, "ClipboardHelper");

    QQmlApplicationEngine engine;
    Helpers::GlobalImageProvider *globalProvider = new Helpers::GlobalImageProvider(QQmlImageProviderBase::Image);

    QQmlContext *rootContext = engine.rootContext();
    rootContext->setContextProperty("artItemsModel", &artItemsModel);
    rootContext->setContextProperty("artworkRepository", &artworkRepository);
    rootContext->setContextProperty("combinedArtworks", &combinedArtworksModel);
    rootContext->setContextProperty("appSettings", &appSettings);
    rootContext->setContextProperty("iptcProvider", &iptcProvider);
    rootContext->setContextProperty("artworkUploader", &artworkUploader);

    engine.addImageProvider("global", globalProvider);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
