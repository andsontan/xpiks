#ifndef METADATAHIGHLIGHTER_H
#define METADATAHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QString>
#include <QTextDocument>

namespace QMLExtensions {
    class ColorsModel;
}

namespace Helpers {
    class MetadataHighlighter:
        public QSyntaxHighlighter
    {
    public:
        MetadataHighlighter(const QString &replaceFrom, bool caseSensitive, QMLExtensions::ColorsModel *colorsModel, QTextDocument *document=0);

    protected:
        void highlightBlock(const QString &text);

    private:
        QMLExtensions::ColorsModel *m_ColorsModel;
        QString m_ReplaceFrom;
        bool m_CaseSensitive;
        QTextCharFormat m_Format;
    };
}

#endif // METADATAHIGHLIGHTER_H
