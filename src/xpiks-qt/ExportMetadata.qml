/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014 Taras Kushnir <kushnirTV@gmail.com>
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

import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Styles 1.3

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

        // This rectangle is the actual popup
        Rectangle {
            id: dialogWindow
            width: 480
            height: 150
            radius: 10
            color: "#eeeeee"
            anchors.centerIn: parent

            ColumnLayout {
                spacing: 10
                anchors.fill: parent
                anchors.margins: 20

                Text {
                    text: qsTr("Export metadata")
                }

                SimpleProgressBar {
                    id: progress
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: parent.width
                    height: 20
                    color: iptcProvider.isError ? "red" : "#77B753"
                    value: iptcProvider.percent

                    onValueChanged: {
                        if (value == 100) {
                            exportButton.text = qsTr("Start Export")
                            artItemsModel.updateAllProperties()
                        }
                    }
                }

                RowLayout {
                    Item {
                        Layout.fillWidth: true
                    }

                    Button {
                        id: exportButton
                        text: qsTr("Start Export")
                        enabled: !iptcProvider.inProgress
                        onClicked: {
                            text = qsTr("Exporting...")
                            iptcProvider.resetModel()
                            iptcProvider.exportMetadata()
                        }
                    }

                    Button {
                        text: qsTr("Close")
                        enabled: !iptcProvider.inProgress
                        onClicked: {
                            if (!iptcProvider.isError) {
                                artItemsModel.setSelectedItemsSaved()
                            }

                            closePopup()
                        }
                    }
                }
            }
        }
    }
}
