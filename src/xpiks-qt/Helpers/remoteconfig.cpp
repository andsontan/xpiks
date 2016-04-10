#include "RemoteConfig.h"

namespace Helpers {
    RemoteConfig::RemoteConfig(QString &Url): m_Url(Url) {
        m_NetworkManager = new QNetworkAccessManager();
        QObject::connect(m_NetworkManager, SIGNAL(finished(QNetworkReply*)),
                         this, SLOT(replyReceived(QNetworkReply*)));
    }

    RemoteConfig::~RemoteConfig() {
        delete m_NetworkManager;
    }

    void RemoteConfig::requestInitConfig() {
        LOG_INFO << "checking for updates...";
        QUrl url;
        url.setUrl(m_Url);
        QNetworkRequest request(url);
        QNetworkReply *reply = m_NetworkManager->get(request);
        Q_UNUSED(reply);
    }

    void RemoteConfig::replyReceived(QNetworkReply *networkReply) {
        LOG_WARNING << "reply handling....";
        if (networkReply->error() == QNetworkReply::NoError) {
            QJsonParseError error;
            m_Config = QJsonDocument::fromJson(networkReply->readAll(),&error);
            if (error.error == QJsonParseError::NoError){
                emit updateIsReady();
            }else{
                LOG_WARNING<<"Failed to parse remote json "<<error.errorString();
            }
        } else {
            LOG_WARNING << "Update List og ftp servers failed:" << networkReply->errorString();
        }
        networkReply->deleteLater();
    }
}
