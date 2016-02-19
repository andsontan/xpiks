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
import "../Constants/Colors.js" as Colors
import "../StyledControls"

Item {
    id: container
    width: childrenRect.width
    height: 24

    property bool isListLayout: true
    signal layoutChanged()
    property color currentColor: enabled ? (layoutMA.containsMouse ? Colors.defaultInputBackground : Colors.selectedArtworkColor) : Colors.defaultControlColor

    Item {
        width: height
        height: parent.height
        anchors.left: parent.left
        anchors.top: parent.top
        visible: !isListLayout

        Rectangle {
            color: container.currentColor
            anchors.left: parent.left
            anchors.top: parent.top
            width: parent.width
            height: parent.height/2 - 2
        }

        Rectangle {
            color: container.currentColor
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            width: parent.width
            height: parent.height/2 - 2
        }
    }

    Item {
        width: height
        height: parent.height
        anchors.left: parent.left
        anchors.top: parent.top
        visible: isListLayout

        Rectangle {
            color: container.currentColor
            anchors.left: parent.left
            anchors.top: parent.top
            width: parent.width/2 - 2
            height: parent.height/2 - 2
        }

        Rectangle {
            color: container.currentColor
            anchors.right: parent.right
            anchors.top: parent.top
            width: parent.width/2 - 2
            height: parent.height/2 - 2
        }

        Rectangle {
            color: container.currentColor
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            width: parent.width/2 - 2
            height: parent.height/2 - 2
        }

        Rectangle {
            color: container.currentColor
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            width: parent.width/2 - 2
            height: parent.height/2 - 2
        }
    }

    StyledText {
        anchors.left: parent.left
        anchors.leftMargin: parent.height + 7
        anchors.verticalCenter: parent.verticalCenter

        text: isListLayout ? qsTr("Grid") : qsTr("List")
        color: container.enabled ? (layoutMA.containsMouse ? Colors.defaultLightColor : Colors.itemsSourceSelected) : Colors.itemsSourceSelected
    }

    MouseArea {
        enabled: container.enabled
        id: layoutMA
        anchors.fill: parent
        hoverEnabled: true
        onClicked: layoutChanged()
    }
}
