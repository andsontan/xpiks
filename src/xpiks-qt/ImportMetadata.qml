import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Styles 1.3

Item {
    id: metadataImportComponent
    anchors.fill: parent

    function closePopup() {
        metadataImportComponent.destroy()
    }

    PropertyAnimation { target: metadataImportComponent; property: "opacity";
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
            height: 150
            radius: 10
            color: "#eeeeee"
            anchors.centerIn: parent

            ColumnLayout {
                spacing: 10
                anchors.fill: parent
                anchors.margins: 20

                Text {
                    text: qsTr("Import existing metadata")
                }

                SimpleProgressBar {
                    id: progress
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: parent.width
                    height: 20
                    color: iptcProvider.isError ? "red" : "#77B753"
                    value: iptcProvider.percent

                    onValueChanged: {
                        if (value == 100) {
                            importButton.text = qsTr("Start Import")
                            artItemsModel.updateAllProperties()
                            if (!iptcProvider.isError) {
                                closePopup()
                            }
                        }
                    }
                }

                RowLayout {
                    Item {
                        Layout.fillWidth: true
                    }

                    Button {
                        id: importButton
                        text: qsTr("Start Import")
                        enabled: !iptcProvider.inProgress
                        onClicked: {
                            text = qsTr("Importing...")
                            iptcProvider.resetModel()
                            iptcProvider.importMetadata()
                        }
                    }

                    Button {
                        text: qsTr("Close")
                        enabled: !iptcProvider.inProgress
                        onClicked: {
                            closePopup()
                        }
                    }
                }
            }
        }
    }
}
