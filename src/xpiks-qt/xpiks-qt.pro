TEMPLATE = app

QT += qml quick widgets concurrent

SOURCES += main.cpp \
    Models/artitemsmodel.cpp \
    Models/artworkmetadata.cpp \
    Helpers/globalimageprovider.cpp \
    Models/artworksrepository.cpp \
    Models/combinedartworksmodel.cpp \
    Helpers/indiceshelper.cpp \
    Models/iptcprovider.cpp \
    Helpers/externaltoolsprovider.cpp \
    Models/artworkuploader.cpp \
    Models/artworksprocessor.cpp \
    Models/uploadinforepository.cpp \
    Helpers/tempmetadatadb.cpp \
    Models/warningsmanager.cpp \
    ../tiny-aes/aes.cpp \
    Encryption/secretsmanager.cpp \
    Helpers/stringhelper.cpp \
    Commands/commandmanager.cpp \
    UndoRedo/undoredomanager.cpp \
    Commands/addartworkscommand.cpp

RESOURCES += qml.qrc

BUILDNO = $$system(git log -n 1 --pretty=format:"%H")
DEFINES += BUILDNUMBER=$${BUILDNO}
DEFINES += QT_NO_CAST_TO_ASCII \
           QT_NO_CAST_FROM_BYTEARRAY

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
    Models/iptcprovider.h \
    Helpers/externaltoolsprovider.h \
    Helpers/exiftoolwrapper.h \
    Models/artworkuploader.h \
    Helpers/curlwrapper.h \
    Models/artworksprocessor.h \
    Models/uploadinfo.h \
    Models/exportinfo.h \
    Models/uploadinforepository.h \
    Models/logsmodel.h \
    Helpers/tempmetadatadb.h \
    Models/warningsinfo.h \
    Models/warningsmanager.h \
    Encryption/aes-qt.h \
    ../tiny-aes/aes.h \
    Encryption/secretsmanager.h \
    Helpers/uploaditem.h \
    Helpers/stringhelper.h \
    Helpers/logger.h \
    Helpers/testconnectionresult.h \
    Commands/commandmanager.h \
    UndoRedo/historyitem.h \
    UndoRedo/undoredomanager.h \
    UndoRedo/addartworksitem.h \
    Commands/commandbase.h \
    Commands/addartworkscommand.h \
    Common/baseentity.h

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
    Dialogs/UndoPopup.qml

macx {
OBJECTIVE_SOURCES += \
    Helpers/osxnsurlhelper.mm

LIBS += -framework Foundation
HEADERS += Helpers/osxnsurlhelper.h
}
