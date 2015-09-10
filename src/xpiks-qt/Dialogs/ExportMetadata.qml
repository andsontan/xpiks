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
    id: metadataExportComponent
    anchors.fill: parent

    function closePopup() {
        metadataExportComponent.destroy()
    }

    PropertyAnimation { target: metadataExportComponent; property: "opacity";
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

            property real old_x : 0
            property real old_y : 0

            onPressed:{
                var tmp = mapToItem(metadataExportComponent, mouse.x, mouse.y);
                old_x = tmp.x;
                old_y = tmp.y;

                var dialogPoint = mapToItem(dialogWindow, mouse.x, mouse.y);
                if (!Common.isInComponent(dialogPoint, dialogWindow)) {
                    if (!iptcProvider.inProgress) {
                        closePopup()
                    }
                }
            }

            onPositionChanged: {
                var old_xy = Common.movePopupInsideComponent(metadataExportComponent, dialogWindow, mouse, old_x, old_y);
                old_x = old_xy[0]; old_y = old_xy[1];
            }
        }

        // This rectangle is the actual popup
        Rectangle {
            id: dialogWindow
            width: 480
            height: 150
            color: Colors.selectedArtworkColor
            anchors.centerIn: parent
            Component.onCompleted: anchors.centerIn = undefined

            ColumnLayout {
                spacing: 10
                anchors.fill: parent
                anchors.margins: 20

                RowLayout {
                    Layout.fillWidth: true

                    StyledText {
                        text: qsTr("Export metadata")
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    StyledText {
                        text: qsTr("%1 modified image(s) selected").arg(iptcProvider.itemsCount)
                        color: Colors.defaultInputBackground
                    }
                }

                SimpleProgressBar {
                    id: progress
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: parent.width
                    height: 20
                    color: iptcProvider.isError ? Colors.destructiveColor : Colors.artworkActiveColor
                    value: iptcProvider.percent
                }

                RowLayout {
                    height: 24

                    StyledCheckbox {
                        text: qsTr("Backup each image")
                        checked: iptcProvider.mustSaveOriginal
                        onCheckedChanged: iptcProvider.mustSaveOriginal = checked
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    StyledButton {
                        id: exportButton
                        text: qsTr("Start Export")
                        width: 130
                        enabled: !iptcProvider.inProgress
                        onClicked: {
                            text = qsTr("Exporting...")
                            iptcProvider.resetModel()
                            iptcProvider.exportMetadata()
                        }

                        Connections {
                            target: iptcProvider
                            onFinishedProcessing: {
                                exportButton.text = qsTr("Start Export")
                                if (!iptcProvider.isError) {
                                    artItemsModel.updateAllProperties()
                                    filteredArtItemsModel.setSelectedItemsSaved()
                                    closePopup()
                                }
                            }
                        }
                    }

                    Item {
                        width: 10
                    }

                    StyledButton {
                        text: qsTr("Close")
                        width: 100
                        enabled: !iptcProvider.inProgress
                        onClicked: {
                            closePopup()
                        }
                    }
                }
            }
        }
    }
}
