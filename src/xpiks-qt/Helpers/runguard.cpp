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

#include "runguard.h"
#include <QCryptographicHash>

namespace Helpers {
    QString generateKeyHash( const QString& key, const QString& salt )
    {
        QByteArray data;

        data.append( key.toUtf8() );
        data.append( salt.toUtf8() );
        data = QCryptographicHash::hash( data, QCryptographicHash::Sha1 ).toHex();

        return data;
    }

    RunGuard::RunGuard( const QString& key )
        : m_Key( key )
        , m_MemLockKey( generateKeyHash( key, "_memLockKey" ) )
        , m_SharedMemKey( generateKeyHash( key, "_sharedmemKey" ) )
        , m_SharedMem( m_SharedMemKey )
        , m_MemLock( m_MemLockKey, 1 )
    {
        QSharedMemory fix( m_SharedMemKey );    // Fix for *nix: http://habrahabr.ru/post/173281/
        fix.attach();
    }

    RunGuard::~RunGuard()
    {
        release();
    }

    bool RunGuard::isAnotherRunning()
    {
        if ( m_SharedMem.isAttached() )
            return false;

        m_MemLock.acquire();
        const bool isRunning = m_SharedMem.attach();
        if ( isRunning )
            m_SharedMem.detach();
        m_MemLock.release();

        return isRunning;
    }

    bool RunGuard::tryToRun()
    {
        if ( isAnotherRunning() )   // Extra check
            return false;

        m_MemLock.acquire();
        const bool result = m_SharedMem.create( sizeof( quint64 ) );
        m_MemLock.release();
        if ( !result )
        {
            release();
            return false;
        }

        return true;
    }

    void RunGuard::release()
    {
        m_MemLock.acquire();
        if ( m_SharedMem.isAttached() )
            m_SharedMem.detach();
        m_MemLock.release();
    }
}

