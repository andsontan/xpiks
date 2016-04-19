#ifndef ABSTRACTUPDATERCONFIGMODEL_H
#define ABSTRACTUPDATERCONFIGMODEL_H

#include <QSet>
#include <QJsonDocument>
#include "../Helpers/comparevaluesjson.h"
#include "../Helpers/remoteconfig.h"
#include "../Helpers/localconfig.h"
#include "../Helpers/jsonhelper.h"

namespace Models {
    class AbstractConfigUpdaterModel: public QObject, public Helpers::CompareValuesJson
    {
        Q_OBJECT
    public:
        AbstractConfigUpdaterModel(QString &url, QString &fileName, bool overwrite);
        virtual ~AbstractConfigUpdaterModel() {}

    public:
        void initHelpers();

    private slots:
        void updateHandler();

    private:
        virtual void initializeConfigList(QJsonDocument &doc) = 0;

    private:
        Helpers::RemoteConfig m_RemoteConfig;
        Helpers::LocalConfig m_LocalConfig;
        bool m_Overwrite;
        QSet<QString> m_ConfigList;
    };
}
#endif // QABSTRACTUPDATERCONFIGMODEL_H
