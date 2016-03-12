TEMPLATE = app

QMAKE_MAC_SDK = macosx10.11

QT += qml quick widgets concurrent svg
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
    Conectivity/ftphelpers.cpp \
    Plugins/pluginmanager.cpp \
    Plugins/pluginwrapper.cpp \
    Plugins/pluginactionsmodel.cpp \
    Plugins/uiprovider.cpp \
    Warnings/warningsservice.cpp \
    Helpers/loghighlighter.cpp \
    Warnings/warningsmodel.cpp \
    Models/languagesmodel.cpp \
    Conectivity/conectivityhelpers.cpp \
    Helpers/filterhelpers.cpp

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

TRANSLATIONS = translations\xpiks_lang_en_US.ts \
               translations\xpiks_lang_uk_UA.ts

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
    Conectivity/ftphelpers.h \
    Common/isafepointer.h \
    Plugins/xpiksplugininterface.h \
    Commands/icommandmanager.h \
    Commands/icommandbase.h \
    UndoRedo/ihistoryitem.h \
    UndoRedo/iundoredomanager.h \
    Plugins/pluginmanager.h \
    Plugins/pluginwrapper.h \
    Plugins/ipluginaction.h \
    Plugins/pluginactionsmodel.h \
    Plugins/uiprovider.h \
    Plugins/iuiprovider.h \
    Common/ibasicartwork.h \
    Common/iartworkssource.h \
    Warnings/warningsservice.h \
    Common/iservicebase.h \
    Helpers/loghighlighter.h \
    Warnings/warningsmodel.h \
    Models/languagesmodel.h \
    Conectivity/conectivityhelpers.h \
    Conectivity/uploadbatch.h \
    Helpers/filterhelpers.h \
    Helpers/ifilenotavailablemodel.h
    Conectivity/iftpcoordinator.h

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
    Dialogs/TermsAndConditionsDialog.qml \
    Dialogs/PluginsDialog.qml \
    Components/LayoutButton.qml \
    Graphics/vector-icon.svg \
    Constants/UIConfig.js

lupdate_only {
SOURCES = *.qml \
          *.js \
          Components/*.qml \
          Dialogs/*.qml \
          StyledControls/*.qml
}

INCLUDEPATH += ../tiny-aes

LIBS += -L"$$PWD/../libs/"
LIBS += -lhunspell
LIBS += -lz
LIBS += -lcurl
LIBS += -lquazip

CONFIG(debug, debug|release)  {
    message("Building debug")
} else {
    message("Building release")
}

macx {
    INCLUDEPATH += "../hunspell-1.3.3/src"
    INCLUDEPATH += "../quazip"
    INCLUDEPATH += "../../libcurl/include"

    HUNSPELL_DICT_FILES.files = dict/en_US.aff dict/en_US.dic dict/license.txt dict/README_en_US.txt
    HUNSPELL_DICT_FILES.path = Contents/Resources
    QMAKE_BUNDLE_DATA += HUNSPELL_DICT_FILES

    WHATS_NEW.files = whatsnew.txt
    WHATS_NEW.path = Contents/Resources
    QMAKE_BUNDLE_DATA += WHATS_NEW

    TERMS_AND_CONDITIONS.files = terms_and_conditions.txt
    TERMS_AND_CONDITIONS.path = Contents/Resources
    QMAKE_BUNDLE_DATA += TERMS_AND_CONDITIONS

    TRANSLATIONS_FILES_LIST = $$system(ls $$PWD/translations/*.qm)
    XPIKS_TRANSLATIONS.files = $$TRANSLATIONS_FILES_LIST
    XPIKS_TRANSLATIONS.path = Contents/Resources/translations
    QMAKE_BUNDLE_DATA += XPIKS_TRANSLATIONS
}

win32 {
    QT += winextras
    INCLUDEPATH += "../zlib-1.2.8"
    INCLUDEPATH += "../hunspell-1.3.3/src"
    INCLUDEPATH += "../quazip"
    INCLUDEPATH += "../libcurl/include"
    LIBS -= -lcurl

    CONFIG(debug, debug|release) {
	EXE_DIR = debug
        LIBS += -llibcurl_debug
    }

    CONFIG(release, debug|release) {
        EXE_DIR = release
        LIBS += -llibcurl
    }

    copywhatsnew.commands = $(COPY_FILE) \"$$shell_path($$PWD/whatsnew.txt)\" \"$$shell_path($$OUT_PWD/$$EXE_DIR/)\"
    copyterms.commands = $(COPY_FILE) \"$$shell_path($$PWD/terms_and_conditions.txt)\" \"$$shell_path($$OUT_PWD/$$EXE_DIR/)\"
    copydicts.commands = $(COPY_DIR) \"$$shell_path($$PWD/dict)\" \"$$shell_path($$OUT_PWD/$$EXE_DIR/dict)\"

    copytranslations.commands = $(COPY_FILE) \"$$shell_path($$PWD/translations/xpiks_*.qm)\" \"$$shell_path($$OUT_PWD/$$EXE_DIR/translations/)\"

    QMAKE_EXTRA_TARGETS += copywhatsnew copyterms copydicts copytranslations
    POST_TARGETDEPS += copywhatsnew copyterms copydicts copytranslations
}

travis-ci {
    message("for Travis CI")
    LIBS -= -lz
    LIBS += /usr/lib/x86_64-linux-gnu/libz.so
}

linux-g++-64 {
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

linux-qtcreator {
        message("in QtCreator")
        LIBS += -L/usr/lib64/
        LIBS += /usr/lib64/libcurl.so.4
        copywhatsnew.commands = $(COPY_FILE) "$$PWD/whatsnew.txt" "$$OUT_PWD/"
        copyterms.commands = $(COPY_FILE) "$$PWD/terms_and_conditions.txt" "$$OUT_PWD/"
        QMAKE_EXTRA_TARGETS += copywhatsnew copyterms
	POST_TARGETDEPS += copywhatsnew copyterms
}

linux-static {
    CONFIG += static
    QTPLUGIN += qt5quick
    DEFINES += STATIC
    message("Static build.")
}
