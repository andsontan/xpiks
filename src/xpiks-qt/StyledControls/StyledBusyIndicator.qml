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
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3
import "../Constants"
import "../Constants/Colors.js" as Colors

BusyIndicator {
    style: BusyIndicatorStyle {
        id: style
        property int lines: 11
        property real length: 30 // % of the width of the control
        property real width: 10 // % of the height of the control
        property real radius: 23 // % of the width of the control
        property real corner: 1 // between 0 and 1
        property real speed: 100 // smaller is faster
        property real trail: 0.6 // between 0 and 1
        property bool clockWise: true

        property real opacity: 0.7
        property string color: "#7B756B"
        property string highlightColor: Colors.defaultLightColor
        property string bgColor: "transparent"

        indicator: Rectangle {
            color: style.bgColor
            visible: control.running
            Repeater {
                id: repeat
                model: style.lines
                Rectangle {
                    property real factor: control.width / 200
                    color: style.color
                    opacity: style.opacity
                    Behavior on color {
                        ColorAnimation {
                            from: style.highlightColor
                            duration: style.speed * style.lines * style.trail
                        }
                    }
                    radius: style.corner * height / 2
                    width: style.length * factor
                    height: style.width * factor
                    x: control.width / 2 + style.radius * factor
                    y: control.height / 2 - height / 2
                    transform: Rotation {
                        origin.x: -style.radius * factor
                        origin.y: height / 2
                        angle: index * (360 / repeat.count)
                    }
                    Timer {
                        id: reset
                        interval: style.speed * (style.clockWise ? index : style.lines - index)
                        onTriggered: {
                            parent.opacity = 1
                            parent.color = style.highlightColor
                            reset2.start()
                        }
                    }
                    Timer {
                        id: reset2
                        interval: style.speed
                        onTriggered: {
                            parent.opacity = style.opacity
                            parent.color = style.color
                        }
                    }
                    Timer {
                        id: globalTimer // for a complete cycle
                        interval: style.speed * style.lines
                        onTriggered: {
                            reset.start()
                        }
                        triggeredOnStart: true
                        repeat: true
                    }

                    Component.onCompleted: {
                        globalTimer.start()
                    }
                }
            }
        }
    }
}
