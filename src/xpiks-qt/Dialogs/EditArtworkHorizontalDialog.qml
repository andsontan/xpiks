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
import xpiks 1.0
import QtGraphicalEffects 1.0
import "../Constants"
import "../Common.js" as Common;
import "../Components"
import "../StyledControls"
import "../Constants/UIConfig.js" as UIConfig

Item {
    id: artworkEditHorizontalDialog
    anchors.fill: parent

    property string imagePath: ''
    property int artworkIndex: -1
    property variant componentParent
    property var callbackObject

    signal dialogDestruction();
    Component.onDestruction: dialogDestruction();

    function closePopup() {
        artworkEditHorizontalDialog.destroy()
    }

    Component.onCompleted: {
        focus = true
        titleTextInput.forceActiveFocus()
        titleTextInput.cursorPosition = titleTextInput.text.length
    }

    Connections {
        target: helpersWrapper
        onGlobalCloseRequested: {
            console.debug("UI:EditArtworkHorizontalDialog # globalCloseRequested")
            closePopup()
        }
    }

    Connections {
        target: artItemsModel
        onFileWithIndexUnavailable: {
            if (artworkIndex == index) {
                console.debug("Artwork unavailable")
                closePopup()
            }
        }
    }
    MessageDialog {
        id: clearKeywordsDialog

        title: i18.n + qsTr("Confirmation")
        text: i18.n + qsTr("Clear all keywords?")
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: combinedArtworks.clearKeywords()
    }

    Keys.onEscapePressed: closePopup()

    PropertyAnimation { target: artworkEditHorizontalDialog; property: "opacity";
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
        id: focusScope

        MouseArea {
            anchors.fill: parent
            onWheel: wheel.accepted = true
            onClicked: mouse.accepted = true
            onDoubleClicked: mouse.accepted = true

            property real old_x : 0
            property real old_y : 0

            onPressed:{
                var tmp = mapToItem(artworkEditHorizontalDialog, mouse.x, mouse.y);
                old_x = tmp.x;
                old_y = tmp.y;
            }

            onPositionChanged: {
                var old_xy = Common.movePopupInsideComponent(artworkEditHorizontalDialog, dialogWindow, mouse, old_x, old_y);
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
            width: parent.width - 50
            height: parent.height - 120
            color: Colors.selectedImageBackground
            anchors.centerIn: parent
            Component.onCompleted: anchors.centerIn = undefined

            property var keywordsModel: combinedArtworks.getKeywordsModel()
            property var autoCompleteBox

            function onAutoCompleteClose() {
                autoCompleteBox = undefined
            }

            Connections {
                target: combinedArtworks
                onCompletionsAvailable: {
                    if (typeof dialogWindow.autoCompleteBox !== "undefined") {
                        // update completion
                        return
                    }

                    var directParent = dialogWindow;
                    var currWordStartRect = flv.editControl.getCurrentWordStartRect()

                    var tmp = flv.editControl.mapToItem(directParent,
                                                        currWordStartRect.x - 17,
                                                        flv.editControl.height + 1)

                    var visibleItemsCount = Math.min(acSource.getCount(), 5);
                    var popupHeight = visibleItemsCount * (25 + 1) + 10

                    var isBelow = (tmp.y + popupHeight) < directParent.height;

                    var options = {
                        model: acSource,
                        editableTags: flv,
                        isBelowEdit: isBelow,
                        "anchors.left": directParent.left,
                        "anchors.leftMargin": Math.min(tmp.x, directParent.width - 200)
                    }

                    if (isBelow) {
                        options["anchors.top"] = directParent.top
                        options["anchors.topMargin"] = tmp.y
                    } else {
                        options["anchors.bottom"] = directParent.bottom
                        options["anchors.bottomMargin"] = directParent.height - tmp.y + flv.editControl.height
                    }

                    var component = Qt.createComponent("../Components/CompletionBox.qml");
                    if (component.status !== Component.Ready) {
                        console.debug("Component Error: " + component.errorString());
                    } else {
                        var instance = component.createObject(directParent, options);

                        instance.boxDestruction.connect(dialogWindow.onAutoCompleteClose)
                        instance.itemSelected.connect(flv.editControl.acceptCompletion)
                        dialogWindow.autoCompleteBox = instance

                        instance.openPopup()
                    }
                }
            }

            Rectangle {
                id: boundsRect
                color: Colors.defaultControlColor
                anchors.left: parent.left
                anchors.leftMargin: 20
                anchors.topMargin: 20
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 20
                anchors.right: fields.left
                anchors.rightMargin: 20

                Image {
                    id: previewImage
                    source: "image://global/" + imagePath
                    cache: false
                    width: boundsRect.width - 20
                    height: boundsRect.height - 20
                    fillMode: Image.PreserveAspectFit
                    anchors.centerIn: parent
                    asynchronous: true
                }
            }

            ColumnLayout {
                id: fields
                anchors.right: parent.right
                anchors.rightMargin: 20
                anchors.topMargin: 20
                anchors.top: parent.top
                width: 450
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 20
                spacing: 0

                RowLayout {
                    spacing: 5

                    StyledText {
                        text: i18.n + qsTr("Title:")
                    }

                    StyledText {
                        text: i18.n + qsTr("(same as Description if empty)")
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    StyledText {
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
                            text: combinedArtworks.title
                            onTextChanged: combinedArtworks.title = text

                            Keys.onBacktabPressed: {
                                event.accepted = true
                            }

                            Keys.onTabPressed: {
                                descriptionTextInput.forceActiveFocus()
                                descriptionTextInput.cursorPosition = descriptionTextInput.text.length
                            }

                            Component.onCompleted: {
                                combinedArtworks.initTitleHighlighting(titleTextInput.textDocument)
                            }

                            onCursorRectangleChanged: titleFlick.ensureVisible(cursorRectangle)

                            onActiveFocusChanged: combinedArtworks.spellCheckTitle()

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
                    height: 20
                }

                RowLayout {
                    anchors.left: parent.left
                    anchors.right: parent.right

                    StyledText {
                        text: i18.n + qsTr("Description:")
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    StyledText {
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
                    height: 60
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
                            text: combinedArtworks.description
                            focus: true
                            property string previousText: text
                            property int maximumLength: 280
                            onTextChanged: {
                                if (text.length > maximumLength) {
                                    var cursor = cursorPosition;
                                    text = previousText;
                                    if (cursor > text.length) {
                                        cursorPosition = text.length;
                                    } else {
                                        cursorPosition = cursor-1;
                                    }
                                }

                                previousText = text
                                combinedArtworks.description = text
                            }

                            wrapMode: TextEdit.Wrap
                            horizontalAlignment: TextEdit.AlignLeft
                            verticalAlignment: TextEdit.AlignTop
                            textFormat: TextEdit.PlainText

                            Component.onCompleted: {
                                combinedArtworks.initDescriptionHighlighting(descriptionTextInput.textDocument)
                            }

                            Keys.onBacktabPressed: {
                                titleTextInput.forceActiveFocus()
                                titleTextInput.cursorPosition = titleTextInput.text.length
                            }

                            Keys.onTabPressed: {
                                flv.activateEdit()
                            }

                            onCursorRectangleChanged: descriptionFlick.ensureVisible(cursorRectangle)

                            onActiveFocusChanged: combinedArtworks.spellCheckDescription()

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
                    height: 20
                }

                RowLayout {
                    spacing: 5

                    StyledText {
                        id: keywordsLabel
                        text: i18.n + qsTr("Keywords:")
                    }

                    StyledText {
                        text: i18.n + qsTr("(comma-separated)")
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    StyledText {
                        text: combinedArtworks.keywordsCount
                    }
                }

                Item {
                    height: 4
                }

                Rectangle {
                    id: keywordsWrapper
                    border.color: Colors.artworkActiveColor
                    border.width: flv.isFocused ? 1 : 0
                    height: 255
                    anchors.left: parent.left
                    anchors.right: parent.right
                    color: Colors.inputBackgroundColor

                    function removeKeyword(index) {
                        combinedArtworks.removeKeywordAt(index)
                    }

                    function removeLastKeyword() {
                        combinedArtworks.removeLastKeyword()
                    }

                    function appendKeyword(keyword) {
                        combinedArtworks.appendKeyword(keyword)
                    }

                    function pasteKeywords(keywordsList) {
                        combinedArtworks.pasteKeywords(keywordsList)
                    }

                    EditableTags {
                        id: flv
                        anchors.fill: parent
                        model: combinedArtworks.getKeywordsModel()
                        property int keywordHeight: 20 * settingsModel.keywordSizeScale + (settingsModel.keywordSizeScale - 1)*10
                        scrollStep: keywordHeight

                        delegate: KeywordWrapper {
                            id: kw
                            isHighlighted: true
                            keywordText: keyword
                            hasSpellCheckError: !iscorrect
                            delegateIndex: index
                            itemHeight: flv.keywordHeight
                            onRemoveClicked: keywordsWrapper.removeKeyword(delegateIndex)
                            onActionDoubleClicked: {
                                var callbackObject = {
                                    onSuccess: function(replacement) {
                                        combinedArtworks.editKeyword(kw.delegateIndex, replacement)
                                    },
                                    onClose: function() {
                                        flv.activateEdit()
                                    }
                                }

                                Common.launchDialog("Dialogs/EditKeywordDialog.qml",
                                                    componentParent,
                                                    {
                                                        callbackObject: callbackObject,
                                                        previousKeyword: keyword,
                                                        keywordsModel: combinedArtworks.getKeywordsModel()
                                                    })
                            }

                            onActionRightClicked: {
                                if (!iscorrect) {
                                    console.log("Context menu for add word")
                                    addWordContextMenu.word = kw.keywordText;
                                    addWordContextMenu.popup()
                                }
                            }
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

                        onCompletionRequested: {
                            helpersWrapper.autoCompleteKeyword(prefix,
                                                               dialogWindow.keywordsModel)
                        }
                    }

                    CustomScrollbar {
                        anchors.topMargin: -5
                        anchors.bottomMargin: -5
                        anchors.rightMargin: -15
                        flickable: flv
                    }
                }

                Item {
                    height: 4
                }

                RowLayout {
                    anchors.right: parent.right
                    anchors.rightMargin: 3
                    spacing: 5

                    StyledText {
                        text: i18.n + qsTr("Fix spelling")
                        enabled: dialogWindow.keywordsModel ? dialogWindow.keywordsModel.hasSpellErrors : false
                        color: enabled ? (fixSpellingMA.pressed ? Colors.linkClickedColor : Colors.artworkActiveColor) : Colors.labelActiveForeground

                        MouseArea {
                            id: fixSpellingMA
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                combinedArtworks.suggestCorrections()
                                Common.launchDialog("Dialogs/SpellCheckSuggestionsDialog.qml",
                                                    componentParent,
                                                    {})
                            }
                        }
                    }

                    StyledText {
                        text: "|"
                        verticalAlignment: Text.AlignVCenter
                    }

                    StyledText {
                        text: i18.n + qsTr("Suggest")
                        color: enabled ? (suggestKeywordsMA.pressed ? Colors.linkClickedColor : Colors.artworkActiveColor) : Colors.labelActiveForeground

                        MouseArea {
                            id: suggestKeywordsMA
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                var callbackObject = {
                                    promoteKeywords: function(keywords) {
                                        combinedArtworks.pasteKeywords(keywords)
                                    }
                                }

                                Common.launchDialog("Dialogs/KeywordsSuggestion.qml",
                                                    componentParent,
                                                    {callbackObject: callbackObject});
                            }
                        }
                    }

                    StyledText {
                        text: "|"
                        verticalAlignment: Text.AlignVCenter
                    }

                    StyledText {
                        text: i18.n + qsTr("Copy")
                        color: copyKeywordsMA.pressed ? Colors.linkClickedColor : Colors.artworkActiveColor

                        MouseArea {
                            id: copyKeywordsMA
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: clipboard.setText(combinedArtworks.getKeywordsString())
                        }
                    }

                    StyledText {
                        text: "|"
                        verticalAlignment: Text.AlignVCenter
                    }

                    StyledText {
                        text: i18.n + qsTr("Clear")
                        color: enabled ? (clearKeywordsMA.pressed ? Colors.linkClickedColor : Colors.artworkActiveColor) : Colors.labelActiveForeground

                        MouseArea {
                            id: clearKeywordsMA
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: clearKeywordsDialog.open()
                        }
                    }

                    StyledText {
                        text: "|"
                        verticalAlignment: Text.AlignVCenter
                    }

                    StyledText {
                        id: plainTextText
                        text: i18.n + qsTr("Edit in plain text")
                        color: plainTextMA.containsMouse ? Colors.linkClickedColor : Colors.artworkActiveColor

                        MouseArea {
                            id: plainTextMA
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                // strange bug with clicking on the keywords field
                                if (!containsMouse) { return; }

                                var callbackObject = {
                                    onSuccess: function(text) {
                                        combinedArtworks.plainTextEdit(text)
                                    },
                                    onClose: function() {
                                        flv.activateEdit()
                                    }
                                }

                                Common.launchDialog("Dialogs/PlainTextKeywordsDialog.qml",
                                                    applicationWindow,
                                                    {
                                                        callbackObject: callbackObject,
                                                        keywordsText: combinedArtworks.getKeywordsString(),
                                                        keywordsModel: combinedArtworks.getKeywordsModel()

                                                    });
                            }
                        }
                    }
                }

                Item {
                    Layout.fillHeight: true
                }

                RowLayout {
                    spacing: 20

                    Item {
                        Layout.fillWidth: true
                    }

                    StyledButton {
                        width: 100
                        text: i18.n + qsTr("Save")
                        onClicked: {
                            flv.onBeforeClose()
                            combinedArtworks.saveEdits()
                            callbackObject.applyCallback()
                            closePopup()
                        }
                    }

                    StyledButton {
                        width: 100
                        text: i18.n + qsTr("Close")
                        onClicked: {
                            closePopup()
                        }
                    }
                }
            }
        }
    }

    ClipboardHelper {
        id: clipboard
    }
}
