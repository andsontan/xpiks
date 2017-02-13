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

#include "currenteditableproxyartwork.h"
#include "../Models/artworkproxybase.h"

namespace QuickBuffer {
    CurrentEditableProxyArtwork::CurrentEditableProxyArtwork(Models::ArtworkProxyBase *artworkProxy):
        m_ArtworkProxy(artworkProxy)
    {
        Q_ASSERT(artworkProxy != nullptr);
    }

    QString CurrentEditableProxyArtwork::getTitle() {
        return m_ArtworkProxy->getTitle();
    }

    QString CurrentEditableProxyArtwork::getDescription() {
        return m_ArtworkProxy->getDescription();
    }

    QStringList CurrentEditableProxyArtwork::getKeywords() {
        return m_ArtworkProxy->getKeywords();
    }

    void CurrentEditableProxyArtwork::setTitle(const QString &title) {
        m_ArtworkProxy->setTitle(title);
    }

    void CurrentEditableProxyArtwork::setDescription(const QString &description) {
        m_ArtworkProxy->setDescription(description);
    }

    void CurrentEditableProxyArtwork::setKeywords(const QStringList &keywords) {
        m_ArtworkProxy->setKeywords(keywords);
    }

    bool CurrentEditableProxyArtwork::expandPreset(int keywordIndex, int presetIndex) {
        return m_ArtworkProxy->doExpandPreset(keywordIndex, presetIndex);
    }

    bool CurrentEditableProxyArtwork::removePreset(int presetIndex) {
        return m_ArtworkProxy->doRemovePreset(presetIndex);
    }

    void CurrentEditableProxyArtwork::spellCheck() {
        m_ArtworkProxy->spellCheckEverything();
    }
}
