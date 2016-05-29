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
import "../Constants"

Item {
    id: scrollbar;
    width: (handleSize + 2);
    visible: (flickable.visibleArea.heightRatio < 1.0);

    anchors {
        top: flickable.top;
        right: flickable.right;
        bottom: flickable.bottom;
        margins: 1;
    }

    property Flickable flickable               : null;
    property int       handleSize              : 10;

    Binding {
        target: handle;
        property: "y";
        value: (flickable.contentY * clicker.drag.maximumY / (flickable.contentHeight - flickable.height));
        when: (!clicker.drag.active);
    }

    Binding {
        target: flickable;
        property: "contentY";
        value: (handle.y * (flickable.contentHeight - flickable.height) / clicker.drag.maximumY);
        when: (clicker.drag.active || clicker.pressed);
    }

    Item {
        id: groove;
        clip: true;
        anchors {
            fill: parent;
            margins: 1
        }

        MouseArea {
            id: clicker;
            drag {
                target: handle;
                minimumY: 0;
                maximumY: (groove.height - handle.height);
                axis: Drag.YAxis;
            }
            anchors { fill: parent; }
            onClicked: { flickable.contentY = (mouse.y / groove.height * (flickable.contentHeight - flickable.height)); }
        }

        Item {
            id: handle;
            height: flickable.visibleArea.heightRatio * groove.height
            width: 10
            anchors {
                left: parent.left;
                right: parent.right;
            }

            Rectangle {
                id: backHandle
                anchors.fill: parent
                radius: 5
                color: Colors.artworkActiveColor
                opacity: clicker.pressed ? 1 : (clicker.containsMouse ? 0.9 : 0.8)
            }
        }
    }
}

