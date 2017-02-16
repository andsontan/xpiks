/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
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

#include <QString>
#include <QStringList>
#include <QReadWriteLock>
#include <QHash>
#include <QSet>
#include "../Common/itemprocessingworker.h"
#include "spellcheckitem.h"

class Hunspell;
class QTextCodec;

namespace SpellCheck {
    class SpellCheckWorker : public QObject, public Common::ItemProcessingWorker<ISpellCheckItem>
    {
        Q_OBJECT

    public:
        SpellCheckWorker(QObject *parent=0);
        virtual ~SpellCheckWorker();

    public:
        const QSet<QString> &getUserDictionary() const { return m_UserDictionary; }
        QStringList retrieveCorrections(const QString &word);
        int getUserDictionarySize() const { return m_UserDictionary.size(); }

    protected:
        virtual bool initWorker() override;
        virtual void processOneItem(std::shared_ptr<ISpellCheckItem> &item) override;

    private:
        void processSeparatorItem(std::shared_ptr<SpellCheckSeparatorItem> &item);
        void processQueryItem(std::shared_ptr<SpellCheckItem> &item);
        void processChangeUserDict(std::shared_ptr<ModifyUserDictItem> &item);

    protected:
        virtual void notifyQueueIsEmpty() override { emit queueIsEmpty(); }
        virtual void workerStopped() override { emit stopped(); }

    public slots:
        void process() { doWork(); }
        void cancel() { stopWorking(); }

    signals:
        void stopped();
        void queueIsEmpty();
        void wordsNumberChanged(int number);
        void userDictUpdate(const QStringList &keywords, bool overwritten);
        void userDictCleared();

#ifdef INTEGRATION_TESTS
    public:
        int getSuggestionsCount() const { return m_Suggestions.count(); }
#endif

    private:
        void detectAffEncoding();
        QStringList suggestCorrections(const QString &word);
        bool checkWordSpelling(const std::shared_ptr<SpellCheckQueryItem> &queryItem);
        bool checkWordSpelling(const QString &word);
        bool isHunspellSpellingCorrect(const QString &word) const;
        void findSuggestions(const QString &word);
        void initUserDictionary();
        void cleanUserDict();
        void changeUserDict(const QStringList &words, bool overwrite);
        void signalUserDictWordsCount();

    private:
        QHash<QString, QStringList> m_Suggestions;
        QSet<QString> m_WrongWords;
        QSet<QString> m_UserDictionary;
        QReadWriteLock m_SuggestionsLock;
        QString m_Encoding;
        Hunspell *m_Hunspell;
        // Coded does not need destruction
        QTextCodec *m_Codec;
        QString m_UserDictionaryPath;
    };
}

#endif // SPELLCHECKWORKER_H
