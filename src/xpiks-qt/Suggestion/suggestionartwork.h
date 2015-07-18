/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014 Taras Kushnir <kushnirTV@gmail.com>
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


#ifndef SUGGESTIONARTWORK_H
#define SUGGESTIONARTWORK_H

#include <QStringList>
#include <QString>

namespace Suggestion {
    class SuggestionArtwork
    {
    public:
        SuggestionArtwork(const QString &url, const QStringList &keywords) :
            m_KeywordsList(keywords),
            m_Url(url),
            m_IsSelected(false)
        { }

    public:
        const QString &getUrl() const { return m_Url; }
        const QStringList &getKeywords() const { return m_KeywordsList; }
        bool getIsSelected() const { return m_IsSelected; }

    public:
        void setIsSelected(bool isSelected) { m_IsSelected = isSelected; }

    private:
        QStringList m_KeywordsList;
        QString m_Url;
        bool m_IsSelected;
    };
}

#endif // SUGGESTIONARTWORK_H
