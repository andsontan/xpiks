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

#include "spellcheckworker.h"

#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QDir>
#include <QUrl>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QThread>
#include "../Helpers/appsettings.h"
#include "spellcheckitem.h"
#include "../Common/defines.h"
#include <hunspell/hunspell.hxx>

#define EN_HUNSPELL_DIC "en_US.dic"
#define EN_HUNSPELL_AFF "en_US.aff"

namespace SpellCheck {
    SpellCheckWorker::SpellCheckWorker(QObject *parent) :
        QObject(parent),
        m_Hunspell(NULL),
        m_Codec(NULL)
    {
    }

    SpellCheckWorker::~SpellCheckWorker() {
        if (m_Hunspell != NULL) {
            delete m_Hunspell;
        }

        qInfo() << "SpellCheckWorker::~SpellCheckWorker #" << "destroyed";
    }

    bool SpellCheckWorker::initWorker() {
        QString resourcesPath;
        QString affPath;
        QString dicPath;

#if !defined(Q_OS_LINUX)
        resourcesPath = QCoreApplication::applicationDirPath();

#if defined(Q_OS_MAC)
        resourcesPath += "/../Resources/";
#elif defined(Q_OS_WIN)
        resourcesPath += "/dict/";
#endif

        QDir resourcesDir(resourcesPath);
        affPath = resourcesDir.absoluteFilePath(EN_HUNSPELL_AFF);
        dicPath = resourcesDir.absoluteFilePath(EN_HUNSPELL_DIC);

#else
        Helpers::AppSettings appSettings;
        resourcesPath = appSettings.value(Constants::DICT_PATH, "").toString();
        if (resourcesPath.isEmpty()) {
            resourcesPath = "hunspell/";
            dicPath = QStandardPaths::locate(QStandardPaths::GenericDataLocation, resourcesPath + EN_HUNSPELL_DIC);
            affPath = QStandardPaths::locate(QStandardPaths::GenericDataLocation, resourcesPath + EN_HUNSPELL_AFF);
        } else {
            QDir resourcesDirectory(resourcesPath);
            affPath = resourcesDirectory.absoluteFilePath(EN_HUNSPELL_AFF);
            dicPath = resourcesDirectory.absoluteFilePath(EN_HUNSPELL_DIC);
        }
#endif

        bool initResult = false;

        if (QFile(affPath).exists() && QFile(dicPath).exists()) {
#ifdef Q_OS_WIN
            // specific Hunspell handling of UTF-8 encoded pathes
            affPath = "\\\\?\\" + QDir::toNativeSeparators(affPath);
            dicPath = "\\\\?\\" + QDir::toNativeSeparators(dicPath);
#endif

            try {
                m_Hunspell = new Hunspell(affPath.toUtf8().constData(),
                                          dicPath.toUtf8().constData());
                qDebug() << "SpellCheckWorker::initWorker #" << "Hunspell with AFF" << affPath << "and DIC" << dicPath;
                initResult = true;
                m_Encoding = m_Hunspell->get_dic_encoding();
                m_Codec = QTextCodec::codecForName(m_Encoding.toLatin1().constData());
            }
            catch(...) {
                qDebug() << "SpellCheckWorker::initWorker #" << "Error in Hunspell with AFF" << affPath << "and DIC" << dicPath;
            }
        } else {
            qWarning() << "SpellCheckWorker::initWorker #" << "DIC or AFF file not found." << dicPath << "||" << affPath;
        }

        return initResult;
    }

    bool SpellCheckWorker::processOneItem(SpellCheckItemBase *item) {
        Q_ASSERT(item != NULL);

        if (dynamic_cast<SpellCheckSeparatorItem*>(item)) {
            emit queueIsEmpty();
            return true;
        }

        bool neededSuggestions = item->needsSuggestions();
        const QVector<SpellCheckQueryItem*> &queryItems = item->getQueries();
        bool anyWrong = false;

        if (!neededSuggestions) {
            int length = queryItems.length();
            for (int i = 0; i < length; ++i) {
                SpellCheckQueryItem *queryItem = queryItems.at(i);
                bool isOk = isWordSpelledOk(queryItem->m_Word);
                queryItem->m_IsCorrect = isOk;
                item->accountResultAt(i);
                anyWrong = anyWrong || !isOk;
            }

            item->submitSpellCheckResult();
        } else {
            foreach (SpellCheckQueryItem *queryItem, queryItems) {
                if (!queryItem->m_IsCorrect) {
                    findSuggestions(queryItem->m_Word);
                }
            }
        }

        bool canDelete = false;

        if (anyWrong) {
            item->requestSuggestions();
            this->submitItem(item);
        } else {
            canDelete = true;
        }

        return canDelete;
    }

    QStringList SpellCheckWorker::retrieveCorrections(const QString &word) {
        QReadLocker locker(&m_SuggestionsLock);
        QStringList result;

        if (m_Suggestions.contains(word)) {
            result = m_Suggestions.value(word);
        }

        return result;
    }

    QStringList SpellCheckWorker::suggestCorrections(const QString &word) {
        QStringList suggestions;
        char **suggestWordList = NULL;

        try {
            // Encode from Unicode to the encoding used by current dictionary
            int count = m_Hunspell->suggest(&suggestWordList, m_Codec->fromUnicode(word).constData());
            QString lowerWord = word.toLower();

            for (int i = 0; i < count; ++i) {
                QString suggestion = m_Codec->toUnicode(suggestWordList[i]);

                if (suggestion.toLower() != lowerWord) {
                    suggestions << suggestion;
                }

                free(suggestWordList[i]);
            }
        }
        catch (...) {
            qWarning() << "SpellCheckWorker::suggestCorrections #" << "Error for keyword:" << word;
        }

        return suggestions;
    }

    bool SpellCheckWorker::isWordSpelledOk(const QString &word) const {
        bool isOk = isHunspellSpellingCorrect(word);

        if (!isOk) {
            QString capitalized = word;
            capitalized[0] = capitalized[0].toUpper();

            if (isHunspellSpellingCorrect(capitalized)) {
                isOk = true;
            }
        }

        return isOk;
    }

    bool SpellCheckWorker::isHunspellSpellingCorrect(const QString &word) const {
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
}
