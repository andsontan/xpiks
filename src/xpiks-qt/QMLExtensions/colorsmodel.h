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

#ifndef COLORSMODEL_H
#define COLORSMODEL_H

#include <QObject>
#include <QColor>
#include <QJsonObject>
#include <QVector>
#include <QStringList>

#include <memory>
#include <vector>
#include <string>

namespace QMLExtensions {
    class ColorsProvider {
    public:
        virtual ~ColorsProvider() {}
        virtual bool tryGetColor(const std::string &colorName, QColor &color) const = 0;
        virtual QString getThemeName() const = 0;
    };

    class ColorsModel : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(QColor defaultDarkColor READ defaultDarkColor WRITE setDefaultDarkColor NOTIFY defaultDarkColorChanged)
        Q_PROPERTY(QColor defaultDarkerColor READ defaultDarkerColor WRITE setDefaultDarkerColor NOTIFY defaultDarkerColorChanged)
        Q_PROPERTY(QColor defaultControlColor READ defaultControlColor WRITE setDefaultControlColor NOTIFY defaultControlColorChanged)
        Q_PROPERTY(QColor whiteColor READ whiteColor WRITE setWhiteColor NOTIFY whiteColorChanged)
        Q_PROPERTY(QColor panelColor READ panelColor WRITE setPanelColor NOTIFY panelColorChanged)
        Q_PROPERTY(QColor panelSelectedColor READ panelSelectedColor WRITE setPanelSelectedColor NOTIFY panelSelectedColorChanged)

        Q_PROPERTY(QColor inputBackgroundColor READ inputBackgroundColor WRITE setInputBackgroundColor NOTIFY inputBackgroundColorChanged)
        Q_PROPERTY(QColor inputInactiveBackground READ inputInactiveBackground WRITE setInputInactiveBackground NOTIFY inputInactiveBackgroundChanged)
        Q_PROPERTY(QColor inputForegroundColor READ inputForegroundColor WRITE setInputForegroundColor NOTIFY inputForegroundColorChanged)
        Q_PROPERTY(QColor inputInactiveForeground READ inputInactiveForeground WRITE setInputInactiveForeground NOTIFY inputInactiveForegroundChanged)
        Q_PROPERTY(QColor labelActiveForeground READ labelActiveForeground WRITE setLabelActiveForeground NOTIFY labelActiveForegroundChanged)
        Q_PROPERTY(QColor labelInactiveForeground READ labelInactiveForeground WRITE setLabelInactiveForeground NOTIFY labelInactiveForegroundChanged)
        Q_PROPERTY(QColor inputHintForeground READ inputHintForeground WRITE setInputHintForeground NOTIFY inputHintForegroundChanged)
        Q_PROPERTY(QColor popupDarkInputBackground READ popupDarkInputBackground WRITE setPopupDarkInputBackground NOTIFY popupDarkInputBackgroundChanged)

        Q_PROPERTY(QColor artworkBackground READ artworkBackground WRITE setArtworkBackground NOTIFY artworkBackgroundChanged)
        Q_PROPERTY(QColor artworkModifiedColor READ artworkModifiedColor WRITE setArtworkModifiedColor NOTIFY artworkModifiedColorChanged)
        Q_PROPERTY(QColor artworkSavedColor READ artworkSavedColor WRITE setArtworkSavedColor NOTIFY artworkSavedColorChanged)
        Q_PROPERTY(QColor artworkActiveColor READ artworkActiveColor WRITE setArtworkActiveColor NOTIFY artworkActiveColorChanged)

        Q_PROPERTY(QColor listSeparatorColor READ listSeparatorColor WRITE setListSeparatorColor NOTIFY listSeparatorColorChanged)

