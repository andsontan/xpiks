TEMPLATE = app

QMAKE_MAC_SDK = macosx10.11

QT += qml quick widgets concurrent svg
CONFIG += qtquickcompiler
CONFIG += c++11

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
    Models/settingsmodel.cpp \
    Helpers/loggingworker.cpp \
    Helpers/logger.cpp \
    Models/logsmodel.cpp \
    Models/filteredartitemsproxymodel.cpp \
    Helpers/filenameshelpers.cpp \
    Helpers/helpersqmlwrapper.cpp \
    Models/recentdirectoriesmodel.cpp \
    Suggestion/locallibrary.cpp \
    Suggestion/libraryqueryworker.cpp \
    Suggestion/libraryloaderworker.cpp \
    Conectivity/updateservice.cpp \
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
    Helpers/filterhelpers.cpp \
    QMLExtensions/triangleelement.cpp \
    Suggestion/shutterstockqueryengine.cpp \
    Suggestion/locallibraryqueryengine.cpp \
    Suggestion/fotoliaqueryengine.cpp \
    QMLExtensions/colorsmodel.cpp \
    Helpers/remoteconfig.cpp \
    Helpers/localconfig.cpp \
    Helpers/jsonhelper.cpp \
    AutoComplete/autocompletemodel.cpp \
    AutoComplete/autocompleteworker.cpp \
    AutoComplete/autocompleteservice.cpp \
    Suggestion/gettyqueryengine.cpp \
    Models/abstractconfigupdatermodel.cpp \
    AutoComplete/stocksftplistmodel.cpp \
    AutoComplete/stringfilterproxymodel.cpp \
    Models/imageartwork.cpp \
    MetadataIO/exiv2readingworker.cpp \
    MetadataIO/readingorchestrator.cpp \
    MetadataIO/exiv2writingworker.cpp \
    MetadataIO/writingorchestrator.cpp \
    Common/flags.cpp \
    Models/proxysettings.cpp \
    QMLExtensions/imagecachingworker.cpp \
    QMLExtensions/imagecachingservice.cpp \
    QMLExtensions/cachingimageprovider.cpp \
    Helpers/deletelogshelper.cpp \
    Commands/findandreplacecommand.cpp \
    Helpers/metadatahighlighter.cpp \
    Models/findandreplacemodel.cpp \
    Commands/deletekeywordscommand.cpp \
    Models/deletekeywordsviewmodel.cpp \
    Models/artworksviewmodel.cpp \
    Helpers/keywordshelpers.cpp \
    Conectivity/uploadwatcher.cpp \
    Conectivity/telemetryworker.cpp \
    Warnings/warningssettingsmodel.cpp \
    Conectivity/simplecurlrequest.cpp \
    Conectivity/curlinithelper.cpp \
    MetadataIO/exiv2inithelper.cpp \
    Conectivity/simplecurldownloader.cpp \
    Helpers/updatehelpers.cpp \
    Common/basicmetadatamodel.cpp \
    PresetKeywords/presetkeywordsmodel.cpp \
    PresetKeywords/presetkeywordsmodelconfig.cpp \
    QMLExtensions/folderelement.cpp \
    Models/artworkproxymodel.cpp \
    Models/artworkproxybase.cpp

RESOURCES += qml.qrc

DEFINES += QT_NO_CAST_TO_ASCII \
           QT_RESTRICTED_CAST_FROM_ASCII \
           QT_NO_CAST_FROM_BYTEARRAY
