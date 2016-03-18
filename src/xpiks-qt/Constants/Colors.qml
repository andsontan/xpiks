pragma Singleton
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
import QtQuick 2.0

QtObject {
    property color defaultDarkColor: "#1e1e1e";
    property color defaultControlColor: "#292929";
    property color whiteColor: "#ffffff";

    property color inputBackgroundColor: "#999999";
    property color inputInactiveBackground: defaultControlColor
    property color inputForegroundColor: "#e6e7e8";
    property color inputInactiveForeground: inputBackgroundColor;
    property color labelActiveForeground: inputBackgroundColor;
    property color labelInactiveForeground: "#545456";

    property color artworkBackground: "#031619";
    property color artworkImageBackground: "#081215";
    property color artworkModifiedColor: "#f49c12";
    property color artworkSavedColor: "#435151";
    property color artworkActiveColor: "#12b9bc"

    property color defaultLightColor: inputInactiveForeground;
    property color defaultLightGrayColor: "#D0D0D0";

    property color buttonHoverBackground: artworkActiveColor;
    property color buttonPressedBackground: "#41b1b7";
    property color buttonPressedForeground: "#283c3f";
    property color buttonDisabledForeground: buttonPressedForeground

    property color linkClickedColor: inputForegroundColor

    property color selectedImageBackground: labelInactiveForeground
    property color selectedArtworkBackground: "#5d5d5d";
    property color checkboxCheckedColor: "#ffffff";

    property color itemsSourceBackground: defaultDarkColor;
    property color itemsSourceSelected: selectedImageBackground;
    property color itemsSourceForeground: inputInactiveForeground;

    property color destructiveColor: "#d10b0b";
    property color greenColor: "#2daf02";

    function initTheme(theme) {
        defaultDarkColor = theme.defaultDarkColor
        defaultControlColor = theme.defaultControlColor
        whiteColor = theme.whiteColor

        defaultLightColor = theme.defaultLightColor
        defaultLightGrayColor = theme.defaultLightGrayColor

        inputBackgroundColor = theme.inputBackgroundColor
        inputInactiveBackground = theme.inputInactiveBackground
        inputForegroundColor = theme.inputForegroundColor
        inputInactiveForeground = theme.inputInactiveForeground
        labelInactiveForeground = theme.labelInactiveForeground
        labelActiveForeground = theme.labelActiveForeground

        artworkBackground = theme.artworkBackground
        artworkImageBackground = theme.artworkImageBackground
        artworkModifiedColor = theme.artworkModifiedColor
        artworkSavedColor = theme.artworkSavedColor
        artworkActiveColor = theme.artworkActiveColor

        buttonHoverBackground = theme.buttonHoverBackground
        buttonPressedBackground = theme.buttonPressedBackground
        buttonPressedForeground = theme.buttonPressedForeground
        buttonDisabledForeground = theme.buttonDisabledForeground

        linkClickedColor = theme.linkClickedColor

        selectedImageBackground = theme.selectedImageBackground
        selectedArtworkBackground = theme.selectedArtworkBackground
        checkboxCheckedColor = theme.checkboxCheckedColor

        itemsSourceBackground = theme.itemsSourceBackground
        itemsSourceSelected = theme.selectedImageBackground
        itemsSourceForeground = theme.inputInactiveForeground

        destructiveColor = theme.destructiveColor
        greenColor = theme.greenColor
    }
}
