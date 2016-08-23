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
#include <memory>
#include <vector>
#include "artworksviewmodel.h"
#include "../Common/basickeywordsmodel.h"
#include "../Common/flags.h"
#include "../SpellCheck/spellcheckiteminfo.h"
#include "../Common/hold.h"
#include "../Models/metadataelement.h"

namespace Models {
    class CombinedArtworksModel:
        public ArtworksViewModel
    {
    Q_OBJECT
    Q_PROPERTY(QString description READ getDescription WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(QString title READ getTitle WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(int keywordsCount READ getKeywordsCount NOTIFY keywordsCountChanged)
    Q_PROPERTY(bool changeDescription READ getChangeDescription WRITE setChangeDescription NOTIFY changeDescriptionChanged)
    Q_PROPERTY(bool changeTitle READ getChangeTitle WRITE setChangeTitle NOTIFY changeTitleChanged)
    Q_PROPERTY(bool changeKeywords READ getChangeKeywords WRITE setChangeKeywords NOTIFY changeKeywordsChanged)
    Q_PROPERTY(bool appendKeywords READ getAppendKeywords WRITE setAppendKeywords NOTIFY appendKeywordsChanged)

    public:
        CombinedArtworksModel(QObject *parent=0);

        virtual ~CombinedArtworksModel() {}

    public:
        virtual void setArtworks(std::vector<MetadataElement> &artworks);

    private:
        enum CombinedEditModifiedFlags {
            FlagTitleModified = 1 << 0,
            FlagDescriptionModified = 1 << 1,
            FlagKeywordsModified = 1 << 2,
            FlagSpellingFixed = 1 << 3
        };

        inline bool getTitleModifiedFlag() const { return Common::HasFlag(m_ModifiedFlags, FlagTitleModified); }
        inline bool getDescriptionModifiedFlag() const { return Common::HasFlag(m_ModifiedFlags, FlagDescriptionModified); }
        inline bool getKeywordsModifiedFlag() const { return Common::HasFlag(m_ModifiedFlags, FlagKeywordsModified); }
        inline bool getSpellingFixedFlag() const { return Common::HasFlag(m_ModifiedFlags, FlagSpellingFixed); }

        inline void setTitleModifiedFlag(bool value) { Common::ApplyFlag(m_ModifiedFlags, value, FlagTitleModified); }
        inline void setDescriptionModifiedFlag(bool value) { Common::ApplyFlag(m_ModifiedFlags, value, FlagDescriptionModified); }
        inline void setKeywordsModifiedFlag(bool value) { Common::ApplyFlag(m_ModifiedFlags, value, FlagKeywordsModified); }
        inline void setSpellingFixedFlag(bool value) { Common::ApplyFlag(m_ModifiedFlags, value, FlagSpellingFixed); }

        void setTitleModified(bool value) { setTitleModifiedFlag(value); }
        void setDescriptionModified(bool value) { setDescriptionModifiedFlag(value); }
        void setKeywordsModified(bool value) { setKeywordsModifiedFlag(value); }
        void setSpellingFixed(bool value) { setSpellingFixedFlag(value); }

    public:
        bool isTitleModified() const { return getTitleModifiedFlag(); }
        bool isDescriptionModified() const { return getDescriptionModifiedFlag(); }
        bool areKeywordsModified() const { return getKeywordsModifiedFlag(); }
        bool isSpellingFixed() const { return getSpellingFixedFlag(); }

    private:
        void initKeywords(const QStringList &ek) { m_CommonKeywordsModel.setKeywords(ek); setKeywordsModified(false); }
        void initDescription(const QString &description) { setDescription(description); setDescriptionModified(false); }
        void initTitle(const QString &title) { setTitle(title); setTitleModified(false); }
        void recombineArtworks();

    public:
        virtual void acceptSuggestedKeywords(const QStringList &keywords);

    public:
        void setKeywords(const QStringList &keywords) { m_CommonKeywordsModel.setKeywords(keywords); }
        QString getDescription() { return m_CommonKeywordsModel.getDescription(); }
        void setDescription(const QString &value) {
            if (m_CommonKeywordsModel.setDescription(value)) {
                emit descriptionChanged();
                setDescriptionModified(true);
            }
        }

        QString getTitle() { return m_CommonKeywordsModel.getTitle(); }
        void setTitle(const QString &value) {
            if (m_CommonKeywordsModel.setTitle(value)) {
                emit titleChanged();
                setTitleModified(true);
            }
        }

        int getKeywordsCount() { return m_CommonKeywordsModel.getKeywordsCount(); }

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
        void changeDescriptionChanged();
        void changeKeywordsChanged();
        void changeTitleChanged();
        void appendKeywordsChanged();
        void artworkUnavailable(int index);
        void completionsAvailable();

    public:
        void generateAboutToBeRemoved();

#ifdef INTEGRATION_TESTS
        Common::BasicKeywordsModel *getBasicKeywordsModel() { return &m_CommonKeywordsModel; }
#endif

#ifdef CORE_TESTS
        QStringList getKeywords();

#endif

    public:
        Q_INVOKABLE void editKeyword(int index, const QString &replacement);
        Q_INVOKABLE QString removeKeywordAt(int keywordIndex);
        Q_INVOKABLE void removeLastKeyword();
        Q_INVOKABLE void appendKeyword(const QString &keyword);
        Q_INVOKABLE void pasteKeywords(const QStringList &keywords);
        Q_INVOKABLE void saveEdits();
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
        void processCombinedEditCommand();
        void enableAllFields();
        void assignFromOneArtwork();
        void assignFromManyArtworks();

    private slots:
        void spellCheckErrorsChangedHandler();
        void spellCheckErrorsFixedHandler();

    public slots:
        void userDictUpdateHandler(const QStringList &keywords);
        void userDictUpdateHandler();

    protected:
        virtual bool doRemoveSelectedArtworks();
        virtual void doResetModel();

    public:
        virtual bool removeUnavailableItems();

    private:
        Common::Hold m_HoldPlaceholder;
        Common::BasicKeywordsModel m_CommonKeywordsModel;
        SpellCheck::SpellCheckItemInfo m_SpellCheckInfo;
        int m_EditFlags;
        int m_ModifiedFlags;
    };
}

#endif // COMBINEDARTWORKSMODEL_H
