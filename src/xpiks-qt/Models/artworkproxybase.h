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

#ifndef ARTWORKPROXYBASE_H
#define ARTWORKPROXYBASE_H

#include <QQuickTextDocument>
#include "../Common/baseentity.h"
#include "../Common/basicmetadatamodel.h"
#include "../Common/imetadataoperator.h"

namespace Models {
    class ArtworkProxyBase:
            public virtual Common::BaseEntity
    {
    public:
        virtual ~ArtworkProxyBase() {}

    public:
        QString getDescription();
        QString getTitle();
        int getKeywordsCount();
        virtual void setDescription(const QString &description);
        virtual void setTitle(const QString &title);

    protected:
        virtual bool doSetDescription(const QString &description);
        virtual bool doSetTitle(const QString &title);

        // TODO: virtual inheritance from qobject is not supported
    //signals:
        virtual void signalDescriptionChanged() = 0;
        virtual void signalTitleChanged() = 0;
        virtual void signalKeywordsCountChanged() = 0;

    protected:
        virtual Common::BasicMetadataModel *getBasicMetadataModel() = 0;
        virtual Common::IMetadataOperator *getMetadataOperator() = 0;

    protected:
        bool doEditKeyword(int index, const QString &replacement);
        bool doRemoveKeywordAt(int keywordIndex, QString &keyword);
        bool doRemoveLastKeyword(QString &keyword);
        bool doAppendKeyword(const QString &keyword);
        int doAppendKeywords(const QStringList &keywords);
        bool doClearKeywords();
        QString doGetKeywordsString();
        void doSuggestCorrections();
        void doInitDescriptionHighlighting(QQuickTextDocument *document);
        void doInitTitleHighlighting(QQuickTextDocument *document);
        void doSpellCheckDescription();
        void doSpellCheckTitle();
        void doPlainTextEdit(const QString &rawKeywords);
        bool getHasTitleWordSpellError(const QString &word);
        bool getHasDescriptionWordSpellError(const QString &word);

    private:
        void spellCheckKeywords();
    };
}

#endif // ARTWORKPROXYBASE_H