        Q_PROPERTY(QColor buttonHoverBackground READ buttonHoverBackground WRITE setButtonHoverBackground NOTIFY buttonHoverBackgroundChanged)
        Q_PROPERTY(QColor buttonPressedBackground READ buttonPressedBackground WRITE setButtonPressedBackground NOTIFY buttonPressedBackgroundChanged)
        Q_PROPERTY(QColor buttonPressedForeground READ buttonPressedForeground WRITE setButtonPressedForeground NOTIFY buttonPressedForegroundChanged)
        Q_PROPERTY(QColor buttonDisabledForeground READ buttonDisabledForeground WRITE setButtonDisabledForeground NOTIFY buttonDisabledForegroundChanged)
        Q_PROPERTY(QColor buttonDefaultBackground READ buttonDefaultBackground WRITE setButtonDefaultBackground NOTIFY buttonDefaultBackgroundChanged)
        Q_PROPERTY(QColor buttonDefaultForeground READ buttonDefaultForeground WRITE setButtonDefaultForeground NOTIFY buttonDefaultForegroundChanged)

        Q_PROPERTY(QColor linkClickedColor READ linkClickedColor WRITE setLinkClickedColor NOTIFY linkClickedColorChanged)

        Q_PROPERTY(QColor selectedArtworkBackground READ selectedArtworkBackground WRITE setSelectedArtworkBackground NOTIFY selectedArtworkBackgroundChanged)
        Q_PROPERTY(QColor checkboxCheckedColor READ checkboxCheckedColor WRITE setCheckboxCheckedColor NOTIFY checkboxCheckedColorChanged)

        Q_PROPERTY(QColor itemsSourceBackground READ itemsSourceBackground WRITE setItemsSourceBackground NOTIFY itemsSourceBackgroundChanged)
        Q_PROPERTY(QColor itemsSourceSelected READ itemsSourceSelected WRITE setItemsSourceSelected NOTIFY itemsSourceSelectedChanged)
        Q_PROPERTY(QColor itemsSourceForeground READ itemsSourceForeground WRITE setItemsSourceForeground NOTIFY itemsSourceForegroundChanged)

        Q_PROPERTY(QColor destructiveColor READ destructiveColor WRITE setDestructiveColor NOTIFY destructiveColorChanged)
        Q_PROPERTY(QColor greenColor READ greenColor WRITE setGreenColor NOTIFY greenColorChanged)

        Q_PROPERTY(QColor statusBarColor READ statusBarColor WRITE setStatusBarColor NOTIFY statusBarColorChanged)
        Q_PROPERTY(QColor leftSliderColor READ leftSliderColor WRITE setLeftSliderColor NOTIFY leftSliderColorChanged)
        Q_PROPERTY(QColor popupBackgroundColor READ popupBackgroundColor WRITE setPopupBackgroundColor NOTIFY popupBackgroundColorChanged)
        Q_PROPERTY(QColor inactiveControlColor READ inactiveControlColor WRITE setInactiveControlColor NOTIFY inactiveControlColorChanged)
        Q_PROPERTY(QColor inactiveKeywordBackground READ inactiveKeywordBackground WRITE setInactiveKeywordBackground NOTIFY inactiveKeywordBackgroundChanged)
        Q_PROPERTY(QColor inactiveKeywordForeground READ inactiveKeywordForeground WRITE setInactiveKeywordForeground NOTIFY inactiveKeywordForegroundChanged)

        Q_PROPERTY(QColor closeIconActiveColor READ closeIconActiveColor WRITE setCloseIconActiveColor NOTIFY closeIconActiveColorChanged)
        Q_PROPERTY(QColor closeIconInactiveColor READ closeIconInactiveColor WRITE setCloseIconInactiveColor NOTIFY closeIconInactiveColorChanged)
        Q_PROPERTY(QColor closeIconDisabledColor READ closeIconDisabledColor WRITE setCloseIconDisabledColor NOTIFY closeIconDisabledColorChanged)

        std::vector<std::shared_ptr<ColorsProvider> > m_RegisteredThemes;
        QStringList m_ThemeNames;

