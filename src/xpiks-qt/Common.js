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

function movePopupInsideComponent(component, window, mouse, old_x, old_y) {
    //            var tmp = root.mapToItem(img,mouse.x,mouse.y);
    var tmp = mapToItem(component, mouse.x, mouse.y);

    var delta_x = tmp.x - old_x;
    var nextX = Math.max(window.x + delta_x, 10);

    if ((nextX + window.width) >= (component.width - 10)) {
        nextX = component.width - 10 - window.width
    }

    window.x = nextX;
    old_x = tmp.x;

    var delta_y = tmp.y - old_y;
    var nextY = Math.max(window.y + delta_y, 10);

    if ((nextY + window.height) >= (component.height - 10)) {
        nextY = component.height - 10 - window.height
    }

    window.y = nextY;
    old_y = tmp.y;

    return [old_x, old_y];
}

function isInComponent(point, component) {
    var result = (point.x >= 0 && point.x <= component.width) &&
            (point.y >= 0 && point.y <= component.height);
    return result;
}

function launchDialog(componentName, directParent, options, functor) {
    var component = Qt.createComponent(componentName);
    if (component.status !== Component.Ready) {
        console.debug("Component Error: " + component.errorString());
    } else {
        var instance = component.createObject(directParent, options);        

        if (typeof instance.onDialogDestruction !== "undefined") {
            if (typeof directParent.openedDialogsCount !== "undefined") {
                directParent.openedDialogsCount += 1
            }

            if (typeof directParent.onDialogClosed !== "undefined") {
                instance.onDialogDestruction.connect(directParent.onDialogClosed);
            }
        }

        if (functor) {
            functor(instance);
        }
    }
}

