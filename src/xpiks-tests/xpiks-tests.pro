#-------------------------------------------------
#
# Project created by QtCreator 2015-02-12T22:20:15
#
#-------------------------------------------------

QT       += core testlib qml quick concurrent

QT       -= gui

TARGET = xpiks-tests
CONFIG   += console
CONFIG   -= app_bundle

win32:INCLUDEPATH += ../tiny-aes
win32:INCLUDEPATH += ../xpiks-qt/Encryption

DEFINES += QT_NO_CAST_TO_ASCII \
           QT_NO_CAST_FROM_BYTEARRAY

TEMPLATE = app

SOURCES += main.cpp \
    encryption_tests.cpp \
    ../tiny-aes/aes.cpp \
    indicestoranges_tests.cpp \
    ../xpiks-qt/Helpers/indiceshelper.cpp \
    ../xpiks-qt/Commands/commandmanager.cpp \
    ../xpiks-qt/Models/artworkmetadata.cpp \
    ../xpiks-qt/Models/artworksrepository.cpp \
    addcommand_tests.cpp \
    ../xpiks-qt/Models/artitemsmodel.cpp \
    ../xpiks-qt/Commands/addartworkscommand.cpp \
    ../xpiks-qt/Models/artworksprocessor.cpp \
    ../xpiks-qt/Models/combinedartworksmodel.cpp \
    ../xpiks-qt/UndoRedo/addartworksitem.cpp \
    ../xpiks-qt/UndoRedo/undoredomanager.cpp \
    ../xpiks-qt/Helpers/tempmetadatadb.cpp \
    ../xpiks-qt/Encryption/secretsmanager.cpp \
    ../xpiks-qt/Commands/combinededitcommand.cpp \
    ../xpiks-qt/Commands/pastekeywordscommand.cpp \
    ../xpiks-qt/Commands/removeartworkscommand.cpp \
    ../xpiks-qt/UndoRedo/artworkmetadatabackup.cpp \
    ../xpiks-qt/UndoRedo/modifyartworkshistoryitem.cpp \
    ../xpiks-qt/UndoRedo/removeartworksitem.cpp \
    ../xpiks-qt/Encryption/aes-qt.cpp \
    removecommand_tests.cpp \
    vectorfilenames_tests.cpp \
    ../xpiks-qt/Helpers/filenameshelpers.cpp \
    ../xpiks-qt/Helpers/helpersqmlwrapper.cpp \
    ../xpiks-qt/Models/recentdirectoriesmodel.cpp \
    ../xpiks-qt/Helpers/keywordvalidator.cpp \
    ../xpiks-qt/Common/flags.cpp

HEADERS += \
    encryption_tests.h \
    ../tiny-aes/aes.h \
    ../xpiks-qt/Encryption/aes-qt.h \
    indicestoranges_tests.h \
    ../xpiks-qt/Helpers/indiceshelper.h \
    Mocks/commandmanagermock.h \
    ../xpiks-qt/Commands/commandmanager.h \
    ../xpiks-qt/Models/artworkmetadata.h \
    ../xpiks-qt/Models/artworksrepository.h \
    Mocks/artworksrepositorymock.h \
    addcommand_tests.h \
    ../xpiks-qt/Models/artitemsmodel.h \
    Mocks/artitemsmodelmock.h \
    ../xpiks-qt/Commands/addartworkscommand.h \
    ../xpiks-qt/Models/artworksprocessor.h \
    ../xpiks-qt/Models/combinedartworksmodel.h \
    ../xpiks-qt/UndoRedo/addartworksitem.h \
    ../xpiks-qt/UndoRedo/historyitem.h \
    ../xpiks-qt/UndoRedo/undoredomanager.h \
    ../xpiks-qt/Helpers/osxnsurlhelper.h \
    ../xpiks-qt/Helpers/tempmetadatadb.h \
    ../xpiks-qt/Encryption/secretsmanager.h \
    ../xpiks-qt/Commands/combinededitcommand.h \
    ../xpiks-qt/Commands/commandbase.h \
    ../xpiks-qt/Commands/pastekeywordscommand.h \
    ../xpiks-qt/Commands/removeartworkscommand.h \
    ../xpiks-qt/UndoRedo/artworkmetadatabackup.h \
    ../xpiks-qt/UndoRedo/modifyartworkshistoryitem.h \
    ../xpiks-qt/UndoRedo/removeartworksitem.h \
    Mocks/artworkmetadatamock.h \
    removecommand_tests.h \
    ../xpiks-qt/Common/basickeywordsmodel.h \
    vectorfilenames_tests.h \
    ../xpiks-qt/Helpers/filenameshelpers.h \
    ../xpiks-qt/Helpers/helpersqmlwrapper.h \
    ../xpiks-qt/Models/recentdirectoriesmodel.h \
    ../xpiks-qt/Helpers/keywordvalidator.h \
    ../xpiks-qt/Common/flags.h

macx {
OBJECTIVE_SOURCES += \
    ../xpiks-qt/Helpers/osxnsurlhelper.mm

LIBS += -framework Foundation
HEADERS += ../xpiks-qt/Helpers/osxnsurlhelper.h
}
