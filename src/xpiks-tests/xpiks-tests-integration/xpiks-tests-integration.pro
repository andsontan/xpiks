TEMPLATE = app
TARGET = xpiks-tests-integration

QT += qml quick widgets concurrent svg
QT -= gui

CONFIG   += console
CONFIG   -= app_bundle

CONFIG += c++11

BUILDNO = $$system(git log -n 1 --pretty=format:"%H")
DEFINES += BUILDNUMBER=$${BUILDNO}

DEFINES += QT_NO_CAST_TO_ASCII \
           QT_NO_CAST_FROM_BYTEARRAY
DEFINES += QUAZIP_STATIC
DEFINES += HUNSPELL_STATIC
DEFINES += TELEMETRY_ENABLED

DEFINES += INTEGRATION_TESTS

SOURCES += main.cpp \
    ../../xpiks-qt/Commands/addartworkscommand.cpp \
    ../../xpiks-qt/Commands/combinededitcommand.cpp \
    ../../xpiks-qt/Commands/commandmanager.cpp \
    ../../xpiks-qt/Commands/pastekeywordscommand.cpp \
    ../../xpiks-qt/Commands/removeartworkscommand.cpp \
    ../../xpiks-qt/Common/basickeywordsmodel.cpp \
    ../../xpiks-qt/Conectivity/conectivityhelpers.cpp \
    ../../xpiks-qt/Conectivity/curlftpuploader.cpp \
    ../../xpiks-qt/Conectivity/ftpcoordinator.cpp \
    ../../xpiks-qt/Conectivity/ftphelpers.cpp \
    ../../xpiks-qt/Conectivity/ftpuploaderworker.cpp \
    ../../xpiks-qt/Conectivity/telemetryservice.cpp \
    ../../xpiks-qt/Conectivity/testconnection.cpp \
    ../../xpiks-qt/Conectivity/updatescheckerworker.cpp \
    ../../xpiks-qt/Encryption/aes-qt.cpp \
    ../../xpiks-qt/Encryption/secretsmanager.cpp \
    ../../xpiks-qt/Helpers/filenameshelpers.cpp \
    ../../xpiks-qt/Helpers/filterhelpers.cpp \
    ../../xpiks-qt/Helpers/globalimageprovider.cpp \
    ../../xpiks-qt/Helpers/helpersqmlwrapper.cpp \
    ../../xpiks-qt/Helpers/indiceshelper.cpp \
    ../../xpiks-qt/Helpers/keywordvalidator.cpp \
    ../../xpiks-qt/Helpers/logger.cpp \
    ../../xpiks-qt/Helpers/loggingworker.cpp \
    ../../xpiks-qt/Helpers/loghighlighter.cpp \
    ../../xpiks-qt/Helpers/runguard.cpp \
    ../../xpiks-qt/Helpers/stringhelper.cpp \
    ../../xpiks-qt/Helpers/ziphelper.cpp \
    ../../xpiks-qt/Helpers/updateservice.cpp \
    ../../xpiks-qt/MetadataIO/backupsaverservice.cpp \
    ../../xpiks-qt/MetadataIO/backupsaverworker.cpp \
    ../../xpiks-qt/MetadataIO/metadataiocoordinator.cpp \
    ../../xpiks-qt/MetadataIO/metadatareadingworker.cpp \
    ../../xpiks-qt/MetadataIO/metadatawritingworker.cpp \
    ../../xpiks-qt/MetadataIO/saverworkerjobitem.cpp \
    ../../xpiks-qt/Models/artitemsmodel.cpp \
    ../../xpiks-qt/Models/artworkmetadata.cpp \
    ../../xpiks-qt/Models/artworksprocessor.cpp \
    ../../xpiks-qt/Models/artworksrepository.cpp \
    ../../xpiks-qt/Models/artworkuploader.cpp \
    ../../xpiks-qt/Models/combinedartworksmodel.cpp \
    ../../xpiks-qt/Models/filteredartitemsproxymodel.cpp \
    ../../xpiks-qt/Models/languagesmodel.cpp \
    ../../xpiks-qt/Models/logsmodel.cpp \
    ../../xpiks-qt/Models/recentdirectoriesmodel.cpp \
    ../../xpiks-qt/Models/settingsmodel.cpp \
    ../../xpiks-qt/Models/ziparchiver.cpp \
    ../../xpiks-qt/Models/uploadinforepository.cpp \
    ../../xpiks-qt/Plugins/pluginactionsmodel.cpp \
    ../../xpiks-qt/Plugins/pluginmanager.cpp \
    ../../xpiks-qt/Plugins/pluginwrapper.cpp \
    ../../xpiks-qt/Plugins/uiprovider.cpp \
    ../../xpiks-qt/SpellCheck/spellcheckerrorshighlighter.cpp \
    ../../xpiks-qt/SpellCheck/spellcheckerservice.cpp \
    ../../xpiks-qt/SpellCheck/spellcheckitem.cpp \
    ../../xpiks-qt/SpellCheck/spellcheckiteminfo.cpp \
    ../../xpiks-qt/SpellCheck/spellchecksuggestionmodel.cpp \
    ../../xpiks-qt/SpellCheck/spellcheckworker.cpp \
    ../../xpiks-qt/SpellCheck/spellsuggestionsitem.cpp \
    ../../xpiks-qt/Suggestion/keywordssuggestor.cpp \
    ../../xpiks-qt/Suggestion/libraryloaderworker.cpp \
    ../../xpiks-qt/Suggestion/libraryqueryworker.cpp \
    ../../xpiks-qt/Suggestion/locallibrary.cpp \
    ../../xpiks-qt/UndoRedo/addartworksitem.cpp \
    ../../xpiks-qt/UndoRedo/artworkmetadatabackup.cpp \
    ../../xpiks-qt/UndoRedo/modifyartworkshistoryitem.cpp \
    ../../xpiks-qt/UndoRedo/removeartworksitem.cpp \
    ../../xpiks-qt/UndoRedo/undoredomanager.cpp \
    ../../xpiks-qt/Warnings/warningscheckingworker.cpp \
    ../../xpiks-qt/Warnings/warningsmodel.cpp \
    ../../xpiks-qt/Warnings/warningsservice.cpp \
    ../../tiny-aes/aes.cpp \
    addfilesbasictest.cpp \
    autoattachvectorstest.cpp \
    savefilebasictest.cpp \
    spellcheckmultireplacetest.cpp \
    spellcheckcombinedmodeltest.cpp \
    zipartworkstest.cpp \
    ../../xpiks-qt/Suggestion/locallibraryqueryengine.cpp \
    ../../xpiks-qt/Suggestion/shutterstockqueryengine.cpp \
    spellcheckundotest.cpp \
    ../../xpiks-qt/Suggestion/fotoliaqueryengine.cpp

