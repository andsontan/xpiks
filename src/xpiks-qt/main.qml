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

            delegate: RowLayout {
                property int indexOfThisDelegate: index
                width: parent.width
                spacing: 10
                height: 20

                Text {
                    id: directoryPath
                    height: 20
                    Layout.fillWidth: true
                    text: modelData
                    elide: Text.ElideMiddle
                }

                Button {
                    id: removeItemButton
                    width: 20
                    height: directoryPath.height
                    text: "X"
                    onClicked: mainModel.removeDirectory(indexOfThisDelegate)
                }
            }
        }

        ListView {
            id: imagesListView
            Layout.fillWidth: true
        }
    }
}
