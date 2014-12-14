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
            Layout.minimumWidth: 250
            Layout.preferredWidth: 250
            Layout.maximumWidth: 350

            RowLayout {
                spacing: 10
                height: 30
                anchors.left: parent.left
                width: parent.width

                Item {
                    width: 1
                }

                Button {
                    width: 50
                    enabled: mainModel.canAddFiles
                    text: qsTr("Add directory")
                    onClicked: mainModel.addDirectoryButtonClicked()
                }

                Button {
                    width: 50
                    text: qsTr("Add files")
                    enabled: mainModel.canAddFiles
                    onClicked: {
                        mainModel.addFilesButtonClicked()
                    }
                }
            }

            ListView {
                id: sourcesListView
                model: artworkRepository
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.minimumWidth: 250
                Layout.maximumWidth: 350

                spacing: 10

                delegate: Rectangle {
                    property int indexOfThisDelxpiegate: index
                    color: "white"
                    width: parent.width
                    height: 20
                    Layout.minimumWidth: 250
                    id: wrapperRect

                    RowLayout {
                        spacing: 10
                        anchors.fill: parent

                        Item {
                            id: placeholder1
                            width: 1
                        }

                        Text {
                            id: directoryPath
                            Layout.fillWidth: true
                            anchors.verticalCenter: parent.verticalCenter
                            height: 20
                            text: path + "(" + usedimagescount + ")"
                            elide: Text.ElideMiddle
                        }

                        Rectangle {
                            width: 15
                            height: 15
                            color: "transparent"
                            Image {
                                anchors.fill: parent
                                source: "qrc:/RemoveDirectoryIcon.svg"
                                sourceSize.width: 100
                                sourceSize.height: 100
                                fillMode: Image.PreserveAspectFit
                                opacity: removeDirectoryMouseArea.containsMouse ? 1 : 0.5
                                scale: removeDirectoryMouseArea.pressed ? 0.8 : 1

                                MouseArea {
                                    id: removeDirectoryMouseArea
                                    anchors.fill: parent
                                    hoverEnabled: true

                                    onClicked: {
                                        mainModel.removeArtworksDirectory(wrapperRect.indexOfThisDelegate)
                                    }
                                }
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
