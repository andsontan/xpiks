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
import QtQuick.Dialogs 1.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import "../Constants"
import "../Common.js" as Common
import "../Components"
import "../StyledControls"
import "../Constants/UIConfig.js" as UIConfig

Item {
    id: replaceSetupComponent
    property var filteredModel
    anchors.fill: parent
    signal conditionsChanged()

    function closePopup() {
        replaceSetupComponent.destroy()
    }

    function initReplace() {
        var titleFlag = searchInTitle.checkedState
        var descriptionFlag = searchInDescription.checkedState
        var keywordsFlag = searchInKeywords.checkedState
        var caseSensitiveFlag = caseSensitive.checkedState
        var replaceToStr = replaceTo.text.trim()
        var replaceFromStr = replaceFrom.text.trim()
        replaceModel.searchInTitle = titleFlag
        replaceModel.searchInDescription = descriptionFlag
        replaceModel.searchInKeywords = keywordsFlag
        replaceModel.caseSensitive = caseSensitiveFlag
        replaceModel.replaceFrom = replaceFromStr
        replaceModel.replaceTo = replaceToStr
        replaceModel.initArtworksList()
    }

    function isSearchValid() {
        var titleFlag = searchInTitle.checkedState
        var descriptionFlag = searchInDescription.checkedState
        var keywordsFlag = searchInKeywords.checkedState
        var anyFlag = titleFlag || descriptionFlag || keywordsFlag
        var replaceFromNotEmpty = (replaceFrom.text.trim().length > 0)
        var replaceToNotEmpty = (replaceTo.text.trim().length > 0)
        var result = (replaceFromNotEmpty && anyFlag && replaceToNotEmpty)
        return result
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

    FocusScope {
        anchors.fill: parent

        MouseArea {
            anchors.fill: parent
            onWheel: wheel.accepted = true
            onClicked: mouse.accepted = true
            onDoubleClicked: mouse.accepted = true

            property real old_x: 0
            property real old_y: 0

            onPressed: {
                var tmp = mapToItem(replaceSetupComponent, mouse.x, mouse.y)
                old_x = tmp.x
                old_y = tmp.y
            }

            onPositionChanged: {
                var old_xy = Common.movePopupInsideComponent(
                            replaceSetupComponent, dialogWindow, mouse,
                            old_x, old_y)
                old_x = old_xy[0]
                old_y = old_xy[1]
            }
        }

        RectangularGlow {
            anchors.fill: dialogWindow
            anchors.topMargin: glowRadius / 2
            anchors.bottomMargin: -glowRadius / 2
            glowRadius: 4
            spread: 0.0
            color: Colors.defaultControlColor
            cornerRadius: glowRadius
        }

        // This rectangle is the actual popup
        Rectangle {
            id: dialogWindow
            width: 450
            height: 220
            property int inputsWidth: 200
            color: Colors.selectedImageBackground
            anchors.centerIn: parent
            Component.onCompleted: anchors.centerIn = undefined

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 20

                Item {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: childrenRect.height

                    ColumnLayout {
                        id: inputsRow
                        anchors.left: parent.left
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        width: dialogWindow.inputsWidth
                        spacing: 0

                        StyledText {
                            text: i18.n + qsTr("Find:")
                        }

                        Item {
                            height: 5
                        }

                        Rectangle {
                            color: enabled ? Colors.inputBackgroundColor : Colors.inputInactiveBackground
                            border.width: (replaceFrom.activeFocus) ? 1 : 0
                            border.color: Colors.artworkActiveColor
                            width: dialogWindow.inputsWidth
                            height: UIConfig.textInputHeight
                            clip: true

                            StyledTextInput {
                                id: replaceFrom
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.leftMargin: 5
                                anchors.rightMargin: 5
                                anchors.verticalCenter: parent.verticalCenter
                                KeyNavigation.tab: replaceTo
                                onTextChanged: {
                                    replaceSetupComponent.conditionsChanged()
                                }
                            }
                        }

                        Item {
                            height: 42
                        }

                        StyledText {
                            text: i18.n + qsTr("Replace:")
                        }

                        Item {
                            height: 5
                        }

                        Rectangle {
                            color: enabled ? Colors.inputBackgroundColor : Colors.inputInactiveBackground
                            border.width: (replaceTo.activeFocus) ? 1 : 0
                            border.color: Colors.artworkActiveColor
                            width: dialogWindow.inputsWidth
                            height: UIConfig.textInputHeight
                            clip: true

                            StyledTextInput {
                                id: replaceTo
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.leftMargin: 5
                                anchors.rightMargin: 5
                                anchors.verticalCenter: parent.verticalCenter
                                KeyNavigation.backtab: replaceFrom
                                onTextChanged: {
                                    replaceSetupComponent.conditionsChanged()
                                }
                            }
                        }
                    }

                    ColumnLayout {
                        id: checkboxesRow
                        anchors.left: inputsRow.right
                        anchors.leftMargin: 30
                        anchors.right: parent.right
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        spacing: 20

                        StyledCheckbox {
                            id: searchInTitle
                            text: i18.n + qsTr("Search in title")

                            onClicked: {
                                replaceSetupComponent.conditionsChanged()
                            }
                        }

                        StyledCheckbox {
                            id: searchInDescription
                            text: i18.n + qsTr("Search in description")

                            onClicked: {
                                replaceSetupComponent.conditionsChanged()
                            }
                        }

                        StyledCheckbox {
                            id: searchInKeywords
                            text: i18.n + qsTr("Search in keywords")

                            onClicked: {
                                replaceSetupComponent.conditionsChanged()
                            }
                        }

                        StyledCheckbox {
                            id: caseSensitive
                            text: i18.n + qsTr("Case sensitive")
                        }
                    }
                }

                Item {
                    Layout.fillHeight: true
                }

                RowLayout {
                    id: submitRow
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: 24
                    spacing: 20

                    Item {
                        Layout.fillWidth: true
                    }

                    StyledButton {
                        id: replaceButton
                        enabled: isSearchValid()
                        text: i18.n + qsTr("Replace")
                        width: 100

                        onClicked: {
                            initReplace()
                            Common.launchDialog("Dialogs/ReplacePreview.qml",
                                                replaceSetupComponent,
                                                {componentParent: replaceSetupComponent
                                                })
                        }

                        Connections{
                            target: replaceSetupComponent
                            onConditionsChanged: replaceButton.enabled = isSearchValid()
                        }
                    }

                    StyledButton {
                        text: i18.n + qsTr("Cancel")
                        width: 100
                        onClicked: {
                            closePopup()
                        }
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        focus = true
        replaceFrom.forceActiveFocus()
    }
}
