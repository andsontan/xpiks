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
            text: qsTr("Add directory")
            onClicked: mainModel.addDirectoryButtonClicked
        }
    }

    SplitView {
        id: mainGrid
        orientation: Qt.Horizontal

        ListView {
            id: sourcesListView
            Layout.maximumWidth: 400

            header: addDirectoryComponent
        }

        ListView {
            id: imagesListView
            Layout.fillWidth: true
        }
    }
}
