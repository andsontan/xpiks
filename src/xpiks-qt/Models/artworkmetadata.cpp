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

#include "artworkmetadata.h"
#include <QReadLocker>
#include <QWriteLocker>
#include <QStringBuilder>
#include <QDir>
#include "../Helpers/keywordshelpers.h"
#include "settingsmodel.h"
#include "../SpellCheck/spellsuggestionsitem.h"
#include "../SpellCheck/spellcheckitem.h"
#include "../SpellCheck/spellcheckiteminfo.h"
#include "../Common/defines.h"

namespace Models {
    ArtworkMetadata::ArtworkMetadata(const QString &filepath, qint64 ID):
        m_MetadataModel(m_Hold),
        m_FileSize(0),
        m_ArtworkFilepath(filepath),
        m_ID(ID),
        m_MetadataFlags(0),
        m_WarningsFlags(Common::WarningFlags::None)
    {
        m_MetadataModel.setSpellCheckInfo(&m_SpellCheckInfo);
        m_BackupTimer.setSingleShot(true);

        QObject::connect(&m_BackupTimer, SIGNAL(timeout()), this, SLOT(backupTimerTriggered()));
        QObject::connect(&m_MetadataModel, SIGNAL(spellCheckErrorsChanged()), this, SIGNAL(spellCheckErrorsChanged()));
    }

    ArtworkMetadata::~ArtworkMetadata() {
    }

    bool ArtworkMetadata::initialize(const QString &title,
                                     const QString &description, const QStringList &rawKeywords, bool overwrite) {
        LOG_INTEGRATION_TESTS << title << description << rawKeywords << overwrite;

        bool anythingModified = false;

        if (overwrite || (m_MetadataModel.isTitleEmpty() && !title.trimmed().isEmpty())) {
            anythingModified = true;
            m_MetadataModel.setTitle(title);
        }

        if (overwrite || (m_MetadataModel.isDescriptionEmpty() && !description.trimmed().isEmpty())) {
            anythingModified = true;
            m_MetadataModel.setDescription(description);
        }

        if (overwrite) {
            anythingModified = true;
            m_MetadataModel.setKeywords(rawKeywords);
        } else if (!rawKeywords.isEmpty()) {
            int appendedCount = m_MetadataModel.appendKeywords(rawKeywords);
            anythingModified = anythingModified || (appendedCount > 0);
        }

        setIsModifiedFlag(false);
        setIsInitializedFlag(true);

        return anythingModified;
    }

    QString ArtworkMetadata::getBaseFilename() const {
        QFileInfo fi(m_ArtworkFilepath);
        return fi.fileName();
    }

    bool ArtworkMetadata::isInDirectory(const QString &directoryAbsolutePath) const {
        bool isInDir = false;
        Q_ASSERT(directoryAbsolutePath == QDir(directoryAbsolutePath).absolutePath());

        if (m_ArtworkFilepath.startsWith(directoryAbsolutePath)) {
            QFileInfo fi(m_ArtworkFilepath);
            QString artworksDirectory = fi.absolutePath();

            isInDir = artworksDirectory == directoryAbsolutePath;
        }

        return isInDir;
    }

    void ArtworkMetadata::clearModel() {
        m_MetadataModel.clearModel();
        markModified();
    }

    bool ArtworkMetadata::clearKeywords() {
        bool result = m_MetadataModel.clearKeywords();
        if (result) { markModified(); }
        return result;
    }

    bool ArtworkMetadata::editKeyword(int index, const QString &replacement) {
        bool result = m_MetadataModel.editKeyword(index, replacement);
        if (result) { markModified(); }
        return result;
    }

    bool ArtworkMetadata::replace(const QString &replaceWhat, const QString &replaceTo, Common::SearchFlags flags) {
        bool result = m_MetadataModel.replace(replaceWhat, replaceTo, flags);
        if (result) { markModified(); }
        return result;
    }

    bool ArtworkMetadata::setIsSelected(bool value) {
        bool result = getIsSelectedFlag() != value;
        if (result) {
            setIsSelectedFlag(value);
            //emit fileSelectedChanged(m_ArtworkFilepath, value);
            emit selectedChanged(value);
        }

        return result;
    }

    bool ArtworkMetadata::removeKeywordAt(int index, QString &removed) {
        bool result = m_MetadataModel.removeKeywordAt(index, removed);
        if (result) { markModified(); }
        return result;
    }

    bool ArtworkMetadata::removeLastKeyword(QString &removed) {
        bool result = m_MetadataModel.removeLastKeyword(removed);
        if (result) { markModified(); }
        return result;
    }

    bool ArtworkMetadata::appendKeyword(const QString &keyword) {
        bool result = m_MetadataModel.appendKeyword(keyword);
        if (result) { markModified(); }
        return result;
    }

    int ArtworkMetadata::appendKeywords(const QStringList &keywordsList) {
        int result = m_MetadataModel.appendKeywords(keywordsList);
        LOG_INFO << "Appended" << result << "keywords out of" << keywordsList.length();
        if (result > 0) { markModified(); }
        return result;
    }

    bool ArtworkMetadata::removeKeywords(const QSet<QString> &keywordsSet, bool caseSensitive) {
        bool result = m_MetadataModel.removeKeywords(keywordsSet, caseSensitive);
        LOG_INFO << "Removed keywords:" << result;
        if (result) { markModified(); }
        return result;
    }

    void ArtworkMetadata::markModified() {
        if (!getIsModifiedFlag()) {
            setIsModifiedFlag(true);
            emit modifiedChanged(true);
        }
    }
}
