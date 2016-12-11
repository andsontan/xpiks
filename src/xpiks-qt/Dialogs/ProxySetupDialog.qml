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
import "../Common.js" as Common
import "../Components"
import "../StyledControls"
import "../Constants/UIConfig.js" as UIConfig

Item {
    id: proxySetupComponent
    property bool firstTime
    property var callbackObject
    anchors.fill: parent

    function closePopup() {
        proxySetupComponent.destroy()
    }

    function saveProxySettings() {
        settingsModel.saveProxySetting(proxyAddress.text,
                                       proxyUser.text,
                                       proxyPassword.text,
                                       proxyPort.text)
    }

    MessageDialog {
        id: invalidAddress
        title: i18.n + qsTr("Warning")
        text: i18.n + qsTr("Please, enter proxy address")
        standardButtons: StandardButton.Ok
    }

    PropertyAnimation {
        target: proxySetupComponent
        property: "opacity"
        duration: 400
        from: 0
        to: 1
        easing.type: Easing.InOutQuad
        running: true
    }

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
            onDoubleClicked: mouse.accepted = true

            property real old_x: 0
            property real old_y: 0

            onPressed: {
                var tmp = mapToItem(proxySetupComponent, mouse.x, mouse.y)
                old_x = tmp.x
                old_y = tmp.y
            }

            onPositionChanged: {
                var old_xy = Common.movePopupInsideComponent(
                            proxySetupComponent, dialogWindow, mouse,
                            old_x, old_y)
                old_x = old_xy[0]
                old_y = old_xy[1]
            }
        }

        RectangularGlow {
            anchors.fill: dialogWindow
            anchors.topMargin: glowRadius / 2
            anchors.bottomMargin: -glowRadius / 2
            glowRadius: 4
            spread: 0.0
            color: Colors.defaultControlColor
            cornerRadius: glowRadius
        }

        // This rectangle is the actual popup
        Rectangle {
            id: dialogWindow
            width: 280
            height: 280
            color: Colors.popupBackgroundColor
            anchors.centerIn: parent
            Component.onCompleted: anchors.centerIn = undefined

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 20

                StyledText {
                    anchors.left: parent.left
                    text: i18.n + qsTr("Proxy settings:")
                }

                RowLayout {
                    id: proxyAddressRow
                    width: parent.width
                    spacing: 10

                    StyledText {
                        Layout.fillWidth: true
                        horizontalAlignment: Text.AlignRight
                        text: i18.n + qsTr("Hostname:")
                    }

                    Rectangle {
                        color: enabled ? Colors.inputBackgroundColor : Colors.inputInactiveBackground
                        border.width: (proxyAddress.activeFocus) ? 1 : 0
                        border.color: Colors.artworkActiveColor
                        width: 135
                        height: UIConfig.textInputHeight
                        clip: true

                        StyledTextInput {
                            id: proxyAddress
                            text: settingsModel.proxyAddress
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.leftMargin: 5
                            anchors.rightMargin: 5
                            anchors.verticalCenter: parent.verticalCenter
                            KeyNavigation.tab: proxyUser
                        }
                    }
                }
                RowLayout {
                    id: proxyUserRow
                    width: parent.width
                    height: 20
                    spacing: 10

                    StyledText {
                        Layout.fillWidth: true
                        horizontalAlignment: Text.AlignRight
                        text: i18.n + qsTr("Username:")
                    }

                    Rectangle {
                        color: enabled ? Colors.inputBackgroundColor : Colors.inputInactiveBackground
                        border.width: (proxyUser.activeFocus) ? 1 : 0
                        border.color: Colors.artworkActiveColor
                        width: 135
                        height: UIConfig.textInputHeight
                        clip: true

                        StyledTextInput {
                            id: proxyUser
                            text: settingsModel.proxyUser
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.leftMargin: 5
                            anchors.rightMargin: 5
                            anchors.verticalCenter: parent.verticalCenter
                            KeyNavigation.backtab: proxyAddress
                            KeyNavigation.tab: proxyPassword
                        }
                    }
                }

                RowLayout {
                    id: proxyPasswordRow
                    width: parent.width
                    height: 20
                    spacing: 10

                    StyledText {
                        Layout.fillWidth: true
                        horizontalAlignment: Text.AlignRight
                        text: i18.n + qsTr("Password:")
                    }

                    Rectangle {
                        color: enabled ? Colors.inputBackgroundColor : Colors.inputInactiveBackground
                        border.width: (proxyPassword.activeFocus) ? 1 : 0
                        border.color: Colors.artworkActiveColor
                        width: 135
                        height: UIConfig.textInputHeight
                        clip: true

                        StyledTextInput {
                            id: proxyPassword
                            text: settingsModel.proxyPassword
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.leftMargin: 5
                            anchors.rightMargin: 5
                            anchors.verticalCenter: parent.verticalCenter
                            echoMode: TextInput.Password
                            KeyNavigation.backtab: proxyUser
                            KeyNavigation.tab: proxyPort
                        }
                    }
                }

                RowLayout {
                    id: proxyPortRow
                    width: parent.width
                    height: 20
                    spacing: 10

                    StyledText {
                        Layout.fillWidth: true
                        horizontalAlignment: Text.AlignRight
                        text: i18.n + qsTr("Port:")
                    }

                    Rectangle {
                        color: enabled ? Colors.inputBackgroundColor : Colors.inputInactiveBackground
                        border.width: (proxyPort.activeFocus) ? 1 : 0
                        border.color: Colors.artworkActiveColor
                        width: 135
                        height: UIConfig.textInputHeight
                        clip: true

                        StyledTextInput {
                            id: proxyPort
                            text: settingsModel.proxyPort
                            validator: IntValidator {
                                bottom: 0
                                top: 65536
                            }
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.leftMargin: 5
                            anchors.rightMargin: 5
                            anchors.verticalCenter: parent.verticalCenter
                            KeyNavigation.backtab: proxyPassword
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
                        onClicked: {
                            if (proxyAddress.text.length == 0){
                                invalidAddress.open()
                            } else {
                                saveProxySettings()
                                callbackObject.onSuccess()
                                closePopup()
                            }
                        }
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
        proxyAddress.forceActiveFocus()
        proxyAddress.cursorPosition = proxyAddress.text.length
    }
}
