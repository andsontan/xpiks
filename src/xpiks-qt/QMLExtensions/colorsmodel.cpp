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
#include <QHash>
#include "../Common/defines.h"

#include <unordered_map>

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

typedef std::unordered_map<std::string, std::string> ColorsMap;

ColorsMap createBlackTheme() {
    ColorsMap BlackTheme;

    BlackTheme["name"] = "black";

    BlackTheme["defaultDarkColor"] = "#1e1e1e";
    BlackTheme["defaultDarkerColor"] = "#333333";
    BlackTheme["defaultControlColor"] = "#292929";
    BlackTheme["inactiveControlColor"] = "#404040";
    BlackTheme["whiteColor"] = "#ffffff";

    BlackTheme["labelActiveForeground"] = "#c9c9c9";
    BlackTheme["labelInactiveForeground"] = "#939393";
    BlackTheme["inputBackgroundColor"] = "#5d5d5d";
    BlackTheme["inputInactiveBackground"] = "#242424";
    BlackTheme["inputForegroundColor"] = BlackTheme["whiteColor"]; //"#e6e7e8";
    BlackTheme["inputInactiveForeground"] = BlackTheme["labelActiveForeground"];

    BlackTheme["artworkBackground"] = "#181818";
    BlackTheme["artworkImageBackground"] = "#071215";
    BlackTheme["artworkModifiedColor"] = "#f49c12";
    BlackTheme["artworkSavedColor"] = "#435151";
    BlackTheme["artworkActiveColor"] = "#12b9bc";

    BlackTheme["defaultLightColor"] = BlackTheme["inputInactiveForeground"];
    BlackTheme["defaultLightGrayColor"] = "#D0D0D0";
    BlackTheme["listSeparatorColor"] = BlackTheme["labelActiveForeground"];

    BlackTheme["buttonHoverBackground"] = BlackTheme["artworkActiveColor"];
    BlackTheme["buttonPressedBackground"] = BlackTheme["defaultControlColor"];
    BlackTheme["buttonPressedForeground"] = BlackTheme["whiteColor"];
    BlackTheme["buttonDisabledForeground"] = BlackTheme["inputBackgroundColor"];

    BlackTheme["linkClickedColor"] = BlackTheme["inputForegroundColor"];

    BlackTheme["selectedImageBackground"] = BlackTheme["labelInactiveForeground"];
    BlackTheme["selectedArtworkBackground"] = "#404040";
    BlackTheme["checkboxCheckedColor"] = "#ffffff";

    BlackTheme["itemsSourceBackground"] = BlackTheme["selectedArtworkBackground"];
    BlackTheme["itemsSourceSelected"] = BlackTheme["selectedImageBackground"];
    BlackTheme["itemsSourceForeground"] = BlackTheme["inputInactiveForeground"];

    BlackTheme["destructiveColor"] = "#e81313";
    BlackTheme["greenColor"] = "#2daf02";

    BlackTheme["statusBarColor"] = "#121212";
    BlackTheme["leftSliderColor"] = BlackTheme["artworkBackground"];
    BlackTheme["popupBackgroundColor"] = BlackTheme["defaultControlColor"];

    return BlackTheme;
}

