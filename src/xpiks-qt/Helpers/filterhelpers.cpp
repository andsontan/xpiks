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
#include "../Models/imageartwork.h"
#include "../Common/basickeywordsmodel.h"
#include "../Common/flags.h"
#include "../Common/defines.h"

namespace Helpers {
    bool fitsSpecialKeywords(const QString &searchTerm, Models::ArtworkMetadata *metadata) {
        bool hasMatch = false;

        const Models::ImageArtwork *image = dynamic_cast<const Models::ImageArtwork*>(metadata);
        if (image == NULL) { return hasMatch; }

        if (searchTerm == QLatin1String("x:modified")) {
            hasMatch = metadata->isModified();
        } else if (searchTerm == QLatin1String("x:empty")) {
            hasMatch = metadata->isEmpty();
        } else if (searchTerm == QLatin1String("x:selected")) {
            hasMatch = metadata->isSelected();
        } else if (searchTerm == QLatin1String("x:vector")) {
            hasMatch = image != NULL && image->hasVectorAttached();
        } else if (searchTerm == QLatin1String("x:image")) {
            hasMatch = image != NULL && !image->hasVectorAttached();
        }

        return hasMatch;
    }

    bool containsAnyPartsSearch(const QString &mainSearchTerm, Models::ArtworkMetadata *metadata, Common::SearchFlags searchFlags) {
        bool hasMatch = false;
        QStringList searchTerms = mainSearchTerm.split(QChar::Space, QString::SkipEmptyParts);

        const QString description = metadata->getDescription();
        const QString title = metadata->getTitle();
        const QString &filepath = metadata->getFilepath();

        const bool needToCheckDescription = Common::HasFlag(searchFlags, Common::SearchFlags::Description);
        const bool needToCheckTitle = Common::HasFlag(searchFlags, Common::SearchFlags::Title);
        const bool needToCheckSpecial = Common::HasFlag(searchFlags, Common::SearchFlags::ReservedTerms);
        const bool needToCheckFilepath = Common::HasFlag(searchFlags, Common::SearchFlags::Filepath);
        const bool caseSensitive = Common::HasFlag(searchFlags, Common::SearchFlags::CaseSensitive);
        Qt::CaseSensitivity caseSensitivity = caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;

        int length = searchTerms.length();

        Common::BasicKeywordsModel *keywordsModel = metadata->getKeywordsModel();

        for (int i = 0; i < length; ++i) {
            const QString &searchTerm = searchTerms.at(i);

            if (needToCheckSpecial) {
                hasMatch = fitsSpecialKeywords(searchTerm, metadata);
            }

            if (!hasMatch && needToCheckDescription) {
                hasMatch = description.contains(searchTerm, caseSensitivity);
            }

            if (!hasMatch && needToCheckTitle) {
                hasMatch = title.contains(searchTerm, caseSensitivity);
            }

            if (!hasMatch && needToCheckFilepath) {
                hasMatch = filepath.contains(searchTerm, caseSensitivity);
            }

            if (hasMatch) { break; }
        }


        const bool needToCheckKeywords = Common::HasFlag(searchFlags, Common::SearchFlags::Keywords);

        if (!hasMatch && needToCheckKeywords) {
            for (int i = 0; i < length; ++i) {
                QString searchTerm = searchTerms[i];
                Common::SearchFlags keywordsFlags = Common::SearchFlags::Keywords;

                if ((searchTerm.length() > 1) && searchTerm[0] == QLatin1Char('!')) {
                    Common::SetFlag(keywordsFlags, Common::SearchFlags::ExactMatch);
                    searchTerm.remove(0, 1);
                }

                if (caseSensitive) {
                    Common::SetFlag(keywordsFlags, Common::SearchFlags::CaseSensitive);
                }

                hasMatch = keywordsModel->containsKeyword(searchTerm, keywordsFlags);
                if (hasMatch) { break; }
            }
        }

        return hasMatch;
    }

    bool containsAllPartsSearch(const QString &mainSearchTerm, Models::ArtworkMetadata *metadata, Common::SearchFlags searchFlags) {
        bool hasMatch = false;
        QStringList searchTerms = mainSearchTerm.split(QChar::Space, QString::SkipEmptyParts);

        const QString description = metadata->getDescription();
        const QString title = metadata->getTitle();
        const QString &filepath = metadata->getFilepath();

        const bool needToCheckDescription = Common::HasFlag(searchFlags, Common::SearchFlags::Description);
        const bool needToCheckTitle = Common::HasFlag(searchFlags, Common::SearchFlags::Title);
        const bool needToCheckSpecial = Common::HasFlag(searchFlags, Common::SearchFlags::ReservedTerms);
        const bool needToCheckFilepath = Common::HasFlag(searchFlags, Common::SearchFlags::Filepath);
        const bool caseSensitive = Common::HasFlag(searchFlags, Common::SearchFlags::CaseSensitive);
        Qt::CaseSensitivity caseSensitivity = caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;

        bool anyError = false;
        int length = searchTerms.length();

        Common::BasicKeywordsModel *keywordsModel = metadata->getKeywordsModel();

        for (int i = 0; i < length; ++i) {
            QString searchTerm = searchTerms[i];
            bool anyContains = false;

            if (needToCheckSpecial) {
                anyContains = fitsSpecialKeywords(searchTerm, metadata);
            }

            if (!anyContains && needToCheckDescription) {
                anyContains = description.contains(searchTerm, caseSensitivity);
            }

            if (!anyContains && needToCheckTitle) {
                anyContains = title.contains(searchTerm, caseSensitivity);
            }

            if (!anyContains && needToCheckFilepath) {
                anyContains = filepath.contains(searchTerm, caseSensitivity);
            }

            const bool needToCheckKeywords = Common::HasFlag(searchFlags, Common::SearchFlags::Keywords);

            if (!anyContains && needToCheckKeywords) {
                Common::SearchFlags keywordsFlags = Common::SearchFlags::Keywords;

                if ((searchTerm.length() > 1) && searchTerm[0] == QLatin1Char('!')) {
                    Common::SetFlag(keywordsFlags, Common::SearchFlags::ExactMatch);
                    searchTerm.remove(0, 1);
                }

                if (caseSensitive) {
                    Common::SetFlag(keywordsFlags, Common::SearchFlags::CaseSensitive);
                }

                anyContains = keywordsModel->containsKeyword(searchTerm, keywordsFlags);
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
        // default search is not case sensitive
        Common::SearchFlags searchFlags = searchUsingAnd ? Common::SearchFlags::AllTermsEverything :
                                           Common::SearchFlags::AnyTermsEverything;
        bool hasMatch = hasSearchMatch(mainSearchTerm, metadata, searchFlags);
        return hasMatch;
    }

    bool hasSearchMatch(const QString &searchTerm, Models::ArtworkMetadata *metadata, Common::SearchFlags searchFlags) {
        bool hasMatch = false;

        const bool searchUsingAnd = Common::HasFlag(searchFlags, Common::SearchFlags::AllTerms);
        LOG_CORE_TESTS << "Search using AND:" << searchUsingAnd;
        LOG_CORE_TESTS << "Case sensitive:" << Common::HasFlag(searchFlags, Common::SearchFlags::CaseSensitive);

        if (searchUsingAnd) {
            hasMatch = containsAllPartsSearch(searchTerm, metadata, searchFlags);
        } else {
            hasMatch = containsAnyPartsSearch(searchTerm, metadata, searchFlags);
        }

        return hasMatch;
    }

}
