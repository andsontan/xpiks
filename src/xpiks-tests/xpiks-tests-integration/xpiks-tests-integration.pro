TEMPLATE = app
TARGET = xpiks-tests-integration

QMAKE_MAC_SDK = macosx10.11

QT += qml quick widgets concurrent svg
QT -= gui

CONFIG   += console
CONFIG   -= app_bundle

CONFIG += c++11

BUILDNO = $$system(git log -n 1 --pretty=format:"%H")
DEFINES += BUILDNUMBER=$${BUILDNO}

DEFINES += QT_NO_CAST_TO_ASCII \
           QT_RESTRICTED_CAST_FROM_ASCII \
           QT_NO_CAST_FROM_BYTEARRAY

DEFINES += QUAZIP_STATIC
DEFINES += HUNSPELL_STATIC
DEFINES += TELEMETRY_ENABLED
DEFINES += LIBFACE_STATIC

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
    ../../xpiks-qt/Helpers/keywordshelpers.cpp \
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
    ../../xpiks-qt/Suggestion/fotoliaqueryengine.cpp \
    ../../xpiks-qt/QMLExtensions/colorsmodel.cpp \
    ../../xpiks-qt/AutoComplete/autocompletemodel.cpp \
    ../../xpiks-qt/AutoComplete/autocompleteservice.cpp \
    ../../xpiks-qt/AutoComplete/autocompleteworker.cpp \
    ../../xpiks-qt/Suggestion/gettyqueryengine.cpp \
    ../../xpiks-qt/AutoComplete/stocksftplistmodel.cpp \
    ../../xpiks-qt/AutoComplete/stringfilterproxymodel.cpp \
    ../../xpiks-qt/Models/abstractconfigupdatermodel.cpp \
    ../../xpiks-qt/Helpers/jsonhelper.cpp \
    ../../xpiks-qt/Helpers/localconfig.cpp \
    ../../xpiks-qt/Helpers/remoteconfig.cpp \
    autocompletebasictest.cpp \
    ../../xpiks-qt/Models/imageartwork.cpp \
    spellingproduceswarningstest.cpp \
    undoaddwithvectorstest.cpp \
    ../../xpiks-qt/MetadataIO/exiv2readingworker.cpp \
    ../../xpiks-qt/MetadataIO/readingorchestrator.cpp \
    ../../xpiks-qt/MetadataIO/exiv2writingworker.cpp \
    ../../xpiks-qt/MetadataIO/writingorchestrator.cpp \
    ../../xpiks-qt/Common/flags.cpp \
    readlegacysavedtest.cpp \
    ../../xpiks-qt/QMLExtensions/imagecachingservice.cpp \
    ../../xpiks-qt/QMLExtensions/imagecachingworker.cpp \
    ../../xpiks-qt/QMLExtensions/cachingimageprovider.cpp \
    clearmetadatatest.cpp \
    savewithemptytitletest.cpp \
    jsonmerge_tests.cpp \
    ../../xpiks-qt/Commands/findandreplacecommand.cpp \
    ../../xpiks-qt/Models/artworksviewmodel.cpp \
    ../../xpiks-qt/Models/deletekeywordsviewmodel.cpp \
    ../../xpiks-qt/Commands/deletekeywordscommand.cpp \
    combinededitfixspellingtest.cpp \
    findandreplacemodeltest.cpp \
    ../../xpiks-qt/Models/findandreplacemodel.cpp \
    ../../xpiks-qt/Conectivity/uploadwatcher.cpp

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
    ../../xpiks-qt/Helpers/keywordshelpers.h \
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
    ../../xpiks-qt/Common/abstractlistmodel.h \
    ../../xpiks-qt/Models/metadataelement.h \
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
    ../../xpiks-qt/Suggestion/fotoliaqueryengine.h \
    ../../xpiks-qt/QMLExtensions/colorsmodel.h \
    ../../xpiks-qt/AutoComplete/autocompletemodel.h \
    ../../xpiks-qt/AutoComplete/autocompleteservice.h \
    ../../xpiks-qt/AutoComplete/autocompleteworker.h \
    ../../xpiks-qt/AutoComplete/completionquery.h \
    ../../xpiks-qt/Suggestion/gettyqueryengine.h \
    ../../xpiks-qt/AutoComplete/stocksftplistmodel.h \
    ../../xpiks-qt/AutoComplete/stringfilterproxymodel.h \
    ../../xpiks-qt/Models/abstractconfigupdatermodel.h \
    ../../xpiks-qt/Helpers/jsonhelper.h \
    ../../xpiks-qt/Helpers/localconfig.h \
    ../../xpiks-qt/Helpers/remoteconfig.h \
    autocompletebasictest.h \
    ../../xpiks-qt/Common/hold.h \
    ../../xpiks-qt/Models/imageartwork.h \
    undoaddwithvectorstest.h \
    ../../xpiks-qt/MetadataIO/exiv2readingworker.h \
    ../../xpiks-qt/MetadataIO/imetadatareader.h \
    ../../xpiks-qt/MetadataIO/importdataresult.h \
    ../../xpiks-qt/MetadataIO/readingorchestrator.h \
    ../../xpiks-qt/MetadataIO/exiv2writingworker.h \
    ../../xpiks-qt/MetadataIO/imetadatawriter.h \
    ../../xpiks-qt/MetadataIO/exiv2tagnames.h \
    ../../xpiks-qt/MetadataIO/writingorchestrator.h \
    readlegacysavedtest.h \
    ../../xpiks-qt/QMLExtensions/imagecacherequest.h \
    ../../xpiks-qt/QMLExtensions/imagecachingservice.h \
    ../../xpiks-qt/QMLExtensions/imagecachingworker.h \
    ../../xpiks-qt/QMLExtensions/cachingimageprovider.h \
    clearmetadatatest.h \
    savewithemptytitletest.h \
    spellingproduceswarningstest.h \
    ../../xpiks-qt/Helpers/comparevaluesjson.h \
    jsonmerge_tests.h \
    ../../xpiks-qt/Commands/findandreplacecommand.h \
    ../../xpiks-qt/Models/artworksviewmodel.h \
    ../../xpiks-qt/Models/deletekeywordsviewmodel.h \
    ../../xpiks-qt/Commands/deletekeywordscommand.h \
    combinededitfixspellingtest.h \
    ../../xpiks-qt/Common/iflagsprovider.h \
    findandreplacemodeltest.h \
    ../../xpiks-qt/Models/findandreplacemodel.h \
    ../../xpiks-qt/Conectivity/uploadwatcher.h

