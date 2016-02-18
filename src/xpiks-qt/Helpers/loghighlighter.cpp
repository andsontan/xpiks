#include "loghighlighter.h"
#include <QColor>

namespace Helpers {
    LogHighlighter::LogHighlighter(QTextDocument *document) :
        QSyntaxHighlighter(document){
        //from Colors.js
        destructiveColor = QColor::fromRgb(209, 11, 11);
        artworkModifiedColor  = QColor::fromRgb(244,156,18);
        defaultInputBackground = QColor::fromRgb(153,153,153);
    }

    void LogHighlighter::highlightBlock(const QString &text){
        int size = text.size();
        QString word = text.mid(24, 24+11);
        if (word.mid(0,7)==QLatin1Literal("- Info:"))
            return;
        if (word.mid(0,8)==QLatin1Literal("- Debug:")){
            setFormat(0, size, defaultInputBackground);
            return;
        }
        if (word.mid(0,11)==QLatin1Literal("- Critical:")){
            setFormat(0, size, destructiveColor);
            return;
        }
        if (word.mid(0,8)==QLatin1Literal("- Fatal:")){
            setFormat(0, size, destructiveColor);
            return;
        }
        if (word.mid(0,10)==QLatin1Literal("- Warning:")){
            setFormat(0, size, artworkModifiedColor);
            return;
        }
    }
}
