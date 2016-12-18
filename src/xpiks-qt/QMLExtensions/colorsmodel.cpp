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

#include <unordered_map>

#include "../Common/defines.h"


#define DEFAULT_DARK_COLOR "defaultDarkColor"
#define DEFAULT_DARKER_COLOR "defaultDarkerColor"
#define DEFAULT_CONTROL_COLOR "defaultControlColor"
#define INACTIVE_CONTROL_COLOR "inactiveControlColor"
#define PANEL_COLOR "panelColor"
#define PANEL_SELECTED_COLOR "panelSelectedColor"
#define WHITE_COLOR "whiteColor"

#define INPUT_BACKGROUND "inputBackgroundColor"
#define INPUT_INACTIVE_BACKGROUND "inputInactiveBackground"
#define INPUT_FOREGROUND "inputForegroundColor"
#define INPUT_INACTIVE_FOREGROUND "inputInactiveForeground"
#define LABEL_ACTIVE_FOREGROUND "labelActiveForeground"
#define LABEL_INACTIVE_FOREGROUND "labelInactiveForeground"
#define INPUT_HINT_FOREGROUND "inputHintForeground"

#define ARTWORK_BACKGROUND "artworkBackground"
#define ARTWORK_MODIFIED_COLOR "artworkModifiedColor"
#define ARTWORK_SAVED_COLOR "artworkSavedColor"
#define ARTWORK_ACTIVE_COLOR "artworkActiveColor"

#define INACTIVE_KEYWORD_BACKGROUND "inactiveKeywordBackground"
#define INACTIVE_KEYWORD_FOREGROUND "inactiveKeywordForeground"
#define CLOSE_ICON_ACTIVE_COLOR "closeIconActiveColor"
#define CLOSE_ICON_INACTIVE_COLOR "closeIconInactiveColor"
#define CLOSE_ICON_DISABLED_COLOR "closeIconDisabledColor"

#define LIST_SEPARATOR_COLOR "listSeparatorColor"

#define BUTTON_DEFAULT_BACKGROUND "buttonDefaultBackground"
#define BUTTON_DEFAULT_FOREGROUND "buttonDefaultForeground"
#define BUTTON_HOVER_BACKGROUND "buttonHoverBackground"
#define BUTTON_PRESSED_BACKGROUND "buttonPressedBackground"
#define BUTTON_PRESSED_FOREGROUND "buttonPressedForeground"
#define BUTTON_DISABLED_FOREGROUND "buttonDisabledForeground"

#define LINK_CLICKED_COLOR "linkClickedColor"

#define SELECTED_ARTWORK_BACKGROUND "selectedArtworkBackground"
#define CHECKBOX_CHECKED_COLOR "checkboxCheckedColor"

#define ITEMS_SOURCE_BACKGROUND "itemsSourceBackground"
#define ITEMS_SOURCE_SELECTED "itemsSourceSelected"
#define ITEMS_SOURCE_FOREGROUND "itemsSourceForeground"

#define DESTRUCTIVE_COLOR "destructiveColor"
#define GREEN_COLOR "greenColor"

#define STATUS_BAR_COLOR "statusBarColor"
#define LEFT_SLIDER_COLOR "leftSliderColor"
#define POPUP_BACKGROUND_COLOR "popupBackgroundColor"
#define POPUP_DARK_INPUT_BACKGROUND "popupDarkInputBackground"

typedef std::unordered_map<std::string, std::string> ColorsMap;

