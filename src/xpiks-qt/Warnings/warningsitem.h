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

#ifndef WARNINGSQUERYITEM
#define WARNINGSQUERYITEM

#include <QStringList>
#include <QString>
#include <QSet>
#include "../Helpers/stringhelper.h"
#include "../Common/flags.h"
#include "../Models/artworkmetadata.h"
#include "../Common/defines.h"

namespace Warnings {
    class WarningsItem {
    public:
        WarningsItem(Models::ArtworkMetadata *checkableItem, Common::WarningsCheckFlags checkingFlags = Common::WarningsCheckFlags::All):
            m_CheckableItem(checkableItem),
            m_CheckingFlags(checkingFlags)
        {
            checkableItem->acquire();
            m_Description = checkableItem->getDescription();
            m_Title = checkableItem->getTitle();
            m_KeywordsSet = checkableItem->getKeywordsSet();
        }

        ~WarningsItem() {
            if (m_CheckableItem->release()) {
                LOG_WARNING << "Item could have been removed";
            }
        }

    public:
        void submitWarnings(Common::WarningFlags warningsFlags) {
            if (m_CheckingFlags == Common::WarningsCheckFlags::All) {
                m_CheckableItem->setWarningsFlags(warningsFlags);
            } else {
                Common::WarningFlags flagsToDrop = Common::WarningFlags::None;

                switch (m_CheckingFlags) {
                case Common::WarningsCheckFlags::Description:
                    flagsToDrop = Common::WarningFlags::DescriptionGroup;
                    break;
                case Common::WarningsCheckFlags::Keywords:
                    flagsToDrop = Common::WarningFlags::KeywordsGroup;
                    break;
                case Common::WarningsCheckFlags::Title:
                    flagsToDrop = Common::WarningFlags::TitleGroup;
                    break;
                case Common::WarningsCheckFlags::Spelling:
                    flagsToDrop = Common::WarningFlags::SpellingGroup;
                    break;
                case Common::WarningsCheckFlags::All:
                    // to make compiler happy
                    break;
                default:
                    break;
                }

                m_CheckableItem->dropWarningsFlags(flagsToDrop);
                m_CheckableItem->addWarningsFlags(warningsFlags);
            }
        }

        bool needCheckAll() const { return m_CheckingFlags == Common::WarningsCheckFlags::All; }
        Common::WarningsCheckFlags getCheckingFlags() const { return m_CheckingFlags; }
        const QString &getDescription() const { return m_Description; }
        const QString &getTitle() const { return m_Title; }
        const QSet<QString> &getKeywordsSet() const { return m_KeywordsSet; }

        QStringList getDescriptionWords() const {
            QStringList words;
            Helpers::splitText(m_Description, words);
            return words;
        }

        QStringList getTitleWords() const {
            QStringList words;
            Helpers::splitText(m_Title, words);
            return words;
        }

        Models::ArtworkMetadata *getCheckableItem() const { return m_CheckableItem; }

    private:
        Models::ArtworkMetadata *m_CheckableItem;
        QString m_Description;
        QString m_Title;
        QSet<QString> m_KeywordsSet;
        Common::WarningsCheckFlags m_CheckingFlags;
    };
}

#endif // WARNINGSQUERYITEM

