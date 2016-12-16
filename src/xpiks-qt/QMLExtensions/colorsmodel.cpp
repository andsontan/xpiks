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

        setDefaultDarkColor(getColor("defaultDarkColor", theme, fallback));
        setDefaultDarkerColor(getColor("defaultDarkerColor", theme, fallback));
        setDefaultControlColor(getColor("defaultControlColor",theme, fallback));
        setInactiveControlColor(getColor("inactiveControlColor", theme, fallback));
        setWhiteColor(getColor("whiteColor", theme, fallback));

        setInputBackgroundColor(getColor("inputBackgroundColor", theme, fallback));
        setInputInactiveBackground(getColor("inputInactiveBackground", theme, fallback));
        setInputForegroundColor(getColor("inputForegroundColor", theme, fallback));
        setInputInactiveForeground(getColor("inputInactiveForeground", theme, fallback));
        setLabelActiveForeground(getColor("labelActiveForeground", theme, fallback));
        setLabelInactiveForeground(getColor("labelInactiveForeground", theme, fallback));

        setArtworkBackground(getColor("artworkBackground", theme, fallback));
        setArtworkImageBackground(getColor("artworkImageBackground", theme, fallback));
        setArtworkModifiedColor(getColor("artworkModifiedColor", theme, fallback));
        setArtworkSavedColor(getColor("artworkSavedColor", theme, fallback));
        setArtworkActiveColor(getColor("artworkActiveColor", theme, fallback));

        setDefaultLightColor(getColor("defaultLightColor", theme, fallback));
        setDefaultLightGrayColor(getColor("defaultLightGrayColor", theme, fallback));
        setListSeparatorColor(getColor("listSeparatorColor", theme, fallback));

        setButtonHoverBackground(getColor("buttonHoverBackground", theme, fallback));
        setButtonPressedBackground(getColor("buttonPressedBackground", theme, fallback));
        setButtonPressedForeground(getColor("buttonPressedForeground", theme, fallback));
        setButtonDisabledForeground(getColor("buttonDisabledForeground", theme, fallback));

        setLinkClickedColor(getColor("linkClickedColor", theme, fallback));

        setSelectedImageBackground(getColor("selectedImageBackground", theme, fallback));
        setSelectedArtworkBackground(getColor("selectedArtworkBackground", theme, fallback));
        setCheckboxCheckedColor(getColor("checkboxCheckedColor", theme, fallback));

        setItemsSourceBackground(getColor("itemsSourceBackground", theme, fallback));
        setItemsSourceSelected(getColor("itemsSourceSelected", theme, fallback));
        setItemsSourceForeground(getColor("itemsSourceForeground", theme, fallback));

        setDestructiveColor(getColor("destructiveColor", theme, fallback));
        setGreenColor(getColor("greenColor", theme, fallback));

        setStatusBarColor(getColor("statusBarColor", theme, fallback));
        setLeftSliderColor(getColor("leftSliderColor", theme, fallback));
        setPopupBackgroundColor(getColor("popupBackgroundColor", theme, fallback));

        emit themeChanged();

        return true;
    }
}
