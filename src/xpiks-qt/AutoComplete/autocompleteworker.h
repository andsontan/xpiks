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

#ifndef AUTOCOMPLETEWORKER_H
#define AUTOCOMPLETEWORKER_H

#include <QObject>
#include "../Common/itemprocessingworker.h"
#include "completionquery.h"
#include <src/libfaceapi.hpp>

namespace AutoComplete {
    class AutoCompleteWorker :
            public QObject,
            public Common::ItemProcessingWorker<CompletionQuery>
    {
        Q_OBJECT
    public:
        explicit AutoCompleteWorker(QObject *parent = 0);
        virtual ~AutoCompleteWorker();

    protected:
        virtual bool initWorker();
        virtual bool processOneItem(CompletionQuery *item);

    protected:
        virtual void notifyQueueIsEmpty() { emit queueIsEmpty(); }
        virtual void notifyStopped() { emit stopped(); }

    public slots:
        void process() { doWork(); }
        void cancel() { stopWorking(); }

    signals:
        void stopped();
        void queueIsEmpty();

    private:
        Souffleur *m_Soufleur;
        const int m_CompletionsCount;
    };
}

#endif // AUTOCOMPLETEWORKER_H
