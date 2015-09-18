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

    onClosing: {
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

    menuBar: MenuBar {
        Menu {
            title: qsTr("File")
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

    Rectangle {
        color: Colors.defaultDarkColor
        anchors.fill: parent

        DropArea {
            enabled: applicationWindow.openedDialogsCount == 0
            anchors.fill: parent
            onDropped: {
                if (drop.hasUrls) {
                    var filesCount = artItemsModel.dropFiles(drop.urls)
                    console.log(filesCount + ' files drag&dropped')
                }
            }
        }

        RowLayout {
            id: mainGrid
            anchors.fill: parent
            spacing: 0

            ColumnLayout {
                width: 250
                Layout.fillHeight: true

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

                    ListView {
                        id: sourcesListView
                        model: artworkRepository
                        boundsBehavior: Flickable.StopAtBounds
                        anchors.fill: parent
                        anchors.margins: { left: 10; top: 5; right: 10 }

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

            Rectangle {
                height: parent.height
                width: 2
                color: Colors.defaultDarkColor
            }

            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: 0

                Rectangle {
                    height: 45
                    color: Colors.defaultDarkColor
                    z: 10000
                    Layout.fillWidth: true

                    RowLayout {
                        spacing: 10
                        anchors.fill: parent
                        anchors.margins: { top: 10; left: 10 }
                        anchors.rightMargin: mainScrollView.flickableItem.contentHeight > mainScrollView.flickableItem.height ? 20 : 10

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
                            width: mainScrollView.flickableItem.contentHeight > mainScrollView.flickableItem.height ? 72 : 82
                            enabled: artworkRepository.artworksSourcesCount > 0
                            onClicked: {
                                if (filteredArtItemsModel.selectedArtworksCount === 0) {
                                    mustSelectDialog.open()
                                }
                                else {
                                    if (filteredArtItemsModel.selectedArtworksCount > 0) {
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
                    Layout.fillWidth: true
                    color: Colors.itemsSourceSelected
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: Colors.defaultControlColor

                    Item {
                        anchors.topMargin: 10
                        anchors.fill: parent

                        Rectangle {
                            id: filterRect
                            color: "transparent"
                            anchors.top: parent.top
                            anchors.left: parent.left
                            width: parent.width
                            height: 30

                            RowLayout {
                                anchors.fill: parent
                                anchors.rightMargin: mainScrollView.flickableItem.contentHeight > mainScrollView.flickableItem.height ? 10 : 0
                                spacing: 10

                                Item {
                                    width: 10
                                }

                                StyledCheckbox {
                                    id: selectAllCheckbox
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

                                Item {
                                    Layout.fillWidth: true
                                }

                                Rectangle {
                                    color: Colors.defaultDarkColor
                                    width: 262
                                    height: 24
                                    enabled: artworkRepository.artworksSourcesCount > 0

                                    StyledTextInput {
                                        id: filterText
                                        width: 230
                                        height: 24
                                        clip: true
                                        anchors.left: parent.left
                                        anchors.leftMargin: 5
                                        color: Colors.defaultInputBackground
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
                                        onItemClicked: filteredArtItemsModel.searchTerm = ''
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
                                    width: 90
                                    text: qsTr("Search")
                                    enabled: artworkRepository.artworksSourcesCount > 0
                                    onClicked: filteredArtItemsModel.searchTerm = filterText.text
                                }

                                Item {
                                    width: 20
                                }
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
                                        }
                                    }
                                }

                                StyledText {
                                    text: qsTr("Dismiss (%1)").arg(10 - (autoDismissTimer.iterations % 11))
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

                                        if (iterations % 11 === 10) {
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
                            width: parent.width
                            anchors.topMargin: 4
                            anchors.top: undoRedoRect.bottom
                            anchors.bottom: parent.bottom
                            //Layout.fillWidth: true
                            //Layout.fillHeight: true
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
                                    property bool isHighlighted: (isselected || descriptionTextInput.activeFocus || flv.isFocused)
                                    color: isHighlighted ? Colors.selectedArtworkColor : Colors.artworkImageBackground
                                    property variant artworkModel: model

                                    function getIndex() {
                                        return filteredArtItemsModel.getOriginalIndex(index)
                                    }

                                    width: parent.width
                                    height: 200

                                    RowLayout {
                                        anchors.fill: parent
                                        anchors.rightMargin: 10
                                        spacing: 5

                                        Rectangle {
                                            id: isModifiedRectangle
                                            color: ismodified ? Colors.artworkModifiedColor : Colors.artworkSavedColor
                                            width: 6
                                            Layout.fillHeight: true
                                        }

                                        Item {
                                            width: 5
                                        }

                                        StyledCheckbox {
                                            id: itemCheckedCheckbox
                                            //checked: isselected
                                            onClicked: editisselected = checked
                                            Component.onCompleted: itemCheckedCheckbox.checked = isselected
                                            Connections {
                                                target: filteredArtItemsModel
                                                onSelectedArtworksCountChanged: {
                                                    itemCheckedCheckbox.checked = rowWrapper.artworkModel.isselected || false
                                                }
                                            }
                                        }

                                        Rectangle {
                                            width: 180
                                            height: parent.height
                                            color: "transparent"

                                            ColumnLayout {
                                                anchors.fill: parent
                                                anchors.margins: { left: 15; right: 15 }
                                                spacing: 5

                                                Item {
                                                    height: 15
                                                }

                                                Rectangle {
                                                    width: 150
                                                    height: 130
                                                    anchors.horizontalCenter: parent.horizontalCenter
                                                    color: "transparent"

                                                    Image {
                                                        id: artworkImage
                                                        anchors.fill: parent
                                                        source: "image://global/" + filename
                                                        sourceSize.width: 150
                                                        sourceSize.height: 150
                                                        fillMode: Image.PreserveAspectCrop
                                                        asynchronous: true
                                                    }

                                                    MouseArea {
                                                        anchors.fill: parent
                                                        onClicked: editisselected = !isselected
                                                        onDoubleClicked: {
                                                            combinedArtworks.resetModelData();
                                                            artItemsModel.combineArtwork(rowWrapper.getIndex());
                                                            Common.launchDialog("Dialogs/CombinedArtworksDialog.qml", applicationWindow, {componentParent: applicationWindow});
                                                        }
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

                                                        onPressed: {
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
                                            height: parent.height
                                            Layout.fillWidth: true
                                            color: rowWrapper.isHighlighted  ? Colors.selectedMetadataColor : Colors.artworkBackground

                                            ColumnLayout {
                                                id: columnLayout
                                                spacing: 3
                                                anchors.fill: parent
                                                anchors.margins: { left: 20; right: 20 }

                                                StyledText {
                                                    text: qsTr("Description:")
                                                    anchors.left: parent.left
                                                }

                                                Rectangle {
                                                    id: rect
                                                    Layout.fillWidth: true
                                                    height: 30
                                                    color: rowWrapper.isHighlighted ? Colors.defaultInputBackground : Colors.defaultControlColor
                                                    border.color: Colors.artworkActiveColor
                                                    border.width: descriptionTextInput.activeFocus ? 1 : 0

                                                    StyledTextInput {
                                                        id: descriptionTextInput
                                                        height: 30
                                                        anchors.left: parent.left
                                                        anchors.right: parent.right
                                                        anchors.leftMargin: 5
                                                        anchors.rightMargin: 5

                                                        anchors.verticalCenter: parent.verticalCenter
                                                        maximumLength: 250
                                                        text: description
                                                        color: rowWrapper.isHighlighted ? Colors.defaultLightColor : Colors.defaultInputBackground
                                                        onTextChanged: model.editdescription = text
                                                        Keys.onTabPressed: {
                                                            flv.activateEdit()
                                                        }

                                                        validator: RegExpValidator {
                                                            regExp: /[a-zA-Z0-9 !@#$%^&*()+="'|-,]*/
                                                        }

                                                        Keys.onPressed: {
                                                            if(event.matches(StandardKey.Paste)) {
                                                                var clipboardText = clipboard.getText();
                                                                // same regexp as in validator
                                                                var sanitizedText = clipboardText.replace(/[^a-zA-Z0-9 !@#$%^&*()+="'|-,]*/g, '');
                                                                descriptionTextInput.paste(sanitizedText)
                                                                event.accepted = true
                                                            }
                                                        }
                                                    }
                                                }

                                                Item {
                                                    height: 1
                                                }

                                                RowLayout {
                                                    anchors.left: parent.left
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
                                                    Layout.fillWidth: true
                                                    height: 80
                                                    anchors.rightMargin: 20
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
                                                        console.log("Saving keywords...")
                                                        artItemsModel.backupItem(rowWrapper.getIndex())
                                                    }

                                                    MouseArea {
                                                        anchors.fill: parent
                                                        propagateComposedEvents: true
                                                        onClicked: {
                                                            flv.activateEdit()
                                                            mouse.accepted = false
                                                        }
                                                    }

                                                    StyledScrollView {
                                                        id: scroller
                                                        height: parent.height
                                                        width: parent.width + 15
                                                        highlightOnFocus: true

                                                        EditableTags {
                                                            id: flv
                                                            model: artItemsModel.getArtworkItself(rowWrapper.getIndex())
                                                            anchors.margins: { left: 5; top: 5; right: 0; bottom: 5 }

                                                            delegate: Rectangle {
                                                                id: itemWrapper
                                                                property int delegateIndex: index
                                                                color: rowWrapper.isHighlighted ? Colors.defaultLightColor : Colors.selectedArtworkColor

                                                                width: childrenRect.width
                                                                height: childrenRect.height

                                                                Row {
                                                                    spacing: 1 + (settingsModel.keywordSizeScale - 1) * 10

                                                                    Rectangle {
                                                                        id: tagTextRect
                                                                        width: childrenRect.width + 5
                                                                        height: 20 * settingsModel.keywordSizeScale + (settingsModel.keywordSizeScale - 1)*10
                                                                        color: "transparent"

                                                                        StyledText {
                                                                            anchors.left: parent.left
                                                                            anchors.leftMargin: 5 + (settingsModel.keywordSizeScale - 1)*10
                                                                            anchors.top: parent.top
                                                                            anchors.bottom: parent.bottom
                                                                            verticalAlignment: Text.AlignVCenter
                                                                            text: keyword
                                                                            color: rowWrapper.isHighlighted ? Colors.defaultControlColor : Colors.defaultLightColor
                                                                            font.pixelSize: 12 * settingsModel.keywordSizeScale
                                                                        }
                                                                    }

                                                                    CloseIcon {
                                                                        width: 14*settingsModel.keywordSizeScale
                                                                        height: 14*settingsModel.keywordSizeScale
                                                                        isActive: rowWrapper.isHighlighted
                                                                        anchors.verticalCenter: tagTextRect.verticalCenter
                                                                        anchors.verticalCenterOffset: -1
                                                                        onItemClicked: keywordsWrapper.removeKeyword(itemWrapper.delegateIndex)
                                                                    }

                                                                    Item {
                                                                        width: 1*settingsModel.keywordSizeScale
                                                                        height: 20
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

                                                            onFocusLost: keywordsWrapper.saveKeywords()
                                                        }
                                                    }
                                                }

                                                RowLayout {
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
                                                            onClicked: {
                                                                combinedArtworks.resetModelData();
                                                                artItemsModel.combineArtwork(rowWrapper.getIndex());
                                                                Common.launchDialog("Dialogs/CombinedArtworksDialog.qml", applicationWindow, {componentParent: applicationWindow});
                                                            }
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

                                                Item {
                                                    height: 1
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
                    }
                }

                ClipboardHelper {
                    id: clipboard
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
                        Common.launchDialog("Dialogs/WarningsDialog.qml", applicationWindow, {});
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