        // AUTOGENERATED
        QColor m_defaultDarkColor;
        QColor m_defaultDarkerColor;
        QColor m_defaultControlColor;
        QColor m_whiteColor;
        QColor m_inputBackgroundColor;
        QColor m_inputInactiveBackground;
        QColor m_inputForegroundColor;
        QColor m_inputInactiveForeground;
        QColor m_labelActiveForeground;
        QColor m_labelInactiveForeground;
        QColor m_artworkBackground;
        QColor m_artworkModifiedColor;
        QColor m_artworkSavedColor;
        QColor m_artworkActiveColor;
        QColor m_buttonHoverBackground;
        QColor m_buttonPressedBackground;
        QColor m_buttonPressedForeground;
        QColor m_buttonDisabledForeground;
        QColor m_linkClickedColor;
        QColor m_selectedArtworkBackground;
        QColor m_checkboxCheckedColor;
        QColor m_itemsSourceBackground;
        QColor m_itemsSourceSelected;
        QColor m_itemsSourceForeground;
        QColor m_destructiveColor;
        QColor m_greenColor;
        QColor m_listSeparatorColor;
        QColor m_statusBarColor;
        QColor m_leftSliderColor;        
        QColor m_popupBackgroundColor;
        QColor m_inactiveControlColor;
        QColor m_panelColor;
        QColor m_panelSelectedColor;
        QColor m_buttonDefaultBackground;
        QColor m_buttonDefaultForeground;
        QColor m_inactiveKeywordBackground;
        QColor m_inactiveKeywordForeground;
        QColor m_closeIconActiveColor;
        QColor m_closeIconInactiveColor;
        QColor m_closeIconDisabledColor;
        QColor m_inputHintForeground;
        QColor m_popupDarkInputBackground;

    public:
        explicit ColorsModel(QObject *parent = 0);

        const std::shared_ptr<ColorsProvider> &getFallbackTheme();
        QColor getColor(const std::string &colorName, const std::shared_ptr<ColorsProvider> &theme,
                        const std::shared_ptr<ColorsProvider> &fallbackTheme);
        void registerTheme(const std::shared_ptr<ColorsProvider> &provider);

        Q_INVOKABLE bool applyTheme(int index);
        Q_INVOKABLE QStringList getThemeNames() const { return m_ThemeNames; }

        QColor defaultDarkColor() const
        {
            return m_defaultDarkColor;
        }

        QColor defaultControlColor() const
        {
            return m_defaultControlColor;
        }

        QColor whiteColor() const
        {
            return m_whiteColor;
        }

        QColor inputBackgroundColor() const
        {
            return m_inputBackgroundColor;
        }

        QColor inputInactiveBackground() const
        {
            return m_inputInactiveBackground;
        }

        QColor inputForegroundColor() const
        {
            return m_inputForegroundColor;
        }

        QColor inputInactiveForeground() const
        {
            return m_inputInactiveForeground;
        }

        QColor labelActiveForeground() const
        {
            return m_labelActiveForeground;
        }

        QColor labelInactiveForeground() const
        {
            return m_labelInactiveForeground;
        }

        QColor artworkBackground() const
        {
            return m_artworkBackground;
        }

        QColor artworkModifiedColor() const
        {
            return m_artworkModifiedColor;
        }

        QColor artworkSavedColor() const
        {
            return m_artworkSavedColor;
        }

        QColor artworkActiveColor() const
        {
            return m_artworkActiveColor;
        }

        QColor buttonHoverBackground() const
        {
            return m_buttonHoverBackground;
        }

        QColor buttonPressedBackground() const
        {
            return m_buttonPressedBackground;
        }

        QColor buttonPressedForeground() const
        {
            return m_buttonPressedForeground;
        }

        QColor buttonDisabledForeground() const
        {
            return m_buttonDisabledForeground;
        }

        QColor linkClickedColor() const
        {
            return m_linkClickedColor;
        }

        QColor selectedArtworkBackground() const
        {
            return m_selectedArtworkBackground;
        }

        QColor checkboxCheckedColor() const
        {
            return m_checkboxCheckedColor;
        }

        QColor itemsSourceBackground() const
        {
            return m_itemsSourceBackground;
        }

        QColor itemsSourceSelected() const
        {
            return m_itemsSourceSelected;
        }

        QColor itemsSourceForeground() const
        {
            return m_itemsSourceForeground;
        }

        QColor destructiveColor() const
        {
            return m_destructiveColor;
        }

