TEMPLATE = app

QT += qml quick widgets

SOURCES += main.cpp \
    Models/keywordsrepository.cpp \
    Models/imagemetadata.cpp \
    Models/keywordsfile.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    Models/keywordsrepository.h \
    Models/imagemetadata.h \
    Models/keywordsfile.h
