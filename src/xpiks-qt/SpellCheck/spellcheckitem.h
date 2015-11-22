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

#ifndef SPELLCHECKITEM_H
#define SPELLCHECKITEM_H

#include <QList>
#include <QStringList>

namespace SpellCheck {
    class ISpellCheckable;

    class SpellCheckQueryItem {
    public:
        SpellCheckQueryItem(int index, const QString &word) :
            m_Word(word),
            m_Index(index),
            m_IsCorrect(true)
        { }

        SpellCheckQueryItem(const SpellCheckQueryItem &copy) :
            m_Word(copy.m_Word),
            m_Index(copy.m_Index),
            m_IsCorrect(copy.m_IsCorrect),
            m_Suggestions(copy.m_Suggestions)
        { }

        QString m_Word;
        int m_Index;
        volatile bool m_IsCorrect;
        QStringList m_Suggestions;
    };

    class SpellCheckItemBase {
    public:
        virtual ~SpellCheckItemBase();

    public:
        const QList<SpellCheckQueryItem*> &getQueries() const { return m_QueryItems; }
        virtual void submitSpellCheckResult() const = 0;
        bool needsSuggestions() const { return m_NeedsSuggestions; }
        void requestSuggestions() { m_NeedsSuggestions = true; }

    protected:
        void appendItem(SpellCheckQueryItem *item);

    private:
        QList<SpellCheckQueryItem*> m_QueryItems;
        volatile bool m_NeedsSuggestions;
    };

    class SpellCheckSeparatorItem : public SpellCheckItemBase {
    public:
        virtual void submitSpellCheckResult() const { /*BUMP*/ }
    };

    class SpellCheckItem : public SpellCheckItemBase {
    public:
        SpellCheckItem(ISpellCheckable *spellCheckable, int keywordIndex);
        SpellCheckItem(ISpellCheckable *spellCheckable);

    public:
        virtual void submitSpellCheckResult() const;

    private:
        ISpellCheckable *m_SpellCheckable;
    };
}

#endif // SPELLCHECKITEM_H