        QColor greenColor() const
        {
            return m_greenColor;
        }

        QColor defaultDarkerColor() const
        {
            return m_defaultDarkerColor;
        }

        QColor listSeparatorColor() const
        {
            return m_listSeparatorColor;
        }

        QColor statusBarColor() const
        {
            return m_statusBarColor;
        }

        QColor leftSliderColor() const
        {
            return m_leftSliderColor;
        }

        QColor popupBackgroundColor() const
        {
            return m_popupBackgroundColor;
        }

        QColor inactiveControlColor() const
        {
            return m_inactiveControlColor;
        }

        QColor panelColor() const
        {
            return m_panelColor;
        }

        QColor panelSelectedColor() const
        {
            return m_panelSelectedColor;
        }

        QColor buttonDefaultBackground() const
        {
            return m_buttonDefaultBackground;
        }

        QColor buttonDefaultForeground() const
        {
            return m_buttonDefaultForeground;
        }

        QColor inactiveKeywordBackground() const
        {
            return m_inactiveKeywordBackground;
        }

        QColor inactiveKeywordForeground() const
        {
            return m_inactiveKeywordForeground;
        }

        QColor closeIconActiveColor() const
        {
            return m_closeIconActiveColor;
        }

        QColor closeIconInactiveColor() const
        {
            return m_closeIconInactiveColor;
        }

        QColor closeIconDisabledColor() const
        {
            return m_closeIconDisabledColor;
        }

        QColor inputHintForeground() const
        {
            return m_inputHintForeground;
        }

        QColor popupDarkInputBackground() const
        {
            return m_popupDarkInputBackground;
        }

    signals:
        void themeChanged();
        void defaultDarkColorChanged(QColor defaultDarkColor);
        void defaultControlColorChanged(QColor defaultControlColor);
        void whiteColorChanged(QColor whiteColor);
        void inputBackgroundColorChanged(QColor inputBackgroundColor);
        void inputInactiveBackgroundChanged(QColor inputInactiveBackground);
        void inputForegroundColorChanged(QColor inputForegroundColor);
        void inputInactiveForegroundChanged(QColor inputInactiveForeground);
        void labelActiveForegroundChanged(QColor labelActiveForeground);
        void labelInactiveForegroundChanged(QColor labelInactiveForeground);
        void artworkBackgroundChanged(QColor artworkBackground);
        void artworkModifiedColorChanged(QColor artworkModifiedColor);
        void artworkSavedColorChanged(QColor artworkSavedColor);
        void artworkActiveColorChanged(QColor artworkActiveColor);
        void buttonHoverBackgroundChanged(QColor buttonHoverBackground);
        void buttonPressedBackgroundChanged(QColor buttonPressedBackground);
        void buttonPressedForegroundChanged(QColor buttonPressedForeground);
        void buttonDisabledForegroundChanged(QColor buttonDisabledForeground);
        void linkClickedColorChanged(QColor linkClickedColor);
        void selectedArtworkBackgroundChanged(QColor selectedArtworkBackground);
        void checkboxCheckedColorChanged(QColor checkboxCheckedColor);
        void itemsSourceBackgroundChanged(QColor itemsSourceBackground);
        void itemsSourceSelectedChanged(QColor itemsSourceSelected);
        void itemsSourceForegroundChanged(QColor itemsSourceForeground);
        void destructiveColorChanged(QColor destructiveColor);
        void greenColorChanged(QColor greenColor);        
        void defaultDarkerColorChanged(QColor defaultDarkerColor);
        void listSeparatorColorChanged(QColor listSeparatorColor);
        void statusBarColorChanged(QColor statusBarColor);
        void leftSliderColorChanged(QColor leftSliderColor);        
        void popupBackgroundColorChanged(QColor popupBackgroundColor);
        void inactiveControlColorChanged(QColor inactiveControlColor);
        void panelColorChanged(QColor panelColor);
        void panelSelectedColorChanged(QColor panelSelectedColor);
        void buttonDefaultBackgroundChanged(QColor buttonDefaultBackground);
        void buttonDefaultForegroundChanged(QColor buttonDefaultForeground);
        void inactiveKeywordBackgroundChanged(QColor inactiveKeywordBackground);
        void inactiveKeywordForegroundChanged(QColor inactiveKeywordForeground);
        void closeIconActiveColorChanged(QColor closeIconActiveColor);
        void closeIconInactiveColorChanged(QColor closeIconInactiveColor);
        void closeIconDisabledColorChanged(QColor closeIconDisabledColor);
        void inputHintForegroundChanged(QColor inputHintForeground);
        void popupDarkInputBackgroundChanged(QColor popupInputBackground);

