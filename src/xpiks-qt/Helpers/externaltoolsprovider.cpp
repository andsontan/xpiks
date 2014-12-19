#include "externaltoolsprovider.h"
#include "appsettings.h"

namespace Helpers {
    QString ExternalToolsProvider::defaultExifToolPath = "exiftoo2";

    QString Helpers::ExternalToolsProvider::getExifToolPath()
    {
        QVariant value = AppSettings().value(Constants::PATH_TO_EXIFTOOL, ExternalToolsProvider::defaultExifToolPath);
        return value.toString();
    }
}
