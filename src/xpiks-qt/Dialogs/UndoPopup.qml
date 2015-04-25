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
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Styles 1.3
import "../Constants"
import "../Constants/Colors.js" as Colors;
import "../Common.js" as Common;
import "../Components"
import "../StyledControls"

Item {
    id: undoPopupComponent
    anchors.fill: parent
    anchors.topMargin: 20

    property string undoMessage

    function closePopup() {
        undoPopupComponent.destroy()
    }

    PropertyAnimation { target: undoPopupComponent; property: "opacity";
        duration: 400; from: 0; to: 1;
        easing.type: Easing.InOutQuad ; running: true }

    // This rectange is the a overlay to partially show the parent through it
    // and clicking outside of the 'dialog' popup will do 'nothing'
    /*Rectangle {
        anchors.fill: parent
        id: overlay
        color: "#000000"
        opacity: 0.6
        // add a mouse area so that clicks outside
        // the dialog window will not do anything
        MouseArea {
            anchors.fill: parent
        }
    }*/

        /*MouseArea {
            anchors.fill: parent
            onWheel: wheel.accepted = true
            onClicked: mouse.accepted = true

            property real old_x : 0
            property real old_y : 0

            onPressed:{
                var tmp = mapToItem(undoPopupComponent, mouse.x, mouse.y);
                old_x = tmp.x;
                old_y = tmp.y;
            }

            onPositionChanged: {
                var old_xy = Common.movePopupInsideComponent(undoPopupComponent, dialogWindow, mouse, old_x, old_y);
                old_x = old_xy[0]; old_y = old_xy[1];
            }
        }*/

    // This rectangle is the actual popup
    Rectangle {
        id: dialogWindow
        width: 200
        height: 30
        color: Colors.defaultInputBackground
        border.color: Colors.defaultLightGrayColor
        anchors.horizontalCenter: parent.horizontalCenter
        Component.onCompleted: anchors.centerIn = undefined
        radius: 8

        Row {
            anchors.centerIn: parent
            spacing: 5

            Item {
                width: 5
            }

            StyledText {
                text: undoMessage
                color: Colors.selectedMetadataColor
                anchors.verticalCenter: parent.verticalCenter
                anchors.leftMargin: 10
                font.bold: true
            }

            StyledText {
                text: qsTr("Undo")
                color: undoMA.pressed ? Colors.selectedMetadataColor : Colors.artworkActiveColor

                MouseArea {
                    id: undoMA
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        undoRedoManager.undoLastAction()
                        closePopup()
                    }
                }
            }
        }
    }
}
