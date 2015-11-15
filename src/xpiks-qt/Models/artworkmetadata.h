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

#ifndef IMAGEMETADATA_H
#define IMAGEMETADATA_H

#include <QAbstractListModel>
#include <QReadWriteLock>
#include <QStringList>
#include <QFileInfo>
#include <QString>
#include <QSet>

namespace Models {
    class SettingsModel;

    class ArtworkMetadata : public QAbstractListModel {
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
            KeywordRole = Qt::UserRole + 1,
            SpellCheckOkRole
        };

    public:
        bool initialize(const QString &title,
                        const QString &description, const QString &rawKeywords, bool overwrite = true);

    public:
        const QString &getTitle() const { return m_ArtworkTitle; }
        const QString &getDescription() const { return m_ArtworkDescription; }
        const QString &getFilepath() const { return m_ArtworkFilepath; }
        virtual QString getDirectory() const { QFileInfo fi(m_ArtworkFilepath); return fi.absolutePath(); }
        int getKeywordsCount();
        QStringList getKeywords();
        const QSet<QString> &getKeywordsSet() const { return m_KeywordsSet; }
        QString getKeywordsString();
        bool isInDirectory(const QString &directory) const;
        bool isModified() const { return m_IsModified; }
        bool getIsSelected() const { return m_IsSelected; }
        bool isEmpty() const;
        void clearMetadata();
        QString retrieveKeyword(int index);
        bool containsKeyword(const QString& searchTerm);

    public:
        bool setDescription(const QString &value) {
            bool result = m_ArtworkDescription != value;
            if (result) {
                m_ArtworkDescription = value;
                setModified();
            }
            return result;
        }

        bool setTitle(const QString &value) {
            bool result = m_ArtworkTitle != value;
            if (result) {
                m_ArtworkTitle = value;
                setModified();
            }
            return result;
        }

        bool setIsSelected(bool value) {
            bool result = m_IsSelected != value;
            if (result) {
                m_IsSelected = value;
                selectedChanged(value);
                fileSelectedChanged(m_ArtworkFilepath, value);
            }
            return result;
        }

        void resetSelected() { m_IsSelected = false; }

    public:
        bool removeKeywordAt(int index);
        bool removeLastKeyword() { return removeKeywordAt(m_KeywordsList.length() - 1); }
        bool appendKeyword(const QString &keyword);

    private:
        bool appendKeywordUnsafe(const QString &keyword);

    public:
        void setKeywords(const QStringList &keywordsList);
        int appendKeywords(const QStringList &keywordsList);

    private:
        int appendKeywordsUnsafe(const QStringList &keywordsList);
        void resetKeywordsUnsafe();

    public:
        void addKeywords(const QString& rawKeywords);
        void setModified() { m_IsModified = true; emit modifiedChanged(m_IsModified); }
        void unsetModified() { m_IsModified = false; }
        void saveBackup(SettingsModel *settings);

    signals:
         void modifiedChanged(bool newValue);
         void selectedChanged(bool newValue);
         void fileSelectedChanged(const QString &filepath, bool newValue);

    public:
        virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
        virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    protected:
        virtual QHash<int, QByteArray> roleNames() const;

    private:
         QReadWriteLock m_RWLock;
         QStringList m_KeywordsList;
         QSet<QString> m_KeywordsSet;
         QString m_ArtworkFilepath;
         QString m_ArtworkDescription;
         QString m_ArtworkTitle;
         volatile bool m_IsModified;
         volatile bool m_IsSelected;
    };
}

Q_DECLARE_METATYPE(Models::ArtworkMetadata*)

#endif // IMAGEMETADATA_H
