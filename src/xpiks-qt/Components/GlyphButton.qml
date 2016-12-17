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
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import "../Constants"
import "../StyledControls"

Rectangle {
    id: control
    property bool isDefault: false
    clip: true
    height: 24
    property alias hovered: glyphButtonMA.containsMouse
    property alias pressed: glyphButtonMA.pressed
    property alias glyph: glyphWrapper.data
    property int leftShift: 0
    property bool glyphVisible: false
    property string text: ''
    property int glyphMargin: 10
    signal clicked()

    color: {
        var result = Colors.buttonDefaultBackground;

        if (control.enabled &&
                (control.isDefault || control.hovered)) {
            result = Colors.buttonHoverBackground;
        }

        return result
    }

    property color textColor: {
        var result = Colors.defaultControlColor;

        if (control.enabled) {
            result = control.pressed ? Colors.buttonPressedForeground : Colors.buttonDefaultForeground;
        } else {
            result = Colors.buttonDisabledForeground;
        }

        return result
    }

    Item {
        id: innerItem
        anchors.horizontalCenterOffset: control.leftShift
        anchors.centerIn: parent
        width: childrenRect.width
        height: glyphWrapper.height

        Item {
            id: glyphWrapper
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            visible: control.glyphVisible
            height: childrenRect.height
            width: childrenRect.width
        }

        StyledText {
            anchors.left: control.glyphVisible ? glyphWrapper.right : parent.left
            anchors.leftMargin: control.glyphVisible ? control.glyphMargin : 0
            anchors.verticalCenter: parent.verticalCenter
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: control.text
            color: control.textColor
        }
    }

    MouseArea {
        id: glyphButtonMA
        anchors.fill: parent
        hoverEnabled: true
        onClicked: control.clicked()
    }
}
