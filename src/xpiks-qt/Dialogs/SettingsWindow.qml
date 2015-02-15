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
import "../Constants"
import "../Constants/Colors.js" as Colors;
import "../Components"
import "../StyledControls"

ApplicationWindow {
    id: settingsWindow
    modality: "ApplicationModal"
    width: 450
    height: 415
    minimumWidth: width
    maximumWidth: width
    minimumHeight: height
    maximumHeight: height
    flags: Qt.Tool

    function closeSettings() {
        settingsWindow.destroy();
    }

    function openMasterPasswordDialog(firstTimeParam) {
        var component = Qt.createComponent("MasterPasswordSetupDialog.qml");
        component.createObject(settingsWindow, {firstTime: firstTimeParam});
    }

    property string defaultExifTool: "exiftool"
    property string defaultCurl: "curl"
    property double defaultMaxKeywords: 50
    property int defaultMaxDescription: 200
    property double defaultMinMegapixels: 4.0

    property string exiftoolpathkey: appSettings.exifToolPathKey
    property string exifToolPath: appSettings.value(exiftoolpathkey, defaultExifTool)

    property string curlpathkey: appSettings.curlPathKey
    property string curlPath: appSettings.value(curlpathkey, defaultCurl)

    property string maxkeywordscountkey: appSettings.maxKeywordsCount;
    property int maxKeywordsCount: appSettings.value(maxkeywordscountkey, defaultMaxKeywords)

    property string maxdescriptionlengthkey: appSettings.maxDescriptionLength;
    property int maxDescriptionLength: appSettings.value(maxdescriptionlengthkey, defaultMaxDescription)

    property string minmegapixelskey: appSettings.minMegapixelCount;
    property double minMegapixelCount: appSettings.value(minmegapixelskey, defaultMinMegapixels)

    property string mustusemasterpasswordkey: appSettings.mustUseMasterPasswordKey
    property bool mustUseMasterPassword: appSettings.value(mustusemasterpasswordkey, false)

    property string masterpasswordhashkey: appSettings.masterPasswordHashKey

    FileDialog {
        id: exifToolFileDialog
        title: "Please choose ExifTool location"
        selectExisting: true
        selectMultiple: false
        nameFilters: [ "All files (*)" ]

        onAccepted: {
            console.log("You chose: " + exifToolFileDialog.fileUrl)
            var path = exifToolFileDialog.fileUrl.toString().replace(/^(file:\/{3})/,"");
            exifToolPath = decodeURIComponent(path);
            exifToolText.text = exifToolPath;
        }

        onRejected: {
            console.log("File dialog canceled")
        }
    }

    FileDialog {
        id: curlFileDialog
        title: "Please choose curl location"
        selectExisting: true
        selectMultiple: false
        nameFilters: [ "All files (*)" ]

        onAccepted: {
            console.log("You chose: " + curlFileDialog.fileUrl)
            var path = curlFileDialog.fileUrl.toString().replace(/^(file:\/{3})/,"");
            curlPath = decodeURIComponent(path);
            curlText.text = curlPath;
        }

        onRejected: {
            console.log("File dialog canceled")
        }
    }

    MessageDialog {
        id: masterPasswordOffWarningDialog
        title: "Warning"
        text: qsTr("Switching off master password will make your passwords storage less secure. Continue?")
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            secretsManager.resetMasterPassword()
            appSettings.setValue(mustusemasterpasswordkey, false)
            appSettings.setValue(masterpasswordhashkey, "")
        }

        onNo: {
            // revert checkbox state
            masterPasswordCheckbox.checked = true
        }
    }

    Rectangle {
        color: Colors.selectedArtworkColor
        anchors.fill: parent

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 10
            spacing: 5

            StyledText {
                text: qsTr("External tools paths:")
            }

            Rectangle {
                Layout.fillWidth: true
                height: 80
                radius: 2
                border.color: Colors.defaultInputBackground
                border.width: 2
                color: Colors.selectedArtworkColor

                GridLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    rows: 2
                    columns: 4
                    rowSpacing: 10
                    columnSpacing: 5

                    StyledText {
                        Layout.row: 0
                        Layout.column: 0
                        Layout.fillWidth: true
                        Layout.maximumWidth: 80

                        horizontalAlignment: Text.AlignRight
                        text: qsTr("ExifTool path:")
                    }

                    StyledInputHost {
                        border.width: exifToolText.activeFocus ? 1 : 0
                        Layout.row: 0
                        Layout.column: 1

                        StyledTextInput {
                            id: exifToolText
                            width: 150
                            height: 24
                            clip: true
                            text: exifToolPath
                            anchors.left: parent.left
                            anchors.leftMargin: 5
                            KeyNavigation.tab: curlText
                        }
                    }

                    StyledButton {
                        Layout.row: 0
                        Layout.column: 2
                        text: qsTr("Select...")
                        width: 70
                        onClicked: exifToolFileDialog.open()
                    }

                    StyledButton {
                        Layout.row: 0
                        Layout.column: 3
                        text: qsTr("Reset")
                        width: 70
                        onClicked: exifToolText.text = defaultExifTool
                    }

                    StyledText {
                        Layout.row: 1
                        Layout.column: 0
                        Layout.fillWidth: true
                        Layout.maximumWidth: 80
                        horizontalAlignment: Text.AlignRight
                        text: qsTr("Curl path:")
                    }

                    StyledInputHost {
                        border.width: curlText.activeFocus ? 1 : 0
                        Layout.row: 1
                        Layout.column: 1

                        StyledTextInput {
                            id: curlText
                            width: 150
                            height: 24
                            clip: true
                            text: curlPath
                            anchors.left: parent.left
                            anchors.leftMargin: 5
                            KeyNavigation.backtab: exifToolText
                            KeyNavigation.tab: megapixelsCount
                        }
                    }

                    StyledButton {
                        Layout.row: 1
                        Layout.column: 2
                        text: qsTr("Select...")
                        width: 70
                        onClicked: curlFileDialog.open()
                    }

                    StyledButton {
                        Layout.row: 1
                        Layout.column: 3
                        text: qsTr("Reset")
                        width: 70
                        onClicked: curlText.text = defaultCurl
                    }
                }
            }

            Item {
                height: 15
            }

            StyledText {
                text: qsTr("Upload warnings limits:")
            }

            Rectangle {
                Layout.fillWidth: true
                height: 120
                radius: 2
                border.color: Colors.defaultInputBackground
                border.width: 2
                color: Colors.selectedArtworkColor

                ColumnLayout {
                    spacing: 5
                    anchors.fill: parent
                    anchors.margins: 10

                    RowLayout {
                        width: parent.width
                        spacing: 10

                        StyledText {
                            Layout.preferredWidth: 130
                            horizontalAlignment: Text.AlignRight
                            text: qsTr("Minimum megapixels:")
                        }

                        StyledInputHost {
                            border.width: megapixelsCount.activeFocus ? 1 : 0

                            StyledTextInput {
                                id: megapixelsCount
                                width: 100
                                height: 24
                                clip: true
                                text: minMegapixelCount
                                anchors.left: parent.left
                                anchors.leftMargin: 5
                                KeyNavigation.backtab: curlText
                                KeyNavigation.tab: keywordsCount

                                validator: DoubleValidator {
                                    bottom: 0
                                    top: 100
                                    decimals: 1
                                    notation: "StandardNotation"
                                }
                            }
                        }
                    }

                    RowLayout {
                        width: parent.width
                        spacing: 10

                        StyledText {
                            Layout.preferredWidth: 130
                            horizontalAlignment: Text.AlignRight
                            text: qsTr("Max keywords count:")
                        }

                        StyledInputHost {
                            border.width: keywordsCount.activeFocus ? 1 : 0

                            StyledTextInput {
                                id: keywordsCount
                                width: 100
                                height: 24
                                clip: true
                                text: maxKeywordsCount
                                anchors.left: parent.left
                                anchors.leftMargin: 5
                                KeyNavigation.backtab: megapixelsCount
                                KeyNavigation.tab: descriptionLength

                                validator: IntValidator {
                                    bottom: 0
                                    top: 200
                                }
                            }
                        }
                    }

                    RowLayout {
                        width: parent.width
                        spacing: 10

                        StyledText {
                            Layout.preferredWidth: 130
                            horizontalAlignment: Text.AlignRight
                            text: qsTr("Max description length:")
                        }

                        StyledInputHost {
                            border.width: descriptionLength.activeFocus ? 1 : 0

                            StyledTextInput {
                                id: descriptionLength
                                width: 100
                                height: 24
                                clip: true
                                text: maxDescriptionLength
                                anchors.left: parent.left
                                anchors.leftMargin: 5
                                KeyNavigation.backtab: keywordsCount

                                validator: IntValidator {
                                    bottom: 0
                                    top: 1000
                                }
                            }
                        }
                    }
                }
            }

            Item {
                height: 15
            }

            StyledText {
                text: qsTr("FTP passwords:")
            }

            Rectangle {
                Layout.fillWidth: true
                height: 45
                radius: 2
                border.color: Colors.defaultInputBackground
                border.width: 2
                color: Colors.selectedArtworkColor

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 10

                    StyledCheckbox {
                        id: masterPasswordCheckbox
                        text: qsTr("Use Master password")
                        onClicked: {
                            if (masterPasswordCheckbox.checked) {
                                if (!mustUseMasterPassword) {
                                    openMasterPasswordDialog(true)
                                }
                            } else {
                                masterPasswordOffWarningDialog.open()
                            }
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    StyledButton {
                        width: 190
                        text: qsTr("Change Master password")
                        enabled: masterPasswordCheckbox.checked

                        onClicked: {
                            openMasterPasswordDialog(false)
                        }
                    }
                }
            }

            Item {
                height: 10
            }

            RowLayout {
                height: 24
                spacing: 0
                width: parent.width

                Item {
                    Layout.fillWidth: true
                }

                StyledButton {
                    text: qsTr("Save and Exit")
                    width: 120
                    onClicked: {
                        exifToolPath = exifToolText.text
                        appSettings.setValue(exiftoolpathkey, exifToolPath)

                        curlPath = curlText.text
                        appSettings.setValue(curlpathkey, curlPath)

                        minMegapixelCount = parseFloat(megapixelsCount.text)
                        appSettings.setValue(minmegapixelskey, minMegapixelCount)

                        maxKeywordsCount = parseInt(keywordsCount.text)
                        appSettings.setValue(maxkeywordscountkey, maxKeywordsCount)

                        maxDescriptionLength = parseInt(descriptionLength.text)
                        appSettings.setValue(maxdescriptionlengthkey, maxDescriptionLength)

                        closeSettings()
                    }
                }

                Item {
                    width: 10
                }

                StyledButton {
                    text: qsTr("Exit")
                    width: 60
                    onClicked: closeSettings()
                }

                Item {
                    width: 10
                }
            }

            Item {
                height: 5
            }
        }
    }

    Component.onCompleted: exifToolText.forceActiveFocus()
}