DEFINES += HUNSPELL_STATIC
DEFINES += LIBFACE_STATIC
DEFINES += QT_MESSAGELOGCONTEXT

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
    Common/abstractlistmodel.h \
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
    Models/settingsmodel.h \
    Helpers/loggingworker.h \
    Common/defines.h \
    Models/filteredartitemsproxymodel.h \
    Helpers/filenameshelpers.h \
    Common/flags.h \
    Helpers/helpersqmlwrapper.h \
    Models/recentdirectoriesmodel.h \
    Common/version.h \
    Suggestion/locallibrary.h \
    Suggestion/libraryqueryworker.h \
    Suggestion/libraryloaderworker.h \
    Conectivity/updateservice.h \
    SpellCheck/spellcheckerservice.h \
    SpellCheck/spellcheckitem.h \
    SpellCheck/spellcheckworker.h \
    SpellCheck/spellchecksuggestionmodel.h \
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
    Conectivity/iftpcoordinator.h \
    QMLExtensions/triangleelement.h \
    Suggestion/shutterstockqueryengine.h \
    Suggestion/locallibraryqueryengine.h \
    Suggestion/suggestionqueryenginebase.h \
    Helpers/ifilenotavailablemodel.h \
    Suggestion/fotoliaqueryengine.h \
    QMLExtensions/colorsmodel.h \
    Helpers/remoteconfig.h \
    Helpers/localconfig.h \
    Helpers/jsonhelper.h \
    Helpers/comparevaluesjson.h \
    AutoComplete/autocompletemodel.h \
    AutoComplete/autocompleteworker.h \
    AutoComplete/completionquery.h \
    AutoComplete/autocompleteservice.h \
    Suggestion/gettyqueryengine.h \
    Models/abstractconfigupdatermodel.h \
    AutoComplete/stocksftplistmodel.h \
    AutoComplete/stringfilterproxymodel.h \
    Models/imageartwork.h \
    Common/hold.h \
    MetadataIO/exiv2readingworker.h \
    MetadataIO/importdataresult.h \
    MetadataIO/readingorchestrator.h \
    MetadataIO/imetadatareader.h \
    MetadataIO/exiv2writingworker.h \
    MetadataIO/writingorchestrator.h \
    MetadataIO/exiv2tagnames.h \
    MetadataIO/imetadatawriter.h \
    Models/proxysettings.h \
    QMLExtensions/imagecachingworker.h \
    QMLExtensions/imagecacherequest.h \
    QMLExtensions/imagecachingservice.h \
    QMLExtensions/cachingimageprovider.h \
    Helpers/deletelogshelper.h \
    Commands/findandreplacecommand.h \
    Models/metadataelement.h \
    Models/previewmetadataelement.h \
    Helpers/metadatahighlighter.h \
    Models/findandreplacemodel.h \
    Commands/deletekeywordscommand.h \
    Models/deletekeywordsviewmodel.h \
    Models/artworksviewmodel.h \
    Helpers/keywordshelpers.h \
    Conectivity/uploadwatcher.h \
    Common/iflagsprovider.h \
    Conectivity/telemetryworker.h \
    Warnings/warningssettingsmodel.h \
    Conectivity/simplecurlrequest.h \
    Conectivity/curlinithelper.h \
    MetadataIO/exiv2inithelper.h \
    Conectivity/simplecurldownloader.h \
    Conectivity/apimanager.h \
    Helpers/updatehelpers.h \
    Common/basicmetadatamodel.h \
    PresetKeywords/presetkeywordsmodel.h \
    PresetKeywords/presetkeywordsmodelconfig.h \
    QMLExtensions/folderelement.h \
    Models/artworkproxymodel.h \
    Models/artworkproxybase.h \
    Common/imetadataoperator.h

