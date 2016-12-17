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
import QtQuick.Dialogs 1.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import "../Constants"
import "../Components"
import "../StyledControls"

ApplicationWindow {
    id: aboutWindow
    modality: "ApplicationModal"
    width: 250
    height: Qt.platform.os === "linux" ? 200 : 190
    minimumWidth: width
    maximumWidth: width
    minimumHeight: height
    maximumHeight: height
    flags: Qt.Dialog
    title: i18.n + qsTr("About")

    signal dialogDestruction();
    onClosing: dialogDestruction();
    Component.onDestruction: dialogDestruction();

    function closeAbout() {
        aboutWindow.close();
    }

    Rectangle {
        color: Colors.popupBackgroundColor
        anchors.fill: parent

        Component.onCompleted: focus = true
        Keys.onEscapePressed: closeAbout()

        MouseArea {
            anchors.fill: parent
            onClicked: closeAbout()
        }

        ColumnLayout {
            anchors.centerIn: parent

            StyledText {
                color: Colors.inputForegroundColor
                anchors.horizontalCenter: parent.horizontalCenter
                text: i18.n + qsTr("Version: %1").arg(appSettings.appVersion)
            }

            StyledText {
                focus: true
                anchors.horizontalCenter: parent.horizontalCenter
                text: i18.n + qsTr("<u>Xpiks webpage</u>")
                color: aboutMA.pressed ? Colors.linkClickedColor : Colors.artworkActiveColor

                MouseArea {
                    id: aboutMA
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    hoverEnabled: true
                    onClicked: {
                        Qt.openUrlExternally("https://ribtoks.github.io/xpiks/")
                    }
                }
            }

            Item {
                height: 10
            }

            Item {
                height: Qt.platform.os === "linux" ? 100 : 80
                width: 220
                anchors.margins: 10

                StyledText {
                    color: Colors.inputForegroundColor
                    wrapMode: TextEdit.Wrap
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignHCenter
                    text: i18.n + qsTr("The program is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE")
                }
            }
        }
    }
}
