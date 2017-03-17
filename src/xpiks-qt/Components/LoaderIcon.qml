/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
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

Item {
    id: control
    implicitWidth: 100
    implicitHeight: 100
    visible: running
    property bool running: false

    property int lines: 11
    property real length: 30 // % of the width of the control
    property real sizeFactor: 10 // % of the height of the control
    property real radius: 23 // % of the width of the control
    property real corner: 1 // between 0 and 1
    property real speed: 100 // smaller is faster
    property real trail: 0.6 // between 0 and 1
    property bool clockWise: true

    property string color: "#7B756B"
    property string highlightColor: "#ffffff"
    property string bgColor: "transparent"

    property variant colors: ["#888888", "#999999", "#a0a0a0", "#a8a8a8", "#b9b9b9", "#cacaca", "#dcdcdc", "#ededed", "#ffffff", color, color]

    Item {
        id: spinnerHost
        visible: control.running
        anchors.fill: parent
        property int currentIndex: 0

        Repeater {
            id: repeat
            model: control.lines

            delegate: Rectangle {
                property int delegateIndex: index
                property real factor: control.width / 200
                color: control.colors[(delegateIndex - spinnerHost.currentIndex + control.lines) % control.lines]

                radius: control.corner * height / 2

                width: control.length * factor
                height: control.sizeFactor * factor

                x: control.width / 2 + control.radius * factor
                y: control.height / 2 - height / 2

                transform: Rotation {
                    origin.x: -control.radius * factor
                    origin.y: height / 2
                    angle: index * (360 / repeat.count)
                }
            }
        }

        Timer {
            running: control.running
            interval: control.speed
            repeat: true
            onTriggered: {
                spinnerHost.currentIndex = (spinnerHost.currentIndex + 1) % control.lines
            }
        }
    }
}
