/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2016 Taras Kushnir <kushnirTV@gmail.com>
 *
 * Xpiks is distributed under the GNU General Public License, version 3.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.1
import QtQuick.Controls.Styles 1.1
import "../Constants"
import "../Constants/Colors.js" as Colors;
import "../Common.js" as Common;
import "../Components"
import "../StyledControls"

Item {
    id: metadataImportComponent
    anchors.fill: parent

    property bool isInProgress: false

    Keys.onEscapePressed: {
        if (!metadataImportComponent.isInProgress) {
            closePopup()
        }

        event.accepted = true
    }

    function closePopup() {
        metadataImportComponent.destroy()
    }

    Component.onCompleted: {
        focus = true
    }

    signal dialogDestruction();
    Component.onDestruction: dialogDestruction();

    MessageDialog {
        id: errorsNotification
        title: "Warning"
        text: i18.n + qsTr("Import finished with errors. See logs for details.")

        onAccepted: {
            closePopup()
        }
    }

    MessageDialog {
        id: installExiftoolDialog
        title: "Warning"
        text: i18.n + qsTr("Please install Exiftool to import metadata")
        informativeText: i18.n + qsTr('<a href="http://www.sno.phy.queensu.ca/~phil/exiftool/">Official Exiftool website</a>')
        standardButtons: StandardButton.Abort | StandardButton.Ignore
        onRejected: {
            closePopup()
        }
        onAccepted: {
            continueImport()
        }
    }

    function continueImport() {
        importButton.text = i18.n + qsTr("Importing...")
        metadataImportComponent.isInProgress = true

        spinner.height = spinner.width
        dialogWindow.height += spinner.height + column.spacing
        spinner.running = true

        metadataIOCoordinator.readMetadata(ignoreAutosavesCheckbox.checked)
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
        id: focusScope

        MouseArea {
            anchors.fill: parent
            onWheel: wheel.accepted = true
            onClicked: mouse.accepted = true;
            onDoubleClicked: mouse.accepted = true

            property real old_x : 0
            property real old_y : 0

            onPressed:{
                var tmp = mapToItem(metadataImportComponent, mouse.x, mouse.y);
                old_x = tmp.x;
                old_y = tmp.y;
            }

            onPositionChanged: {
                var old_xy = Common.movePopupInsideComponent(metadataImportComponent, dialogWindow, mouse, old_x, old_y);
                old_x = old_xy[0]; old_y = old_xy[1];
            }
        }

        // This rectangle is the actual popup
        Rectangle {
            id: dialogWindow
            width: 380
            height: 130
            color: Colors.selectedArtworkColor
            anchors.centerIn: parent
            Component.onCompleted: anchors.centerIn = undefined

            Behavior on height {
                NumberAnimation {
                    duration: 200
                    easing.type: Easing.InQuad
                }
            }

            Column {
                id: column
                spacing: 20
                anchors.fill: parent
                anchors.margins: 20

                add: Transition {
                    NumberAnimation { properties: "x,y"; easing.type: Easing.InQuad; duration: 200 }
                }

                move: Transition {
                    NumberAnimation { properties: "x,y"; easing.type: Easing.InQuad; duration: 200 }
                }

                Item {
                    height: childrenRect.height
                    anchors.left: parent.left
                    anchors.right: parent.right

                    StyledText {
                        anchors.left: parent.left
                        text: i18.n + qsTr("Import existing metadata")
                    }

                    StyledText {
                        anchors.right: parent.right
                        text: i18.n + qsTr("from %1 image(s)").arg(metadataIOCoordinator.processingItemsCount)
                        color: Colors.defaultInputBackground
                    }
                }

                StyledBusyIndicator {
                    id: spinner
                    width: 150
                    height: 0
                    anchors.horizontalCenter: parent.horizontalCenter
                    running: false
                }

                StyledCheckbox {
                    id: ignoreAutosavesCheckbox
                    text: i18.n + qsTr("Ignore autosaves (.xpks)")
                    enabled: settingsModel.saveBackups && !metadataImportComponent.isInProgress
                    checked: false
                }

                RowLayout {
                    height: 24
                    anchors.left: parent.left
                    anchors.right: parent.right

                    StyledButton {
                        id: importButton
                        isDefault: true
                        width: 130
                        text: i18.n + qsTr("Start Import")
                        enabled: !metadataImportComponent.isInProgress
                        onClicked: {
                            if (metadataIOCoordinator.exiftoolNotFound) {
                                installExiftoolDialog.open()
                            } else {
                                continueImport()
                            }
                        }

                        Connections {
                            target: metadataIOCoordinator
                            onMetadataReadingFinished: {
                                console.log("UI::ImportMetadata # Import finished handler")

                                metadataImportComponent.isInProgress = false
                                warningsModel.update()

                                if (metadataIOCoordinator.hasErrors) {
                                    errorsNotification.open()
                                } else {
                                    importButton.text = i18.n + qsTr("Start Import")
                                    closePopup()
                                }
                            }
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    StyledButton {
                        text: i18.n + qsTr("Close")
                        width: 100
                        enabled: !metadataImportComponent.isInProgress
                        onClicked: {
                            closePopup()
                        }
                    }
                }
            }

        }
    }
}
