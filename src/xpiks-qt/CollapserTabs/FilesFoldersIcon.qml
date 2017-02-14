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

import QtQuick 2.0
import QtQuick.Layouts 1.1
import "../Constants" 1.0
import "../Components"
import "../StyledControls"
import "../Dialogs"
import "../Common.js" as Common
import "../Constants/UIConfig.js" as UIConfig
import xpiks 1.0

FolderElement {
    width: 20
    height: 17
    color: isHighlighted ? Colors.labelActiveForeground : Colors.inactiveControlColor
}

/*Item {
    id: host
    width: 24
    height: 20
    property real thickness: 2

    Rectangle {
        anchors.top: parent.top
        anchors.left: parent.left
        width: 10
        height: 5
        color: "transparent"
        border.color: isHighlighted ? Colors.labelActiveForeground : Colors.inactiveControlColor
        border.width: host.thickness
    }

    Rectangle {
        anchors.bottom: parent.bottom
        width: 24
        height: 17
        color: "transparent"
        border.color: isHighlighted ? Colors.labelActiveForeground : Colors.inactiveControlColor
        border.width: host.thickness
    }

    Rectangle {
        width: 6
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: host.thickness
        anchors.leftMargin: host.thickness
        height: host.thickness*2
        color: parentBackground
    }
}*/
