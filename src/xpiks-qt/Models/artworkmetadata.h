/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014 Taras Kushnir <kushnirTV@gmail.com>
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

#ifndef IMAGEMETADATA_H
#define IMAGEMETADATA_H

#include <QAbstractListModel>
#include <QStringList>
#include <QString>
#include <QSet>
#include "../Suggestion/ikeywordssuggesteable.h"

namespace Models {
    class ArtworkMetadata : public QAbstractListModel, public Suggestion::IKeywordsSuggesteable {
        Q_OBJECT
    public:
        ArtworkMetadata(const QString &filepath) :
            QAbstractListModel(),
            m_ArtworkFilepath(filepath),
            m_IsModified(false),
            m_IsSelected(false)
        { }

        virtual ~ArtworkMetadata() { this->disconnect(); }

    public:
        enum ArtworkMetadataRoles {
            KeywordRole = Qt::UserRole + 1
        };

    public:
        bool initialize(const QString &author, const QString &title,
                        const QString &description, const QString &rawKeywords, bool overwrite = true);

    public:
        const QString &getAuthor() const { return m_ArtworkAuthor; }
        const QString &getTitle() const { return m_ArtworkTitle; }
        const QString &getDescription() const { return m_ArtworkDescription; }
        const QString &getFilepath() const { return m_ArtworkFilepath; }
        int getKeywordsCount() const { return m_KeywordsSet.count(); }
        const QStringList &getKeywords() const { return m_KeywordsList; }
        const QSet<QString> &getKeywordsSet() const { return m_KeywordsSet; }
        QString getKeywordsString() const { return m_KeywordsList.join(','); }
        bool isInDirectory(const QString &directory) const { return m_ArtworkFilepath.startsWith(directory); }
        bool isModified() const { return m_IsModified; }
        bool getIsSelected() const { return m_IsSelected; }

    public:
        void setDescription(const QString &value) {
            if (m_ArtworkDescription != value) {
                m_ArtworkDescription = value;
                setModified();
            }
        }

        void setAuthor(const QString &value) {
            if (m_ArtworkAuthor != value) {
                m_ArtworkAuthor = value;
                setModified();
            }
        }

        void setTitle(const QString &value) {
            if (m_ArtworkTitle != value) {
                m_ArtworkTitle = value;
                setModified();
            }
        }

        void setIsSelected(bool value) {
            if (m_IsSelected != value) {
                m_IsSelected = value;
                selectedChanged(value);
                fileSelectedChanged(m_ArtworkFilepath, value);
            }
        }

    public:
        bool removeKeywordAt(int index);
        bool removeLastKeyword() { return removeKeywordAt(m_KeywordsList.length() - 1); }
        bool appendKeyword(const QString &keyword);

    public:
        void setKeywords(const QStringList &keywordsList) { resetKeywords(); appendKeywords(keywordsList); }
        void appendKeywords(const QStringList &keywordsList);

        // IKeywordsSuggesteable interface
   public:
        void acceptSuggestedKeywords(const QStringList &keywords) { appendKeywords(keywords); }

    private:
        void resetKeywords();

    public:
        void addKeywords(const QString& rawKeywords);
        void setModified() { m_IsModified = true; emit modifiedChanged(m_IsModified); }
        void unsetModified() { m_IsModified = false; }
        void saveBackup();

    signals:
         void modifiedChanged(bool newValue);
         void selectedChanged(bool newValue);
         void fileSelectedChanged(const QString &filepath, bool newValue);

    public:
        int rowCount(const QModelIndex & parent = QModelIndex()) const;
        QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    protected:
        QHash<int, QByteArray> roleNames() const;

    private:
         QStringList m_KeywordsList;
         QSet<QString> m_KeywordsSet;
         QString m_ArtworkFilepath;
         QString m_ArtworkDescription;
         QString m_ArtworkTitle;
         QString m_ArtworkAuthor;
         volatile bool m_IsModified;
         volatile bool m_IsSelected;
    };
}

Q_DECLARE_METATYPE(Models::ArtworkMetadata*)

#endif // IMAGEMETADATA_H
