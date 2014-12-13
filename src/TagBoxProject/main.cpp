#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include "clipboardhelper.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<ClipboardHelper>("TagBoxProject", 1, 0, "ClipboardHelper");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
