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

#ifndef ISERVICEBASE_H
#define ISERVICEBASE_H

#include <QVector>

namespace Common {
    template<typename T, typename FlagsType=int>
    class IServiceBase {
    public:
        virtual ~IServiceBase() {}

        virtual void startService() = 0;
        virtual void stopService() = 0;

        // if service is provided via plugin it can be turned off
        virtual bool isAvailable() const = 0;

        virtual bool isBusy() const = 0;

        virtual void submitItem(T *item) = 0;
        virtual void submitItem(T *item, FlagsType flags) = 0;
        virtual void submitItems(const QVector<T*> &items) = 0;
    };
}

#endif // ISERVICEBASE_H
