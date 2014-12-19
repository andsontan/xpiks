import QtQuick 2.3

Item {
    width: 16
    height: 16

    signal itemClicked();

    Rectangle {
        anchors.fill: parent
        border.color: "black"
        border.width: 1
        radius: 8

        Rectangle {
            color: "black"
            width: parent.width - 2
            height: 2
            radius: 1
            border.width: 1
            border.color: "black"
            transformOrigin: Item.Center
            rotation: 45
            anchors.centerIn: parent
        }

        Rectangle {
            color: "black"
            width: parent.width - 2
            height: 2
            radius: 1
            border.width: 1
            border.color: "black"
            transformOrigin: Item.Center
            rotation: 135
            anchors.centerIn: parent
        }

        scale: mouseArea.pressed ? 0.8 : 1

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            hoverEnabled: true
            preventStealing: true
            onClicked: {
                itemClicked()
            }
        }
    }
}

