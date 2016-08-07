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

#include "findandreplacemodel.h"
#include <QAbstractListModel>
#include "../Models/artworkmetadata.h"
#include "../Models/artitemsmodel.h"
#include "../Models/settingsmodel.h"
#include "../Commands/commandmanager.h"
#include "../Helpers/filterhelpers.h"
#include "../Models/filteredartitemsproxymodel.h"
#include "../Models/previewmetadataelement.h"
#include "../Helpers/metadatahighlighter.h"
#include "../Commands/findandreplacecommand.h"
#include "../Common/defines.h"
#include "../Helpers/stringhelper.h"

QString searchFlagsToString(int flags) {
    QStringList items;
    items.reserve(10);

    if (Common::HasFlag(flags, Common::SearchFlagCaseSensitive)) {
        items.append("SearchFlagCaseSensitive");
    }

    if (Common::HasFlag(flags, Common::SearchFlagSearchDescription)) {
        items.append("SearchFlagSearchDescription");
    }

    if (Common::HasFlag(flags, Common::SearchFlagSearchTitle)) {
        items.append("SearchFlagSearchTitle");
    }

    if (Common::HasFlag(flags, Common::SearchFlagSearchKeywords)) {
        items.append("SearchFlagSearchKeywords");
    }

    if (Common::HasFlag(flags, Common::SearchFlagExactMatch)) {
        items.append("SearchFlagExactMatch");
    }

    if (Common::HasFlag(flags, Common::SearchFlagAllSearchTerms)) {
        items.append("SearchFlagAllSearchTerms");
    }

    return items.join(" | ");
}

namespace Models {
    FindAndReplaceModel::FindAndReplaceModel(QMLExtensions::ColorsModel *colorsModel, QObject *parent):
        QAbstractListModel(parent),
        Common::BaseEntity(),
        m_ColorsModel(colorsModel),
        m_Flags(0)
    {
        Q_ASSERT(colorsModel != nullptr);
        initDefaultFlags();
    }

    void FindAndReplaceModel::initArtworksList() {
        LOG_INFO << "Flags:" << searchFlagsToString(m_Flags);
        LOG_INFO << "ReplaceFrom:" << m_ReplaceFrom;

        Models::FilteredArtItemsProxyModel *filteredItemsModel = m_CommandManager->getFilteredArtItemsModel();
        m_ArtworksList = std::move(filteredItemsModel->getSearchablePreviewOriginalItems(m_ReplaceFrom, m_Flags));

        LOG_INFO << "Found" << m_ArtworksList.size() << "item(s)";

        for (auto &preview: m_ArtworksList) {
            Models::ArtworkMetadata *metadata = preview.getOrigin();
            bool hasMatch = false;
            int flags = 0;

            if (getSearchInTitle()) {
                flags = m_Flags;
                Common::UnsetFlag(flags, Common::SearchFlagSearchDescription);
                Common::UnsetFlag(flags, Common::SearchFlagSearchKeywords);

                hasMatch = Helpers::hasSearchMatch(m_ReplaceFrom, metadata, flags);
                preview.setHasTitleMatch(hasMatch);
            }

            if (getSearchInDescription()) {
                flags = m_Flags;
                Common::UnsetFlag(flags, Common::SearchFlagSearchTitle);
                Common::UnsetFlag(flags, Common::SearchFlagSearchKeywords);

                hasMatch = Helpers::hasSearchMatch(m_ReplaceFrom, metadata, flags);
                preview.setHasDescriptionMatch(hasMatch);
            }

            if (getSearchInKeywords()) {
                flags = m_Flags;
                Common::UnsetFlag(flags, Common::SearchFlagSearchDescription);
                Common::UnsetFlag(flags, Common::SearchFlagSearchTitle);

                hasMatch = Helpers::hasSearchMatch(m_ReplaceFrom, metadata, flags);
                preview.setHasKeywordsMatch(hasMatch);
            }
        }
    }

