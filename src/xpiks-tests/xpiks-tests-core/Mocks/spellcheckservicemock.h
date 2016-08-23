#ifndef SPELLCHECKSERVICEMOCK_H
#define SPELLCHECKSERVICEMOCK_H

#include "../../xpiks-qt/SpellCheck/spellcheckerservice.h"

namespace Mocks {
    class SpellCheckServiceMock:
        public SpellCheck::SpellCheckerService
    {
    Q_OBJECT

    public:
        SpellCheckServiceMock():
            SpellCheck::SpellCheckerService() {}

        // SpellCheckerService interface

    public:
        virtual QStringList suggestCorrections(const QString &word) const {
            Q_UNUSED(word);
            return QStringList() << "item1" << "item2" << "item3";
        }
    };
}

#endif // SPELLCHECKSERVICEMOCK_H
