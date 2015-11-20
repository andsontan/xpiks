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
    id: spellCheckSuggestionsDialog
    anchors.fill: parent
    property var callbackObject
    property bool initialized: false

    signal dialogDestruction();
    Component.onDestruction: dialogDestruction();

    Keys.onEscapePressed: {
    }

    function closePopup() {
        spellCheckSuggestionModel.clearModel()
        spellCheckSuggestionsDialog.destroy();
    }

    PropertyAnimation { target: spellCheckSuggestionsDialog; property: "opacity";
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
                var tmp = mapToItem(spellCheckSuggestionsDialog, mouse.x, mouse.y);
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
                var old_xy = Common.movePopupInsideComponent(spellCheckSuggestionsDialog, dialogWindow, mouse, old_x, old_y);
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
                spacing: 10
                anchors.fill: parent
                anchors.margins: 20

                StyledText {
                    text: qsTr("Suggestions")
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: Colors.defaultControlColor

                    StyledScrollView {
                        anchors.fill: parent
                        anchors.margins: 10

                        ListView {
                            id: suggestionsListView
                            model: spellCheckSuggestionModel
                            spacing: 5

                            delegate: Rectangle {
                                id: suggestionsWrapper
                                property int delegateIndex: index
                                color: Colors.itemsSourceBackground
                                width: parent.width - 10
                                height: childrenRect.height
                                property var data: spellCheckSuggestionModel.getSuggestionItself(delegateIndex)

                                StyledText {
                                    id: keywordText
                                    anchors.left: parent.left
                                    anchors.top: parent.top
                                    width: 150
                                    text: data.keyword
                                    elide: Text.ElideMiddle
                                }

                                Rectangle {
                                    anchors.left: keywordText.right
                                    anchors.leftMargin: 20
                                    anchors.top: parent.top
                                    anchors.right: parent.right

                                    Flow {
                                        anchors.left: parent.left
                                        anchors.right: parent.right
                                        anchors.top: parent.top
                                        spacing: 10

                                        Repeater {
                                            model: suggestionsWrapper.data

                                            delegate: SuggestionWrapper {
                                                property int suggestionIndex: index
                                                itemHeight: 20 * settingsModel.keywordSizeScale + (settingsModel.keywordSizeScale - 1)*10
                                                suggestionText: suggestion
                                                isSelected: isselected
                                                onActionClicked: replacementIndex = suggestionIndex
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                RowLayout {
                    Item {
                        Layout.fillWidth: true
                    }

                    StyledButton {
                        text: qsTr("Replace all")
                        onClicked: {
                            spellCheckSuggestionModel.submitCorrections()
                            closePopup()
                        }
                    }

                    StyledButton {
                        text: qsTr("Cancel")
                        onClicked: closePopup()
                    }
                }
            }
        }
    }
}
