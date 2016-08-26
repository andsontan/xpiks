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

#ifndef LANGUAGESMODEL_H
#define LANGUAGESMODEL_H

#include <QAbstractListModel>
#include <QVector>
#include <QString>
#include <QPair>
#include <QDir>
#include "../Common/baseentity.h"

class QTranslator;

namespace Models {
    class LanguagesModel: public QAbstractListModel, public Common::BaseEntity
    {
        Q_OBJECT
        Q_PROPERTY(QString n READ getEmptyString() NOTIFY languageChanged)
    public:
        LanguagesModel(QObject *parent=0);

    public:
        enum LanguagesModel_Roles {
            IsCurrentRole = Qt::UserRole + 1
        };

        const QString &getEmptyString() const { return m_EmptyString; }

    signals:
        void languageChanged();

    public:
        void initFirstLanguage();
        QString getCurrentLanguage() const;
        Q_INVOKABLE void loadLanguages();
        Q_INVOKABLE void switchLanguage(int index);

    public:
        int rowCount(const QModelIndex & parent = QModelIndex()) const { Q_UNUSED(parent); return m_LanguagesList.length(); }
        QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    protected:
        virtual QHash<int, QByteArray> roleNames() const;

    private:
        void loadTranslators(const QDir &dir, const QString &selectedLocale);
        QString getTranslationsPath() const;

    private:
        QVector<QPair<QString, QString> > m_LanguagesList;
        QTranslator *m_XpiksTranslator;
        QTranslator *m_QtTranslator;
        QString m_EmptyString;
        QString m_TranslationsPath;
        int m_CurrentLanguageIndex;
    };
}

#endif // LANGUAGESMODEL_H
