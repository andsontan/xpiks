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
import "../Constants"
import "../StyledControls"

Item {
    id: container
    width: childrenRect.width
    height: 24

    property bool isListLayout: true
    signal layoutChanged()
    property color currentColor: {
        var resultColor = Colors.selectedArtworkBackground
        if (enabled) {
            if (layoutMA.pressed) {
                resultColor = Colors.linkClickedColor
            } else if (layoutMA.containsMouse) {
                resultColor = Colors.labelActiveForeground
            } else {
                resultColor = Colors.selectedArtworkBackground
            }
        } else {
            resultColor = Colors.selectedArtworkBackground
        }

        return resultColor
    }

    Column {
        width: height
        height: parent.height
        anchors.left: parent.left
        anchors.top: parent.top
        visible: !isListLayout
        spacing: 5

        Repeater {
            model: 3
            delegate: Item {
                id: row
                width: parent.width
                height: parent.height / 3 - 3

                Rectangle {
                    anchors.left: parent.left
                    anchors.top: parent.top
                    height: parent.height
                    width: height
                    color: container.currentColor
                }

                Rectangle {
                    anchors.right: parent.right
                    anchors.top: parent.top
                    height: parent.height
                    width: row.width * 0.6
                    color: container.currentColor
                }
            }
        }
    }

    Grid {
        columns: 3
        rows: 3
        width: height
        height: parent.height
        visible: isListLayout
        columnSpacing: 5
        rowSpacing: 5

        Repeater {
            model: 9

            Rectangle {
                width: parent.width / 3 - 3
                height: parent.height / 3 - 3
                color: container.currentColor
            }
        }
    }

    StyledText {
        anchors.left: parent.left
        anchors.leftMargin: parent.height + 10
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: 2
        text: i18.n + (isListLayout ? qsTr("Grid") : qsTr("List"))
        color: container.currentColor
    }

    MouseArea {
        enabled: container.enabled
        id: layoutMA
        anchors.fill: parent
        hoverEnabled: true
        onClicked: layoutChanged()
    }
}
