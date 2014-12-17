#include "globalimageprovider.h"

namespace Helpers
{
    QImage GlobalImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
    {
        QImage image(id);
        QImage result;

        if (requestedSize.isValid()) {
            result = image.scaled(requestedSize, Qt::KeepAspectRatio);
        }
        else {
            result = image;
        }

        *size = result.size();
        return result;
    }

    QPixmap GlobalImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
    {
        QPixmap image(id);
        QPixmap result;

        if (requestedSize.isValid()) {
            result = image.scaled(requestedSize, Qt::KeepAspectRatio);
        }
        else {
            result = image;
        }

        *size = result.size();
        return result;
    }
}
