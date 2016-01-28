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
import QtQuick.Layouts 1.1
import "../Constants"
import "../Constants/Colors.js" as Colors;
import "../Components"
import "../StyledControls"
import "../Common.js" as Common

ApplicationWindow {
    id: settingsWindow
    modality: "ApplicationModal"
    title: qsTr("Settings")
    width: 550
    height: 260
    minimumWidth: width
    maximumWidth: width
    minimumHeight: height
    maximumHeight: height
    flags: Qt.Dialog

    signal dialogDestruction();
    Component.onDestruction: dialogDestruction();

    function closeSettings() {
        settingsWindow.destroy();
    }

    function onCancelMP(firstTime) {
        settingsModel.mustUseMasterPassword = !firstTime
        settingsModel.raiseMasterPasswordSignal()
    }

    function onMasterPasswordSet() {
        console.log('Master password changed')
        appSettings.setValue(appSettings.masterPasswordHashKey, secretsManager.getMasterPasswordHash())
        appSettings.setValue(appSettings.mustUseMasterPasswordKey, true)
        settingsModel.mustUseMasterPassword = true
    }

    function openMasterPasswordDialog(firstTimeParam) {
        var callbackObject = {
            onCancel: onCancelMP,
            onSuccess: onMasterPasswordSet
        }

        Common.launchDialog("Dialogs/MasterPasswordSetupDialog.qml",
                            settingsWindow,
                            {
                                componentParent: settingsWindow,
                                firstTime: firstTimeParam,
                                callbackObject: callbackObject
                            });
    }

    FileDialog {
        id: exifToolFileDialog
        title: "Please choose ExifTool location"
        selectExisting: true
        selectMultiple: false
        nameFilters: [ "All files (*)" ]

        onAccepted: {
            console.log("You chose: " + exifToolFileDialog.fileUrl)
            var path = exifToolFileDialog.fileUrl.toString().replace(/^(file:\/{3})/,"");
            settingsModel.exifToolPath = decodeURIComponent(path);
        }

        onRejected: {
            console.log("File dialog canceled")
        }
    }

    FileDialog {
        id: dictPathDialog
        title: "Please choose dictionaries location"
        selectExisting: true
        selectMultiple: false
        selectFolder: true
        nameFilters: [ "All files (*)" ]

        onAccepted: {
            console.log("You chose: " + dictPathDialog.folder)
            var path = dictPathDialog.folder.toString().replace(/^(file:\/{2})/,"");
            settingsModel.dictionaryPath = decodeURIComponent(path);
        }

        onRejected: {
            console.log("File dialog canceled")
        }
    }

    function turnMasterPasswordOff () {
        secretsManager.resetMasterPassword()
        settingsModel.clearMasterPasswordSettings();
    }

    MessageDialog {
        id: masterPasswordOffWarningDialog
        title: "Warning"
        text: qsTr("Switching off master password will make your passwords storage less secure. Continue?")
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            if (secretsManager.isMasterPasswordSet()) {
                var callbackObject = {
                    onSuccess: turnMasterPasswordOff,
                    onFail: function() {
                        settingsModel.mustUseMasterPassword = true;
                        settingsModel.raiseMasterPasswordSignal()
                    }
                }

                Common.launchDialog("Dialogs/EnterMasterPasswordDialog.qml",
                                       settingsWindow,
                                       {componentParent: settingsWindow, callbackObject: callbackObject})
            } else {
                turnMasterPasswordOff()
            }
        }

        onNo: {
            settingsModel.mustUseMasterPassword = true
            settingsModel.raiseMasterPasswordSignal()
        }
    }

    MessageDialog {
        id: resetSettingsDialog
        title: "Warning"
        text: qsTr("Are you sure you want reset all settings? \nThis action cannot be undone.")
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            secretsManager.removeMasterPassword()
            settingsModel.resetAllValues()
        }
    }

    MessageDialog {
        id: resetMPDialog
        title: "Warning"
        text: qsTr("Are you sure you want reset Master password? \nAll upload hosts' passwords will be purged.")
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            secretsManager.removeMasterPassword()
            settingsModel.clearMasterPasswordSettings()
        }
    }

    Rectangle {
        color: Colors.selectedArtworkColor
        anchors.fill: parent

        Component.onCompleted: focus = true
        Keys.onEscapePressed: closeSettings()

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: {left:10; top:10; right:10}

            StyledTabView {
                Layout.fillHeight: true
                Layout.fillWidth: true

                Tab {
                    id: behaviorTab
                    title: qsTr("Behavior")
                    property bool useStatistics: settingsModel.userStatistic

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: {left: 20; top: 30; right: 20; bottom: 20}
                        spacing: 20

                        RowLayout {
                            width: parent.width
                            spacing: 10

                            StyledCheckbox {
                                id: useConfirmationDialogsCheckbox
                                text: qsTr("Use confirmation dialogs")
                                onCheckedChanged: {
                                    settingsModel.mustUseConfirmations = checked
                                }

                                Component.onCompleted: checked = settingsModel.mustUseConfirmations
                            }

                            StyledText {
                                text: qsTr("(with destructive actions)")
                                color: Colors.defaultInputBackground
                            }
                        }

                        RowLayout {
                            width: parent.width
                            spacing: 10

                            StyledCheckbox {
                                id: saveBackupsCheckbox
                                text: qsTr("Save backups for artworks")
                                onCheckedChanged: {
                                    settingsModel.saveBackups = checked
                                }

                                Component.onCompleted: checked = settingsModel.saveBackups
                            }

                            StyledText {
                                text: qsTr("(edited but not saved)")
                                color: Colors.defaultInputBackground
                            }
                        }

                        RowLayout {
                            width: parent.width
                            spacing: 10

                            StyledCheckbox {
                                id: searchUsingAndCheckbox
                                text: qsTr("Search match all terms")
                                onCheckedChanged: {
                                    settingsModel.searchUsingAnd = checked
                                }

                                Component.onCompleted: checked = settingsModel.searchUsingAnd
                            }

                            StyledText {
                                text: qsTr("(instead of any occurance)")
                                color: Colors.defaultInputBackground
                            }
                        }

                        RowLayout {
                            width: parent.width
                            spacing: 10

                            StyledCheckbox {
                                id: autoSpellCheckCheckbox
                                text: qsTr("Check spelling automatically")
                                onCheckedChanged: {
                                    settingsModel.useSpellCheck = checked
                                }

                                Component.onCompleted: checked = settingsModel.useSpellCheck
                            }

                            StyledText {
                                text: qsTr("(for typed keywords)")
                                color: Colors.defaultInputBackground
                            }
                        }

                        Item {
                            Layout.fillHeight: true
                        }
                    }
                }

                Tab {
                    id: uxTab
                    property double sizeSliderValue: settingsModel.keywordSizeScale
                    property double scrollSliderValue: settingsModel.scrollSpeedScale
                    title: qsTr("Interface")

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: {left: 20; top: 30; right: 20; bottom: 20}
                        spacing: 20

                        RowLayout {
                            width: parent.width
                            spacing: 10

                            StyledCheckbox {
                                id: fitArtworksCheckbox
                                text: qsTr("Fit artwork's preview")
                                onCheckedChanged: {
                                    settingsModel.fitSmallPreview = checked
                                }

                                Component.onCompleted: checked = settingsModel.fitSmallPreview
                            }

                            StyledText {
                                text: qsTr("(instead of filling the square)")
                                color: Colors.defaultInputBackground
                            }
                        }

                        RowLayout {
                            width: parent.width
                            spacing: 10

                            StyledText {
                                text: qsTr("Keywords size")
                            }

                            StyledSlider {
                                id: keywordSizeSlider
                                width: 150
                                minimumValue: 1.0
                                maximumValue: 1.2
                                stepSize: 0.0001
                                orientation: Qt.Horizontal
                                onValueChanged: uxTab.sizeSliderValue = value
                                Component.onCompleted: value = settingsModel.keywordSizeScale
                            }

                            Rectangle {
                                id: keywordPreview
                                color: Colors.defaultLightGrayColor

                                width: childrenRect.width
                                height: childrenRect.height

                                Row {
                                    spacing: 0

                                    Item {
                                        id: tagTextRect
                                        width: childrenRect.width + 5*keywordSizeSlider.value
                                        height: 20 * keywordSizeSlider.value + (keywordSizeSlider.value - 1)*10

                                        StyledText {
                                            anchors.top: parent.top
                                            anchors.bottom: parent.bottom
                                            anchors.left: parent.left
                                            anchors.leftMargin: 5 + (keywordSizeSlider.value - 1)*10
                                            verticalAlignment: Text.AlignVCenter
                                            text: "keyword"
                                            color: Colors.defaultControlColor
                                            font.pixelSize: 12 * keywordSizeSlider.value
                                        }
                                    }

                                    Item {
                                        height: 20 * keywordSizeSlider.value + (keywordSizeSlider.value - 1)*10
                                        width: height

                                        CloseIcon {
                                            width: 14 * keywordSizeSlider.value
                                            height: 14 * keywordSizeSlider.value
                                            isActive: true
                                            anchors.centerIn: parent
                                        }
                                    }
                                }
                            }
                        }

                        RowLayout {
                            width: parent.width
                            spacing: 20

                            StyledText {
                                text: qsTr("Scroll speed")
                            }

                            StyledSlider {
                                id: scrollSpeedSlider
                                width: 150
                                minimumValue: 1.0
                                maximumValue: 6
                                stepSize: 0.01
                                orientation: Qt.Horizontal
                                onValueChanged: uxTab.scrollSliderValue = value
                                Component.onCompleted: value = settingsModel.scrollSpeedScale
                            }
                        }

                        RowLayout {
                            width: parent.width
                            spacing: 10

                            StyledText {
                                Layout.preferredWidth: 130
                                horizontalAlignment: Text.AlignLeft
                                text: qsTr("Undo dismiss duration:")
                            }

                            StyledInputHost {
                                border.width: dismissDuration.activeFocus ? 1 : 0

                                StyledTextInput {
                                    id: dismissDuration
                                    width: 100
                                    height: 24
                                    clip: true
                                    text: settingsModel.dismissDuration
                                    anchors.left: parent.left
                                    anchors.leftMargin: 5
                                    onTextChanged: {
                                        if (text.length > 0) {
                                            settingsModel.dismissDuration = parseInt(text)
                                        }
                                    }

                                    validator: IntValidator {
                                        bottom: 5
                                        top: 20
                                    }
                                }
                            }

                            StyledText {
                                text: qsTr("(seconds)")
                                color: Colors.defaultInputBackground
                            }
                        }

                        Item {
                            Layout.fillHeight: true
                        }
                    }
                }

                Tab {
                    title: qsTr("External")

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: {left: 5; top: 30; right: 20; bottom: 20}

                        GridLayout {
                            width: parent.width
                            rows: 2
                            columns: 4
                            rowSpacing: 20
                            columnSpacing: 15

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
                                    text: settingsModel.exifToolPath
                                    anchors.left: parent.left
                                    anchors.leftMargin: 5
                                    onTextChanged: settingsModel.exifToolPath = text
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
                                onClicked: settingsModel.resetExifTool()
                            }


                            StyledText {
                                Layout.row: 2
                                Layout.column: 0
                                Layout.fillWidth: true
                                Layout.maximumWidth: 80
                                horizontalAlignment: Text.AlignRight
                                text: qsTr("Dictionary path:")
                                visible: Qt.platform.os === "linux"
                            }

                            StyledInputHost {
                                border.width: dictText.activeFocus ? 1 : 0
                                Layout.row: 2
                                Layout.column: 1
                                visible: Qt.platform.os === "linux"

                                StyledTextInput {
                                    id: dictText
                                    width: 150
                                    height: 24
                                    clip: true
                                    text: settingsModel.dictionaryPath
                                    anchors.left: parent.left
                                    anchors.leftMargin: 5
                                    KeyNavigation.backtab: curlText
                                    onTextChanged: settingsModel.dictionaryPath = text
                                }
                            }

                            StyledButton {
                                Layout.row: 2
                                Layout.column: 2
                                text: qsTr("Select...")
                                width: 70
                                onClicked: dictPathDialog.open()
                                visible: Qt.platform.os === "linux"
                            }

                            StyledButton {
                                Layout.row: 2
                                Layout.column: 3
                                text: qsTr("Reset")
                                width: 70
                                onClicked: settingsModel.resetDictPath()
                                visible: Qt.platform.os === "linux"
                            }
                        }

                        Item {
                            Layout.fillHeight: true
                        }
                    }
                }

                Tab {
                    title: qsTr("Warnings")

                    ColumnLayout {
                        spacing: 20
                        anchors.fill: parent
                        anchors.margins: {left: 20; top: 30; right: 20; bottom: 20}

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
                                    text: settingsModel.minMegapixelCount
                                    anchors.left: parent.left
                                    anchors.leftMargin: 5
                                    KeyNavigation.tab: keywordsCount
                                    onTextChanged: {
                                        if (text.length > 0) {
                                            settingsModel.minMegapixelCount = parseFloat(text)
                                        }
                                    }

                                    validator: DoubleValidator {
                                        bottom: 0
                                        top: 100
                                        decimals: 1
                                        notation: "StandardNotation"
                                    }
                                }
                            }

                            StyledText {
                                text: qsTr("(can be real)")
                                color: Colors.defaultInputBackground
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
                                    text: settingsModel.maxKeywordsCount
                                    anchors.left: parent.left
                                    anchors.leftMargin: 5
                                    KeyNavigation.backtab: megapixelsCount
                                    KeyNavigation.tab: descriptionLength
                                    onTextChanged: {
                                        if (text.length > 0) {
                                            settingsModel.maxKeywordsCount = parseInt(text)
                                        }
                                    }

                                    validator: IntValidator {
                                        bottom: 0
                                        top: 200
                                    }
                                }
                            }

                            StyledText {
                                text: qsTr("(keywords)")
                                color: Colors.defaultInputBackground
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
                                    text: settingsModel.maxDescriptionLength
                                    anchors.left: parent.left
                                    anchors.leftMargin: 5
                                    KeyNavigation.backtab: keywordsCount
                                    onTextChanged: {
                                        if (text.length > 0) {
                                            settingsModel.maxDescriptionLength = parseInt(text)
                                        }
                                    }

                                    validator: IntValidator {
                                        bottom: 0
                                        top: 1000
                                    }
                                }
                            }

                            StyledText {
                                text: qsTr("(characters)")
                                color: Colors.defaultInputBackground
                            }
                        }

                        Item {
                            Layout.fillHeight: true
                        }
                    }
                }

                Tab {
                    title: qsTr("Upload")

                    ColumnLayout {
                        spacing: 20
                        anchors.fill: parent
                        anchors.margins: {left: 20; top: 30; right: 20; bottom: 20}

                        RowLayout {
                            width: parent.width
                            spacing: 10

                            StyledText {
                                Layout.preferredWidth: 130
                                horizontalAlignment: Text.AlignRight
                                text: qsTr("File upload timeout:")
                            }

                            StyledInputHost {
                                border.width: timeoutMinutes.activeFocus ? 1 : 0

                                StyledTextInput {
                                    id: timeoutMinutes
                                    width: 100
                                    height: 24
                                    clip: true
                                    text: settingsModel.uploadTimeout
                                    anchors.left: parent.left
                                    anchors.leftMargin: 5
                                    onTextChanged: {
                                        if (text.length > 0) {
                                            settingsModel.uploadTimeout = parseInt(text)
                                        }
                                    }
                                    KeyNavigation.tab: maxParallelUploads
                                    validator: IntValidator {
                                        bottom: 1
                                        top: 30
                                    }
                                }
                            }

                            StyledText {
                                text: qsTr("(minutes)")
                                color: Colors.defaultInputBackground
                            }
                        }

                        RowLayout {
                            width: parent.width
                            spacing: 10

                            StyledText {
                                Layout.preferredWidth: 130
                                horizontalAlignment: Text.AlignRight
                                text: qsTr("Max parallel uploads:")
                            }

                            StyledInputHost {
                                border.width: maxParallelUploads.activeFocus ? 1 : 0

                                StyledTextInput {
                                    id: maxParallelUploads
                                    width: 100
                                    height: 24
                                    clip: true
                                    text: settingsModel.maxParallelUploads
                                    anchors.left: parent.left
                                    anchors.leftMargin: 5
                                    onTextChanged: {
                                        if (text.length > 0) {
                                            settingsModel.maxParallelUploads = parseInt(text)
                                        }
                                    }
                                    KeyNavigation.backtab: timeoutMinutes
                                    validator: IntValidator {
                                        bottom: 1
                                        top: 4
                                    }
                                }
                            }

                            StyledText {
                                text: qsTr("(takes effect after relaunch)")
                                color: Colors.defaultInputBackground
                            }
                        }

                        Item {
                            Layout.fillHeight: true
                        }
                    }
                }

                Tab {
                    title: qsTr("Security")

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: {left: 20; top: 30; right: 20; bottom: 20}

                        RowLayout {
                            StyledCheckbox {
                                id: masterPasswordCheckbox
                                text: qsTr("Use Master password")
                                onClicked: {
                                    if (checked) {
                                        if (!settingsModel.mustUseMasterPassword) {
                                            var firstTime = true;
                                            openMasterPasswordDialog(firstTime)
                                        }
                                    } else {
                                        masterPasswordOffWarningDialog.open()
                                    }
                                }

                                Component.onCompleted: {
                                    checked = settingsModel.mustUseMasterPassword
                                }

                                Connections {
                                    target: settingsModel
                                    onMustUseMasterPasswordChanged: {
                                        masterPasswordCheckbox.checked = settingsModel.mustUseMasterPassword
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

                        RowLayout {
                            Item {
                                Layout.fillWidth: true
                            }

                            StyledButton {
                                width: 190
                                text: qsTr("Reset Master password")
                                enabled: masterPasswordCheckbox.checked

                                onClicked: {
                                    resetMPDialog.open()
                                }
                            }
                        }

                        Item {
                            id: container
                            anchors.left: parent.left
                            anchors.right: parent.right
                            height: childrenRect.height
                            property bool expanded: false

                            StyledText {
                                id: link
                                text: qsTr("More...")
                                color: Colors.artworkActiveColor
                                anchors.top: parent.top
                                anchors.left: parent.left

                                MouseArea {
                                    anchors.fill: parent
                                    cursorShape: Qt.PointingHandCursor
                                    onClicked: {
                                        if (container.expanded) {
                                            container.expanded = false
                                            contentsRect.height = 0
                                            link.text = qsTr("More...")
                                        } else {
                                            contentsRect.height = 50
                                            container.expanded = true
                                            link.text = qsTr("Less...")
                                        }
                                    }
                                }
                            }

                            Rectangle {
                                id: contentsRect
                                color: "transparent"
                                border.color: Colors.defaultControlColor
                                border.width: 1
                                anchors.left: parent.left
                                anchors.top: link.bottom
                                anchors.topMargin: 5
                                height: 0
                                width: parent.width

                                Behavior on height {
                                    NumberAnimation {
                                        duration: 200
                                        easing.type: Easing.InQuad
                                    }
                                }

                                RowLayout {
                                    visible: container.expanded
                                    anchors.left: parent.left
                                    anchors.leftMargin: 10
                                    anchors.verticalCenter: parent.verticalCenter
                                    spacing: 10

                                    StyledCheckbox {
                                        id: userStatisticCheckBox
                                        text: qsTr("Collect usage statistic")
                                        onCheckedChanged: {
                                            behaviorTab.useStatistics = checked
                                        }

                                        Component.onCompleted: checked = settingsModel.userStatistic
                                    }

                                    StyledText {
                                        text: qsTr("(simple statistic of feature usage)")
                                        color: Colors.defaultInputBackground
                                    }
                                }
                            }
                        }

                        Item {
                            Layout.fillHeight: true
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
                    width: 10
                }

                StyledButton {
                    text: qsTr("Reset to defaults")
                    width: 120
                    onClicked: {
                        resetSettingsDialog.open()

                        if (typeof useConfirmationDialogsCheckbox !== "undefined") {
                            useConfirmationDialogsCheckbox.checked = settingsModel.mustUseConfirmations
                        }

                        if (typeof masterPasswordCheckbox !== "undefined") {
                            masterPasswordCheckbox.checked = settingsModel.mustUseMasterPassword;
                        }
                    }
                }

                Item {
                    Layout.fillWidth: true
                }

                StyledButton {
                    text: qsTr("Save and Close")
                    width: 120
                    onClicked: {
                        settingsModel.keywordSizeScale = uxTab.sizeSliderValue
                        settingsModel.scrollSpeedScale = uxTab.scrollSliderValue
                        settingsModel.userStatistic = behaviorTab.useStatistics
                        settingsModel.saveAllValues()
                        closeSettings()
                    }
                }

                Item {
                    width: 10
                }

                StyledButton {
                    text: qsTr("Close")
                    width: 60
                    onClicked: {
                        settingsModel.readAllValues()
                        closeSettings()
                    }
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

    Component.onCompleted: {
        //exifToolText.forceActiveFocus()
    }
}

