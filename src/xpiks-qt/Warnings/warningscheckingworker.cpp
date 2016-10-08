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
#include <QSize>
#include "../Common/defines.h"
#include "../Common/flags.h"
#include "../Models/artworkmetadata.h"
#include "../Models/imageartwork.h"

namespace Warnings {
    QSet<QString> toLowerSet(const QStringList &from) {
        QSet<QString> result;
        result.reserve(from.length());
        foreach(const QString &str, from) {
            result.insert(str.toLower());
        }
        return result;
    }

    WarningsCheckingWorker::WarningsCheckingWorker(AutoComplete::WarningsSettingsModel *warningsSettingsModel, QObject *parent):
        QObject(parent),
        m_WarningsSettingsModel(warningsSettingsModel) {
        Q_ASSERT(m_WarningsSettingsModel != nullptr);
    }

    bool WarningsCheckingWorker::initWorker() {
        LOG_DEBUG << "#";
        return true;
    }

    void WarningsCheckingWorker::processOneItem(std::shared_ptr<WarningsItem> &item) {
        Common::WarningFlags warningsFlags = Common::WarningFlags::None;

        if (item->needCheckAll()) {
            warningsFlags |= checkDimensions(item);
            warningsFlags |= checkDescription(item);
            warningsFlags |= checkTitle(item);
            warningsFlags |= checkKeywords(item);
        } else {
            auto checkingFlags = item->getCheckingFlags();
            switch (checkingFlags) {
                case Common::WarningsCheckFlags::Description:
                    warningsFlags |= checkDescription(item);
                    break;
                case Common::WarningsCheckFlags::Keywords:
                    warningsFlags |= checkKeywords(item);
                    warningsFlags |= checkDuplicates(item);
                    break;
                case Common::WarningsCheckFlags::Title:
                    warningsFlags |= checkTitle(item);
                    break;
                case Common::WarningsCheckFlags::Spelling:
                    warningsFlags |= checkSpelling(item);
                    break;
                case Common::WarningsCheckFlags::All:
                    // to make compiler happy
                    break;
            }
        }

        item->submitWarnings(warningsFlags);
    }

    Common::WarningFlags WarningsCheckingWorker::checkDimensions(std::shared_ptr<WarningsItem> &wi) const {
        LOG_INTEGRATION_TESTS << "#";
        QString allowedFilenameCharacters = m_WarningsSettingsModel->getAllowedFilenameCharacters();
        double minimumMegapixels = m_WarningsSettingsModel->getMinMegapixels();
        Models::ArtworkMetadata *item = wi->getCheckableItem();
        Common::WarningFlags warningsInfo = Common::WarningFlags::None;

        Models::ImageArtwork *image = dynamic_cast<Models::ImageArtwork *>(item);
        if (image != NULL) {
            QSize size = image->getImageSize();

            double currentProd = size.width() * size.height() / 1000000.0;
            if (currentProd < minimumMegapixels) {
                Common::SetFlag(warningsInfo, Common::WarningFlags::SizeLessThanMinimum);
            }
        }

        qint64 filesize = item->getFileSize();
        const qint64 maxSize = 15*1024*1024; // 15 MB
        if (filesize >= maxSize) {
            Common::SetFlag(warningsInfo, Common::WarningFlags::FileIsTooBig);
        }

        QFileInfo fi(item->getFilepath());
        QString filename = fi.fileName();
        int length = filename.length();
        for (int i = 0; i < length; ++i) {
            QChar c = filename[i];
            bool isOk = c.isLetter() || c.isDigit() ||
                        allowedFilenameCharacters.contains(c);
            if (!isOk) {
                Common::SetFlag(warningsInfo, Common::WarningFlags::FilenameSymbols);
                break;
            }
        }

        return warningsInfo;
    }

    Common::WarningFlags WarningsCheckingWorker::checkKeywords(std::shared_ptr<WarningsItem> &wi) const {
        LOG_INTEGRATION_TESTS << "#";
        int maximumKeywordsCount = m_WarningsSettingsModel->getMaxKeywordsCount();
        Common::WarningFlags warningsInfo = Common::WarningFlags::None;
        Models::ArtworkMetadata *item = wi->getCheckableItem();
        Common::BasicKeywordsModel *keywordsModel = item->getKeywordsModel();

        int keywordsCount = keywordsModel->getKeywordsCount();

        if (keywordsCount == 0) {
            Common::SetFlag(warningsInfo, Common::WarningFlags::NoKeywords);
        } else {
            if (keywordsCount < 7) {
                Common::SetFlag(warningsInfo, Common::WarningFlags::TooFewKeywords);
            }

            if (keywordsCount > maximumKeywordsCount) {
                Common::SetFlag(warningsInfo, Common::WarningFlags::TooManyKeywords);
            }

            if (keywordsModel->hasKeywordsSpellError()) {
                Common::SetFlag(warningsInfo, Common::WarningFlags::SpellErrorsInKeywords);
            }
        }

        return warningsInfo;
    }

