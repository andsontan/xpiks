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

    function closePopup(){
        presetEditComponent.destroy();
        presetsModel.loadFromConfigModel();
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

                var dialogPoint = mapToItem(dialogWindow, mouse.x, mouse.y);
                if (!Common.isInComponent(dialogPoint, dialogWindow)) {
                    closePopup()
                }
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
            width: 630
            height: Qt.platform.os == "windows" ? 470 : (Qt.platform.os == "linux" ? 475 : 460)
            color: Colors.popupBackgroundColor
            anchors.centerIn: parent
            Component.onCompleted: anchors.centerIn = undefined

            ColumnLayout {
                spacing: 15
                anchors.fill: parent
                anchors.margins: 20

                Item {
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    ColumnLayout {
                        anchors.left: parent.left
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        id: presetsStack
                        width: 260
                        height: parent.height
                        spacing: 0

                        Rectangle {
                            height: 5
                            Layout.fillWidth: true
                            color: Colors.defaultControlColor
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            Layout.minimumWidth: 250
                            Layout.preferredWidth: 250
                            Layout.maximumWidth: 300

                            color: Colors.defaultControlColor

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
                                        color: ListView.isCurrentItem ? Colors.selectedArtworkBackground : Colors.defaultDarkColor
                                        width: parent.width - 10
                                        anchors.left: parent.left
                                        anchors.leftMargin: 5
                                        height: 31
                                        Layout.minimumWidth: 250

                                        RowLayout {
                                            spacing: 10
                                            anchors.fill: parent

                                            Item {
                                                width: 1
                                            }

                                            StyledTextEdit {
                                                id: infoTitle
                                                Layout.fillWidth: true
                                                anchors.verticalCenter: parent.verticalCenter
                                                height: 31
                                                text: name
                                                font.bold: true

                                                onActiveFocusChanged: {
                                                    if(activeFocus) {
                                                        //TODO:
                                                        presetNamesListView.currentIndex = delegateIndex;
                                                    }
                                                    name = text;
                                                }
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
                            Layout.fillWidth: true



                            RowLayout {
                                anchors.fill: parent
                                anchors.bottomMargin: 5
                                anchors.topMargin: 5
                                spacing: 0

                                Item {
                                    width: 10
                                }

                                StyledAddHostButton {
                                    Layout.fillWidth: true
                                    text: i18.n + qsTr("Add new Preset")
                                    onClicked: {
                                        presetsModel.addItem()
                                        presetNamesListView.currentIndex = presetNamesListView.count - 1
                                    }
                                }

                                Item {
                                    width: 10
                                }
                            }
                        }
                    }

                    ColumnLayout {
                        anchors.left: presetsStack.right
                        anchors.leftMargin: 20
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        id: keywords
                        width: 300
                        height: parent.height
                        spacing: 0

                        Rectangle {
                            height: 5
                            Layout.fillWidth: true
                            color: Colors.defaultControlColor
                        }

                        Rectangle {
                            id: keywordsWrapper
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            Layout.minimumWidth: 250
                            Layout.preferredWidth: 250
                            Layout.maximumWidth: 300

                            color: Colors.defaultControlColor


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


                            StyledScrollView {
                                anchors.fill: parent
                                anchors.margins: { left: 5; top: 5; right: 5; bottom: 5 }


                                EditableTags{
                                    id: flv
                                    anchors.fill: parent
                                    enabled: presetNamesListView.currentIndex  >=0
                                    model: presetsModel.getKeywordsModel(presetNamesListView.currentIndex )
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

                                            Common.launchDialog("Dialogs/EditKeywordDialog.qml",
                                                                componentParent,
                                                                {
                                                                    callbackObject: callbackObject,
                                                                    previousKeyword: keyword,
                                                                    keywordIndex: kw.delegateIndex,
                                                                    keywordsModel:  presetsModel.getBasicModel(sourceWrapper.delegateIndex)
                                                                })
                                        }

                                        onActionRightClicked: {
                                            var showAddToDict = !iscorrect
                                            wordRightClickMenu.showAddToDict = showAddToDict
                                            wordRightClickMenu.word = kw.keywordText;
                                            wordRightClickMenu.showExpandPreset = false
                                            if (wordRightClickMenu.showAddToDict ||
                                                    wordRightClickMenu.showExpandPreset) {
                                                wordRightClickMenu.popup()
                                            }
                                        }
                                    }

                                    onTagAdded: {
                                        presetsModel.appendKeyword(presetNamesListView.currentIndex, text)
                                    }

                                    onRemoveLast: {
                                        presetsModel.removeLastKeyword()
                                    }

                                    onTagsPasted: {
                                        presetsModel.pasteKeywords(tagsList)
                                    }

                                    onBackTabPressed: {
                                        descriptionTextInput.forceActiveFocus()
                                        descriptionTextInput.cursorPosition = descriptionTextInput.text.length
                                    }

                                    onCompletionRequested: {
                                        helpersWrapper.autoCompleteKeyword(prefix,
                                                                           flv.model)
                                    }
                                }
                            }
                        }
                    }

                    Item {
                        anchors.top: parent.top
                        anchors.leftMargin: 10
                        anchors.left: presetsStack.right
                        anchors.right: parent.right
                        width: 290
                        height: (Qt.platform.os == "osx") ? parent.height : parent.height + 10


                    }
                }

                RowLayout {
                    height: 24
                    spacing: 20


                    StyledButton {
                        id: uploadButton
                        text: i18.n + qsTr("Save")
                        width: 130
                        onClicked: {
                            focus = true
                            presetsModel.saveToConfig();
                            presetEditComponent.destroy();
                        }
                    }

                    StyledButton {
                        text: i18.n + qsTr("Close")
                        width: 120
                        onClicked: {
                            closePopup();
                        }
                    }
                }
            }
        }
    }
}
