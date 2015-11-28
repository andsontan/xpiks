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
    id: artworkEditVerticalDialog
    anchors.fill: parent

    property string imagePath: ''
    property int artworkIndex: -1
    property variant componentParent
    property var callbackObject

    signal dialogDestruction();
    Component.onDestruction: dialogDestruction();

    function closePopup() {
        artworkEditVerticalDialog.destroy()
    }

    Component.onCompleted: descriptionTextInput.forceActiveFocus()
    Keys.onEscapePressed: closePopup()

    PropertyAnimation { target: artworkEditVerticalDialog; property: "opacity";
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
                var tmp = mapToItem(artworkEditVerticalDialog, mouse.x, mouse.y);
                old_x = tmp.x;
                old_y = tmp.y;

                var dialogPoint = mapToItem(dialogWindow, mouse.x, mouse.y);
                if (!Common.isInComponent(dialogPoint, dialogWindow)) {
                    closePopup()
                }
            }

            onPositionChanged: {
                var old_xy = Common.movePopupInsideComponent(artworkEditVerticalDialog, dialogWindow, mouse, old_x, old_y);
                old_x = old_xy[0]; old_y = old_xy[1];
            }
        }
        // This rectangle is the actual popup
        Rectangle {
            id: dialogWindow
            width: 650
            height: 620
            color: Colors.selectedArtworkColor
            anchors.centerIn: parent
            Component.onCompleted: anchors.centerIn = undefined

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 0

                Rectangle {
                    id: boundsRect
                    color: Colors.defaultControlColor
                    height: 330
                    anchors.left: parent.left
                    anchors.right: parent.right

                    StyledScrollView {
                        id: scrollview
                        anchors.fill: parent
                        anchors.leftMargin: 10
                        anchors.topMargin: 10

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
                }

                Item {
                    height: 20
                }

                RowLayout {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    spacing: 20

                    ColumnLayout {
                        width: 295
                        spacing: 5

                        StyledText {
                            text: qsTr("Description:")
                        }

                        Rectangle {
                            id: rect
                            anchors.left: parent.left
                            anchors.right: parent.right
                            height: 25
                            color: Colors.defaultInputBackground
                            border.color: Colors.artworkActiveColor
                            border.width: descriptionTextInput.activeFocus ? 1 : 0
                            clip: true

                            Flickable {
                                id: descriptionFlick
                                contentWidth: descriptionTextInput.paintedWidth
                                contentHeight: descriptionTextInput.paintedHeight
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.leftMargin: 5
                                anchors.rightMargin: 5
                                anchors.verticalCenter: parent.verticalCenter
                                interactive: false
                                flickableDirection: Flickable.HorizontalFlick
                                height: 30
                                clip: true
                                focus: false

                                function ensureVisible(r) {
                                    if (contentX >= r.x)
                                        contentX = r.x;
                                    else if (contentX+width <= r.x+r.width)
                                        contentX = r.x+r.width-width;
                                }

                                StyledTextEdit {
                                    id: descriptionTextInput
                                    text: combinedArtworks.description
                                    width: descriptionFlick.width
                                    height: descriptionFlick.height
                                    font.pixelSize: 12*settingsModel.keywordSizeScale
                                    onTextChanged: combinedArtworks.description = text
                                    KeyNavigation.tab: titleTextInput

                                    Component.onCompleted: {
                                        combinedArtworks.initDescriptionHighlighting(descriptionTextInput.textDocument)
                                    }
                                }
                            }
                        }
                    }

                    ColumnLayout {
                        spacing: 5
                        width: 295

                        RowLayout {
                            spacing: 5

                            StyledText {
                                text: qsTr("Title:")
                            }

                            StyledText {
                                text: qsTr("(same as Description if empty)")
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
                                height: 30
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.leftMargin: 5
                                anchors.rightMargin: 5
                                anchors.verticalCenter: parent.verticalCenter
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
                                    width: titleFlick.width
                                    height: titleFlick.height
                                    focus: true
                                    font.pixelSize: 12*settingsModel.keywordSizeScale
                                    text: combinedArtworks.title
                                    onTextChanged: combinedArtworks.title = text
                                    KeyNavigation.backtab: descriptionTextInput

                                    Keys.onTabPressed: {
                                        flv.activateEdit()
                                    }

                                    Component.onCompleted: {
                                        combinedArtworks.initTitleHighlighting(titleTextInput.textDocument)
                                    }
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
                    anchors.left: parent.left
                    anchors.right: parent.right

                    StyledText {
                        id: keywordsLabel
                        text: qsTr("Keywords:")
                    }

                    StyledText {
                        text: qsTr("(comma-separated)")
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
                    height: 105
                    anchors.left: parent.left
                    anchors.right: parent.right
                    color: Colors.defaultInputBackground

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
                            isHighlighted: true
                            keywordText: keyword
                            hasSpellCheckError: !iscorrect
                            delegateIndex: index
                            itemHeight: flv.keywordHeight
                            onActionClicked: keywordsWrapper.removeKeyword(delegateIndex)
                            onSpellSuggestionRequested: {
                                combinedArtworks.suggestCorrections()
                                Common.launchDialog("Dialogs/SpellCheckSuggestionsDialog.qml",
                                                    applicationWindow,
                                                    {})
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
                    }

                    CustomScrollbar {
                        anchors.topMargin: -5
                        anchors.bottomMargin: -5
                        anchors.rightMargin: -15
                        flickable: flv
                    }
                }

                Item {
                    Layout.fillHeight: true
                }

                RowLayout {
                    spacing: 20

                    StyledButton {
                        width: 150
                        text: qsTr("Suggest keywords")

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

                    Item {
                        Layout.fillWidth: true
                    }

                    StyledButton {
                        width: 100
                        text: qsTr("Apply")
                        onClicked: {
                            combinedArtworks.saveEdits()
                            callbackObject.applyCallback()
                            closePopup()
                        }
                    }

                    StyledButton {
                        width: 100
                        text: qsTr("Close")
                        onClicked: {
                            closePopup()
                        }
                    }
                }
            }
        }
    }
}
