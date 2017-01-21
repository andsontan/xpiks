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
#include "../Common/baseentity.h"

namespace Translation {
    class TranslationManager :
            public QObject,
            public Common::BaseEntity
    {
        Q_OBJECT
        Q_PROPERTY(bool isBusy READ getIsBusy WRITE setIsBusy NOTIFY isBusyChanged)
        Q_PROPERTY(QString translation READ getTranslation NOTIFY translationChanged)
        Q_PROPERTY(int selectedDictionaryIndex READ getSelectedDictionaryIndex WRITE setSelectedDictionaryIndex NOTIFY selectedDictionaryIndexChanged)
    public:
        explicit TranslationManager(QObject *parent = 0);

    public:
        bool getIsBusy() const { return m_IsBusy; }
        void setIsBusy(bool value) {
            if (m_IsBusy != value) {
                m_IsBusy = value;
                emit isBusyChanged();
            }
        }

        QString getTranslation() const { return m_Translation; }

    public:
        Q_INVOKABLE void translatePhrase(const QString &phrase);

    signals:
        void isBusyChanged();
        void translationChanged();
        void selectedDictionaryIndexChanged();

    public slots:

    private:
        QString m_Translation;
        int m_SelectedDictionaryIndex;
        volatile bool m_IsBusy;
    };
}

#endif // TRANSLATIONMANAGER_H
