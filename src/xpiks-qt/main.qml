import QtQuick 2.3
import QtQuick.Controls 1.2
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

    Component {
        id: addDirectoryComponent

        Button {
            width: 100
            text: qsTr("Add directory")
            anchors.right: parent.right
            onClicked: mainModel.addDirectoryButtonClicked()
        }
    }

    SplitView {
        id: mainGrid
        anchors.fill: parent
        orientation: Qt.Horizontal

        ListView {
            id: sourcesListView
            model: mainModel.directoryList
            Layout.minimumWidth: 250
            Layout.maximumWidth: 400

            spacing: 10
            header: addDirectoryComponent

            delegate: Rectangle {
                    id: containing_rect
                    property string text
                    color: "transparent"

                    text: modelData

                    Text {
                        id: text_field
                        anchors.top: parent.top
                        anchors.left: parent.left

                        height: parent.height
                        width: parent.width
                        text: parent.text
                        anchors.margins: 10
                        wrapMode: Text.WordWrap
                    }

                    Text {
                        id: dummy_text
                        text: parent.text
                        visible: false
                        anchors.margins: 10
                    }

                    states: [
                        State {
                            name: "wide text"
                                when: containing_rect.text.length > 20
                                PropertyChanges {
                                    target: containing_rect
                                    width: 200
                                    height: text_field.paintedHeight
                                }
                        },
                        State {
                            name: "not wide text"
                                when: containing_rect.text.length <= 20
                                PropertyChanges {
                                    target: containing_rect
                                    width: dummy_text.paintedWidth
                                    height: text_field.paintedHeight
                                }
                        }
                    ]
            }
        }

        ListView {
            id: imagesListView
            Layout.fillWidth: true
        }
    }
}
