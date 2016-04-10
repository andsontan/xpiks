#include "localconfig.h"

namespace Helpers {

    LocalConfig::LocalConfig(QString fileName):
        m_FileName(fileName) {
    }

    void LocalConfig::initConfig() {
        QString text;
        QFile file(m_FileName);
        if (file.open(QIODevice::ReadOnly)) {
            text = file.readAll();
            file.close();
            m_Config = QJsonDocument::fromJson(text.toUtf8());
        } else {
            LOG_WARNING << "opening file " << m_FileName<<" failed";
        }
    }

    void LocalConfig::saveToFile() {
        QFile file(m_FileName);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(m_Config.toJson(QJsonDocument::Indented));
            file.close();
        } else {
            LOG_WARNING << "opening file " << m_FileName<<" failed";
        }
    }
}

