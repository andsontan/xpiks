#-------------------------------------------------
#
# Project created by QtCreator 2015-02-12T22:20:15
#
#-------------------------------------------------

QMAKE_MAC_SDK = macosx10.11

QT       += core testlib qml quick concurrent

QT       -= gui

TARGET = xpiks-tests-core
CONFIG   += console
CONFIG   -= app_bundle

win32:QT += winextras

INCLUDEPATH += ../../tiny-aes
win32:INCLUDEPATH += ../../xpiks-qt/Encryption
INCLUDEPATH += "../../hunspell-1.3.3/src/hunspell"
DEFINES += HUNSPELL_STATIC
DEFINES += TESTS

DEFINES += QT_NO_CAST_TO_ASCII \
           QT_NO_CAST_FROM_BYTEARRAY

LIBS += -L"$$PWD/../../libs/"
LIBS += -lhunspell

macx {
    INCLUDEPATH += "../../hunspell-1.3.3/src"
    #INCLUDEPATH += "../quazip"
    #INCLUDEPATH += "../../libcurl/include"
}

win32 {
    INCLUDEPATH += "../../zlib-1.2.8"
    INCLUDEPATH += "../../hunspell-1.3.3/src"
    #INCLUDEPATH += "../quazip"
    #INCLUDEPATH += "../libcurl/include"
    #LIBS -= -lcurl
    #LIBS += -llibcurl_debug
}

linux-g++-64 {
    target.path=/usr/bin/
    QML_IMPORT_PATH += /usr/lib/x86_64-linux-gnu/qt5/imports/
    UNAME = $$system(cat /proc/version)

    #contains(UNAME, Debian): {
    #    message("on Debian Linux")
    #    LIBS += -L/lib/x86_64-linux-gnu/
    #    LIBS -= -lquazip # temporary static link
    #    LIBS += /usr/lib/x86_64-linux-gnu/libquazip-qt5.a
    #}
    #contains(UNAME, SUSE): {
    #    message("on SUSE Linux")
    #    LIBS += -L/usr/lib64/
    #    LIBS += /usr/lib64/libcurl.so.4
    #}
}

TEMPLATE = app

