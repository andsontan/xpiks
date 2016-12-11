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

#include "colorsmodel.h"
#include <QJsonObject>
#include <QVector>
#include <QString>
#include "../Common/defines.h"

#define DEFAULT_DARK_COLOR "#1e1e1e"
#define DEFAULT_DARKER_COLOR "#333333"
#define DEFAULT_CONTROL_COLOR "#292929"
#define DEFAULT_INACTIVE_CONTROL_COLOR "#404040"
#define DEFAULT_WHITE_COLOR "#ffffff"

#define DEFAULT_INPUT_BACKGROUND "#999999"
#define DEFAULT_INPUT_INACTIVE_BACKGROUND DEFAULT_CONTROL_COLOR
#define DEFAULT_INPUT_FOREGROUND "#e6e7e8"
#define DEFAULT_INPUT_INACTIVE_FOREGROUND DEFAULT_INPUT_BACKGROUND
#define DEFAULT_LABEL_ACTIVE_FOREGROUND DEFAULT_INPUT_BACKGROUND
#define DEFAULT_LABEL_INACTIVE_FOREGROUND "#545456"

#define DEFAULT_ARTWORK_BACKGROUND "#031619"
#define DEFAULT_ARTWORK_IMAGE_BACKGROUND "#081215"
#define DEFAULT_ARTWORK_MODIFIED_COLOR "#f49c12"
#define DEFAULT_ARTWORK_SAVED_COLOR "#435151"
#define DEFAULT_ARTWORK_ACTIVE_COLOR "#12b9bc"

#define DEFAULT_LIGHT_COLOR DEFAULT_INPUT_INACTIVE_FOREGROUND
#define DEFAULT_LIGHT_GRAY_COLOR "#D0D0D0"
#define DEFAULT_LIST_SEPARATOR_COLOR "#ffffff"

#define DEFAULT_BUTTON_HOVER_BACKGROUND DEFAULT_ARTWORK_ACTIVE_COLOR
#define DEFAULT_BUTTON_PRESSED_BACKGROUND "#41b1b7"
#define DEFAULT_BUTTON_PRESSED_FOREGROUND "#283c3f"
#define DEFAULT_BUTTON_DISABLED_FOREGROUND DEFAULT_BUTTON_PRESSED_FOREGROUND

#define DEFAULT_LINK_CLICKED_COLOR DEFAULT_INPUT_FOREGROUND

#define DEFAULT_SELECTED_IMAGE_BACKGROUND DEFAULT_LABEL_INACTIVE_FOREGROUND
#define DEFAULT_SELECTED_ARTWORK_BACKGROUND "#5d5d5d"
#define DEFAULT_CHECKBOX_CHECKED_COLOR DEFAULT_WHITE_COLOR

#define DEFAULT_ITEMS_SOURCE_BACKGROUND DEFAULT_DARK_COLOR
#define DEFAULT_ITEMS_SOURCE_SELECTED DEFAULT_SELECTED_IMAGE_BACKGROUND
#define DEFAULT_ITEMS_SOURCE_FOREGROUND DEFAULT_INPUT_INACTIVE_FOREGROUND

#define DEFAULT_DESTRUCTIVE_COLOR "#d10b0b"
#define DEFAULT_GREEN_COLOR "#2daf02"

#define DEFAULT_STATUS_BAR_COLOR DEFAULT_ARTWORK_IMAGE_BACKGROUND
#define DEFAULT_LEFT_SLIDER_COLOR DEFAULT_ARTWORK_IMAGE_BACKGROUND
#define DEFAULT_POPUP_BACKGROUND_COLOR DEFAULT_CONTROL_COLOR

