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
    property bool isActive: false
    property bool isPlus: false
    property int thickness: 3
    property alias crossOpacity: wrapperRect.opacity
    property color disabledColor: Colors.closeIconDisabledColor
    signal itemClicked();

    Item {
        id: wrapperRect
        anchors.fill: parent
        opacity: closeIconMouseArea.containsMouse ? 1 : 0.9

        property color pressColor: {
            if (closeIconMouseArea.containsMouse) {
                return Colors.artworkModifiedColor;
            } else {
                return item.isActive ? Colors.closeIconActiveColor : Colors.closeIconInactiveColor;
            }
        }

        Rectangle {
            color: enabled ? wrapperRect.pressColor : item.disabledColor
            width: isPlus ? parent.width - 2 : parent.width
            height: item.thickness
            radius: item.thickness/2
            transformOrigin: Item.Center
            rotation: isPlus ? 90 : 45
            transform: Translate { y: (width - thickness)/2 }
        }

        Rectangle {
            color: enabled ? wrapperRect.pressColor : item.disabledColor
            width: isPlus ? parent.width - 2 : parent.width
            height: item.thickness
            radius: item.thickness/2
            transformOrigin: Item.Center
            rotation: isPlus ? 0 : 135
            transform: Translate { y: (width - thickness)/2 }
        }

        scale: closeIconMouseArea.pressed ? 0.8 : 1

        MouseArea {
            id: closeIconMouseArea
            anchors.fill: parent
            hoverEnabled: true
            preventStealing: true
            onClicked: {
                itemClicked()
            }
        }
    }
}

