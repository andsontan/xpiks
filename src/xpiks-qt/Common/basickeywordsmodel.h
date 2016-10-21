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

#include "abstractlistmodel.h"
#include <QStringList>
#include <QVariant>
#include <QByteArray>
#include <QHash>
#include <QSet>
#include <QVector>
#include <QReadWriteLock>
#include "baseentity.h"
#include "hold.h"
#include "../SpellCheck/ispellcheckable.h"
#include "../Common/flags.h"

namespace SpellCheck {
    class SpellCheckQueryItem;
    class KeywordSpellSuggestions;
    class SpellCheckItem;
    class SpellCheckItemInfo;
}

namespace Common {
    class BasicKeywordsModel:
            public AbstractListModel,
            public SpellCheck::IKeywordsSpellCheckable
    {
    Q_OBJECT
    Q_PROPERTY(bool hasSpellErrors READ hasSpellErrors NOTIFY spellCheckErrorsChanged)

    public:
        BasicKeywordsModel(Common::Hold &hold, QObject *parent=0);

        virtual ~BasicKeywordsModel() {}

    public:
        enum BasicKeywordsModel_Roles {
            KeywordRole = Qt::UserRole + 1,
            IsCorrectRole
        };

    public:
#ifdef CORE_TESTS
        QVector<bool> &getSpellCheckResults() { return m_SpellCheckResults; }
        const QString &getKeywordAt(int index) const { return m_KeywordsList.at(index); }
#endif
        virtual void removeItemsAtIndices(const QVector<QPair<int, int> > &ranges);

    protected:
        // UNSAFE
        virtual void removeInnerItem(int row);

    public:
        virtual int rowCount(const QModelIndex &parent=QModelIndex()) const;
        virtual QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;

    public:
        int getKeywordsCount();
        QSet<QString> getKeywordsSet();
        QString getKeywordsString();

    public:
        bool appendKeyword(const QString &keyword);
        bool takeKeywordAt(int index, QString &removedKeyword);
        bool takeLastKeyword(QString &removedKeyword);
        void setKeywords(const QStringList &keywordsList);
        int appendKeywords(const QStringList &keywordsList);
        bool editKeyword(int index, const QString &replacement);
        bool clearKeywords();
        bool areKeywordsEmpty();
        virtual bool replace(const QString &replaceWhat, const QString &replaceTo, Common::SearchFlags flags);
        bool removeKeywords(const QSet<QString> &keywords, bool caseSensitive);

    private:
        bool appendKeywordUnsafe(const QString &keyword);
        void takeKeywordAtUnsafe(int index, QString &removedKeyword, bool &wasCorrect);
        void setKeywordsUnsafe(const QStringList &keywordsList);
        int appendKeywordsUnsafe(const QStringList &keywordsList);
        bool canEditKeywordUnsafe(int index, const QString &replacement) const;
        bool editKeywordUnsafe(int index, const QString &replacement);
        bool replaceKeywordUnsafe(int index, const QString &existing, const QString &replacement);
        bool clearKeywordsUnsafe();
        bool containsKeywordUnsafe(const QString &searchTerm, Common::SearchFlags searchFlags=Common::SearchFlags::Keywords);
        bool hasKeywordsSpellErrorUnsafe() const;
        bool removeKeywordsUnsafe(const QSet<QString> &keywordsToRemove, bool caseSensitive);

        void lockKeywordsRead() { m_KeywordsLock.lockForRead(); }
        void unlockKeywords() { m_KeywordsLock.unlock(); }

    public:
        // ISPELLCHECKABLE
        virtual QString retrieveKeyword(int wordIndex);
        virtual QStringList getKeywords();
        virtual void setKeywordsSpellCheckResults(const std::vector<std::shared_ptr<SpellCheck::SpellCheckQueryItem> > &items);
        virtual std::vector<std::shared_ptr<SpellCheck::SpellSuggestionsItem> > createKeywordsSuggestionsList();
        virtual Common::KeywordReplaceResult fixKeywordSpelling(int index, const QString &existing, const QString &replacement);
        virtual bool processFailedKeywordReplacements(const std::vector<std::shared_ptr<SpellCheck::KeywordSpellSuggestions> > &candidatesForRemoval);
        virtual void connectSignals(SpellCheck::SpellCheckItem *item);
        virtual void afterReplaceCallback();

    private:
        void removeKeywordsAtIndicesUnsafe(const QVector<int> &indices);
        bool replaceInKeywordsUnsafe(const QString &replaceWhat, const QString &replaceTo,
                                     Common::SearchFlags flags);

    public:
        bool containsKeyword(const QString &searchTerm, Common::SearchFlags searchFlags=Common::SearchFlags::ExactKeywords);
        virtual bool isEmpty();
        bool hasKeywordsSpellError();

        virtual bool hasSpellErrors();
        void setSpellStatuses(BasicKeywordsModel *keywordsModel);

    public:
        void notifySpellCheckResults(SpellCheckFlags flags);
        void notifyAboutToBeRemoved() { emit aboutToBeRemoved(); }

    public:
        void acquire() { m_Hold.acquire(); }
        bool release() { return m_Hold.release(); }

    private:
        const QVector<bool> &getSpellStatusesUnsafe() const { return m_SpellCheckResults; }
        void resetSpellCheckResultsUnsafe();
        bool canBeAddedUnsafe(const QString &keyword) const;

    public:
        Q_INVOKABLE bool hasKeyword(const QString &keyword);
        Q_INVOKABLE bool canEditKeyword(int index, const QString &replacement);

    signals:
        void spellCheckResultsReady();
        void spellCheckErrorsChanged();
        void completionsAvailable();
        void aboutToBeRemoved();
        void afterSpellingErrorsFixed();

    protected slots:
        void spellCheckRequestReady(Common::SpellCheckFlags flags, int index);

    private:
        void setSpellCheckResultsUnsafe(const std::vector<std::shared_ptr<SpellCheck::SpellCheckQueryItem> > &items);
        bool isReplacedADuplicateUnsafe(int index, const QString &existingPrev,
                                        const QString &replacement) const;
        void emitSpellCheckChanged(int index=-1);

    protected:
        virtual QHash<int, QByteArray> roleNames() const;

    private:
        Common::Hold &m_Hold;
        QStringList m_KeywordsList;
        QSet<QString> m_KeywordsSet;
        QReadWriteLock m_KeywordsLock;
        QVector<bool> m_SpellCheckResults;
    };
}

Q_DECLARE_METATYPE(Common::BasicKeywordsModel *)

#endif // BASICKEYWORDSMODEL_H
