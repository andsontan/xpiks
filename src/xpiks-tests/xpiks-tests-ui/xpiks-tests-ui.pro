TEMPLATE = app

QT += qml quick
CONFIG += qmltestcase
TARGET = xpiks-tests-ui

SOURCES += main.cpp

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

DISTFILES += \
    ../../xpiks-qt/Components/EditableTags.qml \
    tst_EditableTags.qml \
    tst_CombinedEdit.qml \
    ../../xpiks-qt/Dialogs/CombinedArtworksDialog.qml

HEADERS += \
    ../../xpiks-qt/Helpers/clipboardhelper.h
