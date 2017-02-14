/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
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
import QtQuick.Layouts 1.1
import "../Constants" 1.0
import "../Components"
import "../StyledControls"
import "../Dialogs"
import "../Common.js" as Common
import "../Constants/UIConfig.js" as UIConfig

ColumnLayout {
    anchors.fill: parent
    anchors.leftMargin: 10
    anchors.rightMargin: 10
    anchors.topMargin: 20
    anchors.bottomMargin: 20
    spacing: 0

    function initializeTab() {
        dictionariesComboBox.selectedIndex = translationManager.selectedDictionaryIndex
        trTextEdit.forceActiveFocus()
    }

    StyledText {
        text: i18.n + qsTr("Dictionary:")
    }

    Item {
        height: 10
    }

    CustomComboBox {
        id: dictionariesComboBox
        model: translationManager.dictionaries
        anchors.left: parent.left
        anchors.right: parent.right
        height: 28
        itemHeight: 28
        showColorSign: true
        hasLastItemAction: true
        lastActionText: i18.n + qsTr("Add dictionary...")
        comboboxBackgroundColor: Colors.popupBackgroundColor
        z: 100500

        onLastItemActionInvoked: {
            openDictionaryDialog.open()
        }

        onComboIndexChanged: {
            translationManager.selectedDictionaryIndex = dictionariesComboBox.selectedIndex
            trTextEdit.forceActiveFocus()
            trTextEdit.cursorPosition = trTextEdit.text.length
        }
    }

    Item {
        height: 20
    }

    Rectangle {
        anchors.left: parent.left
        anchors.right: parent.right
        height: 30
        color: Colors.popupDarkInputBackground

        StyledTextEdit {
            id: trTextEdit
            width: parent.width - 10
            height: parent.height
            clip: true
            anchors.left: parent.left
            anchors.leftMargin: 5
            focus: true
            text: translationManager.query
            onTextChanged: translationManager.query = text
            isActive: false
            selectionColor: Colors.inputBackgroundColor

            Keys.onBacktabPressed: {
                event.accepted = true
            }

            Keys.onTabPressed: {
                event.accepted = true
            }

            Keys.onPressed: {
                if(event.matches(StandardKey.Paste)) {
                    var clipboardText = clipboard.getText();
                    if (Common.safeInsert(trTextEdit, clipboardText)) {
                        event.accepted = true
                    }
                } else if ((event.key === Qt.Key_Return) || (event.key === Qt.Key_Enter)) {
                    event.accepted = true
                }
            }
        }

        CloseIcon {
            width: 14
            height: 14
            anchors.right: parent.right
            anchors.rightMargin: 5
            enabled: trTextEdit.length > 0
            anchors.verticalCenter: parent.verticalCenter
            onItemClicked: {
                translationManager.clear()
                trTextEdit.forceActiveFocus()
            }
        }
    }

    Item {
        height: 20
    }

    Rectangle {
        anchors.left: parent.left
        anchors.right: parent.right
        Layout.fillHeight: true
        color: Colors.popupDarkInputBackground

        Flickable {
            id: trFlickOther
            anchors.fill: parent
            anchors.margins: 10
            clip: true
            contentWidth: trTextEdit.paintedWidth
            contentHeight: trTextEdit.paintedHeight

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
                id: trTextEditOther
                anchors.top: parent.top
                width: trFlickOther.width - 10
                height: trFlickOther.height
                focus: true
                readOnly: true
                text: translationManager.shortTranslation
                selectionColor: Colors.inputBackgroundColor
                wrapMode: TextEdit.Wrap
                horizontalAlignment: TextEdit.AlignLeft
                verticalAlignment: TextEdit.AlignTop
                textFormat: TextEdit.RichText
                isActive: false

                Component.onCompleted: {
                    // scrollToBottom()
                }

                Keys.onBacktabPressed: {
                    event.accepted = true
                }

                Keys.onTabPressed: {
                    event.accepted = true
                }
            }
        }

        CustomScrollbar {
            anchors.topMargin: -5
            anchors.bottomMargin: -5
            anchors.rightMargin: -5
            flickable: trFlickOther
        }

        Rectangle {
            anchors.fill: parent
            color: Colors.selectedArtworkBackground
            opacity: 0.2
            visible: translationManager.isBusy
        }

        StyledBusyIndicator {
            width: parent.width/2
            height: parent.width/2
            anchors.centerIn: parent
            running: translationManager.isBusy
        }
    }

    Item {
        height: 10
    }

    Item {
        anchors.left: parent.left
        anchors.right: parent.right
        height: childrenRect.height

        StyledText {
            anchors.right: parent.right
            text: i18.n + qsTr("Show more")
            enabled: translationManager.hasMore
            color: enabled ? (showMoreMA.pressed ? Colors.linkClickedColor : Colors.artworkActiveColor) : Colors.labelInactiveForeground

            MouseArea {
                id: showMoreMA
                anchors.fill: parent
                cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor

                onClicked: {
                    Common.launchDialog("Dialogs/TranslationPreviewDialog.qml",
                                        applicationWindow, {})
                }
            }
        }
    }
}
