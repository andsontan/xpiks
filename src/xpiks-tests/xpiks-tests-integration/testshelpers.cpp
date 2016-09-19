#include "testshelpers.h"
#include <QThread>

void sleepWait(int seconds, const std::function<bool ()> &condition) {
    int times = 0;
    while (times < seconds) {
        if (!condition()) {
            QThread::sleep(1);
        }

        times++;
    }
}
