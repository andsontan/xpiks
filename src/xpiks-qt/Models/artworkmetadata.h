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
#include <QSize>
#include <QAtomicInt>
#include "../Common/basickeywordsmodel.h"
#include "../Common/flags.h"

class QTextDocument;

namespace Models {
    class SettingsModel;

    class ArtworkMetadata : public Common::BasicKeywordsModel {
        Q_OBJECT
    public:
        ArtworkMetadata(const QString &filepath, qint64 ID);
        virtual ~ArtworkMetadata();

    private:
        enum MetadataFlags {
            FlagIsModified = 1 << 0,
            FlagsIsSelected = 1 << 1,
            FlagIsInitialized = 1 << 2,
            FlagHasVectorAttached = 1 << 3,
            FlagIsUnavailable = 1 << 4
        };

    public:
        bool initialize(const QString &title,
                        const QString &description, const QStringList &rawKeywords, bool overwrite = true);

    public:
        virtual const QString &getFilepath() const { return m_ArtworkFilepath; }
        virtual QString getDirectory() const { QFileInfo fi(m_ArtworkFilepath); return fi.absolutePath(); }

    public:
        bool isInDirectory(const QString &directoryAbsolutePath) const;
        bool isModified() const { return Common::HasFlag(m_MetadataFlags, FlagIsModified); }
        bool isSelected() const { return Common::HasFlag(m_MetadataFlags, FlagsIsSelected); }
        bool isUnavailable() const { return Common::HasFlag(m_MetadataFlags, FlagIsUnavailable); }
        bool isInitialized() const { return Common::HasFlag(m_MetadataFlags, FlagIsInitialized); }
        bool hasVectorAttached() const { return Common::HasFlag(m_MetadataFlags, FlagHasVectorAttached); }
        virtual QSize getImageSize() const { return m_ImageSize; }
        virtual qint64 getFileSize() const { return m_FileSize; }
        virtual qint64 getItemID() const { return m_ID; }
        const QString &getAttachedVectorPath() const { return m_AttachedVector; }
        void attachVector(const QString &vectorFilepath);
        void detachVector();

    public:
        virtual void clearModel();
        virtual bool clearKeywords();
        virtual bool editKeyword(int index, const QString &replacement);

    public:
        virtual bool setDescription(const QString &value) {
            bool result = BasicKeywordsModel::setDescription(value);
            if (result) { markModified(); }
            return result;
        }

        virtual bool setTitle(const QString &value) {
            bool result = BasicKeywordsModel::setTitle(value);
            if (result) { markModified(); }
            return result;
        }

        bool setIsSelected(bool value) {
            bool result = Common::HasFlag(m_MetadataFlags, FlagsIsSelected) != value;
            if (result) {
                Common::ApplyFlag(m_MetadataFlags, value, FlagsIsSelected);
                //emit fileSelectedChanged(m_ArtworkFilepath, value);
                emit selectedChanged(value);
            }

            return result;
        }

        void invertSelection() { setIsSelected(!isSelected()); }

        void resetSelected() {
            if (Common::HasFlag(m_MetadataFlags, FlagsIsSelected)) {
                Common::UnsetFlag(m_MetadataFlags, FlagsIsSelected);
                //emit fileSelectedChanged(m_ArtworkFilepath, false);
            }
        }

        void setImageSize(const QSize &size) { m_ImageSize = size; }
        void setFileSize(qint64 size) { m_FileSize = size; }

    public:
        bool removeKeywordAt(int index);
        bool removeLastKeyword();
        virtual bool appendKeyword(const QString &keyword);
        virtual int appendKeywords(const QStringList &keywordsList);

    public:
        void markModified();
        void setModified() { Common::SetFlag(m_MetadataFlags, FlagIsModified); }
        void setUnavailable() { Common::SetFlag(m_MetadataFlags, FlagIsUnavailable); }
        void resetModified() { Common::UnsetFlag(m_MetadataFlags, FlagIsModified); }
        void requestFocus(int directionSign) { emit focusRequested(directionSign); }

    signals:
         void modifiedChanged(bool newValue);
         void selectedChanged(bool newValue);
         void fileSelectedChanged(const QString &filepath, bool newValue);
         void focusRequested(int directionSign);

    private:
         QSize m_ImageSize;
         qint64 m_FileSize; // in bytes
         QString m_ArtworkFilepath;
         QString m_AttachedVector;
         qint64 m_ID;
         volatile int m_MetadataFlags;
    };
}

Q_DECLARE_METATYPE(Models::ArtworkMetadata*)

#endif // IMAGEMETADATA_H