ColorsMap createBlackTheme() {
    ColorsMap BlackTheme;

    BlackTheme["name"] = "black";

    BlackTheme[DEFAULT_DARKER_COLOR] = "#121212";
    BlackTheme[DEFAULT_DARK_COLOR] = "#1e1e1e";
    BlackTheme[DEFAULT_CONTROL_COLOR] = "#292929";
    BlackTheme[INACTIVE_CONTROL_COLOR] = "#404040";
    BlackTheme[PANEL_COLOR] = "#242424";
    BlackTheme[PANEL_SELECTED_COLOR] = "#5d5d5d";
    BlackTheme[WHITE_COLOR] = "#ffffff";

    BlackTheme[LABEL_ACTIVE_FOREGROUND] = "#c9c9c9";
    BlackTheme[LABEL_INACTIVE_FOREGROUND] = "#939393";
    BlackTheme[INPUT_BACKGROUND] = BlackTheme[PANEL_SELECTED_COLOR];
    BlackTheme[INPUT_INACTIVE_BACKGROUND] = BlackTheme[PANEL_COLOR];
    BlackTheme[INPUT_FOREGROUND] = BlackTheme[WHITE_COLOR];
    BlackTheme[INPUT_INACTIVE_FOREGROUND] = BlackTheme[LABEL_ACTIVE_FOREGROUND];
    BlackTheme[INPUT_HINT_FOREGROUND] = BlackTheme[INACTIVE_CONTROL_COLOR];

    BlackTheme[ARTWORK_BACKGROUND] = "#181818";
    //BlackTheme[ARTWORK_IMAGE_BACKGROUND] = "#071215";
    BlackTheme[ARTWORK_MODIFIED_COLOR] = "#f49c12";
    BlackTheme[ARTWORK_SAVED_COLOR] = "#435151";
    BlackTheme[ARTWORK_ACTIVE_COLOR] = "#12b9bc";

    //BlackTheme[LIGHT_COLOR] = BlackTheme[INPUT_INACTIVE_FOREGROUND];
    //BlackTheme[LIGHT_GRAY_COLOR] = "#D0D0D0";
    BlackTheme[LIST_SEPARATOR_COLOR] = BlackTheme[LABEL_ACTIVE_FOREGROUND];

    BlackTheme[BUTTON_DEFAULT_BACKGROUND] = BlackTheme[DEFAULT_CONTROL_COLOR];
    BlackTheme[BUTTON_DEFAULT_FOREGROUND] = BlackTheme[LABEL_ACTIVE_FOREGROUND];
    BlackTheme[BUTTON_HOVER_BACKGROUND] = BlackTheme[ARTWORK_ACTIVE_COLOR];
    BlackTheme[BUTTON_PRESSED_BACKGROUND] = BlackTheme[DEFAULT_CONTROL_COLOR];
    BlackTheme[BUTTON_PRESSED_FOREGROUND] = BlackTheme[WHITE_COLOR];
    BlackTheme[BUTTON_DISABLED_FOREGROUND] = BlackTheme[INPUT_BACKGROUND];

    BlackTheme[LINK_CLICKED_COLOR] = BlackTheme[WHITE_COLOR];

    BlackTheme[SELECTED_ARTWORK_BACKGROUND] = BlackTheme[INACTIVE_CONTROL_COLOR];
    BlackTheme[CHECKBOX_CHECKED_COLOR] = "#ffffff";

    BlackTheme[ITEMS_SOURCE_BACKGROUND] = BlackTheme[INACTIVE_CONTROL_COLOR];
    BlackTheme[ITEMS_SOURCE_SELECTED] = BlackTheme[INPUT_BACKGROUND];
    BlackTheme[ITEMS_SOURCE_FOREGROUND] = BlackTheme[INPUT_INACTIVE_FOREGROUND];

    BlackTheme[DESTRUCTIVE_COLOR] = "#e81313";
    BlackTheme[GREEN_COLOR] = "#2daf02";

    BlackTheme[STATUS_BAR_COLOR] = BlackTheme[DEFAULT_DARKER_COLOR];
    BlackTheme[LEFT_SLIDER_COLOR] = BlackTheme[ARTWORK_BACKGROUND];
    BlackTheme[POPUP_BACKGROUND_COLOR] = BlackTheme[INACTIVE_CONTROL_COLOR];
    BlackTheme[POPUP_DARK_INPUT_BACKGROUND] = BlackTheme[INPUT_INACTIVE_BACKGROUND];

    BlackTheme[INACTIVE_KEYWORD_BACKGROUND] = BlackTheme[INACTIVE_CONTROL_COLOR];
    BlackTheme[INACTIVE_KEYWORD_FOREGROUND] = BlackTheme[LABEL_ACTIVE_FOREGROUND];

    BlackTheme[CLOSE_ICON_ACTIVE_COLOR] = BlackTheme[INACTIVE_CONTROL_COLOR];
    BlackTheme[CLOSE_ICON_INACTIVE_COLOR] = BlackTheme[LABEL_ACTIVE_FOREGROUND];
    BlackTheme[CLOSE_ICON_DISABLED_COLOR] = BlackTheme[INACTIVE_CONTROL_COLOR];

    return BlackTheme;
}

