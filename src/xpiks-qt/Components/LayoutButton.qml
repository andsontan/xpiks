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

Item {
    id: container
    width: 24
    height: 24

    property bool isListLayout: true
    signal layoutChanged()

    Item {
        anchors.fill: parent
        visible: !isListLayout

        Rectangle {
            color: layoutMA.containsMouse ? Colors.selectedArtworkColor : Colors.defaultControlColor
            anchors.left: parent.left
            anchors.top: parent.top
            width: parent.width
            height: parent.height/2 - 2
        }

        Rectangle {
            color: layoutMA.containsMouse ? Colors.selectedArtworkColor : Colors.defaultControlColor
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            width: parent.width
            height: parent.height/2 - 2
        }
    }

    Item {
        anchors.fill: parent
        visible: isListLayout

        Rectangle {
            color: layoutMA.containsMouse ? Colors.selectedArtworkColor : Colors.defaultControlColor
            anchors.left: parent.left
            anchors.top: parent.top
            width: parent.width/2 - 2
            height: parent.height/2 - 2
        }

        Rectangle {
            color: layoutMA.containsMouse ? Colors.selectedArtworkColor : Colors.defaultControlColor
            anchors.right: parent.right
            anchors.top: parent.top
            width: parent.width/2 - 2
            height: parent.height/2 - 2
        }

        Rectangle {
            color: layoutMA.containsMouse ? Colors.selectedArtworkColor : Colors.defaultControlColor
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            width: parent.width/2 - 2
            height: parent.height/2 - 2
        }

        Rectangle {
            color: layoutMA.containsMouse ? Colors.selectedArtworkColor : Colors.defaultControlColor
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            width: parent.width/2 - 2
            height: parent.height/2 - 2
        }
    }

    MouseArea {
        enabled: container.enabled
        id: layoutMA
        anchors.fill: parent
        hoverEnabled: true
        onClicked: layoutChanged()
    }
}
