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

#ifndef SPELLCHECKSUGGESTIONMODEL_H
#define SPELLCHECKSUGGESTIONMODEL_H

#include <QAbstractListModel>
#include <QList>
#include "../Common/baseentity.h"

namespace Models {
    class ArtworkMetadata;
}

namespace SpellCheck {
    class SpellSuggestionsItem;
    class SpellCheckerService;
    class ISpellCheckable;

    class SpellCheckSuggestionModel : public QAbstractListModel, public Common::BaseEntity {
        Q_OBJECT
    public:
        SpellCheckSuggestionModel();
        virtual ~SpellCheckSuggestionModel();

    public:
        enum KeywordSpellSuggestionsRoles {
            WordRole = Qt::UserRole + 1,
            ReplacementIndexRole,
            IsSelectedRole,
            EditIsSelectedRole
        };

    public:
        Q_INVOKABLE QObject *getSuggestionItself(int index) const;
        Q_INVOKABLE void clearModel();
        Q_INVOKABLE void submitCorrections() const;
        Q_INVOKABLE void selectAllSuggestions() { setAllSelected(true); }
        Q_INVOKABLE void unselectAllSuggestions() { setAllSelected(false); }

    signals:
        void selectAllChanged();

    public:
        void setupModel(SpellCheck::SpellCheckerService *service, SpellCheck::ISpellCheckable *item);

    private:
        QVector<SpellSuggestionsItem *> setupSuggestions(const QVector<SpellSuggestionsItem *> &items);
        void setAllSelected(bool selected);

    public:
        virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
        virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
        virtual bool setData(const QModelIndex &index, const QVariant &value, int role);

    protected:
        virtual QHash<int, QByteArray> roleNames() const;

    private:
        QVector<SpellSuggestionsItem*> m_SuggestionsList;
        ISpellCheckable *m_CurrentItem;
    };
}

#endif // SPELLCHECKSUGGESTIONMODEL_H
