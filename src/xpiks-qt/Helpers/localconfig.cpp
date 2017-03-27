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

#include "localconfig.h"
#include <QDir>
#include <QStandardPaths>

namespace Helpers {
    LocalConfig::LocalConfig() {
    }

    void LocalConfig::initConfig(const QString &configPath) {
        m_FilePath = configPath;
        QFile file(m_FilePath);

        if (file.open(QIODevice::ReadOnly)) {
            QString text = QString::fromUtf8(file.readAll());
            file.close();
            m_Config = QJsonDocument::fromJson(text.toUtf8());
        } else {
            LOG_WARNING << "Opening file" << m_FilePath << "failed";
        }
    }

    void LocalConfig::saveToFile() {
        QFile file(m_FilePath);

        Q_ASSERT(!m_FilePath.isEmpty());

        if (file.open(QIODevice::WriteOnly)) {
            file.write(m_Config.toJson(QJsonDocument::Indented));
            file.close();
        } else {
            LOG_WARNING << "Opening file" << m_FilePath << "failed";
        }
    }
}

