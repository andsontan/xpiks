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

#include "translationworker.h"
#include <lookupdictionary.h>
#include <string>
#include "../Common/defines.h"

namespace Translation {
    TranslationWorker::TranslationWorker(QObject *parent) :
        QObject(parent)
    {
    }

    TranslationWorker::~TranslationWorker() {
    }

    void TranslationWorker::selectDictionary(const QString &dictionaryPath) {
        LOG_INFO << dictionaryPath;

        if (m_LookupDictionary->isLoaded()) {
            LOG_INFO << "Unloading old dictionary";
            m_LookupDictionary.reset(new LookupDictionary());
        }

#ifdef Q_OS_WIN
        m_LookupDictionary->setIfoFilePath(dictionaryPath.toStdWString());
#else
        m_LookupDictionary->setIfoFilePath(dictionaryPath.toStdString());
#endif
    }

    bool TranslationWorker::initWorker() {
        LOG_DEBUG << "#";
        m_LookupDictionary.reset(new LookupDictionary());

        return true;
    }

    void TranslationWorker::processOneItem(std::shared_ptr<TranslationQuery> &item) {
        std::string translationData;
        auto &query = item->getQuery();
        QString request = query.simplified().toLower();
        LOG_INFO << "translation request:" << query;

        if (!request.isEmpty()) {
            ensureDictionaryLoaded();
            std::string word = request.toUtf8().toStdString();

            if (m_LookupDictionary->translate(word, translationData)) {
                QString translation = QString::fromUtf8(translationData.c_str());
                item->setTranslation(translation);
            } else {
                item->setFailed();
            }
        } else {
            item->setFailed();
        }
    }

    void TranslationWorker::ensureDictionaryLoaded() {
        Q_ASSERT(m_LookupDictionary);

        if (!m_LookupDictionary->isLoaded()) {
            LOG_INFO << "Loading current dictionary";
            bool result = m_LookupDictionary->loadDictionary();
            if (!result) {
                LOG_WARNING << "Loading dictionary failed!";
            }
        }
    }
}
