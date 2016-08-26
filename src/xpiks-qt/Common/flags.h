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

#include <type_traits>
#include <QObject>

namespace Common {
    template<typename FlagType>
    struct enable_bitmask_operators {
        static constexpr bool enable=false;
    };

    template<typename FlagType>
    typename std::enable_if<enable_bitmask_operators<FlagType>::enable, FlagType>::type
    operator|(FlagType a, FlagType b) {
        typedef typename std::underlying_type<FlagType>::type underlying;
        return static_cast<FlagType>(static_cast<underlying>(a) | static_cast<underlying>(b));
    }

    template<typename FlagType>
    typename std::enable_if<enable_bitmask_operators<FlagType>::enable, FlagType>::type
    &operator|=(FlagType &a, FlagType b) {
        typedef typename std::underlying_type<FlagType>::type underlying;
        a = static_cast<FlagType>(static_cast<underlying>(a) | static_cast<underlying>(b));
        return a;
    }

    enum struct CombinedEditFlags: int {
        None = 0,
        EditTitle = 1 << 0,
        EditDesctiption = 1 << 1,
        EditKeywords = 1 << 2,
        AppendKeywords = 1 << 3,
        Clear = 1 << 4,
        EditEverything = EditTitle | EditDesctiption | EditKeywords
    };

    enum struct SuggestionFlags: int {
        None = 0,
        Title = 1 << 0,
        Description = 1 << 1,
        Keywords = 1 << 2,
        All = Title | Description | Keywords
    };

    enum struct SpellCheckFlags: int {
        Title = 1 << 0,
        Description = 1 << 1,
        Keywords = 1 << 2,
        All = Title | Description | Keywords
    };

    enum struct KeywordReplaceResult: int {
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

#ifdef CORE_TESTS
    template<>
    struct enable_bitmask_operators<SearchFlags> {
        static constexpr bool enable = true;
    };

    template<>
    struct enable_bitmask_operators<CombinedEditFlags> {
        static constexpr bool enable = true;
    };
#endif

    enum struct WarningFlags: int {
        None = 0,
        SizeLessThanMinimum = 1 << 0,
        NoKeywords = 1 << 1,
        TooFewKeywords = 1 << 2,
        TooManyKeywords = 1 << 3,
        DescriptionIsEmpty = 1 << 4,
        DescriptionNotEnoughWords = 1 << 5,
        DescriptionTooBig = 1 << 6,
        TitleIsEmpty = 1 << 7,
        TitleNotEnoughWords = 1 << 8,
        TitleTooManyWords = 1 << 9,
        TitleTooBig = 1 << 10,
        SpellErrorsInKeywords = 1 << 11,
        SpellErrorsInDescription = 1 << 12,
        SpellErrorsInTitle = 1 << 13,
        FileIsTooBig = 1 << 14,
        KeywordsInDescription = 1 << 15,
        KeywordsInTitle = 1 << 16,
        FilenameSymbols = 1 << 17,

        DescriptionGroup = DescriptionIsEmpty |
            DescriptionNotEnoughWords |
            DescriptionTooBig |
            SpellErrorsInDescription |
            KeywordsInDescription,

        TitleGroup = TitleIsEmpty |
            TitleNotEnoughWords |
            TitleTooManyWords |
            TitleTooBig |
            SpellErrorsInTitle |
            KeywordsInTitle,

        KeywordsGroup = NoKeywords |
            TooFewKeywords |
            TooManyKeywords |
            SpellErrorsInKeywords |
            KeywordsInDescription |
            KeywordsInTitle,

        SpellingGroup = SpellErrorsInKeywords |
            SpellErrorsInDescription |
            SpellErrorsInTitle
    };

    template<>
    struct enable_bitmask_operators<WarningFlags> {
        static constexpr bool enable = true;
    };

    enum struct WarningsCheckFlags {
        All = 0,
        Keywords = 1,
        Title = 2,
        Description = 3,
        Spelling = 4
    };

    const char *warningsFlagToString(WarningsCheckFlags flags);

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
    void SetFlag(volatile FlagType &value, FlagType flag) {
        value = static_cast<FlagType>(static_cast<int>(value) | static_cast<int>(flag));
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
    void UnsetFlag(volatile FlagType &value, FlagType flag) {
        value = static_cast<FlagType>(static_cast<int>(value) & (~(static_cast<int>(flag))));
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

Q_DECLARE_METATYPE(Common::SpellCheckFlags)

#endif // FLAGS

