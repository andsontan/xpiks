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
    SpellCheckWorker::SpellCheckWorker(QObject *parent):
        QObject(parent),
        m_Hunspell(NULL),
        m_Codec(NULL),
        m_UserDictionary(""),
        m_UserDictionaryWordsNumber(0)
    {}

    SpellCheckWorker::~SpellCheckWorker() {
        if (m_Hunspell != NULL) {
            delete m_Hunspell;
        }

        LOG_INFO << "destroyed";
    }

    bool SpellCheckWorker::initWorker() {
        LOG_INFO << "#";

        QString resourcesPath;
        QString affPath;
        QString dicPath;

#if !defined(Q_OS_LINUX)
        resourcesPath = QCoreApplication::applicationDirPath();
        qDebug() << "App path:" << resourcesPath;

#if defined(Q_OS_MAC)
        resourcesPath += "/../Resources/";
#elif defined(APPVEYOR)
        resourcesPath += "/../../../xpiks-qt/deps/dict/";
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

        if (QFileInfo(affPath).exists() && QFileInfo(dicPath).exists()) {
#ifdef Q_OS_WIN
            // specific Hunspell handling of UTF-8 encoded pathes
            affPath = "\\\\?\\" + QDir::toNativeSeparators(affPath);
            dicPath = "\\\\?\\" + QDir::toNativeSeparators(dicPath);
#endif

            try {
                m_Hunspell = new Hunspell(affPath.toUtf8().constData(),
                                          dicPath.toUtf8().constData());
                LOG_DEBUG << "Hunspell with AFF" << affPath << "and DIC" << dicPath;
                initResult = true;
                m_Encoding = QString::fromLatin1(m_Hunspell->get_dic_encoding());
                m_Codec = QTextCodec::codecForName(m_Encoding.toLatin1().constData());
            } catch (...) {
                LOG_DEBUG << "Error in Hunspell with AFF" << affPath << "and DIC" << dicPath;
                m_Hunspell = NULL;
            }
        } else {
            LOG_WARNING << "DIC or AFF file not found." << dicPath << "||" << affPath;
        }

        initFromUserDict();

        return initResult;
    }

    void SpellCheckWorker::processOneItem(std::shared_ptr<ISpellCheckItem> &item) {
        auto separatorItem = std::dynamic_pointer_cast<SpellCheckSeparatorItem>(item);
        auto queryItem = std::dynamic_pointer_cast<SpellCheckItem>(item);
        auto addWordItem = std::dynamic_pointer_cast<AddWordItem>(item);

        if (queryItem) {
            processQueryItem(queryItem);
        } else if (separatorItem) {
            processSeparatorItem(separatorItem);
        } else if (addWordItem) {
            processChangeUserDict(addWordItem);
        }
    }

    void SpellCheckWorker::processSeparatorItem(std::shared_ptr<SpellCheckSeparatorItem> &item) {
        Q_UNUSED(item);
        emit queueIsEmpty();
    }

    void SpellCheckWorker::processQueryItem(std::shared_ptr<SpellCheckItem> &item) {
        bool neededSuggestions = item->needsSuggestions();
        auto &queryItems = item->getQueries();
        bool anyWrong = false;

        if (!neededSuggestions) {
            size_t size = queryItems.size();
            for (size_t i = 0; i < size; ++i) {
                auto &queryItem = queryItems.at(i);
                bool isOk = checkWordSpelling(queryItem);
                item->accountResultAt((int)i);
                anyWrong = anyWrong || !isOk;
            }

            item->submitSpellCheckResult();
        } else {
            for (auto &queryItem: queryItems) {
                if (!queryItem->m_IsCorrect) {
                    findSuggestions(queryItem->m_Word);
                }
            }
        }

        if (anyWrong) {
            item->requestSuggestions();
            this->submitItem(item);
        }
    }

    void SpellCheckWorker::processChangeUserDict(std::shared_ptr<AddWordItem> &item) {
        if (m_UserDictionary.isEmpty()) {
            qWarning() << "User dictionary not set.";
            return;
        }

        if (item->getClearFlag()) {
            cleanUserDict();
        } else {
            QStringList words = item->getKeywords();
            addWordUserDict(words);
        }

        emit wordsNumberChanged(m_UserDictionaryWordsNumber);
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
        } catch (...) {
            LOG_WARNING << "Error for keyword:" << word;
        }
        return suggestions;
    }

    bool SpellCheckWorker::checkWordSpelling(const std::shared_ptr<SpellCheckQueryItem> &queryItem) {
        bool isOk = false;

        const QString &word = queryItem->m_Word;
        bool isCached = m_WrongWords.contains(word);
        bool isCachedUser = m_UserWords.contains(word);

        isOk = isCachedUser;

        if (!isCached && !isCachedUser) {
            isOk = isHunspellSpellingCorrect(word);

            if (!isOk) {
                QString capitalized = word;
                capitalized[0] = capitalized[0].toUpper();

                if (isHunspellSpellingCorrect(capitalized)) {
                    isOk = true;
                }
            }
        }

        queryItem->m_IsCorrect = isOk;

        if (!isOk) {
            m_WrongWords.insert(word);
        }

        return isOk;
    }

    bool SpellCheckWorker::isHunspellSpellingCorrect(const QString &word) const {
        bool isOk = false;

        try {
            isOk = m_Hunspell->spell(m_Codec->fromUnicode(word).constData()) != 0;
        } catch (...) {
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

    void SpellCheckWorker::initFromUserDict() {
        LOG_DEBUG << "#";
        QString appDataPath = XPIKS_USERDATA_PATH;
        QDir dir(appDataPath);

        m_UserDictionary = dir.filePath(QString(Constants::USER_DICT_FILENAME));
        QFile userDictonaryFile(m_UserDictionary);

        if (userDictonaryFile.open(QIODevice::ReadOnly)) {
            QTextStream stream(&userDictonaryFile);
            for (QString word = stream.readLine(); !word.isEmpty(); word = stream.readLine()) {
                m_UserWords.insert(word);
            }

            m_UserDictionaryWordsNumber = m_UserWords.size();
            emit wordsNumberChanged(m_UserDictionaryWordsNumber);
        } else {
            LOG_WARNING << "User dictionary in " << m_UserDictionary << "could not be opened";
        }
    }


    void SpellCheckWorker::cleanUserDict() {
        QFile userDictonaryFile(m_UserDictionary);

        if (!userDictonaryFile.open(QIODevice::ReadWrite)) {
            qWarning() << "Unable to open user dictionary";
            return;
        }

        qInfo() <<  "Cleaning user dictionary";
        m_UserWords.clear();
        m_UserDictionaryWordsNumber = 0;
        userDictonaryFile.resize(0);
        emit userDictUpdate();
    }

    void SpellCheckWorker::addWordUserDict(const QStringList &words) {
        QFile userDictonaryFile(m_UserDictionary);

        if (!userDictonaryFile.open(QIODevice::Append)) {
            qWarning() << "Unable to open user dictionary";
            return;
        }

        QTextStream stream(&userDictonaryFile);
        QSet<QString> newWords;
        for (const QString &word: words) {
            bool isCached = m_WrongWords.contains(word);
            if (!isCached) {
                continue;
            }

            LOG_INFO << "adding word "<<word<<" to dictionary";
            if (!m_WrongWords.contains(word)) {
                continue;
            }

            newWords.insert(word);
            stream << word << endl;
            m_UserDictionaryWordsNumber++;
        }

        m_UserWords.unite(newWords);
        emit userDictUpdate(newWords.toList());
    }

}
