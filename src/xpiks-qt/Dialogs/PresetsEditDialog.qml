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
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.1
import QtQuick.Controls.Styles 1.1
import QtGraphicalEffects 1.0
import xpiks 1.0
import "../Constants"
import "../Common.js" as Common;
import "../Components"
import "../StyledControls"

Item {
    id: presetEditComponent
    anchors.fill: parent

    property variant componentParent
    signal dialogDestruction();
    Component.onDestruction: dialogDestruction();

    Component.onCompleted: {
    }

    Keys.onEscapePressed: closePopup()

    function closePopup() {
        presetEditComponent.destroy();
    }

    PropertyAnimation { target: presetEditComponent; property: "opacity";
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

    MessageDialog {
        id: confirmRemoveItemDialog
        property int itemIndex
        title: i18.n + qsTr("Confirmation")
        text: i18.n + qsTr("Are you sure you want to remove this item?")
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            presetsModel.removeItem(itemIndex);
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
                var tmp = mapToItem(presetEditComponent, mouse.x, mouse.y);
                old_x = tmp.x;
                old_y = tmp.y;
            }

            onPositionChanged: {
                var old_xy = Common.movePopupInsideComponent(presetEditComponent, dialogWindow, mouse, old_x, old_y);
                old_x = old_xy[0]; old_y = old_xy[1];
            }
        }

        ClipboardHelper {
            id: clipboard
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
            width: 700
            height: 350
            color: Colors.popupBackgroundColor
            anchors.centerIn: parent
            Component.onCompleted: anchors.centerIn = undefined

            Item {
                id: listPanel
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                width: 250

                ColumnLayout {
                    id: presetsStack
                    anchors.fill: parent
                    anchors.leftMargin: 20
                    anchors.topMargin: 20
                    anchors.bottomMargin: 20
                    spacing: 0

                    Rectangle {
                        height: 5
                        anchors.left: parent.left
                        anchors.right: parent.right
                        color: Colors.defaultControlColor
                    }

                    Rectangle {
                        color: Colors.defaultControlColor
                        anchors.left: parent.left
                        anchors.right: parent.right
                        Layout.fillHeight: true

                        StyledScrollView {
                            anchors.fill: parent
                            anchors.margins: { left: 5; top: 5; right: 5; bottom: 5 }

                            ListView {
                                id: presetNamesListView
                                model: presetsModel
                                boundsBehavior: Flickable.StopAtBounds

                                spacing: 10

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
                                    id: sourceWrapper
                                    property variant myData: model
                                    property int delegateIndex: index
                                    property bool isSelected: ListView.isCurrentItem
                                    color: isSelected ? Colors.selectedArtworkBackground : Colors.defaultDarkColor
                                    width: parent.width - 10
                                    anchors.left: parent.left
                                    anchors.leftMargin: 5
                                    height: 31

                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: {
                                            if (presetNamesListView.currentIndex != sourceWrapper.delegateIndex) {
                                                presetNamesListView.currentIndex = sourceWrapper.delegateIndex
                                            }
                                        }
                                    }

                                    RowLayout {
                                        spacing: 10
                                        anchors.fill: parent

                                        Item {
                                            width: 1
                                        }

                                        StyledText {
                                            id: infoTitle
                                            Layout.fillWidth: true
                                            anchors.verticalCenter: parent.verticalCenter
                                            height: 31
                                            text: name
                                            font.bold: sourceWrapper.isSelected
                                            elide: Text.ElideMiddle
                                        }

                                        CloseIcon {
                                            id: closeIcon
                                            width: 14
                                            height: 14
                                            anchors.verticalCenterOffset: 1
                                            isActive: false

                                            onItemClicked: {
                                                confirmRemoveItemDialog.itemIndex = sourceWrapper.delegateIndex
                                                confirmRemoveItemDialog.open()
                                            }
                                        }

                                        Item {
                                            id: placeholder2
                                            width: 1
                                        }
                                    }
                                }
                            }
                        }
                    }

                    Rectangle {
                        color: Colors.defaultControlColor
                        height: 40
                        anchors.left: parent.left
                        anchors.right: parent.right

                        StyledAddHostButton {
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.leftMargin: 10
                            anchors.rightMargin: 10
                            anchors.verticalCenter: parent.verticalCenter
                            text: i18.n + qsTr("Add new Preset")
                            onClicked: {
                                presetsModel.addItem()
                                presetNamesListView.currentIndex = presetNamesListView.count - 1
                            }
                        }
                    }
                }
            }

            Item {
                id: editsPanel
                anchors.left: listPanel.right
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.bottom: bottomPanel.top

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 4

                    StyledText {
                        text: i18.n + qsTr("Title:")
                        isActive: presetNamesListView.count > 0
                    }

                    Rectangle {
                        id: titleWrapper
                        border.width: titleText.activeFocus ? 1 : 0
                        border.color: Colors.artworkActiveColor
                        Layout.fillWidth: true
                        color: enabled ? Colors.inputBackgroundColor : Colors.inputInactiveBackground
                        height: 30

                        StyledTextInput {
                            id: titleText
                            height: 30
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.rightMargin: 5
                            text: presetNamesListView.currentItem ? presetNamesListView.currentItem.myData.name : ""
                            anchors.leftMargin: 5

                            onTextChanged: {
                                if (presetNamesListView.currentItem) {
                                    presetNamesListView.currentItem.myData.editname = text
                                }
                            }

                            Keys.onTabPressed: {
                                flv.activateEdit()
                            }

                            onEditingFinished: {
                                if (text.length == 0) {
                                    if (presetNamesListView.currentItem) {
                                        presetNamesListView.currentItem.myData.editname = qsTr("Untitled")
                                    }
                                }
                            }

                            onActiveFocusChanged: {
                                if (!activeFocus) {
                                    //ftpListAC.cancelCompletion()
                                }
                            }

                            validator: RegExpValidator {
                                // copy paste in keys.onpressed Paste
                                regExp: /[a-zA-Z0-9 _-]*$/
                            }

                            Keys.onPressed: {
                                if (event.matches(StandardKey.Paste)) {
                                    var clipboardText = clipboard.getText();
                                    clipboardText = clipboardText.replace(/(\r\n|\n|\r)/gm, '');
                                    // same regexp as in validator
                                    var sanitizedText = clipboardText.replace(/[^a-zA-Z0-9 _-]/g, '');
                                    titleText.paste(sanitizedText)
                                    event.accepted = true
                                }
                            }
                        }
                    }

                    Item {
                        height: 1
                    }

                    RowLayout {
                        spacing: 5

                        StyledText {
                            text: i18.n + qsTr("Keywords:")
                            isActive: presetNamesListView.count > 0
                        }

                        Item {
                            Layout.fillWidth: true
                        }

                        StyledText {
                            text: presetNamesListView.currentItem ? presetNamesListView.currentItem.myData.keywordscount : ""
                        }
                    }

                    Rectangle {
                        id: keywordsWrapper
                        anchors.left: parent.left
                        anchors.right: parent.right
                        Layout.fillHeight: true
                        border.color: Colors.artworkActiveColor
                        border.width: flv.isFocused ? 1 : 0
                        color: Colors.inputBackgroundColor

                        function removeKeyword(index) {
                            presetsModel.removeKeywordAt(presetNamesListView.currentIndex, index)
                        }

                        function removeLastKeyword() {
                            presetsModel.removeLastKeyword(presetNamesListView.currentIndex)
                        }

                        function appendKeyword(keyword) {
                            presetsModel.appendKeyword(presetNamesListView.currentIndex, keyword)
                        }

                        function pasteKeywords(keywordsList) {
                            presetsModel.pasteKeywords(presetNamesListView.currentIndex, keywordsList)
                        }

                        EditableTags {
                            id: flv
                            anchors.fill: parent
                            enabled: presetNamesListView.currentIndex >= 0
                            model: presetsModel.getKeywordsModel(presetNamesListView.currentIndex)
                            property int keywordHeight: 20 * settingsModel.keywordSizeScale + (settingsModel.keywordSizeScale - 1)*10
                            scrollStep: keywordHeight

                            delegate: KeywordWrapper {
                                id: kw
                                isHighlighted: true
                                keywordText: keyword
                                hasSpellCheckError: !iscorrect
                                delegateIndex: index
                                itemHeight: flv.keywordHeight
                                onRemoveClicked: presetsModel.removeKeywordAt(presetNamesListView.currentIndex, index)
                                onActionDoubleClicked: {
                                    var callbackObject = {
                                        onSuccess: function(replacement) {
                                            presetsModel.editKeyword(kw.delegateIndex, replacement)
                                        },
                                        onClose: function() {
                                            flv.activateEdit()
                                        }
                                    }

                                    var basicModel = presetsModel.getBasicModel(presetNamesListView.currentIndex)

                                    Common.launchDialog("Dialogs/EditKeywordDialog.qml",
                                                        componentParent,
                                                        {
                                                            callbackObject: callbackObject,
                                                            previousKeyword: keyword,
                                                            keywordIndex: kw.delegateIndex,
                                                            keywordsModel: basicModel
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
                                titleText.forceActiveFocus()
                                titleText.cursorPosition = titleText.text.length
                            }

                            onCompletionRequested: {
                                helpersWrapper.autoCompleteKeyword(prefix,
                                                                   flv.model)
                            }
                        }

                        CustomScrollbar {
                            anchors.topMargin: -5
                            anchors.bottomMargin: -5
                            anchors.rightMargin: -15
                            flickable: flv
                        }
                    }
                }
            }

            RowLayout {
                id: bottomPanel
                anchors.leftMargin: 20
                anchors.left: listPanel.right
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                height: childrenRect.height
                anchors.bottomMargin: 20
                anchors.rightMargin: 20

                StyledText {
                    id: plainTextText
                    text: i18.n + qsTr("<u>edit in plain text</u>")
                    color: plainTextMA.containsMouse ? Colors.linkClickedColor : Colors.labelActiveForeground
                    enabled: presetNamesListView.currentItem ? true : false
                    visible: presetNamesListView.count > 0

                    MouseArea {
                        id: plainTextMA
                        anchors.fill: parent
                        hoverEnabled: true
                        preventStealing: true
                        cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor
                        onClicked: {
                            // strange bug with clicking on the keywords field
                            if (!containsMouse) { return; }
                            if (!presetNamesListView.currentItem) { return; }

                            var callbackObject = {
                                onSuccess: function(text, spaceIsSeparator) {
                                    presetsModel.plainTextEdit(presetNamesListView.currentIndex, text, spaceIsSeparator)
                                },
                                onClose: function() {
                                    flv.activateEdit()
                                }
                            }

                            var basicModel =  presetsModel.getKeywordsModel(presetNamesListView.currentIndex)

                            Common.launchDialog("../Dialogs/PlainTextKeywordsDialog.qml",
                                                applicationWindow,
                                                {
                                                    callbackObject: callbackObject,
                                                    keywordsText: presetNamesListView.currentItem.myData.keywordsstring,
                                                    keywordsModel: basicModel
                                                });
                        }
                    }
                }

                Item {
                    Layout.fillWidth: true
                }

                StyledButton {
                    text: i18.n + qsTr("Close")
                    width: 100
                    onClicked: {
                        presetsModel.saveToConfig();
                        closePopup();
                    }
                }
            }
        }
    }
}
