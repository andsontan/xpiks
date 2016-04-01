#ifndef REMOVECONFIGUPDATER_H
#define REMOVECONFIGUPDATER_H

#include <QString>
#include <QMap>
#include <QNetworkAccessManager>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDir>
#include <QStandardPaths>
#include "../Common/defines.h"
#include "../Helpers/constants.h"

namespace Conectivity {

class RemoveConfigUpdater: public QObject{
    Q_OBJECT
public:
    void process();
    void initFromFile();
    QMap<QString,QString>  getStocksFtp() const { return m_StocksFtp; }

public:
    explicit RemoveConfigUpdater();
    ~RemoveConfigUpdater();

private:
    int getVersionFromJson(QJsonDocument & jd);
    QMap<QString,QString> getStocksFromJson(QJsonDocument & jd);
    int  getOverWriteFlagFromJson(QJsonDocument & jd);
    QString readFile();
    void compareAndUpdate(QMap<QString,QString> & local,QMap<QString,QString> & remote);
    void update(QMap<QString,QString> & local,QMap<QString,QString> & remote, int overwrite);
    void updateRemoteFile();

private slots:
    void replyReceived(QNetworkReply *networkReply);

private:
    QMap<QString,QString> m_StocksFtp;
    QNetworkAccessManager *m_NetworkManager;
    int m_StocksFtpVersion;
    QString m_StocksUrl;
    QString m_StocksFilePath;

};
}

#endif // REMOVECONFIGUPDATER_H
