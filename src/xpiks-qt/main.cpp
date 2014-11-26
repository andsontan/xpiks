#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtDebug>
#include <QQmlContext>
#include <QFile>
#include <QTextStream>
#include "Models/artitemsmodel.h"

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

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    qInstallMessageHandler(myMessageHandler);

    Models::ArtItemsModel mainModel;
    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("mainModel", &mainModel);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
