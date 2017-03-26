/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
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

#define MAX_BACKUP_TIMER_DELAYS 5

namespace Models {
    ArtworkMetadata::ArtworkMetadata(const QString &filepath, qint64 ID, qint64 directoryID):
        m_MetadataModel(m_Hold),
        m_FileSize(0),
        m_ArtworkFilepath(filepath),
        m_BackupTimerDelay(0),
        m_ID(ID),
        m_DirectoryID(directoryID),
        m_MetadataFlags(0),
        m_WarningsFlags(Common::WarningFlags::None),
        m_IsLockedForEditing(false)
    {
        m_MetadataModel.setSpellCheckInfo(&m_SpellCheckInfo);
        m_BackupTimer.setSingleShot(true);

        QObject::connect(&m_BackupTimer, SIGNAL(timeout()), this, SLOT(backupTimerTriggered()));
        QObject::connect(&m_MetadataModel, SIGNAL(spellCheckErrorsChanged()), this, SIGNAL(spellCheckErrorsChanged()));
    }

    ArtworkMetadata::~ArtworkMetadata() {
#if defined(QT_DEBUG) && defined(INTEGRATION_TESTS)
        Q_ASSERT(m_Hold.get() == 0);
#endif
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

    Common::KeywordReplaceResult ArtworkMetadata::fixKeywordSpelling(int index, const QString &existing, const QString &replacement) {
        auto result = m_MetadataModel.fixKeywordSpelling(index, existing, replacement);
        if (Common::KeywordReplaceResult::Succeeded == result) {
            markModified();
        }

        return result;
    }

    bool ArtworkMetadata::fixDescriptionSpelling(const QString &word, const QString &replacement) {
        bool result = m_MetadataModel.fixDescriptionSpelling(word, replacement);
        if (result) {
            markModified();
        }

        return result;
    }

    bool ArtworkMetadata::fixTitleSpelling(const QString &word, const QString &replacement) {
        bool result = m_MetadataModel.fixTitleSpelling(word, replacement);
        if (result) {
            markModified();
        }

        return result;
    }

    std::vector<std::shared_ptr<SpellCheck::SpellSuggestionsItem> > ArtworkMetadata::createDescriptionSuggestionsList() {
        return m_MetadataModel.createDescriptionSuggestionsList();
    }

    std::vector<std::shared_ptr<SpellCheck::SpellSuggestionsItem> > ArtworkMetadata::createTitleSuggestionsList() {
        return m_MetadataModel.createTitleSuggestionsList();
    }

    std::vector<std::shared_ptr<SpellCheck::SpellSuggestionsItem> > ArtworkMetadata::createKeywordsSuggestionsList() {
        return m_MetadataModel.createKeywordsSuggestionsList();
    }

    bool ArtworkMetadata::processFailedKeywordReplacements(const std::vector<std::shared_ptr<SpellCheck::KeywordSpellSuggestions> > &candidatesForRemoval) {
        bool result = m_MetadataModel.processFailedKeywordReplacements(candidatesForRemoval);
        if (result) {
            markModified();
        }

        return result;
    }

    void ArtworkMetadata::afterReplaceCallback() {
        m_MetadataModel.afterReplaceCallback();
    }

    Common::BasicKeywordsModel *ArtworkMetadata::getBasicKeywordsModel() {
        return &m_MetadataModel;
    }

    void ArtworkMetadata::markModified() {
        if (!getIsModifiedFlag()) {
            setIsModifiedFlag(true);
            emit modifiedChanged(true);
        }
    }

    void ArtworkMetadata::requestBackup() {
        if (m_BackupTimerDelay < MAX_BACKUP_TIMER_DELAYS) {
            m_BackupTimer.start(1000);
            m_BackupTimerDelay++;
        } else {
            LOG_INFO << "Maximum backup delays occured, forcing backup";
            Q_ASSERT(m_BackupTimer.isActive());
        }
    }

    bool ArtworkMetadata::expandPreset(int keywordIndex, const QStringList &presetList)  {
        bool result = m_MetadataModel.expandPreset(keywordIndex, presetList);
        if (result) {
            markModified();
        }

        return result;
    }

    bool ArtworkMetadata::appendPreset(const QStringList &presetList) {
        bool result = m_MetadataModel.appendPreset(presetList);
        if (result) {
            markModified();
        }

        return result;
    }

    bool ArtworkMetadata::hasKeywords(const QStringList &keywordsList) {
        bool result = m_MetadataModel.hasKeywords(keywordsList);
        return result;
    }

    void ArtworkMetadata::deepDisconnect() {
        m_MetadataModel.disconnect();
        this->disconnect();
    }
}
