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

#ifndef SPELLCHECKSUGGESTIONMODEL_H
#define SPELLCHECKSUGGESTIONMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <vector>
#include <memory>
#include <utility>
#include "../Common/baseentity.h"
#include "../Common/flags.h"
#include "../Common/imetadataoperator.h"

namespace Models {
    class ArtworkMetadata;
}

namespace Common {
    class IMetadataOperator;
}

namespace SpellCheck {
    class SpellSuggestionsItem;
    class SpellCheckerService;

    typedef std::vector<std::shared_ptr<SpellSuggestionsItem> > SuggestionsVector;
    typedef std::vector<std::shared_ptr<KeywordSpellSuggestions> > KeywordsSuggestionsVector;

    class SpellCheckSuggestionModel : public QAbstractListModel, public Common::BaseEntity {
        Q_OBJECT
        Q_PROPERTY(int artworksCount READ getArtworksCount NOTIFY artworksCountChanged)
    public:
        SpellCheckSuggestionModel();
        virtual ~SpellCheckSuggestionModel();

    public:
        enum KeywordSpellSuggestions_Roles {
            WordRole = Qt::UserRole + 1,
            ReplacementIndexRole,
            ReplacementOriginRole
        };

    public:
        int getArtworksCount() const { return (int)m_ItemsPairs.size(); }

    public:
        Q_INVOKABLE QObject *getSuggestionItself(int index) const;
        Q_INVOKABLE void clearModel();
        Q_INVOKABLE void submitCorrections() const;
        Q_INVOKABLE void resetAllSuggestions();

    signals:
        void selectAllChanged();
        int artworksCountChanged();

    public:
        void setupModel(Common::IMetadataOperator *item, int index, Common::SuggestionFlags flags);
        void setupModel(std::vector<std::pair<Common::IMetadataOperator *, int> > &items, Common::SuggestionFlags flags);
#if defined(INTEGRATION_TESTS) || defined(CORE_TESTS)
        SpellSuggestionsItem *getItem(int i) const { return m_SuggestionsList.at(i).get(); }
#endif

    private:
        SuggestionsVector createSuggestionsRequests(Common::SuggestionFlags flags);
        bool processFailedReplacements(const SuggestionsVector &failedReplacements) const;
        SuggestionsVector setupSuggestions(const SuggestionsVector &items);

    public:
        virtual int rowCount(const QModelIndex & parent = QModelIndex()) const override;
        virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;

    protected:
        virtual QHash<int, QByteArray> roleNames() const override;

    private:
        void updateItems() const;

    private:
        std::vector<std::shared_ptr<SpellSuggestionsItem> > m_SuggestionsList;
        std::vector<std::pair<Common::IMetadataOperator *, int> > m_ItemsPairs;
    };
}

#endif // SPELLCHECKSUGGESTIONMODEL_H
