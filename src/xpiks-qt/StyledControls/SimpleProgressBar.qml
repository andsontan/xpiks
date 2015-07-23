/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2015 Taras Kushnir <kushnirTV@gmail.com>
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

import QtQuick 2.3
import "../Constants"
import "../Constants/Colors.js" as Colors

Item {
    id: progressbar

    property int minimum: 0
    property int maximum: 100
    property int value: 0
    property color color: "#77B753"

    width: 250
    height: 23
    clip: true

    Rectangle {
        id: border
        anchors.fill: parent
        color: Colors.defaultControlColor
        border.width: 0
        border.color: parent.color
    }

    Rectangle {
        id: highlight
        property int widthDest: ( ( (progressbar.width + 20) * ( value - minimum ) ) / ( maximum - minimum ) - 4 )
        width: highlight.widthDest
        radius: height / 2

        Behavior on width {
            SmoothedAnimation {
                velocity: 1200
            }
        }

        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
            leftMargin: -10
            //margins: 1
        }

        color: parent.color
    }
}