DISTFILES += \
    Components/CloseIcon.qml \
    Components/EditableTags.qml \
    StyledControls/SimpleProgressBar.qml \
    StyledControls/StyledButton.qml \
    StyledControls/StyledCheckbox.qml \
    StyledControls/StyledScrollView.qml \
    StyledControls/StyledText.qml \
    StyledControls/StyledTextInput.qml \
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
    StyledControls/StyledBlackButton.qml \
    StyledControls/StyledSlider.qml \
    Components/KeywordWrapper.qml \
    Components/CustomScrollbar.qml \
    Dialogs/UpdateWindow.qml \
    Dialogs/SpellCheckSuggestionsDialog.qml \
    Components/SuggestionWrapper.qml \
    Dialogs/EditKeywordDialog.qml \
    Dialogs/PlainTextKeywordsDialog.qml \
    Dialogs/WhatsNewDialog.qml \
    Dialogs/TermsAndConditionsDialog.qml \
    Dialogs/PluginsDialog.qml \
    Components/LayoutButton.qml \
    Graphics/vector-icon.svg \
    Constants/UIConfig.js \
    Components/SelectedIcon.qml \
    Components/CustomComboBox.qml \
    Components/CompletionBox.qml \
    Components/EditIcon.qml \
    uncrustify.cfg \
    Components/PresentationSlide.qml \
    Graphics/autocomplete.png \
    Graphics/gears.png \
    Graphics/localization.png \
    Graphics/newtheme.png \
    Graphics/predefinedftp.png \
    Graphics/suggestion.png \
    Dialogs/ProxySetupDialog.qml \
    Dialogs/WhatsNewMinorDialog.qml \
    Dialogs/FindAndReplace.qml \
    Dialogs/ReplacePreview.qml \
    Dialogs/DeleteKeywordsDialog.qml \
    Dialogs/FailedUploadArtworks.qml \
    Dialogs/InstallUpdateDialog.qml \
    Dialogs/PresetEditDialog.qml \
    StyledControls/StyledMainTabView.qml \
    Components/CustomTab.qml \
    StackViews/MainGrid.qml \
    StackViews/ArtworkEditView.qml \
    Components/ZoomAmplifier.qml \
    StackViews/WarningsView.qml \
    Components/GlyphButton.qml \
    Components/BackGlyphButton.qml \
    StackViews/CombinedEditView.qml

