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
import QtQuick.Layouts 1.1
import "../Constants"
import "../StyledControls"
import "../Constants/UIConfig.js" as UIConfig

Rectangle {
    id: itemWrapper
    property int delegateIndex
    property bool isSelected
    property string suggestionText
    property alias itemHeight: suggestionText.height

    signal actionClicked();

    color: isSelected ? Colors.whiteColor : (innerMA.containsMouse ? Colors.artworkActiveColor : Colors.inactiveControlColor)

    width: suggestionText.width + 10
    height: childrenRect.height

    StyledText {
        id: suggestionText
        verticalAlignment: Text.AlignVCenter
        anchors.left: parent.left
        anchors.leftMargin: 5
        text: itemWrapper.suggestionText
        color: isSelected ? Colors.inactiveControlColor : (innerMA.containsMouse ? Colors.whiteColor : Colors.labelActiveForeground)
        font.pixelSize: UIConfig.fontPixelSize * settingsModel.keywordSizeScale
    }

    MouseArea {
        id: innerMA
        anchors.left: parent.left
        anchors.top: parent.top
        width: suggestionText.width + 10
        height: suggestionText.height
        hoverEnabled: true
        onClicked: actionClicked()
        cursorShape: Qt.PointingHandCursor
    }
}
