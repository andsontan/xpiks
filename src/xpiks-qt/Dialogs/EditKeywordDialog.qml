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

Item {
    id: editKeywordComponent
    property var callbackObject
    property string previousKeyword: ''
    property var keywordsModel
    property int keywordIndex
    property bool anyError: false
    anchors.fill: parent

    signal dialogDestruction();
    Component.onDestruction: dialogDestruction();

    function submitKeyword() {
        if (anyError) { return; }

        var keyword = keywordInput.text
        if (helpersWrapper.isKeywordValid(keyword)) {
            callbackObject.onSuccess(keyword)
            closePopup()
        }
    }

    Connections {
        target: keywordsModel
        onAboutToBeRemoved: {
            console.info("About to be removed")
            closePopup()
        }
    }

    Keys.onEscapePressed: closePopup()

    function closePopup() {
        callbackObject.onClose()
        editKeywordComponent.destroy()
    }

    PropertyAnimation { target: editKeywordComponent; property: "opacity";
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
                var tmp = mapToItem(editKeywordComponent, mouse.x, mouse.y);
                old_x = tmp.x;
                old_y = tmp.y;
            }

            onPositionChanged: {
                var old_xy = Common.movePopupInsideComponent(editKeywordComponent, dialogWindow, mouse, old_x, old_y);
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
            width: 240
            height: childrenRect.height + 40
            color: Colors.selectedImageBackground
            anchors.centerIn: parent
            Component.onCompleted: anchors.centerIn = undefined

            Column {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                height: childrenRect.height
                anchors.margins: 20
                spacing: 20

                Rectangle {
                    color: enabled ? Colors.inputBackgroundColor : Colors.inputInactiveBackground
                    border.width: keywordInput.activeFocus ? 1 : 0
                    border.color: editKeywordComponent.anyError ? Colors.artworkModifiedColor : Colors.artworkActiveColor
                    width: 200
                    height: 30
                    clip: true

                    StyledTextInput {
                        id: keywordInput
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.leftMargin: 5
                        anchors.rightMargin: 5
                        anchors.verticalCenter: parent.verticalCenter
                        onAccepted: submitKeyword()
                        Component.onCompleted: keywordInput.text = previousKeyword

                        Keys.onBacktabPressed: {
                            event.accepted = true
                        }

                        Keys.onTabPressed: {
                            event.accepted = true
                        }

                        Keys.onPressed: {
                            if (event.key === Qt.Key_Comma) {
                                event.accepted = true
                            }
                        }

                        onTextChanged: {
                            editKeywordComponent.anyError = !keywordsModel.canEditKeyword(keywordIndex, text) &&
                                    (previousKeyword !== text.trim());
                        }
                    }
                }

                RowLayout {
                    width: parent.width
                    height: 20
                    spacing: 20

                    StyledButton {
                        text: i18.n + qsTr("Save")
                        width: 90
                        enabled: !editKeywordComponent.anyError
                        onClicked: submitKeyword()
                    }

                    StyledButton {
                        text: i18.n + qsTr("Cancel")
                        width: 90
                        onClicked: closePopup()
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        focus = true
        keywordInput.forceActiveFocus()
    }
}
