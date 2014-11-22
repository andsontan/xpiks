#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QByteArray>
#include "keywordsfile.h"

namespace Models
{
    KeywordsFile::KeywordsFile(QString filePath)
    {
        m_FilePath = filePath;
    }

    void KeywordsFile::ParseFile(const QString &filename)
    {
        QFile keywordsFile(filename);

        if (keywordsFile.open(QIODevice::ReadOnly))
        {
            QByteArray jsonData = keywordsFile.readAll();
            QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonData);

            if (jsonDocument.isArray())
            {
                LoadAllMetadata(jsonDocument.array());
            }
            else
            {
                qWarning("Main object in file %s is not an array", qPrintable(filename));
            }
        }
    }

    void KeywordsFile::LoadAllMetadata(const QJsonArray &array)
    {
        for (int i = 0; i < array.size(); ++i)
        {
            LoadMetadata(array[i].toObject());
        }
    }

    void KeywordsFile::LoadMetadata(const QJsonObject &json)
    {
        QString imageDescription = json["image_title"].toString();
        QString shutterstockID = json["ss_id"].toString();
        QString keywords = json["keywords"].toString();
        QString categories = json["categories"].toString();

        // TODO: add image filename
        ImageMetadata metadata(imageDescription, "", keywords, categories, shutterstockID);

        m_ImagesMetadata.insert("", metadata);
    }
}
