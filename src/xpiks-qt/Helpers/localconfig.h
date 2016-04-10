#ifndef LOCALCONFIG_H
#define LOCALCONFIG_H

#include <QString>
#include <QJsonDocument>
#include <QFile>
#include "../Common/defines.h"

namespace Helpers {

class LocalConfig {
  public:
    LocalConfig(QString fileName);
    void initConfig();
    void saveToFile();
    QJsonDocument& getConfig() { return m_Config; }

  private:
    QString m_FileName;
    QJsonDocument m_Config;

};
}

#endif // LOCALCONFIG_H
