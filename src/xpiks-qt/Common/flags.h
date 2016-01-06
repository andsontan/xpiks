/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2015 Taras Kushnir <kushnirTV@gmail.com>
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
        Clear = 16
    };

    enum SuggestCorrectionsFlags {
        CorrectTitle = 1,
        CorrectDescription = 2,
        CorrectKeywords = 4
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
        WarningTypeSpellErrorsInTitle = 1 << 13
    };

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
    void UnsetFlag(int &value, FlagType flag) {
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
}

#endif // FLAGS

