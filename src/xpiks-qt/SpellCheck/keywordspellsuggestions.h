/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2015 Taras Kushnir <kushnirTV@gmail.com>
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

#ifndef KEYWORDSPELLSUGGESTIONS_H
#define KEYWORDSPELLSUGGESTIONS_H

#include <QString>
#include <QStringList>
#include <QAbstractListModel>

namespace SpellCheck {
    class KeywordSpellSuggestions: public QAbstractListModel
    {
        Q_OBJECT
    public:
        KeywordSpellSuggestions(const QString &keyword, int originalIndex);

    public:
        Q_PROPERTY(QString keyword READ getKeyword WRITE setKeyword NOTIFY keywordChanged)
        Q_PROPERTY(int replacementIndex READ getReplacementIndex WRITE setReplacementIndex NOTIFY replacementIndexChanged)

    public:
        enum KeywordSpellSuggestionsRoles {
            SuggestionRole = Qt::UserRole + 1,
            IsSelectedRole
        };

    public:
        const QString &getKeyword() const { return m_Keyword; }
        int getReplacementIndex() const { return m_ReplacementIndex; }

        void setKeyword(const QString &keyword) {
            if (keyword != m_Keyword) {
                m_Keyword = keyword;
                emit keywordChanged();
            }
        }

        void setReplacementIndex(int value) {
            if (value != m_ReplacementIndex) {
                QModelIndex prev = this->index(m_ReplacementIndex);
                QModelIndex curr = this->index(value);
                m_ReplacementIndex = value;
                emit replacementIndexChanged();
                QVector<int> roles;
                roles << IsSelectedRole;
                emit dataChanged(prev, prev, roles);
                emit dataChanged(curr, curr, roles);
            }
        }

        const QString &getReplacement() const { return m_Suggestions[m_ReplacementIndex]; }
        void setSuggestions(const QStringList &suggestions);
        int getOriginalIndex() const { return m_OriginalIndex; }

    signals:
        void keywordChanged();
        void replacementIndexChanged();

    public:
        virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
        virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    protected:
        virtual QHash<int, QByteArray> roleNames() const;

    private:
        QStringList m_Suggestions;
        QString m_Keyword;
        int m_ReplacementIndex;
        int m_OriginalIndex;
    };
}

#endif // KEYWORDSPELLSUGGESTIONS_H
