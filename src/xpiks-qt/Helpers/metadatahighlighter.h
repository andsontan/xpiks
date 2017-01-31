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

#ifndef METADATAHIGHLIGHTER_H
#define METADATAHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QString>
#include <QTextDocument>
#include "../Common/iflagsprovider.h"

namespace QMLExtensions {
    class ColorsModel;
}

namespace Helpers {
    class MetadataHighlighter:
        public QSyntaxHighlighter
    {
    public:
        MetadataHighlighter(const QString &replaceFrom, Common::IFlagsProvider *flagsProvider,
                            QMLExtensions::ColorsModel *colorsModel, QTextDocument *document=0);

    protected:
        void highlightBlock(const QString &text);

    private:
        QMLExtensions::ColorsModel *m_ColorsModel;
        QString m_ReplaceFrom;
        Common::IFlagsProvider *m_FlagsProvider;
        QTextCharFormat m_Format;
    };
}

#endif // METADATAHIGHLIGHTER_H
