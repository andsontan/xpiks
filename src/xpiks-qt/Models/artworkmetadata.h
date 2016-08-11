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

#ifndef IMAGEMETADATA_H
#define IMAGEMETADATA_H

#include <QAbstractListModel>
#include <QReadWriteLock>
#include <QStringList>
#include <QFileInfo>
#include <QString>
#include <QVector>
#include <QSet>
#include <QTimer>
#include <QQmlEngine>
#include "../Common/basickeywordsmodel.h"
#include "../Common/flags.h"
#include "../Common/ibasicartwork.h"
#include "../Common/hold.h"
#include "../SpellCheck/spellcheckiteminfo.h"
#include "../UndoRedo/artworkmetadatabackup.h"

class QTextDocument;

namespace Models {
    class SettingsModel;

    class ArtworkMetadata:
        public QObject, public Common::IBasicArtwork
    {
    Q_OBJECT

    public:
        ArtworkMetadata(const QString &filepath, qint64 ID);
        virtual ~ArtworkMetadata();

    private:
        enum MetadataFlags {
            FlagIsModified = 1 << 0,
            FlagsIsSelected = 1 << 1,
            FlagIsInitialized = 1 << 2,
            FlagIsUnavailable = 1 << 3
        };

        inline bool getIsModifiedFlag() const { return Common::HasFlag(m_MetadataFlags, FlagIsModified); }
        inline bool getIsSelectedFlag() const { return Common::HasFlag(m_MetadataFlags, FlagsIsSelected); }
        inline bool getIsUnavailableFlag() const { return Common::HasFlag(m_MetadataFlags, FlagIsUnavailable); }
        inline bool getIsInitializedFlag() const { return Common::HasFlag(m_MetadataFlags, FlagIsInitialized); }

        inline void setIsModifiedFlag(bool value) { Common::ApplyFlag(m_MetadataFlags, value, FlagIsModified); }
        inline void setIsSelectedFlag(bool value) { Common::ApplyFlag(m_MetadataFlags, value, FlagsIsSelected); }
        inline void setIsUnavailableFlag(bool value) { Common::ApplyFlag(m_MetadataFlags, value, FlagIsUnavailable); }
        inline void setIsInitializedFlag(bool value) { Common::ApplyFlag(m_MetadataFlags, value, FlagIsInitialized); }

    public:
        bool initialize(const QString &title,
                        const QString &description, const QStringList &rawKeywords, bool overwrite=true);

    public:
        virtual const QString &getFilepath() const { return m_ArtworkFilepath; }
        virtual QString getDirectory() const { QFileInfo fi(m_ArtworkFilepath); return fi.absolutePath(); }
        QString getBaseFilename() const;
        bool isInDirectory(const QString &directoryAbsolutePath) const;

        bool isModified() const { return getIsModifiedFlag(); }
        bool isSelected() const { return getIsSelectedFlag(); }
        bool isUnavailable() const { return getIsUnavailableFlag(); }
        bool isInitialized() const { return getIsInitializedFlag(); }
        virtual qint64 getFileSize() const { return m_FileSize; }
        virtual qint64 getItemID() const { return m_ID; }

    public:
        int getWarningsFlags() const { return m_WarningsFlags; }
        void setWarningsFlags(int flags) { m_WarningsFlags = flags; }
        void addWarningsFlags(int flags) { m_WarningsFlags |= flags; }
        void dropWarningsFlags(int flagsToDrop) { m_WarningsFlags &= (~flagsToDrop); }

    public:
        Common::BasicKeywordsModel *getKeywordsModel() { return &m_KeywordsModel; }
        const Common::BasicKeywordsModel *getKeywordsModel() const { return &m_KeywordsModel; }

        virtual void clearModel();
        virtual bool clearKeywords();
        virtual bool editKeyword(int index, const QString &replacement);
        virtual bool replace(const QString &replaceWhat, const QString &replaceTo, int flags);

    public:
        virtual bool setDescription(const QString &value) {
            bool result = m_KeywordsModel.setDescription(value);

            if (result) { markModified(); }

            return result;
        }

        virtual bool setTitle(const QString &value) {
            bool result = m_KeywordsModel.setTitle(value);

            if (result) { markModified(); }

            return result;
        }

        virtual void setKeywords(const QStringList &keywords) {
            m_KeywordsModel.setKeywords(keywords);
            markModified();
        }

        bool setIsSelected(bool value);

        void invertSelection() { setIsSelected(!getIsSelectedFlag()); }

        void resetSelected() {
            if (getIsSelectedFlag()) {
                setIsSelectedFlag(false);
                // emit fileSelectedChanged(m_ArtworkFilepath, false);
            }
        }

        void setFileSize(qint64 size) { m_FileSize = size; }

    public:
        bool areKeywordsEmpty() { return m_KeywordsModel.areKeywordsEmpty(); }
        bool removeKeywordAt(int index);
        bool removeLastKeyword();
        virtual bool appendKeyword(const QString &keyword);
        virtual int appendKeywords(const QStringList &keywordsList);
        bool removeKeywords(const QSet<QString> &keywordsSet, bool caseSensitive=true);

    public:
        void acquire() { m_Hold.acquire(); }
        bool release() { return m_Hold.release(); }

    public:
        // IBasicArtwork interface
        virtual QSet<QString> getKeywordsSet() { return m_KeywordsModel.getKeywordsSet(); }
        virtual QStringList getKeywords() { return m_KeywordsModel.getKeywords(); }
        virtual bool isEmpty() { return m_KeywordsModel.isEmpty(); }
        virtual QString getDescription() { return m_KeywordsModel.getDescription(); }
        virtual QString getTitle() { return m_KeywordsModel.getTitle(); }

    public:
        void markModified();
        void setUnavailable() { setIsUnavailableFlag(true); }
        void resetModified() { setIsModifiedFlag(false); }
        void requestFocus(int directionSign) { emit focusRequested(directionSign); }
        void requestBackup() { m_BackupTimer.start(1000); }

#ifndef CORE_TESTS
    private:
#else
    public:
#endif
        void setModified() { setIsModifiedFlag(true); }
        friend class UndoRedo::ArtworkMetadataBackup;

    signals:
        void modifiedChanged(bool newValue);
        void selectedChanged(bool newValue);
        void fileSelectedChanged(const QString &filepath, bool newValue);
        void focusRequested(int directionSign);
        void backupRequired();
        void aboutToBeRemoved();
        void spellCheckErrorsChanged();

    private slots:
        void backupTimerTriggered() { emit backupRequired(); }

    private:
        Common::Hold m_Hold;
        SpellCheck::SpellCheckItemInfo m_SpellCheckInfo;
        Common::BasicKeywordsModel m_KeywordsModel;
        qint64 m_FileSize;  // in bytes
        QString m_ArtworkFilepath;
        QTimer m_BackupTimer;
        qint64 m_ID;
        volatile int m_MetadataFlags;
        volatile int m_WarningsFlags;
    };
}

Q_DECLARE_METATYPE(Models::ArtworkMetadata *)

#endif // IMAGEMETADATA_H