ColorsMap createSlateGrayTheme() {
    ColorsMap SlateGrayTheme;

    SlateGrayTheme["name"] = "slate gray";

    SlateGrayTheme["defaultDarkColor"] = "#22252c";
    SlateGrayTheme["defaultDarkerColor"] = "#373a42";
    SlateGrayTheme["defaultControlColor"] = "#30333b";
    SlateGrayTheme["whiteColor"] = "#ffffff";

    SlateGrayTheme["inputBackgroundColor"] = "#7a7e86";
    SlateGrayTheme["inputInactiveBackground"] = SlateGrayTheme["defaultControlColor"];
    SlateGrayTheme["inputForegroundColor"] = "#c7ccd3";
    SlateGrayTheme["inputInactiveForeground"] = SlateGrayTheme["inputBackgroundColor"];
    SlateGrayTheme["labelInactiveForeground"] = "#474b54";
    SlateGrayTheme["labelActiveForeground"] = SlateGrayTheme["inputBackgroundColor"];

    SlateGrayTheme["artworkBackground"] = SlateGrayTheme["defaultDarkColor"];
    SlateGrayTheme["artworkImageBackground"] = "#1c2025";
    SlateGrayTheme["artworkModifiedColor"] = "#f49c12";
    SlateGrayTheme["artworkSavedColor"] = SlateGrayTheme["inputBackgroundColor"];
    SlateGrayTheme["artworkActiveColor"] = "#2ec1c3";

    SlateGrayTheme["defaultLightColor"] = "#E0E0E0";
    SlateGrayTheme["defaultLightGrayColor"] = "#D0D0D0";
    SlateGrayTheme["listSeparatorColor"] = SlateGrayTheme["defaultLightColor"];

    SlateGrayTheme["buttonHoverBackground"] = "#12b9bc";
    SlateGrayTheme["buttonPressedBackground"] = "#41b1b7";
    SlateGrayTheme["buttonDisabledForeground"] = "#283c3f";
    SlateGrayTheme["buttonPressedForeground"] = "#283c3f";

    SlateGrayTheme["linkClickedColor"] = SlateGrayTheme["inputForegroundColor"];

    SlateGrayTheme["selectedImageBackground"] = "#3e434d";
    SlateGrayTheme["selectedArtworkBackground"] = "#474b54";
    SlateGrayTheme["checkboxCheckedColor"] = "#ffffff";

    SlateGrayTheme["itemsSourceBackground"] = SlateGrayTheme["selectedImageBackground"];
    SlateGrayTheme["itemsSourceSelected"] = SlateGrayTheme["itemsSourceBackground"];
    SlateGrayTheme["itemsSourceForeground"] = SlateGrayTheme["inputForegroundColor"];

    SlateGrayTheme["destructiveColor"] = "#A81A1A";
    SlateGrayTheme["greenColor"] = "#2daf02";

    SlateGrayTheme["statusBarColor"] = SlateGrayTheme["artworkImageBackground"];
    SlateGrayTheme["leftSliderColor"] = SlateGrayTheme["artworkImageBackground"];
    SlateGrayTheme["popupBackgroundColor"] = SlateGrayTheme["defaultControlColor"];

    return SlateGrayTheme;
}

namespace QMLExtensions {
    class HashMapColorsProvider: public ColorsProvider {
    public:
        HashMapColorsProvider(const ColorsMap &map):
            m_ColorsMap(map)
        {}

    public:
        virtual QColor getColor(const std::string &colorName, const std::string &defaultValue) const {
            auto it = m_ColorsMap.find(colorName);
            if (it != m_ColorsMap.end()) {
                return QColor(QString::fromStdString(it->second));
            } else {
                return QColor(QString::fromStdString(defaultValue));
            }
        }

        virtual QString getThemeName() const {
            auto it = m_ColorsMap.find("name");
            if (it != m_ColorsMap.end()) {
                return QString::fromStdString(it->second);
            } else {
                return QLatin1String("unknown");
            }
        }

    private:
        ColorsMap m_ColorsMap;
    };

    ColorsModel::ColorsModel(QObject *parent) : QObject(parent)
    {
        registerTheme(std::shared_ptr<ColorsProvider>(new HashMapColorsProvider(createBlackTheme())));
        registerTheme(std::shared_ptr<ColorsProvider>(new HashMapColorsProvider(createSlateGrayTheme())));
        applyTheme(0);
    }

    void ColorsModel::registerTheme(const std::shared_ptr<ColorsProvider> &provider) {
        QString themeName = provider->getThemeName();
        LOG_DEBUG << themeName;
        m_RegisteredThemes.push_back(provider);
        m_ThemeNames.append(themeName);
    }

