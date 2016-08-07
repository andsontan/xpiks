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

#include "autocompleteworker.h"
#include <QDir>
#include <QCoreApplication>
#include "../Common/defines.h"
#include <src/libfaceapi.hpp>
#include <include/types.hpp>
#include <string>
#include <QStringList>

#define FREQUENCY_TABLE_FILENAME "en_wordlist.tsv"

namespace AutoComplete {
    AutoCompleteWorker::AutoCompleteWorker(QObject *parent) :
        QObject(parent),
        m_Soufleur(NULL),
        m_CompletionsCount(8)
    {
    }

    AutoCompleteWorker::~AutoCompleteWorker() {
        if (m_Soufleur != NULL) {
            delete m_Soufleur;
        }

        LOG_INFO << "destroyed";
    }

    bool AutoCompleteWorker::initWorker() {
        LOG_INFO << "#";
        bool importResult = false;

        QString resourcesPath;
        QString wordlistPath;

//#if !defined(Q_OS_LINUX)
        resourcesPath = QCoreApplication::applicationDirPath();

#if defined(Q_OS_MAC)
#if defined(INTEGRATION_TESTS)
        resourcesPath += "/../../../xpiks-qt/deps/";
#else
        resourcesPath += "/../Resources/";
#endif
#elif  defined(APPVEYOR)
        resourcesPath += "/../../../xpiks-qt/deps/";
#elif defined(Q_OS_WIN)
        resourcesPath += "/ac_sources/";
#elif defined(TRAVIS_CI)
        resourcesPath += "/../../xpiks-qt/deps/";
#endif

        QDir resourcesDir(resourcesPath);
        wordlistPath = resourcesDir.absoluteFilePath(FREQUENCY_TABLE_FILENAME);

        if (QFileInfo(wordlistPath).exists()) {
            try {
                m_Soufleur = new Souffleur();
                importResult = m_Soufleur->import(wordlistPath.toStdString().c_str());
                if (!importResult) {
                    LOG_WARNING << "Failed to import" << wordlistPath;
                } else {
                    LOG_INFO << "LIBFACE initialized with" << wordlistPath;
                }
            }
            catch (...) {
                LOG_WARNING << "Exception while initializing LIBFACE with" << wordlistPath;
            }
        } else {
            LOG_WARNING << "File not found:" << wordlistPath;
        }

        return importResult;
    }

    void AutoCompleteWorker::processOneItem(std::shared_ptr<CompletionQuery> &item) {
        const QString &prefix = item->getPrefix();

        vp_t completions = m_Soufleur->prompt(prefix.toStdString(), m_CompletionsCount);

        QStringList completionsList;
        QSet<QString> completionsSet;

        int size = (int)completions.size(), i = 0;
        completions.reserve(size);
        completionsSet.reserve(size);

        for (; i < size; ++i) {
            const phrase_t &suggestion = completions[i];
            QString phrase = QString::fromStdString(suggestion.phrase).trimmed();

            if (!completionsSet.contains(phrase)) {
                completionsList.append(phrase);
                completionsSet.insert(phrase);
            }
        }

        if (!completionsList.isEmpty()) {
            item->setCompletions(completionsList);
        }
    }

    void AutoCompleteWorker::deleteItem(CompletionQuery *item) const {
        item->deleteLater();
    }
}
