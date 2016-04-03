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

#ifndef AUTOCOMPLETESERVICE_H
#define AUTOCOMPLETESERVICE_H

#include <QObject>
#include <QString>
#include "../Common/iservicebase.h"

namespace AutoComplete {
    class AutoCompleteWorker;
    class AutoCompleteModel;

    class AutoCompleteService:
            public QObject,
            public Common::IServiceBase<QString>
    {
        Q_OBJECT
    public:
        AutoCompleteService(AutoCompleteModel *autoCompleteModel, QObject *parent = 0);
        virtual ~AutoCompleteService();

        virtual void startService();
        virtual void stopService();

        virtual bool isAvailable() const { return true; }

    protected:
        virtual void submitItem(QString *item);
        virtual void submitItem(QString *item, int flags);
        virtual void submitItems(const QVector<QString*> &items);

    public:
        void restartWorker();

        // copy-by-value on purpose
        void findKeywordCompletions(const QString &prefix, QObject *notifyObject);

    private slots:
        void workerFinished();
        void workerDestroyed(QObject* object);

    signals:
        void cancelAutoCompletion();
        void serviceAvailable(bool afterRestart);

    private:
        AutoCompleteWorker *m_AutoCompleteWorker;
        AutoCompleteModel *m_AutoCompleteModel;
        volatile bool m_RestartRequired;
    };
}

#endif // AUTOCOMPLETESERVICE_H
