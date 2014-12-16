import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1

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
                        enabled: mainModel.canAddFiles
                        text: qsTr("Add directory")
                        onClicked: mainModel.addDirectoryButtonClicked()
                    }

                    Button {
                        text: qsTr("Add files")
                        enabled: mainModel.canAddFiles
                        onClicked: {
                            mainModel.addFilesButtonClicked()
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
                        width: 1
                    }

                    Button {
                        text: qsTr("Remove All")
                    }

                    Button {
                        text: qsTr("Edit Selected")
                        onClicked: {
                            Qt.createComponent("CombinedArtworksDialog.qml").createObject(applicationWindow, {});
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
                model: mainModel
                boundsBehavior: Flickable.StopAtBounds
                spacing: 2

                delegate: Rectangle {
                    z:1000
                    id: wrapperRectangle
                    color: "#dddddd"
                    property int indexOfThisDelegate: index

                    width: parent.width
                    height: 200

                    RowLayout {
                        anchors.fill: parent
                        spacing: 5

                        Rectangle {
                            id: isModifiedRectangle
                            color: ismodified ? "orange" : "green"
                            width: 3
                            Layout.fillHeight: true
                        }

                        CheckBox {
                            checked: isselected
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
                                    text: description
                                    onTextChanged: model.editdescription = text
                                    renderType: TextInput.NativeRendering
                                }
                            }

                            Text {
                                id: keywordsLabel
                                anchors.left: parent.left
                                text: qsTr("Keywords:")
                            }

                            Rectangle {
                                id: keywordsWrapper
                                color: "#adadad"
                                Layout.fillHeight: true
                                Layout.fillWidth: true

                                function removeKeyword(index) {
                                    mainModel.removeKeywordAt(wrapperRectangle.indexOfThisDelegate, index)
                                }

                                function removeLastKeyword() {
                                    mainModel.removeLastKeyword(wrapperRectangle.indexOfThisDelegate)
                                }

                                function appendKeyword(keyword) {
                                    mainModel.appendKeyword(wrapperRectangle.indexOfThisDelegate, keyword)
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
        }
    }
}
