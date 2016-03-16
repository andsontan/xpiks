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
 * but WITHOUT ANY WARRANTY, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

var BlackTheme = {
    name: "black",

    defaultDarkColor: "#1e1e1e",
    defaultControlColor: "#292929",

    defaultLightColor: "#E0E0E0",
    defaultLightGrayColor: "#D0D0D0",

    itemsSourceBackground: "#232323",
    itemsSourceSelected: "#333333",
    itemsSourceForeground: "#e6e6e6",

    buttonHoverBackground: "#12b9bc",
    buttonPressedBackground: "#41b1b7",
    buttonDisabledForeground: "#283c3f",
    buttonPressedForeground: "#283c3f",

    artworkBackground: "#031619",
    artworkImageBackground: "#071215",
    artworkModifiedColor: "#f49c12",
    artworkSavedColor: "#435151",
    artworkActiveColor: "#12b9bc",

    selectedArtworkColor: "#545456",
    selectedMetadataColor: "#5d5d5d",
    checkboxCheckedColor: "#ffffff",
    defaultInputBackground: "#999999",

    destructiveColor: "#d10b0b",
    greenColor: "#2daf02",
    whiteColor: "#ffffff"
};

var WhiteTheme = {
    name: "white",

    defaultDarkColor: "#dddddd",
    defaultControlColor: "#eeeeee",

    defaultLightColor: "#E0E0E0",
    defaultLightGrayColor: "#D0D0D0",

    itemsSourceBackground: "#232323",
    itemsSourceSelected: "#333333",
    itemsSourceForeground: "#e6e6e6",

    buttonHoverBackground: "#12b9bc",
    buttonPressedBackground: "#41b1b7",
    buttonDisabledForeground: "#283c3f",
    buttonPressedForeground: "#283c3f",

    artworkBackground: "#031619",
    artworkImageBackground: "#071215",
    artworkModifiedColor: "#f49c12",
    artworkSavedColor: "#435151",
    artworkActiveColor: "#12b9bc",

    selectedArtworkColor: "#545456",
    selectedMetadataColor: "#5d5d5d",
    checkboxCheckedColor: "#ffffff",
    defaultInputBackground: "#999999",

    destructiveColor: "#d10b0b",
    greenColor: "#2daf02",
    whiteColor: "#ffffff"
};

var availableThemes = [BlackTheme, WhiteTheme];
var names = availableThemes.map(function(theme) { return theme.name; })
