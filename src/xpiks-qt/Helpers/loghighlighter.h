#ifndef LOGHIGHLIGHTER_H
#define LOGHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QString>
#include <QTextDocument>

namespace QMLExtensions {
    class ColorsModel;
}

namespace Helpers {
    class LogHighlighter: public QSyntaxHighlighter {
        Q_OBJECT
    public:
        LogHighlighter(QMLExtensions::ColorsModel *colorsModel, QTextDocument* document = 0);

    protected:
        void highlightBlock(const QString &text);

    private:
        QMLExtensions::ColorsModel *m_ColorsModel;
    };
}
#endif // LOGHIGHLIGHTER_H