namespace QMLExtensions {
    ColorsModel::ColorsModel(QObject *parent) : QObject(parent)
    {
        setDefaultDarkColor(QColor(DEFAULT_DARK_COLOR));
        setDefaultDarkerColor(QColor(DEFAULT_DARKER_COLOR));
        setDefaultControlColor(QColor(DEFAULT_CONTROL_COLOR));
        setInactiveControlColor(QColor(DEFAULT_INACTIVE_CONTROL_COLOR));
        setWhiteColor(QColor(DEFAULT_WHITE_COLOR));

        setInputBackgroundColor(QColor(DEFAULT_INPUT_BACKGROUND));
        setInputInactiveBackground(QColor(DEFAULT_INPUT_INACTIVE_BACKGROUND));
        setInputForegroundColor(QColor(DEFAULT_INPUT_FOREGROUND));
        setInputInactiveForeground(QColor(DEFAULT_INPUT_INACTIVE_FOREGROUND));
        setLabelActiveForeground(QColor(DEFAULT_LABEL_ACTIVE_FOREGROUND));
        setLabelInactiveForeground(QColor(DEFAULT_LABEL_INACTIVE_FOREGROUND));

        setArtworkBackground(QColor(DEFAULT_ARTWORK_BACKGROUND));
        setArtworkImageBackground(QColor(DEFAULT_ARTWORK_IMAGE_BACKGROUND));
        setArtworkModifiedColor(QColor(DEFAULT_ARTWORK_MODIFIED_COLOR));
        setArtworkSavedColor(QColor(DEFAULT_ARTWORK_SAVED_COLOR));
        setArtworkActiveColor(QColor(DEFAULT_ARTWORK_ACTIVE_COLOR));

        setDefaultLightColor(QColor(DEFAULT_LIGHT_COLOR));
        setDefaultLightGrayColor(QColor(DEFAULT_LIGHT_GRAY_COLOR));
        setListSeparatorColor(QColor(DEFAULT_LIST_SEPARATOR_COLOR));

        setButtonHoverBackground(QColor(DEFAULT_BUTTON_HOVER_BACKGROUND));
        setButtonPressedBackground(QColor(DEFAULT_BUTTON_PRESSED_BACKGROUND));
        setButtonPressedForeground(QColor(DEFAULT_BUTTON_PRESSED_FOREGROUND));
        setButtonDisabledForeground(QColor(DEFAULT_BUTTON_DISABLED_FOREGROUND));

        setLinkClickedColor(QColor(DEFAULT_LINK_CLICKED_COLOR));

        setSelectedImageBackground(QColor(DEFAULT_SELECTED_IMAGE_BACKGROUND));
        setSelectedArtworkBackground(QColor(DEFAULT_SELECTED_ARTWORK_BACKGROUND));
        setCheckboxCheckedColor(QColor(DEFAULT_CHECKBOX_CHECKED_COLOR));

        setItemsSourceBackground(QColor(DEFAULT_ITEMS_SOURCE_BACKGROUND));
        setItemsSourceSelected(QColor(DEFAULT_ITEMS_SOURCE_SELECTED));
        setItemsSourceForeground(QColor(DEFAULT_ITEMS_SOURCE_FOREGROUND));

        setDestructiveColor(QColor(DEFAULT_DESTRUCTIVE_COLOR));
        setGreenColor(QColor(DEFAULT_GREEN_COLOR));

        setStatusBarColor(QColor(DEFAULT_STATUS_BAR_COLOR));
        setLeftSliderColor(QColor(DEFAULT_LEFT_SLIDER_COLOR));
        setPopupBackgroundColor(QColor(DEFAULT_POPUP_BACKGROUND_COLOR));
    }

    void ColorsModel::registerTheme(const QJsonObject &theme) {
        const QString &themeName = theme["name"].toString(tr("Unknown"));
        LOG_DEBUG << themeName;
        m_RegisteredThemes.append(theme);
        m_ThemeNames.append(themeName);
    }

