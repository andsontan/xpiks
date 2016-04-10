#ifndef ABSTRACTUPDATERCONFIGMODEL_H
#define ABSTRACTUPDATERCONFIGMODEL_H

#include "../Helpers/comparevaluesjson.h"
#include "../Helpers/remoteconfig.h"
#include "../Helpers/localconfig.h"
#include "../Helpers/jsonhelper.h"
#include <QSet>
#include <QJsonDocument>

namespace Models {

    class AbstractUpdaterConfigModel:public Helpers::CompareValuesJson, public QObject
    {
    public:
        AbstractUpdaterConfigModel(QString & Url, QString & fileName, bool overwrite);
        ~AbstractUpdaterConfigModel();
        void initHelpers();
    private slots:
        void updateHandler();
    private:
        virtual void initializeConfigList(QJsonDocument & doc)=0;
    private:
        Helpers::RemoteConfig *m_RemoteConfig;
        Helpers::LocalConfig *m_LocalConfig;
        bool m_Overwrite;
        QSet<QString> m_ConfigList;

    };

}
#endif // QABSTRACTUPDATERCONFIGMODEL_H
