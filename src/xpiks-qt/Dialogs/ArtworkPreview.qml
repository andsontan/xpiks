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
    id: artworkPreviewDialog
    anchors.fill: parent

    property string imagePath: ''
    property int artworkIndex: -1

    signal onDialogDestruction();
    Component.onDestruction: onDialogDestruction();

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
            anchors.fill: parent
            anchors.margins: 50
            color: Colors.selectedArtworkColor

            Rectangle {
                id: boundsRect
                color: Colors.defaultControlColor
                height: parent.height
                width: height
                anchors.left: parent.left

                StyledScrollView {
                    id: scrollview
                    anchors.fill: parent
                    anchors.leftMargin: 10

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

            Rectangle {
                anchors.left: boundsRect.right
                anchors.right: parent.right
                height: parent.height
                color: Colors.defaultDarkColor

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 20
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

                    RowLayout {
                        width: 180
                        anchors.right: parent.right
                        anchors.rightMargin: 10
                        spacing: 10

                        StyledButton {
                            text: qsTr("100%")
                            width: 50
                            onClicked: {
                                previewImage.width = previewImage.sourceSize.width
                                previewImage.height = previewImage.sourceSize.height
                                scrollview.anchors.leftMargin = 0
                                previewImage.fillMode = Image.Pad
                            }
                        }

                        StyledButton {
                            text: qsTr("Fit")
                            width: 50
                            onClicked: {
                                previewImage.width = boundsRect.width - 20
                                previewImage.height = boundsRect.height - 20
                                scrollview.anchors.leftMargin = 10
                                previewImage.fillMode = Image.PreserveAspectFit
                            }
                        }

                        StyledButton {
                            text: qsTr("Close")
                            width: 50
                            onClicked: closePopup()
                        }
                    }
                }
            }
        }
    }
}
