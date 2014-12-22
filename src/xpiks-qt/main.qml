/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014 Taras Kushnir <kushnirTV@gmail.com>
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

import QtQuick 2.4
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.1
import xpiks 1.0
import "Colors.js" as Colors;

ApplicationWindow {
    id: applicationWindow
    visible: true
    width: 900
    height: 640
    minimumHeight: 640
    minimumWidth: 800
    title: qsTr("Xpiks")

    onClosing: {
        if (artItemsModel.modifiedArtworksCount > 0) {
            close.accepted = false
            configExitDialog.open()
        }
    }

    menuBar: MenuBar {
        Menu {
            title: qsTr("File")
            MenuItem {
                text: qsTr("&Settings")
                onTriggered: {
                    console.log("Settings action triggered");
                    var component = Qt.createComponent("SettingsWindow.qml");
                    var window = component.createObject(applicationWindow);
                    window.show();
                }
            }
            MenuItem {
                text: qsTr("Exit")
                onTriggered: Qt.quit();
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
            artItemsModel.removeSelectedArtworks()
        }
    }

    MessageDialog {
        id: confirmRemoveDirectoryDialog
        property int directoryIndex
        title: "Confirmation"
        text: qsTr("Are you sure you want to remove this directory?")
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            artItemsModel.removeArtworksDirectory(directoryIndex)
        }
    }

    FileDialog {
        id: chooseArtworksDialog
        title: "Please choose artworks"
        selectExisting: true
        selectMultiple: true
        nameFilters: [ "Jpeg images (*.jpg), All files (*)" ]

        onAccepted: {
            console.log("You chose: " + chooseArtworksDialog.fileUrls)
            artItemsModel.addLocalArtworks(chooseArtworksDialog.fileUrls)
            Qt.createComponent("ImportMetadata.qml").createObject(applicationWindow, {})
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
            console.log("You chose: " + chooseDirectoryDialog.fileUrl)
            artItemsModel.addLocalDirectory(chooseDirectoryDialog.fileUrl)
            Qt.createComponent("ImportMetadata.qml").createObject(applicationWindow, {})
        }

        onRejected: {
            console.log("Directory dialog canceled")
        }
    }

    MessageDialog {
        id: mustSaveWarning
        title: "Warning"
        text: "Please, save selected items before upload"
    }

    Rectangle {
        color: Colors.appBackground
        anchors.fill: parent

        SplitView {
            id: mainGrid
            anchors.fill: parent
            orientation: Qt.Horizontal

            handleDelegate: Rectangle {
                height: parent.height
                width: 2
                color: Colors.appBackground
            }

            ColumnLayout {
                Layout.minimumWidth: 250
                Layout.preferredWidth: 250
                Layout.maximumWidth: 350

                spacing: 0

                Rectangle {
                    width: parent.width
                    height: 45
                    color: Colors.appBackground

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
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    Layout.minimumWidth: 250
                    Layout.maximumWidth: 350

                    color: Colors.buttonBackground

                    ListView {
                        id: sourcesListView
                        model: artworkRepository
                        boundsBehavior: Flickable.StopAtBounds
                        anchors.fill: parent
                        anchors.margins: { left: 10; top: 5; right: 10 }

                        spacing: 10

                        delegate: Rectangle {
                            id: wrapperRect
                            property int indexOfThisDelegate: index
                            color: Colors.itemsSourceBackground
                            width: parent.width
                            height: 31
                            Layout.minimumWidth: 237

                            RowLayout {
                                spacing: 10
                                anchors.fill: parent

                                Item {
                                    id: placeholder1
                                    width: 1
                                }

                                Text {
                                    id: directoryPath
                                    Layout.fillWidth: true
                                    anchors.verticalCenter: parent.verticalCenter
                                    height: 31
                                    color: Colors.itemsSourceForeground
                                    text: path + "(" + usedimagescount + ")"
                                    elide: Text.ElideMiddle
                                }

                                Item {
                                    width: 14
                                    height: 14

                                    Rectangle {
                                        anchors.fill: parent
                                        radius: 7
                                        border.width: 1
                                        color: "#dddddd"
                                        border.color: "black"

                                        Rectangle {
                                            width: parent.width - 4
                                            anchors.centerIn: parent
                                            height: 2
                                            radius: 1
                                            border.color: "black"
                                            border.width: 1
                                        }
                                    }

                                    opacity: removeDirectoryMouseArea.containsMouse ? 1 : 0.5
                                    scale: removeDirectoryMouseArea.pressed ? 0.8 : 1

                                    MouseArea {
                                        id: removeDirectoryMouseArea
                                        anchors.fill: parent
                                        hoverEnabled: true

                                        onClicked: {
                                            confirmRemoveDirectoryDialog.directoryIndex = wrapperRect.indexOfThisDelegate
                                            confirmRemoveDirectoryDialog.open()

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

            ColumnLayout {
                spacing: 0

                Rectangle {
                    height: 45
                    color: Colors.appBackground
                    z: 10000
                    Layout.fillWidth: true

                    RowLayout {
                        spacing: 10
                        anchors.fill: parent
                        anchors.margins: { top: 10; left: 10; right: 10 }

                        Item {
                            width: 3
                        }

                        StyledCheckbox {
                            id: selectAllCheckbox
                            checked: artItemsModel.selectedArtworksCount > 0

                            onClicked: {
                                if (checked) {
                                    artItemsModel.selectAllArtworks();
                                }
                                else {
                                    artItemsModel.unselectAllArtworks();
                                }
                            }

                            Connections {
                                target: artItemsModel
                                onSelectedArtworksCountChanged: {
                                    selectAllCheckbox.checked = artItemsModel.selectedArtworksCount > 0
                                }
                            }
                        }

                        Item {
                            Layout.fillWidth: true
                        }

                        StyledButton {
                            text: qsTr("Remove")
                            width: 105
                            onClicked: {
                                var itemsCount = artItemsModel.selectedArtworksCount
                                if (itemsCount > 0) {
                                    confirmRemoveSelectedDialog.itemsCount = itemsCount
                                    confirmRemoveSelectedDialog.open()
                                }
                            }
                        }

                        StyledButton {
                            text: qsTr("Edit")
                            width: 90
                            onClicked: {
                                if (artItemsModel.selectedArtworksCount > 0) {
                                    combinedArtworks.resetModelData();
                                    artItemsModel.combineSelectedArtworks();
                                    Qt.createComponent("CombinedArtworksDialog.qml").createObject(applicationWindow, {});
                                }
                            }
                        }

                        StyledButton {
                            text: qsTr("Save")
                            width: 80
                            onClicked: {
                                if (artItemsModel.selectedArtworksCount > 0) {
                                    iptcProvider.resetModel()
                                    artItemsModel.patchSelectedArtworks()
                                    Qt.createComponent("ExportMetadata.qml").createObject(applicationWindow, {})
                                }
                            }
                        }

                        StyledButton {
                            text: qsTr("Upload")
                            width: 90
                            onClicked: {
                                if (artItemsModel.areSelectedArtworksSaved()) {
                                    artworkUploader.resetModel()
                                    artItemsModel.uploadSelectedArtworks()
                                    Qt.createComponent("UploadArtworks.qml").createObject(applicationWindow, {})
                                } else {
                                    mustSaveWarning.open()
                                }
                            }
                        }
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: Colors.buttonBackground

                    ListView {
                        id: imagesListView
                        anchors.fill: parent
                        anchors.topMargin: 5
                        model: artItemsModel
                        boundsBehavior: Flickable.StopAtBounds
                        spacing: 4

                        delegate: Rectangle {
                            id: wrapperRectangle
                            color: isselected ? Colors.selectedArtworkColor : Colors.artworkImageBackground
                            property int indexOfThisDelegate: index

                            width: parent.width
                            height: 180

                            RowLayout {
                                anchors.fill: parent
                                anchors.rightMargin: 10
                                spacing: 5

                                Rectangle {
                                    id: isModifiedRectangle
                                    color: ismodified ? Colors.artworkModifiedColor : Colors.artworkNormalColor
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
                                        target: artItemsModel
                                        onSelectedArtworksCountChanged: {
                                            itemCheckedCheckbox.checked = isselected
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
                                            height: 8
                                        }

                                        Rectangle {
                                            width: 150
                                            Layout.minimumHeight: 100
                                            Layout.maximumHeight: 150
                                            anchors.horizontalCenter: parent.horizontalCenter
                                            color: "transparent"
                                            Image {
                                                anchors.fill: parent
                                                source: "image://global/" + filename
                                                sourceSize.width: 150
                                                sourceSize.height: 150
                                                fillMode: Image.PreserveAspectFit
                                                asynchronous: true
                                            }
                                        }

                                        Item {
                                            height: 5
                                        }

                                        Text {
                                            Layout.fillWidth: true
                                            elide: Text.ElideMiddle
                                            horizontalAlignment: Text.AlignHCenter
                                            text: filename.split(/[\\/]/).pop()
                                            renderType: Text.NativeRendering
                                            color: Colors.imageFilenameForeground
                                            font.family: "Helvetica"
                                            font.pixelSize: 12
                                        }

                                        Item {
                                            Layout.fillHeight: true
                                        }
                                    }
                                }

                                Rectangle {
                                    height: parent.height
                                    Layout.fillWidth: true
                                    color: isselected ? Colors.selectedMetadataColor : Colors.artworkBackground

                                    ColumnLayout {
                                        id: columnLayout
                                        spacing: 3
                                        anchors.fill: parent
                                        anchors.margins: { left: 20; right: 10 }

                                        Text {
                                            text: qsTr("Description:")
                                            anchors.left: parent.left
                                            renderType: Text.NativeRendering
                                            color: Colors.imageFilenameForeground
                                            font.family: "Helvetica"
                                            font.pixelSize: 13
                                        }

                                        Rectangle {
                                            id: rect
                                            Layout.fillWidth: true
                                            height: 30
                                            color: Colors.defaultInputBackground

                                            TextInput {
                                                anchors.left: parent.left
                                                anchors.right: parent.right
                                                anchors.leftMargin: 5
                                                anchors.rightMargin: 5
                                                anchors.verticalCenter: parent.verticalCenter
                                                maximumLength: 250
                                                clip: true
                                                text: description
                                                color: Colors.imageFilenameForeground
                                                font.family: "Helvetica"
                                                font.pixelSize: 12
                                                onTextChanged: model.editdescription = text
                                                renderType: TextInput.NativeRendering
                                                Keys.onTabPressed: {
                                                    flv.activateEdit()
                                                }
                                            }
                                        }

                                        RowLayout {
                                            Layout.fillWidth: true

                                            Text {
                                                id: keywordsLabel
                                                anchors.left: parent.left
                                                text: qsTr("Keywords:")
                                                color: Colors.imageFilenameForeground
                                                font.family: "Helvetica"
                                                font.pixelSize: 12
                                            }

                                            Item {
                                                Layout.fillWidth: true
                                            }

                                            Text {
                                                text: "<a href=\"#\">" + qsTr("Copy keywords") + "</a>"
                                                linkColor: "blue"

                                                MouseArea {
                                                    anchors.fill: parent
                                                    cursorShape: Qt.PointingHandCursor
                                                    onClicked: clipboard.setText(keywordsstring)
                                                }
                                            }
                                        }

                                        Rectangle {
                                            id: keywordsWrapper
                                            color: Colors.defaultInputBackground
                                            Layout.fillWidth: true
                                            height: 80

                                            function removeKeyword(index) {
                                                artItemsModel.removeKeywordAt(wrapperRectangle.indexOfThisDelegate, index)
                                            }

                                            function removeLastKeyword() {
                                                artItemsModel.removeLastKeyword(wrapperRectangle.indexOfThisDelegate)
                                            }

                                            function appendKeyword(keyword) {
                                                artItemsModel.appendKeyword(wrapperRectangle.indexOfThisDelegate, keyword)
                                            }

                                            MouseArea {
                                                anchors.fill: parent
                                                propagateComposedEvents: true
                                                onClicked: {
                                                    flv.activateEdit()
                                                    mouse.accepted = false
                                                }
                                            }

                                            ScrollView {
                                                id: scroller
                                                anchors.fill: parent
                                                highlightOnFocus: true


                                                EditableTags {
                                                    id: flv
                                                    anchors.margins: 5
                                                    model: keywords

                                                    delegate: Rectangle {
                                                        id: itemWrapper
                                                        property int indexOfThisDelegate: index
                                                        property string keyword: modelData
                                                        border.width: 1
                                                        border.color: Colors.artworkActiveColor
                                                        color: "transparent"

                                                        width: childrenRect.width
                                                        height: childrenRect.height

                                                        RowLayout {
                                                            Rectangle {
                                                                color: "transparent"
                                                                width: childrenRect.width + 15
                                                                height: 30

                                                                Text {
                                                                    anchors.left: parent.left
                                                                    anchors.leftMargin: 10
                                                                    anchors.top: parent.top
                                                                    anchors.bottom: parent.bottom
                                                                    verticalAlignment: Text.AlignVCenter
                                                                    text: modelData
                                                                    renderType: Text.NativeRendering
                                                                }
                                                            }

                                                            CloseIcon {
                                                                onItemClicked: keywordsWrapper.removeKeyword(itemWrapper.indexOfThisDelegate)
                                                            }

                                                            Item {
                                                                width: 5
                                                            }
                                                        }
                                                    }

                                                    onTagAdded: {
                                                        keywordsWrapper.appendKeyword(text)
                                                    }

                                                    onRemoveLast: {
                                                        keywordsWrapper.removeLastKeyword()
                                                    }
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
            implicitHeight: 13
            spacing: 5

            Item {
                Layout.fillWidth: true
            }

            Text {
                text: qsTr("(%1) item(s) selected").arg(artItemsModel.selectedArtworksCount)
            }

            Text {
                text: "|"
            }

            Text {
                text: qsTr("(%1) item(s) modified").arg(artItemsModel.modifiedArtworksCount)
            }
        }

        style: StatusBarStyle {
            background: Rectangle {
                color: Colors.appBackground
                anchors.fill: parent
            }
        }
    }
}
