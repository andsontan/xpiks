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
import QtQuick.Controls.Styles 1.1
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import "../Constants"
import "../Common.js" as Common;
import "../Components"
import "../StyledControls"
import "../Constants/UIConfig.js" as UIConfig

Item {
    id: failedUploadsComponent
    anchors.fill: parent

    property var artworkUploader: helpersWrapper.getArtworkUploader()
    property var uploadWatcher: artworkUploader.getUploadWatcher()

    function closePopup() {
        failedUploadsComponent.destroy();
    }

    Keys.onEscapePressed: closePopup()
    Component.onCompleted: focus = true

    PropertyAnimation { target: failedUploadsComponent; property: "opacity";
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

            onPressed: {
                var tmp = mapToItem(failedUploadsComponent, mouse.x, mouse.y);
                old_x = tmp.x;
                old_y = tmp.y;

                var dialogPoint = mapToItem(dialogWindow, mouse.x, mouse.y);
                if (!Common.isInComponent(dialogPoint, dialogWindow)) {
                    closePopup()
                }
            }

            onPositionChanged: {
                var old_xy = Common.movePopupInsideComponent(failedUploadsComponent, dialogWindow, mouse, old_x, old_y);
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
            width: 680
            height: 450
            color: Colors.popupBackgroundColor
            anchors.centerIn: parent
            Component.onCompleted: anchors.centerIn = undefined

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 20

                RowLayout {
                    anchors.left: parent.left
                    anchors.right: parent.right

                    StyledText {
                        text: i18.n + qsTr("Failed uploads")
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    StyledText {
                        text: i18.n + getCaption()

                        function getCaption() {
                            return uploadWatcher.failedImagesCount === 1 ? qsTr("1 item") :
                                                                           qsTr("%1 items").arg(uploadWatcher.failedImagesCount)
                        }
                    }
                }

                Rectangle {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    Layout.fillHeight: true
                    color: Colors.defaultControlColor

                    ListView {
                        id: warningsListView
                        model: uploadWatcher
                        anchors.fill: parent
                        boundsBehavior: Flickable.StopAtBounds
                        spacing: 20
                        clip: true

                        header: Item {
                            height: 10
                        }

                        footer: Item {
                            height: 10
                        }

                        delegate: Rectangle {
                            property int delegateIndex: index
                            color: Colors.defaultDarkColor
                            id: hostRectangleWrapper
                            anchors.margins: 10
                            anchors.left: parent.left
                            anchors.right: parent.right
                            height: childrenRect.height + 20

                            RowLayout {
                                id: header
                                anchors.top: parent.top
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.topMargin: 10
                                anchors.leftMargin: 10
                                anchors.rightMargin: 10
                                spacing: 5

                                Rectangle {
                                    height: 5
                                    width: height
                                    radius: height/2
                                    color: Colors.labelInactiveForeground
                                }

                                StyledText {
                                    id: hostAddress
                                    text: artworkUploader.getFtpName(ftpaddress)
                                    color: Colors.artworkActiveColor
                                    //font.bold: true
                                    font.pixelSize: UIConfig.fontPixelSize + 4
                                }

                                Item {
                                    Layout.fillWidth: true
                                }

                                StyledText {
                                    id: hostName
                                    text: '(' + ftpaddress + ')'
                                    isActive: false
                                }
                            }

                            Flow {
                                id: flow
                                spacing: 20
                                anchors.topMargin: 20
                                anchors.leftMargin: 10
                                anchors.rightMargin: 10
                                anchors.top: header.bottom
                                anchors.left: parent.left
                                anchors.right: parent.right

                                Repeater {
                                    id: photosGrid
                                    model: failedimages

                                    delegate: Item {
                                        id: imageItem
                                        width: 100
                                        height: 120
                                        property string delegateData: modelData

                                        Item {
                                            id: imageHost
                                            height: 100
                                            anchors.left: parent.left
                                            anchors.top: parent.top
                                            anchors.right: parent.right

                                            Image {
                                                id: artworkImage
                                                anchors.fill: parent
                                                source: "image://cached/" + helpersWrapper.toImagePath(imageItem.delegateData)
                                                sourceSize.width: 150
                                                sourceSize.height: 150
                                                fillMode: settingsModel.fitSmallPreview ? Image.PreserveAspectFit : Image.PreserveAspectCrop
                                                asynchronous: true
                                                cache: false
                                            }

                                            Image {
                                                id: imageTypeIcon
                                                property bool isVector: helpersWrapper.isVector(imageItem.delegateData)
                                                visible: isVector
                                                enabled: isVector
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
                                            anchors.topMargin: 5
                                            text: imageItem.delegateData.split(/[\\/]/).pop()
                                            width: parent.width
                                            elide: Text.ElideMiddle
                                            horizontalAlignment: Text.AlignHCenter
                                            isActive: false
                                        }
                                    }
                                }
                            }
                        }
                    }

                    CustomScrollbar {
                        id: mainScrollBar
                        anchors.topMargin: 0
                        anchors.bottomMargin: 0
                        anchors.rightMargin: -17
                        flickable: warningsListView
                    }
                }

                RowLayout {
                    anchors.left: parent.left
                    anchors.right: parent.right

                    Item {
                        Layout.fillWidth: true
                    }

                    StyledButton {
                        text: i18.n + qsTr("Close")
                        width: 100
                        onClicked: closePopup()
                    }
                }
            }
        }
    }
}
