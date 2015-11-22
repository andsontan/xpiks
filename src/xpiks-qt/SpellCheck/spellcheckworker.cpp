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

#include "spellcheckworker.h"

#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QDir>
#include <QUrl>
#include <QCoreApplication>
#include "spellcheckitem.h"
#include "../hunspell/hunspell.hxx"

#define EN_HUNSPELL_DIC "en_US.dic"
#define EN_HUNSPELL_AFF "en_US.aff"

QString getHunspellResourcesPath() {
    QString path = QCoreApplication::applicationDirPath();

#ifdef Q_OS_MAC
    path += "/../Resources/";
#endif

    return path;
}

namespace SpellCheck {
    SpellCheckWorker::SpellCheckWorker(QObject *parent) :
        QObject(parent),
        m_Cancel(false)
    {
    }

    SpellCheckWorker::~SpellCheckWorker() {
        if (m_Hunspell != NULL) {
            delete m_Hunspell;
        }
    }

    void SpellCheckWorker::submitItemToCheck(SpellCheckItemBase *item) {
        m_QueueMutex.lock();
        {
            m_Queue.append(item);
        }
        m_QueueMutex.unlock();
        m_WaitAnyItem.wakeOne();
    }

    void SpellCheckWorker::submitItemsToCheck(const QList<SpellCheckItemBase *> &items) {
        m_QueueMutex.lock();
        {
            m_Queue.append(items);
            qDebug() << "Submitted" << items.count() << "items to spell check loop";
        }
        m_QueueMutex.unlock();
        m_WaitAnyItem.wakeOne();
    }

    void SpellCheckWorker::cancelCurrentBatch() {
        m_QueueMutex.lock();
        {
            qDeleteAll(m_Queue);
            m_Queue.clear();
        }
        m_QueueMutex.unlock();

        m_WaitAnyItem.wakeOne();
        emit queueIsEmpty();
    }

    void SpellCheckWorker::initHunspell() {
        QDir resourcesDir(getHunspellResourcesPath());
        QString affPath = resourcesDir.absoluteFilePath(EN_HUNSPELL_AFF);
        QString dicPath = resourcesDir.absoluteFilePath(EN_HUNSPELL_DIC);

        m_Hunspell = new Hunspell(affPath.toLocal8Bit().constData(),
                                  dicPath.toLocal8Bit().constData());
        detectAffEncoding();
    }

    void SpellCheckWorker::detectAffEncoding() {
        // detect encoding analyzing the SET option in the affix file
        QDir resourcesDir(getHunspellResourcesPath());
        QString affPath = resourcesDir.absoluteFilePath(EN_HUNSPELL_AFF);

        m_Encoding = "ISO8859-1";
        QFile affixFile(affPath);

        if (affixFile.open(QIODevice::ReadOnly)) {
            QTextStream stream(&affixFile);
            QRegExp encDetector("^\\s*SET\\s+([A-Z0-9\\-]+)\\s*", Qt::CaseInsensitive);

            for (QString line = stream.readLine(); !line.isEmpty(); line = stream.readLine()) {
                if (encDetector.indexIn(line) > -1) {
                    m_Encoding = encDetector.cap(1);
                    qDebug() << QString("Encoding of AFF set to ") + m_Encoding;
                    break;
                }
            }

            affixFile.close();
        }

        m_Codec = QTextCodec::codecForName(m_Encoding.toLatin1().constData());
    }

    bool SpellCheckWorker::hasPendingJobs() {
        QMutexLocker locker(&m_QueueMutex);
        bool isEmpty = m_Queue.isEmpty();
        return !isEmpty;
    }

    QStringList SpellCheckWorker::retrieveCorrections(const QString &word) {
        QReadLocker locker(&m_SuggestionsLock);
        QStringList result;

        if (m_Suggestions.contains(word)) {
            result = m_Suggestions.value(word);
        }

        return result;
    }

    void SpellCheckWorker::clearCorrections() {
        QWriteLocker locker(&m_SuggestionsLock);
        m_Suggestions.clear();
    }

