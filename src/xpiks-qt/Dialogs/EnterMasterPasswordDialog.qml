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
    id: enterPasswordComponent
    property bool wrongTry: false
    property var callbackObject
    anchors.fill: parent

    signal dialogDestruction();
    Component.onDestruction: dialogDestruction();

    function closePopup() {
        enterPasswordComponent.destroy()
    }

    function testPassword() {
        var mp = masterPassword.text
        if (secretsManager.testMasterPassword(mp)) {
            secretsManager.setMasterPassword(mp)
            callbackObject.onSuccess()
            closePopup()
        } else {
            wrongTry = true
        }
    }

    PropertyAnimation { target: enterPasswordComponent; property: "opacity";
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
            onDoubleClicked: mouse.accepted = true

            property real old_x : 0
            property real old_y : 0

            onPressed:{
                var tmp = mapToItem(enterPasswordComponent, mouse.x, mouse.y);
                old_x = tmp.x;
                old_y = tmp.y;
            }

            onPositionChanged: {
                var old_xy = Common.movePopupInsideComponent(enterPasswordComponent, dialogWindow, mouse, old_x, old_y);
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
            width: 260
            height: childrenRect.height + 40
            color: Colors.popupBackgroundColor
            anchors.centerIn: parent
            Component.onCompleted: anchors.centerIn = undefined

            ColumnLayout {
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                height: childrenRect.height
                anchors.margins: 20
                spacing: 20

                StyledText {
                    anchors.left: parent.left
                    text: i18.n + qsTr("Enter current Master Password:")
                }

                Rectangle {
                    width: 220
                    height: UIConfig.textInputHeight
                    anchors.left: parent.left
                    color: enabled ? Colors.inputBackgroundColor : Colors.inputInactiveBackground
                    border.width: masterPassword.activeFocus ? 1 : 0
                    border.color: wrongTry ? Colors.destructiveColor : Colors.artworkActiveColor
                    clip: true

                    StyledTextInput {
                        id: masterPassword
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.leftMargin: 5
                        anchors.rightMargin: 5
                        echoMode: showPasswordCheckBox.checked ? TextInput.Normal : TextInput.Password
                        anchors.verticalCenter: parent.verticalCenter

                        Keys.onBacktabPressed: {
                            event.accepted = true
                        }

                        Keys.onTabPressed: {
                            event.accepted = true
                        }

                        onAccepted: {
                            testPassword()
                        }
                    }
                }

                StyledCheckbox {
                    checked: false
                    anchors.left: parent.left
                    id: showPasswordCheckBox
                    text: i18.n + qsTr("Show password")
                }

                RowLayout {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: 24
                    spacing: 0

                    StyledButton {
                        text: i18.n + qsTr("Ok")
                        width: 90
                        onClicked: testPassword()
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    StyledButton {
                        text: i18.n + qsTr("Cancel")
                        tooltip: i18.n + qsTr("This will leave password fields blank")
                        width: 90
                        onClicked: {
                            callbackObject.onFail()
                            closePopup()
                        }
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        focus = true
        masterPassword.forceActiveFocus()
    }
}
