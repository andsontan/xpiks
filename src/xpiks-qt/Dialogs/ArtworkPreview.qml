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
import "../Constants/Colors.js" as Colors;
import "../Common.js" as Common;
import "../Components"
import "../StyledControls"

Item {
    id: artworkPreviewDialog
    anchors.fill: parent

    property string imagePath: ''
    property int artworkIndex: -1

    signal dialogDestruction();
    Component.onDestruction: dialogDestruction();

    function closePopup() {
        artworkPreviewDialog.destroy()
    }

    Connections {
        target: artItemsModel
        onFileUnavailable: {
            if (artworkIndex == index)
                closePopup()
        }
    }


    Component.onCompleted: focus = true
    Keys.onEscapePressed: closePopup()

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
            onDoubleClicked: mouse.accepted = true

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

        RectangularGlow {
            anchors.fill: dialogWindow
            anchors.topMargin: glowRadius/2
            glowRadius: 4
            spread: 0.0
            color: Colors.defaultControlColor
            cornerRadius: glowRadius
        }

        // This rectangle is the actual popup
        Rectangle {
            id: dialogWindow
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.topMargin: 60
            anchors.bottomMargin: 60
            anchors.horizontalCenter: parent.horizontalCenter
            color: Colors.selectedArtworkColor
            width: childrenRect.width

            Component.onCompleted: anchors.horizontalCenter = undefined

            Rectangle {
                id: boundsRect
                color: Colors.defaultControlColor
                width: parent.height
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom

                StyledScrollView {
                    id: scrollview
                    anchors.fill: parent
                    anchors.leftMargin: 10
                    anchors.topMargin: 10

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
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.left: boundsRect.right
                color: Colors.defaultDarkColor
                width: 210

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
                        Component.onCompleted: {
                            var size = artItemsModel.retrieveImageSize(artworkIndex)
                            text = "W %1 x H %2".arg(size.width).arg(size.height)
                        }
                    }

                    StyledText {
                        id: sizeText
                        color: Colors.selectedMetadataColor
                        text: '*'
                        Component.onCompleted: text = artItemsModel.retrieveFileSize(artworkIndex)
                    }

                    Item {
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
                        Layout.fillWidth: true
                        height: 90

                        StyledText {
                            wrapMode: TextEdit.Wrap
                            anchors.fill: parent
                            color: Colors.selectedMetadataColor
                            text: '*'
                            Component.onCompleted: text = artItemsModel.getAttachedVectorPath(artworkIndex)
                            height: 90
                            elide: Text.ElideRight
                        }
                    }

                    Item {
                        Layout.fillHeight: true
                    }

                    RowLayout {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        spacing: 20

                        StyledButton {
                            text: i18.n + qsTr("100%")
                            width: parent.width / 2 - 10
                            onClicked: {
                                previewImage.width = previewImage.sourceSize.width
                                previewImage.height = previewImage.sourceSize.height
                                scrollview.anchors.leftMargin = 0
                                scrollview.anchors.topMargin = 0
                                previewImage.fillMode = Image.Pad
                            }
                        }

                        StyledButton {
                            text: i18.n + qsTr("Fit")
                            width: parent.width / 2 - 10
                            onClicked: {
                                previewImage.width = boundsRect.width - 20
                                previewImage.height = boundsRect.height - 20
                                scrollview.anchors.leftMargin = 10
                                scrollview.anchors.topMargin = 10
                                previewImage.fillMode = Image.PreserveAspectFit
                            }
                        }
                    }

                    Item {
                        height: 1
                    }

                    StyledButton {
                        text: i18.n + qsTr("Close")
                        width: parent.width
                        anchors.horizontalCenter: parent.horizontalCenter
                        onClicked: closePopup()
                    }
                }
            }
        }
    }
}
