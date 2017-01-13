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
import "../Common.js" as Common;
import "../Components"
import "../StyledControls"
import "../Constants/UIConfig.js" as UIConfig

Rectangle {
    id: combinedEditComponent
    color: Colors.selectedArtworkBackground

    property variant componentParent
    property var autoCompleteBox

    property bool wasLeftSideCollapsed

    signal dialogDestruction();
    Component.onDestruction: dialogDestruction();

    function onAutoCompleteClose() {
        autoCompleteBox = undefined
    }

    function closePopup() {
        closeAutoComplete()
        mainStackView.pop()
        combinedArtworks.resetModel()
        restoreLeftPane()
    }

    function restoreLeftPane() {
        if (mainStackView.depth === 1) {
            if (!wasLeftSideCollapsed) {
                expandLeftPane()
            }
        }
    }

    function closeAutoComplete() {
        if (typeof combinedEditComponent.autoCompleteBox !== "undefined") {
            combinedEditComponent.autoCompleteBox.closePopup()
        }
    }

    function doRemoveSelectedArtworks() {
        combinedArtworks.removeSelectedArtworks()
    }

    MessageDialog {
            id: confirmRemoveArtworksDialog
            property int itemsCount
            title: i18.n + qsTr("Confirmation")
            text: i18.n + qsTr("Are you sure you want to remove %1 item(s)?").arg(itemsCount)
            standardButtons: StandardButton.Yes | StandardButton.No
            onYes: {
                doRemoveSelectedArtworks()
            }
    }

    MessageDialog {
        id: clearKeywordsDialog

        title: i18.n + qsTr("Confirmation")
        text: i18.n + qsTr("Clear all keywords?")
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: combinedArtworks.clearKeywords()
    }

    Menu {
        id: wordRightClickMenu
        property string word
        property int keywordIndex
        property bool showAddToDict : true
        property bool showExpandPreset : false

        MenuItem {
            visible: wordRightClickMenu.showAddToDict
            text: qsTr("Add to dictionary")
            onTriggered: spellCheckService.addWordToUserDictionary(wordRightClickMenu.word);
        }

        Menu {
            id : presetSubMenu
            visible: wordRightClickMenu.showExpandPreset
            title: i18.n + qsTr("Expand as preset")
            Instantiator {
                id : presetsInstantiator
                model: filteredPresetsModel
                onObjectAdded: presetSubMenu.insertItem( index, object )
                onObjectRemoved: presetSubMenu.removeItem( object )
                delegate: MenuItem {
                    text: i18.n + qsTr("\"%1\"").arg(filteredPresetsModel.getName(index))
                    onTriggered: {
                        combinedArtworks.replaceFromPreset(wordRightClickMenu.artworkIndex, filteredPresetsModel.getOriginalIndex(index));
                    }
                }
            }
        }
    }

    Menu {
        id: presetsMenu
        property int maxSize : 10

        Instantiator {
            model: presetsModel
            onObjectAdded:{
                presetsMenu.insertItem( index, object )
            }
            onObjectRemoved: presetsMenu.removeItem( object )
            delegate: MenuItem {
                text: i18.n + qsTr("Expand as preset \"%1\"").arg(name)
                onTriggered: {
                    combinedArtworks.appendFromPreset(index);
                }

            }
        }
    }

    Component.onCompleted: {
        focus = true

        titleTextInput.forceActiveFocus()
        titleTextInput.cursorPosition = titleTextInput.text.length
    }

    Connections {
        target: helpersWrapper
        onGlobalBeforeDestruction: {
            console.debug("UI:EditArtworkHorizontalDialog # globalBeforeDestruction")
            //closePopup()
        }
    }

    Connections {
        target: combinedArtworks
        onRequestCloseWindow: {
            closePopup();
        }
    }

    Connections {
        target: combinedArtworks
        onCompletionsAvailable: {
            if (typeof combinedEditComponent.autoCompleteBox !== "undefined") {
                // update completion
                return
            }

            var directParent = combinedEditComponent;
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

                instance.boxDestruction.connect(combinedEditComponent.onAutoCompleteClose)
                instance.itemSelected.connect(flv.editControl.acceptCompletion)
                combinedEditComponent.autoCompleteBox = instance

                instance.openPopup()
            }
        }
    }

    Rectangle {
        id: leftSpacer
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: applicationWindow.leftSideCollapsed ? 0 : 2
        color: Colors.defaultDarkerColor
    }

    Rectangle {
        id: topHeader
        anchors.left: leftSpacer.right
        anchors.right: parent.right
        anchors.top: parent.top
        height: 45
        color: Colors.defaultDarkColor

        RowLayout {
            anchors.leftMargin: 10
            anchors.rightMargin: 20
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            height: childrenRect.height
            spacing: 20

            StyledButton {
                width: 100
                text: i18.n + qsTr("Cancel")
                onClicked: closePopup()
            }

            StyledButton {
                text: i18.n + qsTr("Save")
                width: 100
                isDefault: true
                onClicked: {
                    flv.onBeforeClose()
                    combinedArtworks.saveEdits()
                    closePopup()
                }
            }

            Item {
                Layout.fillWidth: true
            }
        }
    }

    Item {
        id: mainRect
        property real spacing: 5
        anchors.left: leftSpacer.right
        anchors.right: parent.right
        anchors.top: topHeader.bottom
        anchors.bottom: bottomActionsPane.top
        anchors.leftMargin: 10
        anchors.rightMargin: 15
        anchors.topMargin: 10
        anchors.bottomMargin: 10

        RowLayout {
            id: titleWrapper
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            height: 50
            spacing: 0

            Item {
                width: 25
                height: parent.height

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

                        Item {
                            Layout.fillWidth: true
                        }

                        StyledText {
                            text: titleTextInput.length
                        }
                    }

                    Rectangle {
                        id: anotherRect
                        anchors.left: parent.left
                        anchors.right: parent.right
                        height: 25
                        color: enabled ? Colors.inputBackgroundColor : Colors.inputInactiveBackground
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
                                objectName: "titleTextInput"
                                focus: true
                                width: titleFlick.width
                                height: titleFlick.height
                                text: combinedArtworks.title
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
                                    combinedArtworks.title = text
                                }

                                onActionRightClicked: {
                                    var showAddToDict = combinedArtworks.hasTitleWordSpellError(rightClickedWord)
                                    wordRightClickMenu.showAddToDict = showAddToDict
                                    wordRightClickMenu.word = rightClickedWord
                                    wordRightClickMenu.showExpandPreset = false
                                    if (wordRightClickMenu.showAddToDict ||
                                            wordRightClickMenu.showExpandPreset) {
                                        wordRightClickMenu.popup()
                                    }
                                }

                                Keys.onBacktabPressed: {
                                    event.accepted = true
                                }

                                Keys.onTabPressed: {
                                    if (descriptionCheckBox.checked) {
                                        descriptionTextInput.forceActiveFocus()
                                        descriptionTextInput.cursorPosition = descriptionTextInput.text.length
                                        event.accepted = true
                                    } else if (keywordsCheckBox.checked) {
                                        flv.activateEdit()
                                        event.accepted = true
                                    }
                                }

                                onActiveFocusChanged: combinedArtworks.spellCheckTitle()

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
            }
        }

        RowLayout {
            id: descriptionWrapper
            anchors.top: titleWrapper.bottom
            anchors.topMargin: mainRect.spacing
            anchors.left: parent.left
            anchors.right: parent.right
            height: 85
            spacing: 0

            Item {
                width: 25
                height: parent.height

                StyledCheckbox {
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.leftMargin: 5
                    anchors.topMargin: 25
                    text: ''
                    id: descriptionCheckBox
                    objectName: "descriptionCheckBox"
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
                        }
                    }

                    Rectangle {
                        id: rect
                        anchors.left: parent.left
                        anchors.right: parent.right
                        height: 60
                        color: enabled ? Colors.inputBackgroundColor : Colors.inputInactiveBackground
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
                                objectName: "descriptionTextInput"
                                width: descriptionFlick.width
                                height: descriptionFlick.height
                                text: combinedArtworks.description
                                focus: true
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
                                    combinedArtworks.description = text
                                }

                                onActionRightClicked: {
                                    var showAddToDict = combinedArtworks.hasDescriptionWordSpellError(rightClickedWord)
                                    wordRightClickMenu.showAddToDict = showAddToDict
                                    wordRightClickMenu.word = rightClickedWord
                                    wordRightClickMenu.showExpandPreset = false
                                    if (wordRightClickMenu.showAddToDict ||
                                            wordRightClickMenu.showExpandPreset) {
                                        wordRightClickMenu.popup()
                                    }
                                }

                                wrapMode: TextEdit.Wrap
                                horizontalAlignment: TextEdit.AlignLeft
                                verticalAlignment: TextEdit.AlignTop
                                textFormat: TextEdit.PlainText

                                Component.onCompleted: {
                                    combinedArtworks.initDescriptionHighlighting(descriptionTextInput.textDocument)
                                }

                                onActiveFocusChanged: combinedArtworks.spellCheckDescription()

                                onCursorRectangleChanged: descriptionFlick.ensureVisible(cursorRectangle)

                                Keys.onBacktabPressed: {
                                    if (titleCheckBox.checked) {
                                        titleTextInput.forceActiveFocus()
                                        titleTextInput.cursorPosition = titleTextInput.text.length
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
            }
        }

        Item {
            anchors.top: descriptionWrapper.bottom
            anchors.topMargin: mainRect.spacing
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom

            Item {
                id: checkboxPane
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                width: 25

                StyledCheckbox {
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.leftMargin: 5
                    anchors.topMargin: 25
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
                anchors.left: checkboxPane.right
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                enabled: keywordsCheckBox.checked

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
                            id: keywordsLabel
                            text: i18.n + qsTr("Keywords:")
                        }

                        StyledText {
                            text: i18.n + qsTr("(comma-separated)")
                            isActive: false
                        }

                        Item {
                            Layout.fillWidth: true
                        }

                        StyledText {
                            text: combinedArtworks.keywordsCount
                        }
                    }

                    Rectangle {
                        id: keywordsWrapper
                        border.color: Colors.artworkActiveColor
                        border.width: flv.isFocused ? 1 : 0
                        anchors.left: parent.left
                        anchors.right: parent.right
                        Layout.fillHeight: true
                        color: enabled ? Colors.inputBackgroundColor : Colors.inputInactiveBackground
                        property var keywordsModel: combinedArtworks.getBasicModel()

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
                            objectName: "keywordsInput"
                            anchors.fill: parent
                            model: keywordsWrapper.keywordsModel
                            property int keywordHeight: 20 * settingsModel.keywordSizeScale + (settingsModel.keywordSizeScale - 1)*10
                            populateAnimationEnabled: false
                            scrollStep: keywordHeight

                            delegate: KeywordWrapper {
                                id: kw
                                isHighlighted: keywordsCheckBox.checked
                                keywordText: keyword
                                hasSpellCheckError: !iscorrect
                                delegateIndex: index
                                itemHeight: flv.keywordHeight
                                closeIconDisabledColor: Colors.closeIconInactiveColor
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
                                                            keywordIndex: kw.delegateIndex,
                                                            keywordsModel: keywordsWrapper.keywordsModel
                                                        })
                                }

                                onActionRightClicked: {
                                    wordRightClickMenu.showAddToDict = !iscorrect
                                    var keyword = kw.keywordText
                                    wordRightClickMenu.word = keyword
                                    filteredPresetsModel.searchTerm = keyword
                                    wordRightClickMenu.showExpandPreset = (filteredPresetsModel.getItemsCount() !== 0 )
                                    wordRightClickMenu.keywordIndex = kw.delegateIndex
                                    if (wordRightClickMenu.showAddToDict ||
                                            wordRightClickMenu.showExpandPreset) {
                                        wordRightClickMenu.popup()
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
                                if (descriptionCheckBox.checked) {
                                    descriptionTextInput.forceActiveFocus()
                                    descriptionTextInput.cursorPosition = descriptionTextInput.text.length
                                } else if (titleCheckBox.checked) {
                                    titleTextInput.forceActiveFocus()
                                    titleTextInput.cursorPosition = titleTextInput.text.length
                                }
                            }

                            onCompletionRequested: {
                                helpersWrapper.autoCompleteKeyword(prefix,
                                                                   keywordsWrapper.keywordsModel)
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

                    Item { height: 3 }

                    Item {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        height: childrenRect.height

                        RowLayout {
                            anchors.top: parent.top
                            anchors.left: parent.left
                            anchors.leftMargin: 3
                            anchors.right: parent.right
                            anchors.rightMargin: 3
                            spacing: 5

                            StyledText {
                                id: plainTextText
                                text: i18.n + qsTr("<u>edit in plain text</u>")
                                color: plainTextMA.containsMouse ? Colors.linkClickedColor : Colors.labelActiveForeground

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
                                                                keywordsModel: combinedArtworks.getBasicModel()
                                                            });
                                    }
                                }
                            }

                            Item {
                                Layout.fillWidth: true
                            }

                            StyledText {
                                text: i18.n + qsTr("Fix spelling")
                                enabled: keywordsWrapper.keywordsModel ? keywordsWrapper.keywordsModel.hasSpellErrors : false
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
                                color: enabled ? (copyKeywordsMA.pressed ? Colors.linkClickedColor : Colors.artworkActiveColor) : Colors.labelActiveForeground

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
                                    onClicked: {
                                        if (combinedArtworks.keywordsCount > 0) {
                                            clearKeywordsDialog.open()
                                        }
                                    }
                                }
                            }
                        }
                    }

                    Item {
                        height: 10
                    }

                    StyledCheckbox {
                        id: appendKeywordsCheckbox
                        text: i18.n + qsTr("Only append new keywords")
                        onClicked: combinedArtworks.appendKeywords = checked
                        Component.onCompleted: appendKeywordsCheckbox.checked = combinedArtworks.appendKeywords
                    }
                }
            }
        }
    }

    Rectangle {
        id: bottomActionsPane
        anchors.left: leftSpacer.right
        anchors.right: parent.right
        anchors.bottom: bottomPane.top
        height: 45
        color: Colors.defaultDarkColor

        RowLayout {
            spacing: 20
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: 20
            anchors.rightMargin: 20
            height: childrenRect.height
            anchors.verticalCenter: parent.verticalCenter

            Item {
                Layout.fillWidth: true
            }

            StyledButton {
                text: i18.n + qsTr("Assign from selected")
                width: 160
                enabled: combinedArtworks.selectedArtworksCount == 1
                tooltip: i18.n + qsTr("Set metadata from selected artwork to all fields")
                onClicked: {
                    combinedArtworks.assignFromSelected()
                }
            }

            StyledButton {
                text: i18.n + qsTr("Remove selected")
                width: 150
                tooltip: i18.n + qsTr("Remove selected artworks from this dialog")
                enabled: combinedArtworks.selectedArtworksCount > 0
                onClicked: {
                    confirmRemoveArtworksDialog.itemsCount = combinedArtworks.selectedArtworksCount
                    confirmRemoveArtworksDialog.open()
                }
            }
        }
    }

    Rectangle {
        id: bottomPane
        anchors.left: leftSpacer.right
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: 110
        color: Colors.panelColor

        Item {
            id: selectPrevButton
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: 40
            enabled: (rosterListView.contentWidth > rosterListView.width) && (rosterListView.contentX > 0)

            Rectangle {
                anchors.fill: parent
                color: enabled ? (prevButtonMA.containsMouse ? Colors.defaultControlColor : Colors.leftSliderColor) : Colors.panelColor
            }

            TriangleElement {
                width: 7
                height: 14
                anchors.centerIn: parent
                anchors.horizontalCenterOffset: isFlipped ? -1 : +1
                isVertical: true
                isFlipped: true
                color: {
                    if (enabled) {
                        return prevButtonMA.pressed ? Colors.inputForegroundColor : Colors.artworkActiveColor
                    } else {
                        return Colors.inputBackgroundColor
                    }
                }
            }

            MouseArea {
                id: prevButtonMA
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    rosterListView.scrollHorizontally(-rosterListView.height)
                }
            }
        }

        ListView {
            id: rosterListView
            boundsBehavior: Flickable.StopAtBounds
            orientation: ListView.Horizontal
            anchors.left: selectPrevButton.right
            anchors.right: selectNextButton.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            model: combinedArtworks
            highlightFollowsCurrentItem: false
            highlightMoveDuration: 0
            flickableDirection: Flickable.HorizontalFlick
            interactive: false
            focus: true
            clip: true
            spacing: 0

            function scrollHorizontally(shiftX) {
                var flickable = rosterListView
                var epsilon = 1e-6

                if (shiftX > epsilon) { // up/right
                    var maxScrollPos = flickable.contentWidth - flickable.width
                    flickable.contentX = Math.min(maxScrollPos, flickable.contentX + shiftX)
                } else if (shiftX < -epsilon) { // bottom/left
                    flickable.contentX = Math.max(0, flickable.contentX + shiftX)
                }
            }

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

            delegate: Rectangle {
                id: cellItem
                property int delegateIndex: index
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                width: bottomPane.height
                color: isselected ? Colors.panelSelectedColor : "transparent"

                Rectangle {
                    anchors.fill: parent
                    visible: !isselected && imageMA.containsMouse
                    color: Colors.panelSelectedColor
                    opacity: 0.6
                }

                Image {
                    id: artworkImage
                    anchors.fill: parent
                    anchors.margins: 15
                    source: "image://cached/" + filepath
                    sourceSize.width: 150
                    sourceSize.height: 150
                    fillMode: settingsModel.fitSmallPreview ? Image.PreserveAspectFit : Image.PreserveAspectCrop
                    asynchronous: true
                    // caching is implemented on different level
                    cache: false
                }

                Image {
                    id: imageTypeIcon
                    visible: hasvectorattached
                    enabled: hasvectorattached
                    source: "qrc:/Graphics/vector-icon.svg"
                    sourceSize.width: 20
                    sourceSize.height: 20
                    anchors.left: artworkImage.left
                    anchors.bottom: artworkImage.bottom
                    cache: true
                }

                Rectangle {
                    anchors.fill: parent
                    color: Colors.defaultControlColor
                    visible: isselected
                    opacity: imageMA.containsMouse ? 0.6 : 0.7
                }

                SelectedIcon {
                    opacity: imageMA.containsMouse ? 0.85 : 1
                    width: parent.width * 0.33
                    visible: isselected
                    height: parent.height * 0.33
                    anchors.centerIn: parent
                }

                MouseArea {
                    id: imageMA
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        combinedArtworks.setArtworkSelected(delegateIndex, !isselected)
                    }
                }
            }

            // hack until QML will allow scrolling horizontally with mouse wheel
            MouseArea {
                id: horizontalScrollMA
                anchors.fill: parent
                enabled: rosterListView.contentWidth > rosterListView.width
                propagateComposedEvents: true
                preventStealing: true
                property double epsilon: 0.000001

                onWheel: {
                    var shiftX = wheel.angleDelta.y
                    rosterListView.scrollHorizontally(shiftX)
                    wheel.accepted = Math.abs(shiftX) > epsilon
                }
            }
        }

        Item {
            id: selectNextButton
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: 40
            enabled: (rosterListView.contentWidth > rosterListView.width) && (rosterListView.contentX < (rosterListView.contentWidth - rosterListView.width))

            Rectangle {
                anchors.fill: parent
                color: enabled ? (nextButtonMA.containsMouse ? Colors.defaultControlColor : Colors.leftSliderColor) : Colors.panelColor
            }

            TriangleElement {
                width: 7
                height: 14
                anchors.centerIn: parent
                anchors.horizontalCenterOffset: isFlipped ? -1 : +1
                isVertical: true
                color: {
                    if (enabled) {
                        return nextButtonMA.pressed ? Colors.inputForegroundColor : Colors.artworkActiveColor
                    } else {
                        return Colors.inputBackgroundColor
                    }
                }
            }

            MouseArea {
                id: nextButtonMA
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    rosterListView.scrollHorizontally(rosterListView.height)
                }
            }
        }
    }
}