    Common::WarningFlags WarningsCheckingWorker::checkDescription(std::shared_ptr<WarningsItem> &wi) const {
        LOG_INTEGRATION_TESTS << "#";
        int maximumDescriptionLength = m_WarningsSettingsModel->getMaxDescriptionLength();
        Common::WarningFlags warningsInfo = Common::WarningFlags::None;
        Models::ArtworkMetadata *item = wi->getCheckableItem();

        int descriptionLength = wi->getDescription().length();

        if (descriptionLength == 0) {
            Common::SetFlag(warningsInfo, Common::WarningFlags::DescriptionIsEmpty);
        } else {
            Common::BasicKeywordsModel *keywordsModel = item->getKeywordsModel();

            if (descriptionLength > maximumDescriptionLength) {
                Common::SetFlag(warningsInfo, Common::WarningFlags::DescriptionTooBig);
            }

            QStringList descriptionWords = wi->getDescriptionWords();

            int wordsLength = descriptionWords.length();
            if (wordsLength < 3) {
                Common::SetFlag(warningsInfo, Common::WarningFlags::DescriptionNotEnoughWords);
            }

            if (keywordsModel->hasDescriptionSpellError()) {
                Common::SetFlag(warningsInfo, Common::WarningFlags::SpellErrorsInDescription);
            }

            QSet<QString> descriptionWordsSet = toLowerSet(descriptionWords);
            QSet<QString> intersection = descriptionWordsSet.intersect(item->getKeywordsSet());

            if (!intersection.isEmpty()) {
                Common::SetFlag(warningsInfo, Common::WarningFlags::KeywordsInDescription);
            }
        }

        return warningsInfo;
    }

    Common::WarningFlags WarningsCheckingWorker::checkTitle(std::shared_ptr<WarningsItem> &wi) const {
        LOG_INTEGRATION_TESTS << "#";

        Common::WarningFlags warningsInfo = Common::WarningFlags::None;
        Models::ArtworkMetadata *item = wi->getCheckableItem();

        int titleLength = wi->getTitle().length();

        if (titleLength == 0) {
            Common::SetFlag(warningsInfo, Common::WarningFlags::TitleIsEmpty);
        } else {
            Common::BasicKeywordsModel *keywordsModel = item->getKeywordsModel();

            QStringList titleWords = wi->getTitleWords();
            int partsLength = titleWords.length();

            if (partsLength < 3) {
                Common::SetFlag(warningsInfo, Common::WarningFlags::TitleNotEnoughWords);
            }

            if (partsLength > 10) {
                Common::SetFlag(warningsInfo, Common::WarningFlags::TitleTooManyWords);
            }

            if (keywordsModel->hasTitleSpellError()) {
                Common::SetFlag(warningsInfo, Common::WarningFlags::SpellErrorsInTitle);
            }

            QSet<QString> titleWordsSet = toLowerSet(titleWords);
            QSet<QString> intersection = titleWordsSet.intersect(item->getKeywordsSet());

            if (!intersection.isEmpty()) {
                Common::SetFlag(warningsInfo, Common::WarningFlags::KeywordsInTitle);
            }
        }

        return warningsInfo;
    }

    Common::WarningFlags WarningsCheckingWorker::checkSpelling(std::shared_ptr<WarningsItem> &wi) const {
        LOG_INTEGRATION_TESTS << "#";

        Common::WarningFlags warningsInfo = Common::WarningFlags::None;
        Models::ArtworkMetadata *item = wi->getCheckableItem();
        Common::BasicKeywordsModel *keywordsModel = item->getKeywordsModel();

        if (keywordsModel->hasKeywordsSpellError()) {
            LOG_INTEGRATION_TESTS << "Detected keywords spell error";
            Common::SetFlag(warningsInfo, Common::WarningFlags::SpellErrorsInKeywords);
        }

        if (keywordsModel->hasDescriptionSpellError()) {
            LOG_INTEGRATION_TESTS << "Detected description spell error";
            Common::SetFlag(warningsInfo, Common::WarningFlags::SpellErrorsInDescription);
        }

        if (keywordsModel->hasTitleSpellError()) {
            LOG_INTEGRATION_TESTS << "Detected title spell error";
            Common::SetFlag(warningsInfo, Common::WarningFlags::SpellErrorsInTitle);
        }

        return warningsInfo;
    }

    Common::WarningFlags WarningsCheckingWorker::checkDuplicates(std::shared_ptr<WarningsItem> &wi) const {
        Common::WarningFlags warningsInfo = Common::WarningFlags::None;
        Models::ArtworkMetadata *item = wi->getCheckableItem();
        Common::BasicKeywordsModel *keywordsModel = item->getKeywordsModel();

        if (keywordsModel->getKeywordsCount() == 0) {
            return warningsInfo;
        }

        const QSet<QString> &keywordsSet = wi->getKeywordsSet();

        QStringList titleWords = wi->getTitleWords();
        if (!titleWords.isEmpty()) {
            QSet<QString> titleWordsSet = toLowerSet(titleWords);
            QSet<QString> intersection = titleWordsSet.intersect(keywordsSet);

            if (!intersection.isEmpty()) {
                Common::SetFlag(warningsInfo, Common::WarningFlags::KeywordsInTitle);
            }
        }

        QStringList descriptionWords = wi->getDescriptionWords();
        if (!descriptionWords.isEmpty()) {
            QSet<QString> descriptionWordsSet = toLowerSet(descriptionWords);
            QSet<QString> intersection = descriptionWordsSet.intersect(keywordsSet);

            if (!intersection.isEmpty()) {
                Common::SetFlag(warningsInfo, Common::WarningFlags::KeywordsInDescription);
            }
        }

        return warningsInfo;
    }
}
