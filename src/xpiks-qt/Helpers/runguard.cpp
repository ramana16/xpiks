/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "runguard.h"
#include <QCryptographicHash>

namespace Helpers {
    QString generateKeyHash( const QString &key, const QString &salt )
    {
        QByteArray data;

        data.append( key.toUtf8() );
        data.append( salt.toUtf8() );
        data = QCryptographicHash::hash( data, QCryptographicHash::Sha1 ).toHex();

        return QString::fromLatin1(data);
    }

    RunGuard::RunGuard( const QString &key )
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

