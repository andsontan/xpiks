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
BlackTheme.inputInactiveBackground = BlackTheme.defaultControlColor;
BlackTheme.inputForegroundColor = "#e6e7e8";
BlackTheme.inputInactiveForeground = BlackTheme.inputBackgroundColor;
BlackTheme.labelActiveForeground = BlackTheme.inputBackgroundColor;
BlackTheme.labelInactiveForeground = "#545456";

BlackTheme.artworkBackground = "#031619";
BlackTheme.artworkImageBackground = "#071215";
BlackTheme.artworkModifiedColor = "#f49c12";
BlackTheme.artworkSavedColor = "#435151";
BlackTheme.artworkActiveColor = "#12b9bc";

BlackTheme.defaultLightColor = BlackTheme.inputInactiveForeground;
BlackTheme.defaultLightGrayColor = "#D0D0D0";

BlackTheme.buttonHoverBackground = BlackTheme.artworkActiveColor;
BlackTheme.buttonPressedBackground = "#41b1b7";
BlackTheme.buttonPressedForeground = "#283c3f";
BlackTheme.buttonDisabledForeground = BlackTheme.buttonPressedForeground;

BlackTheme.linkClickedColor = BlackTheme.inputForegroundColor;

BlackTheme.selectedImageBackground = BlackTheme.labelInactiveForeground;
BlackTheme.selectedArtworkBackground = "#5d5d5d";
BlackTheme.checkboxCheckedColor = "#ffffff";

BlackTheme.itemsSourceBackground = BlackTheme.defaultDarkColor;
BlackTheme.itemsSourceSelected = BlackTheme.selectedImageBackground;
BlackTheme.itemsSourceForeground = BlackTheme.inputInactiveForeground;

BlackTheme.destructiveColor = "#d10b0b";
BlackTheme.greenColor = "#2daf02";

// ---------------------------------------------------------------

var GraySlateTheme = { name: "grayslate" };

GraySlateTheme.defaultDarkColor = "#22252c";
GraySlateTheme.defaultControlColor = "#30333b";
GraySlateTheme.whiteColor = "#ffffff";

GraySlateTheme.inputBackgroundColor = "#7a7e86";
GraySlateTheme.inputInactiveBackground = "#30333b";
GraySlateTheme.inputForegroundColor = "#c7ccd3";
GraySlateTheme.inputInactiveForeground = "#7a7e86";
GraySlateTheme.labelInactiveForeground = "#474b54";
GraySlateTheme.labelActiveForeground = GraySlateTheme.inputBackgroundColor;

GraySlateTheme.artworkBackground = "#22252c";
GraySlateTheme.artworkImageBackground = "#1c2025";
GraySlateTheme.artworkModifiedColor = "#f49c12";
GraySlateTheme.artworkSavedColor = "#435151";
GraySlateTheme.artworkActiveColor = "#2ec1c3";

GraySlateTheme.defaultLightColor = "#E0E0E0";
GraySlateTheme.defaultLightGrayColor = "#D0D0D0";

GraySlateTheme.buttonHoverBackground = "#12b9bc";
GraySlateTheme.buttonPressedBackground = "#41b1b7";
GraySlateTheme.buttonDisabledForeground = "#283c3f";
GraySlateTheme.buttonPressedForeground = "#283c3f";

GraySlateTheme.linkClickedColor = GraySlateTheme.inputForegroundColor;

GraySlateTheme.selectedImageBackground = "#3e434d";
GraySlateTheme.selectedArtworkBackground = "#474b54";
GraySlateTheme.checkboxCheckedColor = "#ffffff";

GraySlateTheme.itemsSourceBackground = GraySlateTheme.defaultDarkColor;
GraySlateTheme.itemsSourceSelected = GraySlateTheme.selectedImageBackground;
GraySlateTheme.itemsSourceForeground = GraySlateTheme.inputInactiveForeground;

GraySlateTheme.destructiveColor = "#d10b0b";
GraySlateTheme.greenColor = "#2daf02";

var availableThemes = [BlackTheme, GraySlateTheme];
var names = availableThemes.map(function(theme) { return theme.name; })
