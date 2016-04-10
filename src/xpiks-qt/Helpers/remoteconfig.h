#ifndef RemoteConfig_H
#define RemoteConfig_H

#include <QObject>
#include <QString>
#include <QNetworkReply>
#include <QJsonDocument>
#include "../Common/defines.h"

namespace Helpers {
class RemoteConfig: public QObject {
    Q_OBJECT
  public:
    RemoteConfig(QString &Url);
    ~RemoteConfig();
    void requestInitConfig();
    QJsonDocument& getConfig() { return m_Config; }

  signals:
    void updateIsReady();

  private slots:
    void replyReceived(QNetworkReply *networkReply);

  private:
    QString m_Url;
    QNetworkAccessManager *m_NetworkManager;
    QJsonDocument m_Config;
};
}

#endif // RemoteConfig_H
