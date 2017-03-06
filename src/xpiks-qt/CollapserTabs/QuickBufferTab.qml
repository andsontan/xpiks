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
import xpiks 1.0

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
        enabled: {
            var result = (!quickBuffer.isEmpty) && uiManager.hasCurrentEditable;
            if (result) {
                if (mainStackView.areActionsAllowed) {
                    result = (filteredArtItemsModel.selectedArtworksCount <= 1);
                }
            }

            return result;
        }
        implicitHeight: 30
        height: 30
        onClicked: quickBuffer.copyToCurrentEditable()
    }

    Item {
        height: 20
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
            isActive: false
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
        color: Colors.inputInactiveBackground
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
                isActive: false
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
                        var clipboardText = quickClipboard.getText();
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
        height: 15
    }

    Item {
        height: childrenRect.height
        anchors.left: parent.left
        anchors.right: parent.right

        StyledText {
            anchors.left: parent.left
            isActive: false
            text: i18.n + qsTr("Description:")
        }

        StyledText {
            anchors.right: parent.right
            text: descriptionTextInput.length
            isActive: false
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
        color: Colors.inputInactiveBackground
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
                isActive: false
                property string previousText: text
                property int maximumLength: 380
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
                        var clipboardText = quickClipboard.getText();
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
        height: 15
    }

    Item {
        height: childrenRect.height
        anchors.left: parent.left
        anchors.right: parent.right

        StyledText {
            anchors.left: parent.left
            id: keywordsLabel
            isActive: false
            text: i18.n + qsTr("Keywords:")
        }

        StyledText {
            anchors.right: parent.right
            text: quickBuffer.keywordsCount
            isActive: false
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
        anchors.rightMargin: quickScrollBar.visible ? 10 : 0
        color: Colors.inputInactiveBackground
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
                isHighlighted: false
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
                filteredPresetsModel.searchTerm = ''
            }
        }

        CustomScrollbar {
            id: quickScrollBar
            anchors.topMargin: -5
            anchors.bottomMargin: -5
            anchors.rightMargin: -15
            flickable: flv
        }
    }

    Item {
        height: 5
    }

    RowLayout {
        anchors.left: parent.left
        anchors.right: parent.right
        spacing: 5

        Item {
            Layout.fillWidth: true
        }

        StyledText {
            text: i18.n + qsTr("Copy")
            enabled: quickBuffer.keywordsCount > 0
            color: enabled ? (qCopyKeywordsMA.pressed ? Colors.linkClickedColor : Colors.artworkActiveColor) : Colors.labelInactiveForeground

            MouseArea {
                id: qCopyKeywordsMA
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: quickClipboard.setText(quickBuffer.getKeywordsString())
            }
        }

        StyledText {
            text: "|"
            verticalAlignment: Text.AlignVCenter
            isActive: false
        }

        StyledText {
            text: i18.n + qsTr("Clear")
            enabled: quickBuffer.keywordsCount > 0
            color: enabled ? (qClearKeywordsMA.pressed ? Colors.linkClickedColor : Colors.artworkActiveColor) : Colors.labelInactiveForeground

            MouseArea {
                id: qClearKeywordsMA
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: quickBuffer.clearKeywords()
            }
        }
    }

    ClipboardHelper {
        id: quickClipboard
    }
}
