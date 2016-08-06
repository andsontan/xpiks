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

#include "metadatahighlighter.h"
#include "../QMLExtensions/colorsmodel.h"
#include "../Helpers/stringhelper.h"
#include "../Common/flags.h"

namespace Helpers {
    MetadataHighlighter::MetadataHighlighter(const QString &replaceFrom, Common::IFlagsProvider *flagsProvider,
                                             QMLExtensions::ColorsModel *colorsModel,
                                             QTextDocument *document):
        QSyntaxHighlighter(document),
        m_ColorsModel(colorsModel),
        m_ReplaceFrom(replaceFrom),
        m_FlagsProvider(flagsProvider)
    {
        Q_ASSERT(flagsProvider != nullptr);
        Q_ASSERT(colorsModel != nullptr);
    }

    void MetadataHighlighter::highlightBlock(const QString &text) {
        int pos = 0;
        const int size = m_ReplaceFrom.size();
        int flags = m_FlagsProvider->getFlags();
        Qt::CaseSensitivity caseSensitivity = Common::HasFlag(flags, Common::SearchFlagCaseSensitive) ?
                    Qt::CaseSensitive : Qt::CaseInsensitive;
        const bool wholeWords = Common::HasFlag(flags, Common::SearchFlagExactMatch);

        QColor highlighColor = m_ColorsModel->artworkModifiedColor();
        m_Format.setBackground(highlighColor);

        if (!wholeWords) {
            while (pos != -1) {
                pos = text.indexOf(m_ReplaceFrom, pos, caseSensitivity);
                if (pos >= 0) {
                    setFormat(pos, size, m_Format);
                    pos += size;
                }
            }
        } else {
            Helpers::foreachWord(text,
                                 [this, &caseSensitivity](const QString &word) {
                return (QString::compare(this->m_ReplaceFrom, word, caseSensitivity) == 0);
            },
            [this](int start, int length, const QString&) {
                this->setFormat(start, length, this->m_Format);
            });
        }
    }
}
