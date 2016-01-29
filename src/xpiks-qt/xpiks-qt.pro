TEMPLATE = app

QMAKE_MAC_SDK = macosx10.11

QT += qml quick widgets concurrent
CONFIG += qtquickcompiler

SOURCES += main.cpp \
    Models/artitemsmodel.cpp \
    Models/artworkmetadata.cpp \
    Helpers/globalimageprovider.cpp \
    Models/artworksrepository.cpp \
    Models/combinedartworksmodel.cpp \
    Helpers/indiceshelper.cpp \
    Models/artworkuploader.cpp \
    Models/artworksprocessor.cpp \
    Models/uploadinforepository.cpp \
    Models/warningsmanager.cpp \
    ../tiny-aes/aes.cpp \
    Encryption/secretsmanager.cpp \
    Helpers/stringhelper.cpp \
    Commands/commandmanager.cpp \
    UndoRedo/undoredomanager.cpp \
    Commands/addartworkscommand.cpp \
    UndoRedo/addartworksitem.cpp \
    Commands/removeartworkscommand.cpp \
    UndoRedo/removeartworksitem.cpp \
    UndoRedo/artworkmetadatabackup.cpp \
    UndoRedo/modifyartworkshistoryitem.cpp \
    Commands/combinededitcommand.cpp \
    Commands/pastekeywordscommand.cpp \
    Helpers/runguard.cpp \
    Encryption/aes-qt.cpp \
    Models/ziparchiver.cpp \
    Helpers/ziphelper.cpp \
    Suggestion/keywordssuggestor.cpp \
    Suggestion/suggestionqueryengine.cpp \
    Models/settingsmodel.cpp \
    Helpers/loggingworker.cpp \
    Helpers/logger.cpp \
    Models/logsmodel.cpp \
    Models/filteredartitemsproxymodel.cpp \
    Helpers/filenameshelpers.cpp \
    Helpers/keywordvalidator.cpp \
    Helpers/helpersqmlwrapper.cpp \
    Models/recentdirectoriesmodel.cpp \
    Suggestion/locallibrary.cpp \
    Suggestion/libraryqueryworker.cpp \
    Suggestion/libraryloaderworker.cpp \
    Helpers/updateservice.cpp \
    SpellCheck/spellcheckerservice.cpp \
    SpellCheck/spellcheckitem.cpp \
    SpellCheck/spellcheckworker.cpp \
    SpellCheck/spellchecksuggestionmodel.cpp \
    Common/basickeywordsmodel.cpp \
    SpellCheck/spellcheckerrorshighlighter.cpp \
    SpellCheck/spellcheckiteminfo.cpp \
    MetadataIO/backupsaverworker.cpp \
    MetadataIO/backupsaverservice.cpp \
    SpellCheck/spellsuggestionsitem.cpp \
    Conectivity/telemetryservice.cpp \
    Conectivity/updatescheckerworker.cpp \
    Warnings/warningscheckingworker.cpp \
    MetadataIO/metadatareadingworker.cpp \
    MetadataIO/metadataiocoordinator.cpp \
    MetadataIO/saverworkerjobitem.cpp \
    MetadataIO/metadatawritingworker.cpp \
    Conectivity/curlftpuploader.cpp \
    Conectivity/ftpuploaderworker.cpp \
    Conectivity/ftpcoordinator.cpp \
    Conectivity/testconnection.cpp \
    Conectivity/ftphelpers.cpp

RESOURCES += qml.qrc

BUILDNO = $$system(git log -n 1 --pretty=format:"%H")
DEFINES += BUILDNUMBER=$${BUILDNO}
DEFINES += QT_NO_CAST_TO_ASCII \
           QT_NO_CAST_FROM_BYTEARRAY
DEFINES += QUAZIP_STATIC
DEFINES += HUNSPELL_STATIC
DEFINES += TELEMETRY_ENABLED

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

ICON = xpiks-qt.icns

