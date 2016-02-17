#include "loghighlighter.h"

#include <QColor>

LogHighlighter::LogHighlighter(QTextDocument *document) :
    QSyntaxHighlighter(document){


}

void LogHighlighter::highlightBlock(const QString &text){

    int i = 0;
    int size = text.size();

    QColor white      = QColor::fromRgb(255,255,255);
    QColor red        = QColor::fromRgb(255,0,0);
    QColor orange     = QColor::fromRgb(255,165,0);
    QColor grey       = QColor::fromRgb(169,169,169);

    while (i < size) {
                QString word = text.mid(i, i+11);
                if (word.mid(0,7)=="- Info:") {
                    setFormat(0, size, white);
                    break;
                }
                if (word.mid(0,8)=="- Debug:"){
                    setFormat(0, size, grey);
                    break;

                }
                if (word.mid(0,11)=="- Critical:"){
                    setFormat(0, size, red);
                    break;

                }
                if (word.mid(0,8)=="- Fatal:"){
                    setFormat(0, size, red);
                    break;

                }
                if (word.mid(0,10)=="- Warning:"){
                    setFormat(0, size, orange);
                    break;
                }
        i++;
    }


    }


