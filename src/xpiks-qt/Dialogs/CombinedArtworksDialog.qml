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
import "../Constants"
import "../Constants/Colors.js" as Colors;
import "../Common.js" as Common;
import "../Components"
import "../StyledControls"

Item {
    id: dialogComponent
    anchors.fill: parent

    property variant componentParent

    signal dialogDestruction();
    Component.onDestruction: dialogDestruction();

    Keys.onEscapePressed: closePopup()

    function closePopup() {
        dialogComponent.destroy()
    }

    function mustUseConfirmation() {
        return appSettings.boolValue(appSettings.useConfirmationDialogsKey, true)
    }

    Connections {
        target: helpersWrapper
        onGlobalCloseRequested: {
            console.debug("UI:CombinedArtworksDialog # globalCloseRequested")
            closePopup()
        }
    }

    PropertyAnimation { target: dialogComponent; property: "opacity";
        duration: 400; from: 0; to: 1;
        easing.type: Easing.InOutQuad ; running: true }

    MessageDialog {
        id: confirmRemoveArtworksDialog
        property int itemsCount
        title: "Confirmation"
        text: i18.n + qsTr("Are you sure you want to remove %1 item(s)?").arg(itemsCount)
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            doRemoveSelectedArtworks()
        }
    }

    function doRemoveSelectedArtworks() {
        combinedArtworks.removeSelectedArtworks()
        if (combinedArtworks.artworksCount === 0) {
            closePopup()
        }
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

    MessageDialog {
        id: clearKeywordsDialog

        title: "Confirmation"
        text: i18.n + qsTr("Clear all keywords?")
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: combinedArtworks.clearKeywords()
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
                var tmp = mapToItem(dialogComponent, mouse.x, mouse.y);
                old_x = tmp.x;
                old_y = tmp.y;
            }

            onPositionChanged: {
                var old_xy = Common.movePopupInsideComponent(dialogComponent, dialogWindow, mouse, old_x, old_y);
                old_x = old_xy[0]; old_y = old_xy[1];
            }
        }

        // This rectangle is the actual popup
        Rectangle {
            id: dialogWindow
            width: 730
            height: 645
            color: Colors.selectedArtworkColor
            anchors.centerIn: parent
            Component.onCompleted: anchors.centerIn = undefined

            property double disabledOpacity: 0.3

            ColumnLayout {
                anchors.fill: parent
                anchors.leftMargin: 20
                anchors.rightMargin: 30
                anchors.topMargin: 20
                anchors.bottomMargin: 10
                spacing: 3

                RowLayout {
                    width: parent.width

                    StyledText {
                        text: i18.n + qsTr("Edit multiple artworks")
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    StyledText {
                        property string originalText: combinedArtworks.artworksCount == 1 ? qsTr("1 artwork being edited") : qsTr("%1 artworks being edited").arg(combinedArtworks.artworksCount)
                        text: i18.n + originalText
                        color: Colors.defaultInputBackground
                    }
                }

                Item {
                    height: 10
                }

                RowLayout {
                    spacing: 5
                    width: parent.width
                    height: 40

                    StyledButton {
                        text: i18.n + qsTr("Assign from selected")
                        width: 150
                        enabled: combinedArtworks.selectedArtworksCount == 1
                        tooltip: i18.n + qsTr("Set metadata from selected artwork to all fields")
                        onClicked: {
                            combinedArtworks.assignFromSelected()
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    StyledButton {
                        text: i18.n + qsTr("Remove selected")
                        width: 150
                        tooltip: i18.n + qsTr("Remove selected artworks from this dialog")
                        enabled: combinedArtworks.selectedArtworksCount > 0
                        onClicked: {
                            if (mustUseConfirmation()) {
                                confirmRemoveArtworksDialog.itemsCount = combinedArtworks.selectedArtworksCount
                                confirmRemoveArtworksDialog.open()
                            } else {
                                doRemoveSelectedArtworks()
                            }
                        }
                    }
                }

                Item {
                    height: 1
                }

                Item {
                    height: 130
                    width: parent.width

                    Rectangle {
                        anchors.fill: parent
                        color: Colors.defaultControlColor

                        StyledScrollView {
                            id: imagesScrollView
                            height: parent.height + 15
                            width: parent.width
                            anchors.margins: 10

                            ListView {
                                boundsBehavior: Flickable.StopAtBounds
                                anchors.fill: parent
                                anchors.margins: 10
                                orientation: Qt.Horizontal
                                spacing: 10
                                model: combinedArtworks

                                add: Transition {
                                    NumberAnimation { property: "opacity"; from: 0; to: 1; duration: 230 }
                                }

                                remove: Transition {
                                    NumberAnimation { property: "opacity"; to: 0; duration: 230 }
                                }

                                displaced: Transition {
                                    NumberAnimation { properties: "x,y"; duration: 230 }
                                }

                                addDisplaced: Transition {
                                    NumberAnimation { properties: "x,y"; duration: 230 }
                                }

                                removeDisplaced: Transition {
                                    NumberAnimation { properties: "x,y"; duration: 230 }
                                }

                                delegate: Item {
                                    property int delegateIndex: index
                                    id: imageWrapper
                                    height: 110
                                    width: height

                                    Image {
                                        anchors.fill: parent
                                        anchors.margins: 1
                                        source: "image://global/" + path
                                        sourceSize.width: 150
                                        sourceSize.height: 150
                                        fillMode: settingsModel.fitSmallPreview ? Image.PreserveAspectFit : Image.PreserveAspectCrop
                                        asynchronous: true
                                    }

                                    Rectangle {
                                        anchors.fill: parent
                                        color: Colors.defaultControlColor
                                        opacity: isselected ? (mouseArea.containsMouse ? 0.6 : 0.7) : (mouseArea.containsMouse ? 0.4 : 0)
                                    }

                                    LargeRemoveIcon {
                                        opacity: isselected ? (mouseArea.containsMouse ? 0.85 : 1) : (mouseArea.containsMouse ? 0.6 : 0)
                                        width: parent.width
                                        height: parent.height
                                    }

                                    MouseArea {
                                        id: mouseArea
                                        anchors.fill: parent
                                        hoverEnabled: true
                                        onClicked: {
                                            combinedArtworks.setArtworksSelected(delegateIndex, !isselected)
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                Item {
                    height: 15
                }

                RowLayout {
                    width: parent.width
                    height: 50
                    spacing: 0

                    Item {
                        width: 25
                        height: parent.height

                        Rectangle {
                            color: Colors.defaultInputBackground
                            anchors.fill: parent
                            opacity: titleCheckBox.checked ? 0.1 : dialogWindow.disabledOpacity
                        }

                        StyledCheckbox {
                            anchors.left: parent.left
                            anchors.top: parent.top
                            anchors.leftMargin: 5
                            anchors.topMargin: 25
                            text: ''
                            id: titleCheckBox
                            onClicked: {
                                combinedArtworks.changeTitle = checked
                                if (checked) { titleTextInput.forceActiveFocus(); }
                            }

                            Component.onCompleted: titleCheckBox.checked = combinedArtworks.changeTitle
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                        height: parent.height
                        enabled: titleCheckBox.checked

                        Rectangle {
                            color: Colors.defaultInputBackground
                            anchors.fill: parent
                            opacity: titleCheckBox.checked ? 0.1 : 0
                        }

                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 3
                            spacing: 2

                            Item {
                                height: 1
                            }

                            RowLayout {
                                spacing: 5

                                StyledText {
                                    text: i18.n + qsTr("Title:")
                                }

                                StyledText {
                                    text: i18.n + qsTr("(same as Description if empty)")
                                    color: Colors.defaultInputBackground
                                }
                            }

                            Rectangle {
                                id: anotherRect
                                anchors.left: parent.left
                                anchors.right: parent.right
                                height: 25
                                color: Colors.defaultInputBackground
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
                                        font.pixelSize: 12*settingsModel.keywordSizeScale
                                        text: combinedArtworks.title
                                        onTextChanged: combinedArtworks.title = text

                                        Keys.onBacktabPressed: {
                                            event.accepted = true
                                        }

                                        Keys.onTabPressed: {
                                            if (descriptionCheckBox.checked) {
                                                descriptionTextInput.forceActiveFocus()
                                                event.accepted = true
                                            } else if (keywordsCheckBox.checked) {
                                                flv.activateEdit()
                                                event.accepted = true
                                            }
                                        }

                                        onActiveFocusChanged: {
                                            if (titleTextInput.length > 0) {
                                                combinedArtworks.spellCheckTitle()
                                            }
                                        }

                                        Component.onCompleted: {
                                            combinedArtworks.initTitleHighlighting(titleTextInput.textDocument)
                                        }

                                        onCursorRectangleChanged: titleFlick.ensureVisible(cursorRectangle)

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
                        }

                        Rectangle {
                            color: Colors.defaultInputBackground
                            anchors.fill: parent
                            opacity: titleCheckBox.checked ? 0 : dialogWindow.disabledOpacity
                        }
                    }
                }

                Item {
                    height: 1
                }

                RowLayout {
                    width: parent.width
                    height: 80
                    spacing: 0

                    Item {
                        width: 25
                        height: parent.height

                        Rectangle {
                            color: Colors.defaultInputBackground
                            anchors.fill: parent
                            opacity: descriptionCheckBox.checked ? 0.1 : dialogWindow.disabledOpacity
                        }

                        StyledCheckbox {
                            anchors.left: parent.left
                            anchors.top: parent.top
                            anchors.leftMargin: 5
                            anchors.topMargin: 25
                            text: ''
                            id: descriptionCheckBox
                            /*indicatorWidth: 24
                            indicatorHeight: 24*/
                            onClicked: {
                                combinedArtworks.changeDescription = checked
                                if (checked) { descriptionTextInput.forceActiveFocus(); }
                            }

                            Component.onCompleted: descriptionCheckBox.checked = combinedArtworks.changeDescription
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                        height: parent.height
                        enabled: descriptionCheckBox.checked

                        Rectangle {
                            color: Colors.defaultInputBackground
                            anchors.fill: parent
                            opacity: descriptionCheckBox.checked ? 0.1 : 0
                        }

                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 3
                            spacing: 2

                            Item {
                                height: 1
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
                                    color: Colors.defaultInputBackground
                                }
                            }

                            Rectangle {
                                id: rect
                                anchors.left: parent.left
                                anchors.right: parent.right
                                height: 50
                                color: Colors.defaultInputBackground
                                border.color: Colors.artworkActiveColor
                                border.width: descriptionTextInput.activeFocus ? 1 : 0
                                clip: true

                                Flickable {
                                    id: descriptionFlick
                                    contentWidth: descriptionTextInput.paintedWidth
                                    contentHeight: descriptionTextInput.paintedHeight
                                    anchors.fill: parent
                                    anchors.margins: 5
                                    anchors.verticalCenter: parent.verticalCenter
                                    interactive: false
                                    flickableDirection: Flickable.HorizontalFlick
                                    clip: true
                                    focus: false

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
                                        font.pixelSize: 12*settingsModel.keywordSizeScale
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

                                        onActiveFocusChanged: {
                                            if (descriptionTextInput.length > 0) {
                                                combinedArtworks.spellCheckDescription()
                                            }
                                        }

                                        onCursorRectangleChanged: descriptionFlick.ensureVisible(cursorRectangle)

                                        Keys.onBacktabPressed: {
                                            if (titleCheckBox.checked) {
                                                titleTextInput.forceActiveFocus()
                                                event.accepted = true
                                            }
                                        }

                                        Keys.onTabPressed: {
                                            if (keywordsCheckBox.checked) {
                                                flv.activateEdit()
                                                event.accepted = true
                                            }
                                        }

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
                        }

                        Rectangle {
                            color: Colors.defaultInputBackground
                            anchors.fill: parent
                            opacity: descriptionCheckBox.checked ? 0 : dialogWindow.disabledOpacity
                        }
                    }
                }

                Item {
                    height: 1
                }

                RowLayout {
                    width: parent.width
                    height: 200
                    spacing: 0

                    Item {
                        width: 25
                        height: parent.height

                        Rectangle {
                            color: Colors.defaultInputBackground
                            anchors.fill: parent
                            opacity: keywordsCheckBox.checked ? 0.1 : dialogWindow.disabledOpacity
                        }

                        StyledCheckbox {
                            anchors.left: parent.left
                            anchors.top: parent.top
                            anchors.leftMargin: 5
                            anchors.topMargin: 20
                            text: ''
                            id: keywordsCheckBox
                            onClicked: {
                                combinedArtworks.changeKeywords = checked
                                if (checked) { flv.activateEdit(); }
                            }

                            Component.onCompleted: keywordsCheckBox.checked = combinedArtworks.changeKeywords
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                        height: parent.height
                        enabled: keywordsCheckBox.checked

                        Rectangle {
                            color: Colors.defaultInputBackground
                            anchors.fill: parent
                            opacity: keywordsCheckBox.checked ? 0.1 : 0
                        }

                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 3
                            spacing: 0

                            RowLayout {
                                spacing: 5

                                StyledText {
                                    id: keywordsLabel
                                    text: i18.n + qsTr("Keywords:")
                                }

                                StyledText {
                                    text: i18.n + qsTr("(comma-separated)")
                                    color: Colors.defaultInputBackground
                                }

                                Item {
                                    Layout.fillWidth: true
                                }

                                StyledText {
                                    text: combinedArtworks.keywordsCount
                                    color: Colors.defaultInputBackground
                                }
                            }

                            Item {
                                height: 4
                            }

                            Rectangle {
                                id: keywordsWrapper
                                border.color: Colors.artworkActiveColor
                                border.width: flv.isFocused ? 1 : 0
                                height: 155
                                anchors.rightMargin: 20
                                Layout.fillWidth: true
                                color: Colors.defaultInputBackground
                                property var keywordsModel: combinedArtworks.getKeywordsModel()

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
                                    model: keywordsWrapper.keywordsModel
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
                                                                    previousKeyword: keyword
                                                                })
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
                                        if (descriptionCheckBox.checked) {
                                            descriptionTextInput.forceActiveFocus()
                                        } else if (titleCheckBox.checked) {
                                            titleTextInput.forceActiveFocus()
                                        }
                                    }
                                }

                                CustomScrollbar {
                                    anchors.topMargin: -5
                                    anchors.bottomMargin: -5
                                    anchors.rightMargin: -15
                                    flickable: flv
                                }
                            }

                            Item { height: 5 }

                            RowLayout {
                                width: parent.width
                                spacing: 10
                                StyledCheckbox {
                                    id: appendKeywordsCheckbox
                                    text: i18.n + qsTr("Only append new keywords")
                                    onClicked: combinedArtworks.appendKeywords = checked
                                    Component.onCompleted: appendKeywordsCheckbox.checked = combinedArtworks.appendKeywords
                                }

                                Item {
                                    Layout.fillWidth: true
                                }

                                StyledText {
                                    text: i18.n + qsTr("Fix spelling")
                                    enabled: keywordsWrapper.keywordsModel ? keywordsWrapper.keywordsModel.hasSpellErrors : false
                                    color: enabled ? (fixSpellingMA.pressed ? Colors.defaultLightColor : Colors.artworkActiveColor) : Colors.defaultInputBackground

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
                                    text: i18.n + qsTr("Suggest keywords")
                                    color: enabled ? (suggestKeywordsMA.pressed ? Colors.defaultLightColor : Colors.artworkActiveColor) : Colors.defaultInputBackground

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
                                    text: i18.n + qsTr("Clear keywords")
                                    color: enabled ? (clearKeywordsMA.pressed ? Colors.defaultLightColor : Colors.artworkActiveColor) : Colors.defaultInputBackground

                                    MouseArea {
                                        id: clearKeywordsMA
                                        anchors.fill: parent
                                        cursorShape: Qt.PointingHandCursor
                                        onClicked: clearKeywordsDialog.open()
                                    }
                                }
                            }

                            Item {
                                Layout.fillHeight: true
                            }
                        }

                        Rectangle {
                            color: Colors.defaultInputBackground
                            anchors.fill: parent
                            opacity: keywordsCheckBox.checked ? 0 : dialogWindow.disabledOpacity
                        }
                    }
                }

                Item {
                    height: 15
                }

                Item {
                    width: parent.width
                    height: 24

                    RowLayout {
                        anchors.fill: parent
                        spacing: 10

                        StyledText {
                            text: i18.n + qsTr("Select changes to save using checkboxes above")
                        }

                        Item {
                            Layout.fillWidth: true
                        }

                        StyledButton {
                            text: i18.n + qsTr("Save")
                            width: 100
                            onClicked: {
                                combinedArtworks.saveEdits()
                                closePopup()
                            }

                            tooltip: "Save selected metadata"
                        }

                        Item {
                            width: 1
                        }

                        StyledButton {
                            text: i18.n + qsTr("Cancel")
                            width: 100
                            onClicked: {
                                closePopup()
                            }

                            tooltip: "Exit with no changes"
                        }
                    }
                }

                Item {
                    Layout.fillHeight: true
                }
            }
        }
    }

    ClipboardHelper {
        id: clipboard
    }

    Component.onCompleted: {
        focus = true
        titleTextInput.forceActiveFocus()
    }
}
