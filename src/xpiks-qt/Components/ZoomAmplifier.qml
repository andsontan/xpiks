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
    property bool isPlus: true
    property int thickness: 3
    property color lineColor: Colors.artworkActiveColor
    property real circleWidth: item.width*0.75
    property real plusOffset: 0.5

    Rectangle {
        id: circle
        width: item.circleWidth
        height: item.circleWidth
        radius: width/2
        border.width: item.thickness
        border.color: item.lineColor
        anchors.left: parent.left
        anchors.top: parent.top
        color: "transparent"

        Rectangle {
            id: plusHorizontal
            width: circle.width/2
            height: item.thickness
            radius: item.thickness
            color: item.lineColor
            anchors.centerIn: parent
            anchors.verticalCenterOffset: item.plusOffset
            anchors.horizontalCenterOffset: item.plusOffset
        }

        Rectangle {
            id: plusVertical
            visible: item.isPlus
            height: circle.width/2
            width: item.thickness
            radius: item.thickness
            color: item.lineColor
            anchors.centerIn: parent
            anchors.verticalCenterOffset: item.plusOffset
            anchors.horizontalCenterOffset: item.plusOffset
        }
    }

    Rectangle {
        width: circle.width*0.6
        height: item.thickness
        radius: item.thickness
        color: item.lineColor
        transformOrigin: Item.BottomRight
        rotation: 43
        anchors.right: parent.right
        anchors.bottom: parent.bottom
    }
}
