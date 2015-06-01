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
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import xpiks 1.0
import "../Constants"
import "../Constants/Colors.js" as Colors
import "../StyledControls"

Flickable {
    id: flowListView
    anchors.fill: parent
    anchors.rightMargin: 20
    contentWidth: parent.width - 20
    contentHeight: flow.childrenRect.height + 10
    boundsBehavior: Flickable.StopAtBounds

    property alias count: repeater.count
    property int currentIndex: -1
    property variant currentItem;
    property alias delegate: repeater.delegate
    property alias flow: flow.flow
    property alias model: repeater.model
    property alias isFocused: nextTagTextInput.activeFocus

    signal tagAdded(string text)
    signal removeLast()
    signal focusLost()
    signal tagsPasted(var tagsList)

    function activateEdit() {
        nextTagTextInput.forceActiveFocus()
    }

    function getSanitizedText(text) {
        return text.replace(/^\s+|\s+$|-$/g, '');
    }

    function getEditedText() {
        var tagText = nextTagTextInput.text;
        return getSanitizedText(tagText);
    }

    function canBeTag(tag) {
        return (getCharsCount(tag) > 2) &&
                (tag.match(/^(?:[a-zA-Z]+(?:-| |$))+$/));
    }

    function raiseAddTag(text) {
        var sanitizedTagText = getSanitizedText(text);
        if (canBeTag(sanitizedTagText)) {
            tagAdded(sanitizedTagText);
        }
    }

    function getCharsCount(text) {
        return (text.match(/[a-zA-Z]/g) || []).length
    }

    function scrollToBottom() {
        if (flowListView.contentHeight - 10 >= flowListView.height) {
            flowListView.contentY = flowListView.contentHeight - 10 - flowListView.height
        }
    }

    function adjustEdit() {
        if (Math.abs(editWrapper.y - flowListView.contentY) <= flowListView.height)  {
            flowListView.scrollToBottom()
        }
    }

    function forceRefreshRepeater() {
        repeater.update()
    }

    Flow {
        id: flow
        width: parent.width
        spacing: 5

        property real lastHeight

        onHeightChanged: {
            if (!lastHeight) {
                lastHeight = height
            }

            if (height > lastHeight) {
                flowListView.scrollToBottom()
            }

            adjustEdit()

            lastHeight = height
        }

        Repeater {
            id: repeater

            onItemRemoved: {
                adjustEdit()
            }
        }

        Item {
            id: editWrapper
            width: 100
            height: 20

            TextInput {
                id: nextTagTextInput
                maximumLength: 30
                selectedTextColor: Colors.defaultLightColor
                selectionColor: Colors.defaultControlColor
                selectByMouse: true
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                color: Colors.defaultLightColor
                focus: true
                font.family: "Helvetica"
                font.pixelSize: 12
                renderType: TextInput.NativeRendering

                validator: RegExpValidator {
                    // copy paste in keys.onpressed Paste
                    regExp: /^(?:[a-zA-Z]+(?:-| |$))+$/
                }

                onEditingFinished: {
                    var tagText = getEditedText();
                    raiseAddTag(tagText);

                    if (getCharsCount(tagText) > 2) {
                        nextTagTextInput.text = '';
                    }

                    focusLost()
                }

                Keys.onPressed: {
                    if(event.matches(StandardKey.Paste)) {
                        var clipboardText = clipboard.getText();
                        clipboardText = clipboardText.replace(/(\r\n|\n|\r)/gm, '');
                        var keywordsToAdd = [];

                        var words = clipboardText.split(',');
                        for (var i = 0; i < words.length; i++) {
                            var sanitizedTagText = getSanitizedText(words[i]);
                            if (canBeTag(sanitizedTagText)) {
                                keywordsToAdd.push(sanitizedTagText);
                            }
                        }

                        tagsPasted(keywordsToAdd);

                        event.accepted = true;
                    }
                    else if (event.key === Qt.Key_Comma) {
                        var tagText = getEditedText();
                        if (getCharsCount(tagText) > 2) {
                            raiseAddTag(tagText);
                            nextTagTextInput.text = '';
                        }

                        event.accepted = true;
                    }
                    else if (event.key === Qt.Key_Backspace) {
                        if (nextTagTextInput.length == 0) {
                            removeLast();
                            event.accepted = true;
                        }
                    }

                    scrollToBottom()
                }
            }
        }

        ClipboardHelper {
            id: clipboard
        }
    }
}
