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

#ifndef SPELLCHECKWORKER_H
#define SPELLCHECKWORKER_H

#include <QObject>
#include <QList>
#include <QWaitCondition>
#include <QMutex>
#include <QString>
#include <QStringList>

class Hunspell;
class QTextCodec;

namespace SpellCheck {
    class SpellCheckItem;

    class SpellCheckWorker : public QObject
    {
        Q_OBJECT
    public:
        explicit SpellCheckWorker(QObject *parent = 0);
        virtual ~SpellCheckWorker();

    public:
        void submitItemToCheck(SpellCheckItem *item);
        void submitItemsToCheck(const QList<SpellCheckItem*> &items);
        void cancelCurrentBatch();
        QStringList suggestCorrections(const QString &word);
        bool hasPendingJobs();

    private:
        void initHunspell();
        void detectAffEncoding();
        void spellcheckLoop();
        void processOneRequest(const SpellCheckItem *item);
        bool isWordSpelledOk(const QString &word) const;

    public slots:
        void process();
        void cancel();

    signals:
        void stopped();
        void queueIsEmpty();

    private:
        QWaitCondition m_WaitAnyItem;
        QMutex m_Mutex;
        QList<SpellCheckItem*> m_Queue;
        QString m_Encoding;
        QTextCodec *m_Codec;
        Hunspell *m_Hunspell;
        volatile bool m_Cancel;
    };
}

#endif // SPELLCHECKWORKER_H