    bool ColorsModel::applyTheme(int index) {
        LOG_INFO << index << "out of" << m_RegisteredThemes.size();
        if (index < 0 || index >= (int)m_RegisteredThemes.size()) { return false; }

        auto &theme = m_RegisteredThemes[index];

        setDefaultDarkColor(theme->getColor("defaultDarkColor", DEFAULT_DARK_COLOR));
        setDefaultDarkerColor(theme->getColor("defaultDarkerColor", DEFAULT_DARKER_COLOR));
        setDefaultControlColor(theme->getColor("defaultControlColor",DEFAULT_CONTROL_COLOR));
        setInactiveControlColor(theme->getColor("inactiveControlColor", DEFAULT_INACTIVE_CONTROL_COLOR));
        setWhiteColor(theme->getColor("whiteColor", DEFAULT_WHITE_COLOR));

        setInputBackgroundColor(theme->getColor("inputBackgroundColor", DEFAULT_INPUT_BACKGROUND));
        setInputInactiveBackground(theme->getColor("inputInactiveBackground", DEFAULT_INPUT_INACTIVE_BACKGROUND));
        setInputForegroundColor(theme->getColor("inputForegroundColor", DEFAULT_INPUT_FOREGROUND));
        setInputInactiveForeground(theme->getColor("inputInactiveForeground", DEFAULT_INPUT_INACTIVE_FOREGROUND));
        setLabelActiveForeground(theme->getColor("labelActiveForeground", DEFAULT_LABEL_ACTIVE_FOREGROUND));
        setLabelInactiveForeground(theme->getColor("labelInactiveForeground", DEFAULT_LABEL_INACTIVE_FOREGROUND));

        setArtworkBackground(theme->getColor("artworkBackground", DEFAULT_ARTWORK_BACKGROUND));
        setArtworkImageBackground(theme->getColor("artworkImageBackground", DEFAULT_ARTWORK_IMAGE_BACKGROUND));
        setArtworkModifiedColor(theme->getColor("artworkModifiedColor", DEFAULT_ARTWORK_MODIFIED_COLOR));
        setArtworkSavedColor(theme->getColor("artworkSavedColor", DEFAULT_ARTWORK_SAVED_COLOR));
        setArtworkActiveColor(theme->getColor("artworkActiveColor", DEFAULT_ARTWORK_ACTIVE_COLOR));

        setDefaultLightColor(theme->getColor("defaultLightColor", DEFAULT_LIGHT_COLOR));
        setDefaultLightGrayColor(theme->getColor("defaultLightGrayColor", DEFAULT_LIGHT_GRAY_COLOR));
        setListSeparatorColor(theme->getColor("listSeparatorColor", DEFAULT_LIST_SEPARATOR_COLOR));

        setButtonHoverBackground(theme->getColor("buttonHoverBackground", DEFAULT_BUTTON_HOVER_BACKGROUND));
        setButtonPressedBackground(theme->getColor("buttonPressedBackground", DEFAULT_BUTTON_PRESSED_BACKGROUND));
        setButtonPressedForeground(theme->getColor("buttonPressedForeground", DEFAULT_BUTTON_PRESSED_FOREGROUND));
        setButtonDisabledForeground(theme->getColor("buttonDisabledForeground", DEFAULT_BUTTON_DISABLED_FOREGROUND));

        setLinkClickedColor(theme->getColor("linkClickedColor", DEFAULT_LINK_CLICKED_COLOR));

        setSelectedImageBackground(theme->getColor("selectedImageBackground", DEFAULT_SELECTED_IMAGE_BACKGROUND));
        setSelectedArtworkBackground(theme->getColor("selectedArtworkBackground", DEFAULT_SELECTED_ARTWORK_BACKGROUND));
        setCheckboxCheckedColor(theme->getColor("checkboxCheckedColor", DEFAULT_CHECKBOX_CHECKED_COLOR));

        setItemsSourceBackground(theme->getColor("itemsSourceBackground", DEFAULT_ITEMS_SOURCE_BACKGROUND));
        setItemsSourceSelected(theme->getColor("itemsSourceSelected", DEFAULT_ITEMS_SOURCE_SELECTED));
        setItemsSourceForeground(theme->getColor("itemsSourceForeground", DEFAULT_ITEMS_SOURCE_FOREGROUND));

        setDestructiveColor(theme->getColor("destructiveColor", DEFAULT_DESTRUCTIVE_COLOR));
        setGreenColor(theme->getColor("greenColor", DEFAULT_GREEN_COLOR));

        setStatusBarColor(theme->getColor("statusBarColor", DEFAULT_STATUS_BAR_COLOR));
        setLeftSliderColor(theme->getColor("leftSliderColor", DEFAULT_LEFT_SLIDER_COLOR));
        setPopupBackgroundColor(theme->getColor("popupBackgroundColor", DEFAULT_POPUP_BACKGROUND_COLOR));

        emit themeChanged();

        return true;
    }
}
