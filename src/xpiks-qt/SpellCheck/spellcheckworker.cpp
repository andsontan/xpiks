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
    SpellCheckWorker::SpellCheckWorker(QObject *parent) : QObject(parent) {
    }

    SpellCheckWorker::~SpellCheckWorker() {
        if (m_Hunspell != NULL) {
            delete m_Hunspell;
        }
    }

    void SpellCheckWorker::submitItemToCheck(SpellCheckItem *item) {
        m_Mutex.lock();
        {
            m_Queue.append(item);
        }
        m_Mutex.unlock();
        m_WaitAnyItem.wakeOne();
    }

    void SpellCheckWorker::submitItemsToCheck(const QList<SpellCheckItem *> &items) {
        m_Mutex.lock();
        {
            m_Queue.append(items);
            qDebug() << "Submitted" << items.count() << "items to spell check loop";
        }
        m_Mutex.unlock();
        m_WaitAnyItem.wakeOne();
    }

    void SpellCheckWorker::cancelCurrentBatch() {
        m_Mutex.lock();
        {
            qDeleteAll(m_Queue);
            m_Queue.clear();
        }
        m_Mutex.unlock();
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

    QStringList SpellCheckWorker::suggestCorrections(const QString &word) {
        QStringList suggestions;
        char **suggestWordList;

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
                break;
            }

            bool noMoreItems = false;

            m_Mutex.lock();

            if (m_Queue.isEmpty()) {
                m_WaitAnyItem.wait(&m_Mutex);

                // can be cleared by clearCurrectRequests()
                if (m_Queue.isEmpty()) {
                    m_Mutex.unlock();
                    continue;
                }
            }

            SpellCheckItem *item = m_Queue.first();
            m_Queue.removeFirst();

            noMoreItems = m_Queue.isEmpty();

            m_Mutex.unlock();

            if (item == NULL) { break; }

            try {
                processOneRequest(item);
            }
            catch (...) {
                qDebug() << "Error while processing spellcheck";
            }

            if (noMoreItems) {
                emit queueIsEmpty();
            }

            delete item;
        }
    }

    void SpellCheckWorker::processOneRequest(const SpellCheckItem *item) {
        Q_ASSERT(item != NULL);

        const QList<SpellCheckQueryItem*> &queryItems = item->getQueries();
        foreach (SpellCheckQueryItem *queryItem, queryItems) {
            queryItem->m_CheckResult = isWordSpelledOk(queryItem->m_Keyword);
        }

        item->submitSpellCheckResult();
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

    void SpellCheckWorker::process() {
        initHunspell();
        spellcheckLoop();
    }

    void SpellCheckWorker::cancel() {
        m_Cancel = true;
        submitItemToCheck(NULL);
    }
}
