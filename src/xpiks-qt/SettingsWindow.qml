import QtQuick 2.4
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1

ApplicationWindow {
    id: settingsWindow
    modality: "ApplicationModal"
    width: 640
    height: 180
    property string exiftoolpathkey: appSettings.exifToolPathKey
    property string exifToolPath: appSettings.value(exiftoolpathkey, "")

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

        RowLayout {
            width: parent.width
            height: 30
            spacing: 5

            Text {
                text: "ExifTool path: "
            }

            Rectangle {
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
                text: qsTr("Select...")
                onClicked: fileDialog.open()
            }

            Button {
                text: qsTr("Reset")
                onClicked: exifToolPath = "exiftool"
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
                }
            }

            Button {
                text: qsTr("Exit")
                onClicked: settingsWindow.destroy()
            }
        }
    }
}

