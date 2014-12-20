import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Styles 1.3

Item {
    id: metadataExportComponent
    anchors.fill: parent

    function closePopup() {
        metadataExportComponent.destroy()
    }

    PropertyAnimation { target: metadataExportComponent; property: "opacity";
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
                    text: qsTr("Export metadata")
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
                            exportButton.text = qsTr("Start Export")
                            artItemsModel.updateAllProperties()
                        }
                    }
                }

                RowLayout {
                    Item {
                        Layout.fillWidth: true
                    }

                    Button {
                        id: exportButton
                        text: qsTr("Start Export")
                        enabled: !iptcProvider.inProgress
                        onClicked: {
                            text = qsTr("Exporting...")
                            iptcProvider.resetModel()
                            iptcProvider.exportMetadata()
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
