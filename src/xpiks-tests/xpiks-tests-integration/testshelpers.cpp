#include "testshelpers.h"
#include <QThread>
#include <QCoreApplication>

void sleepWait(int seconds, const std::function<bool ()> &condition) {
    int times = 0;
    while (times < seconds) {
        if (!condition()) {
            QCoreApplication::processEvents(QEventLoop::AllEvents);
            QThread::sleep(1);
        }

        times++;
    }
}
