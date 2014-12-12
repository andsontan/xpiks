import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1

ApplicationWindow {
    visible: true
    width: 800
    height: 640
    title: qsTr("Hello World")

    menuBar: MenuBar {
        Menu {
            title: qsTr("File")
            MenuItem {
                text: qsTr("&Open")
                onTriggered: console.log("Open action triggered");
            }
            MenuItem {
                text: qsTr("Exit")
                onTriggered: Qt.quit();
            }
        }
    }

    SplitView {
        id: mainGrid
        anchors.fill: parent
        orientation: Qt.Horizontal

        ColumnLayout {
            Layout.minimumWidth: 200
            Layout.maximumWidth: 400

            RowLayout {
                Layout.fillWidth: true
                spacing: 50
                height: 30

                Button {
                    width: 100
                    enabled: mainModel.canAddFiles
                    text: qsTr("Add directory")
                    onClicked: mainModel.addDirectoryButtonClicked()
                }

                Button {
                    width: 100
                    text: qsTr("Add files")
                    enabled: mainModel.canAddFiles
                    onClicked: mainModel.addFilesButtonClicked()
                }
            }

            ListView {
                id: sourcesListView
                model: artworkRepository
                Layout.fillHeight: true
                Layout.fillWidth: true
                anchors.leftMargin: 10
                anchors.rightMargin: 10

                spacing: 10

                delegate: RowLayout {
                    property int indexOfThixpisDelxpiegate: index
                    height: 20
                    spacing: 10
                    width: parent.width

                    Text {
                        id: directoryPath
                        Layout.fillWidth: true
                        Layout.minimumWidth: 150
                        height: 20
                        text: path + "(" + usedimagescount + ")"
                        elide: Text.ElideMiddle
                    }

                    Button {
                        id: removeItemButton
                        Layout.maximumWidth: 20
                        Layout.minimumWidth: 20
                        height: 20
                        text: "X"
                        onClicked: mainModel.removeArtworksDirectory(indexOfThisDelegate)
                    }
                }
            }
        }

        ListView {
            id: imagesListView
            Layout.fillWidth: true
            model: mainModel
            spacing: 10

            delegate: RowLayout {
                property int indexOfThisDelegate: index
                height: 200
                width: parent.widht

                ColumnLayout {
                    width: 150
                    height: parent.height
                    Layout.maximumWidth: 250

                    Image {
                        source: "image://global/" + filename
                        width: 150
                        height: 200
                        sourceSize.width: 200
                        sourceSize.height: 200
                        fillMode: Image.PreserveAspectFit
                        asynchronous: true
                    }

                    Text {
                        text: filename.split(/[\\/]/).pop()
                    }
                }

                GridLayout {
                    Layout.fillWidth: true
                    columns: 2
                    rows: 2
                    rowSpacing: 5
                    columnSpacing: 5

                    Text { text: qsTr("Description:") }

                    TextInput { text: description }

                    Text { text: qsTr("Keywords:") }

                    TextEdit {
                        //text: keywords
                        Layout.fillHeight: true
                        wrapMode: TextEdit.Wrap
                    }
                }
            }
        }
    }
}
