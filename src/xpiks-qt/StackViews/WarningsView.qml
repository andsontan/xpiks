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
    color: Colors.defaultDarkColor
    property bool wasLeftSideCollapsed
    property bool isRestricted: false

    function closePopup() {
        mainStackView.pop()
        restoreLeftPane()
    }

    function restoreLeftPane() {
        if (mainStackView.depth === 1) {
            if (!wasLeftSideCollapsed) {
                expandLeftPane()
            }
        }
    }

    Rectangle {
        id: header
        color: Colors.defaultDarkColor
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        height: 45

        RowLayout {
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.right: parent.right
            height: childrenRect.height
            spacing: 0
            anchors.leftMargin: 20
            anchors.rightMargin: 40

            BackGlyphButton {
                text: i18.n + qsTr("Back")
                onClicked: closePopup()
            }

            Item {
                Layout.fillWidth: true
            }

            StyledText {
                text: i18.n + qsTr("Warnings")
                isActive: false
            }
        }
    }

    StyledScrollView {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: header.bottom
        anchors.bottom: parent.bottom
        anchors.leftMargin: 20
        anchors.rightMargin: 10
        anchors.bottomMargin: 20

        ListView {
            id: warningsListView
            model: warningsModel
            spacing: 20

            delegate: Rectangle {
                property int delegateIndex: index
                color: Colors.defaultControlColor
                id: imageWrapper
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.rightMargin: 10
                height: columnRectangle.height
                radius: 2

                Item {
                    id: imageItem
                    width: 180
                    anchors.left: parent.left
                    anchors.leftMargin: 10
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom

                    Item {
                        id: imageHost
                        anchors.top: parent.top
                        anchors.topMargin: 25
                        anchors.horizontalCenter: parent.horizontalCenter
                        width: 150
                        height: 120

                        Image {
                            id: artworkImage
                            anchors.fill: parent
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
                    }

                    StyledText {
                        anchors.top: imageHost.bottom
                        anchors.topMargin: 3
                        width: parent.width - 10
                        elide: Text.ElideMiddle
                        horizontalAlignment: Text.AlignHCenter
                        text: basefilename
                        isActive: false
                    }
                }

                Rectangle {
                    id: columnRectangle
                    anchors.left: imageItem.right
                    anchors.top: parent.top
                    anchors.right: parent.right
                    height: (childrenRect.height < 120) ? 180 : (childrenRect.height + 60)
                    color: Colors.defaultControlColor

                    Column {
                        id: warningsTextList
                        spacing: 15

                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.top: parent.top
                        anchors.leftMargin: 10
                        anchors.rightMargin: 10
                        anchors.topMargin: 30

                        Repeater {
                            id: warningsDescriptions
                            model: warningsModel.describeWarnings(imageWrapper.delegateIndex)

                            delegate: RowLayout {
                                width: warningsTextList.width
                                height: 10
                                spacing: 5

                                Rectangle {
                                    height: 6
                                    width: height
                                    radius: height/2
                                    anchors.verticalCenter: parent.verticalCenter
                                    color: Colors.inactiveControlColor
                                }

                                StyledText {
                                    Layout.fillWidth: true
                                    text: i18.n + modelData
                                    color: Colors.artworkModifiedColor
                                    anchors.verticalCenter: parent.verticalCenter
                                }
                            }
                        }
                    }
                }

                EditIcon {
                    backgroundColor: columnRectangle.color
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.verticalCenterOffset: -5
                    anchors.right: parent.right
                    anchors.rightMargin: 30
                    enabled: !isRestricted && warningsListView.count > 0
                    visible: enabled

                    onActionInvoked: {
                        var index = imageWrapper.delegateIndex
                        var originalIndex = warningsModel.getOriginalIndex(index);
                        var derivedIndex = filteredArtItemsModel.getDerivedIndex(originalIndex)
                        var metadata = filteredArtItemsModel.getArtworkMetadata(derivedIndex)
                        startOneItemEditing(metadata, derivedIndex, originalIndex)
                    }
                }
            }
        }
    }

    Item {
        anchors.fill: parent
        visible: warningsListView.count == 0

        StyledText {
            text: i18.n + qsTr("There are no warnings")
            anchors.centerIn: parent
            isActive: false
        }
    }
}
