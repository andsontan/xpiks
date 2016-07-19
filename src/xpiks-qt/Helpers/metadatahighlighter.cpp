#include "metadatahighlighter.h"
#include "../QMLExtensions/colorsmodel.h"

Helpers::MetadataHighlighter::MetadataHighlighter(const QString &replaceFrom, bool caseSensitive, QMLExtensions::ColorsModel *colorsModel,
    QTextDocument *document):
    QSyntaxHighlighter(document),
    m_ReplaceFrom(replaceFrom),
    m_CaseSensitive(caseSensitive),
    m_ColorsModel(colorsModel) {
    QColor highlighColor = m_ColorsModel->artworkModifiedColor();

    m_Format.setBackground(highlighColor);
}

void Helpers::MetadataHighlighter::highlightBlock(const QString &text) {
    int pos = 0;
    int size = m_ReplaceFrom.size();
    Qt::CaseSensitivity caseSensitivity = m_CaseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;

    while (pos != -1) {
        pos = text.indexOf(m_ReplaceFrom, pos, caseSensitivity);
        if (pos >= 0) {
            setFormat(pos, size, m_Format);
            pos += size;
        }
    }
}
