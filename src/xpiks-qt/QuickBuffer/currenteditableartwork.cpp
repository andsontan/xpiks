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

#include "currenteditableartwork.h"
#include "../Models/artworkmetadata.h"
#include "../Common/defines.h"
#include "../Commands/commandmanager.h"

namespace QuickBuffer {
    CurrentEditableArtwork::CurrentEditableArtwork(Models::ArtworkMetadata *artworkMetadata, int originalIndex, const Commands::CommandManager *commandManager):
        m_CommandManager(commandManager),
        m_OriginalIndex(originalIndex)
    {
        Q_ASSERT(commandManager != nullptr);
        Q_ASSERT(artworkMetadata != nullptr);
        m_ArtworkMetadata = artworkMetadata;

        m_ArtworkMetadata->acquire();
    }

    CurrentEditableArtwork::~CurrentEditableArtwork() {
        if (m_ArtworkMetadata->release()) {
            LOG_WARNING << "Item could have been removed";
        }
    }

    QString CurrentEditableArtwork::getTitle() {
        return m_ArtworkMetadata->getTitle();
    }

    QString CurrentEditableArtwork::getDescription() {
        return m_ArtworkMetadata->getDescription();
    }

    QStringList CurrentEditableArtwork::getKeywords() {
        return m_ArtworkMetadata->getKeywords();
    }

    void CurrentEditableArtwork::setTitle(const QString &value) {
        m_ArtworkMetadata->setTitle(value);
    }

    void CurrentEditableArtwork::setDescription(const QString &value) {
        m_ArtworkMetadata->setDescription(value);
    }

    void CurrentEditableArtwork::setKeywords(const QStringList &keywords) {
        m_ArtworkMetadata->setKeywords(keywords);
    }

    void CurrentEditableArtwork::spellCheck() {
        m_CommandManager->submitItemForSpellCheck(m_ArtworkMetadata->getBasicModel());
    }

    void CurrentEditableArtwork::update() {
        m_CommandManager->updateArtworks(QVector<int>() << m_OriginalIndex);
    }
}
