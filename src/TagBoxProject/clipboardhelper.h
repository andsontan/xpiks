#ifndef CLIPBOARDHELPER
#define CLIPBOARDHELPER

#include <QGuiApplication>
#include <QClipboard>
#include <QObject>

class ClipboardHelper : public QObject
{
    Q_OBJECT
public:
    explicit ClipboardHelper(QObject *parent = 0) : QObject(parent) {
        clipboard = QGuiApplication::clipboard();
    }

    Q_INVOKABLE QString getText() {
        return clipboard->text(QClipboard::Clipboard);
    }

private:
    QClipboard *clipboard;
};

#endif // CLIPBOARDHELPER

