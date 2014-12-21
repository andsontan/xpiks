/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014 Taras Kushnir <kushnirTV@gmail.com>
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

#ifndef FTPCREDENTIALS
#define FTPCREDENTIALS

#include <QString>

namespace Models {
    class FtpCredentials {
    public:
        FtpCredentials() {}

    public:
        const QString &getHost() const { return m_Host; }
        const QString &getUsername() const { return m_Username; }
        const QString &getPassword() const { return m_Password; }

    public:
        void setHost(const QString &value) { m_Host = value; }
        void setUsername(const QString &value) { m_Username = value; }
        void setPassword(const QString &value) { m_Password = value; }

    private:
        QString m_Host;
        QString m_Username;
        QString m_Password;
    };
}

#endif // FTPCREDENTIALS

