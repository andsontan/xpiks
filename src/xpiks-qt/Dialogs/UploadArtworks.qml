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
import "../Constants"
import "../Constants/Colors.js" as Colors;
import "../Components"
import "../StyledControls"

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

    MessageDialog {
        id: confirmRemoveItemDialog
        property int itemIndex
        title: "Confirmation"
        text: qsTr("Are you sure you want to remove this item?")
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            uploadInfos.removeItem(itemIndex)
        }
    }

    FocusScope {
        anchors.fill: parent

        // This rectangle is the actual popup
        Rectangle {
            id: dialogWindow
            width: 580
            height: 350
            color: Colors.selectedArtworkColor
            anchors.centerIn: parent

            ColumnLayout {
                spacing: 10
                anchors.fill: parent
                anchors.margins: 20

                SplitView {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    orientation: Qt.Horizontal
                    enabled: !artworkUploader.inProgress

                    handleDelegate: Rectangle {
                        height: parent.height - 29
                        width: 0
                        color: Colors.defaultDarkColor
                    }

                    ColumnLayout {
                        Layout.minimumWidth: 250
                        Layout.preferredWidth: 250
                        Layout.maximumWidth: 300

                        Rectangle {
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            Layout.minimumWidth: 250
                            Layout.maximumWidth: 300

                            color: Colors.defaultControlColor

                            ListView {
                                id: uploadHostsListView
                                model: uploadInfos

                                boundsBehavior: Flickable.StopAtBounds
                                anchors.fill: parent
                                anchors.margins: { left: 10; top: 5; right: 10 }

                                spacing: 10

                                delegate: Rectangle {
                                    id: sourceWrapper
                                    property variant myData: model
                                    property int indexOfThisDelegate: index
                                    color: ListView.isCurrentItem ? Colors.itemsSourceSelected : Colors.itemsSourceBackground
                                    width: parent.width
                                    height: 31
                                    Layout.minimumWidth: 250

                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: {
                                            uploadHostsListView.currentIndex = sourceWrapper.indexOfThisDelegate
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
                                            color: Colors.itemsSourceForeground
                                            text: title
                                            elide: Text.ElideMiddle
                                            font.bold: true
                                        }

                                        CloseIcon {
                                            width: 14
                                            height: 14
                                            anchors.verticalCenterOffset: 1
                                            isActive: false

                                            onItemClicked: {
                                                confirmRemoveItemDialog.itemIndex = sourceWrapper.indexOfThisDelegate
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

                    RowLayout {
                        Layout.fillWidth: true

                        Item {
                            width: 20
                        }

                        ColumnLayout {
                            spacing: 3

                            StyledText {
                                text: qsTr("Title:")
                            }

                            Rectangle {
                                color: Colors.defaultInputBackground
                                border.color: Colors.artworkActiveColor
                                border.width: titleText.activeFocus ? 1 : 0
                                width: 300
                                height: 30

                                StyledTextInput {
                                    id: titleText
                                    anchors.fill: parent
                                    enabled: uploadInfos.infosCount > 0
                                    text: uploadHostsListView.currentItem.myData.title
                                    anchors.leftMargin: 5
                                    onTextChanged: uploadHostsListView.currentItem.myData.edittitle = text
                                    KeyNavigation.tab: ftpHost
                                }
                            }

                            Item {
                                height: 5
                            }

                            StyledText {
                                text: qsTr("Host:")
                            }

                            Rectangle {
                                color: Colors.defaultInputBackground
                                border.color: Colors.artworkActiveColor
                                border.width: ftpHost.activeFocus ? 1 : 0
                                width: 300
                                height: 30

                                StyledTextInput {
                                    id: ftpHost
                                    anchors.fill: parent
                                    enabled: uploadInfos.infosCount > 0
                                    text: uploadHostsListView.currentItem.myData.host
                                    anchors.leftMargin: 5
                                    onTextChanged: uploadHostsListView.currentItem.myData.edithost = text
                                    KeyNavigation.tab: ftpUsername
                                    KeyNavigation.backtab: titleText
                                }
                            }

                            Item {
                                height: 5
                            }

                            StyledText {
                                text: qsTr("Username:")
                            }

                            Rectangle {
                                color: Colors.defaultInputBackground
                                border.color: Colors.artworkActiveColor
                                border.width: ftpUsername.activeFocus ? 1 : 0
                                width: 300
                                height: 30

                                StyledTextInput {
                                    id: ftpUsername
                                    anchors.fill: parent
                                    enabled: uploadInfos.infosCount > 0
                                    text: uploadHostsListView.currentItem.myData.username
                                    anchors.leftMargin: 5
                                    onTextChanged: uploadHostsListView.currentItem.myData.editusername = text
                                    KeyNavigation.tab: ftpPassword
                                    KeyNavigation.backtab: ftpHost
                                }
                            }

                            Item {
                                height: 5
                            }

                            StyledText {
                                text: qsTr("Password:")
                            }

                            Rectangle {
                                color: Colors.defaultInputBackground
                                border.color: Colors.artworkActiveColor
                                border.width: ftpPassword.activeFocus ? 1 : 0
                                width: 300
                                height: 30

                                StyledTextInput {
                                    id: ftpPassword
                                    anchors.fill: parent
                                    enabled: uploadInfos.infosCount > 0
                                    anchors.leftMargin: 5
                                    echoMode: TextInput.Password
                                    text: uploadHostsListView.currentItem.myData.password
                                    onTextChanged: uploadHostsListView.currentItem.myData.editpassword = text
                                    KeyNavigation.backtab: ftpUsername
                                }
                            }

                            Item {
                                height: 5
                            }

                            StyledButton {
                                text: qsTr("Add FTP host")
                                width: 90
                                anchors.left: parent.left
                                onClicked: uploadInfos.addItem()
                            }
                        }
                    }
                }

                ProgressBar {
                    id: progress
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: parent.width
                    height: 20
                    value: artworkUploader.percent

                    style: ProgressBarStyle {
                        background: Rectangle {
                            border.color: Colors.artworkActiveColor
                            color: artworkUploader.isError ? Colors.desctuctiveColor : Colors.artworkActiveColor

                            // Indeterminate animation by animating alternating stripes:
                            Item {
                                anchors.fill: parent
                                anchors.margins: 1
                                visible: control.indeterminate
                                clip: true
                                Row {
                                    Repeater {
                                        Rectangle {
                                            color: index % 2 ? Colors.artworkActiveColor : Colors.defaultLightColor
                                            width: 20 ; height: control.height
                                        }
                                        model: control.width / 20 + 2
                                    }
                                    XAnimator on x {
                                        from: 0 ; to: -40
                                        loops: Animation.Infinite
                                        running: control.indeterminate
                                    }
                                }
                            }
                        }
                    }
                }

                RowLayout {
                    height: 24
                    spacing: 15

                    StyledCheckbox {
                        text: qsTr("Include EPS (for illustrations)")
                        checked: artworkUploader.includeEPS
                        onCheckedChanged: artworkUploader.includeEPS = checked
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    StyledButton {
                        id: uploadButton
                        text: qsTr("Start Upload")
                        width: 130
                        enabled: !artworkUploader.inProgress
                        onClicked: {
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
                        width: 120
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
}