ColorsMap createSlateGrayTheme() {
    ColorsMap SlateGrayTheme;

    SlateGrayTheme["name"] = "slate gray";

    SlateGrayTheme[DEFAULT_DARKER_COLOR] = "#15181c";
    SlateGrayTheme[DEFAULT_DARK_COLOR] = "#22252c";
    SlateGrayTheme[DEFAULT_CONTROL_COLOR] = "#30333b";
    SlateGrayTheme[INACTIVE_CONTROL_COLOR] = "#3e434d";
    SlateGrayTheme[PANEL_COLOR] = "#292c34";
    SlateGrayTheme[PANEL_SELECTED_COLOR] = "#515761";
    SlateGrayTheme[WHITE_COLOR] = "#ffffff";

    SlateGrayTheme[LABEL_ACTIVE_FOREGROUND] = "#c7ccd3";
    SlateGrayTheme[LABEL_INACTIVE_FOREGROUND] = "#979ca5";
    SlateGrayTheme[INPUT_BACKGROUND] = SlateGrayTheme[PANEL_SELECTED_COLOR];
    SlateGrayTheme[INPUT_INACTIVE_BACKGROUND] = SlateGrayTheme[PANEL_COLOR];
    SlateGrayTheme[INPUT_FOREGROUND] = SlateGrayTheme[WHITE_COLOR];
    SlateGrayTheme[INPUT_INACTIVE_FOREGROUND] = SlateGrayTheme[LABEL_ACTIVE_FOREGROUND];
    SlateGrayTheme[INPUT_HINT_FOREGROUND] = SlateGrayTheme[INACTIVE_CONTROL_COLOR];

    SlateGrayTheme[ARTWORK_BACKGROUND] = "#1e2026";
    //SlateGrayTheme[ARTWORK_IMAGE_BACKGROUND] = "#071215";
    SlateGrayTheme[ARTWORK_MODIFIED_COLOR] = "#f49c12";
    SlateGrayTheme[ARTWORK_SAVED_COLOR] = "#435151";
    SlateGrayTheme[ARTWORK_ACTIVE_COLOR] = "#2ec1c3";

    //SlateGrayTheme[LIGHT_COLOR] = SlateGrayTheme[INPUT_INACTIVE_FOREGROUND];
    //SlateGrayTheme[LIGHT_GRAY_COLOR] = "#D0D0D0";
    SlateGrayTheme[LIST_SEPARATOR_COLOR] = SlateGrayTheme[LABEL_ACTIVE_FOREGROUND];

    SlateGrayTheme[BUTTON_DEFAULT_BACKGROUND] = SlateGrayTheme[DEFAULT_CONTROL_COLOR];
    SlateGrayTheme[BUTTON_DEFAULT_FOREGROUND] = SlateGrayTheme[LABEL_ACTIVE_FOREGROUND];
    SlateGrayTheme[BUTTON_HOVER_BACKGROUND] = SlateGrayTheme[ARTWORK_ACTIVE_COLOR];
    SlateGrayTheme[BUTTON_PRESSED_BACKGROUND] = SlateGrayTheme[DEFAULT_CONTROL_COLOR];
    SlateGrayTheme[BUTTON_PRESSED_FOREGROUND] = SlateGrayTheme[WHITE_COLOR];
    SlateGrayTheme[BUTTON_DISABLED_FOREGROUND] = SlateGrayTheme[INPUT_BACKGROUND];

    SlateGrayTheme[LINK_CLICKED_COLOR] = SlateGrayTheme[WHITE_COLOR];

    SlateGrayTheme[SELECTED_ARTWORK_BACKGROUND] = SlateGrayTheme[INACTIVE_CONTROL_COLOR];
    SlateGrayTheme[CHECKBOX_CHECKED_COLOR] = "#ffffff";

    SlateGrayTheme[ITEMS_SOURCE_BACKGROUND] = SlateGrayTheme[INACTIVE_CONTROL_COLOR];
    SlateGrayTheme[ITEMS_SOURCE_SELECTED] = SlateGrayTheme[INPUT_BACKGROUND];
    SlateGrayTheme[ITEMS_SOURCE_FOREGROUND] = SlateGrayTheme[INPUT_INACTIVE_FOREGROUND];

    SlateGrayTheme[DESTRUCTIVE_COLOR] = "#e81313"; //"#A81A1A";
    SlateGrayTheme[GREEN_COLOR] = "#2daf02";

    SlateGrayTheme[STATUS_BAR_COLOR] = SlateGrayTheme[DEFAULT_DARKER_COLOR];
    SlateGrayTheme[LEFT_SLIDER_COLOR] = SlateGrayTheme[ARTWORK_BACKGROUND];
    SlateGrayTheme[POPUP_BACKGROUND_COLOR] = SlateGrayTheme[INACTIVE_CONTROL_COLOR];
    SlateGrayTheme[POPUP_DARK_INPUT_BACKGROUND] = SlateGrayTheme[INPUT_INACTIVE_BACKGROUND];

    SlateGrayTheme[INACTIVE_KEYWORD_BACKGROUND] = SlateGrayTheme[INACTIVE_CONTROL_COLOR];
    SlateGrayTheme[INACTIVE_KEYWORD_FOREGROUND] = SlateGrayTheme[LABEL_ACTIVE_FOREGROUND];

    SlateGrayTheme[CLOSE_ICON_ACTIVE_COLOR] = SlateGrayTheme[INACTIVE_CONTROL_COLOR];
    SlateGrayTheme[CLOSE_ICON_INACTIVE_COLOR] = SlateGrayTheme[LABEL_ACTIVE_FOREGROUND];
    SlateGrayTheme[CLOSE_ICON_DISABLED_COLOR] = SlateGrayTheme[INACTIVE_CONTROL_COLOR];

    return SlateGrayTheme;
}

