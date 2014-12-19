#ifndef EXTERNALTOOLSPROVIDER_H
#define EXTERNALTOOLSPROVIDER_H

#include <QString>
#include "appsettings.h"

namespace Helpers {
    class ExternalToolsProvider
    {
    private:
        ExternalToolsProvider() {}
        ExternalToolsProvider(ExternalToolsProvider const&);
        void operator=(ExternalToolsProvider const&);

    public:
        static QString getExifToolPath();

    public:
        static QString defaultExifToolPath;
        static ExternalToolsProvider &getInstance() {
            static ExternalToolsProvider instance;
            return instance;
        }
    };
}

#endif // EXTERNALTOOLSPROVIDER_H
