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

#ifndef SPELLCHECKERSERVICE_H
#define SPELLCHECKERSERVICE_H

#include <QObject>
#include <QString>
#include <QVector>
#include "../Common/basickeywordsmodel.h"
#include "../Common/iservicebase.h"
#include "../Common/flags.h"
#include "../Models/settingsmodel.h"

namespace Models {
    class ArtworkMetadata;
}

namespace SpellCheck {
    class SpellCheckWorker;

    class SpellCheckerService:
        public QObject,
        public Common::IServiceBase<Common::BasicKeywordsModel, Common::SpellCheckFlags>
    {
    Q_OBJECT
    Q_PROPERTY(int userDictWordsNumber READ getUserDictWordsNumber NOTIFY userDictWordsNumberChanged)

    public:
        SpellCheckerService(Models::SettingsModel *settingsModel = 0);

        virtual ~SpellCheckerService();

    public:
        virtual void startService() override;
        virtual void stopService() override;

        virtual bool isAvailable() const override { return true; }
        virtual bool isBusy() const override;

        virtual void submitItem(Common::BasicKeywordsModel *itemToCheck) override;
        virtual void submitItem(Common::BasicKeywordsModel *itemToCheck, Common::SpellCheckFlags flags) override;
        virtual void submitItems(const QVector<Common::BasicKeywordsModel *> &itemsToCheck) override;
        void submitItems(const QVector<Common::BasicKeywordsModel *> &itemsToCheck, const QStringList &wordsToCheck);
        void submitKeyword(Common::BasicKeywordsModel *itemToCheck, int keywordIndex);
        virtual QStringList suggestCorrections(const QString &word) const;
        void restartWorker();
        int getUserDictWordsNumber();

#ifdef INTEGRATION_TESTS
    public:
        int getSuggestionsCount();
#endif

    public:
        QStringList getUserDictionary() const;
        void updateUserDictionary(const QStringList &words);

    public:
        Q_INVOKABLE void cancelCurrentBatch();
        Q_INVOKABLE bool hasAnyPending();
        Q_INVOKABLE void addWordToUserDictionary(const QString &word);
        Q_INVOKABLE void clearUserDictionary();

    signals:
        void cancelSpellChecking();
        void spellCheckQueueIsEmpty();
        void serviceAvailable(bool afterRestart);
        void userDictWordsNumberChanged();
        void userDictUpdate(const QStringList &keywords, bool overwritten);
        void userDictCleared();

    private slots:
        void workerFinished();
        void workerDestroyed(QObject *object);
        void wordsNumberChangedHandler(int number);

    private:
        SpellCheckWorker *m_SpellCheckWorker;
        Models::SettingsModel *m_SettingsModel;
        volatile bool m_RestartRequired;
        QString m_DictionariesPath;
    };
}

#endif // SPELLCHECKERSERVICE_H