RC_ICONS = xpiks-qt.ico

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    Models/artitemsmodel.h \
    Models/artworkmetadata.h \
    Helpers/globalimageprovider.h \
    Models/artworksrepository.h \
    Helpers/indiceshelper.h \
    Helpers/clipboardhelper.h \
    Models/combinedartworksmodel.h \
    Models/artiteminfo.h \
    Models/abstractlistmodel.h \
    Helpers/constants.h \
    Helpers/appsettings.h \
    Models/artworkuploader.h \
    Models/artworksprocessor.h \
    Models/uploadinfo.h \
    Models/exportinfo.h \
    Models/uploadinforepository.h \
    Models/logsmodel.h \
    Models/warningsinfo.h \
    Models/warningsmanager.h \
    Encryption/aes-qt.h \
    ../tiny-aes/aes.h \
    Encryption/secretsmanager.h \
    Helpers/stringhelper.h \
    Helpers/logger.h \
    Commands/commandmanager.h \
    UndoRedo/historyitem.h \
    UndoRedo/undoredomanager.h \
    UndoRedo/addartworksitem.h \
    Commands/commandbase.h \
    Commands/addartworkscommand.h \
    Common/baseentity.h \
    Commands/removeartworkscommand.h \
    UndoRedo/removeartworksitem.h \
    UndoRedo/artworkmetadatabackup.h \
    UndoRedo/modifyartworkshistoryitem.h \
    Commands/combinededitcommand.h \
    Commands/pastekeywordscommand.h \
    Helpers/runguard.h \
    Models/ziparchiver.h \
    Helpers/ziphelper.h \
    Common/basickeywordsmodel.h \
    Suggestion/keywordssuggestor.h \
    Suggestion/suggestionartwork.h \
    Suggestion/suggestionqueryengine.h \
    Models/settingsmodel.h \
    Helpers/loggingworker.h \
    Common/defines.h \
    Models/filteredartitemsproxymodel.h \
    Helpers/filenameshelpers.h \
    Common/flags.h \
    Helpers/keywordvalidator.h \
    Helpers/helpersqmlwrapper.h \
    Models/recentdirectoriesmodel.h \
    Helpers/settingsprovider.h \
    Common/version.h \
    Suggestion/locallibrary.h \
    Suggestion/libraryqueryworker.h \
    Suggestion/libraryloaderworker.h \
    Helpers/updateservice.h \
    SpellCheck/spellcheckerservice.h \
    SpellCheck/spellcheckitem.h \
    SpellCheck/spellcheckworker.h \
    SpellCheck/spellchecksuggestionmodel.h \
    SpellCheck/ispellcheckable.h \
    SpellCheck/spellcheckerrorshighlighter.h \
    SpellCheck/spellcheckiteminfo.h \
    MetadataIO/backupsaverworker.h \
    Common/itemprocessingworker.h \
    MetadataIO/backupsaverservice.h \
    SpellCheck/spellsuggestionsitem.h \
    Conectivity/analyticsuserevent.h \
    Conectivity/telemetryservice.h \
    Conectivity/updatescheckerworker.h \
    Warnings/warningscheckingworker.h \
    Warnings/iwarningscheckable.h \
    Warnings/warningsitem.h \
    MetadataIO/saverworkerjobitem.h \
    MetadataIO/metadatareadingworker.h \
    MetadataIO/metadataiocoordinator.h \
    MetadataIO/metadatawritingworker.h \
    Conectivity/curlftpuploader.h \
    Conectivity/ftpuploaderworker.h \
    Conectivity/ftpcoordinator.h \
    Conectivity/uploadcontext.h \
    Conectivity/testconnection.h \
    Conectivity/ftphelpers.h

