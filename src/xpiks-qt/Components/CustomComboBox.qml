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
import "../StyledControls"
import xpiks 1.0
import QtGraphicalEffects 1.0

Item {
    id: comboBox
    property color highlightedItemColor: Colors.artworkActiveColor
    property bool showColorSign: true
    property double headerHeight: comboBox.height
    property double itemHeight: comboBox.height
    property double maxCount: 5
    property alias model: dropDownItems.model
    property alias selectedIndex: dropDownItems.currentIndex
    signal comboIndexChanged();

    function closePopup() {
        comboBox.state = ""
    }

    RectangularGlow {
        anchors.left: header.left
        anchors.right: header.right
        anchors.top: header.top
        anchors.bottom: dropDown.bottom
        anchors.topMargin: glowRadius/2
        anchors.bottomMargin: -glowRadius
        visible: dropDown.visible
        height: dropDown.height
        glowRadius: 4
        spread: 0.0
        color: Colors.defaultDarkColor
        cornerRadius: glowRadius
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
                anchors.leftMargin: 10
                verticalAlignment: TextInput.AlignVCenter
            }
        }

        Rectangle {
            id: arrowRect
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: 20
            color: enabled ? Colors.artworkBackground : Colors.panelSelectedColor

            TriangleElement {
                anchors.centerIn: parent
                anchors.verticalCenterOffset: isFlipped ? height*0.3 : 0
                color: (enabled && (headerMA.containsMouse || comboBox.state === "dropDown")) ? Colors.labelActiveForeground : Colors.labelInactiveForeground
                isFlipped: comboBox.state === ""
                width: parent.width * 0.6
                height: width * 0.5
            }
        }

        MouseArea {
            id: headerMA
            hoverEnabled: true
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
        color: Colors.popupBackgroundColor
        visible: false
        height: 0
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
            anchors.topMargin: 5
            anchors.bottomMargin: 5
            anchors.rightMargin: scrollBar.visible ? 14 : 0
            anchors.margins: 1
            boundsBehavior: Flickable.StopAtBounds

            delegate: Rectangle {
                id: currentDelegate
                color: itemMA.containsMouse ? highlightedItemColor : dropDown.color
                property var itemText: modelData
                property bool isCurrentItem: index == comboBox.selectedIndex
                property bool isLastItem: index === (dropDownItems.count - 1)
                anchors.left: parent.left
                anchors.right: parent.right
                height: itemHeight + 1

                StyledText {
                    text: i18.n + itemText
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.leftMargin: 16
                    color: itemMA.containsMouse ? Colors.whiteColor : (isCurrentItem ? highlightedItemColor : Colors.labelActiveForeground)
                }

                Rectangle {
                    visible: !currentDelegate.isLastItem
                    enabled: !currentDelegate.isLastItem
                    height: 1
                    color: itemMA.containsMouse ? highlightedItemColor : Colors.inputBackgroundColor
                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.leftMargin: 6
                    anchors.rightMargin: 20
                    anchors.bottomMargin: -height/2
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
            visible: dropDownItems.count > maxCount
            anchors.rightMargin: -12
            flickable: dropDownItems
        }
    }

    states: State {
        name: "dropDown";
        PropertyChanges {
            target: dropDown;
            height: dropDownItems.count > maxCount ? (maxCount * (comboBox.itemHeight + 1) + 10) : ((comboBox.itemHeight + 1) * dropDownItems.count + 10)
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
