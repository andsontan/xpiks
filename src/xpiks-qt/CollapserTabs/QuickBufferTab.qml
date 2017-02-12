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

import QtQuick 2.0
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
    anchors.topMargin: 15
    anchors.bottomMargin: 10
    spacing: 0

    StyledBlackButton {
        anchors.left: parent.left
        anchors.right: parent.right
        text: i18.n + qsTr("Apply")
        height: 30
    }

    Item {
        height: 10
    }

    Item {
        height: childrenRect.height
        anchors.left: parent.left
        anchors.right: parent.right

        StyledText {
            anchors.left: parent.left
            text: i18.n + qsTr("Title:")
            isActive: false
        }

        StyledText {
            anchors.right: parent.right
            text: titleTextInput.length
        }
    }

    Item {
        height: 5
    }

    Rectangle {
        id: anotherRect
        anchors.left: parent.left
        anchors.right: parent.right
        height: 25
        color: Colors.inputBackgroundColor
        border.color: Colors.artworkActiveColor
        border.width: titleTextInput.activeFocus ? 1 : 0
        clip: true

        Flickable {
            id: titleFlick
            contentWidth: titleTextInput.paintedWidth
            contentHeight: titleTextInput.paintedHeight
            anchors.fill: parent
            anchors.margins: 5
            clip: true
            flickableDirection: Flickable.HorizontalFlick
            interactive: false
            focus: false

            function ensureVisible(r) {
                if (contentX >= r.x)
                    contentX = r.x;
                else if (contentX+width <= r.x+r.width)
                    contentX = r.x+r.width-width;
            }

            StyledTextEdit {
                id: titleTextInput
                focus: true
                width: titleFlick.width
                height: titleFlick.height
                text: quickBuffer.title
                userDictEnabled: true
                property string previousText: text
                onTextChanged: {
                    if (text.length > UIConfig.inputsMaxLength) {
                        var cursor = cursorPosition;
                        text = previousText;
                        if (cursor > text.length) {
                            cursorPosition = text.length;
                        } else {
                            cursorPosition = cursor-1;
                        }
                        console.info("Pasting cancelled: text length exceeded maximum")
                    }

                    previousText = text
                    quickBuffer.title = text
                }

                Keys.onBacktabPressed: {
                    event.accepted = true
                }

                Keys.onTabPressed: {
                    descriptionTextInput.forceActiveFocus()
                    descriptionTextInput.cursorPosition = descriptionTextInput.text.length
                }

                Component.onCompleted: {
                    quickBuffer.initTitleHighlighting(titleTextInput.textDocument)
                }

                onCursorRectangleChanged: titleFlick.ensureVisible(cursorRectangle)

                onActiveFocusChanged: quickBuffer.spellCheckTitle()

                Keys.onPressed: {
                    if(event.matches(StandardKey.Paste)) {
                        var clipboardText = clipboard.getText();
                        if (Common.safeInsert(titleTextInput, clipboardText)) {
                            event.accepted = true
                        }
                    } else if ((event.key === Qt.Key_Return) || (event.key === Qt.Key_Enter)) {
                        event.accepted = true
                    }
                }
            }
        }
    }

    Item {
        height: 10
    }

    Item {
        height: childrenRect.height
        anchors.left: parent.left
        anchors.right: parent.right

        StyledText {
            anchors.left: parent.left
            text: i18.n + qsTr("Description:")
        }

        StyledText {
            anchors.right: parent.right
            text: descriptionTextInput.length
        }
    }

    Item {
        height: 5
    }

    Rectangle {
        id: rect
        anchors.left: parent.left
        anchors.right: parent.right
        height: 70
        color: Colors.inputBackgroundColor
        border.color: Colors.artworkActiveColor
        border.width: descriptionTextInput.activeFocus ? 1 : 0
        clip: true

        Flickable {
            id: descriptionFlick
            contentWidth: descriptionTextInput.paintedWidth
            contentHeight: descriptionTextInput.paintedHeight
            anchors.fill: parent
            anchors.margins: 5
            interactive: false
            flickableDirection: Flickable.HorizontalFlick
            clip: true

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
                id: descriptionTextInput
                width: descriptionFlick.width
                height: descriptionFlick.height
                text: quickBuffer.description
                focus: true
                userDictEnabled: true
                property string previousText: text
                property int maximumLength: 280
                onTextChanged: {
                    if (text.length > UIConfig.inputsMaxLength) {
                        var cursor = cursorPosition;
                        text = previousText;
                        if (cursor > text.length) {
                            cursorPosition = text.length;
                        } else {
                            cursorPosition = cursor-1;
                        }
                        console.info("Pasting cancelled: text length exceeded maximum")
                    }

                    previousText = text
                    quickBuffer.description = text
                }

                wrapMode: TextEdit.Wrap
                horizontalAlignment: TextEdit.AlignLeft
                verticalAlignment: TextEdit.AlignTop
                textFormat: TextEdit.PlainText

                Component.onCompleted: {
                    quickBuffer.initDescriptionHighlighting(descriptionTextInput.textDocument)
                }

                Keys.onBacktabPressed: {
                    titleTextInput.forceActiveFocus()
                    titleTextInput.cursorPosition = titleTextInput.text.length
                }

                Keys.onTabPressed: {
                    flv.activateEdit()
                }

                onCursorRectangleChanged: descriptionFlick.ensureVisible(cursorRectangle)

                onActiveFocusChanged: quickBuffer.spellCheckDescription()

                Keys.onPressed: {
                    if(event.matches(StandardKey.Paste)) {
                        var clipboardText = clipboard.getText();
                        if (Common.safeInsert(descriptionTextInput, clipboardText)) {
                            event.accepted = true
                        }
                    } else if ((event.key === Qt.Key_Return) || (event.key === Qt.Key_Enter)) {
                        event.accepted = true
                    }
                }
            }
        }
    }

    Item {
        height: 10
    }

    Item {
        height: childrenRect.height
        anchors.left: parent.left
        anchors.right: parent.right

        StyledText {
            anchors.left: parent.left
            id: keywordsLabel
            text: i18.n + qsTr("Keywords:")
        }

        StyledText {
            anchors.right: parent.right
            text: quickBuffer.keywordsCount
        }
    }

    Item {
        height: 5
    }

    Rectangle {
        id: keywordsWrapper
        border.color: Colors.artworkActiveColor
        border.width: flv.isFocused ? 1 : 0
        Layout.fillHeight: true
        anchors.left: parent.left
        anchors.right: parent.right
        color: Colors.inputBackgroundColor
        property var keywordsModel: quickBuffer.getBasicModel()

        function removeKeyword(index) {
            quickBuffer.removeKeywordAt(index)
        }

        function removeLastKeyword() {
            quickBuffer.removeLastKeyword()
        }

        function appendKeyword(keyword) {
            quickBuffer.appendKeyword(keyword)
        }

        function pasteKeywords(keywords) {
            quickBuffer.pasteKeywords(keywords)
        }

        function expandLastKeywordAsPreset() {
            quickBuffer.expandLastKeywordAsPreset();
        }

        EditableTags {
            id: flv
            anchors.fill: parent
            model: keywordsWrapper.keywordsModel
            property int keywordHeight: 20 * settingsModel.keywordSizeScale + (settingsModel.keywordSizeScale - 1)*10
            scrollStep: keywordHeight
            populateAnimationEnabled: false

            delegate: KeywordWrapper {
                id: kw
                isHighlighted: true
                keywordText: keyword
                hasSpellCheckError: !iscorrect
                delegateIndex: index
                itemHeight: flv.keywordHeight
                onRemoveClicked: keywordsWrapper.removeKeyword(delegateIndex)
            }

            onTagAdded: {
                keywordsWrapper.appendKeyword(text)
            }

            onRemoveLast: {
                keywordsWrapper.removeLastKeyword()
            }

            onTagsPasted: {
                keywordsWrapper.pasteKeywords(tagsList)
            }

            onBackTabPressed: {
                descriptionTextInput.forceActiveFocus()
                descriptionTextInput.cursorPosition = descriptionTextInput.text.length
            }

            onExpandLastAsPreset: {
                keywordsWrapper.expandLastKeywordAsPreset()
            }

            onRightClickedInside: {
                presetsMenu.popup()
            }
        }

        CustomScrollbar {
            anchors.topMargin: -5
            anchors.bottomMargin: -5
            anchors.rightMargin: -15
            flickable: flv
        }
    }
}
