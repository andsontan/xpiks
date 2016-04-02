#include "removeconfigupdater.h"

#define FTP_LIST_URL  "https://raw.githubusercontent.com/RostaTasha/muha_slon/master/trash/stocks_ftp.json"
namespace Conectivity {

    RemoveConfigUpdater::RemoveConfigUpdater() {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
#else
    QString path = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
#endif
        QDir dir(path);
        path = dir.filePath(Constants::FTP_STOCKS_FILENAME);
        m_StocksFilePath = path;
        m_StocksUrl = FTP_LIST_URL;
        m_StocksFtp.clear();
        m_StocksFtpVersion=0;
        m_NetworkManager = new QNetworkAccessManager();
        QObject::connect(m_NetworkManager, SIGNAL(finished(QNetworkReply*)),
                         this, SLOT(replyReceived(QNetworkReply*)));
    }

    void RemoveConfigUpdater::initFromFile(){
        QString text = readFile();
        if (text.isEmpty() == false) {
            QJsonDocument jd = QJsonDocument::fromJson(text.toUtf8());
            m_StocksFtpVersion = getVersionFromJson(jd);
            m_StocksFtp = getStocksFromJson(jd);
        }
    }

    int  RemoveConfigUpdater::getVersionFromJson(QJsonDocument & jd) {
        return jd.object()["version"].toInt();

    }

    int  RemoveConfigUpdater::getOverWriteFlagFromJson(QJsonDocument & jd) {
        return (jd.object()["overwrite"].toString() == "true" ? 1 : 0);

    }
    QMap<QString,QString>  RemoveConfigUpdater::getStocksFromJson(QJsonDocument & jd) {
        QMap<QString,QString> stocksFtp;
        QJsonArray dataObject = jd.object()["stocks_ftp"].toArray();
        int size = dataObject.size();
        for (int i = 0; i < size; i++){
            QJsonObject obj = dataObject.at(i).toObject();
            stocksFtp.insert(obj.value("name").toString(),obj.value("ftp").toString());
        }
        return stocksFtp;
    }
    void RemoveConfigUpdater::update(QMap<QString,QString> & local,QMap<QString,QString> & remote,int overwrite) {
        if (overwrite) {
            local = remote;
        } else {
            compareAndUpdate(local, remote);
        }
    }


    void RemoveConfigUpdater::compareAndUpdate(QMap<QString,QString> & local, QMap<QString,QString> & remote) {
        QMap<QString,QString>::iterator start,end,it;
        start = remote.begin();
        end = remote.end();
        for (it = start; it != end; it++){
            QString key = it.key();
            QString value = it.value();
            local[key] = value;
        }
    }

    QString RemoveConfigUpdater::readFile() {
        QString text;
        QFile file(m_StocksFilePath);
        if (file.open(QIODevice::ReadOnly)) {
            text = file.readAll();
            file.close();
        } else {
            LOG_WARNING << "opening file " << m_StocksFilePath<<" failed";
        }
        return text;
    }

    void RemoveConfigUpdater::process() {
        LOG_INFO << "checking for updates...";
        QString queryString = QString(m_StocksUrl);
        QUrl url;
        url.setUrl(queryString);
        QNetworkRequest request(url);
        QNetworkReply *reply = m_NetworkManager->get(request);
        Q_UNUSED(reply);
    }


    void RemoveConfigUpdater::replyReceived(QNetworkReply *networkReply) {
         LOG_WARNING << "reply handling....";
        if (networkReply->error() == QNetworkReply::NoError) {
            QJsonDocument document = QJsonDocument::fromJson(networkReply->readAll());
            int version = getVersionFromJson(document);
            if (version > m_StocksFtpVersion) {
                m_StocksFtpVersion = version;
                QMap<QString,QString> remote = getStocksFromJson(document);
                int overwriteFlag = getOverWriteFlagFromJson(document);
                update(m_StocksFtp, remote,overwriteFlag);
                updateRemoteFile();
            }
        }else {
            LOG_WARNING << "Update List og ftp servers failed:" << networkReply->errorString();
        }
        networkReply->deleteLater();
    }

    void RemoveConfigUpdater::updateRemoteFile() {
        QFile file(m_StocksFilePath);
        if (file.open(QIODevice::WriteOnly)) {
            QMap<QString,QString>::iterator start, end, it;
            start = m_StocksFtp.begin();
            end = m_StocksFtp.end();
            QJsonArray array;
            for (it = start; it != end; it++){
                QString key = it.key();
                QString value = it.value();
                QJsonObject object;
                object.insert("name", key);
                object.insert("ftp", value);
                array.append(object);
            }
            QJsonObject all;
            all.insert("version", m_StocksFtpVersion);
            all.insert("stocks_ftp", array);
            QJsonDocument jd;
            jd.setObject(all);
            file.write(jd.toJson(QJsonDocument::Compact));
            file.close();
        } else {
            LOG_WARNING << "opening file " << m_StocksFilePath<<" failed";
        }
    }


    RemoveConfigUpdater::~RemoveConfigUpdater() {
        if (m_NetworkManager != NULL) {
            delete m_NetworkManager;
        }
    }
}

