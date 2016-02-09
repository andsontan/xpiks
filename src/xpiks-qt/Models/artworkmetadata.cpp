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
#include <QDebug>
#include <QReadLocker>
#include <QWriteLocker>
#include <QStringBuilder>
#include <QDir>
#include "../Helpers/keywordvalidator.h"
#include "settingsmodel.h"
#include "../SpellCheck/spellsuggestionsitem.h"
#include "../SpellCheck/spellcheckitem.h"
#include "../SpellCheck/spellcheckiteminfo.h"

namespace Models {
    ArtworkMetadata::ArtworkMetadata(const QString &filepath) :
        Common::BasicKeywordsModel(),
        m_ArtworkFilepath(filepath),
        m_IsModified(false),
        m_IsSelected(false),
        m_IsInitialized(false)
    {
        setSpellCheckInfo(new SpellCheck::SpellCheckItemInfo());
    }

    ArtworkMetadata::~ArtworkMetadata() {
        this->freeSpellCheckInfo();
    }

    bool ArtworkMetadata::initialize(const QString &title,
                                     const QString &description, const QStringList &rawKeywords, bool overwrite) {
        bool anythingModified = false;

        if (overwrite || (isTitleEmpty() && !title.trimmed().isEmpty())) {
            anythingModified = true;
            BasicKeywordsModel::setTitle(title);
        }

        if (overwrite || (isDescriptionEmpty() && !description.trimmed().isEmpty())) {
            anythingModified = true;
            BasicKeywordsModel::setDescription(description);
        }

        if (overwrite && !rawKeywords.isEmpty()) {
            anythingModified = true;
            beginResetModel();
            BasicKeywordsModel::resetKeywords();
            BasicKeywordsModel::addKeywords(rawKeywords);
            endResetModel();
        } else if (!rawKeywords.isEmpty()) {
            int appendedCount = appendKeywords(rawKeywords);
            anythingModified = anythingModified || (appendedCount > 0);
        }

        m_IsModified = false;
        m_IsInitialized = true;

        return anythingModified;
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
        BasicKeywordsModel::clearModel();
        markModified();
    }

    bool ArtworkMetadata::clearKeywords() {
        bool result = BasicKeywordsModel::clearKeywords();
        if (result) { markModified(); }
        return result;
    }

    bool ArtworkMetadata::editKeyword(int index, const QString &replacement) {
        bool result = BasicKeywordsModel::editKeyword(index, replacement);
        if (result) { markModified(); }
        return result;
    }

    bool ArtworkMetadata::removeKeywordAt(int index) {
        QString removed;
        bool result = BasicKeywordsModel::takeKeywordAt(index, removed);
        if (result) { markModified(); }
        return result;
    }

    bool ArtworkMetadata::removeLastKeyword() {
        QString removed;
        bool result = BasicKeywordsModel::takeLastKeyword(removed);
        if (result) { markModified(); }
        return result;
    }

    bool ArtworkMetadata::appendKeyword(const QString &keyword) {
        bool result = BasicKeywordsModel::appendKeyword(keyword);
        if (result) { markModified(); }
        return result;
    }

    int ArtworkMetadata::appendKeywords(const QStringList &keywordsList) {
        int result = BasicKeywordsModel::appendKeywords(keywordsList);
        if (result > 0) { markModified(); }
        return result;
    }

    void ArtworkMetadata::markModified() {
        if (!m_IsModified) {
            m_IsModified = true;
            emit modifiedChanged(m_IsModified);
        }
    }
}
