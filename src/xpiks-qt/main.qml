/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2016 Taras Kushnir <kushnirTV@gmail.com>
 *
 * Xpiks is distributed under the GNU General Public License, version 3.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.2
import QtQuick.Dialogs 1.1
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0
import xpiks 1.0
import "Constants" 1.0
import "Components"
import "StyledControls"
import "Dialogs"
import "Common.js" as Common
import "Constants/UIConfig.js" as UIConfig
import "Constants/Themes.js" as Themes

ApplicationWindow {
    id: applicationWindow
    visible: true
    width: appSettings.getAppWidth(930)
    height: appSettings.getAppHeight(725)
    minimumHeight: 725
    minimumWidth: 930
    title: i18.n + (debug ? "Xpiks (Devel)" : qsTr("Xpiks"))
    property int openedDialogsCount: 0
    property bool showUpdateLink: false
    property bool needToCenter: true
    property bool listLayout: true
    property bool initializedColors: false
    property var spellCheckService: helpersWrapper.getSpellCheckerService()

    onVisibleChanged: {
        if (!initializedColors) {
            initializedColors = true

            Colors.registerTheme(Themes.BlackTheme);
            Colors.registerTheme(Themes.SlateGrayTheme)

            Colors.applyTheme(settingsModel.selectedThemeIndex)
        }

        if (needToCenter) {
            needToCenter = false
            applicationWindow.x = appSettings.getAppPosX((Screen.width - applicationWindow.width) / 2)
            applicationWindow.y = appSettings.getAppPosY((Screen.height - applicationWindow.height) / 2)
        }
    }

    function saveRecentDirectories() {
        appSettings.setValue(appSettings.recentDirectoriesKey, recentDirectories.serializeForSettings())
    }

    function closeHandler(close) {
        console.info("closeHandler")
        saveRecentDirectories()

        if (artItemsModel.modifiedArtworksCount > 0) {
            close.accepted = false
            configExitDialog.open()
        } else {
            console.debug("No modified artworks found. Exiting...")
            shutdownEverything()
            close.accepted = false
        }
    }

    function shutdownEverything() {
        applicationWindow.visibility = "Minimized"
        helpersWrapper.beforeDestruction();
        appSettings.protectTelemetry();
        saveAppGeometry()
        closingTimer.start()
    }

    function saveAppGeometry() {
        console.debug("Saving application geometry")
        appSettings.setAppWidth(applicationWindow.width)
        appSettings.setAppHeight(applicationWindow.height)
        appSettings.setAppPosX(applicationWindow.x)
        appSettings.setAppPosY(applicationWindow.y)
    }

    function closeAutoComplete() {
        if (typeof workflowHost.autoCompleteBox !== "undefined") {
            workflowHost.autoCompleteBox.closePopup()
        }
    }

    onClosing: closeHandler(close)

    Timer {
        id: closingTimer
        interval: 1000
        running: false
        repeat: false
        onTriggered: Qt.quit()
    }

    Timer {
        id: openingTimer
        interval: 500
        running: false
        repeat: false
        onTriggered: {
            console.debug("Delayed onOpen timer triggered");
            helpersWrapper.setProgressIndicator(applicationWindow)

            if (appSettings.needToShowWhatsNew()) {
                Common.launchDialog("Dialogs/WhatsNewDialog.qml", applicationWindow, {})
            } else if (appSettings.needToShowTextWhatsNew()) {
                var text = appSettings.whatsNewText;
                if (text.length > 0) {
                    Common.launchDialog("Dialogs/WhatsNewMinorDialog.qml",
                                        applicationWindow,
                                        {
                                            whatsNewText: text
                                        })
                }
            }

            if (appSettings.needToShowTermsAndConditions()) {
                var licenseText = appSettings.termsAndConditionsText;
                if (licenseText.length > 0) {
                    Common.launchDialog("Dialogs/TermsAndConditionsDialog.qml",
                                        applicationWindow,
                                        {
                                            termsText: licenseText
                                        })
                }
            } else {
                helpersWrapper.reportOpen()
            }
        }
    }

    function onDialogClosed() {
        openedDialogsCount -= 1
        console.debug("Dialog closed. Opened dialogs count is " + openedDialogsCount)
    }

    function mustUseConfirmation() {
        var mustUse = appSettings.boolValue(appSettings.useConfirmationDialogsKey, true)
        return mustUse
    }

    function openUploadDialog(skipUploadItems) {
        if (appSettings.boolValue(appSettings.mustUseMasterPasswordKey, skipUploadItems)) {
            var callbackObject = {
                onSuccess: function() { doOpenUploadDialog(true, skipUploadItems) },
                onFail: function() { doOpenUploadDialog(false, skipUploadItems) }
            }

            Common.launchDialog("Dialogs/EnterMasterPasswordDialog.qml",
                                applicationWindow,
                                {callbackObject: callbackObject})
        } else {
            doOpenUploadDialog(true, skipUploadItems)
        }
    }

    function doOpenUploadDialog(masterPasswordCorrectOrEmpty, skipUploadItems) {
        var artworkUploader = helpersWrapper.getArtworkUploader()
        artworkUploader.resetModel()

        if (!skipUploadItems) {
            filteredArtItemsModel.setSelectedForUpload()
            warningsModel.setShowSelected()
        }

        var uploadInfos = helpersWrapper.getUploadInfos();
        uploadInfos.initializeAccounts(masterPasswordCorrectOrEmpty)
        Common.launchDialog("Dialogs/UploadArtworks.qml",
                            applicationWindow,
                            {
                                componentParent: applicationWindow,
                                skipUploadItems: skipUploadItems
                            })
    }

    function openFindAndReplaceDialog() {
        Common.launchDialog("Dialogs/FindAndReplace.qml", applicationWindow, { componentParent: applicationWindow })
    }

    function openDeleteKeywordsDialog() {
        Common.launchDialog("Dialogs/DeleteKeywordsDialog.qml", applicationWindow, { componentParent: applicationWindow })
    }

    function clearFilter() {
        filteredArtItemsModel.searchTerm = ''
        if (filterText.length > 0) {
            filterText.text = ''
        }
        filterClearTimer.start()
    }

    Component.onCompleted: {
        console.debug("onCompleted handler")
        openingTimer.start()
    }

    Action {
        id: openSettingsAction
        text: i18.n + qsTr("&Settings")
        shortcut: StandardKey.Preferences
        onTriggered: {
            settingsModel.readAllValues()
            Common.launchDialog("Dialogs/SettingsWindow.qml",
                                applicationWindow, {},
                                function(wnd) {wnd.show();});
        }
    }

    Action {
        id: quitAction
        text: i18.n + qsTr("&Exit")
        shortcut: StandardKey.Quit
        onTriggered: closeHandler({accepted: false});
    }

    Action {
        id: editAction
        shortcut: "Ctrl+E"
        enabled: (artworkRepository.artworksSourcesCount > 0) && (applicationWindow.openedDialogsCount == 0)
        onTriggered: {
            if (filteredArtItemsModel.selectedArtworksCount === 0) {
                mustSelectDialog.open()
            }
            else {
                var launched = false

                if (filteredArtItemsModel.selectedArtworksCount == 1) {
                    var index = filteredArtItemsModel.findSelectedItemIndex()

                    if (index !== -1) {
                        Common.launchItemEditing(index, applicationWindow, {
                                                     applyCallback: function() {}
                                                 })
                        launched = true
                    }
                }

                if (!launched) {
                    // also as fallback in case of errors in findSelectedIndex
                    filteredArtItemsModel.combineSelectedArtworks();
                    Common.launchDialog("Dialogs/CombinedArtworksDialog.qml",
                                        applicationWindow, {componentParent: applicationWindow});
                }
            }
        }
    }

    Action {
        id: saveAction
        shortcut: StandardKey.Save
        enabled: (artworkRepository.artworksSourcesCount > 0) && (applicationWindow.openedDialogsCount == 0)
        onTriggered: {
            if (filteredArtItemsModel.selectedArtworksCount == 0) {
                mustSelectDialog.open()
            }
            else {
                var modifiedSelectedCount = filteredArtItemsModel.getModifiedSelectedCount();

                if (filteredArtItemsModel.selectedArtworksCount > 0 && modifiedSelectedCount > 0) {
                    Common.launchDialog("Dialogs/ExportMetadata.qml", applicationWindow, {})
                } else {
                    if (modifiedSelectedCount === 0) {
                        alreadySavedDialog.open()
                    }
                }
            }
        }
    }

    Action {
        id: searchAction
        shortcut: StandardKey.Find
        onTriggered: filterText.forceActiveFocus()
        enabled: (artworkRepository.artworksSourcesCount > 0) && (applicationWindow.openedDialogsCount == 0)
    }

    Action {
        id: searchAndReplaceAction
        shortcut: "Shift+Ctrl+F"
        onTriggered: openFindAndReplaceDialog()
        enabled: (artworkRepository.artworksSourcesCount > 0) && (applicationWindow.openedDialogsCount == 0)
    }

    Action {
        id: selectAllAction
        shortcut: StandardKey.SelectAll
        enabled: (artworkRepository.artworksSourcesCount > 0) && (applicationWindow.openedDialogsCount == 0)
        onTriggered: {
            if (selectAllCheckbox.checked) {
                filteredArtItemsModel.selectFilteredArtworks();
            }
            else {
                filteredArtItemsModel.unselectFilteredArtworks();
            }
        }
    }

    Action {
        id: removeAction
        shortcut: "Ctrl+Del"
        enabled: (artworkRepository.artworksSourcesCount > 0) && (applicationWindow.openedDialogsCount == 0)
        onTriggered: {
            if (filteredArtItemsModel.selectedArtworksCount === 0) {
                mustSelectDialog.open()
            }
            else {
                var itemsCount = filteredArtItemsModel.selectedArtworksCount
                if (itemsCount > 0) {
                    if (mustUseConfirmation()) {
                        confirmRemoveSelectedDialog.itemsCount = itemsCount
                        confirmRemoveSelectedDialog.open()
                    } else {
                        doRemoveSelectedArtworks()
                    }
                }
            }
        }
    }

    Action {
        id: uploadAction
        shortcut: "Shift+Ctrl+U"
        enabled: (artworkRepository.artworksSourcesCount > 0) && (applicationWindow.openedDialogsCount == 0)
        onTriggered: {
            if (filteredArtItemsModel.selectedArtworksCount === 0) {
                filteredArtItemsModel.selectFilteredArtworks();
            }

            if (filteredArtItemsModel.selectedArtworksCount > 0) {
                tryUploadArtworks();
            }
        }
    }

    Action {
        id: addFilesAction
        shortcut: StandardKey.Open
        onTriggered: chooseArtworksDialog.open()
        enabled: (applicationWindow.openedDialogsCount == 0)
    }

    menuBar: MenuBar {
        Menu {
            title: i18.n + qsTr("&File")

            Menu {
                id: recentDirectoriesMenu
                title: i18.n + qsTr("&Recent directories")
                enabled: applicationWindow.openedDialogsCount == 0

                Instantiator {
                    model: recentDirectories
                    onObjectAdded: recentDirectoriesMenu.insertItem( index, object )
                    onObjectRemoved: recentDirectoriesMenu.removeItem( object )

                    delegate: MenuItem {
                        text: display
                        onTriggered: {
                            var filesAdded = artItemsModel.addRecentDirectory(display)
                            if (filesAdded === 0) {
                                noNewFilesDialog.open()
                            }
                        }
                    }
                }
            }

            MenuItem { action: openSettingsAction }

            MenuItem {
                text: i18.n + qsTr("&About")
                onTriggered: {
                    Common.launchDialog("Dialogs/AboutWindow.qml",
                                        applicationWindow, {},
                                        function(wnd) {wnd.show();});
                }
            }

            MenuItem { action: quitAction }
        }

        Menu {
            title: i18.n + qsTr("&Edit")
            enabled: applicationWindow.openedDialogsCount == 0

            MenuItem {
                text: i18.n + qsTr("&Invert selection")
                enabled: artworksHost.count > 0
                onTriggered: {
                    console.info("Invert selection triggered")
                    filteredArtItemsModel.invertSelectionArtworks()
                }
            }

            MenuItem {
                text: i18.n + qsTr("&Sort by filename")
                enabled: artworksHost.count > 0
                checkable: true
                onToggled: {
                    console.info("Sort by filename")
                    filteredArtItemsModel.toggleSorted();
                }
            }

            MenuItem {
                text: i18.n + qsTr("&Find and replace")
                action: searchAndReplaceAction
            }

            MenuItem {
                text: i18.n + qsTr("&Delete keywords from selected")
                enabled: filteredArtItemsModel.selectedArtworksCount > 0
                onTriggered: {
                    console.info("Delete keywords from selected triggered")
                    filteredArtItemsModel.deleteKeywordsFromSelected()
                    openDeleteKeywordsDialog()
                }
            }

            MenuItem {
                text: i18.n + qsTr("&Remove metadata from selected")
                enabled: filteredArtItemsModel.selectedArtworksCount > 0
                onTriggered: {
                    console.info("Remove metadata from selected triggered")
                    removeMetadataDialog.open()
                }
            }

            MenuItem {
                text: i18.n + qsTr("&Detach vectors from selected")
                enabled: filteredArtItemsModel.selectedArtworksCount > 0
                onTriggered: {
                    console.info("Detach vectors from selected triggered")
                    filteredArtItemsModel.detachVectorFromSelected()
                }
            }

            MenuItem {
                text: i18.n + qsTr("&Cleanup local library in background")
                onTriggered: {
                    console.info("Cleanup local library triggered")
                    helpersWrapper.cleanupLocalLibrary()
                }
            }

            MenuItem {
                text: i18.n + qsTr("&Manage upload hosts")
                onTriggered: {
                    console.info("Manage upload hosts triggered")
                    openUploadDialog(true)
                }
            }
        }

        Menu {
            title: i18.n + qsTr("&Plugins")
            id: pluginsMenu
            enabled: helpersWrapper.pluginsAvailable
            visible: helpersWrapper.pluginsAvailable

            Instantiator {
                model: pluginsWithActions
                onObjectAdded: pluginsMenu.insertItem( index, object )
                onObjectRemoved: pluginsMenu.removeItem( object )

                delegate: Menu {
                    id: pluginActionsMenu
                    title: model.prettyname
                    enabled: model.enabled
                    property var actionsModel: pluginManager.getPluginActions(index)

                    Instantiator {
                        model: actionsModel
                        onObjectAdded: pluginActionsMenu.insertItem( index, object )
                        onObjectRemoved: pluginActionsMenu.removeItem( object )

                        delegate: MenuItem {
                            text: aname
                            onTriggered: {
                                pluginManager.triggerPluginAction(pluginID, acode)
                            }
                        }
                    }
                }
            }

            MenuSeparator {
                visible: pluginsMenu.items.length > 2
            }

            MenuItem {
                text: i18.n + qsTr("&Plugin manager")
                onTriggered: {
                    Common.launchDialog("Dialogs/PluginsDialog.qml",
                                        applicationWindow, {});
                }
            }
        }

        Menu {
            title: i18.n + qsTr("&Tools")
            enabled: (applicationWindow.openedDialogsCount == 0) || debug

            Menu {
                title: i18.n + qsTr("&Change language")
                id: languagesMenu

                Instantiator {
                    model: languagesModel
                    onObjectAdded: languagesMenu.insertItem( index, object )
                    onObjectRemoved: languagesMenu.removeItem( object )

                    delegate: MenuItem {
                        checkable: true
                        checked: iscurrent
                        text: display
                        onTriggered: {
                            languagesModel.switchLanguage(index)
                        }
                    }
                }
            }

            MenuItem {
                text: i18.n + qsTr("&Zip selected artworks")
                enabled: filteredArtItemsModel.selectedArtworksCount > 0
                onTriggered: {
                    console.info("Zip archives triggered")

                    filteredArtItemsModel.setSelectedForZipping()
                    Common.launchDialog("Dialogs/ZipArtworksDialog.qml",
                                        applicationWindow,
                                        {});
                }
            }

            MenuItem {
                text: i18.n + qsTr("&Import metadata from selected")
                enabled: filteredArtItemsModel.selectedArtworksCount > 0
                onTriggered: {
                    console.info("Reimport archives triggered")
                    filteredArtItemsModel.reimportMetadataForSelected()
                }
            }

            MenuItem {
                text: i18.n + qsTr("&Overwrite metadata in selected")
                enabled: filteredArtItemsModel.selectedArtworksCount > 0
                onTriggered: {
                    console.info("Overwrite metadata triggered")
                    Common.launchDialog("Dialogs/ExportMetadata.qml", applicationWindow, {overwriteAll: true})
                }
            }
        }

        Menu {
            title: i18.n + qsTr("&Help")

            MenuItem {
                text: i18.n + qsTr("&Video tutorials")
                onTriggered: Qt.openUrlExternally("https://ribtoks.github.io/xpiks/blog/2016/videos")
            }

            MenuItem {
                text: i18.n + qsTr("&FAQ")
                onTriggered: Qt.openUrlExternally("https://ribtoks.github.io/xpiks/faq/")
            }

            MenuItem {
                text: i18.n + qsTr("&Support")
                onTriggered: Qt.openUrlExternally("https://ribtoks.github.io/xpiks/support/")
            }
        }

        Menu {
            title: "Debug"
            visible: debug
            enabled: debug

            MenuItem {
                text: "Keywords Suggestion"
                onTriggered: {
                    var callbackObject = {
                        promoteKeywords: function(keywords) {
                            // bump
                        }
                    }

                    Common.launchDialog("Dialogs/KeywordsSuggestion.qml",
                                        applicationWindow,
                                        {callbackObject: callbackObject});
                }
            }

            MenuItem {
                text: "Update window"
                onTriggered: {
                    Common.launchDialog("Dialogs/UpdateWindow.qml",
                                        applicationWindow, {updateUrl: "https://ribtoks.github.io/xpiks/downloads/"},
                                        function(wnd) {wnd.show();});
                }
            }

            MenuItem {
                text: "What's new dialog"
                onTriggered: {
                    Common.launchDialog("Dialogs/WhatsNewDialog.qml", applicationWindow, {})
                }
            }

            MenuItem {
                text: "Legacy what's new dialog"
                onTriggered: {
                    var text = appSettings.whatsNewText;
                    if (text.length > 0) {
                        Common.launchDialog("Dialogs/WhatsNewMinorDialog.qml",
                                            applicationWindow,
                                            {
                                                whatsNewText: text
                                            })
                    }
                }
            }

            MenuItem {
                text: "Master password"
                onTriggered: {
                    var callbackObject = {
                        onSuccess: function() { },
                        onFail: function() { }
                    }

                    Common.launchDialog("Dialogs/EnterMasterPasswordDialog.qml",
                                        applicationWindow,
                                        {callbackObject: callbackObject})
                }
            }
        }
    }

    Menu {
        id: artworkContextMenu
        property string filename

        MenuItem {
            text: qsTr("Show in folder")
            onTriggered: helpersWrapper.revealArtworkFile(artworkContextMenu.filename);
        }
    }

    Menu {
        id: addWordContextMenu
        property string word

        MenuItem {
            text: qsTr("Add to dictionary")
            onTriggered: spellCheckService.addWordToUserDictionary(addWordContextMenu.word);
        }
    }

    MessageDialog {
        id: configExitDialog

        title: i18.n + qsTr("Confirmation")
        text: i18.n + qsTr("You have some artworks modified. Really exit?")
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: shutdownEverything()
    }

    MessageDialog {
        id: unavailableArtworksDialog
        title: i18.n + qsTr("Warning")
        text: qsTr("Some files are not available anymore.\nThey will be removed from the workflow.")
        standardButtons: StandardButton.Ok
        onAccepted: {
            helpersWrapper.removeUnavailableFiles()
        }
    }

    MessageDialog {
        id: unavailableVectorsDialog
        title: i18.n + qsTr("Warning")
        text: qsTr("Some vectors are not available anymore.\nThey will be detached automatically.")
        standardButtons: StandardButton.Ok
    }

    MessageDialog {
        id: removeMetadataDialog

        title: i18.n + qsTr("Confirmation")
        text: i18.n + qsTr("Remove metadata from selected artworks?")
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            filteredArtItemsModel.removeMetadataInSelected()
        }
    }

    MessageDialog {
        id: confirmRemoveSelectedDialog
        property int itemsCount
        title: i18.n + qsTr("Confirmation")
        text: i18.n + qsTr("Are you sure you want to remove %1 item(s)?").arg(itemsCount)
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            doRemoveSelectedArtworks()
        }
    }

    function doRemoveSelectedArtworks() {
        filteredArtItemsModel.removeSelectedArtworks()
    }

    function tryUploadArtworks() {
        if (filteredArtItemsModel.areSelectedArtworksSaved()) {
            openUploadDialog(false)
        } else {
            mustSaveWarning.open()
        }
    }

    MessageDialog {
        id: confirmRemoveDirectoryDialog
        property int directoryIndex
        title: i18.n + qsTr("Confirmation")
        text: i18.n + qsTr("Are you sure you want to remove this directory?")
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            filteredArtItemsModel.removeArtworksDirectory(directoryIndex)
        }
    }

    FileDialog {
        id: chooseArtworksDialog
        title: "Please choose artworks"
        selectExisting: true
        selectMultiple: true
        folder: shortcuts.pictures
        nameFilters: [ "Image files (*.jpg *.tiff *.eps *.ai)", "All files (*)" ]

        onAccepted: {
            console.debug("You chose: " + chooseArtworksDialog.fileUrls)
            var filesAdded = artItemsModel.addLocalArtworks(chooseArtworksDialog.fileUrls)
            if (filesAdded > 0) {
                saveRecentDirectories()
                console.debug("" + filesAdded + ' files via Open File(s)')
            } else {
                noNewFilesDialog.open()
            }
        }

        onRejected: {
            console.debug("Open files dialog canceled")
        }
    }

    FileDialog {
        id: chooseDirectoryDialog
        title: "Please choose artworks"
        selectExisting: true
        selectMultiple: false
        selectFolder: true
        folder: shortcuts.pictures

        onAccepted: {
            console.debug("You chose: " + chooseDirectoryDialog.fileUrls)
            var filesAdded = artItemsModel.addLocalDirectories(chooseDirectoryDialog.fileUrls)
            if (filesAdded > 0) {
                saveRecentDirectories()
                console.debug("" + filesAdded + ' files via Open Directory')
            } else {
                noNewFilesDialog.open()
            }
        }

        onRejected: {
            console.debug("Directory dialog canceled")
        }
    }

    MessageDialog {
        id: mustSaveWarning
        title: i18.n + qsTr("Warning")
        text: i18.n + qsTr("Please, save selected items before upload")
    }

    MessageDialog {
        id: mustSelectDialog
        title: i18.n + qsTr("Warning")
        text: i18.n + qsTr("Please, select some artworks first")
    }

    MessageDialog {
        id: noNewFilesDialog
        title: i18.n + qsTr("Information")
        text: i18.n + qsTr("No new files were added")
    }

    MessageDialog {
        id: alreadySavedDialog
        title: i18.n + qsTr("Information")
        text: i18.n + qsTr("All selected items are already saved")
    }

    MessageDialog {
        id: vectorsAttachedDialog
        title: i18.n + qsTr("Information")
        property int vectorsAttached: 0
        property string originalText: vectorsAttached > 1 ? qsTr("%1 vectors attached").arg(vectorsAttached) : qsTr("1 vector attached")
        text: i18.n + originalText
    }

    Connections {
        target: artItemsModel

        onUnavailableArtworksFound: {
            console.debug("UI:onUnavailableArtworksFound")
            unavailableArtworksDialog.open()
        }

        onUnavailableVectorsFound: {
            console.debug("UI:onUnavailableVectorsFound")
            unavailableVectorsDialog.open()
        }

        onArtworksAdded: {
            if ((imagesCount === 0) && (vectorsCount === 0)) {
                noNewFilesDialog.open();
                return;
            }

            var latestDir = recentDirectories.getLatestDirectory()
            chooseArtworksDialog.folder = latestDir
            chooseDirectoryDialog.folder = latestDir

            if (imagesCount > 0) {
                Common.launchDialog("Dialogs/ImportMetadata.qml", applicationWindow, {})
            } else if (vectorsCount > 0) {
                vectorsAttachedDialog.vectorsAttached = vectorsCount
                vectorsAttachedDialog.open()
            }
        }
    }

    Connections {
        target: helpersWrapper
        onUpdateAvailable: {
            Common.launchDialog("Dialogs/UpdateWindow.qml",
                                applicationWindow, {updateUrl: updateLink},
                                function(wnd) {wnd.show();});
            applicationWindow.showUpdateLink = true
        }
    }

    Rectangle {
        color: Colors.defaultDarkColor
        anchors.fill: parent

        DropArea {
            enabled: applicationWindow.openedDialogsCount == 0
            anchors.fill: parent
            onDropped: {
                if (drop.hasUrls) {
                    var filesCount = artItemsModel.dropFiles(drop.urls)
                    if (filesCount > 0) {
                        saveRecentDirectories()
                        console.debug(filesCount + ' files added via drag&drop')
                    }
                }
            }
        }

        ColumnLayout {
            id: repositoriesColumnLayout
            width: 250
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom

            spacing: 0

            Rectangle {
                width: parent.width
                height: 45
                color: Colors.defaultDarkColor

                StyledButton {
                    anchors.left: parent.left
                    anchors.leftMargin: 10
                    anchors.verticalCenter: parent.verticalCenter
                    id: addDirectoryButton
                    text: i18.n + qsTr("Add directory")
                    width: 110
                    onClicked: chooseDirectoryDialog.open()
                    enabled: (applicationWindow.openedDialogsCount == 0)
                }

                StyledButton {
                    anchors.leftMargin: 10
                    anchors.left: addDirectoryButton.right
                    anchors.verticalCenter: parent.verticalCenter
                    text: i18.n + qsTr("Add files", "button")
                    width: 110
                    action: addFilesAction
                }
            }

            Rectangle {
                height: 1
                width: parent.width
                color: Colors.labelInactiveForeground
            }

            Rectangle {
                Layout.fillHeight: true
                width: 250

                color: Colors.defaultControlColor

                StyledScrollView {
                    anchors.fill: parent
                    anchors.topMargin: 5

                    ListView {
                        id: sourcesListView
                        model: artworkRepository
                        boundsBehavior: Flickable.StopAtBounds
                        anchors.fill: parent
                        anchors.rightMargin: 10
                        anchors.leftMargin: 10

                        spacing: 10

                        displaced: Transition {
                            NumberAnimation { properties: "x,y"; duration: 230 }
                        }

                        addDisplaced: Transition {
                            NumberAnimation { properties: "x,y"; duration: 230 }
                        }

                        removeDisplaced: Transition {
                            NumberAnimation { properties: "x,y"; duration: 230 }
                        }

                        delegate: Rectangle {
                            id: sourceWrapper
                            property int delegateIndex: index
                            color: isselected ? Colors.itemsSourceSelected : Colors.itemsSourceBackground
                            width: parent.width
                            height: 31
                            Layout.minimumWidth: 237

                            /*MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    filteredArtItemsModel.selectDirectory(sourceWrapper.delegateIndex)
                                }
                            }*/

                            RowLayout {
                                spacing: 10
                                anchors.fill: parent

                                Item {
                                    id: placeholder1
                                    width: 1
                                }

                                StyledText {
                                    id: directoryPath
                                    Layout.fillWidth: true
                                    anchors.verticalCenter: parent.verticalCenter
                                    height: 31
                                    color: Colors.inputForegroundColor
                                    text: path + " (" + usedimagescount + ")"
                                    elide: Text.ElideMiddle
                                }

                                CloseIcon {
                                    width: 14
                                    height: 14
                                    anchors.verticalCenter: parent.verticalCenter
                                    isActive: false
                                    crossOpacity: 1

                                    onItemClicked: {
                                        if (mustUseConfirmation()) {
                                            confirmRemoveDirectoryDialog.directoryIndex = sourceWrapper.delegateIndex
                                            confirmRemoveDirectoryDialog.open()
                                        } else {
                                            filteredArtItemsModel.removeArtworksDirectory(sourceWrapper.delegateIndex)
                                        }
                                    }
                                }

                                Item {
                                    id: placeholder2
                                    width: 1
                                }
                            }
                        }
                    }
                }
            }
        }

        ColumnLayout {
            anchors.left: repositoriesColumnLayout.right
            anchors.leftMargin: 2
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            spacing: 0

            Rectangle {
                height: 45
                color: Colors.defaultDarkColor
                z: 2000
                anchors.left: parent.left
                anchors.right: parent.right

                RowLayout {
                    spacing: 10
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.leftMargin: 0
                    anchors.rightMargin: mainScrollView.areScrollbarsVisible ? 35 : 20

                    Item {
                        width: 5
                    }

                    LayoutButton {
                        enabled: artworkRepository.artworksSourcesCount > 0
                        anchors.verticalCenter: parent.verticalCenter
                        isListLayout: applicationWindow.listLayout
                        onLayoutChanged: {
                            applicationWindow.listLayout = !applicationWindow.listLayout
                            closeAutoComplete()
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    StyledButton {
                        text: i18.n + qsTr("Remove")
                        width: 90
                        action: removeAction
                    }

                    StyledButton {
                        text: i18.n + qsTr("Edit")
                        width: 90
                        action: editAction
                    }

                    StyledButton {
                        text: i18.n + qsTr("Save")
                        width: mainScrollView.areScrollbarsVisible ? 88 : 98
                        action: saveAction
                    }

                    StyledButton {
                        text: i18.n + qsTr("Upload")
                        width: 100
                        action: uploadAction
                    }
                }
            }

            Rectangle {
                height: 1
                anchors.left: parent.left
                anchors.right: parent.right
                color: Colors.labelInactiveForeground
            }

            Rectangle {
                anchors.left: parent.left
                anchors.right: parent.right
                Layout.fillHeight: true
                color: Colors.defaultControlColor

                Item {
                    id: workflowHost
                    anchors.topMargin: 10
                    anchors.fill: parent
                    property var autoCompleteBox

                    function onAutoCompleteClose() {
                        autoCompleteBox = undefined
                    }

                    Item {
                        id: filterRect
                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.right: parent.right
                        height: 24

                        StyledCheckbox {
                            id: selectAllCheckbox
                            anchors.left: parent.left
                            anchors.leftMargin: 20
                            anchors.verticalCenter: parent.verticalCenter
                            isContrast: true
                            enabled: artworkRepository.artworksSourcesCount > 0
                            text: i18.n + getOriginalText()
                            checked: filteredArtItemsModel.selectedArtworksCount > 0

                            function getOriginalText() {
                                return filteredArtItemsModel.selectedArtworksCount === 0 ? qsTr("Select all") : qsTr("Select none")
                            }

                            onClicked: {
                                if (checked) {
                                    filteredArtItemsModel.selectFilteredArtworks();
                                }
                                else {
                                    filteredArtItemsModel.unselectFilteredArtworks();
                                }
                            }

                            Connections {
                                target: filteredArtItemsModel
                                onSelectedArtworksCountChanged: {
                                    selectAllCheckbox.checked = filteredArtItemsModel.selectedArtworksCount > 0
                                }
                            }
                        }

                        Rectangle {
                            anchors.right: searchButton.left
                            anchors.rightMargin: 10
                            anchors.verticalCenter: parent.verticalCenter
                            color: Colors.defaultDarkColor
                            width: (artworkRepository.artworksSourcesCount > 0 && mainScrollView.areScrollbarsVisible) ? 288 : 298
                            height: UIConfig.textInputHeight

                            StyledTextInput {
                                id: filterText
                                width: (artworkRepository.artworksSourcesCount > 0 && mainScrollView.areScrollbarsVisible) ? 250 : 260
                                height: UIConfig.textInputHeight
                                clip: true
                                anchors.left: parent.left
                                anchors.leftMargin: 5
                                enabled: artworkRepository.artworksSourcesCount > 0
                                selectionColor: Colors.inputInactiveForeground
                                selectedTextColor: Colors.whiteColor

                                onAccepted: {
                                    filteredArtItemsModel.searchTerm = text
                                }

                                Connections {
                                    target: filteredArtItemsModel
                                    onSearchTermChanged: filterText.text = filteredArtItemsModel.searchTerm
                                }
                            }

                            CloseIcon {
                                width: 14
                                height: 14
                                anchors.right: parent.right
                                anchors.rightMargin: 5
                                enabled: filterText.length > 0
                                anchors.verticalCenter: parent.verticalCenter
                                onItemClicked: clearFilter()
                            }

                            StyledText {
                                text: i18.n + qsTr("Search...   x:empty  x:modified")
                                visible: !(filterClearTimer.running || filterText.activeFocus || (filterText.length > 0))
                                color: Colors.selectedArtworkBackground
                                anchors.left: parent.left
                                anchors.leftMargin: 7
                                anchors.verticalCenter: parent.verticalCenter
                            }

                            Timer {
                                id: filterClearTimer
                                running: false
                                interval: 750
                                repeat: false
                            }
                        }

                        StyledBlackButton {
                            id: searchButton
                            anchors.right: parent.right
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.rightMargin:  mainScrollView.areScrollbarsVisible ? 35 : 20
                            width: 100
                            text: i18.n + qsTr("Search")
                            enabled: artworkRepository.artworksSourcesCount > 0
                            onClicked: filteredArtItemsModel.searchTerm = filterText.text
                        }
                    }

                    Rectangle {
                        id: undoRedoRect
                        color: Colors.defaultDarkColor
                        width: parent.width
                        height: 4
                        opacity: 0
                        anchors.topMargin: 0
                        anchors.top: filterRect.bottom

                        states: [
                            State {
                                name: "canundo"
                                when: undoRedoManager.canUndo
                                PropertyChanges {
                                    target: undoRedoRect
                                    height: 40
                                }
                            }
                        ]

                        transitions: [
                            Transition {
                                from: ""; to: "canundo"
                                ParallelAnimation {
                                    NumberAnimation {
                                        target: undoRedoRect
                                        properties: "height";
                                        from: 4
                                        to: 40
                                        easing.type: "InOutQuad";
                                        duration: 250
                                    }
                                    NumberAnimation {
                                        target: undoRedoRect
                                        properties: "anchors.topMargin";
                                        from: 0
                                        to: 4
                                        easing.type: "InOutQuad";
                                        duration: 250
                                    }
                                    NumberAnimation {
                                        target: undoRedoRect
                                        properties: "opacity";
                                        from: 0
                                        to: 1
                                        easing.type: "InOutQuad";
                                        duration: 250
                                    }
                                }
                            },
                            Transition {
                                from: "canundo"; to: ""
                                ParallelAnimation {
                                    NumberAnimation {
                                        target: undoRedoRect
                                        properties: "height";
                                        from: 40
                                        to: 4
                                        easing.type: "InOutQuad";
                                        duration: 250
                                    }
                                    NumberAnimation {
                                        target: undoRedoRect
                                        properties: "anchors.topMargin";
                                        from: 4
                                        to: 0
                                        easing.type: "InOutQuad";
                                        duration: 250
                                    }
                                    NumberAnimation {
                                        target: undoRedoRect
                                        properties: "opacity";
                                        from: 1
                                        to: 0
                                        easing.type: "InOutQuad";
                                        duration: 250
                                    }
                                }
                            }
                        ]

                        Rectangle {
                            anchors.left: parent.left
                            color: Colors.artworkSavedColor
                            width: applicationWindow.listLayout ? 6 : 3
                            height: parent.height
                        }

                        RowLayout {
                            anchors.centerIn: parent

                            StyledText {
                                id: undoDescription
                                text: undoRedoManager.undoDescription
                            }

                            Item {
                                width: 10
                            }

                            StyledText {
                                text: i18.n + qsTr("Undo")
                                color: undoMA.pressed ? Colors.linkClickedColor : Colors.artworkActiveColor

                                MouseArea {
                                    id: undoMA
                                    anchors.fill: parent
                                    enabled: undoRedoManager.canUndo
                                    cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
                                    onClicked: {
                                        undoRedoManager.undoLastAction()
                                        filteredArtItemsModel.updateFilter()
                                    }
                                }
                            }

                            StyledText {
                                text: i18.n + qsTr("Dismiss (%1)").arg(settingsModel.dismissDuration - (autoDismissTimer.iterations % (settingsModel.dismissDuration + 1)))
                                color: dismissUndoMA.pressed ? Colors.linkClickedColor : Colors.labelActiveForeground

                                MouseArea {
                                    id: dismissUndoMA
                                    anchors.fill: parent
                                    enabled: undoRedoManager.canUndo
                                    cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
                                    onClicked: {
                                        undoRedoManager.discardLastAction()
                                    }
                                }
                            }

                        }

                        Timer {
                            id: autoDismissTimer
                            property int iterations: 0
                            interval: 1000
                            repeat: true
                            running: undoRedoManager.canUndo
                            onTriggered: {
                                iterations += 1

                                if (iterations % (settingsModel.dismissDuration + 1) === settingsModel.dismissDuration) {
                                    undoRedoManager.discardLastAction()
                                    iterations = 0
                                }
                            }
                        }

                        Connections {
                            target: undoRedoManager
                            onItemRecorded: {
                                autoDismissTimer.iterations = 0
                            }
                        }
                    }

                    Rectangle {
                        id: separator
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.rightMargin: artworksHost.count > 3 ? 15 : 0
                        anchors.topMargin: visible ? 2 : 4
                        anchors.top: undoRedoRect.bottom
                        height: visible ? 2 : 0
                        color: Colors.defaultDarkColor
                        visible: !undoRedoManager.canUndo && (artworksHost.count > 0)
                    }

                    Item {
                        id: mainScrollView
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.top: separator.bottom
                        anchors.bottom: parent.bottom
                        clip: true
                        property bool areScrollbarsVisible: artworksHost.contentHeight > artworksHost.height
                        //__wheelAreaScrollSpeed: 50 + 10*settingsModel.scrollSpeedScale
                        // does not work for now in Qt 5.4.1 in combination with ListView
                        //verticalScrollBarPolicy: Qt.ScrollBarAlwaysOn

                        GridView {
                            id: artworksHost
                            anchors.fill: parent
                            anchors.rightMargin: mainScrollView.areScrollbarsVisible ? 10 : 0
                            model: filteredArtItemsModel
                            boundsBehavior: Flickable.StopAtBounds
                            property int cellSpacing: 4
                            property double defaultRowHeight: 205
                            cellHeight: applicationWindow.listLayout ? (defaultRowHeight + 80*(settingsModel.keywordSizeScale - 1.0) + cellSpacing) : (defaultRowHeight + cellSpacing)
                            cellWidth: applicationWindow.listLayout ? artworksHost.width : (208 + cellSpacing)
                            highlightFollowsCurrentItem: false
                            currentIndex: -1
                            focus: true
                            flickDeceleration: 1500/settingsModel.scrollSpeedScale

                            function forceUpdateArtworks(needToMoveCurrentItem) {
                                console.debug("UI::forceUpdateArtworks # updating main listview")
                                artworksHost.returnToBounds()
                                artworksHost.forceLayout()
                                artworksHost.update()

                                // this piece of code is here in order to beat caching of ListView
                                // when you remove all items but few it fails to draw them
                                if (needToMoveCurrentItem) {
                                    console.debug("UI::forceUpdateArtworks # Moving into current item " + artworksHost.currentIndex)
                                    artworksHost.moveCurrentIndexDown()
                                    artworksHost.positionViewAtIndex(artworksHost.currentIndex, GridView.Visible)
                                }
                            }

                            add: Transition {
                                NumberAnimation { property: "opacity"; from: 0; to: 1; duration: 230 }
                            }

                            remove: Transition {
                                NumberAnimation { property: "opacity"; to: 0; duration: 230 }
                            }

                            displaced: Transition {
                                NumberAnimation { properties: "x,y"; duration: 230 }
                            }

                            addDisplaced: Transition {
                                NumberAnimation { properties: "x,y"; duration: 230 }
                            }

                            removeDisplaced: Transition {
                                NumberAnimation { properties: "x,y"; duration: 230 }
                            }

                            onContentYChanged: closeAutoComplete()

                            delegate: FocusScope {
                                id: wrappersScope
                                width: applicationWindow.listLayout ? (mainScrollView.areScrollbarsVisible ? (parent.width - 5) : parent.width) : 208
                                height: applicationWindow.listLayout ? (artworksHost.defaultRowHeight + 80*(settingsModel.keywordSizeScale - 1.0)) : artworksHost.defaultRowHeight

                                function updateCurrentIndex() {
                                    GridView.view.currentIndex = rowWrapper.delegateIndex
                                }

                                Rectangle {
                                    id: rowWrapper
                                    objectName: "artworkDelegate"
                                    property bool isHighlighted: isselected || wrappersScope.GridView.isCurrentItem
                                    color: isHighlighted ? Colors.selectedImageBackground : Colors.artworkImageBackground
                                    property var artworkModel: filteredArtItemsModel.getArtworkMetadata(index)
                                    property var keywordsModel: filteredArtItemsModel.getKeywordsModel(index)
                                    property int delegateIndex: index
                                    property bool isItemSelected: isselected
                                    anchors.fill: parent

                                    function getIndex() {
                                        return filteredArtItemsModel.getOriginalIndex(index)
                                    }

                                    function focusDescription() {
                                        descriptionTextInput.forceActiveFocus()
                                        descriptionTextInput.cursorPosition = descriptionTextInput.text.length
                                    }

                                    function isAnyFocused() {
                                        return descriptionTextInput.activeFocus || flv.isFocused || titleTextInput.activeFocus;
                                    }

                                    function focusIfNeeded() {
                                        if (!isAnyFocused()) {
                                            focusDescription()
                                        }

                                        if (!isselected) {
                                            flv.removeFocus()
                                        }
                                    }

                                    function switchChecked() {
                                        editisselected = !isselected
                                        itemCheckedCheckbox.checked = isselected
                                        wrappersScope.updateCurrentIndex()
                                    }

                                    Connections {
                                        target: rowWrapper.artworkModel

                                        onFocusRequested: {
                                            if (directionSign === +1) {
                                                descriptionTextInput.forceActiveFocus()
                                                descriptionTextInput.cursorPosition = descriptionTextInput.text.length
                                            } else {
                                                flv.activateEdit()
                                            }

                                            artworksHost.positionViewAtIndex(rowWrapper.delegateIndex, GridView.Contain)
                                        }
                                    }

                                    Connections {
                                        target: rowWrapper.keywordsModel

                                        onSpellCheckResultsReady: {
                                            descriptionTextInput.deselect()
                                            if (columnLayout.isWideEnough) {
                                                titleTextInput.deselect()
                                            }
                                        }

                                        onCompletionsAvailable: {
                                            if (typeof workflowHost.autoCompleteBox !== "undefined") {
                                                // update completion
                                                return
                                            }

                                            var directParent = mainScrollView;
                                            var currWordStartRect = flv.editControl.getCurrentWordStartRect()

                                            var tmp = flv.editControl.mapToItem(directParent,
                                                                                currWordStartRect.x - 17,
                                                                                flv.editControl.height + 1)

                                            var visibleItemsCount = Math.min(acSource.getCount(), 5);
                                            var popupHeight = visibleItemsCount * (25 + 1) + 10

                                            var isBelow = (tmp.y + popupHeight) < directParent.height;

                                            var options = {
                                                model: acSource,
                                                isBelowEdit: isBelow,
                                                "anchors.left": directParent.left,
                                                "anchors.leftMargin": Math.min(tmp.x, directParent.width - 200),
                                                "anchors.top": directParent.top
                                            }

                                            if (isBelow) {
                                                options["anchors.topMargin"] = tmp.y
                                            } else {
                                                options["anchors.topMargin"] = tmp.y - popupHeight - flv.editControl.height - 2
                                            }

                                            var component = Qt.createComponent("Components/CompletionBox.qml");
                                            if (component.status !== Component.Ready) {
                                                console.debug("Component Error: " + component.errorString());
                                            } else {
                                                var instance = component.createObject(directParent, options);

                                                instance.boxDestruction.connect(workflowHost.onAutoCompleteClose)
                                                instance.itemSelected.connect(flv.editControl.acceptCompletion)
                                                workflowHost.autoCompleteBox = instance

                                                instance.openPopup()
                                            }
                                        }
                                    }

                                    Item {
                                        anchors.fill: parent

                                        Rectangle {
                                            id: isModifiedRectangle
                                            color: ismodified ? Colors.artworkModifiedColor : Colors.artworkSavedColor
                                            width: applicationWindow.listLayout ? 6 : 3
                                            anchors.left: parent.left
                                            anchors.top: parent.top
                                            anchors.bottom: parent.bottom
                                        }

                                        Item {
                                            id: checkboxSpacer
                                            width: applicationWindow.listLayout ? 15 : 8
                                            anchors.left: isModifiedRectangle.right
                                            anchors.top: parent.top
                                            anchors.bottom: parent.bottom

                                            MouseArea {
                                                anchors.fill: parent
                                                enabled: applicationWindow.listLayout
                                                onClicked: {
                                                    rowWrapper.switchChecked()
                                                    rowWrapper.focusIfNeeded()
                                                }
                                            }
                                        }

                                        Item {
                                            id: checkboxRectangle
                                            anchors.left: checkboxSpacer.right
                                            anchors.top: parent.top
                                            anchors.bottom: parent.bottom
                                            width: 20

                                            MouseArea {
                                                anchors.fill: parent
                                                onClicked: {
                                                    rowWrapper.switchChecked()
                                                    rowWrapper.focusIfNeeded()
                                                }
                                            }

                                            StyledCheckbox {
                                                id: itemCheckedCheckbox
                                                //checked: isselected
                                                focus: false
                                                anchors.left: parent.left
                                                anchors.verticalCenter: parent.verticalCenter
                                                activeFocusOnPress: false
                                                onClicked: editisselected = checked
                                                Component.onCompleted: itemCheckedCheckbox.checked = isselected

                                                Connections {
                                                    target: filteredArtItemsModel
                                                    onAllItemsSelectedChanged: {
                                                        itemCheckedCheckbox.checked = rowWrapper.isItemSelected
                                                    }
                                                }
                                            }
                                        }

                                        Item {
                                            id: imageColumnWrapper
                                            width: applicationWindow.listLayout ? 180 : 160
                                            anchors.left: checkboxRectangle.right
                                            anchors.top: parent.top
                                            anchors.bottom: parent.bottom

                                            MouseArea {
                                                anchors.fill: parent
                                                onClicked: {
                                                    rowWrapper.switchChecked()
                                                    rowWrapper.focusIfNeeded()
                                                }
                                            }

                                            Item {
                                                anchors.fill: parent
                                                anchors.leftMargin: applicationWindow.listLayout ? 10 : 0
                                                anchors.rightMargin: applicationWindow.listLayout ? 15 : 0

                                                Item {
                                                    id: imageHost
                                                    anchors.top: parent.top
                                                    anchors.topMargin: descriptionText.height + 24
                                                    anchors.horizontalCenter: parent.horizontalCenter
                                                    width: 150
                                                    property double desiredHeight: descriptionRect.height + keywordsWrapper.height + keywordsLabel.height + 10
                                                    height: desiredHeight > 150 ? 150 : desiredHeight

                                                    Image {
                                                        id: artworkImage
                                                        anchors.fill: parent
                                                        source: "image://cached/" + filename
                                                        sourceSize.width: 150
                                                        sourceSize.height: 150
                                                        fillMode: settingsModel.fitSmallPreview ? Image.PreserveAspectFit : Image.PreserveAspectCrop
                                                        asynchronous: true
                                                        // caching is implemented on different level
                                                        cache: false
                                                    }

                                                    Image {
                                                        id: imageTypeIcon
                                                        visible: hasvectorattached
                                                        enabled: hasvectorattached
                                                        source: "qrc:/Graphics/vector-icon.svg"
                                                        sourceSize.width: 20
                                                        sourceSize.height: 20
                                                        anchors.left: artworkImage.left
                                                        anchors.bottom: artworkImage.bottom
                                                        cache: true
                                                    }

                                                    MouseArea {
                                                        anchors.fill: parent
                                                        propagateComposedEvents: true
                                                        acceptedButtons: Qt.LeftButton | Qt.RightButton

                                                        function dblClickHandler() {
                                                            Common.launchItemEditing(rowWrapper.getIndex(), applicationWindow, {
                                                                                         applyCallback: function() {}
                                                                                     })
                                                        }

                                                        Timer {
                                                            id: dblClickTimer
                                                            interval: 200
                                                            onTriggered: {
                                                                rowWrapper.switchChecked()
                                                                rowWrapper.focusIfNeeded()
                                                            }
                                                        }

                                                        onClicked: {
                                                            if (mouse.button == Qt.RightButton && openedDialogsCount == 0) {
                                                                console.log("Context menu for artwork")
                                                                artworkContextMenu.filename = filename;
                                                                artworkContextMenu.popup()
                                                            } else {
                                                                if (dblClickTimer.running) {
                                                                    dblClickTimer.stop()
                                                                    dblClickHandler()
                                                                } else {
                                                                    dblClickTimer.restart()
                                                                }
                                                            }
                                                        }
                                                    }
                                                }

                                                StyledText {
                                                    anchors.top: imageHost.bottom
                                                    anchors.topMargin: 3
                                                    width: parent.width
                                                    elide: Text.ElideMiddle
                                                    color: moreInfoMA.pressed ? Colors.linkClickedColor : Colors.labelActiveForeground
                                                    horizontalAlignment: Text.AlignHCenter
                                                    text: basefilename

                                                    MouseArea {
                                                        id: moreInfoMA
                                                        anchors.fill: parent
                                                        cursorShape: Qt.PointingHandCursor

                                                        onClicked: {
                                                            Common.launchDialog("Dialogs/ArtworkPreview.qml", applicationWindow,
                                                                                {
                                                                                    imagePath: filename,
                                                                                    artworkIndex: rowWrapper.getIndex()
                                                                                });
                                                        }
                                                    }
                                                }
                                            }
                                        }

                                        Rectangle {
                                            id: columnRectangle
                                            visible: applicationWindow.listLayout
                                            anchors.top: applicationWindow.listLayout ? parent.top : undefined
                                            anchors.bottom: applicationWindow.listLayout ? parent.bottom : undefined
                                            anchors.left: applicationWindow.listLayout ? imageColumnWrapper.right : undefined
                                            anchors.leftMargin: applicationWindow.listLayout ? 5 : 0
                                            anchors.right: applicationWindow.listLayout ? parent.right : undefined
                                            color: rowWrapper.isHighlighted ? Colors.selectedArtworkBackground : Colors.artworkBackground

                                            Item {
                                                id: columnLayout
                                                anchors.fill: parent
                                                anchors.leftMargin: 20
                                                anchors.rightMargin: 20
                                                anchors.topMargin: 20
                                                property bool isWideEnough: width > 450

                                                StyledText {
                                                    id: descriptionText
                                                    anchors.left: parent.left
                                                    anchors.top: parent.top
                                                    text: i18.n + qsTr("Description:")
                                                    isActive: rowWrapper.isHighlighted
                                                }

                                                StyledText {
                                                    id: titleText
                                                    anchors.left: titleRect.left
                                                    anchors.top: parent.top
                                                    visible: columnLayout.isWideEnough
                                                    text: i18.n + qsTr("Title:")
                                                    isActive: rowWrapper.isHighlighted
                                                }

                                                Rectangle {
                                                    id: descriptionRect
                                                    height: 30
                                                    anchors.left: parent.left
                                                    anchors.right: titleRect.left
                                                    anchors.rightMargin: columnLayout.isWideEnough ? 20 : 0
                                                    anchors.top: descriptionText.bottom
                                                    anchors.topMargin: 3
                                                    color: rowWrapper.isHighlighted ? Colors.inputBackgroundColor : Colors.inputInactiveBackground
                                                    border.color: Colors.artworkActiveColor
                                                    border.width: descriptionTextInput.activeFocus ? 1 : 0
                                                    clip: true
                                                    focus: false

                                                    Flickable {
                                                        id: descriptionFlick
                                                        contentWidth: descriptionTextInput.paintedWidth
                                                        contentHeight: descriptionTextInput.paintedHeight
                                                        anchors.left: parent.left
                                                        anchors.right: parent.right
                                                        anchors.leftMargin: 5
                                                        anchors.rightMargin: 5
                                                        interactive: false
                                                        flickableDirection: Flickable.HorizontalFlick
                                                        height: parent.height
                                                        anchors.verticalCenter: parent.verticalCenter
                                                        clip: true
                                                        focus: false

                                                        function ensureVisible(r) {
                                                            if (contentX >= r.x)
                                                                contentX = r.x;
                                                            else if (contentX+width <= r.x+r.width)
                                                                contentX = r.x+r.width-width;
                                                        }

                                                        StyledTextEdit {
                                                            id: descriptionTextInput
                                                            width: paintedWidth > descriptionFlick.width ? paintedWidth : descriptionFlick.width
                                                            height: descriptionFlick.height
                                                            text: description
                                                            focus: true
                                                            isActive: rowWrapper.isHighlighted
                                                            onTextChanged: model.editdescription = text

                                                            onActionRightClicked: {
                                                                if (filteredArtItemsModel.hasDescriptionWordSpellError(rowWrapper.delegateIndex, rightClickedWord)){
                                                                    console.log("Context menu for add word " + rightClickedWord)
                                                                    addWordContextMenu.word = rightClickedWord
                                                                    addWordContextMenu.popup()
                                                                }

                                                            }


                                                            Keys.onTabPressed: {
                                                                if (columnLayout.isWideEnough) {
                                                                    titleTextInput.forceActiveFocus()
                                                                    titleTextInput.cursorPosition = titleTextInput.text.length
                                                                } else {
                                                                    flv.activateEdit()
                                                                }
                                                            }

                                                            onActiveFocusChanged: {
                                                                filteredArtItemsModel.spellCheckDescription(rowWrapper.delegateIndex)
                                                                if (activeFocus) {
                                                                    wrappersScope.updateCurrentIndex()
                                                                }
                                                            }

                                                            Keys.onBacktabPressed: {
                                                                filteredArtItemsModel.focusPreviousItem(rowWrapper.delegateIndex)
                                                            }

                                                            Keys.onPressed: {
                                                                if(event.matches(StandardKey.Paste)) {
                                                                    var clipboardText = clipboard.getText();
                                                                    if (Common.safeInsert(descriptionTextInput, clipboardText)) {
                                                                        event.accepted = true
                                                                    }
                                                                } else if ((event.key === Qt.Key_Return) || (event.key === Qt.Key_Enter)) {
                                                                    event.accepted = true
                                                                }
                                                            }

                                                            Component.onCompleted: {
                                                                var index = rowWrapper.getIndex()
                                                                artItemsModel.initDescriptionHighlighting(index, descriptionTextInput.textDocument)
                                                            }

                                                            onCursorRectangleChanged: descriptionFlick.ensureVisible(cursorRectangle)
                                                        }
                                                    }
                                                }

                                                StyledText {
                                                    text: descriptionTextInput.length
                                                    anchors.right: descriptionRect.right
                                                    anchors.bottom: descriptionRect.top
                                                    anchors.bottomMargin: 3
                                                    isActive: rowWrapper.isHighlighted
                                                }

                                                Rectangle {
                                                    id: titleRect
                                                    height: 30
                                                    visible: columnLayout.isWideEnough
                                                    enabled: columnLayout.isWideEnough
                                                    width: columnLayout.isWideEnough ? ((columnLayout.width / 2) - 10) : 0
                                                    anchors.right: parent.right
                                                    anchors.top: descriptionText.bottom
                                                    anchors.topMargin: 3
                                                    color: rowWrapper.isHighlighted ? Colors.inputBackgroundColor : Colors.inputInactiveBackground
                                                    border.color: Colors.artworkActiveColor
                                                    border.width: titleTextInput.activeFocus ? 1 : 0
                                                    clip: true
                                                    focus: false

                                                    Flickable {
                                                        id: titleFlick
                                                        contentWidth: titleTextInput.paintedWidth
                                                        contentHeight: titleTextInput.paintedHeight
                                                        height: parent.height
                                                        anchors.verticalCenter: parent.verticalCenter
                                                        anchors.left: parent.left
                                                        anchors.right: parent.right
                                                        anchors.leftMargin: 5
                                                        anchors.rightMargin: 5
                                                        clip: true
                                                        flickableDirection: Flickable.HorizontalFlick
                                                        interactive: false
                                                        focus: false

                                                        function ensureVisible(r) {
                                                            if (contentX >= r.x)
                                                                contentX = r.x;
                                                            else if (contentX+width <= r.x+r.width)
                                                                contentX = r.x+r.width-width;
                                                        }

                                                        StyledTextEdit {
                                                            id: titleTextInput
                                                            width: paintedWidth > titleFlick.width ? paintedWidth : titleFlick.width
                                                            height: titleFlick.height
                                                            text: title
                                                            focus: true
                                                            isActive: rowWrapper.isHighlighted
                                                            onTextChanged: model.edittitle = text


                                                            onActionRightClicked: {
                                                                if (filteredArtItemsModel.hasTitleWordSpellError(rowWrapper.delegateIndex, rightClickedWord)){
                                                                    console.log("Context menu for add word " + rightClickedWord)
                                                                    addWordContextMenu.word = rightClickedWord
                                                                    addWordContextMenu.popup()
                                                                }
                                                            }

                                                            Keys.onTabPressed: {
                                                                flv.activateEdit()
                                                            }

                                                            Keys.onBacktabPressed: {
                                                                descriptionTextInput.forceActiveFocus()
                                                                descriptionTextInput.cursorPosition = descriptionTextInput.text.length
                                                            }

                                                            onActiveFocusChanged: {
                                                                filteredArtItemsModel.spellCheckTitle(rowWrapper.delegateIndex)
                                                                if (activeFocus) {
                                                                    wrappersScope.updateCurrentIndex()
                                                                }
                                                            }

                                                            Keys.onPressed: {
                                                                if (event.matches(StandardKey.Paste)) {
                                                                    var clipboardText = clipboard.getText();
                                                                    if (Common.safeInsert(titleTextInput, clipboardText)) {
                                                                        event.accepted = true
                                                                    }
                                                                } else if ((event.key === Qt.Key_Return) || (event.key === Qt.Key_Enter)) {
                                                                    event.accepted = true
                                                                }
                                                            }

                                                            Component.onCompleted: {
                                                                var index = rowWrapper.getIndex()
                                                                artItemsModel.initTitleHighlighting(index, titleTextInput.textDocument)
                                                            }

                                                            onCursorRectangleChanged: titleFlick.ensureVisible(cursorRectangle)
                                                        }
                                                    }
                                                }

                                                StyledText {
                                                    text: titleTextInput.length
                                                    anchors.right: titleRect.right
                                                    anchors.bottom: titleRect.top
                                                    anchors.bottomMargin: 3
                                                    isActive: rowWrapper.isHighlighted
                                                    visible: columnLayout.isWideEnough
                                                }

                                                StyledText {
                                                    id: keywordsLabel
                                                    anchors.left: parent.left
                                                    anchors.top: descriptionRect.bottom
                                                    anchors.topMargin: 7
                                                    text: i18.n + qsTr("Keywords:")
                                                    isActive: rowWrapper.isHighlighted
                                                }

                                                StyledText {
                                                    text: keywordscount
                                                    anchors.right: parent.right
                                                    anchors.top: descriptionRect.bottom
                                                    anchors.topMargin: 7
                                                    isActive: rowWrapper.isHighlighted
                                                }

                                                Rectangle {
                                                    id: keywordsWrapper
                                                    anchors.top: keywordsLabel.bottom
                                                    anchors.topMargin: 3
                                                    anchors.left: parent.left
                                                    anchors.right: parent.right
                                                    height: 80*settingsModel.keywordSizeScale
                                                    border.color: Colors.artworkActiveColor
                                                    border.width: flv.isFocused ? 1 : 0
                                                    color: rowWrapper.isHighlighted ? Colors.inputBackgroundColor : Colors.inputInactiveBackground

                                                    function removeKeyword(index) {
                                                        artItemsModel.removeKeywordAt(rowWrapper.getIndex(), index)
                                                    }

                                                    function removeLastKeyword() {
                                                        artItemsModel.removeLastKeyword(rowWrapper.getIndex())
                                                    }

                                                    function appendKeyword(keyword) {
                                                        artItemsModel.appendKeyword(rowWrapper.getIndex(), keyword)
                                                    }

                                                    function pasteKeywords(keywords) {
                                                        artItemsModel.pasteKeywords(rowWrapper.getIndex(), keywords)
                                                    }

                                                    EditableTags {
                                                        id: flv
                                                        model: rowWrapper.keywordsModel
                                                        anchors.fill: parent
                                                        property int keywordHeight: 20 * settingsModel.keywordSizeScale + (settingsModel.keywordSizeScale - 1)*10
                                                        scrollStep: keywordHeight
                                                        stealWheel: false
                                                        focus: true

                                                        delegate: KeywordWrapper {
                                                            id: kw
                                                            isHighlighted: rowWrapper.isHighlighted
                                                            delegateIndex: index
                                                            keywordText: keyword
                                                            itemHeight: flv.keywordHeight
                                                            hasSpellCheckError: !iscorrect
                                                            onRemoveClicked: keywordsWrapper.removeKeyword(kw.delegateIndex)

                                                            onActionDoubleClicked: {
                                                                var callbackObject = {
                                                                    onSuccess: function(replacement) {
                                                                        artItemsModel.editKeyword(rowWrapper.getIndex(), kw.delegateIndex, replacement)
                                                                    },
                                                                    onClose: function() {
                                                                        flv.activateEdit()
                                                                    }
                                                                }

                                                                Common.launchDialog("Dialogs/EditKeywordDialog.qml",
                                                                                    applicationWindow,
                                                                                    {
                                                                                        callbackObject: callbackObject,
                                                                                        previousKeyword: keyword,
                                                                                        keywordsModel: filteredArtItemsModel.getKeywordsModel(rowWrapper.delegateIndex)
                                                                                    })
                                                            }

                                                            onActionRightClicked: {
                                                                if (!iscorrect) {
                                                                    console.log("Context menu for add word " + kw.keywordText);
                                                                    addWordContextMenu.word = kw.keywordText;
                                                                    addWordContextMenu.popup()
                                                                }
                                                            }
                                                        }

                                                        onTagAdded: {
                                                            keywordsWrapper.appendKeyword(text)
                                                        }

                                                        onTagsPasted: {
                                                            keywordsWrapper.pasteKeywords(tagsList)
                                                        }

                                                        onRemoveLast: {
                                                            keywordsWrapper.removeLastKeyword()
                                                        }

                                                        onBackTabPressed: {
                                                            if (columnLayout.isWideEnough) {
                                                                titleTextInput.forceActiveFocus()
                                                                titleTextInput.cursorPosition = titleTextInput.text.length
                                                            } else {
                                                                descriptionTextInput.forceActiveFocus()
                                                                descriptionTextInput.cursorPosition = descriptionTextInput.text.length
                                                            }
                                                        }

                                                        onTabPressed: {
                                                            filteredArtItemsModel.focusNextItem(rowWrapper.delegateIndex)
                                                        }

                                                        onCopyRequest: clipboard.setText(keywordsstring)

                                                        onCompletionRequested: {
                                                            helpersWrapper.autoCompleteKeyword(prefix,
                                                                                               rowWrapper.keywordsModel)
                                                        }

                                                        onEditActivated: {
                                                            wrappersScope.updateCurrentIndex()
                                                            flv.activateEdit()
                                                        }
                                                    }

                                                    CustomScrollbar {
                                                        anchors.topMargin: -5
                                                        anchors.bottomMargin: -5
                                                        anchors.rightMargin: -15
                                                        flickable: flv
                                                    }
                                                }

                                                RowLayout {
                                                    anchors.left: parent.left
                                                    anchors.right: parent.right
                                                    anchors.rightMargin: 3
                                                    anchors.top: keywordsWrapper.bottom
                                                    anchors.topMargin: 3
                                                    spacing: 5

                                                    StyledText {
                                                        id: plainTextText
                                                        text: i18.n + qsTr("<u>edit in plain text</u>")
                                                        color: plainTextMA.containsMouse ? Colors.linkClickedColor : Colors.labelActiveForeground
                                                        visible: rowWrapper.isHighlighted

                                                        MouseArea {
                                                            id: plainTextMA
                                                            anchors.fill: parent
                                                            hoverEnabled: true
                                                            enabled: rowWrapper.isHighlighted
                                                            preventStealing: true
                                                            cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor
                                                            onClicked: {
                                                                // strange bug with clicking on the keywords field
                                                                if (!containsMouse) { return; }

                                                                var callbackObject = {
                                                                    onSuccess: function(text) {
                                                                        artItemsModel.plainTextEdit(rowWrapper.getIndex(), text)
                                                                    },
                                                                    onClose: function() {
                                                                        flv.activateEdit()
                                                                    }
                                                                }

                                                                Common.launchDialog("Dialogs/PlainTextKeywordsDialog.qml",
                                                                                    applicationWindow,
                                                                                    {
                                                                                        callbackObject: callbackObject,
                                                                                        keywordsText: keywordsstring,
                                                                                        keywordsModel: filteredArtItemsModel.getKeywordsModel(rowWrapper.delegateIndex)

                                                                                    });
                                                            }
                                                        }
                                                    }

                                                    Item {
                                                        Layout.fillWidth: true
                                                    }

                                                    StyledText {
                                                        id: fixSpellingText
                                                        text: i18.n + qsTr("Fix spelling")
                                                        enabled: rowWrapper.keywordsModel ? rowWrapper.keywordsModel.hasSpellErrors : false
                                                        color: enabled ? (fixSpellingMA.pressed ? Colors.linkClickedColor : Colors.artworkActiveColor)  : (rowWrapper.isHighlighted ? Colors.labelActiveForeground : Colors.labelInactiveForeground)

                                                        MouseArea {
                                                            id: fixSpellingMA
                                                            anchors.fill: parent
                                                            cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor

                                                            onClicked: {
                                                                artItemsModel.suggestCorrections(rowWrapper.getIndex())
                                                                Common.launchDialog("Dialogs/SpellCheckSuggestionsDialog.qml",
                                                                                    applicationWindow,
                                                                                    {})
                                                            }
                                                        }
                                                    }

                                                    StyledText {
                                                        text: "|"
                                                        isActive: rowWrapper.isHighlighted
                                                        verticalAlignment: Text.AlignVCenter
                                                    }

                                                    StyledText {
                                                        text: i18.n + qsTr("Suggest")
                                                        color: suggestKeywordsMA.pressed ? Colors.linkClickedColor : Colors.artworkActiveColor

                                                        MouseArea {
                                                            id: suggestKeywordsMA
                                                            anchors.fill: parent
                                                            cursorShape: Qt.PointingHandCursor
                                                            onClicked: {
                                                                var callbackObject = {
                                                                    promoteKeywords: function(keywords) {
                                                                        artItemsModel.addSuggestedKeywords(rowWrapper.getIndex(), keywords)
                                                                    }
                                                                }

                                                                Common.launchDialog("Dialogs/KeywordsSuggestion.qml",
                                                                                    applicationWindow,
                                                                                    {callbackObject: callbackObject});
                                                            }
                                                        }
                                                    }

                                                    StyledText {
                                                        text: "|"
                                                        isActive: rowWrapper.isHighlighted
                                                        verticalAlignment: Text.AlignVCenter
                                                    }

                                                    StyledText {
                                                        text: i18.n + qsTr("Copy")
                                                        color: copyKeywordsMA.pressed ? Colors.linkClickedColor : Colors.artworkActiveColor

                                                        MouseArea {
                                                            id: copyKeywordsMA
                                                            anchors.fill: parent
                                                            cursorShape: Qt.PointingHandCursor
                                                            onClicked: clipboard.setText(keywordsstring)
                                                        }
                                                    }

                                                    StyledText {
                                                        text: "|"
                                                        isActive: rowWrapper.isHighlighted
                                                        verticalAlignment: Text.AlignVCenter
                                                    }

                                                    StyledText {
                                                        text: i18.n + qsTr("Clear")
                                                        color: clearKeywordsMA.pressed ? Colors.linkClickedColor : Colors.artworkActiveColor

                                                        MouseArea {
                                                            id: clearKeywordsMA
                                                            anchors.fill: parent
                                                            cursorShape: Qt.PointingHandCursor
                                                            onClicked: filteredArtItemsModel.clearKeywords(rowWrapper.delegateIndex)
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }

                            }
                            Connections {
                                target: artItemsModel
                                onArtworksChanged: artworksHost.forceUpdateArtworks(needToMoveCurrentItem)
                            }

                            Connections {
                                target: filteredArtItemsModel
                                onAfterInvalidateFilter: {
                                    artworksHost.forceUpdateArtworks(true)
                                    artworksHost.positionViewAtBeginning()
                                }
                            }
                        }

                        CustomScrollbar {
                            id: mainScrollBar
                            anchors.topMargin: 0
                            anchors.bottomMargin: 0
                            anchors.rightMargin: -10
                            flickable: artworksHost
                        }
                    }

                    Item {
                        visible: artworksHost.count == 0
                        anchors.fill: parent

                        RowLayout {
                            anchors.centerIn: parent
                            spacing: 5

                            StyledText {
                                text: i18.n + qsTr("No items available.")
                                color: Colors.selectedArtworkBackground
                            }

                            StyledText {
                                text: i18.n + qsTr("Add files", "link")
                                color: addFilesMA.pressed ? Colors.linkClickedColor : Colors.artworkActiveColor

                                MouseArea {
                                    id: addFilesMA
                                    anchors.fill: parent
                                    cursorShape: Qt.PointingHandCursor
                                    onClicked: chooseArtworksDialog.open()
                                }
                            }

                            StyledText {
                                text: i18.n + qsTr("or")
                                color: Colors.selectedArtworkBackground
                            }

                            StyledText {
                                text: i18.n + qsTr("clear the filter")
                                color: clearFilterMA.pressed ? Colors.linkClickedColor : Colors.artworkActiveColor

                                MouseArea {
                                    id: clearFilterMA
                                    anchors.fill: parent
                                    cursorShape: Qt.PointingHandCursor
                                    onClicked: clearFilter()
                                }
                            }
                        }
                    }
                }
            }

            ClipboardHelper {
                id: clipboard
            }
        }
    }

    statusBar: StatusBar {
        RowLayout {
            width: parent.width
            height: 20
            implicitHeight: 20
            spacing: 5

            Item {
                width: 20
            }

            StyledText {
                text: i18.n + qsTr("Show logs")
                color: logsMA.pressed ? Colors.linkClickedColor : Colors.labelInactiveForeground

                MouseArea {
                    id: logsMA
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        var logsModel = helpersWrapper.getLogsModel()
                        var allText = logsModel.getAllLogsText()
                        Common.launchDialog("Dialogs/LogsDialog.qml",
                                            applicationWindow,
                                            {
                                                logText: allText
                                            });
                    }
                }
            }

            StyledText {
                text: "|"
                color: Colors.labelInactiveForeground
                verticalAlignment: Text.AlignVCenter
            }

            StyledText {
                text: i18.n + qsTr("Check warnings")
                color: warningsMA.pressed ? Colors.linkClickedColor : warningsModel.warningsCount > 0 ? Colors.artworkModifiedColor : Colors.labelInactiveForeground

                MouseArea {
                    id: warningsMA
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        warningsModel.update()
                        //filteredArtItemsModel.checkForWarnings()
                        Common.launchDialog("Dialogs/WarningsDialog.qml", applicationWindow, {
                                                componentParent: applicationWindow
                                            });
                    }
                }
            }

            StyledText {
                visible: applicationWindow.showUpdateLink
                text: "|"
                color: Colors.labelInactiveForeground
                verticalAlignment: Text.AlignVCenter
            }

            StyledText {
                visible: applicationWindow.showUpdateLink
                enabled: applicationWindow.showUpdateLink
                text: i18.n + qsTr("Update available!")
                color: updateMA.pressed ? Colors.linkClickedColor : Colors.greenColor

                MouseArea {
                    id: updateMA
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        Qt.openUrlExternally("https://ribtoks.github.io/xpiks/downloads/")
                    }
                }
            }

            Item {
                Layout.fillWidth: true
            }

            StyledText {
                id: filteredCountText
                text: i18.n + qsTr("No items available")
                color: Colors.labelInactiveForeground
                verticalAlignment: Text.AlignVCenter

                function updateText() {
                    var itemsCount = filteredArtItemsModel.getItemsCount()
                    if (itemsCount > 0) {
                        text = itemsCount > 1 ? qsTr("%1 items available").arg(itemsCount) : qsTr("1 item available")
                    } else {
                        text = qsTr("No items available")
                    }
                }

                Component.onCompleted: updateText()
                Connections {
                    target: filteredArtItemsModel
                    onRowsInserted: filteredCountText.updateText()
                    onRowsRemoved: filteredCountText.updateText()
                }

                Connections {
                    target: languagesModel
                    onLanguageChanged: filteredCountText.updateText()
                }
            }

            StyledText {
                text: "|"
                color: Colors.labelInactiveForeground
                verticalAlignment: Text.AlignVCenter
            }

            StyledText {
                text: i18.n + getOriginalText()
                color: Colors.labelInactiveForeground
                verticalAlignment: Text.AlignVCenter

                function getOriginalText() {
                    return filteredArtItemsModel.selectedArtworksCount > 1 ? qsTr("%1 selected items").arg(filteredArtItemsModel.selectedArtworksCount) : (filteredArtItemsModel.selectedArtworksCount === 1 ? qsTr("1 selected item") : qsTr("No selected items"))
                }

                MouseArea {
                    id: selectSelectedMA
                    anchors.fill: parent
                    cursorShape: filteredArtItemsModel.selectedArtworksCount > 0 ? Qt.PointingHandCursor : Qt.ArrowCursor
                    onClicked: {
                        if (filteredArtItemsModel.selectedArtworksCount > 0) {
                            filteredArtItemsModel.searchTerm = "x:selected"
                        }
                    }
                }
            }

            StyledText {
                text: "|"
                color: Colors.labelInactiveForeground
                verticalAlignment: Text.AlignVCenter
            }

            StyledText {
                text: i18.n + getOriginalText()
                verticalAlignment: Text.AlignVCenter
                color: artItemsModel.modifiedArtworksCount > 0 ? Colors.artworkModifiedColor : Colors.labelInactiveForeground

                function getOriginalText() {
                    return artItemsModel.modifiedArtworksCount > 1 ? qsTr("%1 modified items").arg(artItemsModel.modifiedArtworksCount) : (artItemsModel.modifiedArtworksCount === 1 ? qsTr("1 modified item") : qsTr("No modified items"))
                }

                MouseArea {
                    id: selectModifiedMA
                    anchors.fill: parent
                    cursorShape: artItemsModel.modifiedArtworksCount > 0 ? Qt.PointingHandCursor : Qt.ArrowCursor
                    onClicked: {
                        if (artItemsModel.modifiedArtworksCount > 0) {
                            filteredArtItemsModel.searchTerm = "x:modified"
                        }
                    }
                }
            }

            Item {
                width: 20
            }
        }

        style: StatusBarStyle {
            background: Rectangle {
                implicitHeight: 20
                implicitWidth: 200
                color: Colors.defaultDarkColor
            }
        }
    }
}
