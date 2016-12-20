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

#ifndef HOLD_H
#define HOLD_H

#include <QAtomicInt>

namespace Common {
    class Hold {
    public:
        Hold(): m_RefCount(1)
        {}

    public:
        virtual void acquire() { m_RefCount.fetchAndAddOrdered(1); }
        virtual bool release() { return m_RefCount.fetchAndSubOrdered(1) == 1; }

    private:
        QAtomicInt m_RefCount;
    };

    class FakeHold: public Hold {
    public:
        virtual void acquire() { Q_ASSERT(false); }
        virtual bool release() { Q_ASSERT(false); return false; }
    };
}

#endif // HOLD_H
