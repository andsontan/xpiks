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
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Styles 1.3
import xpiks 1.0
import "Colors.js" as Colors

Item {
    id: dialogComponent
    anchors.fill: parent

    function closePopup() {
        dialogComponent.destroy()
    }

    PropertyAnimation { target: dialogComponent; property: "opacity";
        duration: 400; from: 0; to: 1;
        easing.type: Easing.InOutQuad ; running: true }

    MessageDialog {
        id: confirmRemoveArtworksDialog
        property int itemsCount
        title: "Confirmation"
        text: qsTr("Are you sure you want to remove %1 item(s)?").arg(itemsCount)
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            combinedArtworks.removeSelectedArtworks()
            if (combinedArtworks.getArtworksCount() === 0) {
                closePopup()
            }
        }
    }

    // This rectange is the a overlay to partially show the parent through it
    // and clicking outside of the 'dialog' popup will do 'nothing'
    Rectangle {
        anchors.fill: parent
        id: overlay
        color: "#000000"
        opacity: 0.6
        // add a mouse area so that clicks outside
        // the dialog window will not do anything
        MouseArea {
            anchors.fill: parent
        }
    }


    FocusScope {
        anchors.fill: parent

        // This rectangle is the actual popup
        Rectangle {
            id: dialogWindow
            width: 730
            height: 530
            color: Colors.selectedArtworkColor
            anchors.centerIn: parent

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 30
                spacing: 3

                RowLayout {
                    spacing: 5
                    Layout.fillWidth: true
                    height: 40

                    Text {
                        text: qsTr("Select to remove")
                        color: Colors.defaultInputBackground
                        font.family: "Helvetica"
                        font.pixelSize: 12
                        renderType: Text.NativeRendering
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    StyledButton {
                        text: qsTr("Remove")
                        width: 100
                        enabled: combinedArtworks.selectedArtworksCount > 0
                        onClicked: {
                            confirmRemoveArtworksDialog.itemsCount = combinedArtworks.selectedArtworksCount
                            confirmRemoveArtworksDialog.open()
                        }
                    }
                }

                Item {
                    height: 1
                }

                Rectangle {
                    height: 130
                    width: parent.width
                    color: Colors.defaultControlColor

                    StyledScrollView {
                        id: imagesScrollView
                        height: parent.height + 15
                        width: parent.width
                        anchors.margins: 10

                        ListView {
                            boundsBehavior: Flickable.StopAtBounds
                            anchors.fill: parent
                            anchors.margins: 10
                            orientation: Qt.Horizontal
                            spacing: 10
                            model: combinedArtworks

                            delegate: Rectangle {
                                property int indexOfThisDelegate: index
                                id: imageWrapper
                                height: 110
                                width: height
                                color: "transparent"

                                Image {
                                    anchors.fill: parent
                                    anchors.margins: 1
                                    source: "image://global/" + path
                                    sourceSize.width: 150
                                    sourceSize.height: 150
                                    fillMode: Image.PreserveAspectFit
                                    asynchronous: true
                                }

                                Rectangle {
                                    anchors.fill: parent
                                    color: Colors.defaultControlColor
                                    opacity: mouseArea.containsMouse ? 0.4 : (isselected ? 0.7 : 0)
                                }

                                LargeRemoveIcon {
                                    visible: isselected
                                    width: parent.width
                                    height: parent.height
                                }

                                MouseArea {
                                    id: mouseArea
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    onClicked: {
                                        if (isselected) {
                                            combinedArtworks.deselectArtwork(indexOfThisDelegate)
                                        } else {
                                            combinedArtworks.selectArtwork(indexOfThisDelegate)
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                Item {
                    height: 30
                }

                Text {
                    text: qsTr("Title:")
                    anchors.left: parent.left
                    color: Colors.defaultLightColor
                    font.family: "Helvetica"
                    font.pixelSize: 12
                    renderType: Text.NativeRendering
                }

                Rectangle {
                    id: anotherRect
                    width: 300
                    height: 25
                    anchors.left: parent.left
                    color: Colors.defaultInputBackground
                    border.color: Colors.artworkActiveColor
                    border.width: titleTextInput.activeFocus ? 1 : 0

                    TextInput {
                        id: titleTextInput
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.leftMargin: 5
                        anchors.rightMargin: 5
                        anchors.verticalCenter: parent.verticalCenter
                        maximumLength: 250
                        clip: true
                        focus: true
                        color: Colors.defaultLightColor
                        text: combinedArtworks.title
                        onTextChanged: combinedArtworks.title = text
                        renderType: TextInput.NativeRendering
                        KeyNavigation.tab: descriptionTextInput
                        KeyNavigation.priority: KeyNavigation.BeforeItem
                    }
                }

                Item {
                    height: 5
                }

                Text {
                    text: qsTr("Description:")
                    anchors.left: parent.left
                    color: Colors.defaultLightColor
                    font.family: "Helvetica"
                    font.pixelSize: 12
                    renderType: Text.NativeRendering
                }

                Rectangle {
                    id: rect
                    width: 300
                    height: 25
                    color: Colors.defaultInputBackground
                    border.color: Colors.artworkActiveColor
                    border.width: descriptionTextInput.activeFocus ? 1 : 0
                    anchors.left: parent.left

                    TextInput {
                        id: descriptionTextInput
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.leftMargin: 5
                        anchors.rightMargin: 5
                        anchors.verticalCenter: parent.verticalCenter
                        maximumLength: 250
                        clip: true
                        color: Colors.defaultLightColor
                        text: combinedArtworks.description
                        onTextChanged: combinedArtworks.description = text
                        renderType: TextInput.NativeRendering
                        Keys.onTabPressed: {
                            flv.activateEdit()
                        }
                        KeyNavigation.backtab: titleTextInput
                        KeyNavigation.priority: KeyNavigation.BeforeItem
                    }
                }

                Item {
                    height: 5
                }

                RowLayout {
                    anchors.left: parent.left
                    spacing: 5

                    Text {
                        id: keywordsLabel
                        text: qsTr("Keywords:")
                        color: Colors.defaultLightColor
                        font.family: "Helvetica"
                        font.pixelSize: 12
                        renderType: Text.NativeRendering
                    }

                    Text {
                        text: qsTr("(comma-separated)")
                        color: Colors.defaultInputBackground
                        font.family: "Helvetica"
                        font.pixelSize: 12
                        renderType: Text.NativeRendering
                    }
                }

                Rectangle {
                    id: keywordsWrapper
                    border.color: Colors.artworkActiveColor
                    border.width: flv.isFocused ? 1 : 0
                    height: 100
                    anchors.rightMargin: 20
                    Layout.fillWidth: true
                    color: Colors.defaultInputBackground

                    function removeKeyword(index) {
                        combinedArtworks.removeKeywordAt(index)
                    }

                    function removeLastKeyword() {
                        combinedArtworks.removeLastKeyword()
                    }

                    function appendKeyword(keyword) {
                        combinedArtworks.appendKeyword(keyword)
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
                        width: parent.width + 15
                        height: parent.height
                        highlightOnFocus: true

                        EditableTags {
                            id: flv
                            anchors.margins: { left: 5; top: 5; right: 0; bottom: 5 }
                            model: combinedArtworks.keywords

                            delegate: Rectangle {
                                id: itemWrapper
                                property int indexOfThisDelegate: index
                                property string keyword: modelData
                                color: Colors.defaultLightColor

                                width: childrenRect.width
                                height: childrenRect.height

                                RowLayout {
                                    spacing: 1

                                    Rectangle {
                                        id: tagTextRect
                                        width: childrenRect.width + 5
                                        height: 20
                                        color: "transparent"

                                        Text {
                                            anchors.left: parent.left
                                            anchors.leftMargin: 5
                                            anchors.top: parent.top
                                            anchors.bottom: parent.bottom
                                            verticalAlignment: Text.AlignVCenter
                                            text: modelData
                                            renderType: Text.NativeRendering
                                            color: Colors.defaultControlColor
                                        }
                                    }

                                    CloseIcon {
                                        width: 14
                                        height: 14
                                        isActive: true
                                        anchors.verticalCenter: tagTextRect.verticalCenter
                                        anchors.verticalCenterOffset: 1
                                        onItemClicked: keywordsWrapper.removeKeyword(itemWrapper.indexOfThisDelegate)
                                    }

                                    Item {
                                        width: 1
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

                RowLayout {
                    Text {
                        text: combinedArtworks.keywordsCount
                        font.family: "Helvetica"
                        font.pixelSize: 12
                        renderType: TextInput.NativeRendering
                        color: Colors.defaultControlColor
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    Text {
                        text: qsTr("Copy keywords")
                        color: Colors.artworkActiveColor
                        renderType: TextInput.NativeRendering
                        font.family: "Helvetica"
                        font.pixelSize: 12

                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: combinedClipboard.setText(combinedArtworks.getKeywordsString())
                        }
                    }

                    ClipboardHelper {
                        id: combinedClipboard
                    }
                }

                Item {
                    height: 5
                }

                Rectangle {
                    width: parent.width
                    color: "transparent"
                    height: 24

                    RowLayout {
                        anchors.fill: parent
                        spacing: 10

                        Item {
                            Layout.fillWidth: true
                        }

                        StyledButton {
                            width: 130
                            text: qsTr("Save and append")
                            onClicked: {
                                combinedArtworks.saveAddKeywords()
                                closePopup()
                                artItemsModel.updateSelectedArtworks()
                            }

                            tooltip: "Sets description and appends keywords in edited files"
                        }

                        StyledButton {
                            text: qsTr("Save and replace")
                            width: 130
                            onClicked: {
                                combinedArtworks.saveSetKeywords()
                                closePopup()
                                artItemsModel.updateSelectedArtworks()
                            }

                            tooltip: "Sets description and replaces keywords in edited files"
                        }

                        StyledButton {
                            text: qsTr("Cancel")
                            width: 100
                            onClicked: {
                                closePopup()
                            }

                            tooltip: "Exits with no changes"
                        }
                    }
                }
            }
        }
    }

    Component.onCompleted: titleTextInput.forceActiveFocus()
}
