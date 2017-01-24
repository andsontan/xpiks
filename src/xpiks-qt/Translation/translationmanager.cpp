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

#include "translationmanager.h"
#include <QDir>
#include <QSet>
#include <QFile>
#include <QTextStream>
#include "../Common/defines.h"
#include "translationquery.h"
#include "translationservice.h"
#include "../Commands/commandmanager.h"

#define SHORT_TRANSLATION_SYMBOLS 200
#define BOOKNAME QLatin1String("bookname")

namespace Translation {
    bool parseIfoFile(const QString &fullIfoPath, DictionaryInfo &info) {
        QFile file(fullIfoPath);
        bool success = true;

        if (file.open(QIODevice::ReadOnly)) {
            QTextStream in(&file);

            QString line;

            do {
                if (!in.atEnd()) {
                    line = in.readLine();
                    if ((line != "StarDict's treedict ifo file") ||
                            (line != "StarDict's dict ifo file")) {
                        LOG_WARNING << "Wrong header" << line;
                        break;
                    }
                }

                while (!in.atEnd()) {
                    line = in.readLine();
                    if (line.startsWith(BOOKNAME)) {
                        info.m_Description = line.mid(BOOKNAME.size() + 1).simplified();
                        info.m_FullIfoPath = fullIfoPath;
                        success = true;
                        break;
                    }
                }
            } while (false);
        }

        return success;
    }

    TranslationManager::TranslationManager(QObject *parent) :
        QObject(parent),
        Common::BaseEntity(),
        m_SelectedDictionaryIndex(-1),
        m_IsBusy(false),
        m_HasMore(false)
    {
        m_TranslateTimer.setSingleShot(true);
        QObject::connect(&m_TranslateTimer, SIGNAL(timeout()), this, SLOT(updateTranslationTimer()));
    }

    void TranslationManager::setQuery(const QString &value) {
        bool anySignificantDifference = value.simplified() != m_Query;

        if (m_Query != value) {
            m_Query = value;
            emit queryChanged();
        }

        if (anySignificantDifference) {
            m_TranslateTimer.start(1000);
        }
    }

    void TranslationManager::setSelectedDictionaryIndex(int value) {
        if (m_SelectedDictionaryIndex != value) {
            m_SelectedDictionaryIndex = value;
            emit selectedDictionaryIndexChanged();

            m_TranslateTimer.start(1000);
        }
    }

    QStringList TranslationManager::getDictionariesDescriptions() const {
        QStringList descriptions;
        descriptions.reserve(m_DictionariesList.length());

        foreach (const DictionaryInfo &dict, m_DictionariesList) {
            descriptions.append(dict.m_Description);
        }

        return descriptions;
    }

    void TranslationManager::clear() {
        setQuery("");
        m_FullTranslation.clear();
        m_ShortenedTranslation.clear();
        emit fullTranslationChanged();
        emit shortTranslationChanged();
        m_HasMore = false;
        emit hasMoreChanged();
    }

    bool TranslationManager::addDictionary(const QString &anyDictFilePath) {
        LOG_INFO << anyDictFilePath;
        QFileInfo fi(anyDictFilePath);
        bool success = false;

        QSet<QString> allowedSuffixes = {"idx", "idx.dz", "idx.oft", "dict.dz", "dict", "ifo"};

        do {
            if (!fi.exists()) {
                LOG_WARNING << "File does not exist:" << anyDictFilePath;
                break;
            }

            if (!fi.isFile()) {
                LOG_WARNING << "Path is not a file:" << anyDictFilePath;
                break;
            }

            if (!allowedSuffixes.contains(fi.completeSuffix().toLower())) {
                LOG_WARNING << "File" << fi.completeSuffix() << "is unsupported";
                break;
            }

            QString probableIfoPath = QDir::cleanPath(fi.absolutePath() + QDir::separator() +
                                                      fi.baseName() + ".ifo");
            DictionaryInfo di;
            if (parseIfoFile(probableIfoPath, di)) {
                m_DictionariesList.append(di);
                LOG_INFO << "Parsed" << di.m_Description;
                success = true;
            } else {
                LOG_WARNING << "Failed to parse IFO:" << probableIfoPath;
            }
        } while (false);

        return success;
    }

    void TranslationManager::translationArrived() {
        LOG_DEBUG << "#";
        TranslationQuery *query = qobject_cast<TranslationQuery *>(sender());
        Q_ASSERT(query != nullptr);

        if (query->hasTranslation()) {
            QString translation = query->getTranslation();

            m_FullTranslation = translation;
            m_ShortenedTranslation = translation.left(SHORT_TRANSLATION_SYMBOLS);
        } else {
            QString nothingFound = QObject::tr("No results");

            m_FullTranslation = nothingFound;
            m_ShortenedTranslation = nothingFound;
        }

        emit fullTranslationChanged();
        emit shortTranslationChanged();

        m_HasMore = m_FullTranslation.length() != m_ShortenedTranslation.length();
        emit hasMoreChanged();

        setIsBusy(false);
    }

    void TranslationManager::updateTranslationTimer() {
        TranslationService *translationService = m_CommandManager->getTranslationService();
        translationService->translate(m_Query);
        setIsBusy(true);
    }
}
