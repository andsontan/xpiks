/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
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

#include <QObject>
#include <QQmlEngine>
#include "../Models/artworkproxybase.h"
#include "../Common/basicmetadatamodel.h"
#include "../SpellCheck/spellcheckiteminfo.h"

#ifndef SPELLCHECKUSERDICT_H
#define SPELLCHECKUSERDICT_H

namespace SpellCheck {
    class UserDictEditModel: public QObject, public Models::ArtworkProxyBase
    {
        Q_OBJECT
        Q_PROPERTY(int keywordsCount READ getKeywordsCount NOTIFY keywordsCountChanged)

    public:
        UserDictEditModel(QObject *parent=0);

        virtual ~UserDictEditModel() {}

    signals:
        void keywordsCountChanged();

    protected:
        virtual void signalDescriptionChanged() override { /* BUMP */ }
        virtual void signalTitleChanged() override { /* BUMP */ }
        virtual void signalKeywordsCountChanged() override { emit keywordsCountChanged(); }

    public:
        Q_INVOKABLE void initializeModel();
        Q_INVOKABLE void removeKeywordAt(int keywordIndex);
        Q_INVOKABLE void removeLastKeyword();
        Q_INVOKABLE void appendKeyword(const QString &keyword);
        Q_INVOKABLE void clearKeywords();
        Q_INVOKABLE void resetModel();
        Q_INVOKABLE QObject *getBasicModel() {
            QObject *item = getBasicMetadataModel();
            QQmlEngine::setObjectOwnership(item, QQmlEngine::CppOwnership);

            return item;
        }
        Q_INVOKABLE void saveUserDict();


    protected:
        virtual Common::BasicMetadataModel *getBasicMetadataModel() override { return &m_BasicModel; }
        virtual Common::IMetadataOperator *getMetadataOperator() override { return &m_BasicModel; }

    private:
        Common::Hold m_HoldPlaceholder;
        Common::BasicMetadataModel m_BasicModel;
        SpellCheck::SpellCheckItemInfo m_SpellCheckInfo;
    };
}
#endif // SPELLCHECKUSERDICT_H