SOURCES += main.cpp \
    encryption_tests.cpp \
    ../../tiny-aes/aes.cpp \
    indicestoranges_tests.cpp \
    ../../xpiks-qt/Helpers/indiceshelper.cpp \
    ../../xpiks-qt/Commands/commandmanager.cpp \
    ../../xpiks-qt/Models/artworkmetadata.cpp \
    ../../xpiks-qt/Models/artworksrepository.cpp \
    addcommand_tests.cpp \
    ../../xpiks-qt/Models/artitemsmodel.cpp \
	../../xpiks-qt/Models/filteredartitemsproxymodel.cpp \
    ../../xpiks-qt/Commands/addartworkscommand.cpp \
    ../../xpiks-qt/Models/artworksprocessor.cpp \
    ../../xpiks-qt/Models/combinedartworksmodel.cpp \
    ../../xpiks-qt/UndoRedo/addartworksitem.cpp \
    ../../xpiks-qt/UndoRedo/undoredomanager.cpp \
    ../../xpiks-qt/Encryption/secretsmanager.cpp \
    ../../xpiks-qt/Commands/combinededitcommand.cpp \
    ../../xpiks-qt/Commands/pastekeywordscommand.cpp \
    ../../xpiks-qt/Commands/removeartworkscommand.cpp \
    ../../xpiks-qt/UndoRedo/artworkmetadatabackup.cpp \
    ../../xpiks-qt/UndoRedo/modifyartworkshistoryitem.cpp \
    ../../xpiks-qt/UndoRedo/removeartworksitem.cpp \
    ../../xpiks-qt/Encryption/aes-qt.cpp \
    removecommand_tests.cpp \
    vectorfilenames_tests.cpp \
    ../../xpiks-qt/Helpers/filenameshelpers.cpp \
    ../../xpiks-qt/Helpers/helpersqmlwrapper.cpp \
    ../../xpiks-qt/Models/recentdirectoriesmodel.cpp \
    ../../xpiks-qt/Helpers/keywordvalidator.cpp \
    ../../xpiks-qt/SpellCheck/spellcheckerservice.cpp \
    ../../xpiks-qt/SpellCheck/spellcheckitem.cpp \
    ../../xpiks-qt/SpellCheck/spellcheckworker.cpp \
    ../../xpiks-qt/SpellCheck/spellchecksuggestionmodel.cpp \
    ../../xpiks-qt/MetadataIO/backupsaverservice.cpp \
    ../../xpiks-qt/MetadataIO/backupsaverworker.cpp \
    ../../xpiks-qt/Conectivity/telemetryservice.cpp \
    ../../xpiks-qt/Helpers/updateservice.cpp \
    ../../xpiks-qt/SpellCheck/spellcheckiteminfo.cpp \
    ../../xpiks-qt/SpellCheck/spellsuggestionsitem.cpp \
    ../../xpiks-qt/Common/basickeywordsmodel.cpp \
    ../../xpiks-qt/SpellCheck/spellcheckerrorshighlighter.cpp \
    artworkmetadata_tests.cpp \
    basickeywordsmodel_tests.cpp \
    combinedmodel_tests.cpp \
    ../../xpiks-qt/Models/settingsmodel.cpp \
    stringhelpers_tests.cpp \
    ../../xpiks-qt/Helpers/stringhelper.cpp \
    keywordvalidation_tests.cpp \
    artworkrepository_tests.cpp \
    ../../xpiks-qt/Conectivity/updatescheckerworker.cpp \
    ../../xpiks-qt/MetadataIO/metadataiocoordinator.cpp \
    ../../xpiks-qt/MetadataIO/metadatareadingworker.cpp \
    ../../xpiks-qt/MetadataIO/saverworkerjobitem.cpp \
    ../../xpiks-qt/Suggestion/locallibrary.cpp \
    ../../xpiks-qt/Suggestion/libraryloaderworker.cpp \
    ../../xpiks-qt/MetadataIO/metadatawritingworker.cpp \
    filteredmodel_tests.cpp \
    conectivityhelpers_tests.cpp \
    ../../xpiks-qt/Conectivity/conectivityhelpers.cpp \
    undoredo_tests.cpp \
    ../../xpiks-qt/Helpers/filterhelpers.cpp \
    artworkfilter_tests.cpp \
    ../../xpiks-qt/Models/artworkuploader.cpp \
    ../../xpiks-qt/Models/ziparchiver.cpp

