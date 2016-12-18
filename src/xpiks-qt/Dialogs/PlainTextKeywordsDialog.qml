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
import QtQuick.Dialogs 1.1
import QtQuick.Controls.Styles 1.1
import QtGraphicalEffects 1.0
import xpiks 1.0
import "../Constants"
import "../Common.js" as Common;
import "../Components"
import "../StyledControls"
import "../Constants/UIConfig.js" as UIConfig

Item {
    id: plainTextKeywordsComponent
    property string keywordsText
    property var callbackObject
    property var keywordsModel
    anchors.fill: parent

    signal dialogDestruction();
    Component.onDestruction: dialogDestruction();

    function closePopup() {
        callbackObject.onClose()
        plainTextKeywordsComponent.destroy()
    }

    Component.onCompleted: {
        focus = true
        textEdit.forceActiveFocus()
    }

    Connections {
        target: keywordsModel
        onAboutToBeRemoved: {
            closePopup()
        }
    }

    Keys.onEscapePressed: closePopup()

    function scrollToBottom() {
        var flickable = flick
        if (flickable.contentHeight > flickable.height) {
            flickable.contentY = flickable.contentHeight - flickable.height
        } else {
            flickable.contentY = 0
        }
    }

    function submitKeywords() {
        callbackObject.onSuccess(textEdit.text)
        closePopup()
    }

    PropertyAnimation { target: plainTextKeywordsComponent; property: "opacity";
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
                var tmp = mapToItem(plainTextKeywordsComponent, mouse.x, mouse.y);
                old_x = tmp.x;
                old_y = tmp.y;
            }

            /*
              TODO: fix when user moves window through flickable
            onPositionChanged: {
                var old_xy = Common.movePopupInsideComponent(plainTextKeywordsComponent, dialogWindow, mouse, old_x, old_y);
                old_x = old_xy[0]; old_y = old_xy[1];
            }*/
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
            width: 600
            height: 400
            color: Colors.popupBackgroundColor
            anchors.centerIn: parent
            Component.onCompleted: anchors.centerIn = undefined

            StyledText {
                id: header
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.topMargin: 20
                anchors.leftMargin: 20
                text: i18.n + qsTr("Keywords")
            }

            Rectangle {
                anchors.top: header.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: 20
                anchors.rightMargin: 20
                anchors.topMargin: 10
                anchors.bottom: footer.top
                anchors.bottomMargin: 20
                color: Colors.popupDarkInputBackground

                Flickable {
                    id: flick
                    anchors.fill: parent
                    anchors.margins: 10
                    clip: true
                    contentWidth: textEdit.paintedWidth
                    contentHeight: textEdit.paintedHeight

                    function ensureVisible(r) {
                        if (contentX >= r.x)
                            contentX = r.x;
                        else if (contentX+width <= r.x+r.width)
                            contentX = r.x+r.width-width;
                        if (contentY >= r.y)
                            contentY = r.y;
                        else if (contentY+height <= r.y+r.height)
                            contentY = r.y+r.height-height;
                    }

                    StyledTextEdit {
                        id: textEdit
                        anchors.top: parent.top
                        width: flick.width - 10
                        height: flick.height
                        focus: true
                        text: plainTextKeywordsComponent.keywordsText
                        font.pixelSize: UIConfig.fontPixelSize*settingsModel.keywordSizeScale
                        selectionColor: Colors.inputBackgroundColor
                        wrapMode: TextEdit.Wrap
                        horizontalAlignment: TextEdit.AlignLeft
                        verticalAlignment: TextEdit.AlignTop
                        textFormat: TextEdit.PlainText

                        Component.onCompleted: {
                            scrollToBottom()
                            textEdit.cursorPosition += plainTextKeywordsComponent.keywordsText.length
                        }

                        Keys.onPressed: {
                            if(event.matches(StandardKey.Paste)) {
                                var clipboardText = clipboard.getText();
                                if (Common.safeInsert(textEdit, clipboardText)) {
                                    event.accepted = true
                                }
                            } else if ((event.key === Qt.Key_Return) || (event.key === Qt.Key_Enter)) {
                                if (event.modifiers === Qt.ControlModifier) {
                                    submitKeywords()
                                } else {
                                    event.accepted = true
                                }
                            }
                        }

                        Keys.onBacktabPressed: {
                            event.accepted = true
                        }

                        Keys.onTabPressed: {
                            event.accepted = true
                        }

                        onCursorRectangleChanged: flick.ensureVisible(cursorRectangle)
                    }
                }

                CustomScrollbar {
                    anchors.topMargin: -5
                    anchors.bottomMargin: -5
                    anchors.rightMargin: -5
                    flickable: flick
                }
            }

            RowLayout {
                id: footer
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 20
                anchors.left: parent.left
                anchors.leftMargin: 20
                anchors.right: parent.right
                anchors.rightMargin: 20
                height: 24
                spacing: 20

                Item {
                    Layout.fillWidth: true
                }

                StyledButton {
                    id: okButton
                    text: i18.n + qsTr("Save")
                    width: 100
                    onClicked: submitKeywords()
                }

                StyledButton {
                    text: i18.n + qsTr("Cancel")
                    width: 100
                    onClicked: closePopup()
                }
            }
        }
    }

    ClipboardHelper {
        id: clipboard
    }
}
