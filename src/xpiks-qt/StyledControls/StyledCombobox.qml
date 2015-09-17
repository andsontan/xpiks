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

import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import "../Constants"
import "../Constants/Colors.js" as Colors

ComboBox {
    style: ComboBoxStyle {
        //renderType: Text.NativeRendering
        //textColor: Colors.defaultLightColor

        background: Rectangle {
            implicitHeight: 24
            implicitWidth: 100
            color: Colors.defaultControlColor

            Rectangle {
                id: glyph
                property color glyphColor: Colors.defaultLightColor
                width: 10
                height: 10
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.rightMargin: 10
                transformOrigin: Item.Center
                color: "transparent"

                Rectangle {
                    id: topBar
                    width: parent.width + 2
                    height: 3
                    radius: 3
                    color: glyph.glyphColor
                    transformOrigin: Item.Center
                    transform: Translate { x: -1; y: -1 }
                }

                Rectangle {
                    id: rightSlash
                    width: parent.width*1.2
                    radius: 3
                    height: 3
                    color: glyph.glyphColor
                    transformOrigin: Item.Center
                    transform: [
                        Rotation { angle: 120; origin.x: glyph.width/2; origin.y: glyph.height/2 },
                        Translate { x: 0; y: -2 }
                    ]
                }

                Rectangle {
                    id: leftSlash
                    width: parent.width*1.2
                    radius: 3
                    height: 3
                    color: glyph.glyphColor
                    transformOrigin: Item.Center
                    transform: [
                        Rotation { angle: 60; origin.x: glyph.width/2; origin.y: glyph.height/2 },
                        Translate { x: -6; y: 1 }
                    ]
                }

                transitions: Transition {
                    RotationAnimation { duration: 50; direction: RotationAnimation.Clockwise }
                }

                states: State {
                    name: "inverted"
                    when: __popup.__popupVisible
                    PropertyChanges { target: glyph; rotation: 180; }
                }
            }
        }

        label: StyledText {
            anchors.fill: parent
            text: control.currentText
            verticalAlignment: Qt.AlignVCenter
            anchors.leftMargin: 5
        }
    }
}

