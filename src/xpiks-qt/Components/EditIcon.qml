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

import QtQuick 2.0

Item {
    id: host
    width: 30
    height: 30
    property color iconColor: editMA.pressed ? Colors.linkClickedColor : (editMA.containsMouse ? Colors.artworkActiveColor : Colors.inputBackgroundColor)
    property color backgroundColor: Colors.defaultControlColor
    signal actionInvoked()

    Rectangle {
        id: background
        border.width: 3
        border.color: host.iconColor
        anchors.fill: parent
        anchors.rightMargin: 5
        anchors.topMargin: 5
        color: "transparent"
    }

    Item {
        id: pencil
        anchors.fill: parent
        anchors.bottomMargin: 9
        anchors.leftMargin: 9

        Rectangle {
            height: 5
            width: 1
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            color: host.iconColor
            radius: 1
        }

        Rectangle {
            height: 1
            width: 5
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            color: host.iconColor
            radius: 1
        }

        Rectangle {
            color: host.backgroundColor
            height: 11
            width: 16.5
            rotation: 135
            transformOrigin: Item.Center
            anchors.centerIn: parent
            anchors.verticalCenterOffset: 0.5
            anchors.horizontalCenterOffset: -1

            Rectangle {
                anchors.fill: parent
                anchors.leftMargin: 2
                anchors.rightMargin: -2
                anchors.topMargin: 2
                anchors.bottomMargin: 2
                color: host.iconColor
            }

            Rectangle {
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.leftMargin: -4
                anchors.topMargin: 2
                anchors.bottomMargin: 2

                color: host.iconColor
                width: 4
            }
        }
    }

    MouseArea {
        id: editMA
        anchors.fill: parent
        hoverEnabled: true
        onClicked: actionInvoked()
    }
}

