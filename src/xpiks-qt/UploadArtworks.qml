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
import "Colors.js" as Colors

Item {
    id: uploadArtworksComponent
    anchors.fill: parent

    function closePopup() {
        uploadArtworksComponent.destroy()
    }

    PropertyAnimation { target: uploadArtworksComponent; property: "opacity";
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
            height: 300
            color: Colors.selectedArtworkColor
            anchors.centerIn: parent

            ColumnLayout {
                spacing: 10
                anchors.fill: parent
                anchors.margins: 20

                StyledText {
                    text: qsTr("Upload artworks")
                }

                GridLayout {
                    rows: 3
                    columns: 2
                    rowSpacing: 5
                    columnSpacing: 10

                    StyledText {
                        Layout.row: 0
                        Layout.column: 0
                        Layout.fillWidth: true
                        Layout.maximumWidth: 150
                        horizontalAlignment: Text.AlignRight
                        text: qsTr("FTP host:")
                    }

                    Rectangle {
                        Layout.row: 0
                        Layout.column: 1
                        color: Colors.defaultInputBackground
                        border.color: Colors.artworkActiveColor
                        border.width: ftpHost.activeFocus ? 1 : 0

                        width: 200
                        height: 20

                        StyledTextInput {
                            id: ftpHost
                            anchors.fill: parent
                            text: artworkUploader.host
                            enabled: !artworkUploader.inProgress
                            anchors.leftMargin: 5
                            onTextChanged: artworkUploader.host = text
                            verticalAlignment: TextInput.AlignVCenter
                            KeyNavigation.tab: ftpUsername
                            KeyNavigation.priority: KeyNavigation.BeforeItem
                        }
                    }

                    StyledText {
                        Layout.row: 1
                        Layout.column: 0
                        Layout.fillWidth: true
                        Layout.maximumWidth: 150
                        text: qsTr("FTP User name:")
                        horizontalAlignment: Text.AlignRight
                    }

                    Rectangle {
                        Layout.row: 1
                        Layout.column: 1
                        color: Colors.defaultInputBackground
                        border.color: Colors.artworkActiveColor
                        border.width: ftpUsername.activeFocus ? 1 : 0

                        width: 200
                        height: 20

                        StyledTextInput {
                            id: ftpUsername
                            anchors.fill: parent
                            text: artworkUploader.username
                            enabled: !artworkUploader.inProgress
                            anchors.leftMargin: 5
                            onTextChanged: artworkUploader.username = text
                            KeyNavigation.tab: ftpPassword
                            KeyNavigation.backtab: ftpHost
                        }
                    }

                    StyledText {
                        Layout.row: 2
                        Layout.column: 0
                        text: qsTr("FTP User password:")
                        Layout.fillWidth: true
                        Layout.maximumWidth: 150
                        horizontalAlignment: Text.AlignRight
                    }

                    Rectangle {
                        Layout.row: 2
                        Layout.column: 1
                        color: Colors.defaultInputBackground
                        border.color: Colors.artworkActiveColor
                        border.width: ftpPassword.activeFocus ? 1 : 0

                        width: 200
                        height: 20

                        StyledTextInput {
                            id: ftpPassword
                            anchors.fill: parent
                            enabled: !artworkUploader.inProgress
                            anchors.leftMargin: 5
                            echoMode: TextInput.Password
                            text: artworkUploader.password
                            onTextChanged: artworkUploader.password = text
                            KeyNavigation.backtab: ftpUsername
                        }
                    }
                }

                StyledCheckbox {
                    text: qsTr("Include EPS (for illustrations)")
                    checked: artworkUploader.includeEPS
                    onCheckedChanged: artworkUploader.includeEPS = checked
                }

                SimpleProgressBar {
                    id: progress
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: parent.width
                    height: 20
                    color: artworkUploader.isError ? "red" : "#77B753"
                    value: artworkUploader.percent
                }

                RowLayout {
                    height: 24
                    Item {
                        Layout.fillWidth: true
                    }

                    StyledButton {
                        id: uploadButton
                        text: qsTr("Start Upload")
                        width: 130
                        enabled: !(artworkUploader.inProgress || ftpHost.length == 0 || ftpUsername.length == 0)
                        onClicked: {
                            var ftpString = ftpHost.text.substring(0, 6)
                            var ftpsString = ftpHost.text.substring(0, 7)
                            if (ftpString != "ftp://" && ftpsString != "ftps://") {
                                ftpHost.text = "ftp://" + ftpHost.text
                            }

                            text = qsTr("Uploading...")
                            artworkUploader.resetModel()
                            artworkUploader.uploadArtworks()
                        }

                        Connections {
                            target: artworkUploader
                            onFinishedProcessing: {
                                uploadButton.text = qsTr("Start Upload")
                            }
                        }
                    }

                    StyledButton {
                        text: qsTr("Close")
                        width: 100
                        enabled: !artworkUploader.inProgress
                        onClicked: {
                            artItemsModel.updateAllProperties()
                            closePopup()
                        }
                    }
                }
            }
        }
    }

    Component.onCompleted: ftpHost.forceActiveFocus()
}
