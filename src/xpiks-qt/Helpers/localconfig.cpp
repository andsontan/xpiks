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

#include "localconfig.h"

namespace Helpers {
    LocalConfig::LocalConfig(const QString &fileName):
        m_FileName(fileName)
    {
    }

    void LocalConfig::initConfig() {
        QFile file(m_FileName);

        if (file.open(QIODevice::ReadOnly)) {
            QString text = file.readAll();
            file.close();
            m_Config = QJsonDocument::fromJson(text.toUtf8());
        } else {
            LOG_WARNING << "Opening file" << m_FileName << "failed";
        }
    }

    void LocalConfig::saveToFile() {
        QFile file(m_FileName);

        if (file.open(QIODevice::WriteOnly)) {
            file.write(m_Config.toJson(QJsonDocument::Indented));
            file.close();
        } else {
            LOG_WARNING << "Opening file" << m_FileName << "failed";
        }
    }
}

