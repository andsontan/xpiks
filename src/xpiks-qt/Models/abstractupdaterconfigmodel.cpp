#include "abstractupdaterconfigmodel.h"

namespace Models {
    AbstractUpdaterConfigModel::AbstractUpdaterConfigModel(QString & Url, QString & fileName, bool overwrite)
    {
        m_RemoteConfig = new Helpers::RemoteConfig(Url);
        m_LocalConfig = new Helpers::LocalConfig(fileName);
        m_Overwrite = overwrite;
        QObject::connect(m_RemoteConfig, SIGNAL(updateIsReady()), this, SLOT(updateHandler()));
    }

    AbstractUpdaterConfigModel::~AbstractUpdaterConfigModel()
    {
        delete m_RemoteConfig;
        delete m_LocalConfig;
    }


    void AbstractUpdaterConfigModel::initHelpers(){
        m_LocalConfig->initConfig();
        m_RemoteConfig->requestInitConfig();
    }



    void AbstractUpdaterConfigModel::updateHandler(){
        QJsonDocument localDoc = m_LocalConfig->getConfig();
        QJsonDocument remoteDoc = m_RemoteConfig->getConfig();
        Helpers::mergeJson(localDoc,remoteDoc,m_Overwrite,*this);
        m_LocalConfig->saveToFile();
        initializeConfigList(localDoc);
    }

}
