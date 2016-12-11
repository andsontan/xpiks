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
import QtGraphicalEffects 1.0
import "../StyledControls"

Item {
    id: completeBox
    property color highlightedItemColor: Colors.artworkActiveColor
    property double maxCount: 5
    property bool isBelowEdit: true
    property alias model: dropDownItems.model
    property var autoCompleteSource: acSource
    property real itemHeight: 25

    signal boxDestruction();
    Component.onDestruction: boxDestruction();

    width: 200
    height: childrenRect.height

    signal itemSelected(string completion);

    function openPopup() {
        completeBox.state = "dropDown"
    }

    function closePopup() {
        console.debug("Closing completion popup")
        completeBox.state = ""
        completeBox.destroy()
    }

    Connections {
        target: autoCompleteSource
        onDismissPopupRequested: closePopup()
        onCompletionAccepted: itemSelected(completion)
        onSelectedIndexChanged: dropDownItems.positionViewAtIndex(autoCompleteSource.selectedIndex, ListView.Contain)
    }

    RectangularGlow {
        anchors.left: dropDown.left
        anchors.right: dropDown.right
        anchors.top: dropDown.top
        anchors.bottom: dropDown.bottom
        anchors.topMargin: isBelowEdit ? glowRadius : -glowRadius/2
        anchors.bottomMargin: isBelowEdit ? -glowRadius : glowRadius
        visible: dropDown.visible
        height: dropDown.height
        glowRadius: 4
        spread: 0.0
        color: Colors.defaultControlColor
        cornerRadius: glowRadius
    }

    Rectangle {
        id: dropDown
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        color: Colors.inputForegroundColor
        height: 0
        focus: false
        clip: true

        onActiveFocusChanged: {
            if (!activeFocus) {
                completeBox.state = ""
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
                color: isSelected ? highlightedItemColor : Colors.inputForegroundColor
                property var itemText: display
                property bool isLastItem: index === (dropDownItems.count - 1)
                property bool isSelected: index === autoCompleteSource.selectedIndex
                anchors.left: parent.left
                anchors.right: parent.right
                height: itemHeight + 1

                StyledText {
                    text: parent.itemText
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.leftMargin: 16
                    color: isSelected ? Colors.whiteColor : Colors.inputBackgroundColor
                }

                Rectangle {
                    visible: !currentDelegate.isLastItem
                    enabled: !currentDelegate.isLastItem
                    height: 1
                    color: isSelected ? highlightedItemColor : Colors.listSeparatorColor
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
                        autoCompleteSource.acceptSelected()
                    }

                    onPositionChanged: {
                        autoCompleteSource.selectedIndex = index
                    }
                }
            }
        }

        CustomScrollbar {
            id: scrollBar
            visible: dropDownItems.count > maxCount
            anchors.rightMargin: -13
            anchors.topMargin: -2
            anchors.bottomMargin: -2
            flickable: dropDownItems
        }
    }

    states: State {
        name: "dropDown";
        PropertyChanges {
            target: dropDown;
            height: dropDownItems.count > maxCount ? (maxCount * (completeBox.itemHeight + 1) + 10) : ((completeBox.itemHeight + 1) * dropDownItems.count + 10)
        }
    }

    transitions: Transition {
        NumberAnimation {
            target: dropDown
            properties: "height";
            easing.type: Easing.OutQuad;
            duration: 100
        }
    }
}
