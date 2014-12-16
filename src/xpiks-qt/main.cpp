#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtDebug>
#include <QQmlContext>
#include <QtQml>
#include <QFile>
#include <QTextStream>
#include "Helpers/globalimageprovider.h"
#include "Helpers/clipboardhelper.h"
#include "Models/artitemsmodel.h"

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

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

#ifdef QT_NO_DEBUG
    qInstallMessageHandler(myMessageHandler);
#endif

    Models::ArtworksRepository artworkRepository;
    Models::ArtItemsModel artItemsModel;
    Models::CombinedArtworksModel combinedArtworksModel;

    artItemsModel.setArtworksRepository(&artworkRepository);
    artItemsModel.setCombinedArtworksModel(&combinedArtworksModel);

    qmlRegisterType<Helpers::ClipboardHelper>("xpiks", 1, 0, "ClipboardHelper");

    QQmlApplicationEngine engine;
    Helpers::GlobalImageProvider *globalProvider = new Helpers::GlobalImageProvider(QQmlImageProviderBase::Image);

    engine.rootContext()->setContextProperty("artItemsModel", &artItemsModel);
    engine.rootContext()->setContextProperty("artworkRepository", &artworkRepository);
    engine.rootContext()->setContextProperty("combinedArtworks", &combinedArtworksModel);
    engine.addImageProvider("global", globalProvider);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
