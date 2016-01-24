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
import QtQuick.Controls.Styles 1.1
import "../Constants"
import "../Constants/Colors.js" as Colors

CheckBox {
    property bool isContrast: false
    property int indicatorWidth: 17
    property int indicatorHeight: 17

    style: CheckBoxStyle {
        spacing: 8

        indicator: Rectangle {
            implicitHeight: indicatorHeight
            implicitWidth: indicatorWidth
            color: (control.checked || control.hovered) ? Colors.artworkActiveColor : (isContrast ? Colors.defaultDarkColor : Colors.defaultControlColor)
            opacity: control.hovered ? 0.6 : 1

            Rectangle {
                visible: control.checked
                color: Colors.checkboxCheckedColor
                width: parent.width * 0.7
                height: 2
                radius: 1
                transformOrigin: Item.Center
                rotation: 120
                transform: Translate { x: parent.width * 0.3; y: parent.height * 0.45 }
            }

            Rectangle {
                visible: control.checked
                color: Colors.checkboxCheckedColor
                width: parent.width / 2
                radius: 1
                height: 2
                transformOrigin: Item.Center
                rotation: 65
                transform: Translate { x: parent.width * 0.15; y: parent.height * 0.55 }
            }
        }

        label: StyledText {
            text: control.text
            color: control.enabled ? Colors.defaultLightGrayColor : (isContrast ? Colors.selectedArtworkColor : Colors.itemsSourceSelected)
        }
    }
}
