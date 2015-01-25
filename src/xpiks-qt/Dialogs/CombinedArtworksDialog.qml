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
import "../Constants"
import "../Constants/Colors.js" as Colors;
import "../Components"
import "../StyledControls"

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

        MouseArea {
            anchors.fill: parent
            onWheel: wheel.accepted = true
            onClicked: mouse.accepted = true

            property real old_x : 0
            property real old_y : 0

            onPressed:{
                //            var tmp = root.mapToItem(img,mouse.x,mouse.y);
                var tmp = mapToItem(dialogComponent, mouse.x, mouse.y);
                old_x = tmp.x;
                old_y = tmp.y;
            }

            onPositionChanged: {
                //            var tmp = root.mapToItem(img,mouse.x,mouse.y);
                var tmp = mapToItem(dialogComponent, mouse.x, mouse.y);
                var delta_x = tmp.x - old_x;
                var delta_y = tmp.y - old_y;
                dialogWindow.x += delta_x;
                dialogWindow.y += delta_y;
                old_x = tmp.x;
                old_y = tmp.y;
            }
        }

        // This rectangle is the actual popup
        Rectangle {
            id: dialogWindow
            width: 730
            height: 530
            color: Colors.selectedArtworkColor
            anchors.centerIn: parent
            Component.onCompleted: anchors.centerIn = undefined

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 30
                spacing: 3

                RowLayout {
                    spacing: 5
                    Layout.fillWidth: true
                    height: 40

                    StyledText {
                        height: parent.height
                        text: qsTr("Select to remove")
                        verticalAlignment: Text.AlignBottom
                        color: Colors.defaultInputBackground
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
                                    fillMode: Image.PreserveAspectCrop
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

                RowLayout {
                    width: parent.width
                    spacing: 20

                    ColumnLayout {
                        spacing: 3
                        width: 325

                        RowLayout {
                            spacing: 5

                            StyledText {
                                text: qsTr("Title:")
                            }

                            StyledText {
                                text: qsTr("(same as Description if empty)")
                                color: Colors.defaultInputBackground
                            }
                        }

                        Rectangle {
                            id: anotherRect
                            anchors.left: parent.left
                            anchors.right: parent.right
                            height: 25
                            color: Colors.defaultInputBackground
                            border.color: Colors.artworkActiveColor
                            border.width: titleTextInput.activeFocus ? 1 : 0

                            StyledTextInput {
                                id: titleTextInput
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.leftMargin: 5
                                anchors.rightMargin: 5
                                anchors.verticalCenter: parent.verticalCenter
                                maximumLength: 200
                                focus: true
                                text: combinedArtworks.title
                                onTextChanged: combinedArtworks.title = text
                                KeyNavigation.tab: authorTextInput
                            }
                        }
                    }

                    ColumnLayout {
                        spacing: 3
                        width: 325

                        StyledText {
                            text: qsTr("Author:")
                            anchors.left: parent.left
                        }

                        Rectangle {
                            id: yetAnotherRect
                            anchors.left: parent.left
                            anchors.right: parent.right
                            height: 25
                            color: Colors.defaultInputBackground
                            border.color: Colors.artworkActiveColor
                            border.width: authorTextInput.activeFocus ? 1 : 0

                            StyledTextInput {
                                id: authorTextInput
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.leftMargin: 5
                                anchors.rightMargin: 5
                                anchors.verticalCenter: parent.verticalCenter
                                maximumLength: 200
                                focus: true
                                text: combinedArtworks.author
                                onTextChanged: combinedArtworks.author = text
                                KeyNavigation.backtab: titleTextInput
                                KeyNavigation.tab: descriptionTextInput
                            }
                        }
                    }
                }

                Item {
                    height: 5
                }

                StyledText {
                    text: qsTr("Description:")
                    anchors.left: parent.left
                }

                Rectangle {
                    id: rect
                    width: parent.width
                    height: 25
                    color: Colors.defaultInputBackground
                    border.color: Colors.artworkActiveColor
                    border.width: descriptionTextInput.activeFocus ? 1 : 0
                    anchors.left: parent.left

                    StyledTextInput {
                        id: descriptionTextInput
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.leftMargin: 5
                        anchors.rightMargin: 5
                        anchors.verticalCenter: parent.verticalCenter
                        maximumLength: 250
                        text: combinedArtworks.description
                        onTextChanged: combinedArtworks.description = text
                        Keys.onTabPressed: {
                            flv.activateEdit()
                        }
                        KeyNavigation.backtab: authorTextInput
                    }
                }

                Item {
                    height: 5
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
                        color: Colors.defaultInputBackground
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
                            model: combinedArtworks.getKeywordsModel()

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

                                        StyledText {
                                            anchors.left: parent.left
                                            anchors.leftMargin: 5
                                            anchors.top: parent.top
                                            anchors.bottom: parent.bottom
                                            verticalAlignment: Text.AlignVCenter
                                            text: modelData
                                            color: Colors.defaultControlColor
                                        }
                                    }

                                    CloseIcon {
                                        width: 14
                                        height: 14
                                        isActive: true
                                        anchors.verticalCenter: tagTextRect.verticalCenter
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
                    StyledText {
                        text: combinedArtworks.keywordsCount
                        color: Colors.defaultControlColor
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    StyledText {
                        text: qsTr("Copy keywords")
                        color: combinedCopyKeywordsMA.pressed ? Colors.defaultLightColor : Colors.artworkActiveColor

                        MouseArea {
                            id: combinedCopyKeywordsMA
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
                                artItemsModel.updateSelectedArtworks()
                                closePopup()
                            }

                            tooltip: "Sets description and appends keywords in edited files"
                        }

                        StyledButton {
                            text: qsTr("Save and replace")
                            width: 130
                            onClicked: {
                                combinedArtworks.saveSetKeywords()
                                artItemsModel.updateSelectedArtworks()
                                closePopup()
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