lupdate_only {
SOURCES += *.qml \
          *.js \
          Components/*.qml \
          Dialogs/*.qml \
          StyledControls/*.qml
}

INCLUDEPATH += ../tiny-aes
INCLUDEPATH += ../cpp-libface

LIBS += -L"$$PWD/../libs/"
LIBS += -lhunspell
LIBS += -lz
LIBS += -lcurl
LIBS += -lquazip
LIBS += -lface
LIBS += -lexiv2

BUILDNO=$$system(git log -n 1 --pretty=format:"%h")

CONFIG(debug, debug|release)  {
    message("Building debug")
    #QMAKE_CXXFLAGS += -fsanitize=thread
} else {
    message("Building release")
}

macx {
    LIBS += -liconv
    LIBS += -lexpat

    INCLUDEPATH += "../hunspell-1.3.3/src"
    INCLUDEPATH += "../quazip"
    INCLUDEPATH += "../../libcurl/include"
    INCLUDEPATH += ../exiv2-0.25/include

    LIBS += -lxmpsdk

    HUNSPELL_DICT_FILES.files = deps/dict/en_US.aff deps/dict/en_US.dic deps/dict/license.txt deps/dict/README_en_US.txt
    HUNSPELL_DICT_FILES.path = Contents/Resources
    QMAKE_BUNDLE_DATA += HUNSPELL_DICT_FILES

    WHATS_NEW.files = deps/whatsnew.txt
    WHATS_NEW.path = Contents/Resources
    QMAKE_BUNDLE_DATA += WHATS_NEW

    TERMS_AND_CONDITIONS.files = deps/terms_and_conditions.txt
    TERMS_AND_CONDITIONS.path = Contents/Resources
    QMAKE_BUNDLE_DATA += TERMS_AND_CONDITIONS

    TRANSLATIONS_FILES_LIST = $$system(ls $$PWD/deps/translations/*.qm)
    XPIKS_TRANSLATIONS.files = $$TRANSLATIONS_FILES_LIST
    XPIKS_TRANSLATIONS.path = Contents/Resources/translations
    QMAKE_BUNDLE_DATA += XPIKS_TRANSLATIONS

    FREQ_TABLES.files = deps/en_wordlist.tsv
    FREQ_TABLES.path = Contents/Resources
    QMAKE_BUNDLE_DATA += FREQ_TABLES
}

win32 {
    DEFINES += QT_NO_PROCESS_COMBINED_ARGUMENT_START
    QT += winextras
    INCLUDEPATH += "../zlib-1.2.8"
    INCLUDEPATH += "../hunspell-1.3.3/src"
    INCLUDEPATH += "../quazip"
    INCLUDEPATH += "../libcurl/include"
    INCLUDEPATH += "../exiv2-0.25/include"

    LIBS -= -lcurl
    LIBS -= -lexiv2

    CONFIG(debug, debug|release) {
	EXE_DIR = debug
        LIBS += -llibcurl_debug
    }

    CONFIG(release, debug|release) {
        EXE_DIR = release
        LIBS += -llibcurl
    }

    LIBS += -lmman
    LIBS += -llibexpat
    LIBS += -llibexiv2

    copywhatsnew.commands = $(COPY_FILE) \"$$shell_path($$PWD/deps/whatsnew.txt)\" \"$$shell_path($$OUT_PWD/$$EXE_DIR/)\"
    copyterms.commands = $(COPY_FILE) \"$$shell_path($$PWD/deps/terms_and_conditions.txt)\" \"$$shell_path($$OUT_PWD/$$EXE_DIR/)\"
    copydicts.commands = $(COPY_DIR) \"$$shell_path($$PWD/deps/dict)\" \"$$shell_path($$OUT_PWD/$$EXE_DIR/dict)\"

    appveyor {
        DEFINES += WITH_LOGS

        copytranslations.commands = echo "Skip translations"
        create_ac_sources.commands = $(MKDIR) \"$$shell_path($$OUT_PWD/$$EXE_DIR/ac_sources)\"
        QMAKE_EXTRA_TARGETS += create_ac_sources
        POST_TARGETDEPS += create_ac_sources
    } else {
        copytranslations.commands = $(COPY_FILE) \"$$shell_path($$PWD/deps/translations/xpiks_*.qm)\" \"$$shell_path($$OUT_PWD/$$EXE_DIR/translations/)\"
    }

    copyfreqtables.commands = $(COPY_FILE) \"$$shell_path($$PWD/deps/en_wordlist.tsv)\" \"$$shell_path($$OUT_PWD/$$EXE_DIR/ac_sources/en_wordlist.tsv)\"

    QMAKE_EXTRA_TARGETS += copywhatsnew copyterms copydicts copytranslations copyfreqtables
    POST_TARGETDEPS += copywhatsnew copyterms copydicts copytranslations copyfreqtables
}

travis-ci {
    message("for Travis CI")
    LIBS -= -lz
    LIBS += /usr/lib/x86_64-linux-gnu/libz.so
    DEFINES += TRAVIS_CI
    INCLUDEPATH += "../quazip"
}

linux-g++-64 {
    message("for Linux")
    target.path=/usr/bin/
    QML_IMPORT_PATH += /usr/lib/x86_64-linux-gnu/qt5/imports/
    LIBS += -L/lib/x86_64-linux-gnu/
    BUILDNO = $$system($$PWD/buildno.sh)

    #DEFINES += TELEMETRY_DISABLED

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
    BUILDNO = $$system($$PWD/buildno.sh)
    LIBS += -L/usr/lib64/
    LIBS += -L/lib/x86_64-linux-gnu/
    copywhatsnew.commands = $(COPY_FILE) "$$PWD/deps/whatsnew.txt" "$$OUT_PWD/"
    copyterms.commands = $(COPY_FILE) "$$PWD/deps/terms_and_conditions.txt" "$$OUT_PWD/"
    QMAKE_EXTRA_TARGETS += copywhatsnew copyterms
    POST_TARGETDEPS += copywhatsnew copyterms
}

linux-static {
    CONFIG += static
    QTPLUGIN += qt5quick
    DEFINES += STATIC
    message("Static build.")
}

DEFINES += BUILDNUMBER=$${BUILDNO}
