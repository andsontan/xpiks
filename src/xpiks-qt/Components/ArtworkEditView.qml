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

Rectangle {
    id: artworkEditComponent
    anchors.fill: parent
    color: Colors.artworkImageBackground

    property variant componentParent
    property var autoCompleteBox

    property int artworkIndex: -1
    property var keywordsModel

    signal dialogDestruction();
    Component.onDestruction: dialogDestruction();

    function onAutoCompleteClose() {
        autoCompleteBox = undefined
    }

    function reloadItemEditing(itemIndex) {
        if (itemIndex === artworkIndex) { return }

        closeAutoComplete()

        var originalIndex = filteredArtItemsModel.getOriginalIndex(itemIndex)
        var metadata = filteredArtItemsModel.getArtworkMetadata(itemIndex)
        var keywordsModel = filteredArtItemsModel.getBasicModel(itemIndex)

        artworkProxy.setSourceArtwork(metadata, originalIndex)

        artworkEditComponent.artworkIndex = itemIndex
        artworkEditComponent.keywordsModel = keywordsModel
        rosterListView.currentIndex = itemIndex
        rosterListView.positionViewAtIndex(itemIndex, ListView.Contain)

        titleTextInput.forceActiveFocus()
        titleTextInput.cursorPosition = titleTextInput.text.length
    }

    function closePopup() {
        closeAutoComplete()
        mainStackView.pop()
        artworkProxy.resetModel()
        expandLeftPane()
        settingsModel.saveArtworkEditUISettings()
    }

    function closeAutoComplete() {
        if (typeof artworkEditComponent.autoCompleteBox !== "undefined") {
            artworkEditComponent.autoCompleteBox.closePopup()
        }
    }

    Menu {
        id: addWordContextMenu
        property string word

        MenuItem {
            text: qsTr("Add to dictionary")
            onTriggered: spellCheckService.addWordToUserDictionary(addWordContextMenu.word);
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
        onYes: filteredArtItemsModel.clearKeywords(artworkEditComponent.artworkIndex)
    }

    // Keys.onEscapePressed: closePopup()

    Connections {
        target: artworkProxy

        onCompletionsAvailable: {
            if (typeof artworkEditComponent.autoCompleteBox !== "undefined") {
                // update completion
                return
            }

            var directParent = artworkEditComponent;
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

            var component = Qt.createComponent("CompletionBox.qml");
            if (component.status !== Component.Ready) {
                console.debug("Component Error: " + component.errorString());
            } else {
                var instance = component.createObject(directParent, options);

                instance.boxDestruction.connect(artworkEditComponent.onAutoCompleteClose)
                instance.itemSelected.connect(flv.editControl.acceptCompletion)
                artworkEditComponent.autoCompleteBox = instance

                instance.openPopup()
            }
        }
    }

    Rectangle {
        id: leftSpacer
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: applicationWindow.leftSideCollapsed ? 2 : 0
        color: Colors.defaultControlColor
    }

    SplitView {
        orientation: Qt.Horizontal
        anchors.left: leftSpacer.right
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: bottomPane.top

        handleDelegate: Rectangle {
            color: Colors.defaultDarkerColor
        }

        onResizingChanged: {
            settingsModel.artworkEditRightPaneWidth = rightPane.width
        }

        Item {
            id: boundsRect
            Layout.fillWidth: true
            anchors.top: parent.top
            anchors.bottom: parent.bottom

            Rectangle {
                id: topHeader
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                height: 45
                color: Colors.defaultDarkColor

                RowLayout {
                    anchors.leftMargin: 10
                    anchors.rightMargin: 10
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    height: childrenRect.height
                    spacing: 0

                    StyledButton {
                        width: 100
                        text: qsTr("Back")
                        onClicked: closePopup()
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    StyledText {
                        text: artworkProxy.basename
                    }
                }
            }

            Item {
                id: imageWrapper
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: topHeader.bottom
                anchors.bottom: parent.bottom

                Image {
                    id: previewImage
                    source: "image://global/" + artworkProxy.imagePath
                    cache: false
                    width: imageWrapper.width - 20
                    height: imageWrapper.height - 20
                    fillMode: Image.PreserveAspectFit
                    anchors.centerIn: parent
                    asynchronous: true
                }
            }
        }

        Item {
            id: rightPane
            Layout.maximumWidth: 550
            Layout.minimumWidth: 250
            //width: 300
            anchors.top: parent.top
            anchors.bottom: parent.bottom

            Component.onCompleted: {
                rightPane.width = settingsModel.artworkEditRightPaneWidth
            }

            Row {
                id: tabsHeader
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                height: 45
                spacing: 0

                Repeater {
                    model: [qsTr("Edit"), qsTr("Info")]
                    delegate: CustomTab {
                        width: rightPane.width/2
                        property int delegateIndex: index
                        tabIndex: delegateIndex
                        isSelected: tabIndex === editTabView.currentIndex
                        color: isSelected ? Colors.defaultControlColor : Colors.defaultDarkColor
                        hovered: tabMA.containsMouse

                        StyledText {
                            color: parent.isSelected ? Colors.artworkActiveColor : (parent.hovered ? Colors.inputForegroundColor : Colors.labelActiveForeground)
                            text: modelData
                            anchors.centerIn: parent
                        }

                        MouseArea {
                            id: tabMA
                            anchors.fill: parent
                            hoverEnabled: true
                            onClicked: editTabView.setCurrentIndex(parent.tabIndex)
                        }
                    }
                }
            }

            Item {
                id: editTabView
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: tabsHeader.bottom
                anchors.bottom: parent.bottom
                property int currentIndex: 0

                function setCurrentIndex(index) {
                    if (index === 0) {
                        infoTab.visible = false
                        editTab.visible = true
                    } else if (index === 1) {
                        infoTab.visible = true
                        editTab.visible = false
                    }

                    editTabView.currentIndex = index
                }

                Rectangle {
                    id: editTab
                    color: Colors.defaultControlColor
                    anchors.fill: parent
                    property color inputBackgroundColor: Colors.selectedImageBackground

                    ColumnLayout {
                        id: fields
                        anchors.fill: parent
                        anchors.leftMargin: 10
                        anchors.rightMargin: 20
                        anchors.topMargin: 5
                        anchors.bottomMargin: 10
                        spacing: 0

                        Item {
                            height: childrenRect.height
                            anchors.left: parent.left
                            anchors.right: parent.right

                            StyledText {
                                anchors.left: parent.left
                                text: i18.n + qsTr("Title:")
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
                            color: editTab.inputBackgroundColor
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
                                    text: artworkProxy.title
                                    onTextChanged: artworkProxy.title = text
                                    userDictEnabled: true

                                    onActionRightClicked: {
                                        if (artworkProxy.hasTitleWordSpellError(rightClickedWord)) {
                                            console.log("Context menu for add word " + rightClickedWord)
                                            addWordContextMenu.word = rightClickedWord
                                            addWordContextMenu.popup()
                                        }
                                    }

                                    Keys.onBacktabPressed: {
                                        event.accepted = true
                                    }

                                    Keys.onTabPressed: {
                                        descriptionTextInput.forceActiveFocus()
                                        descriptionTextInput.cursorPosition = descriptionTextInput.text.length
                                    }

                                    Component.onCompleted: {
                                        artworkProxy.initTitleHighlighting(titleTextInput.textDocument)
                                    }

                                    onCursorRectangleChanged: titleFlick.ensureVisible(cursorRectangle)

                                    onActiveFocusChanged: artworkProxy.spellCheckTitle()

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
                            color: editTab.inputBackgroundColor
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
                                    text: artworkProxy.description
                                    focus: true
                                    userDictEnabled: true
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
                                        artworkProxy.description = text
                                    }

                                    onActionRightClicked: {
                                        if (artworkProxy.hasDescriptionWordSpellError(rightClickedWord)) {
                                            console.log("Context menu for add word " + rightClickedWord)
                                            addWordContextMenu.word = rightClickedWord
                                            addWordContextMenu.popup()
                                        }
                                    }

                                    wrapMode: TextEdit.Wrap
                                    horizontalAlignment: TextEdit.AlignLeft
                                    verticalAlignment: TextEdit.AlignTop
                                    textFormat: TextEdit.PlainText

                                    Component.onCompleted: {
                                        artworkProxy.initDescriptionHighlighting(descriptionTextInput.textDocument)
                                    }

                                    Keys.onBacktabPressed: {
                                        titleTextInput.forceActiveFocus()
                                        titleTextInput.cursorPosition = titleTextInput.text.length
                                    }

                                    Keys.onTabPressed: {
                                        flv.activateEdit()
                                    }

                                    onCursorRectangleChanged: descriptionFlick.ensureVisible(cursorRectangle)

                                    onActiveFocusChanged: artworkProxy.spellCheckDescription()

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
                                text: artworkProxy.keywordsCount
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
                            color: editTab.inputBackgroundColor

                            function removeKeyword(index) {
                                artworkProxy.removeKeywordAt(index)
                            }

                            function removeLastKeyword() {
                                artworkProxy.removeLastKeyword()
                            }

                            function appendKeyword(keyword) {
                                artworkProxy.appendKeyword(keyword)
                            }

                            function pasteKeywords(keywords) {
                                artworkProxy.pasteKeywords(keywords)
                            }

                            EditableTags {
                                id: flv
                                anchors.fill: parent
                                model: artworkEditComponent.keywordsModel
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
                                                artworkProxy.editKeyword(kw.delegateIndex, replacement)
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
                                                                keywordsModel: artworkProxy.getBasicModel()
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
                                                                       artworkProxy.getBasicModel())
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

                        StyledText {
                            text: i18.n + qsTr("Fix spelling")
                            enabled: artworkEditComponent.keywordsModel ? artworkEditComponent.keywordsModel.hasSpellErrors : false
                            color: enabled ? (fixSpellingMA.pressed ? Colors.linkClickedColor : Colors.artworkActiveColor) : Colors.labelActiveForeground

                            MouseArea {
                                id: fixSpellingMA
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                onClicked: {
                                    artworkProxy.suggestCorrections()
                                    Common.launchDialog("Dialogs/SpellCheckSuggestionsDialog.qml",
                                                        componentParent,
                                                        {})
                                }
                            }
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
                                            artworkProxy.pasteKeywords(keywords)
                                        }
                                    }

                                    Common.launchDialog("Dialogs/KeywordsSuggestion.qml",
                                                        componentParent,
                                                        {callbackObject: callbackObject});
                                }
                            }
                        }

                        StyledText {
                            text: i18.n + qsTr("Copy")
                            color: copyKeywordsMA.pressed ? Colors.linkClickedColor : Colors.artworkActiveColor

                            MouseArea {
                                id: copyKeywordsMA
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                onClicked: clipboard.setText(artworkProxy.getKeywordsString())
                            }
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
                                            artworkProxy.plainTextEdit(text)
                                        },
                                        onClose: function() {
                                            flv.activateEdit()
                                        }
                                    }

                                    Common.launchDialog("Dialogs/PlainTextKeywordsDialog.qml",
                                                        applicationWindow,
                                                        {
                                                            callbackObject: callbackObject,
                                                            keywordsText: artworkProxy.getKeywordsString(),
                                                            keywordsModel: artworkProxy.getBasicModel()
                                                        });
                                }
                            }
                        }

                        Item {
                            Layout.fillHeight: true
                        }
                    }
                }

                Rectangle {
                    id: infoTab
                    visible: false
                    color: Colors.defaultControlColor
                    anchors.fill: parent

                    StyledText {
                        text: "Test here"
                        anchors.centerIn: parent
                    }
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
        color: Colors.artworkBackground

        Item {
            id: selectPrevButton
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: 40

            TriangleElement {
                width: 7
                height: 14
                anchors.centerIn: parent
                anchors.horizontalCenterOffset: isFlipped ? -1 : +1
                isVertical: true
                isFlipped: true
                color: {
                    if (leftCollapseMA.pressed) {
                        return Colors.whiteColor
                    } else {
                        return prevButtonMA.containsMouse ? Colors.defaultLightGrayColor : Colors.inputBackgroundColor
                    }
                }
            }

            MouseArea {
                id: prevButtonMA
                anchors.fill: parent
                hoverEnabled: true
                enabled: rosterListView.currentIndex > 0
                onClicked: {
                    reloadItemEditing(rosterListView.currentIndex - 1)
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
            model: filteredArtItemsModel
            highlightFollowsCurrentItem: false
            highlightMoveDuration: 0
            flickableDirection: Flickable.HorizontalFlick
            interactive: false
            focus: true
            clip: true
            spacing: 0

            Component.onCompleted: {
                rosterListView.currentIndex = artworkEditComponent.artworkIndex
                rosterListView.positionViewAtIndex(artworkEditComponent.artworkIndex, ListView.Center)
            }

            delegate: Rectangle {
                id: cellItem
                property int delegateIndex: index
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                width: bottomPane.height
                color: (ListView.isCurrentItem || imageMA.containsMouse) ? Colors.selectedArtworkBackground : "transparent"

                Image {
                    id: artworkImage
                    anchors.fill: parent
                    anchors.margins: 15
                    source: "image://cached/" + filename
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

                MouseArea {
                    id: imageMA
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        reloadItemEditing(cellItem.delegateIndex)
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
                    var flickable = rosterListView

                    if (shiftX > epsilon) { // up/right
                        var maxScrollPos = flickable.contentWidth - flickable.width
                        flickable.contentX = Math.min(maxScrollPos, flickable.contentX + shiftX)
                        wheel.accepted = true
                    } else if (shiftX < -epsilon) { // bottom/left
                        flickable.contentX = Math.max(0, flickable.contentX + shiftX)
                        wheel.accepted = true
                    }

                }
            }
        }

        Item {
            id: selectNextButton
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: 40

            TriangleElement {
                width: 7
                height: 14
                anchors.centerIn: parent
                anchors.horizontalCenterOffset: isFlipped ? -1 : +1
                isVertical: true
                color: {
                    if (leftCollapseMA.pressed) {
                        return Colors.whiteColor
                    } else {
                        return nextButtonMA.containsMouse ? Colors.defaultLightGrayColor : Colors.inputBackgroundColor
                    }
                }
            }

            MouseArea {
                id: nextButtonMA
                anchors.fill: parent
                hoverEnabled: true
                enabled: rosterListView.currentIndex < (rosterListView.count - 1)
                onClicked: {
                    reloadItemEditing(rosterListView.currentIndex + 1)
                }
            }
        }
    }

    ClipboardHelper {
        id: clipboard
    }
}
