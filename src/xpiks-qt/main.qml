import QtQuick 2.4
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import xpiks 1.0

ApplicationWindow {
    id: applicationWindow
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

    MessageDialog {
        id: confirmRemoveSelectedDialog
        property int itemsCount
        title: "Confirmation"
        text: qsTr("Are you sure you want to remove %1 item(s)?").arg(itemsCount)
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            artItemsModel.removeSelectedArtworks()
        }
    }

    MessageDialog {
        id: confirmRemoveDirectoryDialog
        property int directoryIndex
        title: "Confirmation"
        text: qsTr("Are you sure you want to remove this directory?")
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            artItemsModel.removeArtworksDirectory(directoryIndex)
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

            spacing: 5

            Rectangle {
                Layout.fillWidth: true
                height: 30
                color: "#dddddd"

                RowLayout {
                    spacing: 5
                    anchors.fill: parent

                    Item {
                        width: 1
                    }

                    Button {
                        enabled: artItemsModel.canAddFiles
                        text: qsTr("Add directory")
                        onClicked: artItemsModel.addDirectoryButtonClicked()
                    }

                    Button {
                        text: qsTr("Add files")
                        enabled: artItemsModel.canAddFiles
                        onClicked: {
                            artItemsModel.addFilesButtonClicked()
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                    }
                }

            }

            ListView {
                id: sourcesListView
                model: artworkRepository
                boundsBehavior: Flickable.StopAtBounds
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.minimumWidth: 250
                Layout.maximumWidth: 350

                spacing: 10

                delegate: Rectangle {
                    id: wrapperRect
                    property int indexOfThisDelegate: index
                    color: "white"
                    width: parent.width
                    height: 20
                    Layout.minimumWidth: 250

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
                                        confirmRemoveDirectoryDialog.directoryIndex = wrapperRect.indexOfThisDelegate
                                        confirmRemoveDirectoryDialog.open()

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

        ColumnLayout {
            spacing: 5

            Rectangle {
                color: "#dddddd"
                z: 10000
                Layout.fillWidth: true
                height: 30

                RowLayout {
                    spacing: 5
                    anchors.fill: parent

                    Item {
                        width: 3
                    }

                    CheckBox {
                        id: selectAllCheckbox

                        onCheckedChanged: {
                            if (checked) {
                                artItemsModel.selectAllArtworks();
                            } else {
                                artItemsModel.unselectAllArtworks();
                            }
                        }
                    }

                    Button {
                        text: qsTr("Remove Selected")
                        onClicked: {
                            var itemsCount = artItemsModel.getSelectedItemsCount()
                            if (itemsCount > 0) {
                                confirmRemoveSelectedDialog.itemsCount = itemsCount
                                confirmRemoveSelectedDialog.open()
                            }
                        }
                    }

                    Button {
                        text: qsTr("Edit Selected")
                        onClicked: {
                            if (artItemsModel.getSelectedItemsCount() > 0) {
                                combinedArtworks.resetModelData();
                                artItemsModel.combineSelectedArtworks();
                                Qt.createComponent("CombinedArtworksDialog.qml").createObject(applicationWindow, {});
                            }
                        }
                    }

                    // TODO: status line like reshaper (X items modified)
                    Item {
                        Layout.fillWidth: true
                    }

                    Button {
                        text: qsTr("Save All")
                    }

                    Button {
                        text: qsTr("Reset All")
                    }

                    Item {
                        width: 1
                    }
                }
            }

            ListView {
                id: imagesListView
                Layout.fillWidth: true
                Layout.fillHeight: true
                model: artItemsModel
                boundsBehavior: Flickable.StopAtBounds
                spacing: 2

                delegate: Rectangle {
                    id: wrapperRectangle
                    color: "#dddddd"
                    property int indexOfThisDelegate: index

                    width: parent.width
                    height: 200

                    RowLayout {
                        anchors.fill: parent
                        anchors.rightMargin: 5
                        spacing: 5

                        Rectangle {
                            id: isModifiedRectangle
                            color: ismodified ? "orange" : "green"
                            width: 3
                            Layout.fillHeight: true
                        }

                        CheckBox {
                            checked: isselected
                            onCheckedChanged: {
                                model.isselected = checked
                            }
                        }

                        ColumnLayout {
                            width: 150
                            spacing: 5

                            Item {
                                Layout.fillHeight: true
                            }

                            Rectangle {
                                width: 150
                                Layout.minimumHeight: 100
                                Layout.maximumHeight: 150
                                anchors.horizontalCenter: parent.horizontalCenter
                                color: "transparent"
                                Image {
                                    anchors.fill: parent
                                    source: "image://global/" + filename
                                    sourceSize.width: 150
                                    sourceSize.height: 150
                                    fillMode: Image.PreserveAspectFit
                                    asynchronous: true
                                }
                            }

                            Text {
                                Layout.fillWidth: true
                                elide: Text.ElideMiddle
                                horizontalAlignment: Text.AlignHCenter
                                text: filename.split(/[\\/]/).pop()
                                renderType: Text.NativeRendering
                            }

                            Item {
                                Layout.fillHeight: true
                            }
                        }

                        ColumnLayout {
                            id: columnLayout
                            spacing: 2

                            Layout.fillWidth: true
                            Layout.fillHeight: true

                            Text {
                                text: qsTr("Description:")
                                anchors.left: parent.left
                                renderType: Text.NativeRendering
                            }

                            Rectangle {
                                id: rect
                                width: 300
                                height: 25
                                color: "white"
                                anchors.left: parent.left

                                TextInput {
                                    anchors.left: parent.left
                                    anchors.right: parent.right
                                    anchors.leftMargin: 5
                                    anchors.rightMargin: 5
                                    anchors.verticalCenter: parent.verticalCenter
                                    maximumLength: 250
                                    clip: true
                                    text: description
                                    onTextChanged: model.editdescription = text
                                    renderType: TextInput.NativeRendering
                                }
                            }

                            RowLayout {
                                Layout.fillWidth: true

                                Text {
                                    id: keywordsLabel
                                    anchors.left: parent.left
                                    text: qsTr("Keywords:")
                                }

                                Item {
                                    Layout.fillWidth: true
                                }

                                Text {
                                    text: "<a href=\"#\">" + qsTr("Copy keywords") + "</a>"
                                    linkColor: "blue"

                                    MouseArea {
                                        anchors.fill: parent
                                        cursorShape: Qt.PointingHandCursor
                                        onClicked: clipboard.setText(keywordsstring)
                                    }
                                }
                            }

                            Rectangle {
                                id: keywordsWrapper
                                color: "#adadad"
                                Layout.fillHeight: true
                                Layout.fillWidth: true

                                function removeKeyword(index) {
                                    artItemsModel.removeKeywordAt(wrapperRectangle.indexOfThisDelegate, index)
                                }

                                function removeLastKeyword() {
                                    artItemsModel.removeLastKeyword(wrapperRectangle.indexOfThisDelegate)
                                }

                                function appendKeyword(keyword) {
                                    artItemsModel.appendKeyword(wrapperRectangle.indexOfThisDelegate, keyword)
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    propagateComposedEvents: true
                                    onClicked: {
                                        flv.activateEdit()
                                        mouse.accepted = false
                                    }
                                }

                                ScrollView {
                                    id: scroller
                                    anchors.fill: parent
                                    highlightOnFocus: true

                                    EditableTags {
                                        id: flv
                                        anchors.margins: 5
                                        model: keywords

                                        delegate: Rectangle {
                                            id: itemWrapper
                                            property int indexOfThisDelegate: index
                                            property string keyword: modelData
                                            border.width: 1
                                            border.color: "black"
                                            color: "#cccccc"

                                            width: childrenRect.width
                                            height: childrenRect.height

                                            RowLayout {
                                                Rectangle {
                                                    color: "transparent"
                                                    width: childrenRect.width + 15
                                                    height: 30

                                                    Text {
                                                        anchors.left: parent.left
                                                        anchors.leftMargin: 10
                                                        anchors.top: parent.top
                                                        anchors.bottom: parent.bottom
                                                        verticalAlignment: Text.AlignVCenter
                                                        text: modelData
                                                        renderType: Text.NativeRendering
                                                    }
                                                }

                                                Rectangle {
                                                    width: 15
                                                    height: 15
                                                    color: "transparent"
                                                    Image {
                                                        anchors.fill: parent
                                                        source: "qrc:/CloseIcon.svg"
                                                        sourceSize.width: 100
                                                        sourceSize.height: 100
                                                        fillMode: Image.PreserveAspectFit
                                                        opacity: mouseArea.containsMouse ? 1 : 0.5
                                                        scale: mouseArea.pressed ? 0.8 : 1

                                                        MouseArea {
                                                            id: mouseArea
                                                            anchors.fill: parent
                                                            hoverEnabled: true
                                                            preventStealing: true
                                                            onClicked: {
                                                                keywordsWrapper.removeKeyword(itemWrapper.indexOfThisDelegate)
                                                            }
                                                        }
                                                    }
                                                }

                                                Item {
                                                    width: 5
                                                }
                                            }
                                        }

                                        onTagAdded: {
                                            keywordsWrapper.appendKeyword(text)
                                        }

                                        onRemoveLast: {
                                            keywordsWrapper.removeLastKeyword()
                                        }
                                    }
                                }
                            }

                            Item {
                                height: 1
                            }
                        }
                    }
                }
            }

            ClipboardHelper {
                id: clipboard
            }
        }
    }

    statusBar: StatusBar {
        RowLayout {
            implicitHeight: 13
            width: parent.width
            spacing: 5

            Item {
                Layout.fillWidth: true
            }

            Text {
                text: qsTr("(%1) item(s) modified").arg(artItemsModel.modifiedArtworksCount)
            }
        }
    }
}