RESOURCES +=

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    ../../xpiks-qt/Commands/addartworkscommand.h \
    ../../xpiks-qt/Commands/combinededitcommand.h \
    ../../xpiks-qt/Commands/commandbase.h \
    ../../xpiks-qt/Commands/commandmanager.h \
    ../../xpiks-qt/Commands/icommandbase.h \
    ../../xpiks-qt/Commands/icommandmanager.h \
    ../../xpiks-qt/Commands/pastekeywordscommand.h \
    ../../xpiks-qt/Commands/removeartworkscommand.h \
    ../../xpiks-qt/Common/baseentity.h \
    ../../xpiks-qt/Common/basickeywordsmodel.h \
    ../../xpiks-qt/Common/defines.h \
    ../../xpiks-qt/Common/flags.h \
    ../../xpiks-qt/Common/iartworkssource.h \
    ../../xpiks-qt/Common/ibasicartwork.h \
    ../../xpiks-qt/Common/isafepointer.h \
    ../../xpiks-qt/Common/iservicebase.h \
    ../../xpiks-qt/Common/itemprocessingworker.h \
    ../../xpiks-qt/Common/version.h \
    ../../xpiks-qt/Conectivity/analyticsuserevent.h \
    ../../xpiks-qt/Conectivity/conectivityhelpers.h \
    ../../xpiks-qt/Conectivity/curlftpuploader.h \
    ../../xpiks-qt/Conectivity/ftpcoordinator.h \
    ../../xpiks-qt/Conectivity/ftphelpers.h \
    ../../xpiks-qt/Conectivity/ftpuploaderworker.h \
    ../../xpiks-qt/Conectivity/iftpcoordinator.h \
    ../../xpiks-qt/Conectivity/telemetryservice.h \
    ../../xpiks-qt/Conectivity/testconnection.h \
    ../../xpiks-qt/Conectivity/updatescheckerworker.h \
    ../../xpiks-qt/Conectivity/uploadbatch.h \
    ../../xpiks-qt/Conectivity/uploadcontext.h \
    ../../xpiks-qt/Encryption/aes-qt.h \
    ../../xpiks-qt/Encryption/secretsmanager.h \
    ../../xpiks-qt/Helpers/appsettings.h \
    ../../xpiks-qt/Helpers/clipboardhelper.h \
    ../../xpiks-qt/Helpers/constants.h \
    ../../xpiks-qt/Helpers/filenameshelpers.h \
    ../../xpiks-qt/Helpers/filterhelpers.h \
    ../../xpiks-qt/Helpers/globalimageprovider.h \
    ../../xpiks-qt/Helpers/helpersqmlwrapper.h \
    ../../xpiks-qt/Helpers/indiceshelper.h \
    ../../xpiks-qt/Helpers/keywordvalidator.h \
    ../../xpiks-qt/Helpers/logger.h \
    ../../xpiks-qt/Helpers/loggingworker.h \
    ../../xpiks-qt/Helpers/loghighlighter.h \
    ../../xpiks-qt/Helpers/runguard.h \
    ../../xpiks-qt/Helpers/stringhelper.h \
    ../../xpiks-qt/Helpers/ziphelper.h \
    ../../xpiks-qt/Helpers/updateservice.h \
    ../../xpiks-qt/MetadataIO/backupsaverservice.h \
    ../../xpiks-qt/MetadataIO/backupsaverworker.h \
    ../../xpiks-qt/MetadataIO/metadataiocoordinator.h \
    ../../xpiks-qt/MetadataIO/metadatareadingworker.h \
    ../../xpiks-qt/MetadataIO/metadatawritingworker.h \
    ../../xpiks-qt/MetadataIO/saverworkerjobitem.h \
    ../../xpiks-qt/Models/abstractlistmodel.h \
    ../../xpiks-qt/Models/artiteminfo.h \
    ../../xpiks-qt/Models/artitemsmodel.h \
    ../../xpiks-qt/Models/artworkmetadata.h \
    ../../xpiks-qt/Models/artworksprocessor.h \
    ../../xpiks-qt/Models/artworksrepository.h \
    ../../xpiks-qt/Models/artworkuploader.h \
    ../../xpiks-qt/Models/combinedartworksmodel.h \
    ../../xpiks-qt/Models/exportinfo.h \
    ../../xpiks-qt/Models/filteredartitemsproxymodel.h \
    ../../xpiks-qt/Models/languagesmodel.h \
    ../../xpiks-qt/Models/logsmodel.h \
    ../../xpiks-qt/Models/recentdirectoriesmodel.h \
    ../../xpiks-qt/Models/settingsmodel.h \
    ../../xpiks-qt/Models/ziparchiver.h \
    ../../xpiks-qt/Models/uploadinfo.h \
    ../../xpiks-qt/Models/uploadinforepository.h \
    ../../xpiks-qt/Plugins/ipluginaction.h \
    ../../xpiks-qt/Plugins/iuiprovider.h \
    ../../xpiks-qt/Plugins/pluginactionsmodel.h \
    ../../xpiks-qt/Plugins/pluginmanager.h \
    ../../xpiks-qt/Plugins/pluginwrapper.h \
    ../../xpiks-qt/Plugins/uiprovider.h \
    ../../xpiks-qt/Plugins/xpiksplugininterface.h \
    ../../xpiks-qt/SpellCheck/ispellcheckable.h \
    ../../xpiks-qt/SpellCheck/spellcheckerrorshighlighter.h \
    ../../xpiks-qt/SpellCheck/spellcheckerservice.h \
    ../../xpiks-qt/SpellCheck/spellcheckitem.h \
    ../../xpiks-qt/SpellCheck/spellcheckiteminfo.h \
    ../../xpiks-qt/SpellCheck/spellchecksuggestionmodel.h \
    ../../xpiks-qt/SpellCheck/spellcheckworker.h \
    ../../xpiks-qt/SpellCheck/spellsuggestionsitem.h \
    ../../xpiks-qt/Suggestion/keywordssuggestor.h \
    ../../xpiks-qt/Suggestion/libraryloaderworker.h \
    ../../xpiks-qt/Suggestion/libraryqueryworker.h \
    ../../xpiks-qt/Suggestion/locallibrary.h \
    ../../xpiks-qt/Suggestion/suggestionartwork.h \
    ../../xpiks-qt/UndoRedo/addartworksitem.h \
    ../../xpiks-qt/UndoRedo/artworkmetadatabackup.h \
    ../../xpiks-qt/UndoRedo/historyitem.h \
    ../../xpiks-qt/UndoRedo/ihistoryitem.h \
    ../../xpiks-qt/UndoRedo/iundoredomanager.h \
    ../../xpiks-qt/UndoRedo/modifyartworkshistoryitem.h \
    ../../xpiks-qt/UndoRedo/removeartworksitem.h \
    ../../xpiks-qt/UndoRedo/undoredomanager.h \
    ../../xpiks-qt/Warnings/iwarningscheckable.h \
    ../../xpiks-qt/Warnings/warningscheckingworker.h \
    ../../xpiks-qt/Warnings/warningsitem.h \
    ../../xpiks-qt/Warnings/warningsmodel.h \
    ../../xpiks-qt/Warnings/warningsservice.h \
    ../../tiny-aes/aes.h \
    integrationtestbase.h \
    addfilesbasictest.h \
    signalwaiter.h \
    autoattachvectorstest.h \
    savefilebasictest.h \
    spellcheckmultireplacetest.h \
    spellcheckcombinedmodeltest.h \
    zipartworkstest.h \
    ../../xpiks-qt/Suggestion/locallibraryqueryengine.h \
    ../../xpiks-qt/Suggestion/shutterstockqueryengine.h \
    ../../xpiks-qt/Suggestion/suggestionqueryenginebase.h \
    spellcheckundotest.h \
    ../../xpiks-qt/Suggestion/fotoliaqueryengine.h