    bool ColorsModel::applyTheme(int index) {
        LOG_INFO << index;
        if (index < 0 || index >= m_RegisteredThemes.length()) { return false; }

        const QJsonObject &theme = m_RegisteredThemes.at(index);

        setDefaultDarkColor(QColor(theme["defaultDarkColor"].toString(DEFAULT_DARK_COLOR)));
        setDefaultDarkerColor(QColor(theme["defaultDarkerColor"].toString(DEFAULT_DARKER_COLOR)));
        setDefaultControlColor(QColor(theme["defaultControlColor"].toString(DEFAULT_CONTROL_COLOR)));
        setInactiveControlColor(QColor(theme["inactiveControlColor"].toString(DEFAULT_INACTIVE_CONTROL_COLOR)));
        setWhiteColor(QColor(theme["whiteColor"].toString(DEFAULT_WHITE_COLOR)));

        setInputBackgroundColor(QColor(theme["inputBackgroundColor"].toString(DEFAULT_INPUT_BACKGROUND)));
        setInputInactiveBackground(QColor(theme["inputInactiveBackground"].toString(DEFAULT_INPUT_INACTIVE_BACKGROUND)));
        setInputForegroundColor(QColor(theme["inputForegroundColor"].toString(DEFAULT_INPUT_FOREGROUND)));
        setInputInactiveForeground(QColor(theme["inputInactiveForeground"].toString(DEFAULT_INPUT_INACTIVE_FOREGROUND)));
        setLabelActiveForeground(QColor(theme["labelActiveForeground"].toString(DEFAULT_LABEL_ACTIVE_FOREGROUND)));
        setLabelInactiveForeground(QColor(theme["labelInactiveForeground"].toString(DEFAULT_LABEL_INACTIVE_FOREGROUND)));

        setArtworkBackground(QColor(theme["artworkBackground"].toString(DEFAULT_ARTWORK_BACKGROUND)));
        setArtworkImageBackground(QColor(theme["artworkImageBackground"].toString(DEFAULT_ARTWORK_IMAGE_BACKGROUND)));
        setArtworkModifiedColor(QColor(theme["artworkModifiedColor"].toString(DEFAULT_ARTWORK_MODIFIED_COLOR)));
        setArtworkSavedColor(QColor(theme["artworkSavedColor"].toString(DEFAULT_ARTWORK_SAVED_COLOR)));
        setArtworkActiveColor(QColor(theme["artworkActiveColor"].toString(DEFAULT_ARTWORK_ACTIVE_COLOR)));

        setDefaultLightColor(QColor(theme["defaultLightColor"].toString(DEFAULT_LIGHT_COLOR)));
        setDefaultLightGrayColor(QColor(theme["defaultLightGrayColor"].toString(DEFAULT_LIGHT_GRAY_COLOR)));
        setListSeparatorColor(QColor(theme["listSeparatorColor"].toString(DEFAULT_LIST_SEPARATOR_COLOR)));

        setButtonHoverBackground(QColor(theme["buttonHoverBackground"].toString(DEFAULT_BUTTON_HOVER_BACKGROUND)));
        setButtonPressedBackground(QColor(theme["buttonPressedBackground"].toString(DEFAULT_BUTTON_PRESSED_BACKGROUND)));
        setButtonPressedForeground(QColor(theme["buttonPressedForeground"].toString(DEFAULT_BUTTON_PRESSED_FOREGROUND)));
        setButtonDisabledForeground(QColor(theme["buttonDisabledForeground"].toString(DEFAULT_BUTTON_DISABLED_FOREGROUND)));

        setLinkClickedColor(QColor(theme["linkClickedColor"].toString(DEFAULT_LINK_CLICKED_COLOR)));

        setSelectedImageBackground(QColor(theme["selectedImageBackground"].toString(DEFAULT_SELECTED_IMAGE_BACKGROUND)));
        setSelectedArtworkBackground(QColor(theme["selectedArtworkBackground"].toString(DEFAULT_SELECTED_ARTWORK_BACKGROUND)));
        setCheckboxCheckedColor(QColor(theme["checkboxCheckedColor"].toString(DEFAULT_CHECKBOX_CHECKED_COLOR)));

        setItemsSourceBackground(QColor(theme["itemsSourceBackground"].toString(DEFAULT_ITEMS_SOURCE_BACKGROUND)));
        setItemsSourceSelected(QColor(theme["itemsSourceSelected"].toString(DEFAULT_ITEMS_SOURCE_SELECTED)));
        setItemsSourceForeground(QColor(theme["itemsSourceForeground"].toString(DEFAULT_ITEMS_SOURCE_FOREGROUND)));

        setDestructiveColor(QColor(theme["destructiveColor"].toString(DEFAULT_DESTRUCTIVE_COLOR)));
        setGreenColor(QColor(theme["greenColor"].toString(DEFAULT_GREEN_COLOR)));

        setStatusBarColor(QColor(theme["statusBarColor"].toString(DEFAULT_STATUS_BAR_COLOR)));
        setLeftSliderColor(QColor(theme["leftSliderColor"].toString(DEFAULT_LEFT_SLIDER_COLOR)));
        setPopupBackgroundColor(QColor(theme["popupBackgroundColor"].toString(DEFAULT_POPUP_BACKGROUND_COLOR)));

        emit themeChanged();

        return true;
    }
}
