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
#include "../Common/defines.h"
#include "../Common/flags.h"
#include "../Models/settingsmodel.h"

namespace Warnings {

    WarningsCheckingWorker::WarningsCheckingWorker(Models::SettingsModel *settingsModel, QObject *parent):
        QObject(parent),
        m_SettingsModel(settingsModel)
    {
    }

    bool WarningsCheckingWorker::initWorker() {
        qDebug() << "WarningsCheckingWorker::initWorker #";
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
                break;
            case Common::WarningsCheckTitle:
                warningsFlags |= checkTitle(checkableItem);
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

            int wordsLength = item->getDescriptionWords().length();
            if (wordsLength < 3) {
                Common::SetFlag(warningsInfo, Common::WarningTypeDescriptionNotEnoughWords);
            }

            if (item->hasDescriptionSpellError()) {
                Common::SetFlag(warningsInfo, Common::WarningTypeSpellErrorsInDescription);
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
            int partsLength = item->getTitleWords().length();

            if (partsLength < 3) {
                Common::SetFlag(warningsInfo, Common::WarningTypeTitleNotEnoughWords);
            }

            if (partsLength > 10) {
                Common::SetFlag(warningsInfo, Common::WarningTypeTitleTooManyWords);
            }

            if (item->hasTitleSpellError()) {
                Common::SetFlag(warningsInfo, Common::WarningTypeSpellErrorsInTitle);
            }
        }

        return warningsInfo;
    }
}

