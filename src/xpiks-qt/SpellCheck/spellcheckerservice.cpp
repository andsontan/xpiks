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

#include "spellcheckerservice.h"
#include "spellcheckworker.h"
#include <QThread>

namespace SpellCheck {
    SpellCheckerService::SpellCheckerService() {
        m_SpellCheckWorker = new SpellCheckWorker();
    }

    void SpellCheckerService::startWorker() {
        QThread *thread = new QThread();
        m_SpellCheckWorker->moveToThread(thread);

        QObject::connect(thread, SIGNAL(started()), m_SpellCheckWorker, SLOT(process()));
        QObject::connect(m_SpellCheckWorker, SIGNAL(stopped()), thread, SLOT(quit()));

        QObject::connect(m_SpellCheckWorker, SIGNAL(stopped()), m_SpellCheckWorker, SLOT(deleteLater()));
        QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

        QObject::connect(this, SIGNAL(cancelSpellChecking()),
                         m_SpellCheckWorker, SLOT(cancel()));

        thread->start();
    }
}
