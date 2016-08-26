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
    enum struct CombinedEditFlags {
        EditTitle = 1 << 0,
        EditDesctiption = 1 << 1,
        EditKeywords = 1 << 2,
        AppendKeywords = 1 << 3,
        Clear = 1 << 4,
        EditEverything = EditTitle | EditDesctiption | EditKeywords
    };

    enum struct SuggestionFlags {
        None = 0,
        Title = 1 << 0,
        Description = 1 << 1,
        Keywords = 1 << 2,
        All = Title | Description | Keywords
    };

    enum struct SpellCheckFlags {
        Title = 1 << 0,
        Description = 1 << 1,
        Keywords = 1 << 2,
        All = Title | Description | Keywords
    };

    enum struct KeywordReplaceResult {
        Succeeded = 0,
        FailedIndex = 1,
        FailedDuplicate = 2,
        Unknown = 1 << 20
    };

    enum struct SearchFlags: int {
        None = 0,
        CaseSensitive = 1 << 0,
        Description = 1 << 1,
        Title = 1 << 2,
        Keywords = 1 << 3,
        ReservedTerms = 1 << 4, // include reserved terms like "x:empty"
        AllTerms = 1 << 5, // all of the search terms included in the result
        Filepath = 1 << 6,
        ExactMatch = 1 << 7,

        Metadata = Description | Title | Keywords,
        ExactKeywords = ExactMatch | Keywords,
        MetadataCaseSensitive = Metadata | CaseSensitive,

        Everything = Metadata | Filepath | ReservedTerms,
        AllTermsEverything = Everything | AllTerms,
        AnyTermsEverything = Everything
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
    bool HasFlag(FlagType value, FlagType flag) {
        int intValue = static_cast<int>(value);
        int intFlag = static_cast<int>(flag);
        bool result = (intValue & intFlag) == intFlag;
        return result;
    }

    template<typename FlagType>
    void SetFlag(int &value, FlagType flag) {
        value |= static_cast<int>(flag);
    }

    template<typename FlagType>
    void SetFlag(FlagType &value, FlagType flag) {
        value = static_cast<FlagType>(static_cast<int>(value) | static_cast<int>(flag));
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
    void UnsetFlag(FlagType &value, FlagType flag) {
        value = static_cast<FlagType>(static_cast<int>(value) & (~(static_cast<int>(flag))));
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
    void ApplyFlag(FlagType &value, bool applySwitch, FlagType flag) {
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

