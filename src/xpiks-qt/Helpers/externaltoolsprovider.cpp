#include "externaltoolsprovider.h"
#include "appsettings.h"

namespace Helpers {
    QString ExternalToolsProvider::defaultExifToolPath = "exiftool";
    QString ExternalToolsProvider::defaultCurlPath = "curl";

    QString Helpers::ExternalToolsProvider::getExifToolPath()
    {
        QVariant value = AppSettings().value(Constants::PATH_TO_EXIFTOOL, ExternalToolsProvider::defaultExifToolPath);
        return value.toString();
    }

    QString Helpers::ExternalToolsProvider::getCurlPath()
    {
        QVariant value = AppSettings().value(Constants::PATH_TO_CURL, ExternalToolsProvider::defaultCurlPath);
        return value.toString();
    }
}
