#ifndef GLOBALIMAGEPROVIDER_H
#define GLOBALIMAGEPROVIDER_H

 #include <QQuickImageProvider>

namespace Helpers
{
    class GlobalImageProvider : public QObject, public QQuickImageProvider
    {
        Q_OBJECT
       public:

        GlobalImageProvider(ImageType type, Flags flags = 0) : QQuickImageProvider(type, flags) {}
        ~GlobalImageProvider() {}

        QImage requestImage(const QString& id, QSize* size, const QSize& requestedSize);
        QPixmap requestPixmap(const QString& id, QSize* size, const QSize& requestedSize);
     };
}
#endif // GLOBALIMAGEPROVIDER_H
