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
import xpiks 1.0
import "../Constants"
import "../StyledControls"
import "../Constants/UIConfig.js" as UIConfig

Flickable {
    id: flowListView
    anchors.fill: parent
    anchors.leftMargin: 5
    anchors.topMargin: 5
    anchors.bottomMargin: 5
    contentWidth: parent.width
    contentHeight: flow.childrenRect.height
    boundsBehavior: Flickable.StopAtBounds
    flickableDirection: Flickable.VerticalFlick
    rightMargin: 10
    interactive: false
    clip: true

    property int scrollStep: 10
    property alias count: repeater.count
    property int currentIndex: -1
    property variant currentItem;
    property alias delegate: repeater.delegate
    property alias flow: flow.flow
    property alias model: repeater.model
    property alias isFocused: nextTagTextInput.activeFocus
    property alias editEnabled: editWrapper.enabled
    property int flowSpacing: 5
    property bool stealWheel: true
    property alias editControl: nextTagTextInput
    property bool autoCompleteActive: acSource.isActive
    property double epsilon: 0.000001

    signal tagAdded(string text)
    signal removeLast()
    signal tagsPasted(var tagsList)
    signal copyRequest();
    signal backTabPressed();
    signal tabPressed();
    signal editActivated();
    signal clickedInside();

    signal completionRequested(string prefix);

    function completionCancel() {
        acSource.cancelCompletion();
    }

    function moveSelectionUp() {
        acSource.moveSelectionUp();
    }

    function moveSelectionDown() {
        acSource.moveSelectionDown();
    }

    function acceptSelected() {
        acSource.acceptSelected()
    }

    function hasSelectedCompletion() {
        return acSource.hasSelectedCompletion()
    }

    function activateEdit() {
        if (editEnabled && !nextTagTextInput.activeFocus) {
            scrollToBottom()
            nextTagTextInput.forceActiveFocus()
        }
    }

    function getEditedText() {
        var tagText = nextTagTextInput.text;
        return helpersWrapper.sanitizeKeyword(tagText);
    }

    function raiseAddTag(text) {
        var canBeAdded = helpersWrapper.isKeywordValid(text);
        if (canBeAdded) {
            tagAdded(text);
        }
        return canBeAdded;
    }

    function scrollDown () {
        var flickable = flowListView;
        if (flowListView.contentHeight >= flowListView.height) {
            flickable.contentY = Math.min (flickable.contentY + scrollStep + flowSpacing, flickable.contentHeight - flickable.height);

            var lines = Math.floor(flickable.contentY / (scrollStep + flowSpacing));
            flickable.contentY = lines*(scrollStep + flowSpacing)
        }
    }

    function scrollUp () {
        var flickable = flowListView;
        if (flowListView.contentHeight >= flowListView.height) {
            flickable.contentY = Math.max (flickable.contentY - scrollStep - flowSpacing, 0);

            var lines = Math.floor(flickable.contentY / (scrollStep + flowSpacing));
            flickable.contentY = lines*(scrollStep + flowSpacing)
        }
    }

    function scrollToBottom() {
        if (flowListView.contentHeight >= flowListView.height) {
            flowListView.contentY = flowListView.contentHeight - flowListView.height
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

    function removeFocus() {
        // bump
    }

    function submitCurrentKeyword() {
        var tagText = getEditedText();
        console.debug("Submit current keyword: " + tagText)

        if (raiseAddTag(tagText)) {
            nextTagTextInput.text = '';
        }
    }

    function onBeforeClose() {
        submitCurrentKeyword()
    }

    MouseArea {
        anchors.left: parent.left
        anchors.top: parent.top
        width: parent.width
        height: flowListView.contentHeight > flowListView.height ? flowListView.contentHeight : flowListView.height

        onClicked: {
            activateEdit()
            clickedInside()
            mouse.accepted = false
        }

        propagateComposedEvents: true
        preventStealing: true

        onWheel: {
            if (!stealWheel && !nextTagTextInput.activeFocus || flowListView.height >= flowListView.contentHeight) {
                wheel.accepted = false
                return
            }

            var shift = wheel.angleDelta.y

            if (shift < -epsilon) {
                var maxScrollPos = flowListView.contentHeight - flowListView.height
                if (Math.abs(flowListView.contentY - maxScrollPos) > scrollStep) {
                    scrollDown()
                } else {
                    scrollToBottom()
                    wheel.accepted = false
                }
            } else if (shift > epsilon) {
                if (flowListView.contentY > scrollStep) {
                    scrollUp()
                } else {
                    flowListView.contentY = 0
                    wheel.accepted = false
                }
            }

            completionCancel()
        }
    }

    Flow {
        id: flow
        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.left: parent.left
        spacing: flowSpacing

        property real lastHeight

        /*add: Transition {
            NumberAnimation { properties: "x,y"; easing.type: Easing.Bezier }
        }*/

        populate: Transition {
            NumberAnimation { properties: "x,y"; from: 200; duration: 100; easing.type: Easing.OutBounce }
        }

        /*move: Transition {
            NumberAnimation { properties: "x,y"; easing.type: Easing.OutBounce }
        }*/

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
            height: 20*settingsModel.keywordSizeScale

            TextInput {
                id: nextTagTextInput
                objectName: "nextTagTextInput"
                maximumLength: 30
                selectedTextColor: Colors.inputForegroundColor
                selectionColor: Colors.defaultControlColor
                selectByMouse: true
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                color: Colors.inputForegroundColor
                font.family: Qt.platform.os === "windows" ? "Arial" : "Helvetica"
                font.pixelSize: UIConfig.fontPixelSize*settingsModel.keywordSizeScale
                verticalAlignment: TextInput.AlignVCenter
                renderType: TextInput.NativeRendering
                focus: true

                function getCurrentWordStart() {
                    var pos = nextTagTextInput.cursorPosition
                    if (pos === text.length || text.charAt(pos) === ' ') { pos-- }
                    while ((pos >= 0) && (text.charAt(pos) !== ' ')) {
                        pos--;
                    }

                    return pos + 1;
                }

                function getCurrentWordStartRect() {
                    var start = getCurrentWordStart();
                    return positionToRectangle(start);
                }

                function getCurrentWordEnd() {
                    var pos = nextTagTextInput.cursorPosition
                    var len = text.length
                    while ((pos < len) && (text.charAt(pos) !== ' ')) {
                        pos++
                    }

                    // substr does not include end
                    return pos
                }

                function requestCompletion() {
                    var canAutoComplete = false;
                    var start = getCurrentWordStart()
                    var end = getCurrentWordEnd()

                    canAutoComplete = end - start >= 3
                    if (canAutoComplete) {
                        var prefix = text.substring(start, end)
                        completionRequested(prefix)
                    }

                    return canAutoComplete;
                }

                function getWordsCount() {
                    var inputText = nextTagTextInput.text
                    var array = inputText.split(' ').filter(function(el) {return el.length !== 0});
                    return array.length;
                }

                function acceptCompletion(completion) {
                    var start = getCurrentWordStart()
                    var end = getCurrentWordEnd()
                    var currText = nextTagTextInput.text;
                    var newText = currText.slice(0, start) + completion + currText.slice(end);
                    var newCursorPosition = start + completion.length

                    nextTagTextInput.text = newText
                    nextTagTextInput.cursorPosition = newCursorPosition

                    if (getWordsCount() === 1) {
                        submitCurrentKeyword()
                    }
                }

                /*validator: RegExpValidator {
                    // copy paste in keys.onpressed Paste
                    regExp: /^(?:\c+(?:-| |$))+$/
                }*/

                onActiveFocusChanged: {
                    if (!activeFocus) {
                        completionCancel()
                    } else {
                        editActivated();
                    }
                }

                onEditingFinished: {
                    console.log("Editing finished for EditableTags")
                    submitCurrentKeyword()
                    completionCancel()
                }

                onCursorPositionChanged: {
                    //completionCancel()
                }

                Keys.onPressed: {
                    if (event.matches(StandardKey.Paste)) {
                        var clipboardText = clipboard.getText();
                        clipboardText = clipboardText.replace(/(\r\n|\n|\r)/gm, '');
                        var keywordsToAdd = [];

                        var words = clipboardText.split(/,|;/);

                        if (words.length > 0) {
                            for (var i = 0; i < words.length; i++) {
                                var sanitizedTagText = helpersWrapper.sanitizeKeyword(words[i]);
                                if (helpersWrapper.isKeywordValid(sanitizedTagText)) {
                                    keywordsToAdd.push(sanitizedTagText);
                                }
                            }

                            if (keywordsToAdd.length > 1) {
                                tagsPasted(keywordsToAdd);
                                event.accepted = true;
                            }
                        }
                    }
                    else if (event.matches(StandardKey.Copy)) {
                        if (nextTagTextInput.length === 0) {
                            copyRequest()
                            event.accepted = true
                        }
                    }
                    else if (event.key === Qt.Key_Comma) {
                        submitCurrentKeyword()
                        completionCancel()
                        event.accepted = true;
                    }
                    else if (event.key === Qt.Key_Tab) {
                        tabPressed()
                        event.accepted = true;
                    }
                    else if (event.key === Qt.Key_Backtab) {
                        backTabPressed()
                        event.accepted = true;
                    } else if (event.key === Qt.Key_Space) {
                        if ((event.modifiers & Qt.ControlModifier) ||
                                (event.modifiers & Qt.MetaModifier)) {
                            requestCompletion()
                            event.accepted = true;
                        } else {
                            completionCancel()
                        }
                    } else if (autoCompleteActive && (event.key === Qt.Key_Return)) {
                        acceptSelected();
                        if (hasSelectedCompletion()) {
                            event.accepted = true
                        }
                    } else if (autoCompleteActive && (event.key === Qt.Key_Up)) {
                        moveSelectionUp()
                        event.accepted = true
                    } else if (autoCompleteActive && (event.key === Qt.Key_Down)) {
                        moveSelectionDown()
                        event.accepted = true;
                    } else if (autoCompleteActive && (event.key === Qt.Key_Escape)) {
                        completionCancel()
                        event.accepted = true;
                    } else if (event.key === Qt.Key_Backspace) {
                        if (nextTagTextInput.length == 0) {
                            removeLast();
                            completionCancel()
                            event.accepted = true;
                        }
                    }

                    scrollToBottom()
                }

                Keys.onReleased: {
                    if (Qt.Key_A <= event.key && event.key <= Qt.Key_Z) {
                        if (text.length >= 3) {
                            requestCompletion()
                        }
                    } else if ((event.key === Qt.Key_Left) ||
                               (event.key === Qt.Key_Right)) {
                        if (autoCompleteActive) {
                            completionCancel()
                        }
                    } else if (event.key === Qt.Key_Backspace) {
                        if (!requestCompletion()) {
                            completionCancel()
                        }
                    }
                }
            }
        }

        ClipboardHelper {
            id: clipboard
        }
    }
}
