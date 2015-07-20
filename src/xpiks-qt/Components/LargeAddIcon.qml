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
import "../Constants"
import "../Constants/Colors.js" as Colors

Item {
    id: item
    signal itemClicked();

    Rectangle {
        anchors.fill: parent
        color: "transparent"

        Rectangle {
            color: Colors.defaultLightGrayColor
            width: parent.width/2
            height: 8
            border.width: 4
            border.color: color
            radius: 4
            transformOrigin: Item.Center
            rotation: 90
            anchors.centerIn: parent
        }

        Rectangle {
            color: Colors.defaultLightGrayColor
            width: parent.width/2
            height: 8
            radius: 4
            border.width: 4
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

