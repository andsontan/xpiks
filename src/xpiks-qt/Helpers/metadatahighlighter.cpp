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

namespace Helpers {
    MetadataHighlighter::MetadataHighlighter(const QString &replaceFrom, bool caseSensitive, QMLExtensions::ColorsModel *colorsModel,
                                             QTextDocument *document):
        QSyntaxHighlighter(document),
        m_ColorsModel(colorsModel),
        m_ReplaceFrom(replaceFrom),
        m_CaseSensitive(caseSensitive)
    {
    }

    void MetadataHighlighter::highlightBlock(const QString &text) {
        int pos = 0;
        int size = m_ReplaceFrom.size();
        Qt::CaseSensitivity caseSensitivity = m_CaseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;

        QColor highlighColor = m_ColorsModel->artworkModifiedColor();
        m_Format.setBackground(highlighColor);

        while (pos != -1) {
            pos = text.indexOf(m_ReplaceFrom, pos, caseSensitivity);
            if (pos >= 0) {
                setFormat(pos, size, m_Format);
                pos += size;
            }
        }
    }
}
