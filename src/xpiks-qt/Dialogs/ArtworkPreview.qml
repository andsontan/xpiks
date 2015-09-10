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

import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Styles 1.2
import "../Constants"
import "../Constants/Colors.js" as Colors;
import "../Common.js" as Common;
import "../Components"
import "../StyledControls"

Item {
    id: artworkPreviewDialog
    anchors.fill: parent

    property string imagePath: ''
    property int artworkIndex: -1

    function closePopup() {
        artworkPreviewDialog.destroy()
    }

    PropertyAnimation { target: artworkPreviewDialog; property: "opacity";
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

            property real old_x : 0
            property real old_y : 0

            onPressed:{
                var tmp = mapToItem(artworkPreviewDialog, mouse.x, mouse.y);
                old_x = tmp.x;
                old_y = tmp.y;

                var dialogPoint = mapToItem(dialogWindow, mouse.x, mouse.y);
                if (!Common.isInComponent(dialogPoint, dialogWindow)) {
                    closePopup()
                }
            }

            onPositionChanged: {
                var old_xy = Common.movePopupInsideComponent(artworkPreviewDialog, dialogWindow, mouse, old_x, old_y);
                old_x = old_xy[0]; old_y = old_xy[1];
            }
        }
        // This rectangle is the actual popup
        Rectangle {
            id: dialogWindow
            width: 600
            height: 450
            color: Colors.selectedArtworkColor
            anchors.centerIn: parent
            Component.onCompleted: anchors.centerIn = undefined

            RowLayout {
                anchors.fill: parent
                spacing: 0

                Rectangle {
                    height: parent.height
                    width: height
                    color: Colors.defaultControlColor

                    Image {
                        source: "image://global/" + imagePath
                        sourceSize.width: parent.width - 30
                        sourceSize.height: parent.height - 30
                        fillMode: Image.PreserveAspectFit
                        anchors.centerIn: parent
                        asynchronous: true
                    }
                }

                Rectangle {
                    height: parent.height
                    width: 25
                    color: Colors.defaultDarkColor
                }

                Rectangle {
                    Layout.fillWidth: true
                    height: parent.height
                    color: Colors.defaultDarkColor

                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 10

                        Item {
                            height: 15
                        }

                        StyledText {
                            id: dimensionsText
                            color: Colors.selectedMetadataColor
                            text: '*'
                            Component.onCompleted: text = artItemsModel.retrieveImageSize(artworkIndex)
                        }

                        StyledText {
                            id: sizeText
                            color: Colors.selectedMetadataColor
                            text: '*'
                            Component.onCompleted: text = artItemsModel.retrieveFileSize(artworkIndex)
                        }

                        Rectangle {
                            color: "transparent"
                            Layout.fillWidth: true
                            height: 90

                            StyledText {
                                wrapMode: TextEdit.Wrap
                                anchors.fill: parent
                                color: Colors.selectedMetadataColor
                                text: imagePath
                                height: 90
                                elide: Text.ElideRight
                            }
                        }

                        Item {
                            Layout.fillHeight: true
                        }

                        StyledButton {
                            text: qsTr("Close")
                            width: 100
                            onClicked: closePopup()
                        }

                        Item {
                            height: 15
                        }
                    }
                }
            }
        }
    }
}
