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

ScrollView {
    style: ScrollViewStyle {
        //transientScrollBars: true
        minimumHandleLength: 20

        handle: Item {
            implicitHeight: 10
            implicitWidth: 10

            Rectangle {
                anchors.fill: parent
                radius: 5
                color: Colors.artworkActiveColor
                opacity: styleData.pressed ? 1 : (styleData.hovered ? 0.9 : 0.8)
            }
        }

        scrollBarBackground: Item {
            property bool sticky: false
            property bool hovered: styleData.hovered
            implicitWidth: 10
            implicitHeight: 10
            clip: true

            onHoveredChanged: if (hovered) sticky = true
        }

        corner: Item {}
        decrementControl: Item { width: 1; height: 1 }
        incrementControl: Item { width: 1; height: 1 }
    }
}
