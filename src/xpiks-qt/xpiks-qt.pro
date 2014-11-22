TEMPLATE = app

QT += qml quick widgets

SOURCES += main.cpp \
    Models/imagemetadata.cpp \
    Models/artitemsmodel.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    Models/imagemetadata.h \
    Models/artitemsmodel.h