DISTFILES += \
    Components/CloseIcon.qml \
    Components/EditableTags.qml \
    Components/LargeRemoveIcon.qml \
    Constants/Colors.js \
    StyledControls/SimpleProgressBar.qml \
    StyledControls/StyledButton.qml \
    StyledControls/StyledCheckbox.qml \
    StyledControls/StyledScrollView.qml \
    StyledControls/StyledText.qml \
    StyledControls/StyledTextInput.qml \
    Dialogs/CombinedArtworksDialog.qml \
    Dialogs/ExportMetadata.qml \
    Dialogs/ImportMetadata.qml \
    Dialogs/SettingsWindow.qml \
    Dialogs/UploadArtworks.qml \
    Dialogs/LogsDialog.qml \
    StyledControls/StyledTextEdit.qml \
    Dialogs/WarningsDialog.qml \
    xpiks-qt.ico \
    Dialogs/AboutWindow.qml \
    StyledControls/StyledAddHostButton.qml \
    StyledControls/StyledInputHost.qml \
    Dialogs/EnterMasterPasswordDialog.qml \
    Dialogs/MasterPasswordSetupDialog.qml \
    Components/CheckedComponent.qml \
    Dialogs/ZipArtworksDialog.qml \
    Components/AddIcon.qml \
    Dialogs/KeywordsSuggestion.qml \
    Components/LargeAddIcon.qml \
    StyledControls/StyledBusyIndicator.qml \
    StyledControls/StyledTabView.qml \
    Components/CustomBorder.qml \
    Dialogs/ArtworkPreview.qml \
    StyledControls/StyledBlackButton.qml \
    StyledControls/StyledSlider.qml \
    Components/KeywordWrapper.qml \
    Components/CustomScrollbar.qml \
    Dialogs/EditArtworkVerticalDialog.qml \
    Dialogs/EditArtworkHorizontalDialog.qml \
    Dialogs/UpdateWindow.qml \
    Dialogs/SpellCheckDialog.qml \
    Dialogs/SpellCheckSuggestionsDialog.qml \
    Components/SuggestionWrapper.qml \
    Dialogs/EditKeywordDialog.qml \
    Dialogs/PlainTextKeywordsDialog.qml \
    Dialogs/WhatsNewDialog.qml \
    Dialogs/TermsAndConditionsDialog.qml

LIBS += -L"$$PWD/../libs/"
LIBS += -lhunspell
LIBS += -lz
LIBS += -lcurl
LIBS += -lquazip

CONFIG(debug, debug|release)  {
    message("Debug build.")
} else {
    message("Building release")
}

macx {
    INCLUDEPATH += "../hunspell-1.3.3/src"
    INCLUDEPATH += "../quazip/quazip/"
    INCLUDEPATH += "../../libcurl/include"
}

win32 {
    INCLUDEPATH += "../zlib-1.2.8"
    INCLUDEPATH += "../hunspell-1.3.3/src"
    INCLUDEPATH += "../quazip/quazip/"
    INCLUDEPATH += "../../libcurl/include"
    LIBS -= -lcurl
    LIBS += -llibcurl_debug
}

linux-g++-64 {
    target.path=/usr/bin/
    QML_IMPORT_PATH += /usr/lib/x86_64-linux-gnu/qt5/imports/
    UNAME = $$system(cat /proc/version)

    contains(UNAME, Debian): {
        message("on Debian Linux")
        LIBS += -L/lib/x86_64-linux-gnu/
        LIBS -= -lquazip # temporary static link
        LIBS += /usr/lib/x86_64-linux-gnu/libquazip-qt5.a
    }
    contains(UNAME, SUSE): {
        message("on SUSE Linux")
        LIBS += -L/usr/lib64/
        LIBS += /usr/lib64/libcurl.so.4
    }
}

linux-static {
    CONFIG += static
    QTPLUGIN += qt5quick
    DEFINES += STATIC
    message("Static build.")
}

HUNSPELL_DICT_FILES.files = dict/en_US.aff dict/en_US.dic dict/license.txt dict/README_en_US.txt
HUNSPELL_DICT_FILES.path = Contents/Resources
QMAKE_BUNDLE_DATA += HUNSPELL_DICT_FILES

WHATS_NEW.files = whatsnew.txt
WHATS_NEW.path = Contents/Resources
QMAKE_BUNDLE_DATA += WHATS_NEW

TERMS_AND_CONDITIONS.files = terms_and_conditions.txt
TERMS_AND_CONDITIONS.path = Contents/Resources
QMAKE_BUNDLE_DATA += TERMS_AND_CONDITIONS
