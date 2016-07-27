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

#ifndef FINDANDREPLACEMODEL_H
#define FINDANDREPLACEMODEL_H

#include "../Common/baseentity.h"
#include <QObject>
#include <QQuickTextDocument>
#include "../Models/previewmetadataelement.h"
#include "../Common/flags.h"

namespace Models {
    class FindAndReplaceModel:
        public QAbstractListModel,
        public Common::BaseEntity
    {
        Q_OBJECT
        Q_PROPERTY(QString replaceFrom READ getReplaceFrom WRITE setReplaceFrom NOTIFY replaceFromChanged)
        Q_PROPERTY(QString replaceTo READ getReplaceTo WRITE setReplaceTo NOTIFY replaceToChanged)
        Q_PROPERTY(bool searchInTitle READ getSearchInTitle WRITE setSearchInTitle NOTIFY searchInTitleChanged)
        Q_PROPERTY(bool searchInDescription READ getSearchInDescription WRITE setSearchInDescription NOTIFY searchInDescriptionChanged)
        Q_PROPERTY(bool searchInKeywords READ getSearchInKeywords WRITE setSearchInKeywords NOTIFY searchInKeywordsChanged)
        Q_PROPERTY(bool caseSensitive READ getCaseSensitive WRITE setCaseSensitive NOTIFY caseSensitiveChanged)
        Q_PROPERTY(int count READ getCount NOTIFY countChanged)

    public:
        FindAndReplaceModel(QMLExtensions::ColorsModel *colorsModel, QObject *parent=0);

        virtual ~FindAndReplaceModel() {}

        const QString &getReplaceFrom() const { return m_ReplaceFrom; }
        void setReplaceFrom(const QString &value) {
            QString valueTrimmed = value.trimmed();

            if (valueTrimmed != m_ReplaceFrom) {
                m_ReplaceFrom = valueTrimmed;
                emit replaceFromChanged(valueTrimmed);
            }
        }

        const QString &getReplaceTo() const { return m_ReplaceTo; }
        void setReplaceTo(const QString &value) {
            QString valueTrimmed = value.trimmed();

            if (valueTrimmed != m_ReplaceTo) {
                m_ReplaceTo = valueTrimmed;
                emit replaceToChanged(valueTrimmed);
            }
        }

        bool inline getSearchInTitle() const {
            return Common::HasFlag(m_Flags, Common::SearchFlagSearchTitle);
        }

        void setSearchInTitle(bool value) {
            if (value != getSearchInTitle()) {
                Common::ApplyFlag(m_Flags, value, Common::SearchFlagSearchTitle);
                emit searchInTitleChanged(value);
            }
        }

        bool inline getSearchInDescription() const {
            return Common::HasFlag(m_Flags, Common::SearchFlagSearchDescription);
        }

        void setSearchInDescription(bool value) {
            if (value != getSearchInDescription()) {
                Common::ApplyFlag(m_Flags, value, Common::SearchFlagSearchDescription);
                emit searchInDescriptionChanged(value);
            }
        }

        bool inline getSearchInKeywords() const {
            return Common::HasFlag(m_Flags, Common::SearchFlagSearchKeywords);
        }

        void setSearchInKeywords(bool value) {
            if (value != getSearchInKeywords()) {
                Common::ApplyFlag(m_Flags, value, Common::SearchFlagSearchKeywords);
                emit searchInKeywordsChanged(value);
            }
        }

        bool inline getCaseSensitive() const {
            return Common::HasFlag(m_Flags, Common::SearchFlagCaseSensitive);
        }

        void setCaseSensitive(bool value) {
            if (value != getCaseSensitive()) {
                Common::ApplyFlag(m_Flags, value, Common::SearchFlagCaseSensitive);
                emit caseSensitiveChanged(value);
            }
        }

        int getCount() const { return (int)m_ArtworksList.size(); }

    public:
        enum FindAndReplaceModelRoles {
            PathRole = Qt::UserRole + 1,
        };

    public:
        Q_INVOKABLE void initArtworksList();

#ifndef CORE_TESTS
        Q_INVOKABLE void initHighlighting(QQuickTextDocument *document);
#endif
        Q_INVOKABLE QString getSearchTitle(int index);
        Q_INVOKABLE QString getSearchDescription(int index);
        Q_INVOKABLE QString getSearchKeywords(int index);
        Q_INVOKABLE void replace();
        Q_INVOKABLE void setReplacableState(int index, bool state);

    public:
        virtual int rowCount(const QModelIndex &parent=QModelIndex()) const;
        virtual QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;

    protected:
        virtual QHash<int, QByteArray> roleNames() const;

    signals:
        void replaceFromChanged(const QString &value);
        void replaceToChanged(const QString &value);
        void searchInTitleChanged(bool value);
        void searchInDescriptionChanged(bool value);
        void searchInKeywordsChanged(bool value);
        void caseSensitiveChanged(bool value);
        void countChanged(int value);

    private:
        QString filterText(const QString &text);

    private:
        std::vector<Models::PreviewMetadataElement> m_ArtworksList;
        QString m_ReplaceFrom;
        QString m_ReplaceTo;
        int m_Flags;
        QMLExtensions::ColorsModel *m_ColorsModel;
    };
}
#endif // FINDANDREPLACEMODEL_H
