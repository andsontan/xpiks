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
CONFIG   += c++11

win32:QT += winextras

INCLUDEPATH += ../../../vendors/tiny-aes
win32:INCLUDEPATH += ../../xpiks-qt/Encryption
INCLUDEPATH += "../../../vendors/hunspell-1.6.0/src"
DEFINES += HUNSPELL_STATIC
DEFINES += CORE_TESTS

DEFINES += QT_NO_CAST_TO_ASCII \
           QT_RESTRICTED_CAST_FROM_ASCII \
           QT_NO_CAST_FROM_BYTEARRAY

LIBS += -L"$$PWD/../../../libs/"
LIBS += -lhunspell

macx {
    #INCLUDEPATH += "../quazip"
    #INCLUDEPATH += "../../libcurl/include"
}

win32 {
    INCLUDEPATH += "../../../vendors/zlib-1.2.8"
    #INCLUDEPATH += "../quazip"
    #INCLUDEPATH += "../libcurl/include"
    #LIBS -= -lcurl
    #LIBS += -llibcurl_debug
}

travis-ci {
    message("for Travis CI")
    DEFINES += TRAVIS_CI
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
    ../../../vendors/tiny-aes/aes.cpp \
    indicestoranges_tests.cpp \
    ../../xpiks-qt/Helpers/indiceshelper.cpp \
    ../../xpiks-qt/Commands/commandmanager.cpp \
    ../../xpiks-qt/Commands/findandreplacecommand.cpp \
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
    ../../xpiks-qt/Models/recentitemsmodel.cpp \
    ../../xpiks-qt/Models/recentdirectoriesmodel.cpp \
    ../../xpiks-qt/Models/recentfilesmodel.cpp \
    ../../xpiks-qt/Helpers/keywordshelpers.cpp \
    ../../xpiks-qt/SpellCheck/spellcheckerservice.cpp \
    ../../xpiks-qt/SpellCheck/spellcheckitem.cpp \
    ../../xpiks-qt/SpellCheck/spellcheckworker.cpp \
    ../../xpiks-qt/SpellCheck/spellchecksuggestionmodel.cpp \
    ../../xpiks-qt/MetadataIO/backupsaverservice.cpp \
    ../../xpiks-qt/MetadataIO/backupsaverworker.cpp \
    ../../xpiks-qt/SpellCheck/spellcheckiteminfo.cpp \
    ../../xpiks-qt/SpellCheck/spellsuggestionsitem.cpp \
    ../../xpiks-qt/Common/basickeywordsmodel.cpp \
    ../../xpiks-qt/Common/basicmetadatamodel.cpp \
    ../../xpiks-qt/SpellCheck/spellcheckerrorshighlighter.cpp \
    artworkmetadata_tests.cpp \
    combinedmodel_tests.cpp \
    ../../xpiks-qt/Helpers/localconfig.cpp \
    ../../xpiks-qt/Models/proxysettings.cpp \
    ../../xpiks-qt/Models/uploadinforepository.cpp \
    ../../xpiks-qt/Models/settingsmodel.cpp \
    stringhelpers_tests.cpp \
    ../../xpiks-qt/Helpers/stringhelper.cpp \
    keywordvalidation_tests.cpp \
    artworkrepository_tests.cpp \
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
    ../../xpiks-qt/Models/ziparchiver.cpp \
    removefilesfs_tests.cpp \
    ../../xpiks-qt/Helpers/jsonhelper.cpp \
    ../../xpiks-qt/AutoComplete/stringfilterproxymodel.cpp \
    ../../xpiks-qt/Models/imageartwork.cpp \
    recentitems_tests.cpp \
    artitemsmodel_tests.cpp \
    ../../xpiks-qt/Common/flags.cpp \
    fixspelling_tests.cpp \
    deleteoldlogstest.cpp \
    ../../xpiks-qt/Helpers/deletelogshelper.cpp \
    ../../xpiks-qt/Models/findandreplacemodel.cpp \
    replacepreview_tests.cpp \
    replace_tests.cpp \
    stringhelpersfortests.cpp \
    ../../xpiks-qt/Models/artworksviewmodel.cpp \
    deletekeywords_tests.cpp \
    ../../xpiks-qt/Models/deletekeywordsviewmodel.cpp \
    ../../xpiks-qt/Commands/deletekeywordscommand.cpp \
    ../../xpiks-qt/Conectivity/uploadwatcher.cpp \
    ../../xpiks-qt/Helpers/updatehelpers.cpp \
    basicmetadatamodel_tests.cpp \
    ../../xpiks-qt/KeywordsPresets/presetkeywordsmodel.cpp \
    ../../xpiks-qt/Models/artworkproxybase.cpp \
    preset_tests.cpp \
    ../../xpiks-qt/Commands/expandpresetcommand.cpp \
    quickbuffer_tests.cpp \
    ../../xpiks-qt/QuickBuffer/currenteditableartwork.cpp \
    ../../xpiks-qt/QuickBuffer/currenteditableproxyartwork.cpp \
    ../../xpiks-qt/QuickBuffer/quickbuffer.cpp \
    ../../xpiks-qt/Models/artworkproxymodel.cpp \
    ../../xpiks-qt/Models/uimanager.cpp \
    ../../xpiks-qt/QMLExtensions/tabsmodel.cpp \
    ../../xpiks-qt/Models/videoartwork.cpp

HEADERS += \
    encryption_tests.h \
    ../../../vendors/tiny-aes/aes.h \
    ../../xpiks-qt/Encryption/aes-qt.h \
    indicestoranges_tests.h \
    ../../xpiks-qt/Helpers/indiceshelper.h \
    Mocks/commandmanagermock.h \
    ../../xpiks-qt/Common/abstractlistmodel.h \
    ../../xpiks-qt/Commands/commandmanager.h \
    ../../xpiks-qt/Commands/findandreplacecommand.h \
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
    ../../xpiks-qt/Common/basicmetadatamodel.h \
    vectorfilenames_tests.h \
    ../../xpiks-qt/Helpers/filenameshelpers.h \
    ../../xpiks-qt/Helpers/helpersqmlwrapper.h \
    ../../xpiks-qt/Models/recentitemsmodel.h \
    ../../xpiks-qt/Models/recentdirectoriesmodel.h \
    ../../xpiks-qt/Models/recentfilesmodel.h \
    ../../xpiks-qt/Helpers/keywordshelpers.h \
    ../../xpiks-qt/Common/flags.h \
    ../../xpiks-qt/SpellCheck/spellcheckerservice.h \
    ../../xpiks-qt/SpellCheck/spellcheckitem.h \
    ../../xpiks-qt/SpellCheck/spellcheckworker.h \
    ../../xpiks-qt/SpellCheck/spellchecksuggestionmodel.h \
    ../../xpiks-qt/MetadataIO/backupsaverservice.h \
    ../../xpiks-qt/MetadataIO/backupsaverworker.h \
    ../../xpiks-qt/Conectivity/analyticsuserevent.h \
    ../../xpiks-qt/SpellCheck/spellcheckiteminfo.h \
    ../../xpiks-qt/SpellCheck/spellsuggestionsitem.h \
    ../../xpiks-qt/SpellCheck/spellcheckerrorshighlighter.h \
    artworkmetadata_tests.h \
    combinedmodel_tests.h \
    ../../xpiks-qt/Helpers/localconfig.h \
    ../../xpiks-qt/Models/proxysettings.h \
    ../../xpiks-qt/Models/uploadinfo.h \
    ../../xpiks-qt/Models/uploadinforepository.h \
    ../../xpiks-qt/Models/settingsmodel.h \
    stringhelpers_tests.h \
    keywordvalidation_tests.h \
    artworkrepository_tests.h \
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
    ../../xpiks-qt/Conectivity/iftpcoordinator.h \
    ../../xpiks-qt/Models/ziparchiver.h \
    removefilesfs_tests.h \
    Mocks/artworksrepositorymock.h \
    ../../xpiks-qt/Helpers/jsonhelper.h \
    ../../xpiks-qt/AutoComplete/stringfilterproxymodel.h \
    ../../xpiks-qt/Models/imageartwork.h \
    deleteoldlogstest.h \
    ../../xpiks-qt/Common/hold.h \
    recentitems_tests.h \
    removefilesfs_tests.h \
    artitemsmodel_tests.h \
    fixspelling_tests.h \
    Mocks/spellcheckservicemock.h \
    ../../xpiks-qt/Helpers/deletelogshelper.h \
    ../../xpiks-qt/Models/findandreplacemodel.h \
    replacepreview_tests.h \
    replace_tests.h \
    stringhelpersfortests.h \
    ../../xpiks-qt/Models/artworksviewmodel.h \
    deletekeywords_tests.h \
    ../../xpiks-qt/Models/deletekeywordsviewmodel.h \
    ../../xpiks-qt/Commands/deletekeywordscommand.h \
    ../../xpiks-qt/Common/iflagsprovider.h \
    ../../xpiks-qt/Conectivity/uploadwatcher.h \
    ../../xpiks-qt/Helpers/updatehelpers.h \
    basicmetadatamodel_tests.h \
    ../../xpiks-qt/KeywordsPresets/presetkeywordsmodel.h \
    ../../xpiks-qt/Common/imetadataoperator.h \
    ../../xpiks-qt/Models/artworkproxybase.h \
    preset_tests.h \
    ../../xpiks-qt/Commands/expandpresetcommand.h \
    quickbuffer_tests.h \
    ../../xpiks-qt/QuickBuffer/currenteditableartwork.h \
    ../../xpiks-qt/QuickBuffer/currenteditableproxyartwork.h \
    ../../xpiks-qt/QuickBuffer/icurrenteditable.h \
    ../../xpiks-qt/QuickBuffer/quickbuffer.h \
    ../../xpiks-qt/Models/artworkproxymodel.h \
    ../../xpiks-qt/Models/uimanager.h \
    ../../xpiks-qt/KeywordsPresets/ipresetsmanager.h \
    ../../xpiks-qt/QMLExtensions/tabsmodel.h \
    ../../xpiks-qt/Models/videoartwork.h