HEADERS += \
    encryption_tests.h \
    ../../tiny-aes/aes.h \
    ../../xpiks-qt/Encryption/aes-qt.h \
    indicestoranges_tests.h \
    ../../xpiks-qt/Helpers/indiceshelper.h \
    Mocks/commandmanagermock.h \
    ../../xpiks-qt/Commands/commandmanager.h \
    ../../xpiks-qt/Models/artworkmetadata.h \
    ../../xpiks-qt/Models/artworksrepository.h \
    addcommand_tests.h \
    ../../xpiks-qt/Models/artitemsmodel.h \
	../../xpiks-qt/Models/filteredartitemsproxymodel.h \
    Mocks/artitemsmodelmock.h \
    ../../xpiks-qt/Commands/addartworkscommand.h \
    ../../xpiks-qt/Models/artworksprocessor.h \
    ../../xpiks-qt/Models/combinedartworksmodel.h \
    ../../xpiks-qt/UndoRedo/addartworksitem.h \
    ../../xpiks-qt/UndoRedo/historyitem.h \
    ../../xpiks-qt/UndoRedo/undoredomanager.h \
    ../../xpiks-qt/Encryption/secretsmanager.h \
    ../../xpiks-qt/Commands/combinededitcommand.h \
    ../../xpiks-qt/Commands/commandbase.h \
    ../../xpiks-qt/Commands/pastekeywordscommand.h \
    ../../xpiks-qt/Commands/removeartworkscommand.h \
    ../../xpiks-qt/UndoRedo/artworkmetadatabackup.h \
    ../../xpiks-qt/UndoRedo/modifyartworkshistoryitem.h \
    ../../xpiks-qt/UndoRedo/removeartworksitem.h \
    Mocks/artworkmetadatamock.h \
    removecommand_tests.h \
    ../../xpiks-qt/Common/basickeywordsmodel.h \
    vectorfilenames_tests.h \
    ../../xpiks-qt/Helpers/filenameshelpers.h \
    ../../xpiks-qt/Helpers/helpersqmlwrapper.h \
    ../../xpiks-qt/Models/recentdirectoriesmodel.h \
    ../../xpiks-qt/Helpers/keywordvalidator.h \
    ../../xpiks-qt/Common/flags.h \
    ../../xpiks-qt/SpellCheck/spellcheckerservice.h \
    ../../xpiks-qt/SpellCheck/spellcheckitem.h \
    ../../xpiks-qt/SpellCheck/spellcheckworker.h \
    ../../xpiks-qt/SpellCheck/spellchecksuggestionmodel.h \
    ../../xpiks-qt/MetadataIO/backupsaverservice.h \
    ../../xpiks-qt/MetadataIO/backupsaverworker.h \
    ../../xpiks-qt/Conectivity/analyticsuserevent.h \
    ../../xpiks-qt/Conectivity/telemetryservice.h \
    ../../xpiks-qt/Helpers/updateservice.h \
    ../../xpiks-qt/SpellCheck/ispellcheckable.h \
    ../../xpiks-qt/SpellCheck/spellcheckiteminfo.h \
    ../../xpiks-qt/SpellCheck/spellsuggestionsitem.h \
    ../../xpiks-qt/SpellCheck/spellcheckerrorshighlighter.h \
    artworkmetadata_tests.h \
    basickeywordsmodel_tests.h \
    combinedmodel_tests.h \
    ../../xpiks-qt/Models/settingsmodel.h \
    ../../xpiks-qt/Helpers/appsettings.h \
    stringhelpers_tests.h \
    keywordvalidation_tests.h \
    artworkrepository_tests.h \
    ../../xpiks-qt/Conectivity/updatescheckerworker.h \
    ../../xpiks-qt/Common/itemprocessingworker.h \
    ../../xpiks-qt/MetadataIO/metadataiocoordinator.h \
    ../../xpiks-qt/MetadataIO/metadatareadingworker.h \
    ../../xpiks-qt/MetadataIO/saverworkerjobitem.h \
    ../../xpiks-qt/Suggestion/locallibrary.h \
    ../../xpiks-qt/Suggestion/libraryloaderworker.h \
    ../../xpiks-qt/MetadataIO/metadatawritingworker.h \
    filteredmodel_tests.h \
    ../../xpiks-qt/Common/baseentity.h \
    ../../xpiks-qt/Common/defines.h \
    ../../xpiks-qt/Common/iartworkssource.h \
    ../../xpiks-qt/Common/ibasicartwork.h \
    ../../xpiks-qt/Common/isafepointer.h \
    ../../xpiks-qt/Common/iservicebase.h \
    ../../xpiks-qt/Common/version.h \
    ../../xpiks-qt/Commands/icommandbase.h \
    ../../xpiks-qt/Commands/icommandmanager.h \
    conectivityhelpers_tests.h \
    ../../xpiks-qt/Conectivity/conectivityhelpers.h \
    ../../xpiks-qt/Conectivity/uploadbatch.h \
    ../../xpiks-qt/Conectivity/uploadcontext.h \
    undoredo_tests.h \
    ../../xpiks-qt/Helpers/filterhelpers.h \
    artworkfilter_tests.h \
    ../../xpiks-qt/Models/artworkuploader.h \
    ../../xpiks-qt/Conectivity/iftpcoordinator.h \
    ../../xpiks-qt/Models/ziparchiver.h