namespace QMLExtensions {
    class HashMapColorsProvider: public ColorsProvider {
    public:
        HashMapColorsProvider(const ColorsMap &map):
            m_ColorsMap(map)
        {}

    public:
        virtual bool tryGetColor(const std::string &colorName, QColor &color) const {
            auto it = m_ColorsMap.find(colorName);
            bool contains = it != m_ColorsMap.end();
            if (contains) {
                color = QColor(QString::fromStdString(it->second));
            }

            return contains;
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

    const std::shared_ptr<ColorsProvider> &ColorsModel::getFallbackTheme() {
        return m_RegisteredThemes.at(0);
    }

    QColor ColorsModel::getColor(const std::string &colorName, const std::shared_ptr<ColorsProvider> &theme,
                                 const std::shared_ptr<ColorsProvider> &fallbackTheme) {
        QColor result;
        if (!theme->tryGetColor(colorName, result)) {
            if (!fallbackTheme->tryGetColor(colorName, result)) {
                LOG_WARNING << "Color not found:" << QString::fromStdString(colorName);
                result = QColor("#000000");
            }
        }

        return result;
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
        auto &fallback = getFallbackTheme();

        setDefaultDarkColor(getColor(DEFAULT_DARK_COLOR, theme, fallback));
        setDefaultDarkerColor(getColor(DEFAULT_DARKER_COLOR, theme, fallback));
        setDefaultControlColor(getColor(DEFAULT_CONTROL_COLOR,theme, fallback));
        setInactiveControlColor(getColor(INACTIVE_CONTROL_COLOR, theme, fallback));
        setPanelColor(getColor(PANEL_COLOR, theme, fallback));
        setPanelSelectedColor(getColor(PANEL_SELECTED_COLOR, theme, fallback));
        setWhiteColor(getColor(WHITE_COLOR, theme, fallback));

        setInputBackgroundColor(getColor(INPUT_BACKGROUND, theme, fallback));
        setInputInactiveBackground(getColor(INPUT_INACTIVE_BACKGROUND, theme, fallback));
        setInputForegroundColor(getColor(INPUT_FOREGROUND, theme, fallback));
        setInputInactiveForeground(getColor(INPUT_INACTIVE_FOREGROUND, theme, fallback));
        setLabelActiveForeground(getColor(LABEL_ACTIVE_FOREGROUND, theme, fallback));
        setLabelInactiveForeground(getColor(LABEL_INACTIVE_FOREGROUND, theme, fallback));
        setInputHintForeground(getColor(INPUT_HINT_FOREGROUND, theme, fallback));
        setPopupDarkInputBackground(getColor(POPUP_DARK_INPUT_BACKGROUND, theme, fallback));

        setArtworkBackground(getColor(ARTWORK_BACKGROUND, theme, fallback));
        setArtworkModifiedColor(getColor(ARTWORK_MODIFIED_COLOR, theme, fallback));
        setArtworkSavedColor(getColor(ARTWORK_SAVED_COLOR, theme, fallback));
        setArtworkActiveColor(getColor(ARTWORK_ACTIVE_COLOR, theme, fallback));

        setListSeparatorColor(getColor(LIST_SEPARATOR_COLOR, theme, fallback));

        setButtonDefaultBackground(getColor(BUTTON_DEFAULT_BACKGROUND, theme, fallback));
        setButtonDefaultForeground(getColor(BUTTON_DEFAULT_FOREGROUND, theme, fallback));
        setButtonHoverBackground(getColor(BUTTON_HOVER_BACKGROUND, theme, fallback));
        setButtonPressedBackground(getColor(BUTTON_PRESSED_BACKGROUND, theme, fallback));
        setButtonPressedForeground(getColor(BUTTON_PRESSED_FOREGROUND, theme, fallback));
        setButtonDisabledForeground(getColor(BUTTON_DISABLED_FOREGROUND, theme, fallback));

        setLinkClickedColor(getColor(LINK_CLICKED_COLOR, theme, fallback));

        setSelectedArtworkBackground(getColor(SELECTED_ARTWORK_BACKGROUND, theme, fallback));
        setCheckboxCheckedColor(getColor(CHECKBOX_CHECKED_COLOR, theme, fallback));

        setItemsSourceBackground(getColor(ITEMS_SOURCE_BACKGROUND, theme, fallback));
        setItemsSourceSelected(getColor(ITEMS_SOURCE_SELECTED, theme, fallback));
        setItemsSourceForeground(getColor(ITEMS_SOURCE_FOREGROUND, theme, fallback));

        setDestructiveColor(getColor(DESTRUCTIVE_COLOR, theme, fallback));
        setGreenColor(getColor(GREEN_COLOR, theme, fallback));

        setStatusBarColor(getColor(STATUS_BAR_COLOR, theme, fallback));
        setLeftSliderColor(getColor(LEFT_SLIDER_COLOR, theme, fallback));
        setPopupBackgroundColor(getColor(POPUP_BACKGROUND_COLOR, theme, fallback));
        setInactiveKeywordBackground(getColor(INACTIVE_KEYWORD_BACKGROUND, theme, fallback));
        setInactiveKeywordForeground(getColor(INACTIVE_KEYWORD_FOREGROUND, theme, fallback));

        setCloseIconActiveColor(getColor(CLOSE_ICON_ACTIVE_COLOR, theme, fallback));
        setCloseIconInactiveColor(getColor(CLOSE_ICON_INACTIVE_COLOR, theme, fallback));
        setCloseIconDisabledColor(getColor(CLOSE_ICON_DISABLED_COLOR, theme, fallback));

        emit themeChanged();

        return true;
    }
}