    int FindAndReplaceModel::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return (int)m_ArtworksList.size();
    }

    QVariant FindAndReplaceModel::data(const QModelIndex &index, int role) const {
        int indexRow = index.row();
        if (indexRow < 0 || indexRow >= (int)m_ArtworksList.size()) {
            return QVariant();
        }

        const Models::PreviewMetadataElement &item = m_ArtworksList.at(indexRow);

        switch (role) {
        case PathRole:
            return item.getOrigin()->getFilepath();
        case IsSelectedRole:
            return item.isSelected();
        case HasTitleMatchRole:
            return item.hasTitleMatch();
        case HasDescriptionMatchRole:
            return item.hasDescriptionMatch();
        case HasKeywordsMatchRole:
            return item.hasKeywordsMatch();
        default:
            return QVariant();
        }
    }

    Qt::ItemFlags FindAndReplaceModel::flags(const QModelIndex &index) const {
        int row = index.row();
        if (row < 0 || row >= (int)m_ArtworksList.size()) {
            return Qt::ItemIsEnabled;
        }

        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
    }

    bool FindAndReplaceModel::setData(const QModelIndex &index, const QVariant &value, int role) {
        int indexRow = index.row();
        if (indexRow < 0 || indexRow >= (int)m_ArtworksList.size()) {
            return false;
        }

        int roleToUpdate = 0;

        switch (role) {
            case EditIsSelectedRole:
                m_ArtworksList.at(indexRow).setSelected(value.toBool());
                roleToUpdate = IsSelectedRole;
                break;
            default:
                return false;
        }

        emit dataChanged(index, index, QVector<int>() << roleToUpdate);
        return true;
    }

    QHash<int, QByteArray> FindAndReplaceModel::roleNames() const {
        QHash<int, QByteArray> roles;
        roles[PathRole] = "path";
        roles[IsSelectedRole] = "isselected";
        roles[EditIsSelectedRole] = "editisselected";
        roles[HasTitleMatchRole] = "hastitle";
        roles[HasDescriptionMatchRole] = "hasdescription";
        roles[HasKeywordsMatchRole] = "haskeywords";
        return roles;
    }

#if !defined(CORE_TESTS) && !defined(INTEGRATION_TESTS)
    void FindAndReplaceModel::initHighlighting(QQuickTextDocument *document) {
        Helpers::MetadataHighlighter *highlighter = new Helpers::MetadataHighlighter(m_ReplaceFrom,
                                                                                     this,
                                                                                     m_ColorsModel,
                                                                                     document->textDocument());

        Q_UNUSED(highlighter);
    }
