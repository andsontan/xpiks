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


#ifndef SUGGESTIONARTWORK_H
#define SUGGESTIONARTWORK_H

#include <QVector>
#include <QString>
#include <QSet>

namespace Suggestion {
    class SuggestionArtwork
    {
    public:
        SuggestionArtwork(const QString &url, const QStringList &keywords, bool isLocal = true) :
            m_KeywordsSet(keywords.toSet()),
            m_IsSelected(false)
        {
            if (isLocal) {
                m_Url = QLatin1String("image://global/") + url;
            } else {
                m_Url = url;
            }
        }

    public:
        const QString &getUrl() const { return m_Url; }
        const QSet<QString> &getKeywordsSet() const { return m_KeywordsSet; }
        bool getIsSelected() const { return m_IsSelected; }

    public:
        void setIsSelected(bool isSelected) { m_IsSelected = isSelected; }

    private:
        QSet<QString> m_KeywordsSet;
        QString m_Url;
        bool m_IsSelected;
        //bool m_IsLocal;
    };
}

#endif // SUGGESTIONARTWORK_H
