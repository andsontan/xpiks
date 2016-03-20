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

#include "warningscheckingworker.h"
#include <QSet>
#include <QFileInfo>
#include "../Common/defines.h"
#include "../Common/flags.h"
#include "../Models/settingsmodel.h"

namespace Warnings {
    QSet<QString> toLowerSet(const QStringList &from) {
        QSet<QString> result;
        result.reserve(from.length());
        foreach(const QString &str, from) {
            result.insert(str.toLower());
        }
        return result;
    }

    WarningsCheckingWorker::WarningsCheckingWorker(Models::SettingsModel *settingsModel, QObject *parent):
        QObject(parent),
        m_SettingsModel(settingsModel),
        m_MinimumMegapixels(4),
        m_MaximumKeywordsCount(50),
        m_MaximumDescriptionLength(200)
    {
    }

    bool WarningsCheckingWorker::initWorker() {
        LOG_DEBUG << "#";
        return true;
    }

    bool WarningsCheckingWorker::processOneItem(WarningsItem *item) {
        Q_ASSERT(item != NULL);

        int warningsFlags = 0;

        initValuesFromSettings();
        IWarningsCheckable *checkableItem = item->getCheckableItem();

        if (item->needCheckAll()) {
            warningsFlags |= checkDimensions(checkableItem);
            warningsFlags |= checkDescription(checkableItem);
            warningsFlags |= checkTitle(checkableItem);
            warningsFlags |= checkKeywords(checkableItem);
        } else {
            int checkingFlags = item->getCheckingFlags();
            switch (checkingFlags) {
            case Common::WarningsCheckDescription:
                warningsFlags |= checkDescription(checkableItem);
                break;
            case Common::WarningsCheckKeywords:
                warningsFlags |= checkKeywords(checkableItem);
                warningsFlags |= checkDuplicates(checkableItem);
                break;
            case Common::WarningsCheckTitle:
                warningsFlags |= checkTitle(checkableItem);
                break;
            case Common::WarningsCheckSpelling:
                warningsFlags |= checkSpelling(checkableItem);
                break;
            }
        }

        item->submitWarnings(warningsFlags);

        return true;
    }

    void WarningsCheckingWorker::initValuesFromSettings() {
        m_MaximumDescriptionLength = m_SettingsModel->getMaxDescriptionLength();
        m_MinimumMegapixels = m_SettingsModel->getMinMegapixelCount();
        m_MaximumKeywordsCount = m_SettingsModel->getMaxKeywordsCount();
    }

    int WarningsCheckingWorker::checkDimensions(IWarningsCheckable *item) const {
        int warningsInfo = 0;
        QSize size = item->getImageSize();

        double currentProd = size.width() * size.height() / 1000000.0;
        if (currentProd < m_MinimumMegapixels) {
            Common::SetFlag(warningsInfo, Common::WarningTypeSizeLessThanMinimum);
        }

        qint64 filesize = item->getFileSize();
        const qint64 maxSize = 15*1024*1024; // 15 MB
        if (filesize >= maxSize) {
            Common::SetFlag(warningsInfo, Common::WarningTypeFileIsTooBig);
        }

        QFileInfo fi(item->getFilepath());
        QString filename = fi.fileName();
        int length = filename.length();
        for (int i = 0; i < length; ++i) {
            QChar c = filename[i];
            bool isOk = c.isLetter() || c.isDigit() ||
                    (c == '_') || (c == '.');
            if (!isOk) {
                Common::SetFlag(warningsInfo, Common::WarningTypeFilenameSymbols);
                break;
            }
        }

        return warningsInfo;
    }

    int WarningsCheckingWorker::checkKeywords(IWarningsCheckable *item) const {
        int warningsInfo = 0;

        int keywordsCount = item->getKeywordsCount();

        if (keywordsCount == 0) {
            Common::SetFlag(warningsInfo, Common::WarningTypeNoKeywords);
        } else {
            if (keywordsCount < 7) {
                Common::SetFlag(warningsInfo, Common::WarningTypeTooFewKeywords);
            }

            if (keywordsCount > m_MaximumKeywordsCount) {
                Common::SetFlag(warningsInfo, Common::WarningTypeTooManyKeywords);
            }

            if (item->hasKeywordsSpellError()) {
                Common::SetFlag(warningsInfo, Common::WarningTypeSpellErrorsInKeywords);
            }
        }

        return warningsInfo;
    }

