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
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import "../Constants"
import "../Constants/Colors.js" as Colors;
import "../Common.js" as Common;
import "../Components"
import "../StyledControls"

Item {
    id: masterPasswordComponent
    property bool wrongTry: false
    anchors.fill: parent

    function closePopup() {
        masterPasswordComponent.destroy()
        masterPasswordComponent.parent.doOpenUploadDialog()
    }

    PropertyAnimation { target: masterPasswordComponent; property: "opacity";
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
                var tmp = mapToItem(masterPasswordComponent, mouse.x, mouse.y);
                old_x = tmp.x;
                old_y = tmp.y;
            }

            onPositionChanged: {
                var old_xy = Common.movePopupInsideComponent(masterPasswordComponent, dialogWindow, mouse, old_x, old_y);
                old_x = old_xy[0]; old_y = old_xy[1];
            }
        }

        // This rectangle is the actual popup
        Rectangle {
            id: dialogWindow
            width: 300
            height: 80
            color: Colors.selectedArtworkColor
            anchors.centerIn: parent
            Component.onCompleted: anchors.centerIn = undefined

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 10

                RowLayout {
                    width: parent.width
                    height: 20
                    enabled: firstTime
                    spacing: 10

                    StyledText {
                        Layout.fillWidth: true
                        Layout.preferredWidth: 130
                        horizontalAlignment: Text.AlignRight
                        text: qsTr("Enter Master Password:")
                    }

                    StyledInputHost {
                        border.width: currentPassword.activeFocus ? 1 : 0
                        border.color: wrongTry ? Colors.destructiveColor : Colors.artworkActiveColor

                        StyledTextInput {
                            id: masterPassword
                            width: 120
                            height: 24
                            clip: true
                            anchors.left: parent.left
                            anchors.leftMargin: 5
                            echoMode: showPasswordCheckBox.checked ? TextInput.Normal : TextInput.Password
                        }
                    }
                }

                RowLayout {
                    width: parent.width
                    height: 20
                    spacing: 10

                    StyledCheckbox {
                        id: showPasswordCheckBox
                        text: qsTr("Show password")
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    StyledButton {
                        text: qsTr("Ok")
                        width: 57
                        onClicked: {
                            var mp = masterPassword.text
                            if (secretsManager.testMasterPassword(mp)) {
                                secretsManager.setMasterPassword(mp)
                                closePopup()
                            } else {
                                wrongTry = true
                            }
                        }
                    }

                    StyledButton {
                        text: qsTr("Cancel")
                        tooltip: qsTr("This will leave password fields blank")
                        width: 58
                        onClicked: closePopup()
                    }
                }
            }
        }
    }

    Component.onCompleted: masterPassword.forceActiveFocus()
}
