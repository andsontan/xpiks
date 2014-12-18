#ifndef CLIPBOARDHELPER
#define CLIPBOARDHELPER

#include <QApplication>
#include <QClipboard>
#include <QObject>

namespace Helpers {
    class ClipboardHelper : public QObject
    {
        Q_OBJECT
    public:
        explicit ClipboardHelper(QObject *parent = 0) : QObject(parent) {
            clipboard = QApplication::clipboard();
        }

        Q_INVOKABLE QString getText() {
            return clipboard->text(QClipboard::Clipboard);
        }

        Q_INVOKABLE void setText(const QString &text) {
            clipboard->setText(text, QClipboard::Clipboard);
        }

    private:
        QClipboard *clipboard;
    };
}

#endif // CLIPBOARDHELPER

