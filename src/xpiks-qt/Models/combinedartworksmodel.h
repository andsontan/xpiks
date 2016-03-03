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

#ifndef COMBINEDARTWORKSMODEL_H
#define COMBINEDARTWORKSMODEL_H

#include <QAbstractListModel>
#include <QQmlEngine>
#include <QStringList>
#include <QString>
#include <QList>
#include <QSet>
#include <QQuickTextDocument>
#include "abstractlistmodel.h"
#include "../Common/baseentity.h"
#include "../Common/basickeywordsmodel.h"
#include "../Common/flags.h"
#include "../SpellCheck/spellcheckiteminfo.h"

namespace Models {
    class ArtItemInfo;

    class CombinedArtworksModel :
            public AbstractListModel,
            public Common::BaseEntity
    {
        Q_OBJECT
        Q_PROPERTY(QString description READ getDescription WRITE setDescription NOTIFY descriptionChanged)
        Q_PROPERTY(QString title READ getTitle WRITE setTitle NOTIFY titleChanged)
        Q_PROPERTY(int keywordsCount READ getKeywordsCount NOTIFY keywordsCountChanged)
        Q_PROPERTY(int selectedArtworksCount READ getSelectedArtworksCount NOTIFY selectedArtworksCountChanged)
        Q_PROPERTY(int artworksCount READ getArtworksCount NOTIFY artworksCountChanged)
        Q_PROPERTY(bool changeDescription READ getChangeDescription WRITE setChangeDescription NOTIFY changeDescriptionChanged)
        Q_PROPERTY(bool changeTitle READ getChangeTitle WRITE setChangeTitle NOTIFY changeTitleChanged)
        Q_PROPERTY(bool changeKeywords READ getChangeKeywords WRITE setChangeKeywords NOTIFY changeKeywordsChanged)
        Q_PROPERTY(bool appendKeywords READ getAppendKeywords WRITE setAppendKeywords NOTIFY appendKeywordsChanged)

    public:
        CombinedArtworksModel(QObject *parent = 0);
        virtual ~CombinedArtworksModel();

    public:
        void initArtworks(const QVector<ArtItemInfo *> &artworks);

    private:
        void initKeywords(const QStringList &ek) { m_CommonKeywordsModel.resetKeywords(ek); m_AreKeywordsModified = false; }
        void initDescription(const QString &description) { setDescription(description); m_IsDescriptionModified = false; }
        void initTitle(const QString &title) { setTitle(title); m_IsTitleModified = false; }

    public:
        void recombineArtworks();

    public:
        virtual void acceptSuggestedKeywords(const QStringList &keywords);

    public:
        void setKeywords(const QStringList& keywords) { m_CommonKeywordsModel.resetKeywords(keywords); }
        const QString &getDescription() const { return m_CommonKeywordsModel.getDescription(); }
        void setDescription(const QString &value) {
            if (m_CommonKeywordsModel.setDescription(value)) {
                emit descriptionChanged();
                m_IsDescriptionModified = true;
            }
        }

        const QString &getTitle() const { return m_CommonKeywordsModel.getTitle(); }
        void setTitle(const QString &value) {
            if (m_CommonKeywordsModel.setTitle(value)) {
                emit titleChanged();
                m_IsTitleModified = true;
            }
        }

        int getKeywordsCount() const { return m_CommonKeywordsModel.getKeywordsCount(); }

        bool getChangeDescription() const { return Common::HasFlag(m_EditFlags, Common::EditDesctiption); }
        void setChangeDescription(bool value);

        bool getChangeTitle() const { return Common::HasFlag(m_EditFlags, Common::EditTitle); }
        void setChangeTitle(bool value);

        bool getChangeKeywords() const { return Common::HasFlag(m_EditFlags, Common::EditKeywords); }
        void setChangeKeywords(bool value);

        bool getAppendKeywords() const { return Common::HasFlag(m_EditFlags, Common::AppendKeywords); }
        void setAppendKeywords(bool value);

    signals:
        void descriptionChanged();
        void titleChanged();
        void keywordsCountChanged();
        void selectedArtworksCountChanged();
        void artworksCountChanged();
        void changeDescriptionChanged();
        void changeKeywordsChanged();
        void changeTitleChanged();
        void appendKeywordsChanged();
        void artworksFileSystemDeleted();

    public:
        int getSelectedArtworksCount() const;
        int getArtworksCount() const { return m_ArtworksList.length(); }

#if defined(TESTS)
        const QStringList &getKeywords() const;
        bool getAreKeywordsModified() const { return m_AreKeywordsModified; }
        bool getIsDescriptionModified() const { return m_IsDescriptionModified; }
#endif

    public:
        Q_INVOKABLE void editKeyword(int index, const QString &replacement);
        Q_INVOKABLE QString removeKeywordAt(int keywordIndex);
        Q_INVOKABLE void removeLastKeyword();
        Q_INVOKABLE void appendKeyword(const QString &keyword);
        Q_INVOKABLE void pasteKeywords(const QStringList &keywords);
        Q_INVOKABLE void setArtworksSelected(int index, bool newState);
        Q_INVOKABLE void removeSelectedArtworks();
        Q_INVOKABLE void saveEdits() const;
        Q_INVOKABLE void resetModelData();
        Q_INVOKABLE void clearKeywords();
        Q_INVOKABLE QString getKeywordsString() { return m_CommonKeywordsModel.getKeywordsString(); }
        Q_INVOKABLE QObject *getKeywordsModel() {
            QObject *item = &m_CommonKeywordsModel;
            QQmlEngine::setObjectOwnership(item, QQmlEngine::CppOwnership);
            return item;
        }
        Q_INVOKABLE void suggestCorrections();
        Q_INVOKABLE void initDescriptionHighlighting(QQuickTextDocument *document);
        Q_INVOKABLE void initTitleHighlighting(QQuickTextDocument *document);
        Q_INVOKABLE void spellCheckDescription();
        Q_INVOKABLE void spellCheckTitle();
        Q_INVOKABLE void assignFromSelected();

    private:
        void processCombinedEditCommand() const;
        void enableAllFields();
        void assignFromOneArtwork();
        void assignFromManyArtworks();

    private slots:
        void spellCheckErrorsChangedHandler();
    public slots:
        void onFileDeletedHandler();

    public:
        enum CombinedArtworksModelRoles {
            PathRole = Qt::UserRole + 1,
            IsSelectedRole
        };

    public:
        virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
        virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    protected:
        virtual QHash<int, QByteArray> roleNames() const;

    protected:
        void removeInnerItem(int row);

    private:
        QVector<ArtItemInfo*> m_ArtworksList;
        Common::BasicKeywordsModel m_CommonKeywordsModel;
        SpellCheck::SpellCheckItemInfo m_SpellCheckInfo;
        int m_EditFlags;
        bool m_AreKeywordsModified;
        bool m_IsDescriptionModified;
        bool m_IsTitleModified;
    };
}

#endif // COMBINEDARTWORKSMODEL_H
