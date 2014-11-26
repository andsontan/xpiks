TEMPLATE = app

QT += qml quick widgets

SOURCES += main.cpp \
    Models/artitemsmodel.cpp \
    Models/keywordsmodel.cpp \
    Models/artworkmetadata.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    Models/artitemsmodel.h \
    Models/keywordsmodel.h \
    Models/artworkmetadata.h