    int WarningsCheckingWorker::checkDescription(IWarningsCheckable *item) const {
        int warningsInfo = 0;

        int descriptionLength = item->getDescription().length();

        if (descriptionLength == 0) {
            Common::SetFlag(warningsInfo, Common::WarningTypeDescriptionIsEmpty);
        } else {
            if (descriptionLength > m_MaximumDescriptionLength) {
                Common::SetFlag(warningsInfo, Common::WarningTypeDescriptionTooBig);
            }

            QStringList descriptionWords = item->getDescriptionWords();

            int wordsLength = descriptionWords.length();
            if (wordsLength < 3) {
                Common::SetFlag(warningsInfo, Common::WarningTypeDescriptionNotEnoughWords);
            }

            if (item->hasDescriptionSpellError()) {
                Common::SetFlag(warningsInfo, Common::WarningTypeSpellErrorsInDescription);
            }

            QSet<QString> descriptionWordsSet = toLowerSet(descriptionWords);
            QSet<QString> intersection = descriptionWordsSet.intersect(item->getKeywordsSet());

            if (!intersection.isEmpty()) {
                Common::SetFlag(warningsInfo, Common::WarningTypeKeywordsInDescription);
            }
        }

        return warningsInfo;
    }

    int WarningsCheckingWorker::checkTitle(IWarningsCheckable *item) const {
        int warningsInfo = 0;

        int titleLength = item->getTitle().length();

        if (titleLength == 0) {
            Common::SetFlag(warningsInfo, Common::WarningTypeTitleIsEmpty);
        } else {
            QStringList titleWords = item->getTitleWords();
            int partsLength = titleWords.length();

            if (partsLength < 3) {
                Common::SetFlag(warningsInfo, Common::WarningTypeTitleNotEnoughWords);
            }

            if (partsLength > 10) {
                Common::SetFlag(warningsInfo, Common::WarningTypeTitleTooManyWords);
            }

            if (item->hasTitleSpellError()) {
                Common::SetFlag(warningsInfo, Common::WarningTypeSpellErrorsInTitle);
            }

            QSet<QString> titleWordsSet = toLowerSet(titleWords);
            QSet<QString> intersection = titleWordsSet.intersect(item->getKeywordsSet());

            if (!intersection.isEmpty()) {
                Common::SetFlag(warningsInfo, Common::WarningTypeKeywordsInTitle);
            }
        }

        return warningsInfo;
    }

    int WarningsCheckingWorker::checkSpelling(IWarningsCheckable *item) const {
        int warningsInfo = 0;

        if (item->hasKeywordsSpellError()) {
            Common::SetFlag(warningsInfo, Common::WarningTypeSpellErrorsInKeywords);
        }

        if (item->hasDescriptionSpellError()) {
            Common::SetFlag(warningsInfo, Common::WarningTypeSpellErrorsInDescription);
        }

        if (item->hasTitleSpellError()) {
            Common::SetFlag(warningsInfo, Common::WarningTypeSpellErrorsInTitle);
        }

        return warningsInfo;
    }

    int WarningsCheckingWorker::checkDuplicates(IWarningsCheckable *item) const {
        int warningsInfo = 0;

        if (item->getKeywordsCount() == 0) { return warningsInfo; }

        const QSet<QString> &keywordsSet = item->getKeywordsSet();

        QStringList titleWords = item->getTitleWords();
        if (!titleWords.isEmpty()) {
            QSet<QString> titleWordsSet = toLowerSet(titleWords);
            QSet<QString> intersection = titleWordsSet.intersect(keywordsSet);

            if (!intersection.isEmpty()) {
                Common::SetFlag(warningsInfo, Common::WarningTypeKeywordsInTitle);
            }
        }

        QStringList descriptionWords = item->getDescriptionWords();
        if (!descriptionWords.isEmpty()) {
            QSet<QString> descriptionWordsSet = toLowerSet(descriptionWords);
            QSet<QString> intersection = descriptionWordsSet.intersect(keywordsSet);

            if (!intersection.isEmpty()) {
                Common::SetFlag(warningsInfo, Common::WarningTypeKeywordsInDescription);
            }
        }

        return warningsInfo;
    }
}

