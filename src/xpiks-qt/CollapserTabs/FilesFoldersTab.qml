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

import QtQuick 2.0
import QtQuick.Layouts 1.1
import "../Constants" 1.0
import "../Components"
import "../StyledControls"
import "../Dialogs"
import "../Common.js" as Common
import "../Constants/UIConfig.js" as UIConfig

ColumnLayout {
    anchors.fill: parent
    anchors.leftMargin: 10
    anchors.rightMargin: 10
    anchors.topMargin: 15
    anchors.bottomMargin: 10
    spacing: 0
    enabled: mainStackView.areActionsAllowed

    StyledBlackButton {
        implicitHeight: 30
        height: 30
        anchors.left: parent.left
        anchors.right: parent.right
        text: i18.n + qsTr("Add directory")
        onClicked: chooseDirectoryDialog.open()
        enabled: (applicationWindow.openedDialogsCount == 0)
    }

    Item {
        height: 10
    }

    StyledBlackButton {
        implicitHeight: 30
        height: 30
        anchors.left: parent.left
        anchors.right: parent.right
        text: i18.n + qsTr("Add files", "button")
        action: addFilesAction
    }

    Item {
        height: 20
    }

    Item {
        Layout.fillHeight: true
        anchors.left: parent.left
        anchors.right: parent.right

        StyledScrollView {
            anchors.fill: parent
            anchors.topMargin: 5

            ListView {
                id: sourcesListView
                model: artworkRepository
                boundsBehavior: Flickable.StopAtBounds
                anchors.fill: parent

                spacing: 10

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
                    property int delegateIndex: index
                    color: isselected ? Colors.itemsSourceSelected : Colors.itemsSourceBackground
                    width: parent.width
                    height: 31
                    Layout.minimumWidth: 237

                    /*MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            filteredArtItemsModel.selectDirectory(sourceWrapper.delegateIndex)
                        }
                    }*/

                    RowLayout {
                        spacing: 10
                        anchors.fill: parent

                        Item {
                            id: placeholder1
                            width: 1
                        }

                        StyledText {
                            id: directoryPath
                            Layout.fillWidth: true
                            anchors.verticalCenter: parent.verticalCenter
                            height: 31
                            color: Colors.itemsSourceForeground
                            text: path + " (" + usedimagescount + ")"
                            elide: Text.ElideMiddle
                            font.bold: isselected

                            MouseArea {
                                id: selectionMA
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                hoverEnabled: true
                                onClicked: {
                                    artworkRepository.selectDirectory(sourceWrapper.delegateIndex)
                                }
                            }
                        }

                        CloseIcon {
                            width: 14
                            height: 14
                            anchors.verticalCenter: parent.verticalCenter
                            isActive: false
                            crossOpacity: 1

                            onItemClicked: {
                                if (mustUseConfirmation()) {
                                    confirmRemoveDirectoryDialog.directoryIndex = sourceWrapper.delegateIndex
                                    confirmRemoveDirectoryDialog.open()
                                } else {
                                    filteredArtItemsModel.removeArtworksDirectory(sourceWrapper.delegateIndex)
                                }
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
}
