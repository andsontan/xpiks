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
    id: updateWindow
    modality: "ApplicationModal"
    width: 250
    height: 150
    minimumWidth: width
    maximumWidth: width
    minimumHeight: height
    maximumHeight: height
    flags: Qt.Dialog
    title: i18.n + qsTr("Update")

    property string updateUrl

    signal dialogDestruction();
    onClosing: dialogDestruction();

    function closeUpdateWindow() {
        updateWindow.close()
    }

    Rectangle {
        color: Colors.popupBackgroundColor
        anchors.fill: parent

        Component.onCompleted: focus = true
        Keys.onEscapePressed: closeUpdateWindow()

        MouseArea {
            anchors.fill: parent
            onClicked: closeUpdateWindow()
        }

        ColumnLayout {
            anchors.centerIn: parent

            StyledText {
                anchors.horizontalCenter: parent.horizontalCenter
                text: i18.n + qsTr("Xpiks update is available!")
            }

            StyledText {
                anchors.horizontalCenter: parent.horizontalCenter
                text: i18.n + qsTr("<u>Download Xpiks update</u>")
                color: downloadMA.pressed ? Colors.linkClickedColor : Colors.artworkActiveColor

                MouseArea {
                    id: downloadMA
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        Qt.openUrlExternally(updateUrl)
                        closeUpdateWindow()
                    }
                }
            }
        }
    }
}
