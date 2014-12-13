import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1

ApplicationWindow {
    visible: true
    width: 800
    height: 600

    ListModel {
        id: myModel
        ListElement { itemText: "tag1" }
        ListElement { itemText: "tag" }
        ListElement { itemText: "tagkjkjjkhkjhkjh" }
        ListElement { itemText: "tag1" }
        ListElement { itemText: "Yet another long tag" }
        ListElement { itemText: "Yet another long tag" }
        ListElement { itemText: "tag1" }
        ListElement { itemText: "tagkjkjjkhkjhkjh" }
        ListElement { itemText: "tagkjkjjkhkjhkjh" }
        ListElement { itemText: "Yet another long tag" }
        ListElement { itemText: "Yet another long tag" }
        ListElement { itemText: "Yet another long tag" }
        ListElement { itemText: "tag1" }
        ListElement { itemText: "tagkjkjjkhkjhkjh" }
        ListElement { itemText: "Yet another long tag" }
        ListElement { itemText: "tag1" }
        ListElement { itemText: "Yet another long tag" }
        ListElement { itemText: "tag1" }
        ListElement { itemText: "tag1" }
    }

    EditableTags {
        id: flv
        model: myModel
        anchors.fill: parent

        delegate: Rectangle {
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
                        text: itemText
                    }
                }

                Image {
                    source: "qrc:/CloseIcon.svg"
                    sourceSize.width: 15
                    sourceSize.height: 15

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            // remove current item from flow
                        }
                    }
                }

                Item {
                    width: 5
                }
            }
        }

        onCurrentItemChanged: {
            console.log(currentItem.foo)
        }
    }
}
