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

#ifndef SPELLCHECKERSERVICE_H
#define SPELLCHECKERSERVICE_H

#include <QObject>
#include <QString>
#include <QVector>
#include "../Common/basickeywordsmodel.h"
#include "../Common/iservicebase.h"

namespace Models {
    class ArtworkMetadata;
}

namespace SpellCheck {
    class SpellCheckWorker;

    class SpellCheckerService :
            public QObject,
            public Common::IServiceBase<Common::BasicKeywordsModel>
    {
        Q_OBJECT
    public:
        SpellCheckerService();
        virtual ~SpellCheckerService();

    public:
        virtual void startService();
        virtual void stopService();

        virtual bool isAvailable() const { return true; }
        virtual bool isBusy() const;

        virtual void submitItem(Common::BasicKeywordsModel *itemToCheck);
        virtual void submitItem(Common::BasicKeywordsModel *itemToCheck, int flags);
        virtual void submitItems(const QVector<Common::BasicKeywordsModel *> &itemsToCheck);
        void submitItems(const QVector<Common::BasicKeywordsModel *> &itemsToCheck, const QStringList &wordsToCheck);
        void submitKeyword(Common::BasicKeywordsModel *itemToCheck, int keywordIndex);
        virtual QStringList suggestCorrections(const QString &word) const;
        void restartWorker();

    public:
        Q_INVOKABLE void cancelCurrentBatch();
        Q_INVOKABLE bool hasAnyPending();

    signals:
        void cancelSpellChecking();
        void spellCheckQueueIsEmpty();
        void serviceAvailable(bool afterRestart);

    private slots:
        void workerFinished();
        void workerDestroyed(QObject* object);

    private:
        SpellCheckWorker *m_SpellCheckWorker;
        volatile bool m_RestartRequired;
        QString m_DictionariesPath;
    };
}

#endif // SPELLCHECKERSERVICE_H
