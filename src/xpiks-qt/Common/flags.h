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

#ifndef FLAGS
#define FLAGS

namespace Common {
    enum CombinedEditFlags {
        EditTitle = 1,
        EditDesctiption = 2,
        EditKeywords = 4,
        AppendKeywords = 8,
        Clear = 16,
        EditEverything = EditTitle | EditDesctiption | EditKeywords
    };

    enum SuggestCorrectionsFlags {
        CorrectTitle = 1,
        CorrectDescription = 2,
        CorrectKeywords = 4,
        CorrectAll = 7
    };

    enum SpellCheckFlags {
        SpellCheckTitle = 1,
        SpellCheckDescription = 2,
        SpellCheckKeywords = 4,
        SpellCheckAll = 7
    };

    enum KeywordReplaceResult {
        KeywordReplaceSucceeded = 0,
        KeywordReplaceFailedIndex = 1,
        KeywordReplaceFailedDuplicate = 2,
        KeywordReplaceUnknown = 1 << 20
    };

    enum SearchFlags {
        SearchFlagCaseSensitive = 1 << 0,
        SearchFlagSearchDescription = 1 << 1,
        SearchFlagSearchTitle = 1 << 2,
        SearchFlagSearchKeywords = 1 << 3,
        SearchFlagReservedTerms = 1 << 4, // include reserved terms like "x:empty"
        SearchFlagAllSearchTerms = 1 << 5, // all of the search terms included in the result
        SearchFlagSearchFilepath = 1 << 6,
        SearchFlagExactMatch = 1 << 7,

        SearchFlagSearchMetadata = SearchFlagSearchDescription |
            SearchFlagSearchTitle |
            SearchFlagSearchKeywords,

        SearchFlagExactKeywords = SearchFlagExactMatch |
            SearchFlagSearchKeywords,

        SearchFlagsSearchMetadataCaseSensitive = SearchFlagSearchMetadata | SearchFlagCaseSensitive,

        SearchFlagSearchEverything = SearchFlagSearchMetadata | SearchFlagSearchFilepath | SearchFlagReservedTerms,
        SearchFlagSearchAllTermsEverything = SearchFlagSearchEverything | SearchFlagAllSearchTerms,
        SearchFlagSearchAnyTermsEverything = SearchFlagSearchEverything
    };

    enum WarningType {
        WarningTypeNoWarnings = 0,
        WarningTypeSizeLessThanMinimum = 1 << 0,
        WarningTypeNoKeywords = 1 << 1,
        WarningTypeTooFewKeywords = 1 << 2,
        WarningTypeTooManyKeywords = 1 << 3,
        WarningTypeDescriptionIsEmpty = 1 << 4,
        WarningTypeDescriptionNotEnoughWords = 1 << 5,
        WarningTypeDescriptionTooBig = 1 << 6,
        WarningTypeTitleIsEmpty = 1 << 7,
        WarningTypeTitleNotEnoughWords = 1 << 8,
        WarningTypeTitleTooManyWords = 1 << 9,
        WarningTypeTitleTooBig = 1 << 10,
        WarningTypeSpellErrorsInKeywords = 1 << 11,
        WarningTypeSpellErrorsInDescription = 1 << 12,
        WarningTypeSpellErrorsInTitle = 1 << 13,
        WarningTypeFileIsTooBig = 1 << 14,
        WarningTypeKeywordsInDescription = 1 << 15,
        WarningTypeKeywordsInTitle = 1 << 16,
        WarningTypeFilenameSymbols = 1 << 17,

        WarningTypeDescriptionGroup = WarningTypeDescriptionIsEmpty |
            WarningTypeDescriptionNotEnoughWords |
            WarningTypeDescriptionTooBig |
            WarningTypeSpellErrorsInDescription |
            WarningTypeKeywordsInDescription,

        WarningTypeTitleGroup = WarningTypeTitleIsEmpty |
            WarningTypeTitleNotEnoughWords |
            WarningTypeTitleTooManyWords |
            WarningTypeTitleTooBig |
            WarningTypeSpellErrorsInTitle |
            WarningTypeKeywordsInTitle,

        WarningTypeKeywordsGroup = WarningTypeNoKeywords |
            WarningTypeTooFewKeywords |
            WarningTypeTooManyKeywords |
            WarningTypeSpellErrorsInKeywords |
            WarningTypeKeywordsInDescription |
            WarningTypeKeywordsInTitle,

        WarningTypeSpellingGroup = WarningTypeSpellErrorsInKeywords |
            WarningTypeSpellErrorsInDescription |
            WarningTypeSpellErrorsInTitle
    };

    enum WarningsCheckFlags {
        WarningsCheckAll = 0,
        WarningsCheckKeywords = 1,
        WarningsCheckTitle = 2,
        WarningsCheckDescription = 3,
        WarningsCheckSpelling = 4
    };

    const char *warningsFlagToString(int flags);

    template<typename FlagType>
    bool HasFlag(int value, FlagType flag) {
        int intFlag = static_cast<int>(flag);
        bool result = (value & intFlag) == intFlag;
        return result;
    }

    template<typename FlagType>
    void SetFlag(int &value, FlagType flag) {
        value |= static_cast<int>(flag);
    }

    template<typename FlagType>
    void SetFlag(volatile int &value, FlagType flag) {
        value |= static_cast<int>(flag);
    }

    template<typename FlagType>
    void UnsetFlag(int &value, FlagType flag) {
        value &= ~(static_cast<int>(flag));
    }

    template<typename FlagType>
    void UnsetFlag(volatile int &value, FlagType flag) {
        value &= ~(static_cast<int>(flag));
    }

    template<typename FlagType>
    void ApplyFlag(int &value, bool applySwitch, FlagType flag) {
        if (applySwitch) {
            SetFlag(value, flag);
        } else {
            UnsetFlag(value, flag);
        }
    }

    template<typename FlagType>
    void ApplyFlag(volatile int &value, bool applySwitch, FlagType flag) {
        if (applySwitch) {
            SetFlag(value, flag);
        } else {
            UnsetFlag(value, flag);
        }
    }
}

#endif // FLAGS

