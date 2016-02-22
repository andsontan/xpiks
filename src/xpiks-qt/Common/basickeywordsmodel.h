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

#ifndef BASICKEYWORDSMODEL_H
#define BASICKEYWORDSMODEL_H

#include <QAbstractListModel>
#include <QStringList>
#include <QVariant>
#include <QByteArray>
#include <QHash>
#include <QSet>
#include <QVector>
#include "baseentity.h"
#include "../SpellCheck/ispellcheckable.h"
#include "../Warnings/iwarningscheckable.h"

namespace SpellCheck {
    class SpellCheckQueryItem;
    class KeywordSpellSuggestions;
    class SpellCheckItem;
    class SpellCheckItemInfo;
}

namespace Common {
    class BasicKeywordsModel :
            public QAbstractListModel,
            public SpellCheck::ISpellCheckable,
            public Warnings::IWarningsCheckable
    {
        Q_OBJECT
        Q_PROPERTY(bool hasSpellErrors READ hasSpellErrors NOTIFY spellCheckErrorsChanged)
    public:
        BasicKeywordsModel(QObject *parent=0);
        virtual ~BasicKeywordsModel() {}

    public:
        enum BasicKeywordsModellRoles {
            KeywordRole = Qt::UserRole + 1,
            IsCorrectRole
        };

    public:
        virtual int rowCount(const QModelIndex & parent = QModelIndex()) const { Q_UNUSED(parent); return m_KeywordsList.length(); }
        virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    public:
        virtual const QString &getDescription() const { return m_Description; }
        virtual const QString &getTitle() const { return m_Title; }
        int getKeywordsCount() const { return m_KeywordsSet.count(); }
        virtual const QSet<QString> &getKeywordsSet() const { return m_KeywordsSet; }
        const QVector<bool> &getSpellStatuses() const { return m_SpellCheckResults; }
        QString getKeywordsString() { return m_KeywordsList.join(", "); }
        int getWarningsFlags() const { return m_WarningsFlags; }

    public:
        virtual bool appendKeyword(const QString &keyword);
        virtual bool takeKeywordAt(int index, QString &removedKeyword);
        bool takeLastKeyword(QString &removedKeyword) { return takeKeywordAt(m_KeywordsList.length() - 1, removedKeyword); }
        virtual void setKeywords(const QStringList &keywordsList);
        virtual int appendKeywords(const QStringList &keywordsList);
        virtual bool editKeyword(int index, const QString &replacement);

        virtual bool setDescription(const QString &value);
        virtual bool setTitle(const QString &value);
        virtual bool isEmpty() const;
        bool isTitleEmpty() const;
        bool isDescriptionEmpty() const;
        bool areKeywordsEmpty() const { return m_KeywordsList.isEmpty(); }
        bool containsKeyword(const QString &searchTerm, bool exactMatch=false);

        virtual bool hasKeywordsSpellError() const;
        virtual bool hasDescriptionSpellError() const;
        virtual bool hasTitleSpellError() const;

        virtual QSize getImageSize() const { throw 0; }
        virtual qint64 getFileSize() const { throw 0; }
        virtual const QString &getFilepath() const { throw 0; }

        bool hasSpellErrors() const;

        void setSpellStatuses(const QVector<bool> &statuses);
        virtual void setWarningsFlags(int flags) { m_WarningsFlags = flags; }
        virtual void addWarningsFlags(int flags) { m_WarningsFlags |= flags; }
        virtual void dropWarningsFlags(int flagsToDrop) { m_WarningsFlags &= (~flagsToDrop); }

        virtual void clearModel();
        virtual bool clearKeywords();
        void resetKeywords(const QStringList &keywords);

    public:
        // ISAFEPOINTER
        virtual void acquire() { m_RefCount.fetchAndAddOrdered(1); }
        virtual bool release() { return m_RefCount.fetchAndSubOrdered(1) == 1; }

    public:
        SpellCheck::SpellCheckItemInfo *getSpellCheckInfo() const { return m_SpellCheckInfo; }
        void setSpellCheckInfo(SpellCheck::SpellCheckItemInfo *info) { m_SpellCheckInfo = info; }
        void notifySpellCheckResults(int flags);

    private:
        void updateDescriptionSpellErrors(const QHash<QString, bool> &results);
        void updateTitleSpellErrors(const QHash<QString, bool> &results);
        void resetSpellCheckResults();
        bool canBeAdded(const QString &keyword) const;

    public:
        // ISPELLCHECKABLE
        virtual QString retrieveKeyword(int wordIndex);
        virtual const QStringList &getKeywords() const { return m_KeywordsList; }
        virtual void setSpellCheckResults(const QVector<SpellCheck::SpellCheckQueryItem*> &items, bool onlyOneKeyword);
        virtual void setSpellCheckResults(const QHash<QString, bool> &results, int flags);
        virtual QVector<SpellCheck::SpellSuggestionsItem *> createKeywordsSuggestionsList();
        virtual QVector<SpellCheck::SpellSuggestionsItem*> createDescriptionSuggestionsList();
        virtual QVector<SpellCheck::SpellSuggestionsItem*> createTitleSuggestionsList();
        virtual void replaceKeyword(int index, const QString &existing, const QString &replacement);
        virtual void replaceWordInDescription(const QString &word, const QString &replacement);
        virtual void replaceWordInTitle(const QString &word, const QString &replacement);
        virtual void afterReplaceCallback();
        virtual void connectSignals(SpellCheck::SpellCheckItem *item);
        virtual QStringList getDescriptionWords() const;
        virtual QStringList getTitleWords() const;

    signals:
        void spellCheckResultsReady();
        void spellCheckErrorsChanged();

    protected slots:
         void spellCheckRequestReady(int flags, int index);

    private:
        void emitSpellCheckChanged(int index=-1);

    protected:
        virtual QHash<int, QByteArray> roleNames() const;
        void resetKeywords();
        void addKeywords(const QStringList &rawKeywords);
        void freeSpellCheckInfo();

    private:
        QStringList m_KeywordsList;
        QSet<QString> m_KeywordsSet;
        QVector<bool> m_SpellCheckResults;
        SpellCheck::SpellCheckItemInfo *m_SpellCheckInfo;
        QString m_Description;
        QString m_Title;
        // used for background workers
        QAtomicInt m_RefCount;
        volatile int m_WarningsFlags;
    };
}

Q_DECLARE_METATYPE(Common::BasicKeywordsModel*)

#endif // BASICKEYWORDSMODEL_H
