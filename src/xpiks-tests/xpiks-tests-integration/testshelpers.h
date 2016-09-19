#ifndef TESTSHELPERS_H
#define TESTSHELPERS_H

#include <functional>

void sleepWait(int seconds, const std::function<bool ()> &condition);

#endif // TESTSHELPERS_H
