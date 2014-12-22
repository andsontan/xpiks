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
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import "Colors.js" as Colors

ApplicationWindow {
    id: settingsWindow
    modality: "ApplicationModal"
    width: 600
    height: 180

    property string defaultExifTool: "exiftool"
    property string defaultCurl: "curl"

    property string exiftoolpathkey: appSettings.exifToolPathKey
    property string exifToolPath: appSettings.value(exiftoolpathkey, defaultExifTool)

    property string curlpathkey: appSettings.curlPathKey
    property string curlPath: appSettings.value(curlpathkey, defaultCurl)

    FileDialog {
        id: fileDialog
        title: "Please choose ExifTool"
        selectExisting: true
        selectMultiple: false
        nameFilters: [ "All files (*)" ]

        onAccepted: {
            console.log("You chose: " + fileDialog.fileUrl)
            var path = fileDialog.fileUrl.toString().replace(/^(file:\/{3})/,"");
            exifToolPath = decodeURIComponent(path);
            exifToolText.text = exifToolPath;
        }

        onRejected: {
            console.log("File dialog canceled")
        }
    }

    Rectangle {
        color: Colors.selectedArtworkColor
        anchors.fill: parent

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 5

            GridLayout {
                width: parent.width
                rows: 2
                columns: 4
                rowSpacing: 10
                columnSpacing: 5

                Text {
                    Layout.row: 0
                    Layout.column: 0
                    Layout.fillWidth: true
                    Layout.maximumWidth: 100

                    horizontalAlignment: Text.AlignRight
                    text: "ExifTool path:"
                    color: Colors.defaultLightColor
                    font.family: "Helvetica"
                    font.pixelSize: 12
                    renderType: Text.NativeRendering
                }

                Rectangle {
                    Layout.row: 0
                    Layout.column: 1
                    height: childrenRect.height
                    width: childrenRect.width + 5
                    color: Colors.defaultInputBackground
                    border.color: Colors.artworkActiveColor
                    border.width: exifToolText.activeFocus ? 1 : 0

                    TextInput {
                        id: exifToolText
                        width: 300
                        height: 20
                        clip: true
                        text: exifToolPath
                        anchors.left: parent.left
                        anchors.leftMargin: 5
                        verticalAlignment: Text.AlignVCenter
                        color: Colors.defaultLightColor
                        font.family: "Helvetica"
                        font.pixelSize: 12
                        renderType: Text.NativeRendering
                    }
                }

                StyledButton {
                    Layout.row: 0
                    Layout.column: 2
                    text: qsTr("Select...")
                    width: 50
                    Layout.preferredWidth: 50
                    onClicked: fileDialog.open()
                }

                StyledButton {
                    Layout.row: 0
                    Layout.column: 3
                    text: qsTr("Reset")
                    width: 50
                    Layout.preferredWidth: 50
                    onClicked: exifToolPath = defaultExifTool
                }

                Text {
                    Layout.row: 1
                    Layout.column: 0
                    Layout.fillWidth: true
                    Layout.maximumWidth: 100
                    horizontalAlignment: Text.AlignRight
                    text: "Curl path:"
                    color: Colors.defaultLightColor
                    font.family: "Helvetica"
                    font.pixelSize: 12
                    renderType: Text.NativeRendering
                }

                Rectangle {
                    Layout.row: 1
                    Layout.column: 1
                    height: childrenRect.height
                    width: childrenRect.width + 5
                    color: Colors.defaultInputBackground
                    border.color: Colors.artworkActiveColor
                    border.width: curlText.activeFocus ? 1 : 0

                    TextInput {
                        id: curlText
                        width: 300
                        height: 20
                        clip: true
                        text: curlPath
                        anchors.left: parent.left
                        anchors.leftMargin: 5
                        verticalAlignment: Text.AlignVCenter
                        color: Colors.defaultLightColor
                        font.family: "Helvetica"
                        font.pixelSize: 12
                        renderType: Text.NativeRendering
                    }
                }

                StyledButton {
                    Layout.row: 1
                    Layout.column: 2
                    text: qsTr("Select...")
                    width: 50
                    Layout.preferredWidth: 50
                    onClicked: fileDialog.open()
                }

                StyledButton {
                    Layout.row: 1
                    Layout.column: 3
                    text: qsTr("Reset")
                    width: 50
                    Layout.preferredWidth: 50
                    onClicked: curlPath = defaultCurl
                }
            }

            RowLayout {
                height: 24
                spacing: 10

                Item {
                    Layout.fillWidth: true
                }

                StyledButton {
                    text: qsTr("Save")
                    width: 100
                    onClicked: {
                        exifToolPath = exifToolText.text
                        appSettings.setValue(exiftoolpathkey, exifToolPath)

                        curlPath = curlText.text
                        appSettings.setValue(curlpathkey, curlPath)
                    }
                }

                StyledButton {
                    text: qsTr("Exit")
                    width: 100
                    onClicked: settingsWindow.destroy()
                }
            }
        }
    }
}

