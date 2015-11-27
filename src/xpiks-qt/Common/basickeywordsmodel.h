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

#ifndef BASICKEYWORDSMODEL_H
#define BASICKEYWORDSMODEL_H

#include <QAbstractListModel>
#include <QStringList>
#include <QVariant>
#include <QByteArray>
#include <QHash>
#include <QList>
#include "baseentity.h"
#include "../SpellCheck/ispellcheckable.h"

namespace SpellCheck {
    class SpellCheckQueryItem;
    class KeywordSpellSuggestions;
    class SpellCheckItem;
    class SpellCheckItemInfo;
}

namespace Common {
    class BasicKeywordsModel : public QAbstractListModel, public SpellCheck::ISpellCheckable {
        Q_OBJECT
    public:
        BasicKeywordsModel(QObject *parent) :
            QAbstractListModel(parent),
            m_SpellCheckInfo(NULL)
        { }

    public:
        enum BasicKeywordsModellRoles {
            KeywordRole = Qt::UserRole + 1,
            IsCorrectRole
        };

    public:
        virtual int rowCount(const QModelIndex & parent = QModelIndex()) const { Q_UNUSED(parent); return m_KeywordsList.length(); }
        virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
        const QStringList &getKeywords() const { return m_KeywordsList; }
        void reset(const QStringList &items);
        void clear();
        void appendKeyword(const QString &keyword);
        bool removeKeyword(int index, QString &keyword);
        bool removeLastKeyword(QString &keyword) { return removeKeyword(m_KeywordsList.length() - 1, keyword); }
        const QString &getDescription() const { return m_Description; }
        const QString &getTitle() const { return m_Title; }
        bool setDescription(const QString &value);
        bool setTitle(const QString &value);

    public:
        SpellCheck::SpellCheckItemInfo *getSpellCheckInfo() const { return m_SpellCheckInfo; }
        void setSpellCheckInfo(SpellCheck::SpellCheckItemInfo *info) { m_SpellCheckInfo = info; }
        void updateDescriptionSpellErrors(const QHash<QString, bool> &results);
        void updateTitleSpellErrors(const QHash<QString, bool> &results);

    public:
        // ISPELLCHECKABLE
        virtual QString retrieveKeyword(int wordIndex);
        virtual QStringList getKeywords();
        virtual void setSpellCheckResults(const QList<SpellCheck::SpellCheckQueryItem*> &items);
        virtual void setSpellCheckResults(const QHash<QString, bool> &results);
        virtual QList<SpellCheck::KeywordSpellSuggestions*> createKeywordsSuggestionsList();
        virtual void replaceKeyword(int index, const QString &existing, const QString &replacement);
        virtual void connectSignals(SpellCheck::SpellCheckItem *item);
        virtual QStringList getDescriptionWords() const;
        virtual QStringList getTitleWords() const;

    signals:
        void spellCheckResultsReady();

    private slots:
         void spellCheckRequestReady(int index);

    private:
        void emitSpellCheckChanged(int index);

    protected:
        virtual QHash<int, QByteArray> roleNames() const;

    private:
        QStringList m_KeywordsList;
        QList<bool> m_SpellCheckResults;
        SpellCheck::SpellCheckItemInfo *m_SpellCheckInfo;
        QString m_Description;
        QString m_Title;
    };
}

Q_DECLARE_METATYPE(Common::BasicKeywordsModel*)

#endif // BASICKEYWORDSMODEL_H
