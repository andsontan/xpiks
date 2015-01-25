/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014 Taras Kushnir <kushnirTV@gmail.com>
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

import QtQuick 2.4
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import "../Constants"
import "../Constants/Colors.js" as Colors;
import "../Components"
import "../StyledControls"

ApplicationWindow {
    id: aboutWindow
    modality: "ApplicationModal"
    width: 200
    height: 100
    minimumHeight: height
    maximumHeight: height
    minimumWidth: width
    maximumWidth: width

    function closeAbout() {
        aboutWindow.destroy();
    }

    Rectangle {
        color: Colors.selectedArtworkColor
        anchors.fill: parent

        ColumnLayout {
            anchors.centerIn: parent

            StyledText {
                text: qsTr("Version: %1").arg(appSettings.appVersion)
            }

            StyledText {
                text: qsTr("<u>Home webpage of Xpiks</u>")
                color: Colors.artworkActiveColor

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        Qt.openUrlExternally("http://ribtoks.github.io/xpiks/")
                    }
                }
            }
        }
    }
}
