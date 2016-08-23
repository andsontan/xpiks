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
    property bool isHighlighted
    property bool hasSpellCheckError: false
    property string keywordText
    property bool hasPlusSign: false
    property alias itemHeight: tagTextRect.height

    signal removeClicked();
    signal actionDoubleClicked();
    signal actionRightClicked();
    signal spellSuggestionRequested();

    color: isHighlighted ? Colors.inputForegroundColor : Colors.labelInactiveForeground

    width: childrenRect.width
    height: childrenRect.height

    Row {
        id: row
        spacing: 0

        Item {
            id: tagTextRect
            width: childrenRect.width + 5
            height: itemHeight

            StyledText {
                id: keywordText
                anchors.left: parent.left
                anchors.leftMargin: 5 + (settingsModel.keywordSizeScale - 1)*10
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                verticalAlignment: Text.AlignVCenter
                text: itemWrapper.keywordText
                color: itemWrapper.isHighlighted ? Colors.defaultControlColor : Colors.inputForegroundColor
                font.pixelSize: UIConfig.fontPixelSize * settingsModel.keywordSizeScale
            }

            MouseArea {
                anchors.fill: keywordText
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                onDoubleClicked: actionDoubleClicked()
                onClicked: {
                    if (mouse.button == Qt.RightButton) {
                        console.log("Click in keyword registered")
                        actionRightClicked()
                    }
                }

                preventStealing: true
                propagateComposedEvents: true
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
                onItemClicked: removeClicked()
            }
        }
    }

    Rectangle {
        height: 1.5
        anchors.left: row.left
        anchors.right: row.right
        anchors.bottom: row.bottom
        color: Colors.destructiveColor
        visible: itemWrapper.hasSpellCheckError
    }
}
