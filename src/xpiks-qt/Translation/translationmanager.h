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

#ifndef TRANSLATIONMANAGER_H
#define TRANSLATIONMANAGER_H

#include <QObject>
#include <QString>
#include <QTimer>
#include <QVector>
#include "../Common/baseentity.h"

namespace Translation {
    struct DictionaryInfo {
        QString m_FullIfoPath;
        QString m_Description;
    };

    class TranslationManager :
            public QObject,
            public Common::BaseEntity
    {
        Q_OBJECT
        Q_PROPERTY(bool isBusy READ getIsBusy WRITE setIsBusy NOTIFY isBusyChanged)
        Q_PROPERTY(QString fullTranslation READ getFullTranslation NOTIFY fullTranslationChanged)
        Q_PROPERTY(QString shortTranslation READ getShortTranslation NOTIFY shortTranslationChanged)
        Q_PROPERTY(int selectedDictionaryIndex READ getSelectedDictionaryIndex WRITE setSelectedDictionaryIndex NOTIFY selectedDictionaryIndexChanged)
        Q_PROPERTY(bool hasMore READ getHasMore NOTIFY hasMoreChanged)
        Q_PROPERTY(QString query READ getQuery WRITE setQuery NOTIFY queryChanged)
        Q_PROPERTY(QStringList dictionaries READ getDictionariesDescriptions NOTIFY dictionariesChanged)

    public:
        explicit TranslationManager(QObject *parent = 0);

    public:
        bool getHasMore() const { return m_HasMore; }
        bool getIsBusy() const { return m_IsBusy; }
        void setIsBusy(bool value) {
            if (m_IsBusy != value) {
                m_IsBusy = value;
                emit isBusyChanged();
            }
        }

        const QString &getQuery() const { return m_Query; }
        const QString &getFullTranslation() const { return m_FullTranslation; }
        const QString &getShortTranslation() const { return m_ShortenedTranslation; }
        int getSelectedDictionaryIndex() const { return m_SelectedDictionaryIndex; }

    public:
        void initializeDictionaries();
        void setQuery(const QString &value);
        void setSelectedDictionaryIndex(int value);
        QStringList getDictionariesDescriptions() const;

    private:
        void doInitializeDictionaries();
        bool acquireDictionary(const QString &anyDictFilePath);

    public:
        Q_INVOKABLE void clear();
        Q_INVOKABLE bool addDictionary(const QString &anyDictFilePath);

    signals:
        void isBusyChanged();
        void fullTranslationChanged();
        void shortTranslationChanged();
        void hasMoreChanged();
        void selectedDictionaryIndexChanged();
        void queryChanged();
        void dictionariesChanged();

    public slots:
        void translationArrived();

    private slots:
        void updateTranslationTimer();

    private:
        QStringList m_AllowedSuffixes;
        QVector<DictionaryInfo> m_DictionariesList;
        QTimer m_TranslateTimer;
        QString m_DictionariesDirPath;
        QString m_Query;
        QString m_FullTranslation;
        QString m_ShortenedTranslation;
        int m_SelectedDictionaryIndex;
        volatile bool m_IsBusy;
        volatile bool m_HasMore;
    };
}

#endif // TRANSLATIONMANAGER_H
