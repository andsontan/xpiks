/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
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
import QtQuick.Layouts 1.3
import QtQuick.Window 2.0
import xpiks 1.0
import "Constants" 1.0
import "Components"
import "StyledControls"
import "Dialogs"
import "StackViews"
import "Common.js" as Common
import "Constants/UIConfig.js" as UIConfig

ApplicationWindow {
    id: applicationWindow
    visible: true
    width: settingsModel.getAppWidth(930)
    height: settingsModel.getAppHeight(725)
    minimumHeight: 725
    minimumWidth: 930
    title: i18.n + (debug ? "Xpiks (Devel)" : qsTr("Xpiks"))
    property int openedDialogsCount: 0
    property bool showUpdateLink: false
    property bool needToCenter: true
    property bool listLayout: true
    property var spellCheckService: helpersWrapper.getSpellCheckerService()
    property bool leftSideCollapsed: false

    onVisibleChanged: {
        if (needToCenter) {
            needToCenter = false
            applicationWindow.x = settingsModel.getAppPosX((Screen.width - applicationWindow.width) / 2)
            applicationWindow.y = settingsModel.getAppPosY((Screen.height - applicationWindow.height) / 2)
        }
    }

    function closeHandler(close) {
        console.info("closeHandler")
        settingsModel.saveRecentDirectories()
        settingsModel.saveRecentFiles()

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
        settingsModel.protectTelemetry();
        saveAppGeometry()
        closingTimer.start()
    }

    function saveAppGeometry() {
        console.debug("Saving application geometry")
        settingsModel.setAppWidth(applicationWindow.width)
        settingsModel.setAppHeight(applicationWindow.height)
        settingsModel.setAppPosX(applicationWindow.x)
        settingsModel.setAppPosY(applicationWindow.y)
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

            if (settingsModel.needToShowWhatsNew()) {
                Common.launchDialog("Dialogs/WhatsNewDialog.qml", applicationWindow, {})
            } else if (settingsModel.needToShowTextWhatsNew()) {
                var text = settingsModel.whatsNewText;
                if (text.length > 0) {
                    Common.launchDialog("Dialogs/WhatsNewMinorDialog.qml",
                                        applicationWindow,
                                        {
                                            whatsNewText: text
                                        })
                }
            }

            if (settingsModel.needToShowTermsAndConditions()) {
                var licenseText = settingsModel.termsAndConditionsText;
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
        return settingsModel.mustUseConfirmations
    }

    function openUploadDialog(skipUploadItems) {
        if (settingsModel.mustUseMasterPassword) {
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

    function collapseLeftPane() {
        leftDockingGroup.state = "collapsed"
        applicationWindow.leftSideCollapsed = true
    }

    function expandLeftPane() {
        leftDockingGroup.state = ""
        applicationWindow.leftSideCollapsed = false
    }

    function toggleLeftPane() {
        leftDockingGroup.state = (leftDockingGroup.state == "collapsed") ? "" : "collapsed"
        applicationWindow.leftSideCollapsed = !leftSideCollapsed
    }

    function startOneItemEditing(metadata, index, originalIndex) {
        var keywordsModel = filteredArtItemsModel.getBasicModel(index)
        artworkProxy.setSourceArtwork(metadata, originalIndex)
        var wasCollapsed = applicationWindow.leftSideCollapsed
        applicationWindow.collapseLeftPane()
        mainStackView.push({
                               item: "qrc:/StackViews/ArtworkEditView.qml",
                               properties: {
                                   artworkIndex: index,
                                   keywordsModel: keywordsModel,
                                   componentParent: applicationWindow,
                                   wasLeftSideCollapsed: wasCollapsed
                               },
                               destroyOnPop: true
                           })
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
        id: upgradeAction
        text: i18.n + qsTr("&Upgrade Now!")
        enabled: helpersWrapper.isUpdateDownloaded && (applicationWindow.openedDialogsCount == 0)
        onTriggered: {
            helpersWrapper.setUpgradeConsent()
            closeHandler({accepted: false})
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
                var index = filteredArtItemsModel.findSelectedItemIndex()

                if (index !== -1) {
                    var originalIndex = filteredArtItemsModel.getOriginalIndex(index)
                    var metadata = filteredArtItemsModel.getArtworkMetadata(index)
                    startOneItemEditing(metadata, index, originalIndex)
                    launched = true
                }

                if (!launched) {
                    // also as fallback in case of errors in findSelectedIndex
                    filteredArtItemsModel.combineSelectedArtworks();
                    mainStackView.push({
                                           item: "qrc:/StackViews/CombinedEditView.qml",
                                           properties: {
                                               componentParent: applicationWindow
                                           },
                                           destroyOnPop: true
                                       })
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
        id: searchAndReplaceAction
        shortcut: "Shift+Ctrl+F"
        onTriggered: openFindAndReplaceDialog()
        enabled: (artworkRepository.artworksSourcesCount > 0) && (applicationWindow.openedDialogsCount == 0)
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

            Menu {
                id: recentFilesMenu
                title: i18.n + qsTr("&Recent files")
                enabled: applicationWindow.openedDialogsCount == 0

                Instantiator {
                    model: recentFiles
                    onObjectAdded: recentFilesMenu.insertItem( index, object )
                    onObjectRemoved: recentFilesMenu.removeItem( object )

                    delegate: MenuItem {
                        text: display
                        onTriggered: {
                            var filesAdded = artItemsModel.addRecentFile(display)
                            if (filesAdded === 0) {
                                noNewFilesDialog.open()
                            }
                        }
                    }
                }
            }

            MenuItem {
                action: upgradeAction
                visible: helpersWrapper.isUpdateDownloaded
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
                text: i18.n + qsTr("&Presets")
                onTriggered: {
                    console.info("Presets triggered")
                    Common.launchDialog("Dialogs/PresetsEditDialog.qml", applicationWindow, {})
                }
            }

            MenuItem {
                text: i18.n + qsTr("&Invert selection")
                onTriggered: {
                    console.info("Invert selection triggered")
                    if (filteredArtItemsModel.getItemsCount() > 0) {
                        filteredArtItemsModel.invertSelectionArtworks()
                    }
                }
            }

            MenuItem {
                text: i18.n + qsTr("&Sort by filename")
                checkable: true
                onToggled: {
                    console.info("Sort by filename")
                    if (filteredArtItemsModel.getItemsCount() > 0) {
                        filteredArtItemsModel.toggleSorted();
                    }
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
                text: i18.n + qsTr("&Fix spelling in selected")
                enabled: filteredArtItemsModel.selectedArtworksCount > 0
                onTriggered: {
                    console.info("Fix spelling in selected triggered")
                    filteredArtItemsModel.suggestCorrectionsForSelected()
                    Common.launchDialog("Dialogs/SpellCheckSuggestionsDialog.qml",
                                        applicationWindow,
                                        {})
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
                text: i18.n + qsTr("&Manage upload hosts")
                onTriggered: {
                    console.info("Manage upload hosts triggered")
                    openUploadDialog(true)
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

            Menu {
                title: i18.n + qsTr("&Advanced")

                MenuItem {
                    text: i18.n + qsTr("&Cleanup local library in background")
                    onTriggered: {
                        console.info("Cleanup local library triggered")
                        helpersWrapper.cleanupLocalLibrary()
                    }
                }

                MenuItem {
                    text: i18.n + qsTr("Manage &user dictionary")
                    onTriggered: {
                        console.info("User dictionary triggered")
                        userDictEditModel.initializeModel()
                        Common.launchDialog("Dialogs/UserDictEditDialog.qml", applicationWindow, {})
                    }
                }

                MenuItem {
                    text: i18.n + qsTr("Show logs")
                    onTriggered: {
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
                text: "Update all items"
                onTriggered: {
                    artItemsModel.updateAllItems()
                }
            }

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
                    var text = settingsModel.whatsNewText;
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

            MenuItem {
                text: "Install update"
                onTriggered: {
                    Common.launchDialog("Dialogs/InstallUpdateDialog.qml", applicationWindow, {})
                }
            }
        }
    }

    Menu {
        id: artworkContextMenu
        property string filename
        property int index

        MenuItem {
            text: i18.n + qsTr("Copy to Quick Buffer")
            onTriggered: filteredArtItemsModel.copyToQuickBuffer(artworkContextMenu.index)
        }

        MenuItem {
            text: i18.n + qsTr("Fill from Quick Buffer")
            onTriggered: filteredArtItemsModel.fillFromQuickBuffer(artworkContextMenu.index)
        }

        MenuItem {
            text: i18.n + qsTr("Show in folder")
            onTriggered: helpersWrapper.revealArtworkFile(artworkContextMenu.filename);
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
        text: i18.n + qsTr("Some files are not available anymore.\nThey will be removed from the workflow.")
        standardButtons: StandardButton.Ok
        onAccepted: {
            helpersWrapper.removeUnavailableFiles()
        }
    }

    MessageDialog {
        id: unavailableVectorsDialog
        title: i18.n + qsTr("Warning")
        text: i18.n + qsTr("Some vectors are not available anymore.\nThey will be detached automatically.")
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
        nameFilters: [ "Image files (*.jpg *.tiff *.tif *.eps *.ai)", "All files (*)" ]

        onAccepted: {
            console.debug("You chose: " + chooseArtworksDialog.fileUrls)
            var filesAdded = artItemsModel.addLocalArtworks(chooseArtworksDialog.fileUrls)
            if (filesAdded > 0) {
                settingsModel.saveRecentDirectories()
                settingsModel.saveRecentFiles()
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
                settingsModel.saveRecentDirectories()
                settingsModel.saveRecentFiles()
                console.debug("" + filesAdded + ' files via Open Directory')
            } else {
                noNewFilesDialog.open()
            }
        }

        onRejected: {
            console.debug("Directory dialog canceled")
        }
    }

    FileDialog {
        id: openDictionaryDialog
        title: "Select any dictionary file"
        selectExisting: true
        selectMultiple: false
        folder: shortcuts.home
        nameFilters: [ "StarDict files (*.ifo *.idx *.dict *.idx.dz *.dict.dz)"]

        onAccepted: {
            console.debug("You chose: " + openDictionaryDialog.fileUrl)
            var added = translationManager.addDictionary(openDictionaryDialog.fileUrl)
            if (added) {
                dictionaryAddedMB.open()
            }
        }

        onRejected: {
            console.debug("Add dictionary dialog canceled")
        }
    }

    MessageDialog {
        id: dictionaryAddedMB
        title: i18.n + qsTr("Info")
        text: i18.n + qsTr("Dictionary has been added")
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

            var latestDir = recentDirectories.getLatestItem()
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

        onUpdateDownloaded: {
            if (applicationWindow.openedDialogsCount == 0) {
                Common.launchDialog("Dialogs/InstallUpdateDialog.qml", applicationWindow, {})
            } else {
                console.debug("Opened dialogs found. Postponing upgrade flow...");
                upgradeTimer.start()
            }
        }

        onUpgradeInitiated: {
            console.debug("UI:onUpgradeInitiated handler")
            closeHandler({accepted: false});
        }
    }

    Timer {
        id: upgradeTimer
        interval: 5000
        repeat: true
        running: false
        triggeredOnStart: false
        onTriggered: {
            if (applicationWindow.openedDialogsCount == 0) {
                upgradeTimer.stop()
                Common.launchDialog("Dialogs/InstallUpdateDialog.qml", applicationWindow, {})
            }
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
                        settingsModel.saveRecentDirectories()
                        settingsModel.saveRecentFiles()
                        console.debug(filesCount + ' files added via drag&drop')
                    }
                }
            }
        }

        Item {
            id: leftDockingGroup
            width: 250
            anchors.left: parent.left
            anchors.leftMargin: leftCollapser.width
            anchors.top: parent.top
            anchors.bottom: parent.bottom

            states: [
                State {
                    name: "collapsed"

                    PropertyChanges {
                        target: leftDockingGroup
                        anchors.leftMargin: -leftDockingGroup.width + leftCollapser.width
                    }
                }
            ]

            Behavior on anchors.leftMargin {
                NumberAnimation {
                    duration: 100
                    easing.type: Easing.InQuad
                }
            }

            Row {
                id: tabsHolder
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                property int currentIndex: 0
                property bool moreTabsAvailable: tabsModel.tabsCount > 3
                property real expanderWidth: tabsHolder.moreTabsAvailable ? plusTab.width : 0
                height: 45
                spacing: 0

                Repeater {
                    id: tabsRepeater
                    model: activeTabs

                    delegate: CustomTab {
                        id: customTab
                        tabIndex: index
                        width: (tabsHolder.width - tabsHolder.expanderWidth) / tabsRepeater.count
                        isSelected: tabsHolder.currentIndex == tabIndex
                        hovered: (!isSelected) && tabMA.containsMouse

                        function activateThisTab() {
                            var tabIndex = customTab.tabIndex;
                            tabsHolder.currentIndex = tabIndex
                        }

                        Loader {
                            property bool isHighlighted: customTab.isSelected || customTab.hovered
                            property color parentBackground: customTab.color
                            anchors.centerIn: parent
                            source: tabicon
                        }

                        Connections {
                            target: activeTabs
                            onTabActivateRequested: {
                                console.log("On tab opened " + originalTabIndex)
                                if (activeTabs.getIndex(customTab.tabIndex) === originalTabIndex) {
                                    customTab.activateThisTab()
                                }
                            }
                        }

                        StyledText {
                            enabled: debugTabs
                            visible: debugTabs
                            anchors.top: parent.top
                            anchors.right: parent.right
                            text: cachetag
                            isActive: false
                        }

                        MouseArea {
                            id: tabMA
                            anchors.fill: parent
                            hoverEnabled: true
                            onClicked: {
                                customTab.activateThisTab()
                                activeTabs.openTab(tabIndex)
                            }
                        }
                    }
                }

                CustomTab {
                    id: plusTab
                    enabled: tabsHolder.moreTabsAvailable
                    visible: tabsHolder.moreTabsAvailable
                    tabIndex: tabsRepeater.count
                    isSelected: tabsHolder.currentIndex == tabIndex
                    hovered: (!isSelected) && plusMA.containsMouse
                    width: 20
                    property bool isHighlighted: isSelected || hovered

                    TriangleElement {
                        anchors.centerIn: parent
                        anchors.verticalCenterOffset: isFlipped ? height*0.3 : 0
                        color: (enabled && (plusMA.containsMouse || plusTab.isHighlighted)) ? Colors.labelActiveForeground : Colors.labelInactiveForeground
                        isFlipped: !plusTab.isSelected
                        width: parent.width * 0.6
                        height: width * 0.5
                        enabled: tabsHolder.moreTabsAvailable
                    }

                    MouseArea {
                        id: plusMA
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            var tabIndex = tabsRepeater.count;
                            tabsHolder.currentIndex = tabIndex
                        }
                    }
                }
            }

            Rectangle {
                color: Colors.defaultControlColor
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: tabsHolder.bottom
                anchors.bottom: parent.bottom

                StackLayout {
                    id: mainTabView
                    anchors.fill: parent
                    currentIndex: tabsHolder.currentIndex

                    Repeater {
                        model: activeTabs

                        delegate: Loader {
                            id: tabLoader
                            source: tabcomponent
                            asynchronous: true
                            property int myIndex: index
                            property var tabModel: uiManager.retrieveTabsModel(tabid)
                            property int selectedArtworksCount: filteredArtItemsModel.selectedArtworksCount
                            property bool areActionsAllowed: mainStackView.areActionsAllowed

                            Connections {
                                target: mainTabView
                                onCurrentIndexChanged: {
                                    if (tabLoader.myIndex == mainTabView.currentIndex) {
                                        if (tabLoader.status == Loader.Ready) {
                                            if (typeof tabLoader.item.initializeTab !== "undefined") {
                                                tabLoader.item.initializeTab()
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }

                    Flow {
                        anchors.fill: parent
                        anchors.leftMargin: 5
                        anchors.topMargin: 5
                        spacing: 5

                        Repeater {
                            id: inactiveTabsRepeater
                            model: inactiveTabs

                            delegate: CustomTab {
                                anchors.top: undefined
                                anchors.bottom: undefined
                                id: customInactiveTab
                                tabIndex: index
                                width: (tabsHolder.width - plusTab.width) / tabsRepeater.count
                                height: 45
                                isSelected: false
                                hovered: (!isSelected) && inactiveTabMA.containsMouse

                                Loader {
                                    property bool isHighlighted: customInactiveTab.isSelected || customInactiveTab.hovered
                                    property color parentBackground: customInactiveTab.color
                                    anchors.centerIn: parent
                                    source: tabicon
                                }

                                StyledText {
                                    enabled: debugTabs
                                    visible: debugTabs
                                    anchors.top: parent.top
                                    anchors.right: parent.right
                                    text: cachetag
                                    isActive: false
                                }

                                MouseArea {
                                    id: inactiveTabMA
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    onClicked: {
                                        inactiveTabs.openTab(tabIndex)
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        // hack for visual order of components (slider will be created after left panel)
        // in order not to deal with Z animation/settings
        Rectangle {
            id: leftCollapser
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            color: Colors.leftSliderColor
            width: 20

            TriangleElement {
                width: leftCollapseMA.pressed ? 6 : 7
                height: leftCollapseMA.pressed ? 12 : 14
                isVertical: true
                isFlipped: !applicationWindow.leftSideCollapsed
                anchors.centerIn: parent
                anchors.horizontalCenterOffset: isFlipped ? -1 : 0
                color: {
                    if (leftCollapseMA.pressed) {
                        return Colors.whiteColor
                    } else {
                        return leftCollapseMA.containsMouse ? Colors.labelInactiveForeground : Colors.inputBackgroundColor
                    }
                }
            }

            MouseArea {
                id: leftCollapseMA
                hoverEnabled: true
                anchors.fill: parent
                onClicked: toggleLeftPane()
            }
        }

        StackView {
            id: mainStackView
            anchors.left: leftDockingGroup.right
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            focus: true
            property bool areActionsAllowed: depth <= 1

            initialItem: MainGrid {
            }

            delegate: StackViewDelegate {
                function transitionFinished(properties)
                {
                    properties.exitItem.opacity = 1
                }

                pushTransition: StackViewTransition {
                    PropertyAnimation {
                        target: enterItem
                        property: "opacity"
                        from: 0
                        to: 1
                        duration: 100
                    }

                    PropertyAnimation {
                        target: exitItem
                        property: "opacity"
                        from: 1
                        to: 0
                        duration: 100
                    }
                }
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
                text: i18.n + qsTr("Check warnings")
                color: warningsMA.pressed ? Colors.linkClickedColor : (warningsModel.warningsCount > 0 ? Colors.artworkModifiedColor : Colors.labelInactiveForeground)

                MouseArea {
                    id: warningsMA
                    anchors.fill: parent
                    cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
                    enabled: mainStackView.areActionsAllowed
                    onClicked: {
                        warningsModel.update()
                        //filteredArtItemsModel.checkForWarnings()

                        var wasCollapsed = applicationWindow.leftSideCollapsed
                        applicationWindow.collapseLeftPane()
                        mainStackView.push({
                                               item: "qrc:/StackViews/WarningsView.qml",
                                               properties: {
                                                   componentParent: applicationWindow,
                                                   wasLeftSideCollapsed: wasCollapsed
                                               },
                                               destroyOnPop: true
                                           })
                    }
                }
            }

            StyledText {
                visible: applicationWindow.showUpdateLink
                text: "|"
                isActive: false
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
                isActive: false
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
                    enabled: mainStackView.areActionsAllowed
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
                isActive: false
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
                    enabled: mainStackView.areActionsAllowed
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
                color: Colors.statusBarColor
            }
        }
    }
}
