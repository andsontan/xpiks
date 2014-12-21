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
            width: 640
            height: 520
            radius: 10
            color: "#eeeeee"
            anchors.centerIn: parent

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 5

                RowLayout {
                    spacing: 5
                    Layout.fillWidth: true

                    Item {
                        Layout.fillWidth: true
                    }

                    Button {
                        text: qsTr("Remove selected")
                        onClicked: {
                            var itemsCount = combinedArtworks.getSelectedArtworksCount()
                            if (itemsCount > 0) {
                                confirmRemoveArtworksDialog.itemsCount = itemsCount
                                confirmRemoveArtworksDialog.open()
                            }
                        }
                    }
                }

                Rectangle {
                    border.color: "black"
                    border.width: 1
                    height: 60
                    width: parent.width

                    ScrollView {
                        id: imagesScrollView
                        anchors.fill: parent
                        anchors.margins: 2

                        style: ScrollViewStyle {
                            //transientScrollBars: true
                            minimumHandleLength: 20

                            handle: Item {
                                implicitHeight: 3
                                implicitWidth: 10

                                Rectangle {
                                    anchors.fill: parent
                                    color: "black"
                                    opacity: styleData.pressed ? 1 : (styleData.hovered ? 0.8 : 0.5)
                                }
                            }

                            scrollBarBackground: Item {
                                property bool sticky: false
                                property bool hovered: styleData.hovered
                                implicitWidth: 3
                                implicitHeight: 3
                                clip: true

                                Rectangle {
                                    anchors.fill: parent
                                    anchors.leftMargin: 3
                                    anchors.rightMargin: 3
                                    color: "#dddddd"
                                    opacity: styleData.hovered ? 1 : 0.8
                                }

                                onHoveredChanged: if (hovered) sticky = true
                            }

                            corner: Item {}
                            decrementControl: Item {}
                            incrementControl: Item {}
                        }

                        ListView {
                            boundsBehavior: Flickable.StopAtBounds
                            anchors.fill: parent
                            orientation: Qt.Horizontal
                            spacing: 3
                            model: combinedArtworks

                            delegate: Rectangle {
                                property int indexOfThisDelegate: index
                                id: imageWrapper
                                height: 50
                                width: height
                                border.width: isselected ? 1 : 0
                                border.color: "black"

                                MouseArea {
                                    id: mouseArea
                                    anchors.fill: parent
                                    onClicked: {
                                        if (isselected) {
                                            combinedArtworks.deselectArtwork(indexOfThisDelegate)
                                        } else {
                                            combinedArtworks.selectArtwork(indexOfThisDelegate)
                                        }
                                    }
                                }

                                Image {
                                    anchors.fill: parent
                                    anchors.margins: 1
                                    source: "image://global/" + path
                                    sourceSize.width: 150
                                    sourceSize.height: 150
                                    fillMode: Image.PreserveAspectFit
                                    asynchronous: true
                                }
                            }
                        }
                    }
                }

                Text {
                    text: qsTr("Title:")
                    anchors.left: parent.left
                    renderType: Text.NativeRendering
                }

                Rectangle {
                    id: anotherRect
                    width: 300
                    height: 25
                    color: "white"
                    anchors.left: parent.left

                    TextInput {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.leftMargin: 5
                        anchors.rightMargin: 5
                        anchors.verticalCenter: parent.verticalCenter
                        maximumLength: 250
                        clip: true
                        text: combinedArtworks.title
                        onTextChanged: combinedArtworks.title = text
                        renderType: TextInput.NativeRendering
                    }
                }

                Text {
                    text: qsTr("Description:")
                    anchors.left: parent.left
                    renderType: Text.NativeRendering
                }

                Rectangle {
                    id: rect
                    width: 300
                    height: 25
                    color: "white"
                    anchors.left: parent.left

                    TextInput {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.leftMargin: 5
                        anchors.rightMargin: 5
                        anchors.verticalCenter: parent.verticalCenter
                        maximumLength: 250
                        clip: true
                        text: combinedArtworks.description
                        onTextChanged: combinedArtworks.description = text
                        renderType: TextInput.NativeRendering
                    }
                }

                Text {
                    id: keywordsLabel
                    anchors.left: parent.left
                    text: qsTr("Keywords:")
                }

                Rectangle {
                    id: keywordsWrapper
                    color: "#adadad"
                    Layout.fillHeight: true
                    Layout.fillWidth: true

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

                    ScrollView {
                        id: scroller
                        anchors.fill: parent
                        highlightOnFocus: true

                        EditableTags {
                            id: flv
                            anchors.margins: 5
                            model: combinedArtworks.keywords

                            delegate: Rectangle {
                                id: itemWrapper
                                property int indexOfThisDelegate: index
                                property string keyword: modelData
                                border.width: 1
                                border.color: "black"
                                color: "#cccccc"

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

                Rectangle {
                    width: parent.width
                    height: 30

                    RowLayout {
                        anchors.fill: parent
                        spacing: 5

                        Item {
                            Layout.fillWidth: true
                        }

                        Button {
                            text: qsTr("Save and append")
                            onClicked: {
                                combinedArtworks.saveAddKeywords()
                                closePopup()
                                artItemsModel.updateSelectedArtworks()
                            }

                            tooltip: "Sets description and appends keywords in edited files"
                        }

                        Button {
                            text: qsTr("Save and replace")
                            onClicked: {
                                combinedArtworks.saveSetKeywords()
                                closePopup()
                                artItemsModel.updateSelectedArtworks()
                            }

                            tooltip: "Sets description and replaces keywords in edited files"
                        }

                        Button {
                            text: qsTr("Cancel")

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
}
