import QtQuick 2.0

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
        width: 100
        height: 62
        radius: 10
        anchors.centerIn: parent

        Text {
            anchors.centerIn: parent
            text: "This is the popup"
        }

        // For demo I do not put any buttons, or other fancy stuff on the popup
        // clicking the whole dialogWindow will dismiss it
        MouseArea{
            anchors.fill: parent
            onClicked: {
                // destroy object is needed when you dynamically create it
                dialogComponent.destroy()
            }
        }
    }
}
