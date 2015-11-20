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
import QtQuick.Dialogs 1.1
import QtQuick.Controls.Styles 1.1
import "../Constants"
import "../Constants/Colors.js" as Colors;
import "../Common.js" as Common;
import "../Components"
import "../StyledControls"

Item {
    id: spellCheckDialog
    anchors.fill: parent
    property var callbackObject
    property bool initialized: false

    signal dialogDestruction();
    Component.onDestruction: dialogDestruction();

    Keys.onEscapePressed: {
        if (!spinnerTimer.running) {
            spellCheckerService.cancelCurrentBatch()
        }
    }

    function closePopup() {
        spellCheckDialog.destroy();
    }

    PropertyAnimation { target: spellCheckDialog; property: "opacity";
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

    Connections {
        target: spellCheckerService
        onSpellCheckQueueIsEmpty: {
            spinnerTimer.start()
        }
    }

    FocusScope {
        anchors.fill: parent

        MouseArea {
            anchors.fill: parent
            onWheel: wheel.accepted = true
            onClicked: mouse.accepted = true
            onDoubleClicked: mouse.accepted = true
        }

        // This rectangle is the actual popup
        Rectangle {
            id: dialogWindow
            width: 500
            height: 400
            color: Colors.selectedArtworkColor
            anchors.centerIn: parent
            Component.onCompleted: anchors.centerIn = undefined

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 0

                StyledText {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: qsTr("Checking selected items...")
                }

                StyledBusyIndicator {
                    id: spinner
                    width: 300
                    height: 300
                    anchors.horizontalCenter: parent.horizontalCenter
                    running: true
                }

                StyledButton {
                    text: qsTr("Cancel")
                    width: 100
                    anchors.horizontalCenter: parent.horizontalCenter
                    onClicked: {
                        if (!spinnerTimer.running) {
                            spellCheckerService.cancelCurrentBatch()
                        }
                    }
                }
            }
        }

        Timer {
            id: spinnerTimer
            property int iterations: 0
            interval: 1000
            repeat: false
            running: false
            onTriggered: {
                spinner.running = false
                closePopup()
            }
        }
    }
}
