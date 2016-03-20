#include "loghighlighter.h"
#include <QColor>
#include <QtGlobal>

namespace Helpers {
    LogHighlighter::LogHighlighter(QTextDocument *document) :
        QSyntaxHighlighter(document)
    {
        //from Colors.js
        m_DestructiveColor = QColor::fromRgb(209, 11, 11);
        m_ArtworkModifiedColor  = QColor::fromRgb(244, 156, 18);
        m_inputBackgroundColor = QColor::fromRgb(153, 153, 153);
    }

    void LogHighlighter::highlightBlock(const QString &text) {
        int size = text.size();

        QString word = text.mid(13, 13+8).toLower();

        if (word.startsWith(QLatin1Literal("debug"))) {
            // for Qt < 5.5.1 "info" msgtype does not exist
            // so we will use default color for debug in old Qt
#if (QT_VERSION >= QT_VERSION_CHECK(5, 5, 1))
            setFormat(0, size, m_inputBackgroundColor);
#endif
        } else if (word.startsWith(QLatin1Literal("info"))) {
            // DO NOTHING - USE DEFAULT COLOR
        } else if (word.startsWith(QLatin1Literal("warning"))) {
            setFormat(0, size, m_ArtworkModifiedColor);
        } else if (word.startsWith(QLatin1Literal("critical"))) {
            setFormat(0, size, m_DestructiveColor);
        } else if (word.startsWith(QLatin1Literal("fatal"))) {
            setFormat(0, size, m_DestructiveColor);
        }
    }
}
