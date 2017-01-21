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

#ifndef TRANSLATIONSERVICE_H
#define TRANSLATIONSERVICE_H

#include <QObject>
#include "../Common/iservicebase.h"

namespace Translation {
    class TranslationWorker;

    class TranslationService :
            public QObject,
            public Common::IServiceBase<QString>
    {
        Q_OBJECT
    public:
        explicit TranslationService(QObject *parent = 0);
        virtual ~TranslationService();

        virtual void startService();
        virtual void stopService();

        virtual bool isAvailable() const { return true; }
        virtual bool isBusy() const;

    protected:
        virtual void submitItem(QString *item);
        virtual void submitItem(QString *item, int flags);
        virtual void submitItems(const QVector<QString*> &items);

    public:
        void selectDictionary(const QString &dictionaryPath);

    private slots:
        void workerFinished();
        void workerDestroyed(QObject* object);

    private:
        TranslationWorker *m_TranslationWorker;
        volatile bool m_RestartRequired;
    };
}

#endif // TRANSLATIONSERVICE_H