#endif

    QString FindAndReplaceModel::getSearchTitle(int index) {
        QString text;

        if (index < 0 || index >= (int)m_ArtworksList.size()) {
            return text;
        }

        Models::PreviewMetadataElement const &item = m_ArtworksList.at(index);
        Models::ArtworkMetadata *metadata = item.getOrigin();

        if (item.hasTitleMatch()) {
            text = filterText(metadata->getTitle());
        } else {
            auto title = metadata->getTitle();
            if (title.size() > PREVIEWOFFSET*2) {
                text = title.left(PREVIEWOFFSET*2) + " ...";
            } else {
                text = title;
            }
        }

        return text;
    }

    QString FindAndReplaceModel::getSearchDescription(int index) {
        QString text;

        if (index < 0 || index >= (int)m_ArtworksList.size()) {
            return text;
        }

        Models::PreviewMetadataElement const &item = m_ArtworksList.at(index);
        Models::ArtworkMetadata *metadata = item.getOrigin();

        if (item.hasDescriptionMatch()) {
            text = filterText(metadata->getDescription());
        } else {
            auto description = metadata->getDescription();
            if (description.size() > PREVIEWOFFSET*2) {
                text = description.left(PREVIEWOFFSET*2) + " ...";
            } else {
                text = description;
            }
        }

        return text;
    }

    QString FindAndReplaceModel::getSearchKeywords(int index) {
        QString text;

        if (index < 0 || index >= (int)m_ArtworksList.size()) {
            return text;
        }

        Models::PreviewMetadataElement const &item = m_ArtworksList.at(index);
        Models::ArtworkMetadata *metadata = item.getOrigin();
        QStringList list = metadata->getKeywords();

        if (item.hasKeywordsMatch()) {
            Qt::CaseSensitivity caseSensitivity = FindAndReplaceModel::getCaseSensitive() ? Qt::CaseSensitive : Qt::CaseInsensitive;
            const bool wholeWords = getSearchWholeWords();

            QStringList listNew;
            for (auto &el: list) {
                if (!wholeWords) {
                    if (el.contains(m_ReplaceFrom, caseSensitivity)) {
                        listNew.append(el);
                    }
                } else if (QString::compare(m_ReplaceFrom, el, caseSensitivity) == 0) {
                    listNew.append(el);
                }
            }

            text.append(listNew.join(", "));
        } else {
            if (list.length() > PREVIEWKEYWORDSCOUNT) {
                auto part = list.mid(0, PREVIEWKEYWORDSCOUNT);
                text = part.join(", ") + " ...";
            } else {
                text = list.join(", ");
            }
        }

        return text;
    }

    void FindAndReplaceModel::replace() {
        LOG_INFO << "Flags:" << searchFlagsToString(m_Flags);

        std::shared_ptr<Commands::FindAndReplaceCommand> replaceCommand(new Commands::FindAndReplaceCommand(m_ArtworksList, m_ReplaceFrom,
                                                                                                            m_ReplaceTo,
                                                                                                            m_Flags));

        m_CommandManager->processCommand(replaceCommand);

        emit replaceSucceeded();
    }

    bool FindAndReplaceModel::anySearchDestination() const {
        return
                getSearchInDescription() ||
                getSearchInTitle() ||
                getSearchInKeywords();
    }

    void FindAndReplaceModel::resetModel() {
        LOG_DEBUG << "#";

        initDefaultFlags();
        m_ArtworksList.clear();
        m_ReplaceFrom.clear();
        m_ReplaceTo.clear();
    }

    QString FindAndReplaceModel::filterText(const QString &text) {
        int size = m_ReplaceFrom.size();

#ifndef QT_DEBUG
        if (text.size() <= 2*PREVIEWOFFSET) {
            return text;
        }
#endif

        QString result;
        Qt::CaseSensitivity caseSensitivity = FindAndReplaceModel::getCaseSensitive() ? Qt::CaseSensitive : Qt::CaseInsensitive;
        int pos = 0;
        std::vector<int> hits;

        while (pos != -1) {
            pos = text.indexOf(m_ReplaceFrom, pos, caseSensitivity);
            if (pos >= 0) {
                hits.push_back(pos);
                pos += size;
            }
        }

        if (!hits.empty()) {
            result = Helpers::getUnitedHitsString(text, hits, PREVIEWOFFSET);
        }

        return result;
    }

    void FindAndReplaceModel::setAllSelected(bool isSelected) {
        LOG_INFO << "isSelected:" << isSelected;

        for (auto &item: m_ArtworksList) {
            item.setSelected(isSelected);
        }

        auto first = this->index(0);
        auto last = this->index(this->rowCount() - 1);
        emit dataChanged(first, last, QVector<int>() << IsSelectedRole);

        emit allSelectedChanged();
    }

    void FindAndReplaceModel::initDefaultFlags() {
        m_Flags = 0;
        Common::SetFlag(m_Flags, Common::SearchFlagCaseSensitive);
        Common::SetFlag(m_Flags, Common::SearchFlagSearchTitle);
        Common::SetFlag(m_Flags, Common::SearchFlagSearchDescription);
        Common::SetFlag(m_Flags, Common::SearchFlagSearchKeywords);
        // Common::SetFlag(m_Flags, Common::SearchFlagExactMatch);
    }
}
