#ifndef IFILENOTAVAILABLEMODEL_H
#define IFILENOTAVAILABLEMODEL_H
#include <QObject>

namespace Helpers {
class IFileNotAvailableModel
{
public:
    virtual void removeUnavailableItems()=0;
    virtual void UpdateMyself()=0;
};
}
#endif // IFILENOTAVAILABLEMODEL_H
