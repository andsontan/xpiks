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
import QtQuick.Dialogs 1.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import "../Constants"
import "../Common.js" as Common;
import "../Components"
import "../StyledControls"
import "../Constants/UIConfig.js" as UIConfig

Item {
    id: masterPasswordComponent
    property bool firstTime
    property bool emptyMP: false
    property bool wrongMP: false
    anchors.fill: parent
    property var callbackObject

    function closePopup() {
        secretsManager.purgeMasterPassword()
        masterPasswordComponent.destroy()
    }

    function trySetupMP() {
        if (!firstTime && currentPassword.length == 0) {
            currentPassword.forceActiveFocus()
            emptyMP = true
            return
        }

        if (repeatMasterPassword.text == newMasterPassword.text) {

            if (repeatMasterPassword.length == 0) {
                callbackObject.onCancel(firstTime)
                closePopup()
            }

            if (secretsManager.changeMasterPassword(firstTime,
                                                    currentPassword.text,
                                                    newMasterPassword.text)) {
                callbackObject.onSuccess()
                closePopup()
            } else {
                wrongCurrentMPDialog.open()
            }
        } else {
            notEqualPasswordsDialog.open()
        }
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

    MessageDialog {
        id: notEqualPasswordsDialog
        title: i18.n + qsTr("Warning")
        text: i18.n + qsTr("Passwords are not equal")
        standardButtons: StandardButton.Ok
    }

    MessageDialog {
        id: wrongCurrentMPDialog
        title: i18.n + qsTr("Warning")
        text: i18.n + qsTr("Please, enter correct current master password")
        standardButtons: StandardButton.Ok
        onAccepted: {
            currentPassword.forceActiveFocus()
            wrongMP = true
        }
    }

    FocusScope {
        anchors.fill: parent

        MouseArea {
            anchors.fill: parent
            onWheel: wheel.accepted = true
            onClicked: mouse.accepted = true
            onDoubleClicked: mouse.accepted = true

            property real old_x : 0
            property real old_y : 0

            onPressed:{
                var tmp = mapToItem(masterPasswordComponent, mouse.x, mouse.y);
                old_x = tmp.x;
                old_y = tmp.y;
            }

            onPositionChanged: {
                var old_xy = Common.movePopupInsideComponent(masterPasswordComponent, dialogWindow, mouse, old_x, old_y);
                old_x = old_xy[0]; old_y = old_xy[1];
            }
        }

        RectangularGlow {
            anchors.fill: dialogWindow
            anchors.topMargin: glowRadius/2
            anchors.bottomMargin: -glowRadius/2
            glowRadius: 4
            spread: 0.0
            color: Colors.defaultControlColor
            cornerRadius: glowRadius
        }

        // This rectangle is the actual popup
        Rectangle {
            id: dialogWindow
            width: 360
            height: 220
            color: Colors.popupBackgroundColor
            anchors.centerIn: parent
            Component.onCompleted: anchors.centerIn = undefined

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 10

                RowLayout {
                    id: currentPasswordRow
                    width: parent.width
                    height: 20
                    enabled: !firstTime
                    spacing: 10

                    StyledText {
                        Layout.fillWidth: true
                        Layout.preferredWidth: 130
                        horizontalAlignment: Text.AlignRight
                        text: i18.n + qsTr("Current Master Password:")
                    }

                    Rectangle {
                        color: enabled ? Colors.inputBackgroundColor : Colors.inputInactiveBackground
                        border.width: (currentPassword.activeFocus || emptyMP) ? 1 : 0
                        border.color: (emptyMP || wrongMP) ? Colors.artworkModifiedColor : Colors.artworkActiveColor
                        width: 135
                        height: UIConfig.textInputHeight
                        clip: true

                        StyledTextInput {
                            id: currentPassword
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.leftMargin: 5
                            anchors.rightMargin: 5
                            echoMode: TextInput.Password
                            anchors.verticalCenter: parent.verticalCenter

                            Keys.onBacktabPressed: {
                                event.accepted = true
                            }

                            KeyNavigation.tab: newMasterPassword
                            onAccepted: {
                                if (repeatMasterPassword.text != newMasterPassword.text) {
                                    newMasterPassword.forceActiveFocus()
                                } else {
                                    trySetupMP()
                                }
                            }
                        }
                    }
                }

                RowLayout {
                    width: parent.width
                    height: 20
                    spacing: 10

                    StyledText {
                        Layout.fillWidth: true
                        Layout.preferredWidth: 130
                        horizontalAlignment: Text.AlignRight
                        text: i18.n + qsTr("New Master Password:")
                    }

                    Rectangle {
                        color: enabled ? Colors.inputBackgroundColor : Colors.inputInactiveBackground
                        border.width: newMasterPassword.activeFocus ? 1 : 0
                        border.color: Colors.artworkActiveColor
                        width: 135
                        height: UIConfig.textInputHeight
                        clip: true

                        StyledTextInput {
                            id: newMasterPassword
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.leftMargin: 5
                            anchors.rightMargin: 5
                            echoMode: TextInput.Password
                            KeyNavigation.backtab: currentPassword
                            KeyNavigation.tab: repeatMasterPassword
                            anchors.verticalCenter: parent.verticalCenter

                            onAccepted: {
                                if (repeatMasterPassword.text != newMasterPassword.text) {
                                    repeatMasterPassword.forceActiveFocus()
                                } else {
                                    trySetupMP()
                                }
                            }
                        }
                    }
                }

                RowLayout {
                    width: parent.width
                    height: 20
                    spacing: 10

                    StyledText {
                        Layout.fillWidth: true
                        Layout.preferredWidth: 130
                        horizontalAlignment: Text.AlignRight
                        text: i18.n + qsTr("Repeat Master Password:")
                    }

                    Rectangle {
                        border.width: enabled ? ((repeatMasterPassword.activeFocus || newMasterPassword.length > 0) ? 1 : 0) : 0
                        border.color: repeatMasterPassword.text == newMasterPassword.text ? Colors.artworkActiveColor : Colors.artworkModifiedColor
                        color: enabled ? Colors.inputBackgroundColor : Colors.inputInactiveBackground
                        width: 135
                        height: UIConfig.textInputHeight
                        clip: true

                        StyledTextInput {
                            id: repeatMasterPassword
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.leftMargin: 5
                            anchors.rightMargin: 5
                            anchors.verticalCenter: parent.verticalCenter
                            echoMode: TextInput.Password
                            KeyNavigation.backtab: newMasterPassword
                            onAccepted: trySetupMP()
                        }
                    }
                }

                RowLayout {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: 24
                    spacing: 20

                    Item {
                        Layout.fillWidth: true
                    }

                    StyledButton {
                        text: i18.n + qsTr("Ok")
                        width: 80
                        onClicked: trySetupMP()
                    }

                    StyledButton {
                        text: i18.n + qsTr("Cancel")
                        width: 80
                        onClicked: {
                            callbackObject.onCancel(firstTime)
                            closePopup()
                        }
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        focus = true
        if (firstTime) {
            newMasterPassword.forceActiveFocus()
        } else {
            currentPassword.forceActiveFocus()
        }
    }
}

