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
import "../Components"
import "../StyledControls"
import "../Common.js" as Common
import "../Constants/UIConfig.js" as UIConfig
import "../Constants/Themes.js" as Themes;

ApplicationWindow {
    id: settingsWindow
    modality: "ApplicationModal"
    title: i18.n + qsTr("Settings")
    width: 550
    height: 280
    minimumWidth: width
    maximumWidth: width
    minimumHeight: height
    maximumHeight: height
    flags: Qt.Dialog

    signal dialogDestruction();

    onClosing: dialogDestruction();

    function closeSettings() {
        settingsWindow.close()
    }

    function onCancelMP(firstTime) {
        settingsModel.mustUseMasterPassword = !firstTime
        settingsModel.raiseMasterPasswordSignal()
    }

    function onMasterPasswordSet() {
        console.log('UI:SettingsWindow # Master password changed')
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
            console.log("UI:SettingsWindow # You chose: " + exifToolFileDialog.fileUrl)
            var path = exifToolFileDialog.fileUrl.toString().replace(/^(file:\/{3})/,"");
            settingsModel.exifToolPath = decodeURIComponent(path);
        }

        onRejected: {
            console.log("UI:SettingsWindow # File dialog canceled")
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
            console.log("UI:SettingsWindow # You chose: " + dictPathDialog.folder)
            var path = dictPathDialog.folder.toString().replace(/^(file:\/{2})/,"");
            settingsModel.dictionaryPath = decodeURIComponent(path);
        }

        onRejected: {
            console.log("UI:SettingsWindow # File dialog canceled")
        }
    }

    function turnMasterPasswordOff () {
        secretsManager.resetMasterPassword()
        settingsModel.clearMasterPasswordSettings();
    }

    MessageDialog {
        id: masterPasswordOffWarningDialog
        title: "Warning"
        text: i18.n + qsTr("Switching off master password will make your passwords storage less secure. Continue?")
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
        text: i18.n + qsTr("Are you sure you want reset all settings? \nThis action cannot be undone.")
        standardButtons: StandardButton.Yes | StandardButton.No

        onYes: {
            secretsManager.removeMasterPassword()
            settingsModel.resetAllValues()
        }
    }

    MessageDialog {
        id: resetMPDialog
        title: "Warning"
        text: i18.n + qsTr("Are you sure you want reset Master password? \nAll upload hosts' passwords will be purged.")
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            secretsManager.removeMasterPassword()
            settingsModel.clearMasterPasswordSettings()
        }
    }

    Rectangle {
        color: Colors.selectedImageBackground
        anchors.fill: parent

        Component.onCompleted: focus = true
        Keys.onEscapePressed: closeSettings()

        StyledTabView {
            id: tabView
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: bottomRow.top

            anchors.margins: 10

            Connections {
                target: settingsModel
                onSettingsReset: {
                    tabView.getTab(tabView.currentIndex).resetRequested();
                }
            }

            Tab {
                id: behaviorTab
                title: i18.n + qsTr("Behavior")
                signal resetRequested()

                Item {
                    anchors.fill: parent

                    ColumnLayout {
                        id: leftColumn
                        anchors.left: parent.left
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        width: parent.width / 2
                        anchors.leftMargin: 20
                        anchors.rightMargin: 20
                        anchors.topMargin: 20
                        anchors.bottomMargin: 10
                        spacing: 20

                        StyledCheckbox {
                            id: checkForUpdatesCheckbox
                            text: i18.n + qsTr("Check for updates")
                            onCheckedChanged: {
                                settingsModel.updateService = checked
                            }
                            function onResetRequested()  {
                                checked = settingsModel.updateService
                            }

                            Component.onCompleted: {
                                checked = settingsModel.updateService
                                behaviorTab.resetRequested.connect(checkForUpdatesCheckbox.onResetRequested)
                            }
                        }

                        StyledCheckbox {
                            id: useConfirmationDialogsCheckbox
                            text: i18.n + qsTr("Use confirmation dialogs")
                            onCheckedChanged: {
                                settingsModel.mustUseConfirmations = checked
                            }
                            function onResetRequested()  {
                                checked = settingsModel.mustUseConfirmations
                            }

                            Component.onCompleted: {
                                checked = settingsModel.mustUseConfirmations
                                behaviorTab.resetRequested.connect(useConfirmationDialogsCheckbox.onResetRequested)
                            }

                        }

                        StyledCheckbox {
                            id: searchUsingAndCheckbox
                            text: i18.n + qsTr("Search match all terms")
                            onCheckedChanged: {
                                settingsModel.searchUsingAnd = checked
                            }
                            function onResetRequested()  {
                                checked = settingsModel.searchUsingAnd
                            }
                            Component.onCompleted: {
                                checked = settingsModel.searchUsingAnd
                                behaviorTab.resetRequested.connect(searchUsingAndCheckbox.onResetRequested)
                            }
                        }

                        Item {
                            Layout.fillHeight: true
                        }
                    }

                    ColumnLayout {
                        anchors.left: leftColumn.right
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        anchors.right: parent.right
                        anchors.leftMargin: 20
                        anchors.rightMargin: 20
                        anchors.topMargin: 20
                        anchors.bottomMargin: 10
                        spacing: 20

                        StyledCheckbox {
                            id: searchForVectorCheckbox
                            text: i18.n + qsTr("Attach vector automatically")
                            onCheckedChanged: {
                                settingsModel.autoFindVectors = checked
                            }
                            function onResetRequested()  {
                                checked = settingsModel.autoFindVectors
                            }

                            Component.onCompleted: {
                                checked = settingsModel.autoFindVectors
                                behaviorTab.resetRequested.connect(searchForVectorCheckbox.onResetRequested)
                            }
                        }

                        StyledCheckbox {
                            id: saveBackupsCheckbox
                            text: i18.n + qsTr("Save backups for artworks")
                            onCheckedChanged: {
                                settingsModel.saveBackups = checked
                            }
                            function onResetRequested()  {
                                checked = settingsModel.saveBackups
                            }

                            Component.onCompleted: {
                                checked = settingsModel.saveBackups
                                behaviorTab.resetRequested.connect(saveBackupsCheckbox.onResetRequested)
                            }
                        }

                        StyledCheckbox {
                            id: autoSpellCheckCheckbox
                            text: i18.n + qsTr("Check spelling automatically")
                            onCheckedChanged: {
                                settingsModel.useSpellCheck = checked
                            }
                            function onResetRequested()  {
                                checked = settingsModel.useSpellCheck
                            }

                            Component.onCompleted: {
                                checked = settingsModel.useSpellCheck
                                behaviorTab.resetRequested.connect(autoSpellCheckCheckbox.onResetRequested)
                            }
                        }

                        Item {
                            Layout.fillHeight: true
                        }
                    }
                }
            }

            Tab {
                id: uxTab
                property double sizeSliderValue: settingsModel.keywordSizeScale
                property double scrollSliderValue: settingsModel.scrollSpeedScale
                property int themeIndex: settingsModel.selectedThemeIndex
                title: i18.n + qsTr("Interface")
                signal resetRequested()

                ColumnLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 20
                    anchors.rightMargin: 20
                    anchors.topMargin: 20
                    anchors.bottomMargin: 10
                    spacing: 20

                    RowLayout {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        spacing: 20
                        z: 10000

                        StyledCheckbox {
                            id: fitArtworksCheckbox
                            text: i18.n + qsTr("Fit artwork's preview")
                            onCheckedChanged: {
                                settingsModel.fitSmallPreview = checked
                            }
                            function onResetRequested()  {
                                checked =  settingsModel.fitSmallPreview
                            }

                            Component.onCompleted: {
                                checked = settingsModel.fitSmallPreview
                                uxTab.resetRequested.connect(fitArtworksCheckbox.onResetRequested)
                            }
                        }

                        Item {
                            width: 20
                        }

                        StyledText {
                            text: i18.n + qsTr("Theme:")
                        }

                        CustomComboBox {
                            id: themeComboBox
                            model: Themes.names
                            width: 100
                            height: 24
                            itemHeight: 28
                            onComboIndexChanged: {
                                uxTab.themeIndex = themeComboBox.selectedIndex
                            }

                            function onResetRequested()  {
                                selectedIndex  = settingsModel.selectedThemeIndex
                            }

                            Component.onCompleted: {
                                themeComboBox.selectedIndex = settingsModel.selectedThemeIndex
                                uxTab.resetRequested.connect(themeComboBox.onResetRequested)
                            }
                        }
                    }

                    RowLayout {
                        width: parent.width
                        spacing: 10

                        StyledText {
                            text: i18.n + qsTr("Keywords size")
                        }

                        StyledSlider {
                            id: keywordSizeSlider
                            width: 150
                            minimumValue: 1.0
                            maximumValue: 1.2
                            stepSize: 0.0001
                            orientation: Qt.Horizontal
                            onValueChanged: uxTab.sizeSliderValue = value
                            Component.onCompleted:{
                                value = settingsModel.keywordSizeScale
                                uxTab.resetRequested.connect(keywordSizeSlider.onResetRequested)
                            }

                            function onResetRequested()  {
                                value = settingsModel.keywordSizeScale
                                uxTab.sizeSliderValue = value
                            }
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
                                        text: qsTr("keyword", "standalone")
                                        color: Colors.defaultControlColor
                                        font.pixelSize: UIConfig.fontPixelSize * keywordSizeSlider.value
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
                            text: i18.n + qsTr("Scroll speed")
                        }

                        StyledSlider {
                            id: scrollSpeedSlider
                            width: 150
                            minimumValue: 1.0
                            maximumValue: 6
                            stepSize: 0.01
                            orientation: Qt.Horizontal
                            onValueChanged: uxTab.scrollSliderValue = value
                            Component.onCompleted: {
                                value = settingsModel.scrollSpeedScale
                                uxTab.resetRequested.connect(scrollSpeedSlider.onResetRequested)
                            }

                            function onResetRequested() {
                                value = settingsModel.scrollSpeedScale
                                uxTab.sizeSliderValue = value
                            }
                        }
                    }

                    RowLayout {
                        width: parent.width
                        spacing: 10

                        StyledText {
                            Layout.preferredWidth: 130
                            horizontalAlignment: Text.AlignLeft
                            text: i18.n + qsTr("Undo dismiss duration:")
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

                                function onResetRequested()  {
                                    text = settingsModel.dismissDuration
                                }

                                validator: IntValidator {
                                    bottom: 5
                                    top: 20
                                }
                            }
                        }

                        StyledText {
                            text: i18.n + qsTr("(seconds)")
                        }
                    }

                    Item {
                        Layout.fillHeight: true
                    }
                }
            }

            Tab {
                id: extTab
                title: i18.n + qsTr("External")
                signal resetRequested()

                ColumnLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 5
                    anchors.rightMargin: 20
                    anchors.topMargin: 20
                    anchors.bottomMargin: 10

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
                            text: i18.n + qsTr("ExifTool path:")
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

                                function onResetRequested()  {
                                    value =  settingsModel.exifToolPath
                                    text = value
                                }
                                Component.onCompleted: {
                                    extTab.resetRequested.connect(exifToolText.onResetRequested)
                                }
                            }
                        }

                        StyledButton {
                            Layout.row: 0
                            Layout.column: 2
                            text: i18.n + qsTr("Select...")
                            width: 100
                            onClicked: exifToolFileDialog.open()
                        }

                        StyledButton {
                            Layout.row: 0
                            Layout.column: 3
                            text: i18.n + qsTr("Reset")
                            width: 100
                            onClicked: settingsModel.resetExifTool()
                        }


                        StyledText {
                            Layout.row: 2
                            Layout.column: 0
                            Layout.fillWidth: true
                            Layout.maximumWidth: 80
                            horizontalAlignment: Text.AlignRight
                            text: i18.n + qsTr("Dictionary path:")
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
                                onTextChanged: settingsModel.dictionaryPath = text
                            }
                        }

                        StyledButton {
                            Layout.row: 2
                            Layout.column: 2
                            text: i18.n + qsTr("Select...")
                            width: 100
                            onClicked: dictPathDialog.open()
                            visible: Qt.platform.os === "linux"
                        }

                        StyledButton {
                            Layout.row: 2
                            Layout.column: 3
                            text: i18.n + qsTr("Reset")
                            width: 100
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
                id: warnTab
                title: i18.n + qsTr("Warnings")
                signal resetRequested()

                ColumnLayout {
                    spacing: 20
                    anchors.fill: parent
                    anchors.leftMargin: 20
                    anchors.rightMargin: 20
                    anchors.topMargin: 20
                    anchors.bottomMargin: 10

                    RowLayout {
                        width: parent.width
                        spacing: 10

                        StyledText {
                            Layout.preferredWidth: 130
                            horizontalAlignment: Text.AlignRight
                            text: i18.n + qsTr("Minimum megapixels:")
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

                                function onResetRequested()  {
                                    value =  settingsModel.minMegapixelCount
                                    text = value
                                }
                                Component.onCompleted: {
                                    warnTab.resetRequested.connect(megapixelsCount.onResetRequested)
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
                            text: i18.n + qsTr("(can be real)")
                        }
                    }

                    RowLayout {
                        width: parent.width
                        spacing: 10

                        StyledText {
                            Layout.preferredWidth: 130
                            horizontalAlignment: Text.AlignRight
                            text: i18.n + qsTr("Max keywords count:")
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

                                function onResetRequested()  {
                                    value =  settingsModel.maxKeywordsCount
                                    text = value
                                }

                                Component.onCompleted: {
                                    warnTab.resetRequested.connect(keywordsCount.onResetRequested)
                                }

                                validator: IntValidator {
                                    bottom: 0
                                    top: 200
                                }
                            }
                        }

                        StyledText {
                            text: i18.n + qsTr("(keywords)")
                        }
                    }

                    RowLayout {
                        width: parent.width
                        spacing: 10

                        StyledText {
                            Layout.preferredWidth: 130
                            horizontalAlignment: Text.AlignRight
                            text: i18.n + qsTr("Max description length:")
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

                                function onResetRequested()  {
                                    value =  settingsModel.maxDescriptionLength
                                    text = value
                                }

                                Component.onCompleted: {
                                    warnTab.resetRequested.connect(descriptionLength.onResetRequested)
                                }

                                validator: IntValidator {
                                    bottom: 0
                                    top: 1000
                                }
                            }
                        }

                        StyledText {
                            text: i18.n + qsTr("(characters)")
                        }
                    }

                    Item {
                        Layout.fillHeight: true
                    }
                }
            }

            Tab {
                id:uploadTab
                title: i18.n + qsTr("Upload")
                signal resetRequested()

                ColumnLayout {
                    spacing: 20
                    anchors.fill: parent
                    anchors.leftMargin: 20
                    anchors.rightMargin: 20
                    anchors.topMargin: 20
                    anchors.bottomMargin: 10

                    RowLayout {
                        width: parent.width
                        spacing: 10

                        StyledText {
                            Layout.preferredWidth: 130
                            horizontalAlignment: Text.AlignRight
                            text: i18.n + qsTr("File upload timeout:")
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

                                function onResetRequested()  {
                                    value =  settingsModel.uploadTimeout
                                    text = value
                                }

                                Component.onCompleted: {
                                    uploadTab.resetRequested.connect(timeoutMinutes.onResetRequested)
                                }
                                KeyNavigation.tab: maxParallelUploads
                                validator: IntValidator {
                                    bottom: 1
                                    top: 30
                                }
                            }
                        }

                        StyledText {
                            text: i18.n + qsTr("(seconds)")
                            color: Colors.inputBackgroundColor
                        }
                    }

                    RowLayout {
                        width: parent.width
                        spacing: 10

                        StyledText {
                            Layout.preferredWidth: 130
                            horizontalAlignment: Text.AlignRight
                            text: i18.n + qsTr("Max parallel uploads:")
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

                                function onResetRequested()  {
                                    value =  settingsModel.maxParallelUploads
                                    text = value
                                }

                                Component.onCompleted: {
                                    uploadTab.resetRequested.connect(maxParallelUploads.onResetRequested)
                                }
                                KeyNavigation.backtab: timeoutMinutes
                                validator: IntValidator {
                                    bottom: 1
                                    top: 4
                                }
                            }
                        }

                        StyledText {
                            text: i18.n + qsTr("(takes effect after relaunch)")
                        }
                    }

                    Item {
                        Layout.fillHeight: true
                    }
                }
            }

            Tab {
                id : secTab
                title: i18.n + qsTr("Security")
                signal resetRequested()

                property bool useStatistics: settingsModel.userStatistics

                ColumnLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 20
                    anchors.rightMargin: 20
                    anchors.topMargin: 20
                    anchors.bottomMargin: 10

                    RowLayout {
                        StyledCheckbox {
                            id: masterPasswordCheckbox
                            text: i18.n + qsTr("Use Master password")
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
                                secTab.resetRequested.connect(masterPasswordCheckbox.onResetRequested)
                            }

                            Connections {
                                target: settingsModel
                                onMustUseMasterPasswordChanged: {
                                    masterPasswordCheckbox.checked = settingsModel.mustUseMasterPassword
                                }

                            }

                            function onResetRequested()  {
                                checked =  settingsModel.mustUseMasterPassword
                            }
                        }

                        Item {
                            Layout.fillWidth: true
                        }

                        StyledButton {
                            width: 190
                            text: i18.n + qsTr("Change Master password")
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
                            text: i18.n + qsTr("Reset Master password")
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
                            text: i18.n + qsTr("More...")
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
                                        link.text = i18.n + qsTr("More...")
                                    } else {
                                        contentsRect.height = 50
                                        container.expanded = true
                                        link.text = i18.n + qsTr("Less...")
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
                                    text: i18.n + qsTr("Collect usage statistics")

                                    function onResetRequested()  {
                                        checked = settingsModel.userStatistics
                                    }

                                    Component.onCompleted: {
                                        checked = settingsModel.userStatistics
                                        secTab.resetRequested.connect(userStatisticCheckBox.onResetRequested)
                                    }

                                    onCheckedChanged: {
                                        secTab.useStatistics = checked
                                    }

                                }

                                StyledText {
                                    text: i18.n + qsTr("(simple statistics of feature usage)")
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

        RowLayout {
            id: bottomRow
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.leftMargin: 20
            anchors.rightMargin: 20
            anchors.bottomMargin: 20
            height: 24
            spacing: 0
            width: parent.width

            Item {
                width: 10
            }

            StyledButton {
                text: i18.n + qsTr("Reset to defaults")
                width: 140
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
                text: i18.n + qsTr("Save")
                width: 100
                onClicked: {
                    settingsModel.keywordSizeScale = uxTab.sizeSliderValue
                    settingsModel.scrollSpeedScale = uxTab.scrollSliderValue
                    settingsModel.selectedThemeIndex = uxTab.themeIndex
                    settingsModel.userStatistic = secTab.useStatistics
                    settingsModel.saveAllValues()
                    closeSettings()
                    Colors.initTheme(Themes.availableThemes[settingsModel.selectedThemeIndex])
                }
            }

            Item {
                width: 20
            }

            StyledButton {
                text: i18.n + qsTr("Close")
                width: 100
                onClicked: {
                    settingsModel.readAllValues()
                    closeSettings()
                }
            }

            Item {
                width: 10
            }
        }

    }

    Component.onCompleted: {
        //exifToolText.forceActiveFocus()
    }
}

