#ifndef LOGHIGHLIGHTER_H
#define LOGHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QString>
#include <QTextDocument>

namespace Helpers {
    class LogHighlighter: public QSyntaxHighlighter {
        Q_OBJECT
    public:
        LogHighlighter(QTextDocument* document = 0);
    protected:
        void highlightBlock(const QString &text);

    private:
        //from Colors.js
        QColor m_DestructiveColor;
        QColor m_ArtworkModifiedColor;
        QColor m_DefaultInputBackground;
    };
}
#endif // LOGHIGHLIGHTER_H
