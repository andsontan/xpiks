#include "findandreplacemodel.h"
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
#include <QAbstractListModel>


namespace Models {
    FindAndReplaceModel::FindAndReplaceModel(QMLExtensions::ColorsModel *colorsModel, QObject *parent):
        QAbstractListModel(parent),
        Common::BaseEntity(),
        m_ColorsModel(colorsModel)
    {}
    void FindAndReplaceModel::initArtworksList() {
        m_ArtworksList.clear();

        Models::FilteredArtItemsProxyModel *filteredItemsModel = m_CommandManager->getFilteredArtItemsModel();

        m_ArtworksList = std::move(filteredItemsModel->getSearchablePreviewOriginalItems(m_ReplaceFrom, m_Flags));
        int initialFlag = 0;
        Common::ApplyFlag(initialFlag, getCaseSensitive(), Common::SearchFlagCaseSensitive);
        int size = (int)m_ArtworksList.size();
        for (int i = 0; i < size; i++) {
            Models::PreviewMetadataElement &preview = m_ArtworksList[i];
            Models::ArtworkMetadata *metadata = preview.getOrigin();
            int flags = initialFlag;
            bool value = false;
            if (FindAndReplaceModel::getSearchInTitle()) {
                flags = initialFlag;
                Common::ApplyFlag(flags, true, Common::SearchFlagSearchTitle);
                value = Helpers::hasSearchMatch(m_ReplaceFrom, metadata, flags);
                preview.setShowTitle(value);
            }

            if (FindAndReplaceModel::getSearchInDescription()) {
                flags = initialFlag;
                Common::ApplyFlag(flags, true, Common::SearchFlagSearchDescription);
                value = Helpers::hasSearchMatch(m_ReplaceFrom, metadata, flags);
                preview.setShowDescription(value);
            }

            if (FindAndReplaceModel::getSearchInKeywords()) {
                flags = initialFlag;
                Common::ApplyFlag(flags, true, Common::SearchFlagSearchKeywords);
                value = Helpers::hasSearchMatch(m_ReplaceFrom, metadata, flags);
                preview.setShowKeywords(value);
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

        Models::PreviewMetadataElement const &item = m_ArtworksList.at(indexRow);

        switch (role) {
            case PathRole:
                return item.getOrigin()->getFilepath();
            default:
                return QVariant();
        }
    }

    QHash<int, QByteArray> FindAndReplaceModel::roleNames() const {
        QHash<int, QByteArray> roles;
        roles[PathRole] = "path";
        return roles;
    }

#ifndef CORE_TESTS
    void FindAndReplaceModel::initHighlighting(QQuickTextDocument *document) {
        Helpers::MetadataHighlighter *highlighter = new Helpers::MetadataHighlighter(m_ReplaceFrom, FindAndReplaceModel::getCaseSensitive(),
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
        if (!(item.isTitleShowable())) {
            return text;
        }

        Models::ArtworkMetadata *metadata = item.getOrigin();
        text = filterText(metadata->getTitle());
        return text;
    }

    QString FindAndReplaceModel::getSearchDescription(int index) {
        QString text;

        if (index < 0 || index >= (int)m_ArtworksList.size()) {
            return text;
        }

        Models::PreviewMetadataElement const &item = m_ArtworksList.at(index);
        if (!(item.isDescriptionShowable())) {
            return text;
        }

        Models::ArtworkMetadata *metadata = item.getOrigin();
        text = filterText(metadata->getDescription());
        return text;
    }

    QString FindAndReplaceModel::getSearchKeywords(int index) {
        QString text;

        if (index < 0 || index >= (int)m_ArtworksList.size()) {
            return text;
        }

        Models::PreviewMetadataElement const &item = m_ArtworksList.at(index);
        if (!(item.isKeywordsShowable())) {
            return text;
        }

        Models::ArtworkMetadata *metadata = item.getOrigin();
        QStringList list = metadata->getKeywords();
        Qt::CaseSensitivity caseSensitivity = FindAndReplaceModel::getCaseSensitive() ? Qt::CaseSensitive : Qt::CaseInsensitive;
        QStringList listNew;
        for (QString el : list) {
            if (el.contains(m_ReplaceFrom, caseSensitivity)) {
                listNew.append(el);
            }
        }

        text.append(listNew.join(','));

        return text;
    }

    void FindAndReplaceModel::replace() {
        std::shared_ptr<Commands::FindAndReplaceCommand> replaceCommand(new Commands::FindAndReplaceCommand(m_ArtworksList, m_ReplaceFrom,
                                                                                                            m_ReplaceTo,
                                                                                                            m_Flags));

        m_CommandManager->processCommand(replaceCommand);
    }

    QString FindAndReplaceModel::filterText(const QString &text) {
        int size = m_ReplaceFrom.size();

        if (text.size() <= 2*PREVIEWOFFSET) {
            return text;
        }

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

        if (hits.size() == 0) {
            return result;
        }

        return getSubstrings(text, hits, size);
    }

    QString FindAndReplaceModel::getSubstrings(const QString &text, const std::vector<int> &hits, int size) {
        QString result;
        QVector<QPair<int, int> > positions;
        Q_ASSERT(!hits.empty());
        int start = (hits[0] > PREVIEWOFFSET) ? (hits[0] - PREVIEWOFFSET) : 0;
        int end = hits[0] + PREVIEWOFFSET;

        positions.push_back(qMakePair(start, 2*PREVIEWOFFSET));
        int i = 0;
        for (int el : hits) {
            if ((el - PREVIEWOFFSET) > end) {
                start = el - PREVIEWOFFSET;
                end = el + PREVIEWOFFSET;
                positions.push_back(qMakePair(start, 2*PREVIEWOFFSET));
                i++;
            } else {
                int delta = (el + size) - end;
                if (delta > 0) {
                    positions[i].second += delta;
                    end += delta;
                }
            }
        }

        for (auto &element: positions) {
            result.append(text.mid(element.first, element.second));
        }

        return result;
    }
}
