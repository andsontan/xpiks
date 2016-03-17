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

var BlackTheme = { name: "black" };

BlackTheme.defaultDarkColor = "#1e1e1e";
BlackTheme.defaultControlColor = "#292929";
BlackTheme.whiteColor = "#ffffff";

BlackTheme.inputBackgroundColor = "#999999";
BlackTheme.inputForegroundColor = BlackTheme.whiteColor
BlackTheme.inputInactiveForeground = "#e6e7e8";
BlackTheme.labelInactiveForeground = "#545456";
BlackTheme.inputActiveForeground = BlackTheme.inputBackgroundColor

BlackTheme.artworkBackground = "#031619";
BlackTheme.artworkImageBackground = "#071215";
BlackTheme.artworkModifiedColor = "#f49c12";
BlackTheme.artworkSavedColor = "#435151";
BlackTheme.artworkActiveColor = "#12b9bc";

BlackTheme.defaultLightColor = BlackTheme.inputInactiveForeground;
BlackTheme.defaultLightGrayColor = "#D0D0D0";

BlackTheme.itemsSourceBackground = "#232323";
BlackTheme.itemsSourceSelected = "#333333";
BlackTheme.itemsSourceForeground = BlackTheme.inputInactiveForeground;

BlackTheme.buttonHoverBackground = BlackTheme.artworkActiveColor;
BlackTheme.buttonPressedBackground = "#41b1b7";
BlackTheme.buttonPressedForeground = "#283c3f";
BlackTheme.buttonDisabledForeground = BlackTheme.buttonPressedForeground;

BlackTheme.selectedImageBackground = "#545456";
BlackTheme.selectedArtworkBackground = "#5d5d5d";
BlackTheme.checkboxCheckedColor = "#ffffff";

BlackTheme.destructiveColor = "#d10b0b";
BlackTheme.greenColor = "#2daf02";

// ---------------------------------------------------------------

var GrayTheme = {
    name: "gray",

    defaultDarkColor: "#22252c",
    defaultControlColor: "#30333b",

    defaultLightColor: "#E0E0E0",
    defaultLightGrayColor: "#D0D0D0",

    itemsSourceBackground: "#232323",
    itemsSourceSelected: "#333333",
    itemsSourceForeground: "#979ca5",

    buttonHoverBackground: "#12b9bc",
    buttonPressedBackground: "#41b1b7",
    buttonDisabledForeground: "#283c3f",
    buttonPressedForeground: "#283c3f",

    artworkBackground: "#22252c",
    artworkImageBackground: "#1c2025",
    artworkModifiedColor: "#f49c12",
    artworkSavedColor: "#435151",
    artworkActiveColor: "#2ec1c3",

    selectedImageBackground: "#3e434d",
    selectedArtworkBackground: "#474b54",
    checkboxCheckedColor: "#ffffff",
    inputBackgroundColor: "#7a7e86",

    destructiveColor: "#d10b0b",
    greenColor: "#2daf02",
    whiteColor: "#ffffff"
};

var availableThemes = [BlackTheme, GrayTheme];
var names = availableThemes.map(function(theme) { return theme.name; })
