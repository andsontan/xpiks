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

CheckBox {
    property bool isContrast: false
    property int indicatorWidth: 17
    property int indicatorHeight: 17
    checked: true
    property color labelColor: enabled ? Colors.labelActiveForeground : (isContrast ? Colors.inputBackgroundColor : Colors.labelInactiveForeground)
    property bool isBold: false

    style: CheckBoxStyle {
        spacing: 10

        indicator: Rectangle {
            implicitHeight: indicatorHeight
            implicitWidth: indicatorWidth
            color: (control.checked || control.hovered) ? Colors.artworkActiveColor : (isContrast ? Colors.defaultDarkColor : Colors.defaultControlColor)
            opacity: control.hovered ? 0.6 : 1

            Rectangle {
                visible: control.checked
                color: Colors.checkboxCheckedColor
                width: parent.width * 0.55
                height: parent.width*0.1
                radius: height / 2
                transformOrigin: Item.TopLeft
                rotation: 235
                x: parent.width * 0.45 + radius
                y: parent.height * 0.8 + radius
            }

            Rectangle {
                visible: control.checked
                color: Colors.checkboxCheckedColor
                width: parent.width * 0.7
                radius: height / 2
                height: parent.width*0.1
                transformOrigin: Item.TopLeft
                rotation: 300
                x: parent.width * 0.45
                y: parent.height * 0.8
            }
        }

        label: StyledText {
            text: control.text
            color: control.labelColor
            font.bold: control.isBold
        }
    }
}
