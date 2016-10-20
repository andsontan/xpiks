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

#ifndef BASICMETADATAMODEL_H
#define BASICMETADATAMODEL_H

#include "basickeywordsmodel.h"
#include <QStringList>
#include <QHash>
#include <QReadWriteLock>
#include "../SpellCheck/ispellcheckable.h"
#include "../Common/flags.h"

namespace SpellCheck {
    class SpellCheckQueryItem;
    class KeywordSpellSuggestions;
    class SpellCheckItem;
    class SpellCheckItemInfo;
}

namespace Common {
    class BasicMetadataModel :
            public BasicKeywordsModel,
            public SpellCheck::ISpellCheckable
    {
        Q_OBJECT
    public:
        BasicMetadataModel(Common::Hold &hold, QObject *parent=0);

    public:
        void setSpellCheckInfo(SpellCheck::SpellCheckItemInfo *info) { m_SpellCheckInfo = info; }
        SpellCheck::SpellCheckItemInfo *getSpellCheckInfo() const { return m_SpellCheckInfo; }
        QString getDescription();
        QString getTitle();

    public:
#ifdef CORE_TESTS
        void initialize(const QString &title, const QString &description, const QString &rawKeywords);
#endif

    public:
        // ISPELLCHECKABLE
        virtual void setSpellCheckResults(const QHash<QString, bool> &results, SpellCheckFlags flags);
        virtual std::vector<std::shared_ptr<SpellCheck::SpellSuggestionsItem> > createDescriptionSuggestionsList();
        virtual std::vector<std::shared_ptr<SpellCheck::SpellSuggestionsItem> > createTitleSuggestionsList();
        virtual void fixDescriptionSpelling(const QString &word, const QString &replacement);
        virtual void fixTitleSpelling(const QString &word, const QString &replacement);
        virtual QStringList getDescriptionWords();
        virtual QStringList getTitleWords();

    private:
        bool replaceInDescription(const QString &replaceWhat, const QString &replaceTo,
                                  Common::SearchFlags flags);
        bool replaceInTitle(const QString &replaceWhat, const QString &replaceTo,
                            SearchFlags flags);

    public:
        virtual bool replace(const QString &replaceWhat, const QString &replaceTo, Common::SearchFlags flags);
        bool hasDescriptionSpellError();
        bool hasTitleSpellError();
        bool hasDescriptionWordSpellError(const QString &word);
        bool hasTitleWordSpellError(const QString &word);
        virtual bool hasSpellErrors();

    public:
        bool setDescription(const QString &value);
        bool setTitle(const QString &value);
        virtual bool isEmpty();
        bool isTitleEmpty();
        bool isDescriptionEmpty();
        void clearModel();

    public:
        void notifyDescriptionSpellCheck();
        void notifyTitleSpellCheck();

    private:
        void updateDescriptionSpellErrors(const QHash<QString, bool> &results);
        void updateTitleSpellErrors(const QHash<QString, bool> &results);

    private:
        QReadWriteLock m_DescriptionLock;
        QReadWriteLock m_TitleLock;
        SpellCheck::SpellCheckItemInfo *m_SpellCheckInfo;
        QString m_Description;
        QString m_Title;
    };
}

#endif // BASICMETADATAMODEL_H
