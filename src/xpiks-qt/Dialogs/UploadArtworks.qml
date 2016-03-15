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
import xpiks 1.0
import "../Constants"
import "../Constants/Colors.js" as Colors;
import "../Common.js" as Common;
import "../Components"
import "../StyledControls"

Item {
    id: uploadArtworksComponent
    anchors.fill: parent
    property string uploadhostskey: appSettings.uploadHostsKey
    // if MasterPassword wasn't entered do not show passwords
    property bool emptyPasswords: false
    property variant componentParent
    property bool uploadEnabled: (artworkRepository.artworksSourcesCount > 0) && (filteredArtItemsModel.selectedArtworksCount > 0)

    signal dialogDestruction();
    Component.onDestruction: dialogDestruction();

    Component.onCompleted: focus = true

    function closePopup() {
        secretsManager.purgeMasterPassword()
        warningsModel.resetShowSelected()
        uploadInfos.finalizeAccounts()
        saveSettings()
        uploadArtworksComponent.destroy()
    }

    function saveSettings() {
        appSettings.setValue(uploadhostskey, uploadInfos.getInfoString())
    }

    function mainAction() {
        artworkUploader.resetModel()
        artworkUploader.uploadArtworks()
    }

    function startUpload() {
        if (artworkUploader.needCreateArchives()) {
            var callbackObject = {
                afterZipped: function() {
                    mainAction();
                }
            }

            filteredArtItemsModel.setSelectedForZipping()
            Common.launchDialog("Dialogs/ZipArtworksDialog.qml",
                                uploadArtworksComponent.componentParent,
                                {
                                    immediateProcessing: true,
                                    callbackObject: callbackObject
                                });
        } else {
            mainAction();
        }
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

    MessageDialog {
        id: confirmRemoveItemDialog
        property int itemIndex
        title: "Confirmation"
        text: i18.n + qsTr("Are you sure you want to remove this item?")
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            uploadInfos.removeItem(itemIndex)
            if (uploadInfos.infosCount == 0) {
                if (typeof titleText !== "undefined") {
                    titleText.text = ""
                    ftpHost.text = ""
                    ftpUsername.text = ""
                    ftpPassword.text = ""
                }
            }
        }
    }

    MessageDialog {
        id: noPasswordDialog
        property string agenciesList
        title: "Warning"
        text: i18.n + qsTr("Some agencies (%1) miss FTP credentials. Start upload anyway?").arg(agenciesList)
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            startUpload()
        }
    }

    MessageDialog {
        id: selectHostsMessageBox
        title: "Warning"
        text: i18.n + qsTr("Please, select some hosts first")
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

            onPressed:{
                var tmp = mapToItem(uploadArtworksComponent, mouse.x, mouse.y);
                old_x = tmp.x;
                old_y = tmp.y;

                var dialogPoint = mapToItem(dialogWindow, mouse.x, mouse.y);
                if (!Common.isInComponent(dialogPoint, dialogWindow)) {
                    if (!artworkUploader.inProgress) {
                        closePopup()
                    }
                }
            }

            onPositionChanged: {
                var old_xy = Common.movePopupInsideComponent(uploadArtworksComponent, dialogWindow, mouse, old_x, old_y);
                old_x = old_xy[0]; old_y = old_xy[1];
            }
        }

        ClipboardHelper {
            id: clipboard
        }

        RectangularGlow {
            anchors.fill: dialogWindow
            glowRadius: 4
            spread: 0.1
            color: Colors.defaultControlColor
            cornerRadius: glowRadius
        }

        // This rectangle is the actual popup
        Rectangle {
            id: dialogWindow
            width: 610
            height: Qt.platform.os == "windows" ? 470 : (Qt.platform.os == "linux" ? 475 : 455)
            color: Colors.selectedArtworkColor
            anchors.centerIn: parent
            Component.onCompleted: anchors.centerIn = undefined

            ColumnLayout {
                spacing: 15
                anchors.fill: parent
                anchors.margins: 20

                RowLayout {
                    Layout.fillWidth: true

                    StyledText {
                        text: i18.n + qsTr("Upload artworks")
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    StyledText {
                        property string originalText: artworkUploader.itemsCount === 1 ? qsTr("1 artwork selected") : qsTr("%1 artworks selected").arg(artworkUploader.itemsCount)
                        text: i18.n + originalText
                        color: Colors.defaultInputBackground
                    }
                }

                Item {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    enabled: !artworkUploader.inProgress

                    ColumnLayout {
                        anchors.left: parent.left
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        id: uploadInfosStack
                        width: 260
                        height: parent.height
                        spacing: 0

                        Rectangle {
                            height: 5
                            Layout.fillWidth: true
                            color: Colors.defaultControlColor
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            Layout.minimumWidth: 250
                            Layout.preferredWidth: 250
                            Layout.maximumWidth: 300

                            color: Colors.defaultControlColor

                            StyledScrollView {
                                anchors.fill: parent
                                anchors.margins: { left: 5; top: 5; right: 5; bottom: 5 }

                                ListView {
                                    id: uploadHostsListView
                                    model: uploadInfos
                                    boundsBehavior: Flickable.StopAtBounds

                                    spacing: 10

                                    add: Transition {
                                        NumberAnimation { property: "opacity"; from: 0; to: 1; duration: 230 }
                                    }

                                    remove: Transition {
                                        NumberAnimation { property: "opacity"; to: 0; duration: 230 }
                                    }

                                    displaced: Transition {
                                        NumberAnimation { properties: "x,y"; duration: 230 }
                                    }

                                    addDisplaced: Transition {
                                        NumberAnimation { properties: "x,y"; duration: 230 }
                                    }

                                    removeDisplaced: Transition {
                                        NumberAnimation { properties: "x,y"; duration: 230 }
                                    }

                                    delegate: Rectangle {
                                        id: sourceWrapper
                                        property variant myData: model
                                        property int delegateIndex: index
                                        color: ListView.isCurrentItem ? Colors.itemsSourceSelected : Colors.defaultDarkColor
                                        width: parent.width - 10
                                        anchors.left: parent.left
                                        anchors.leftMargin: 5
                                        height: 31
                                        Layout.minimumWidth: 250

                                        MouseArea {
                                            anchors.fill: parent
                                            onClicked: {
                                                if (uploadHostsListView.currentIndex != sourceWrapper.delegateIndex) {
                                                    // "if" is needed for tabControl
                                                    if (typeof credentialsStatus !== "undefined") {
                                                        credentialsStatus.enabled = false
                                                        credentialsStatus.isGreen = false
                                                    }

                                                    uploadHostsListView.currentIndex = sourceWrapper.delegateIndex
                                                    uploadInfos.updateProperties(sourceWrapper.delegateIndex)
                                                }
                                            }

                                            onDoubleClicked: {
                                                myData.editisselected = !myData.isselected
                                                itemCheckedCheckbox.checked = isselected
                                            }
                                        }

                                        RowLayout {
                                            spacing: 10
                                            anchors.fill: parent

                                            Item {
                                                width: 1
                                            }

                                            StyledCheckbox {
                                                id: itemCheckedCheckbox
                                                onClicked: editisselected = checked
                                                Component.onCompleted: itemCheckedCheckbox.checked = isselected
                                            }

                                            StyledText {
                                                id: infoTitle
                                                Layout.fillWidth: true
                                                anchors.verticalCenter: parent.verticalCenter
                                                height: 31
                                                color: Colors.defaultLightGrayColor
                                                text: title
                                                elide: Text.ElideMiddle
                                                font.bold: true
                                            }

                                            StyledText {
                                                id: percentText
                                                text: percent + '%'
                                                visible: artworkUploader.inProgress && isselected
                                                color: Colors.artworkActiveColor
                                                font.bold: true
                                            }

                                            CloseIcon {
                                                width: 14
                                                height: 14
                                                anchors.verticalCenterOffset: 1
                                                isActive: false

                                                onItemClicked: {
                                                    confirmRemoveItemDialog.itemIndex = sourceWrapper.delegateIndex
                                                    confirmRemoveItemDialog.open()
                                                }
                                            }

                                            Item {
                                                id: placeholder2
                                                width: 1
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        Rectangle {
                            color: Colors.defaultControlColor
                            height: 40
                            Layout.fillWidth: true

                            RowLayout {
                                anchors.fill: parent
                                anchors.bottomMargin: 5
                                anchors.topMargin: 5
                                spacing: 0

                                Item {
                                    width: 10
                                }

                                StyledAddHostButton {
                                    Layout.fillWidth: true
                                    text: i18.n + qsTr("Add FTP host")
                                    onClicked: {
                                        uploadInfos.addItem()
                                        uploadHostsListView.currentIndex = uploadHostsListView.count - 1
                                    }
                                }

                                Item {
                                    width: 10
                                }
                            }
                        }
                    }

                    Item {
                        anchors.top: parent.top
                        anchors.leftMargin: 10
                        anchors.left: uploadInfosStack.right
                        anchors.right: parent.right
                        width: 290
                        height: Qt.platform.os == "windows" ? parent.height + 10 : parent.height

                        StyledTabView {
                            anchors.fill: parent
                            anchors.leftMargin: 10
                            anchors.topMargin: 1
                            enabled: uploadInfos.infosCount > 0

                            Tab {
                                title: i18.n + qsTr("General")

                                ColumnLayout {
                                    anchors.fill: parent
                                    anchors.topMargin: 15
                                    spacing: 4

                                    StyledText {
                                        text: i18.n + qsTr("Title:")
                                    }

                                    Rectangle {
                                        border.width: titleText.activeFocus ? 1 : 0
                                        border.color: Colors.artworkActiveColor
                                        Layout.fillWidth: true
                                        color: Colors.defaultInputBackground
                                        height: 30

                                        StyledTextInput {
                                            id: titleText
                                            height: 30
                                            anchors.left: parent.left
                                            anchors.right: parent.right
                                            anchors.rightMargin: 5
                                            text: uploadHostsListView.currentItem ? uploadHostsListView.currentItem.myData.title : ""
                                            anchors.leftMargin: 5
                                            onTextChanged: {
                                                if (uploadHostsListView.currentItem) {
                                                    uploadHostsListView.currentItem.myData.edittitle = text
                                                }
                                            }
                                            KeyNavigation.tab: ftpHost
                                            onEditingFinished: {
                                                if (text.length == 0) {
                                                    uploadHostsListView.currentItem.myData.edittitle = "Untitled"
                                                }
                                            }

                                            validator: RegExpValidator {
                                                // copy paste in keys.onpressed Paste
                                                regExp: /[a-zA-Z0-9 _-]*$/
                                            }

                                            Keys.onPressed: {
                                                if(event.matches(StandardKey.Paste)) {
                                                    var clipboardText = clipboard.getText();
                                                    // same regexp as in validator
                                                    var sanitizedText = clipboardText.replace(/[^a-zA-Z0-9 _-]/g, '');
                                                    titleText.paste(sanitizedText)
                                                    event.accepted = true
                                                }
                                            }
                                        }
                                    }

                                    Item {
                                        height: 5
                                    }

                                    StyledText {
                                        text: i18.n + qsTr("Host:")
                                    }

                                    Rectangle {
                                        border.width: ftpHost.activeFocus ? 1 : 0
                                        border.color: Colors.artworkActiveColor
                                        Layout.fillWidth: true
                                        color: Colors.defaultInputBackground
                                        height: 30

                                        StyledTextInput {
                                            id: ftpHost
                                            height: 30
                                            anchors.left: parent.left
                                            anchors.leftMargin: 5
                                            anchors.right: parent.right
                                            anchors.rightMargin: 5
                                            text: uploadHostsListView.currentItem ? uploadHostsListView.currentItem.myData.host : ""
                                            onTextChanged: {
                                                if (uploadHostsListView.currentItem) {
                                                    uploadHostsListView.currentItem.myData.edithost = text
                                                }
                                                credentialsStatus.enabled = false
                                            }
                                            KeyNavigation.tab: ftpUsername
                                            KeyNavigation.backtab: titleText
                                        }
                                    }

                                    Item {
                                        height: 6
                                    }

                                    StyledText {
                                        text: i18.n + qsTr("Username:")
                                    }

                                    Rectangle {
                                        border.width: ftpUsername.activeFocus ? 1 : 0
                                        border.color: Colors.artworkActiveColor
                                        Layout.fillWidth: true
                                        color: Colors.defaultInputBackground
                                        height: 30

                                        StyledTextInput {
                                            id: ftpUsername
                                            height: 30
                                            anchors.left: parent.left
                                            anchors.leftMargin: 5
                                            anchors.right: parent.right
                                            anchors.rightMargin: 5
                                            text: uploadHostsListView.currentItem ? uploadHostsListView.currentItem.myData.username : ""
                                            onTextChanged: {
                                                if (uploadHostsListView.currentItem) {
                                                    uploadHostsListView.currentItem.myData.editusername = text
                                                }
                                            }
                                            KeyNavigation.tab: ftpPassword
                                            KeyNavigation.backtab: ftpHost
                                        }
                                    }

                                    Item {
                                        height: 5
                                    }

                                    StyledText {
                                        text: i18.n + qsTr("Password:")
                                    }

                                    Rectangle {
                                        border.width: ftpPassword.activeFocus ? 1 : 0
                                        border.color: Colors.artworkActiveColor
                                        Layout.fillWidth: true
                                        height: 30
                                        color: Colors.defaultInputBackground

                                        StyledTextInput {
                                            id: ftpPassword
                                            height: 30
                                            anchors.left: parent.left
                                            anchors.leftMargin: 5
                                            anchors.right: parent.right
                                            anchors.rightMargin: 5
                                            echoMode: showPasswordCheckBox.checked ? TextInput.Normal : TextInput.Password
                                            text: uploadHostsListView.currentItem ? uploadHostsListView.currentItem.myData.password : ""
                                            onTextChanged: {
                                                if (uploadHostsListView.currentItem) {
                                                    uploadHostsListView.currentItem.myData.editpassword = text
                                                }
                                                credentialsStatus.enabled = false
                                            }
                                            KeyNavigation.backtab: ftpUsername
                                        }
                                    }

                                    Item {
                                        height: 4
                                    }

                                    RowLayout {
                                        height: 25
                                        Layout.fillWidth: true
                                        spacing: 5

                                        StyledCheckbox {
                                            id: showPasswordCheckBox
                                            text: i18.n + qsTr("Show password")
                                        }

                                        Item {
                                            Layout.fillWidth: true
                                        }

                                        CheckedComponent {
                                            width: 18
                                            height: 18
                                            id: credentialsStatus
                                            enabled: false
                                            isGreen: false
                                        }

                                        StyledButton {
                                            id: testButton
                                            text: i18.n + qsTr("Test connection")
                                            height: 24
                                            width: 130
                                            onClicked: {
                                                testButton.enabled = false
                                                credentialsStatus.enabled = false
                                                var disablePassiveMode = uploadHostsListView.currentItem.myData.disablepassivemode
                                                artworkUploader.checkCredentials(ftpHost.text, ftpUsername.text, ftpPassword.text, disablePassiveMode)
                                            }

                                            Connections {
                                                target: artworkUploader
                                                onCredentialsChecked: {
                                                    var currHost = ftpHost.text
                                                    if (url.indexOf(currHost) > -1) {
                                                        credentialsStatus.enabled = true
                                                        credentialsStatus.isGreen = result
                                                    }

                                                    testButton.enabled = true
                                                }
                                            }
                                        }
                                    }

                                    Item {
                                        Layout.fillHeight: true
                                    }
                                }
                            }

                            Tab {
                                title: i18.n + qsTr("Advanced")

                                ColumnLayout {
                                    anchors.fill: parent
                                    anchors.topMargin: 15
                                    spacing: 10

                                    StyledCheckbox {
                                        id: zipBeforeUploadCheckBox
                                        text: i18.n + qsTr("Zip vector with preview")
                                        Component.onCompleted: checked = uploadHostsListView.currentItem ? uploadHostsListView.currentItem.myData.zipbeforeupload : false

                                        onClicked: {
                                            if (uploadHostsListView.currentItem) {
                                                uploadHostsListView.currentItem.myData.editzipbeforeupload = checked
                                            }
                                        }

                                        Connections {
                                            target: uploadInfos
                                            onDataChanged: {
                                                zipBeforeUploadCheckBox.checked = uploadHostsListView.currentItem ? uploadHostsListView.currentItem.myData.zipbeforeupload : false
                                            }
                                        }
                                    }

                                    StyledCheckbox {
                                        id: ftpPassiveModeCheckBox
                                        text: i18.n + qsTr("Disable FTP passive mode")
                                        Component.onCompleted: checked = uploadHostsListView.currentItem ? uploadHostsListView.currentItem.myData.disablepassivemode : false

                                        onClicked: {
                                            if (uploadHostsListView.currentItem) {
                                                uploadHostsListView.currentItem.myData.editdisablepassivemode = checked
                                            }
                                        }

                                        Connections {
                                            target: uploadInfos
                                            onDataChanged: {
                                                ftpPassiveModeCheckBox.checked = uploadHostsListView.currentItem ? uploadHostsListView.currentItem.myData.disablepassivemode : false
                                            }
                                        }
                                    }

                                    Item {
                                        Layout.fillHeight: true
                                    }
                                }
                            }
                        }

                        Rectangle {
                            id: overlayRectangle
                            anchors.fill: parent
                            color: Colors.selectedArtworkColor
                            opacity: 0.6
                            visible: (uploadInfos.infosCount == 0) || artworkUploader.inProgress
                        }
                    }
                }

                SimpleProgressBar {
                    id: progress
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: parent.width
                    height: 20
                    color: artworkUploader.isError ? Colors.destructiveColor : Colors.artworkActiveColor
                    value: artworkUploader.percent
                }

                RowLayout {
                    height: 24
                    spacing: 20

                    StyledText {
                        enabled: uploadArtworksComponent.uploadEnabled
                        property string originalText: warningsModel.warningsCount == 1 ? qsTr("1 warning") : qsTr("%1 warnings").arg(warningsModel.warningsCount)
                        text: i18.n + originalText
                        color: uploadWarmingsMA.pressed ? Colors.defaultLightGrayColor : warningsModel.warningsCount > 0 ? Colors.artworkModifiedColor : Colors.defaultInputBackground

                        MouseArea {
                            id: uploadWarmingsMA
                            anchors.fill: parent
                            cursorShape: warningsModel.warningsCount > 0 ? Qt.PointingHandCursor : Qt.ArrowCursor
                            enabled: warningsModel.warningsCount > 0
                            onClicked: {
                                if (warningsModel.warningsCount > 0) {
                                    Common.launchDialog("Dialogs/WarningsDialog.qml",
                                                        uploadArtworksComponent.componentParent,
                                                        {
                                                            componentParent: uploadArtworksComponent.componentParent,
                                                            isRestricted: true
                                                        });
                                }
                            }
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    StyledButton {
                        id: uploadButton
                        enabled: uploadArtworksComponent.uploadEnabled
                        property string originalText: artworkUploader.inProgress ? qsTr("Stop") : qsTr("Start Upload")
                        text: i18.n + originalText
                        width: 130
                        onClicked: {
                            if (!artworkUploader.inProgress) {
                                if (uploadInfos.getSelectedInfosCount() === 0) {
                                    selectHostsMessageBox.open()
                                } else {
                                    var agencies = uploadInfos.getAgenciesWithMissingDetails();
                                    if (agencies.length !== 0) {
                                        noPasswordDialog.agenciesList = agencies
                                        noPasswordDialog.open()
                                    } else {
                                        startUpload()
                                    }
                                }
                            } else {
                                console.log("UI:UploadArtworks # About to cancel upload...")
                                enabled = false
                                artworkUploader.cancelOperation()
                            }
                        }

                        Connections {
                            target: artworkUploader
                            onStartedProcessing: {
                                helpersWrapper.turnTaskbarProgressOn()
                            }
                            onPercentChanged: {
                                helpersWrapper.setTaskbarProgress(artworkUploader.percent)
                            }
                            onFinishedProcessing: {
                                uploadButton.enabled = true
                                helpersWrapper.turnTaskbarProgressOff()
                            }
                        }
                    }

                    StyledButton {
                        text: i18.n + qsTr("Close")
                        width: 120
                        enabled: !artworkUploader.inProgress
                        onClicked: {
                            filteredArtItemsModel.updateSelectedArtworks()
                            uploadInfos.setAllUnselected()
                            closePopup()
                        }
                    }
                }
            }
        }
    }
}
