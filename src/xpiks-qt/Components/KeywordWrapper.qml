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
import QtQuick.Layouts 1.1
import "../Constants"
import "../Constants/Colors.js" as Colors
import "../StyledControls"

Rectangle {
    id: itemWrapper
    property int delegateIndex
    property bool isHighlighted
    property string keywordText
    property bool hasPlusSign: false
    property alias itemHeight: tagTextRect.height

    signal actionClicked();

    color: isHighlighted ? Colors.defaultLightColor : Colors.selectedArtworkColor

    width: childrenRect.width
    height: childrenRect.height

    Row {
        spacing: 0

        Rectangle {
            id: tagTextRect
            width: childrenRect.width + 5
            height: itemHeight
            color: "transparent"

            StyledText {
                anchors.left: parent.left
                anchors.leftMargin: 5 + (settingsModel.keywordSizeScale - 1)*10
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                verticalAlignment: Text.AlignVCenter
                text: itemWrapper.keywordText
                color: itemWrapper.isHighlighted ? Colors.defaultControlColor : Colors.defaultLightColor
                font.pixelSize: 12 * settingsModel.keywordSizeScale
            }
        }

        Item {
            height: 20 * settingsModel.keywordSizeScale + (settingsModel.keywordSizeScale - 1)*10
            width: height

            CloseIcon {
                isPlus: itemWrapper.hasPlusSign
                width: 14*settingsModel.keywordSizeScale
                height: 14*settingsModel.keywordSizeScale
                isActive: itemWrapper.isHighlighted
                anchors.centerIn: parent
                onItemClicked: actionClicked()
            }
        }
    }
}
