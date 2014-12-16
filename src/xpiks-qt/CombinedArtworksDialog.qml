import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1

Item {
    id: dialogComponent
    anchors.fill: parent

    PropertyAnimation { target: dialogComponent; property: "opacity";
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

    // This rectangle is the actual popup
    Rectangle {
        id: dialogWindow
        width: 640
        height: 480
        radius: 10
        color: "#eeeeee"
        anchors.centerIn: parent

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 10
            spacing: 5

            RowLayout {
                spacing: 5
                Layout.fillWidth: true

                Item {
                    Layout.fillWidth: true
                }

                Button {
                    text: qsTr("Remove selected")
                }
            }

            Rectangle {
                border.color: "black"
                border.width: 1
                height: 50
                width: parent.width

                ListView {
                    boundsBehavior: Flickable.StopAtBounds
                    anchors.fill: parent
                    anchors.margins: 5
                    orientation: Qt.Horizontal
                    spacing: 3

                    delegate: Rectangle {
                        id: imageWrapper
                        height: parent.height
                        width: height
                        border.width: 0
                        border.color: black

                        MouseArea {
                            id: mouseArea
                            anchors.fill: parent
                            onClicked: imageWrapper.state == 'clicked' ? imageWrapper.state = "" : imageWrapper.state = 'clicked';
                        }

                        Image {
                            anchors.fill: parent
                            anchors.margins: 1
                            source: "image://global/" + path
                            sourceSize.width: 50
                            sourceSize.height: 50
                            fillMode: Image.PreserveAspectFit
                            asynchronous: true
                        }

                        states: [
                            State {
                                name: "clicked"
                                PropertyChanges {
                                    target: imageWrapper;
                                    border.width: 1
                                }
                            }
                        ]
                    }
                }
            }

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
                    //text: description
                    //onTextChanged: model.editdescription = text
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
                    //mainModel.removeKeywordAt(wrapperRectangle.indexOfThisDelegate, index)
                }

                function removeLastKeyword() {
                    //mainModel.removeLastKeyword(wrapperRectangle.indexOfThisDelegate)
                }

                function appendKeyword(keyword) {
                    //mainModel.appendKeyword(wrapperRectangle.indexOfThisDelegate, keyword)
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
                        //model: keywords

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
                                        opacity: closeIconMouseArea.containsMouse ? 1 : 0.5
                                        scale: closeIconMouseArea.pressed ? 0.8 : 1

                                        MouseArea {
                                            id: closeIconMouseArea
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

            Rectangle {
                width: parent.width
                height: 30

                RowLayout {
                    anchors.fill: parent
                    spacing: 5

                    Item {
                        Layout.fillWidth: true
                    }

                    Button {
                        text: qsTr("Save all")
                        onClicked: {
                            dialogComponent.destroy()
                        }
                    }

                    Button {
                        text: qsTr("Cancel")
                        onClicked: {
                            dialogComponent.destroy()
                        }
                    }
                }
            }
        }
    }
}
