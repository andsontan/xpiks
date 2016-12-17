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

Item {
    id: item
    property bool isActive

    signal itemClicked();

    Item {
        anchors.fill: parent
        opacity: mouseArea.containsMouse ? 1 : 0.8

        Rectangle {
            color: mouseArea.pressed ? Colors.artworkModifiedColor : (isActive ? Colors.defaultControlColor : Colors.inputForegroundColor)
            width: parent.width - 4
            height: 3
            border.width: 1
            border.color: color
            radius: 2
            transformOrigin: Item.Center
            rotation: 90
            anchors.centerIn: parent
        }

        Rectangle {
            color: mouseArea.pressed ? Colors.artworkModifiedColor : (isActive ? Colors.defaultControlColor : Colors.inputForegroundColor)
            width: parent.width - 4
            height: 3
            radius: 2
            border.width: 1
            border.color: color
            anchors.centerIn: parent
        }

        scale: mouseArea.pressed ? 0.8 : 1

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            hoverEnabled: true
            preventStealing: true
            onClicked: {
                itemClicked()
            }
        }
    }
}