    public slots:
        void setDefaultDarkColor(QColor defaultDarkColor)
        {
            if (m_defaultDarkColor == defaultDarkColor)
                return;

            m_defaultDarkColor = defaultDarkColor;
            emit defaultDarkColorChanged(defaultDarkColor);
        }
        void setDefaultControlColor(QColor defaultControlColor)
        {
            if (m_defaultControlColor == defaultControlColor)
                return;

            m_defaultControlColor = defaultControlColor;
            emit defaultControlColorChanged(defaultControlColor);
        }
        void setWhiteColor(QColor whiteColor)
        {
            if (m_whiteColor == whiteColor)
                return;

            m_whiteColor = whiteColor;
            emit whiteColorChanged(whiteColor);
        }
        void setInputBackgroundColor(QColor inputBackgroundColor)
        {
            if (m_inputBackgroundColor == inputBackgroundColor)
                return;

            m_inputBackgroundColor = inputBackgroundColor;
            emit inputBackgroundColorChanged(inputBackgroundColor);
        }
        void setInputInactiveBackground(QColor inputInactiveBackground)
        {
            if (m_inputInactiveBackground == inputInactiveBackground)
                return;

            m_inputInactiveBackground = inputInactiveBackground;
            emit inputInactiveBackgroundChanged(inputInactiveBackground);
        }
        void setInputForegroundColor(QColor inputForegroundColor)
        {
            if (m_inputForegroundColor == inputForegroundColor)
                return;

            m_inputForegroundColor = inputForegroundColor;
            emit inputForegroundColorChanged(inputForegroundColor);
        }
        void setInputInactiveForeground(QColor inputInactiveForeground)
        {
            if (m_inputInactiveForeground == inputInactiveForeground)
                return;

            m_inputInactiveForeground = inputInactiveForeground;
            emit inputInactiveForegroundChanged(inputInactiveForeground);
        }
        void setLabelActiveForeground(QColor labelActiveForeground)
        {
            if (m_labelActiveForeground == labelActiveForeground)
                return;

            m_labelActiveForeground = labelActiveForeground;
            emit labelActiveForegroundChanged(labelActiveForeground);
        }
        void setLabelInactiveForeground(QColor labelInactiveForeground)
        {
            if (m_labelInactiveForeground == labelInactiveForeground)
                return;

            m_labelInactiveForeground = labelInactiveForeground;
            emit labelInactiveForegroundChanged(labelInactiveForeground);
        }
        void setArtworkBackground(QColor artworkBackground)
        {
            if (m_artworkBackground == artworkBackground)
                return;

            m_artworkBackground = artworkBackground;
            emit artworkBackgroundChanged(artworkBackground);
        }
        void setArtworkModifiedColor(QColor artworkModifiedColor)
        {
            if (m_artworkModifiedColor == artworkModifiedColor)
                return;

            m_artworkModifiedColor = artworkModifiedColor;
            emit artworkModifiedColorChanged(artworkModifiedColor);
        }
        void setArtworkSavedColor(QColor artworkSavedColor)
        {
            if (m_artworkSavedColor == artworkSavedColor)
                return;

            m_artworkSavedColor = artworkSavedColor;
            emit artworkSavedColorChanged(artworkSavedColor);
        }
        void setArtworkActiveColor(QColor artworkActiveColor)
        {
            if (m_artworkActiveColor == artworkActiveColor)
                return;

            m_artworkActiveColor = artworkActiveColor;
            emit artworkActiveColorChanged(artworkActiveColor);
        }
        void setButtonHoverBackground(QColor buttonHoverBackground)
        {
            if (m_buttonHoverBackground == buttonHoverBackground)
                return;

            m_buttonHoverBackground = buttonHoverBackground;
            emit buttonHoverBackgroundChanged(buttonHoverBackground);
        }
        void setButtonPressedBackground(QColor buttonPressedBackground)
        {
            if (m_buttonPressedBackground == buttonPressedBackground)
                return;

            m_buttonPressedBackground = buttonPressedBackground;
            emit buttonPressedBackgroundChanged(buttonPressedBackground);
        }
        void setButtonPressedForeground(QColor buttonPressedForeground)
        {
            if (m_buttonPressedForeground == buttonPressedForeground)
                return;

            m_buttonPressedForeground = buttonPressedForeground;
            emit buttonPressedForegroundChanged(buttonPressedForeground);
        }
        void setButtonDisabledForeground(QColor buttonDisabledForeground)
        {
            if (m_buttonDisabledForeground == buttonDisabledForeground)
                return;

            m_buttonDisabledForeground = buttonDisabledForeground;
            emit buttonDisabledForegroundChanged(buttonDisabledForeground);
        }
        void setLinkClickedColor(QColor linkClickedColor)
        {
            if (m_linkClickedColor == linkClickedColor)
                return;

            m_linkClickedColor = linkClickedColor;
            emit linkClickedColorChanged(linkClickedColor);
        }
        void setSelectedArtworkBackground(QColor selectedArtworkBackground)
        {
            if (m_selectedArtworkBackground == selectedArtworkBackground)
                return;

            m_selectedArtworkBackground = selectedArtworkBackground;
            emit selectedArtworkBackgroundChanged(selectedArtworkBackground);
        }
        void setCheckboxCheckedColor(QColor checkboxCheckedColor)
        {
            if (m_checkboxCheckedColor == checkboxCheckedColor)
                return;

            m_checkboxCheckedColor = checkboxCheckedColor;
            emit checkboxCheckedColorChanged(checkboxCheckedColor);
        }
        void setItemsSourceBackground(QColor itemsSourceBackground)
        {
            if (m_itemsSourceBackground == itemsSourceBackground)
                return;

            m_itemsSourceBackground = itemsSourceBackground;
            emit itemsSourceBackgroundChanged(itemsSourceBackground);
        }
        void setItemsSourceSelected(QColor itemsSourceSelected)
        {
            if (m_itemsSourceSelected == itemsSourceSelected)
                return;

            m_itemsSourceSelected = itemsSourceSelected;
            emit itemsSourceSelectedChanged(itemsSourceSelected);
        }
        void setItemsSourceForeground(QColor itemsSourceForeground)
        {
            if (m_itemsSourceForeground == itemsSourceForeground)
                return;

            m_itemsSourceForeground = itemsSourceForeground;
            emit itemsSourceForegroundChanged(itemsSourceForeground);
        }
        void setDestructiveColor(QColor destructiveColor)
        {
            if (m_destructiveColor == destructiveColor)
                return;

            m_destructiveColor = destructiveColor;
            emit destructiveColorChanged(destructiveColor);
        }
        void setGreenColor(QColor greenColor)
        {
            if (m_greenColor == greenColor)
                return;

            m_greenColor = greenColor;
            emit greenColorChanged(greenColor);
        }
        void setDefaultDarkerColor(QColor defaultDarkerColor)
        {
            if (m_defaultDarkerColor == defaultDarkerColor)
                return;

            m_defaultDarkerColor = defaultDarkerColor;
            emit defaultDarkerColorChanged(defaultDarkerColor);
        }
        void setListSeparatorColor(QColor listSeparatorColor)
        {
            if (m_listSeparatorColor == listSeparatorColor)
                return;

            m_listSeparatorColor = listSeparatorColor;
            emit listSeparatorColorChanged(listSeparatorColor);
        }
        void setStatusBarColor(QColor statusBarColor)
        {
            if (m_statusBarColor == statusBarColor)
                return;

            m_statusBarColor = statusBarColor;
            emit statusBarColorChanged(statusBarColor);
        }
        void setLeftSliderColor(QColor leftSliderColor)
        {
            if (m_leftSliderColor == leftSliderColor)
                return;

            m_leftSliderColor = leftSliderColor;
            emit leftSliderColorChanged(leftSliderColor);
        }
        void setPopupBackgroundColor(QColor popupBackgroundColor)
        {
            if (m_popupBackgroundColor == popupBackgroundColor)
                return;

            m_popupBackgroundColor = popupBackgroundColor;
            emit popupBackgroundColorChanged(popupBackgroundColor);
        }
        void setInactiveControlColor(QColor inactiveControlColor)
        {
            if (m_inactiveControlColor == inactiveControlColor)
                return;

            m_inactiveControlColor = inactiveControlColor;
            emit inactiveControlColorChanged(inactiveControlColor);
        }
        void setPanelColor(QColor panelColor)
        {
            if (m_panelColor == panelColor)
                return;

            m_panelColor = panelColor;
            emit panelColorChanged(panelColor);
        }
        void setPanelSelectedColor(QColor panelSelectedColor)
        {
            if (m_panelSelectedColor == panelSelectedColor)
                return;

            m_panelSelectedColor = panelSelectedColor;
            emit panelSelectedColorChanged(panelSelectedColor);
        }
        void setButtonDefaultBackground(QColor buttonDefaultBackground)
        {
            if (m_buttonDefaultBackground == buttonDefaultBackground)
                return;

            m_buttonDefaultBackground = buttonDefaultBackground;
            emit buttonDefaultBackgroundChanged(buttonDefaultBackground);
        }
        void setButtonDefaultForeground(QColor buttonDefaultForeground)
        {
            if (m_buttonDefaultForeground == buttonDefaultForeground)
                return;

            m_buttonDefaultForeground = buttonDefaultForeground;
            emit buttonDefaultForegroundChanged(buttonDefaultForeground);
        }
        void setInactiveKeywordBackground(QColor inactiveKeywordBackground)
        {
            if (m_inactiveKeywordBackground == inactiveKeywordBackground)
                return;

            m_inactiveKeywordBackground = inactiveKeywordBackground;
            emit inactiveKeywordBackgroundChanged(inactiveKeywordBackground);
        }
        void setInactiveKeywordForeground(QColor inactiveKeywordForeground)
        {
            if (m_inactiveKeywordForeground == inactiveKeywordForeground)
                return;

            m_inactiveKeywordForeground = inactiveKeywordForeground;
            emit inactiveKeywordForegroundChanged(inactiveKeywordForeground);
        }
        void setCloseIconActiveColor(QColor closeIconActiveColor)
        {
            if (m_closeIconActiveColor == closeIconActiveColor)
                return;

            m_closeIconActiveColor = closeIconActiveColor;
            emit closeIconActiveColorChanged(closeIconActiveColor);
        }
        void setCloseIconInactiveColor(QColor closeIconInactiveColor)
        {
            if (m_closeIconInactiveColor == closeIconInactiveColor)
                return;

            m_closeIconInactiveColor = closeIconInactiveColor;
            emit closeIconInactiveColorChanged(closeIconInactiveColor);
        }
        void setCloseIconDisabledColor(QColor closeIconDisabledColor)
        {
            if (m_closeIconDisabledColor == closeIconDisabledColor)
                return;

            m_closeIconDisabledColor = closeIconDisabledColor;
            emit closeIconDisabledColorChanged(closeIconDisabledColor);
        }
        void setInputHintForeground(QColor inputHintForeground)
        {
            if (m_inputHintForeground == inputHintForeground)
                return;

            m_inputHintForeground = inputHintForeground;
            emit inputHintForegroundChanged(inputHintForeground);
        }
        void setPopupDarkInputBackground(QColor popupInputBackground)
        {
            if (m_popupDarkInputBackground == popupInputBackground)
                return;

            m_popupDarkInputBackground = popupInputBackground;
            emit popupDarkInputBackgroundChanged(popupInputBackground);
        }
    };
}

#endif // COLORSMODEL_H
