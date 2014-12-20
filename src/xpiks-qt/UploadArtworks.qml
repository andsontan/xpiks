import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Styles 1.3

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
            radius: 10
            color: "#eeeeee"
            anchors.centerIn: parent

            ColumnLayout {
                spacing: 10
                anchors.fill: parent
                anchors.margins: 20

                Text {
                    text: qsTr("Upload artworks")
                }

                GridLayout {
                    rows: 3
                    columns: 2
                    rowSpacing: 5
                    columnSpacing: 10

                    Text {
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

                        width: 200
                        height: 20

                        TextEdit {
                            id: ftpHost
                            clip: true
                            anchors.fill: parent
                            text: artworkUploader.host
                            enabled: !artworkUploader.inProgress
                            anchors.leftMargin: 5
                            verticalAlignment: TextInput.AlignVCenter
                            onTextChanged: artworkUploader.host = text
                            KeyNavigation.tab: ftpUsername
                            KeyNavigation.priority: KeyNavigation.BeforeItem
                        }
                    }

                    Text {
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

                        width: 200
                        height: 20

                        TextInput {
                            id: ftpUsername
                            anchors.fill: parent
                            clip: true
                            text: artworkUploader.username
                            enabled: !artworkUploader.inProgress
                            anchors.leftMargin: 5
                            verticalAlignment: TextInput.AlignVCenter
                            onTextChanged: artworkUploader.username = text
                            KeyNavigation.tab: ftpPassword
                            KeyNavigation.backtab: ftpHost
                            KeyNavigation.priority: KeyNavigation.BeforeItem
                        }
                    }

                    Text {
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

                        width: 200
                        height: 20

                        TextInput {
                            id: ftpPassword
                            anchors.fill: parent
                            enabled: !artworkUploader.inProgress
                            anchors.leftMargin: 5
                            verticalAlignment: TextInput.AlignVCenter
                            echoMode: TextInput.Password
                            text: artworkUploader.password
                            onTextChanged: artworkUploader.password = text
                            KeyNavigation.backtab: ftpUsername
                            KeyNavigation.priority: KeyNavigation.BeforeItem
                        }
                    }
                }

                SimpleProgressBar {
                    id: progress
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: parent.width
                    height: 20
                    color: artworkUploader.isError ? "red" : "#77B753"
                    value: artworkUploader.percent

                    onValueChanged: {
                        if (value == 100) {
                            uploadButton.text = qsTr("Start Upload")
                        }
                    }
                }

                RowLayout {
                    Item {
                        Layout.fillWidth: true
                    }

                    Button {
                        id: uploadButton
                        text: qsTr("Start Upload")
                        enabled: !(artworkUploader.inProgress || ftpHost.length == 0 || ftpUsername.length == 0)
                        onClicked: {
                            text = qsTr("Uploading...")
                            artworkUploader.resetModel()
                            artworkUploader.uploadArtworks()
                        }
                    }

                    Button {
                        text: qsTr("Close")
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
