import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1

ApplicationWindow {
    visible: true
    width: 800
    height: 600
    color: "white"

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

    Rectangle {
        width: 300
        height: 200

        EditableTags {
            id: flv
            model: myModel
            anchors.fill: parent

            delegate: Rectangle {
                id: itemWrapper
                property int indexOfThisDelegate: index
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
                                onClicked: {
                                    myModel.remove(itemWrapper.indexOfThisDelegate, 1)
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
                myModel.append({ itemText: text })
            }

            onRemoveLast: {
                myModel.remove(myModel.count - 1, 1);
            }

            onCurrentItemChanged: {
                console.log(currentItem.foo)
            }
        }
    }

}
