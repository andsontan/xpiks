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
import QtQuick.Controls 1.2
import "../Constants"
import "../Constants/Colors.js" as Colors
import "../StyledControls"
import xpiks 1.0

Item {
    id: comboBox
    property color highlightedItemColor: Colors.selectedMetadataColor
    property bool showColorSign: false
    property double headerHeight: comboBox.height
    property double itemHeight: comboBox.height
    property double maxHeight: 150
    property alias model: dropDownItems.model
    property alias selectedIndex: dropDownItems.currentIndex
    signal comboIndexChanged();

    function closePopup() {
        comboBox.state = ""
    }

    Item {
        id: header
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        height: headerHeight

        Rectangle {
            id: colorSign
            anchors.left: parent.left
            width: showColorSign ? 6 : 0
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            color: highlightedItemColor
        }

        Rectangle {
            id: selectedItem
            anchors.left: colorSign.right
            anchors.right: arrowRect.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            color: Colors.defaultControlColor

            StyledText {
                id: selectedText
                text: dropDownItems.currentItem.itemText
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.leftMargin: 5
                verticalAlignment: TextInput.AlignVCenter
            }
        }

        Rectangle {
            id: arrowRect
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: 15
            color: Colors.artworkActiveColor

            TriangleElement {
                anchors.centerIn: parent
                anchors.verticalCenterOffset: isFlipped ? height*0.3 : 0
                color: Colors.checkboxCheckedColor
                isFlipped: comboBox.state === ""
                width: parent.width * 0.6
                height: width * 0.5
            }
        }

        MouseArea {
            anchors.fill: parent;
            onClicked: {
                comboBox.state = comboBox.state === "dropDown" ? "" : "dropDown"
                dropDown.forceActiveFocus()
            }
        }
    }

    Rectangle {
        id: dropDown
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: header.bottom
        color: Colors.itemsSourceBackground
        anchors.topMargin: 1
        visible: false
        height: 0
        border.width: 1
        border.color: Colors.selectedMetadataColor
        focus: true
        clip: true

        onActiveFocusChanged: {
            if (!activeFocus) {
                comboBox.state = ""
            }
        }

        ListView {
            id: dropDownItems
            anchors.fill: parent
            anchors.rightMargin: scrollBar.visible ? 14 : 0
            anchors.margins: 1
            boundsBehavior: Flickable.StopAtBounds

            delegate: Rectangle {
                color: itemMA.containsMouse ? Colors.artworkActiveColor : Colors.selectedArtworkColor
                property var itemText: modelData
                anchors.left: parent.left
                anchors.right: parent.right
                height: itemHeight

                StyledText {
                    text: itemText
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.leftMargin: 10
                    color: itemMA.containsMouse ? Colors.checkboxCheckedColor : Colors.defaultLightColor
                    font.bold: index == comboBox.selectedIndex
                }

                MouseArea {
                    id: itemMA
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        if (index === comboBox.selectedIndex) {
                            return;
                        }

                        comboBox.selectedIndex = index
                        comboBox.state = ""
                        comboIndexChanged()
                    }
                }
            }
        }

        CustomScrollbar {
            id: scrollBar
            visible: comboBox.itemHeight * dropDownItems.count > maxHeight
            anchors.rightMargin: -12
            flickable: dropDownItems
        }
    }

    states: State {
        name: "dropDown";
        PropertyChanges {
            target: dropDown;
            height: (comboBox.itemHeight * dropDownItems.count) > maxHeight ? maxHeight : (comboBox.itemHeight * dropDownItems.count)
            visible: true
        }
    }

    transitions: Transition {
        NumberAnimation {
            target: dropDown;
            properties: "height";
            easing.type: Easing.OutExpo; duration: 400
        }
    }
}
