/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2015 Taras Kushnir <kushnirTV@gmail.com>
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
import xpiks 1.0
import "Constants"
import "Constants/Colors.js" as Colors
import "Components"
import "StyledControls"
import "Dialogs"
import "Common.js" as Common

ApplicationWindow {
    id: applicationWindow
    visible: true
    width: 900
    height: 670
    minimumHeight: 670
    minimumWidth: 900
    title: qsTr("Xpiks")
    property int openedDialogsCount: 0
    property bool showUpdateLink: false

    function saveRecentDirectories() {
        appSettings.setValue(appSettings.recentDirectoriesKey, recentDirectories.serializeForSettings())
    }

    onClosing: {
        saveRecentDirectories()

        if (artItemsModel.modifiedArtworksCount > 0) {
            close.accepted = false
            configExitDialog.open()
        }
    }

    function onDialogClosed() {
        openedDialogsCount -= 1
    }

    function mustUseConfirmation() {
        var mustUse = appSettings.boolValue(appSettings.useConfirmationDialogsKey, true)
        return mustUse
    }

    function openUploadDialog() {
        if (appSettings.boolValue(appSettings.mustUseMasterPasswordKey, false)) {
            var callbackObject = {
                onSuccess: function() { doOpenUploadDialog(true) },
                onFail: function() { doOpenUploadDialog(false) }
            }

            Common.launchDialog("Dialogs/EnterMasterPasswordDialog.qml",
                         applicationWindow,
                         {componentParent: applicationWindow, callbackObject: callbackObject})
        } else {
            doOpenUploadDialog(true)
        }
    }

    function doOpenUploadDialog(masterPasswordCorrectOrEmpty) {
        artworkUploader.resetModel()
        filteredArtItemsModel.setSelectedForUpload()
        uploadInfos.initializeAccounts(masterPasswordCorrectOrEmpty)

        Common.launchDialog("Dialogs/UploadArtworks.qml",
                     applicationWindow,
                     {componentParent: applicationWindow})
    }

    function clearFilter() {
        filteredArtItemsModel.searchTerm = ''
        if (filterText.length > 0) {
            filterText.text = ''
        }
    }

    menuBar: MenuBar {
        Menu {
            title: qsTr("File")
            Menu {
                id: recentDirectoriesMenu
                title: qsTr("Recent directories")

                Instantiator {
                    model: recentDirectories
                    onObjectAdded: recentDirectoriesMenu.insertItem( index, object )
                    onObjectRemoved: recentDirectoriesMenu.removeItem( object )

                    delegate: MenuItem {
                        text: display
                        onTriggered: artItemsModel.addRecentDirectory(display)
                    }
                }
            }

            MenuItem {
                text: qsTr("&Settings")
                onTriggered: {
                    settingsModel.readAllValues()
                    Common.launchDialog("Dialogs/SettingsWindow.qml",
                                    applicationWindow, {},
                                    function(wnd) {wnd.show();});
                }
            }
            MenuItem {
                text: qsTr("&About")
                onTriggered: {
                    Common.launchDialog("Dialogs/AboutWindow.qml",
                                    applicationWindow, {},
                                    function(wnd) {wnd.show();});
                }
            }

            MenuItem {
                text: qsTr("Exit")
                onTriggered: Qt.quit();
            }
        }

        Menu {
            title: qsTr("Tools")
            enabled: applicationWindow.openedDialogsCount == 0

            MenuItem {
                text: qsTr("&Zip selected artworks")
                enabled: filteredArtItemsModel.selectedArtworksCount > 0
                onTriggered: {
                    console.log("Zip archives triggered")

                    filteredArtItemsModel.setSelectedForZipping()
                    Common.launchDialog("Dialogs/ZipArtworksDialog.qml",
                                    applicationWindow,
                                    {componentParent: applicationWindow});
                }
            }

            MenuItem {
                text: qsTr("&Import metadata from selected")
                enabled: filteredArtItemsModel.selectedArtworksCount > 0
                onTriggered: {
                    console.log("Reimport archives triggered")
                    filteredArtItemsModel.reimportMetadataForSelected()
                }
            }

            MenuItem {
                text: qsTr("&Check spelling in selected")
                enabled: filteredArtItemsModel.selectedArtworksCount > 0
                onTriggered: {
                    console.log("Spell check in selected")
                    filteredArtItemsModel.spellCheckSelected()
                    Common.launchDialog("Dialogs/SpellCheckDialog.qml",
                                        applicationWindow, {});
                }
            }

            MenuItem {
                text: qsTr("&Remove metadata from selected")
                enabled: filteredArtItemsModel.selectedArtworksCount > 0
                onTriggered: {
                    console.log("Remove metadata from selected triggered")
                    removeMetadataDialog.open()
                }
            }

            MenuItem {
                text: qsTr("&Cleanup local library in background")
                onTriggered: {
                    console.log("Cleanup local library triggered")
                    iptcProvider.cleanupLibrary()
                }
            }
        }
    }

    MessageDialog {
        id: configExitDialog

        title: "Confirmation"
        text: qsTr("You have some artworks modified. Really exit?")
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            Qt.quit()
        }
    }

    MessageDialog {
        id: removeMetadataDialog

        title: "Confirmation"
        text: qsTr("Remove metadata from selected artworks?")
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            filteredArtItemsModel.removeMetadataInSelected()
        }
    }

    MessageDialog {
        id: confirmRemoveSelectedDialog
        property int itemsCount
        title: "Confirmation"
        text: qsTr("Are you sure you want to remove %1 item(s)?").arg(itemsCount)
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            doRemoveSelectedArtworks()
        }
    }

    function doRemoveSelectedArtworks() {
        filteredArtItemsModel.removeSelectedArtworks()
        filteredArtItemsModel.checkForWarnings()
    }

    function tryUploadArtworks() {
        if (filteredArtItemsModel.areSelectedArtworksSaved()) {
            openUploadDialog()
        } else {
            mustSaveWarning.open()
        }
    }

    MessageDialog {
        id: confirmRemoveDirectoryDialog
        property int directoryIndex
        title: "Confirmation"
        text: qsTr("Are you sure you want to remove this directory?")
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            filteredArtItemsModel.removeArtworksDirectory(directoryIndex)
            filteredArtItemsModel.checkForWarnings()
        }
    }

    FileDialog {
        id: chooseArtworksDialog
        title: "Please choose artworks"
        selectExisting: true
        selectMultiple: true
        nameFilters: [ "Jpeg images (*.jpg), Tiff images(*.tiff), All files (*)" ]

        onAccepted: {
            console.log("You chose: " + chooseArtworksDialog.fileUrls)
            var filesAdded = artItemsModel.addLocalArtworks(chooseArtworksDialog.fileUrls)
            saveRecentDirectories()
            console.log(filesAdded + ' files via Open File(s)')
        }

        onRejected: {
            console.log("Artworks dialog canceled")
        }
    }

    FileDialog {
        id: chooseDirectoryDialog
        title: "Please choose artworks"
        selectExisting: true
        selectMultiple: false
        selectFolder: true

        onAccepted: {
            console.log("You chose: " + chooseDirectoryDialog.fileUrls)
            var filesAdded = artItemsModel.addLocalDirectories(chooseDirectoryDialog.fileUrls)
            saveRecentDirectories()
            console.log(filesAdded + ' files via Open Directory')
        }

        onRejected: {
            console.log("Directory dialog canceled")
        }
    }

    MessageDialog {
        id: mustSaveWarning
        title: "Warning"
        text: qsTr("Please, save selected items before upload")
    }

    MessageDialog {
        id: mustSelectDialog
        title: "Warning"
        text: qsTr("Please, select some artworks first")
    }

    MessageDialog {
        id: alreadySavedDialog
        title: "Information"
        text: qsTr("All selected items are already saved")
    }

    Connections {
        target: artItemsModel
        onArtworksAdded: {
           if (count > 0) {
               Common.launchDialog("Dialogs/ImportMetadata.qml", applicationWindow, {})
           }
        }
    }

    Connections {
        target: updateService
        onUpdateAvailable: {
            if (Qt.platform.os !== "linux") {
                Common.launchDialog("Dialogs/UpdateWindow.qml",
                                    applicationWindow, {updateUrl: updateLink},
                                    function(wnd) {wnd.show();});
            }

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
                    saveRecentDirectories()
                    console.log(filesCount + ' files added via drag&drop')
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

                RowLayout {
                    spacing: 10
                    anchors.fill: parent
                    anchors.margins: { top: 10; left: 10 }

                    StyledButton {
                        text: qsTr("Add directory")
                        width: 120
                        onClicked: chooseDirectoryDialog.open()
                    }

                    StyledButton {
                        text: qsTr("Add files")
                        width: 100
                        onClicked: chooseArtworksDialog.open()
                    }

                    Item {
                        Layout.fillWidth: true
                    }
                }
            }

            Rectangle {
                height: 1
                width: parent.width
                color: Colors.itemsSourceSelected
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

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    filteredArtItemsModel.selectDirectory(sourceWrapper.delegateIndex)
                                }
                            }

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
                                    color: Colors.itemsSourceForeground
                                    text: path + " (" + usedimagescount + ")"
                                    elide: Text.ElideMiddle
                                    font.bold: true
                                }

                                CloseIcon {
                                    width: 14
                                    height: 14
                                    anchors.verticalCenter: parent.verticalCenter
                                    isActive: false

                                    onItemClicked: {
                                        if (mustUseConfirmation()) {
                                            confirmRemoveDirectoryDialog.directoryIndex = sourceWrapper.delegateIndex
                                            confirmRemoveDirectoryDialog.open()
                                        } else {
                                            filteredArtItemsModel.removeArtworksDirectory(sourceWrapper.delegateIndex)
                                            filteredArtItemsModel.checkForWarnings()
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
                z: 10000
                anchors.left: parent.left
                anchors.right: parent.right

                RowLayout {
                    spacing: 10
                    anchors.fill: parent
                    anchors.margins: { top: 10; left: 10 }
                    anchors.rightMargin: mainScrollView.areScrollbarsVisible ? 20 : 10

                    Item {
                        Layout.fillWidth: true
                    }

                    StyledButton {
                        text: qsTr("Remove")
                        enabled: artworkRepository.artworksSourcesCount > 0
                        width: 80
                        onClicked: {
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

                    StyledButton {
                        text: qsTr("Edit")
                        width: mainScrollView.areScrollbarsVisible ? 72 : 82
                        enabled: artworkRepository.artworksSourcesCount > 0
                        onClicked: {
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
                                    combinedArtworks.resetModelData();
                                    filteredArtItemsModel.combineSelectedArtworks();
                                    Common.launchDialog("Dialogs/CombinedArtworksDialog.qml", applicationWindow, {componentParent: applicationWindow});
                                }
                            }
                        }
                    }

                    StyledButton {
                        text: qsTr("Save")
                        width: 80
                        enabled: artworkRepository.artworksSourcesCount > 0
                        onClicked: {
                            if (filteredArtItemsModel.selectedArtworksCount == 0) {
                                mustSelectDialog.open()
                            }
                            else {
                                var modifiedSelectedCount = filteredArtItemsModel.getModifiedSelectedCount();

                                if (filteredArtItemsModel.selectedArtworksCount > 0 && modifiedSelectedCount > 0) {
                                    iptcProvider.resetModel()
                                    filteredArtItemsModel.saveSelectedArtworks()
                                    Common.launchDialog("Dialogs/ExportMetadata.qml", applicationWindow, {})
                                } else {
                                    if (modifiedSelectedCount === 0) {
                                        alreadySavedDialog.open()
                                    }
                                }
                            }
                        }
                    }

                    StyledButton {
                        text: qsTr("Upload")
                        width: 90
                        enabled: artworkRepository.artworksSourcesCount > 0
                        onClicked: {
                            if (filteredArtItemsModel.selectedArtworksCount === 0) {
                                filteredArtItemsModel.selectFilteredArtworks();
                            }

                            if (filteredArtItemsModel.selectedArtworksCount > 0) {
                                tryUploadArtworks();
                            }
                        }
                    }

                    Item {
                        width: 10
                    }
                }
            }

            Rectangle {
                height: 1
                anchors.left: parent.left
                anchors.right: parent.right
                color: Colors.itemsSourceSelected
            }

            Rectangle {
                anchors.left: parent.left
                anchors.right: parent.right
                Layout.fillHeight: true
                color: Colors.defaultControlColor

                Item {
                    anchors.topMargin: 10
                    anchors.fill: parent

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
                            text: filteredArtItemsModel.selectedArtworksCount === 0 ? qsTr("Select all") : qsTr("Select none")
                            checked: filteredArtItemsModel.selectedArtworksCount > 0

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
                            width: (artworkRepository.artworksSourcesCount > 0 && mainScrollView.areScrollbarsVisible) ? 252 : 262
                            height: 24

                            StyledTextInput {
                                id: filterText
                                width: (artworkRepository.artworksSourcesCount > 0 && mainScrollView.areScrollbarsVisible) ? 220 : 230
                                height: 24
                                clip: true
                                anchors.left: parent.left
                                anchors.leftMargin: 5
                                color: Colors.defaultInputBackground
                                enabled: artworkRepository.artworksSourcesCount > 0

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
                                text: qsTr("Search...   x:empty  x:modified")
                                color: Colors.defaultInputBackground
                                opacity: (filterText.activeFocus || filterText.length > 0) ? 0 : 0.1
                                anchors.left: parent.left
                                anchors.leftMargin: 7
                                anchors.verticalCenter: parent.verticalCenter
                            }
                        }

                        StyledBlackButton {
                            id: searchButton
                            anchors.right: parent.right
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.rightMargin:  mainScrollView.areScrollbarsVisible ? 40 : 30
                            width: 90
                            text: qsTr("Search")
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

                        RowLayout {
                            anchors.fill: parent
                            spacing: 10

                            Rectangle {
                                color: Colors.artworkSavedColor
                                width: 6
                                Layout.fillHeight: true
                            }

                            Item {
                                Layout.fillWidth: true
                            }

                            StyledText {
                                id: undoDescription
                                text: undoRedoManager.undoDescription
                            }

                            Item {
                                width: 10
                            }

                            StyledText {
                                text: qsTr("Undo")
                                color: undoMA.pressed ? Colors.defaultLightColor : Colors.artworkActiveColor

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
                                text: qsTr("Dismiss (%1)").arg(settingsModel.dismissDuration - (autoDismissTimer.iterations % (settingsModel.dismissDuration + 1)))
                                color: dismissUndoMA.pressed ? Colors.defaultLightColor : Colors.defaultInputBackground

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

                            Item {
                                Layout.fillWidth: true
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
                    }

                    StyledScrollView {
                        id: mainScrollView
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.topMargin: 4
                        anchors.top: undoRedoRect.bottom
                        anchors.bottom: parent.bottom
                        property bool areScrollbarsVisible: flickableItem.contentHeight > flickableItem.height
                        __wheelAreaScrollSpeed: 50 + 10*settingsModel.scrollSpeedScale
                        // does not work for now in Qt 5.4.1 in combination with ListView
                        //verticalScrollBarPolicy: Qt.ScrollBarAlwaysOn

                        ListView {
                            id: imagesListView
                            model: filteredArtItemsModel
                            boundsBehavior: Flickable.StopAtBounds
                            spacing: 4

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

                            delegate: Rectangle {
                                id: rowWrapper
                                property bool isHighlighted: (isselected || descriptionTextInput.activeFocus || flv.isFocused || titleTextInput.activeFocus)
                                color: isHighlighted ? Colors.selectedArtworkColor : Colors.artworkImageBackground
                                property variant artworkModel: model

                                function getIndex() {
                                    return filteredArtItemsModel.getOriginalIndex(index)
                                }

                                function focusInputs() {
                                    descriptionTextInput.forceActiveFocus()
                                }

                                function isAnyFocused() {
                                    return descriptionTextInput.activeFocus || flv.isFocused || titleTextInput.activeFocus;
                                }

                                function focusIfNeeded() {
                                    if (!isAnyFocused()) {
                                        focusInputs()
                                    }

                                    if (!isselected) {
                                        descriptionTextInput.focus = false
                                        flv.removeFocus()
                                    }
                                }

                                width: parent.width
                                height: 200 + 80*(settingsModel.keywordSizeScale - 1.0)

                                Item {
                                    anchors.fill: parent
                                    anchors.rightMargin: 10

                                    Rectangle {
                                        id: isModifiedRectangle
                                        color: ismodified ? Colors.artworkModifiedColor : Colors.artworkSavedColor
                                        width: 6
                                        anchors.left: parent.left
                                        anchors.top: parent.top
                                        anchors.bottom: parent.bottom
                                    }

                                    Item {
                                        id: checkboxSpacer
                                        width: 5
                                        anchors.left: isModifiedRectangle.right
                                        anchors.leftMargin: 5
                                        anchors.top: parent.top
                                        anchors.bottom: parent.bottom

                                        MouseArea {
                                            anchors.fill: parent
                                            onClicked: {
                                                editisselected = !isselected
                                                rowWrapper.focusIfNeeded()
                                            }
                                        }
                                    }

                                    Item {
                                        id: checkboxRectangle
                                        anchors.left: checkboxSpacer.right
                                        anchors.leftMargin: 5
                                        anchors.top: parent.top
                                        anchors.bottom: parent.bottom
                                        width: itemCheckedCheckbox.width

                                        MouseArea {
                                            anchors.fill: parent
                                            onClicked: {
                                                editisselected = !isselected
                                                rowWrapper.focusIfNeeded()
                                            }
                                        }

                                        StyledCheckbox {
                                            id: itemCheckedCheckbox
                                            //checked: isselected
                                            anchors.centerIn: parent
                                            activeFocusOnPress: true
                                            onClicked: editisselected = checked
                                            Component.onCompleted: itemCheckedCheckbox.checked = isselected
                                            Connections {
                                                target: filteredArtItemsModel
                                                onSelectedArtworksCountChanged: {
                                                    itemCheckedCheckbox.checked = rowWrapper.artworkModel.isselected || false
                                                }
                                            }
                                        }
                                    }

                                    Item {
                                        id: imageColumnWrapper
                                        width: 180
                                        anchors.left: checkboxRectangle.right
                                        anchors.leftMargin: 5
                                        anchors.top: parent.top
                                        anchors.bottom: parent.bottom

                                        MouseArea {
                                            anchors.fill: parent
                                            onClicked: {
                                                editisselected = !isselected
                                                rowWrapper.focusIfNeeded()
                                            }
                                        }

                                        ColumnLayout {
                                            anchors.fill: parent
                                            anchors.margins: { left: 15; right: 15 }
                                            spacing: 5

                                            Item {
                                                height: 15
                                            }

                                            Item {
                                                width: 150
                                                height: 130
                                                anchors.horizontalCenter: parent.horizontalCenter

                                                Image {
                                                    id: artworkImage
                                                    anchors.fill: parent
                                                    source: "image://global/" + filename
                                                    sourceSize.width: 150
                                                    sourceSize.height: 150
                                                    fillMode: settingsModel.fitSmallPreview ? Image.PreserveAspectFit : Image.PreserveAspectCrop
                                                    asynchronous: true
                                                }

                                                MouseArea {
                                                    anchors.fill: parent
                                                    onClicked: {
                                                        editisselected = !isselected
                                                        rowWrapper.focusIfNeeded()
                                                    }
                                                    onDoubleClicked: Common.launchItemEditing(rowWrapper.getIndex(), applicationWindow, {
                                                                                                  applyCallback: function() {}
                                                                                              })
                                                }
                                            }

                                            StyledText {
                                                Layout.fillWidth: true
                                                elide: Text.ElideMiddle
                                                color: moreInfoMA.pressed ? Colors.defaultLightColor : Colors.defaultInputBackground
                                                horizontalAlignment: Text.AlignHCenter
                                                text: filename.split(/[\\/]/).pop()

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

                                            Item {
                                                Layout.fillHeight: true
                                            }
                                        }
                                    }

                                    Rectangle {
                                        id: columnRectangle
                                        anchors.top: parent.top
                                        anchors.bottom: parent.bottom
                                        anchors.left: imageColumnWrapper.right
                                        anchors.leftMargin: 5
                                        anchors.right: parent.right
                                        color: rowWrapper.isHighlighted  ? Colors.selectedMetadataColor : Colors.artworkBackground

                                        Item {
                                            id: columnLayout
                                            //spacing: 3
                                            anchors.fill: parent
                                            anchors.margins: { left: 20; right: 20 }
                                            property bool isWideEnough: width > 400

                                            StyledText {
                                                id: descriptionText
                                                anchors.left: parent.left
                                                anchors.top: parent.top
                                                text: qsTr("Description:")
                                            }

                                            StyledText {
                                                id: titleText
                                                anchors.left: titleRect.left
                                                anchors.top: parent.top
                                                visible: columnLayout.isWideEnough
                                                text: qsTr("Title:")
                                            }

                                            Rectangle {
                                                id: descriptionRect
                                                height: 30
                                                anchors.left: parent.left
                                                anchors.right: titleRect.left
                                                anchors.rightMargin: columnLayout.isWideEnough ? 20 : 0
                                                anchors.top: descriptionText.bottom
                                                anchors.topMargin: 3
                                                color: rowWrapper.isHighlighted ? Colors.defaultInputBackground : Colors.defaultControlColor
                                                border.color: Colors.artworkActiveColor
                                                border.width: descriptionTextInput.activeFocus ? 1 : 0

                                                StyledTextEdit {
                                                    id: descriptionTextInput
                                                    height: 30
                                                    anchors.left: parent.left
                                                    anchors.right: parent.right
                                                    anchors.leftMargin: 5
                                                    anchors.rightMargin: 5
                                                    font.pixelSize: 12 * settingsModel.keywordSizeScale
                                                    anchors.verticalCenter: parent.verticalCenter
                                                    //maximumLength: 250
                                                    text: description
                                                    color: rowWrapper.isHighlighted ? Colors.defaultLightColor : Colors.defaultInputBackground
                                                    onTextChanged: model.editdescription = text

                                                    Keys.onTabPressed: {
                                                        if (columnLayout.isWideEnough) {
                                                            titleTextInput.forceActiveFocus()
                                                        } else {
                                                            flv.activateEdit()
                                                        }
                                                    }

                                                    Keys.onPressed: {
                                                        if(event.matches(StandardKey.Paste)) {
                                                            var clipboardText = clipboard.getText();
                                                            // same regexp as in validator
                                                            descriptionTextInput.paste(clipboardText)
                                                            event.accepted = true
                                                        }
                                                    }

                                                    Component.onCompleted: {
                                                        var index = rowWrapper.getIndex()
                                                        artItemsModel.initDescriptionHighlighting(index, descriptionTextInput.textDocument)
                                                    }
                                                }
                                            }

                                            Rectangle {
                                                id: titleRect
                                                height: 30
                                                width: columnLayout.isWideEnough ? ((columnLayout.width / 2) - 10 ): 0
                                                visible: columnLayout.isWideEnough
                                                //anchors.left: descriptionRect.right
                                                anchors.right: parent.right
                                                anchors.top: descriptionText.bottom
                                                anchors.topMargin: 3
                                                color: rowWrapper.isHighlighted ? Colors.defaultInputBackground : Colors.defaultControlColor
                                                border.color: Colors.artworkActiveColor
                                                border.width: titleTextInput.activeFocus ? 1 : 0

                                                StyledTextEdit {
                                                    id: titleTextInput
                                                    height: 30
                                                    anchors.left: parent.left
                                                    anchors.right: parent.right
                                                    anchors.leftMargin: 5
                                                    anchors.rightMargin: 5
                                                    font.pixelSize: 12 * settingsModel.keywordSizeScale
                                                    anchors.verticalCenter: parent.verticalCenter
                                                    //maximumLength: 250
                                                    text: title
                                                    color: rowWrapper.isHighlighted ? Colors.defaultLightColor : Colors.defaultInputBackground
                                                    onTextChanged: model.edittitle = text
                                                    KeyNavigation.backtab: descriptionTextInput

                                                    Keys.onTabPressed: {
                                                        flv.activateEdit()
                                                    }

                                                    Keys.onPressed: {
                                                        if(event.matches(StandardKey.Paste)) {
                                                            var clipboardText = clipboard.getText();
                                                            // same regexp as in validator
                                                            titleTextInput.paste(clipboardText)
                                                            event.accepted = true
                                                        }
                                                    }

                                                    Component.onCompleted: {
                                                        var index = rowWrapper.getIndex()
                                                        artItemsModel.initTitleHighlighting(index, titleTextInput.textDocument)
                                                    }
                                                }
                                            }

                                            RowLayout {
                                                id: keywordsLabelsRow
                                                anchors.left: parent.left
                                                anchors.top: descriptionRect.bottom
                                                anchors.topMargin: 7
                                                spacing: 5

                                                StyledText {
                                                    id: keywordsLabel
                                                    text: qsTr("Keywords:")
                                                }

                                                StyledText {
                                                    text: qsTr("(comma-separated)")
                                                    visible: rowWrapper.isHighlighted
                                                    color: Colors.defaultInputBackground
                                                }
                                            }

                                            Rectangle {
                                                id: keywordsWrapper
                                                anchors.top: keywordsLabelsRow.bottom
                                                anchors.topMargin: 3
                                                anchors.left: parent.left
                                                anchors.right: parent.right
                                                height: 80*settingsModel.keywordSizeScale
                                                border.color: Colors.artworkActiveColor
                                                border.width: flv.isFocused ? 1 : 0
                                                color: rowWrapper.isHighlighted ? Colors.defaultInputBackground : Colors.defaultControlColor

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

                                                function saveKeywords() {
                                                    artItemsModel.backupItem(rowWrapper.getIndex())
                                                }

                                                EditableTags {
                                                    id: flv
                                                    model: artItemsModel.getArtworkItself(rowWrapper.getIndex())
                                                    anchors.fill: parent
                                                    property int keywordHeight: 20 * settingsModel.keywordSizeScale + (settingsModel.keywordSizeScale - 1)*10
                                                    scrollStep: keywordHeight
                                                    stealWheel: false

                                                    delegate: KeywordWrapper {
                                                        id: kw
                                                        isHighlighted: rowWrapper.isHighlighted
                                                        delegateIndex: index
                                                        keywordText: keyword
                                                        itemHeight: flv.keywordHeight
                                                        hasSpellCheckError: !iscorrect
                                                        onActionClicked: keywordsWrapper.removeKeyword(kw.delegateIndex)
                                                        onSpellSuggestionRequested: {
                                                            artItemsModel.suggestCorrections(rowWrapper.getIndex())
                                                            Common.launchDialog("Dialogs/SpellCheckSuggestionsDialog.qml",
                                                                                applicationWindow,
                                                                                {})
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

                                                    onFocusLost: keywordsWrapper.saveKeywords()

                                                    onCopyRequest: clipboard.setText(keywordsstring)
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
                                                anchors.top: keywordsWrapper.bottom
                                                anchors.topMargin: 3
                                                spacing: 15

                                                StyledText {
                                                    text: keywordscount
                                                    color: rowWrapper.isHighlighted ? Colors.defaultControlColor : Colors.selectedArtworkColor
                                                }

                                                Item {
                                                    Layout.fillWidth: true
                                                }

                                                StyledText {
                                                    text: qsTr("Suggest keywords")
                                                    color: suggestKeywordsMA.pressed ? Colors.defaultLightColor : Colors.artworkActiveColor

                                                    MouseArea {
                                                        id: suggestKeywordsMA
                                                        anchors.fill: parent
                                                        cursorShape: Qt.PointingHandCursor
                                                        onClicked: {
                                                            var callbackObject = {
                                                                promoteKeywords: function(keywords) {
                                                                    artItemsModel.pasteKeywords(rowWrapper.getIndex(), keywords)
                                                                }
                                                            }

                                                            Common.launchDialog("Dialogs/KeywordsSuggestion.qml",
                                                                                applicationWindow,
                                                                                {callbackObject: callbackObject});
                                                        }
                                                    }
                                                }

                                                StyledText {
                                                    text: qsTr("More Edits")
                                                    color: moreEditsMA.pressed ? Colors.defaultLightColor : Colors.artworkActiveColor

                                                    MouseArea {
                                                        id: moreEditsMA
                                                        anchors.fill: parent
                                                        cursorShape: Qt.PointingHandCursor
                                                        onClicked: Common.launchItemEditing(rowWrapper.getIndex(), applicationWindow, {
                                                                                                applyCallback: function() {}
                                                                                            })
                                                    }
                                                }

                                                StyledText {
                                                    text: qsTr("Copy keywords")
                                                    color: copyKeywordsMA.pressed ? Colors.defaultLightColor : Colors.artworkActiveColor

                                                    MouseArea {
                                                        id: copyKeywordsMA
                                                        anchors.fill: parent
                                                        cursorShape: Qt.PointingHandCursor
                                                        onClicked: clipboard.setText(keywordsstring)
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }

                            Connections {
                                target: artItemsModel
                                onArtworksChanged: {
                                    console.log("ArtItemsModel: Force layout for artworks list view")
                                    imagesListView.forceLayout()
                                    imagesListView.update()
                                }
                            }

                            Connections {
                                target: filteredArtItemsModel
                                onAfterInvalidateFilter: {
                                    console.log("Filtered Model: Force layout for artworks list view")
                                    imagesListView.forceLayout()
                                    imagesListView.update()
                                }
                            }
                        }
                    }

                    Item {
                        visible: imagesListView.count == 0
                        anchors.fill: parent

                        RowLayout {
                            anchors.centerIn: parent
                            spacing: 5

                            StyledText {
                                text: qsTr("No items available.")
                                color: Colors.selectedMetadataColor
                            }

                            StyledText {
                                text: qsTr("Add files")
                                color: addFilesMA.pressed ? Colors.defaultLightColor : Colors.artworkActiveColor

                                MouseArea {
                                    id: addFilesMA
                                    anchors.fill: parent
                                    cursorShape: Qt.PointingHandCursor
                                    onClicked: chooseArtworksDialog.open()
                                }
                            }

                            StyledText {
                                text: qsTr("or")
                                color: Colors.selectedMetadataColor
                            }

                            StyledText {
                                text: qsTr("clear the filter")
                                color: clearFilterMA.pressed ? Colors.defaultLightColor : Colors.artworkActiveColor

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
                text: qsTr("Show logs")
                color: logsMA.pressed ? Colors.defaultInputBackground : Colors.selectedMetadataColor

                MouseArea {
                    id: logsMA
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        Common.launchDialog("Dialogs/LogsDialog.qml",
                                        applicationWindow,
                                        { logText: logsModel.getAllLogsText() });
                    }
                }
            }

            StyledText {
                text: "|"
                color: Colors.selectedMetadataColor
                verticalAlignment: Text.AlignVCenter
            }

            StyledText {
                text: qsTr("Check warnings")
                color: warningsMA.pressed ? Colors.defaultInputBackground : warningsManager.warningsCount > 0 ? Colors.artworkModifiedColor : Colors.selectedMetadataColor

                MouseArea {
                    id: warningsMA
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        filteredArtItemsModel.checkForWarnings()
                        Common.launchDialog("Dialogs/WarningsDialog.qml", applicationWindow, {
                                                componentParent: applicationWindow
                                            });
                    }
                }
            }

            StyledText {
                visible: applicationWindow.showUpdateLink
                text: "|"
                color: Colors.selectedMetadataColor
                verticalAlignment: Text.AlignVCenter
            }

            StyledText {
                visible: applicationWindow.showUpdateLink
                enabled: applicationWindow.showUpdateLink
                text: qsTr("Update available!")
                color: updateMA.pressed ? Colors.defaultInputBackground : Colors.greenColor

                MouseArea {
                    id: updateMA
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        Qt.openUrlExternally("http://ribtoks.github.io/xpiks/downloads/")
                    }
                }
            }

            Item {
                Layout.fillWidth: true
            }

            StyledText {
                id: filteredCountText
                text: qsTr("No items available")
                color: Colors.selectedMetadataColor
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
            }

            StyledText {
                text: "|"
                color: Colors.selectedMetadataColor
                verticalAlignment: Text.AlignVCenter
            }

            StyledText {
                text: filteredArtItemsModel.selectedArtworksCount > 1 ? qsTr("%1 selected items").arg(filteredArtItemsModel.selectedArtworksCount) : (filteredArtItemsModel.selectedArtworksCount === 1 ? qsTr("1 selected item") : qsTr("No selected items"))
                color: Colors.selectedMetadataColor
                verticalAlignment: Text.AlignVCenter

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
                color: Colors.selectedMetadataColor
                verticalAlignment: Text.AlignVCenter
            }

            StyledText {
                text: artItemsModel.modifiedArtworksCount > 1 ? qsTr("%1 modified items").arg(artItemsModel.modifiedArtworksCount) : (artItemsModel.modifiedArtworksCount === 1 ? qsTr("1 modified item") : qsTr("No modified items"))
                verticalAlignment: Text.AlignVCenter
                color: artItemsModel.modifiedArtworksCount > 0 ? Colors.artworkModifiedColor : Colors.selectedMetadataColor

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
