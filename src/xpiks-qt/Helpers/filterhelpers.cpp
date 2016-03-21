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

#include "filterhelpers.h"
#include <QString>
#include "../Models/artworkmetadata.h"

namespace Helpers {
    bool fitsSpecialKeywords(const QString &searchTerm, const Models::ArtworkMetadata *metadata) {
        bool hasMatch = false;

        if (searchTerm == QLatin1String("x:modified")) {
            hasMatch = metadata->isModified();
        } else if (searchTerm == QLatin1String("x:empty")) {
            hasMatch = metadata->isEmpty();
        } else if (searchTerm == QLatin1String("x:selected")) {
            hasMatch = metadata->isSelected();
        } else if (searchTerm == QLatin1String("x:vector")) {
            hasMatch = metadata->hasVectorAttached();
        } else if (searchTerm == QLatin1String("x:image")) {
            hasMatch = !metadata->hasVectorAttached();
        }

        return hasMatch;
    }

    bool containsAnyPartsSearch(const QString &mainSearchTerm, Models::ArtworkMetadata *metadata) {
        bool hasMatch = false;
        QStringList searchTerms = mainSearchTerm.split(QChar::Space, QString::SkipEmptyParts);

        const QString &description = metadata->getDescription();
        const QString &title = metadata->getTitle();
        const QString &filepath = metadata->getFilepath();

        int length = searchTerms.length();

        for (int i = 0; i < length; ++i) {
            const QString &searchTerm = searchTerms.at(i);

            hasMatch = fitsSpecialKeywords(searchTerm, metadata);

            if (!hasMatch) {
                hasMatch = description.contains(searchTerm, Qt::CaseInsensitive);
            }

            if (!hasMatch) {
                hasMatch = title.contains(searchTerm, Qt::CaseInsensitive);
            }

            if (!hasMatch) {
                hasMatch = filepath.contains(searchTerm, Qt::CaseInsensitive);
            }

            if (hasMatch) { break; }
        }

        if (!hasMatch) {
            for (int i = 0; i < length; ++i) {
                QString searchTerm = searchTerms[i];
                bool strictMatch = false;

                if ((searchTerm.length() > 1) && searchTerm[0] == QChar('!')) {
                    strictMatch = true;
                    searchTerm.remove(0, 1);
                }

                hasMatch = metadata->containsKeyword(searchTerm, strictMatch);
                if (hasMatch) { break; }
            }
        }

        return hasMatch;
    }

    bool containsAllPartsSearch(const QString &mainSearchTerm, Models::ArtworkMetadata *metadata) {
        bool hasMatch = false;
        QStringList searchTerms = mainSearchTerm.split(QChar::Space, QString::SkipEmptyParts);

        const QString &description = metadata->getDescription();
        const QString &title = metadata->getTitle();
        const QString &filepath = metadata->getFilepath();

        bool anyError = false;
        int length = searchTerms.length();

        for (int i = 0; i < length; ++i) {
            QString searchTerm = searchTerms[i];
            bool anyContains = false;
            bool strictMatch = false;

            anyContains = fitsSpecialKeywords(searchTerm, metadata);

            if (!anyContains) {
                anyContains = description.contains(searchTerm, Qt::CaseInsensitive);
            }

            if (!anyContains) {
                anyContains = title.contains(searchTerm, Qt::CaseInsensitive);
            }

            if (!anyContains) {
                anyContains = filepath.contains(searchTerm, Qt::CaseInsensitive);
            }

            if (!anyContains) {
                if ((searchTerm.length() > 1) && searchTerm[0] == QChar('!')) {
                    strictMatch = true;
                    searchTerm.remove(0, 1);
                }

                anyContains = metadata->containsKeyword(searchTerm, strictMatch);
            }

            if (!anyContains) {
                anyError = true;
                break;
            }
        }

        hasMatch = !anyError;
        return hasMatch;
    }

    bool containsPartsSearch(const QString &mainSearchTerm, Models::ArtworkMetadata *metadata, bool searchUsingAnd) {
        bool hasMatch = false;

        if (searchUsingAnd) {
            hasMatch = containsAllPartsSearch(mainSearchTerm, metadata);
        } else {
            hasMatch = containsAnyPartsSearch(mainSearchTerm, metadata);
        }

        return hasMatch;
    }
}