INCLUDEPATH += ../../tiny-aes

LIBS += -L"$$PWD/../../libs/"
LIBS += -lhunspell
LIBS += -lz
LIBS += -lcurl
LIBS += -lquazip

macx {
    INCLUDEPATH += "../../hunspell-1.3.3/src"
    INCLUDEPATH += "../../quazip"
    INCLUDEPATH += "../../../libcurl/include"
}


win32 {
    QT += winextras
    INCLUDEPATH += "../../zlib-1.2.8"
    INCLUDEPATH += "../../hunspell-1.3.3/src"
    INCLUDEPATH += "../../quazip"
    INCLUDEPATH += "../../libcurl/include"
    LIBS -= -lcurl

    CONFIG(debug, debug|release) {
        EXE_DIR = debug
        LIBS += -llibcurl_debug
    }

    CONFIG(release, debug|release) {
        EXE_DIR = release
        LIBS += -llibcurl
    }
}

linux-g++-64 {
    message("for Linux")
    target.path=/usr/bin/
    QML_IMPORT_PATH += /usr/lib/x86_64-linux-gnu/qt5/imports/
    #DEFINES -= TELEMETRY_ENABLED
    LIBS += -L/lib/x86_64-linux-gnu/

    UNAME = $$system(cat /proc/version | tr -d \'()\')
    contains( UNAME, Debian ) {
        message("distribution : Debian")
        LIBS -= -lquazip # temporary static link
        LIBS += /usr/lib/x86_64-linux-gnu/libquazip-qt5.a
    }
    contains( UNAME, SUSE ) {
        message("distribution : SUSE")
    }
}

travis-ci {
    message("for Travis CI")
    LIBS -= -lz
    LIBS += /usr/lib/x86_64-linux-gnu/libz.so
}
