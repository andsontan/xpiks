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
import QtQuick.Dialogs 1.1
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Layouts 1.1
import "../Constants"
import "../Constants/Colors.js" as Colors;
import "../Common.js" as Common;
import "../Components"
import "../StyledControls"

Item {
    id: warningsComponent
    anchors.fill: parent

    property variant componentParent
    property bool isRestricted: false

    signal dialogDestruction();
    Component.onDestruction: dialogDestruction();

    function closePopup() {
        warningsComponent.destroy()
    }

    Component.onCompleted: focus = true
    Keys.onEscapePressed: closePopup()

    PropertyAnimation { target: warningsComponent; property: "opacity";
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
                var tmp = mapToItem(warningsComponent, mouse.x, mouse.y);
                old_x = tmp.x;
                old_y = tmp.y;

                var dialogPoint = mapToItem(dialogWindow, mouse.x, mouse.y);
                if (!Common.isInComponent(dialogPoint, dialogWindow)) {
                    closePopup()
                }
            }

            onPositionChanged: {
                var old_xy = Common.movePopupInsideComponent(warningsComponent, dialogWindow, mouse, old_x, old_y);
                old_x = old_xy[0]; old_y = old_xy[1];
            }
        }

        // This rectangle is the actual popup
        Rectangle {
            id: dialogWindow
            width: 700
            height: 580
            color: Colors.selectedArtworkColor
            anchors.centerIn: parent
            Component.onCompleted: anchors.centerIn = undefined

            ColumnLayout {
                spacing: 10
                anchors.fill: parent
                anchors.margins: 20

                StyledText {
                    text: qsTr("Warnings")
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: Colors.defaultControlColor

                    StyledScrollView {
                        anchors.fill: parent
                        anchors.margins: 10

                        ListView {
                            id: warningsListView
                            model: warningsManager
                            spacing: 10

                            delegate: Rectangle {
                                property int delegateIndex: index
                                color: Colors.itemsSourceBackground
                                id: imageWrapper
                                width: parent.width - 10
                                height: 100
                                radius: 2

                                RowLayout {
                                    anchors.fill: parent
                                    spacing: 0

                                    Item {
                                        width: 120
                                        height: parent.height

                                        ColumnLayout {
                                            anchors.fill: parent
                                            anchors.margins: { left: 15; right: 15 }
                                            spacing: 7

                                            Item {
                                                width: 90
                                                height: 60
                                                anchors.horizontalCenter: parent.horizontalCenter

                                                Image {
                                                    anchors.fill: parent
                                                    source: "image://global/" + filename
                                                    sourceSize.width: 150
                                                    sourceSize.height: 150
                                                    fillMode: settingsModel.fitSmallPreview ? Image.PreserveAspectFit : Image.PreserveAspectCrop
                                                    asynchronous: true
                                                }
                                            }

                                            StyledText {
                                                Layout.fillWidth: true
                                                elide: Text.ElideMiddle
                                                color: Colors.defaultInputBackground
                                                horizontalAlignment: Text.AlignHCenter
                                                text: filename.split(/[\\/]/).pop()
                                                font.pointSize: 10
                                            }

                                            Item {
                                                Layout.fillHeight: true
                                            }
                                        }
                                    }

                                    Item {
                                        width: 5
                                    }

                                    Rectangle {
                                        id: columnRectangle
                                        height: parent.height
                                        Layout.fillWidth: true
                                        color: Colors.itemsSourceSelected

                                        ListView {
                                            id: warningsTextList
                                            anchors.fill: parent
                                            anchors.margins: 10
                                            boundsBehavior: Flickable.StopAtBounds
                                            model: warnings
                                            spacing: 10

                                            delegate: RowLayout {
                                                width: parent.width
                                                height: 10
                                                spacing: 5

                                                Rectangle {
                                                    height: 8
                                                    width: height
                                                    radius: height/2
                                                    anchors.verticalCenter: parent.verticalCenter
                                                    color: Colors.selectedArtworkColor
                                                }

                                                StyledText {
                                                    Layout.fillWidth: true
                                                    text: modelData
                                                    color: Colors.artworkModifiedColor
                                                }
                                            }
                                        }
                                    }

                                    Rectangle {
                                        width: 40
                                        height: parent.height
                                        color: Colors.selectedArtworkColor

                                        StyledButton {
                                            text: qsTr("Fix")
                                            width: 30
                                            anchors.centerIn: parent
                                            enabled: !isRestricted && warningsListView.count > 0

                                            onClicked: {
                                                Common.launchItemEditing(itemindex, componentParent,
                                                                         {
                                                                             applyCallback: function() {
                                                                                 console.log("Rechecking [" + imageWrapper.delegateIndex + "] item")
                                                                                 warningsManager.recheckItem(imageWrapper.delegateIndex)
                                                                             }
                                                                         })
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }

                    Item {
                        anchors.fill: parent
                        visible: warningsListView.count == 0

                        StyledText {
                            text: qsTr("There are no warnings")
                            anchors.centerIn: parent
                            color: Colors.selectedMetadataColor
                        }
                    }
                }

                Item {
                    height: 1
                }

                RowLayout {
                    spacing: 20
                    height: 24

                    Item {
                        Layout.fillWidth: true
                    }

                    StyledButton {
                        text: qsTr("Close")
                        width: 100
                        onClicked: {
                            closePopup()
                        }
                    }
                }
            }
        }
    }
}
