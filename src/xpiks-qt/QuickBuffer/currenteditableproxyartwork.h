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

#ifndef CURRENTEDITABLEPROXYARTWORK_H
#define CURRENTEDITABLEPROXYARTWORK_H

#include "icurrenteditable.h"

namespace Models {
    class ArtworkProxyBase;
}

namespace QuickBuffer {
    class CurrentEditableProxyArtwork : public ICurrentEditable
    {
    public:
        CurrentEditableProxyArtwork(Models::ArtworkProxyBase *artworkProxy);

        // ICurrentEditable interface
    public:
        virtual QString getTitle() override;
        virtual QString getDescription() override;
        virtual QStringList getKeywords() override;

        virtual void setTitle(const QString &title) override;
        virtual void setDescription(const QString &description) override;
        virtual void setKeywords(const QStringList &keywords) override;

        virtual bool expandPreset(int keywordIndex, int presetIndex) override;
        virtual bool removePreset(int presetIndex) override;

        virtual void spellCheck() override;
        virtual void update() override { /* BUMP */ }

    private:
        Models::ArtworkProxyBase *m_ArtworkProxy;
    };
}

#endif // CURRENTEDITABLEPROXYARTWORK_H
