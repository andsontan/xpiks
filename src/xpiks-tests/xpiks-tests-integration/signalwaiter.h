#ifndef SIGNALWAITER
#define SIGNALWAITER

#include <QObject>
#include <QEventLoop>
#include <QTimer>
#include <QDebug>
#include <QSignalSpy>

class SignalWaiter: public QObject {
    Q_OBJECT
public:
    SignalWaiter(QObject *parent=0):
        QObject(parent),
        m_SignalSpy(this, SIGNAL(finished()))
    {
    }

    bool wait(int timeoutSeconds=5) {
        if (m_SignalSpy.count() > 0) {
            return true;
        }

        QEventLoop loop;
        QObject::connect(this, SIGNAL(finished()), &loop, SLOT(quit()));

        QTimer timeoutTimer;
        timeoutTimer.setSingleShot(true);
        QObject::connect(&timeoutTimer, SIGNAL(timeout()), &loop, SLOT(quit()));

        timeoutTimer.start(timeoutSeconds*1000);
        loop.exec();

        // true - no timeout
        // false - timeout

        return timeoutTimer.isActive();
    }

signals:
    void finished();

private:
    QSignalSpy m_SignalSpy;
};

#endif // SIGNALWAITER