INCLUDEPATH += ../../tiny-aes
INCLUDEPATH += ../../cpp-libface

LIBS += -L"$$PWD/../../libs/"
LIBS += -lhunspell
LIBS += -lz
LIBS += -lcurl
LIBS += -lquazip
LIBS += -lface

macx {
    INCLUDEPATH += "../../hunspell-1.3.3/src"
    INCLUDEPATH += "../../quazip"
    INCLUDEPATH += "../../../libcurl/include"
    INCLUDEPATH += "../../exiv2-0.25/include"

    LIBS += -liconv
    LIBS += -lexpat

    LIBS += -lxmpsdk
    LIBS += -lexiv2
}

win32 {
    DEFINES += QT_NO_PROCESS_COMBINED_ARGUMENT_START
    QT += winextras
    INCLUDEPATH += "../../zlib-1.2.8"
    INCLUDEPATH += "../../hunspell-1.3.3/src"
    INCLUDEPATH += "../../quazip"
    INCLUDEPATH += "../../libcurl/include"
    INCLUDEPATH += "../../exiv2-0.25/include"
    LIBS -= -lcurl
    LIBS += -lmman

    LIBS += -llibexpat
    LIBS += -llibexiv2

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
    LIBS += -lexiv2

    message("for Linux")
    target.path=/usr/bin/
    QML_IMPORT_PATH += /usr/lib/x86_64-linux-gnu/qt5/imports/
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
    LIBS += -lexiv2
    DEFINES += TRAVIS_CI
}

appveyor {
    message("for Appveyor")
    DEFINES += APPVEYOR
}
