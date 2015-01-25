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
import "../Components"
import "../StyledControls"

Item {
    id: logsComponent
    property string logText
    anchors.fill: parent

    function closePopup() {
        logsComponent.destroy()
    }

    PropertyAnimation { target: logsComponent; property: "opacity";
        duration: 400; from: 0; to: 1;
        easing.type: Easing.InOutQuad ; running: true }

    // This rectange is the a overlay to partially show the parent through it
    // and clicking outside of the 'dialog' popup will do 'nothing'
    Rectangle {
        anchors.fill: parent
        id: overlay
        color: "#000000"
        opacity: 0.6
        // add a mouse area so that clicks outside
        // the dialog window will not do anything
        MouseArea {
            anchors.fill: parent
        }
    }

    MessageDialog {
        id: confirmClearLogsDialog
        property int itemIndex
        title: "Confirmation"
        text: qsTr("Are you sure you want to clear logs?")
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            logsModel.clearLogs()
            logsComponent.logText = logsModel.getAllLogsText()
        }
    }

    FocusScope {
        anchors.fill: parent

        MouseArea {
            anchors.fill: parent
            onWheel: wheel.accepted = true
            onClicked: mouse.accepted = true

            property real old_x : 0
            property real old_y : 0

            onPressed:{
                //            var tmp = root.mapToItem(img,mouse.x,mouse.y);
                var tmp = mapToItem(logsComponent, mouse.x, mouse.y);
                old_x = tmp.x;
                old_y = tmp.y;
            }

            onPositionChanged: {
                //            var tmp = root.mapToItem(img,mouse.x,mouse.y);
                var tmp = mapToItem(logsComponent, mouse.x, mouse.y);
                var delta_x = tmp.x - old_x;
                var delta_y = tmp.y - old_y;
                dialogWindow.x += delta_x;
                dialogWindow.y += delta_y;
                old_x = tmp.x;
                old_y = tmp.y;
            }
        }

        // This rectangle is the actual popup
        Rectangle {
            id: dialogWindow
            width: 480
            height: 580
            color: Colors.selectedArtworkColor
            anchors.centerIn: parent
            Component.onCompleted: anchors.centerIn = undefined

            ColumnLayout {
                spacing: 10
                anchors.fill: parent
                anchors.margins: 20

                StyledText {
                    text: qsTr("Logs")
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: Colors.defaultControlColor

                    StyledScrollView {
                        anchors.fill: parent
                        anchors.margins: 10

                        StyledTextEdit {
                            text: logsComponent.logText
                            readOnly: true
                        }
                    }
                }

                Item {
                    height: 1
                }

                RowLayout {
                    height: 24

                    Item {
                        Layout.fillWidth: true
                    }

                    StyledButton {
                        text: qsTr("Clear logs")
                        width: 100
                        onClicked: {
                            confirmClearLogsDialog.open()
                        }
                    }

                    StyledButton {
                        text: qsTr("Close")
                        width: 100
                        onClicked: {
                            closePopup()
                        }
                    }
                }
            }
        }
    }
}
