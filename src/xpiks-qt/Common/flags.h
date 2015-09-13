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
        AppendKeywords = 8
    };

    bool HasFlag(int value, CombinedEditFlags flag);
    void SetFlag(int &value, CombinedEditFlags flag);
    void UnsetFlag(int &value, CombinedEditFlags flag);
    void ApplyFlag(int &value, bool applySwitch, CombinedEditFlags flag);
}

#endif // FLAGS

