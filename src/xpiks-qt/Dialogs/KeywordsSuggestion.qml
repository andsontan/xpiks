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

                    StyledText {
                        text: qsTr("(powered by Shutterstock API)")
                        color: Colors.defaultControlColor
                    }
                }

                Item {
                    height: 10
                }

                Rectangle {
                    height: 250
                    width: parent.width
                    color: Colors.defaultControlColor

                    StyledScrollView {
                        id: imagesScrollView
                        height: parent.height
                        width: parent.width + 15
                        enabled: !keywordsSuggestor.isInProgress

                        Flickable {
                            anchors.fill: parent
                            contentHeight: grid.height
                            contentWidth: grid.width
                            anchors.margins: {left: 40; right: 0; top: 20}

                            GridLayout {
                                id: grid
                                columns: 5
                                columnSpacing: 20
                                rowSpacing: 20

                                Repeater {
                                    model: keywordsSuggestor
                                    delegate: Rectangle {
                                        property int delegateIndex: index
                                        id: imageWrapper
                                        height: 110
                                        width: height
                                        color: "transparent"

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

                                Item { height: 110; width: height }
                                Item { height: 110; width: height }
                                Item { height: 110; width: height }
                                Item { height: 110; width: height }
                                Item { height: 110; width: height }
                            }
                        }
                    }

                    Rectangle {
                        anchors.fill: parent
                        color: Colors.selectedArtworkColor
                        opacity: 0.4
                        visible: keywordsSuggestor.isInProgress
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

                    StyledScrollView {
                        id: suggestedScroller
                        width: parent.width + 15
                        height: parent.height
                        highlightOnFocus: true

                        EditableTags {
                            id: suggestedFlv
                            anchors.margins: { left: 5; top: 5; right: 0; bottom: 5 }
                            model: keywordsSuggestor.getSuggestedKeywordsModel()
                            editEnabled: false

                            delegate: KeywordWrapper {
                                isHighlighted: true
                                delegateIndex: index
                                keywordText: modelData
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
                    }

                    MouseArea {
                        anchors.fill: parent
                        propagateComposedEvents: true
                        onClicked: {
                            suggestedFlv.activateEdit()
                            mouse.accepted = false
                        }
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

                    StyledScrollView {
                        id: otherKeywordsScroller
                        width: parent.width + 15
                        height: parent.height
                        highlightOnFocus: true

                        EditableTags {
                            id: otherFlv
                            anchors.margins: { left: 5; top: 5; right: 0; bottom: 5 }
                            model: keywordsSuggestor.getAllOtherKeywordsModel()

                            delegate: KeywordWrapper {
                                delegateIndex: index
                                keywordText: modelData
                                isHighlighted: true
                                hasPlusSign: true
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
                    }

                    MouseArea {
                        anchors.fill: parent
                        propagateComposedEvents: true
                        onClicked: {
                            otherFlv.activateEdit()
                            mouse.accepted = false
                        }
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
                        onTriggered: addKeywordsButton.text = qsTr("Add suggested keywords")
                    }

                    StyledButton {
                        id: addKeywordsButton
                        text: qsTr("Add suggested keywords")
                        enabled: !keywordsSuggestor.isInProgress
                        width: 200
                        onClicked: {
                            callbackObject.promoteKeywords(keywordsSuggestor.getSuggestedKeywords())
                            text = qsTr("Added!")
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
        // for some reason onCompleted was called twice
        if (!initialized) {
            queryText.forceActiveFocus()
            initialized = true
        }
    }
}
