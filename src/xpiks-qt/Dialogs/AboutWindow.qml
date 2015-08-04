/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2015 Taras Kushnir <kushnirTV@gmail.com>
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

import QtQuick 2.3
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import "../Constants"
import "../Constants/Colors.js" as Colors;
import "../Components"
import "../StyledControls"

ApplicationWindow {
    id: aboutWindow
    modality: "ApplicationModal"
    width: 250
    height: 150
    minimumWidth: width
    maximumWidth: width
    minimumHeight: height
    maximumHeight: height
    flags: Qt.Dialog
    title: qsTr("About")

    function closeAbout() {
        aboutWindow.destroy();
    }

    Rectangle {
        color: Colors.selectedArtworkColor
        anchors.fill: parent

        ColumnLayout {
            anchors.centerIn: parent

            StyledText {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Version: %1").arg(appSettings.appVersion)
            }

            StyledText {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("<u>Home webpage of Xpiks</u>")
                color: aboutMA.pressed ? Colors.defaultLightColor : Colors.artworkActiveColor

                MouseArea {
                    id: aboutMA
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        Qt.openUrlExternally("http://ribtoks.github.io/xpiks/")
                    }
                }
            }

            Item {
                height: 10
            }

            Rectangle {
                height: 50
                width: 220
                anchors.margins: 10
                color: "transparent"

                StyledText {
                    wrapMode: TextEdit.Wrap
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignHCenter
                    text: qsTr("The program is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE")
                }
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: closeAbout()
        }
    }
}
