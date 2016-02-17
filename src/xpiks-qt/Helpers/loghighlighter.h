#ifndef LOGHIGHLIGHTER_H
#define LOGHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QString>
#include <QTextDocument>

class LogHighlighter: public QSyntaxHighlighter {
    Q_OBJECT
public:
    LogHighlighter(QTextDocument* document = 0);
protected:
    void highlightBlock(const QString &text);

private:

};

#endif // LOGHIGHLIGHTER_H
