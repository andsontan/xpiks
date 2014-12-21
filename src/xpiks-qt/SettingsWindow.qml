import QtQuick 2.4
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1

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

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 5

        GridLayout {
            width: parent.width
            rows: 2
            columns: 4

            Text {
                Layout.row: 0
                Layout.column: 0
                Layout.fillWidth: true
                Layout.maximumWidth: 150

                horizontalAlignment: Text.AlignRight
                text: "ExifTool path:"
            }

            Rectangle {
                Layout.row: 0
                Layout.column: 1
                height: childrenRect.height
                width: childrenRect.width + 5
                color: "#dddddd"

                TextInput {
                    id: exifToolText
                    width: 300
                    height: 20
                    clip: true
                    text: exifToolPath
                    anchors.left: parent.left
                    anchors.leftMargin: 5
                    verticalAlignment: Text.AlignVCenter
                    renderType: Text.NativeRendering
                }
            }

            Button {
                Layout.row: 0
                Layout.column: 2
                text: qsTr("Select...")
                onClicked: fileDialog.open()
            }

            Button {
                Layout.row: 0
                Layout.column: 3
                text: qsTr("Reset")
                onClicked: exifToolPath = defaultExifTool
            }

            Text {
                Layout.row: 1
                Layout.column: 0
                Layout.fillWidth: true
                Layout.maximumWidth: 150
                horizontalAlignment: Text.AlignRight
                text: "Curl path:"
            }

            Rectangle {
                Layout.row: 1
                Layout.column: 1
                height: childrenRect.height
                width: childrenRect.width + 5
                color: "#dddddd"

                TextInput {
                    id: curlText
                    width: 300
                    height: 20
                    clip: true
                    text: curlPath
                    anchors.left: parent.left
                    anchors.leftMargin: 5
                    verticalAlignment: Text.AlignVCenter
                    renderType: Text.NativeRendering
                }
            }

            Button {
                Layout.row: 1
                Layout.column: 2
                text: qsTr("Select...")
                onClicked: fileDialog.open()
            }

            Button {
                Layout.row: 1
                Layout.column: 3
                text: qsTr("Reset")
                onClicked: curlPath = defaultCurl
            }
        }

        RowLayout {
            height: 30

            Item {
                Layout.fillWidth: true
            }

            Button {
                text: qsTr("Save")
                onClicked: {
                    exifToolPath = exifToolText.text
                    appSettings.setValue(exiftoolpathkey, exifToolPath)

                    curlPath = curlText.text
                    appSettings.setValue(curlpathkey, curlPath)
                }
            }

            Button {
                text: qsTr("Exit")
                onClicked: settingsWindow.destroy()
            }
        }
    }
}

