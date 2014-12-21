/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014 Taras Kushnir <kushnirTV@gmail.com>
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

#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QString>
#include <QSettings>
#include <QCoreApplication>
#include "constants.h"

namespace Helpers {
    class AppSettings : public QSettings
    {
        Q_OBJECT

    public:
        explicit AppSettings(QObject *parent = 0) : QSettings(QSettings::UserScope,
                                                              QCoreApplication::instance()->organizationName(),
                                                              QCoreApplication::instance()->applicationName(),
                                                              parent)
        {}

        Q_PROPERTY(QString exifToolPathKey READ getExifToolPathKey)
        QString getExifToolPathKey() const { return Constants::PATH_TO_EXIFTOOL; }

        Q_PROPERTY(QString curlPathKey READ getCurlPathKey)
        QString getCurlPathKey() const { return Constants::PATH_TO_CURL; }

        Q_INVOKABLE inline void setValue(const QString &key, const QVariant &value) {
            QSettings::setValue(key, value);
        }

        Q_INVOKABLE inline QVariant value(const QString &key, const QVariant &defaultValue = QVariant()) const {
            return QSettings::value(key, defaultValue);
        }
    };
}

Q_DECLARE_METATYPE(Helpers::AppSettings*)

#endif // APPSETTINGS_H
