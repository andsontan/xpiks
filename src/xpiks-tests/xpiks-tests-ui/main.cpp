#include <QtQuickTest/quicktest.h>
#include <QtQml>
#include "../../xpiks-qt/Helpers/clipboardhelper.h"

int main(int argc, char **argv) {
    qmlRegisterType<Helpers::ClipboardHelper>("xpiks", 1, 0, "ClipboardHelper");
    return quick_test_main(argc, argv, "xpiks_tests_ui", QUICK_TEST_SOURCE_DIR);
}