    QStringList SpellCheckWorker::suggestCorrections(const QString &word) {
        QStringList suggestions;
        char **suggestWordList = NULL;

        try {
            // Encode from Unicode to the encoding used by current dictionary
            int count = m_Hunspell->suggest(&suggestWordList, m_Codec->fromUnicode(word).constData());

            for (int i = 0; i < count; ++i) {
                suggestions << m_Codec->toUnicode(suggestWordList[i]);
                free(suggestWordList[i]);
            }
        }
        catch (...) {
            qDebug() << "Error in suggestCorrections for keyword:" << word;
        }

        return suggestions;
    }

    void SpellCheckWorker::spellcheckLoop() {
        for (;;) {
            if (m_Cancel) {
                qDebug() << "SpellCheck loop cancelled. Exiting...";
                break;
            }

            bool noMoreItems = false;

            m_QueueMutex.lock();

            if (m_Queue.isEmpty()) {
                m_WaitAnyItem.wait(&m_QueueMutex);

                // can be cleared by clearCurrectRequests()
                if (m_Queue.isEmpty()) {
                    emit queueIsEmpty();
                    m_QueueMutex.unlock();
                    continue;
                }
            }

            SpellCheckItemBase *item = m_Queue.first();
            m_Queue.removeFirst();

            noMoreItems = m_Queue.isEmpty();

            m_QueueMutex.unlock();

            if (item == NULL) { break; }

            bool canDelete = true;

            try {
                canDelete = processOneRequest(item);
            }
            catch (...) {
                qDebug() << "Error while processing spellcheck";
            }

            if (canDelete) {
                delete item;
            }

            if (noMoreItems) {
                emit queueIsEmpty();
            }
        }
    }

    bool SpellCheckWorker::processOneRequest(SpellCheckItemBase *item) {
        Q_ASSERT(item != NULL);

        if (dynamic_cast<SpellCheckSeparatorItem*>(item)) {
            emit queueIsEmpty();
            return true;
        }

        bool neededSuggestions = item->needsSuggestions();
        const QList<SpellCheckQueryItem*> &queryItems = item->getQueries();
        bool anyWrong = false;

        if (!neededSuggestions) {
            foreach (SpellCheckQueryItem *queryItem, queryItems) {
                bool isOk = isWordSpelledOk(queryItem->m_Word);
                queryItem->m_IsCorrect = isOk;
                anyWrong = anyWrong || !isOk;
            }
        } else {
            foreach (SpellCheckQueryItem *queryItem, queryItems) {
                if (!queryItem->m_IsCorrect) {
                    findSuggestions(queryItem->m_Word);
                }
            }
        }

        bool canDelete = false;

        if (anyWrong && !neededSuggestions) {
            item->submitSpellCheckResult();
            item->requestSuggestions();
            this->submitItemToCheck(item);
        } else {
            canDelete = true;
        }

        return canDelete;
    }

    bool SpellCheckWorker::isWordSpelledOk(const QString &word) const {
        bool isOk = false;
        try {
            isOk = m_Hunspell->spell(m_Codec->fromUnicode(word).constData()) != 0;
        }
        catch (...) {
            isOk = false;
        }
        return isOk;
    }

    void SpellCheckWorker::findSuggestions(const QString &word) {
        bool needsCorrections = false;

        m_SuggestionsLock.lockForRead();
        needsCorrections = !m_Suggestions.contains(word);
        m_SuggestionsLock.unlock();

        if (needsCorrections) {
            QStringList suggestions = suggestCorrections(word);
            m_SuggestionsLock.lockForWrite();
            m_Suggestions.insert(word, suggestions);
            m_SuggestionsLock.unlock();
        }
    }

    void SpellCheckWorker::process() {
        initHunspell();
        spellcheckLoop();
    }

    void SpellCheckWorker::cancel() {
        m_Cancel = true;
        submitItemToCheck(NULL);
    }
}
