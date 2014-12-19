import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Styles 1.3

Item {
    id: artworkPatcherComponent
    anchors.fill: parent

    function closePopup() {
        artworkPatcherComponent.destroy()
    }

    PropertyAnimation { target: artworkPatcherComponent; property: "opacity";
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

    FocusScope {
        anchors.fill: parent

        // This rectangle is the actual popup
        Rectangle {
            id: dialogWindow
            width: 480
            height: 100
            radius: 10
            color: "#eeeeee"
            anchors.centerIn: parent

            ColumnLayout {
                spacing: 10
                anchors.fill: parent
                anchors.margins: 20

                SimpleProgressBar {
                    id: progress
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: parent.width
                    height: 20
                    color: iptcProvider.isError ? "red" : "#77B753"
                    value: iptcProvider.percent
                }

                RowLayout {
                    Item {
                        Layout.fillWidth: true
                    }

                    Button {
                        text: qsTr("Run")
                        enabled: !iptcProvider.inProgress
                        onClicked: {
                            iptcProvider.resetModel()
                            iptcProvider.patchArtworks()
                        }
                    }

                    Button {
                        text: qsTr("Close")
                        enabled: !iptcProvider.inProgress
                        onClicked: {
                            if (!iptcProvider.isError) {
                                artItemsModel.setSelectedItemsSaved()
                            }

                            closePopup()
                        }
                    }
                }
            }
        }
    }
}
