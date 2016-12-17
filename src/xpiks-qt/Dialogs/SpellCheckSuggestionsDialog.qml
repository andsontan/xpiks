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
import "../Constants"
import "../Common.js" as Common;
import "../Components"
import "../StyledControls"
import "../Constants/UIConfig.js" as UIConfig

Item {
    id: spellCheckSuggestionsDialog
    anchors.fill: parent
    property var callbackObject
    property bool initialized: false
    property var spellCheckSuggestionModel: helpersWrapper.getSpellCheckSuggestionsModel()

    signal dialogDestruction();
    Component.onDestruction: dialogDestruction();

    Component.onCompleted: focus = true

    Keys.onEscapePressed: closePopup()

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
            }

            onPositionChanged: {
                var old_xy = Common.movePopupInsideComponent(spellCheckSuggestionsDialog, dialogWindow, mouse, old_x, old_y);
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
            width: 730
            height: 610
            color: Colors.popupBackgroundColor
            anchors.centerIn: parent
            Component.onCompleted: anchors.centerIn = undefined

            ColumnLayout {
                spacing: 10
                anchors.fill: parent
                anchors.margins: 20

                StyledText {
                    text: i18.n + qsTr("Suggestions")
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: Colors.defaultControlColor

                    Item {
                        visible: suggestionsListView.count == 0
                        anchors.fill: parent

                        StyledText {
                            text: i18.n + qsTr("No suggestions available")
                            isActive: false
                            anchors.centerIn: parent
                        }
                    }

                    StyledScrollView {
                        anchors.fill: parent
                        anchors.margins: 10
                        focus: true

                        ListView {
                            id: suggestionsListView
                            model: spellCheckSuggestionModel
                            focus: true
                            spacing: 5

                            delegate: Rectangle {
                                id: suggestionsWrapper
                                property int delegateIndex: index
                                color: Colors.panelColor
                                width: parent.width - 10
                                height: suggestionsListRect.height

                                Item {
                                    id: wordsColumn
                                    anchors.left: parent.left
                                    anchors.top: parent.top
                                    width: 100
                                    height: suggestionsListRect.height

                                    ColumnLayout {
                                        anchors.centerIn: parent

                                        StyledText {
                                            id: originText
                                            anchors.horizontalCenter: parent.horizontalCenter
                                            verticalAlignment: Text.AlignVCenter
                                            text: word
                                            font.pixelSize: UIConfig.fontPixelSize * settingsModel.keywordSizeScale
                                            color: Colors.artworkModifiedColor
                                            elide: Text.ElideMiddle
                                        }

                                        StyledText {
                                            anchors.horizontalCenter: parent.horizontalCenter
                                            verticalAlignment: Text.AlignVCenter
                                            visible: replacementorigin != word
                                            text: replacementorigin
                                            font.pixelSize: 11
                                            color: Colors.selectedArtworkBackground
                                            elide: Text.ElideMiddle
                                        }
                                    }
                                }

                                Item {
                                    id: suggestionsListRect
                                    anchors.left: wordsColumn.right
                                    anchors.leftMargin: 5
                                    anchors.top: parent.top
                                    anchors.right: parent.right
                                    height: childrenRect.height + 20

                                    Flow {
                                        anchors.left: parent.left
                                        anchors.leftMargin: 10
                                        anchors.right: parent.right
                                        anchors.rightMargin: 10
                                        anchors.top: parent.top
                                        anchors.topMargin: 10
                                        spacing: 10
                                        focus: true

                                        Repeater {
                                            model: spellCheckSuggestionModel.getSuggestionItself(delegateIndex)

                                            delegate: SuggestionWrapper {
                                                property int suggestionIndex: index
                                                itemHeight: 20 * settingsModel.keywordSizeScale + (settingsModel.keywordSizeScale - 1)*10
                                                suggestionText: suggestion
                                                isSelected: isselected
                                                onActionClicked: editreplacementindex = suggestionIndex
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                Item {
                    height: 1
                }

                RowLayout {
                    spacing: 20

                    StyledButton {
                        text: i18.n + qsTr("Reset all")
                        width: 100
                        onClicked: spellCheckSuggestionModel.resetAllSuggestions()
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    StyledButton {
                        text: i18.n + qsTr("Replace")
                        width: 100
                        onClicked: {
                            spellCheckSuggestionModel.submitCorrections()
                            closePopup()
                        }
                    }

                    StyledButton {
                        text: i18.n + qsTr("Cancel")
                        width: 80
                        onClicked: closePopup()
                    }
                }
            }
        }
    }
}
