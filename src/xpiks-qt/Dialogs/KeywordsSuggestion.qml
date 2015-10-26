/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2015 Taras Kushnir <kushnirTV@gmail.com>
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
import "../Constants"
import "../Constants/Colors.js" as Colors;
import "../Common.js" as Common;
import "../Components"
import "../StyledControls"

Item {
    id: keywordsSuggestionComponent
    anchors.fill: parent
    property var callbackObject
    property bool initialized: false

    signal dialogDestruction();
    Component.onDestruction: dialogDestruction();

    Keys.onEscapePressed: {
        if (!keywordsSuggestor.isInProgress) {
            closePopup()
        }
    }

    function closePopup() {
        keywordsSuggestor.cancelSearch()
        keywordsSuggestor.close();
        keywordsSuggestionComponent.destroy();
    }

    PropertyAnimation { target: keywordsSuggestionComponent; property: "opacity";
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
                var tmp = mapToItem(keywordsSuggestionComponent, mouse.x, mouse.y);
                old_x = tmp.x;
                old_y = tmp.y;

                var dialogPoint = mapToItem(dialogWindow, mouse.x, mouse.y);
                if (!Common.isInComponent(dialogPoint, dialogWindow)) {
                    if (!keywordsSuggestor.isInProgress) {
                        closePopup()
                    }
                }
            }

            onPositionChanged: {
                var old_xy = Common.movePopupInsideComponent(keywordsSuggestionComponent, dialogWindow, mouse, old_x, old_y);
                old_x = old_xy[0]; old_y = old_xy[1];
            }
        }

        // This rectangle is the actual popup
        Rectangle {
            id: dialogWindow
            width: 730
            height: 610
            color: Colors.selectedArtworkColor
            anchors.centerIn: parent
            Component.onCompleted: anchors.centerIn = undefined

            ColumnLayout {
                anchors.fill: parent
                anchors.leftMargin: 30
                anchors.rightMargin: 30
                anchors.topMargin: 20
                anchors.bottomMargin: 20
                spacing: 5

                RowLayout {
                    height: 24
                    spacing: 20

                    StyledInputHost {
                        border.width: queryText.activeFocus ? 1 : 0
                        Layout.row: 0
                        Layout.column: 1

                        StyledTextInput {
                            id: queryText
                            width: 380
                            height: 24
                            clip: true
                            anchors.left: parent.left
                            anchors.leftMargin: 5
                            onAccepted: keywordsSuggestor.searchArtworks(queryText.text)
                        }
                    }

                    StyledButton {
                        text: qsTr("Search")
                        width: 70
                        activeFocusOnPress: true
                        enabled: !keywordsSuggestor.isInProgress
                        onClicked: keywordsSuggestor.searchArtworks(queryText.text)
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    StyledCheckbox {
                        id: searchUsingAndCheckbox
                        text: qsTr("Use only local source")
                        onCheckedChanged: {
                            keywordsSuggestor.useLocal = checked
                        }

                        Component.onCompleted: checked = keywordsSuggestor.useLocal
                    }
                }

                Item {
                    height: 10
                }

                Rectangle {
                    height: 250
                    width: parent.width
                    color: Colors.defaultControlColor

                    Flickable {
                        clip: true
                        id: suggestionsWrapper
                        anchors.fill: parent
                        contentHeight: flow.childrenRect.height + 40
                        contentWidth: parent.width
                        enabled: !keywordsSuggestor.isInProgress
                        flickableDirection: Flickable.VerticalFlick
                        boundsBehavior: Flickable.StopAtBounds

                        Flow {
                            id: flow
                            spacing: 20
                            anchors.top: parent.top
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.margins: 20

                            Repeater {
                                id: suggestionsRepeater
                                model: keywordsSuggestor

                                delegate: Item {
                                    property int delegateIndex: index
                                    id: imageWrapper
                                    height: 110
                                    width: height

                                    Image {
                                        anchors.fill: parent
                                        anchors.margins: 1
                                        source: url
                                        sourceSize.width: 150
                                        sourceSize.height: 150
                                        fillMode: Image.PreserveAspectCrop
                                        asynchronous: true
                                    }

                                    Rectangle {
                                        anchors.fill: parent
                                        color: Colors.defaultControlColor
                                        opacity: isselected ? (mouseArea.containsMouse ? 0.6 : 0.7) : (mouseArea.containsMouse ? 0.4 : 0)
                                    }

                                    LargeAddIcon {
                                        opacity: isselected ? (mouseArea.containsMouse ? 0.85 : 1) : (mouseArea.containsMouse ? 0.6 : 0)
                                        width: parent.width
                                        height: parent.height
                                        rotation: isselected ? 45 : 0
                                    }

                                    MouseArea {
                                        id: mouseArea
                                        anchors.fill: parent
                                        hoverEnabled: true
                                        onClicked: {
                                            keywordsSuggestor.setArtworkSelected(delegateIndex, !isselected)
                                        }
                                    }
                                }
                            }
                        }
                    }

                    CustomScrollbar {
                        anchors.topMargin: 0
                        anchors.bottomMargin: 0
                        anchors.rightMargin: -20
                        flickable: suggestionsWrapper
                    }

                    Rectangle {
                        anchors.fill: parent
                        color: Colors.selectedArtworkColor
                        opacity: 0.4
                        visible: keywordsSuggestor.isInProgress
                    }

                    Item {
                        anchors.fill: parent
                        visible: !keywordsSuggestor.isInProgress && (suggestionsRepeater.count == 0)

                        StyledText {
                            anchors.centerIn: parent
                            text: qsTr("No results found")
                            color: Colors.selectedMetadataColor
                        }
                    }

                    StyledBusyIndicator {
                        width: parent.height
                        height: parent.height
                        anchors.centerIn: parent
                        running: keywordsSuggestor.isInProgress
                    }
                }

                Item {
                    height: 1
                }

                RowLayout {
                    StyledText {
                        text: qsTr("Suggested keywords:")
                        color: Colors.defaultInputBackground
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    StyledText {
                        text: keywordsSuggestor.suggestedKeywordsCount
                        color: Colors.defaultInputBackground
                    }
                }

                Rectangle {
                    id: suggestedKeywordsWrapper
                    border.color: Colors.artworkActiveColor
                    border.width: suggestedFlv.isFocused ? 1 : 0
                    height: 80
                    anchors.rightMargin: 20
                    Layout.fillWidth: true
                    color: Colors.defaultInputBackground

                    function removeKeyword(index) {
                        var keyword = keywordsSuggestor.removeSuggestedKeywordAt(index)
                        keywordsSuggestor.appendKeywordToOther(keyword)
                    }

                    EditableTags {
                        id: suggestedFlv
                        anchors.fill: parent
                        model: keywordsSuggestor.getSuggestedKeywordsModel()
                        property int keywordHeight: 20 * settingsModel.keywordSizeScale + (settingsModel.keywordSizeScale - 1)*10
                        scrollStep: keywordHeight
                        editEnabled: false

                        delegate: KeywordWrapper {
                            isHighlighted: true
                            delegateIndex: index
                            keywordText: modelData
                            itemHeight: suggestedFlv.keywordHeight
                            onActionClicked: suggestedKeywordsWrapper.removeKeyword(delegateIndex)
                        }

                        onTagAdded: {
                            //suggestedKeywordsWrapper.appendKeyword(text)
                        }

                        onRemoveLast: {
                            //suggestedKeywordsWrapper.removeLastKeyword()
                        }

                        onTagsPasted: {
                            //suggestedKeywordsWrapper.pasteKeywords(tagsList)
                        }
                    }

                    CustomScrollbar {
                        anchors.topMargin: -5
                        anchors.bottomMargin: -5
                        anchors.rightMargin: -15
                        flickable: suggestedFlv
                    }
                }

                Item {
                    height: 1
                }

                RowLayout {
                    StyledText {
                        text: qsTr("Other keywords:")
                        color: Colors.defaultInputBackground
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    StyledText {
                        text: keywordsSuggestor.otherKeywordsCount
                        color: Colors.defaultInputBackground
                    }
                }

                Rectangle {
                    id: otherKeywordsWrapper
                    border.color: Colors.artworkActiveColor
                    border.width: otherFlv.isFocused ? 1 : 0
                    height: 80
                    anchors.rightMargin: 20
                    Layout.fillWidth: true
                    color: Colors.defaultInputBackground

                    function removeKeyword(index) {
                        var keyword = keywordsSuggestor.removeOtherKeywordAt(index)
                        keywordsSuggestor.appendKeywordToSuggested(keyword)
                    }

                    EditableTags {
                        id: otherFlv
                        anchors.fill: parent
                        model: keywordsSuggestor.getAllOtherKeywordsModel()
                        property int keywordHeight: 20 * settingsModel.keywordSizeScale + (settingsModel.keywordSizeScale - 1)*10
                        scrollStep: keywordHeight
                        editEnabled: false

                        delegate: KeywordWrapper {
                            delegateIndex: index
                            keywordText: modelData
                            isHighlighted: true
                            hasPlusSign: true
                            itemHeight: otherFlv.keywordHeight
                            onActionClicked: otherKeywordsWrapper.removeKeyword(delegateIndex)
                        }

                        onTagAdded: {
                            //eywordsWrapper.appendKeyword(text)
                        }

                        onRemoveLast: {
                            //keywordsWrapper.removeLastKeyword()
                        }

                        onTagsPasted: {
                            //keywordsWrapper.pasteKeywords(tagsList)
                        }
                    }

                    CustomScrollbar {
                        anchors.topMargin: -5
                        anchors.bottomMargin: -5
                        anchors.rightMargin: -15
                        flickable: otherFlv
                    }
                }

                Item {
                    height: 10
                }

                RowLayout {
                    spacing: 20

                    Item {
                        Layout.fillWidth: true
                    }

                    Timer {
                        id: suggestedAddedTimer
                        property int iterations: 0
                        interval: 2000
                        repeat: false
                        running: false
                        onTriggered: {
                            addKeywordsButton.text = qsTr("Add suggested keywords")
                            addKeywordsButton.enabled = true
                        }
                    }

                    StyledButton {
                        id: addKeywordsButton
                        text: qsTr("Add suggested keywords")
                        enabled: !keywordsSuggestor.isInProgress
                        width: 200
                        onClicked: {
                            callbackObject.promoteKeywords(keywordsSuggestor.getSuggestedKeywords())
                            text = qsTr("Added!")
                            addKeywordsButton.enabled = false
                            suggestedAddedTimer.start()
                        }
                    }

                    StyledButton {
                        width: 70
                        text: qsTr("Close")
                        onClicked: closePopup()
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        focus = true
        // for some reason onCompleted was called twice
        if (!initialized) {
            queryText.forceActiveFocus()
            initialized = true
        }
    }
}
