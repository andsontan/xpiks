#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include <QHash>
#include "imagemetadata.h"

#ifndef KEYWORDSSOURCE_H
#define KEYWORDSSOURCE_H

namespace Models
{
    class KeywordsFile
    {
    public:
        KeywordsFile(QString filePath);

        const QString &GetFilePath() const { return m_FilePath; }

    private:
        void LoadAllMetadata(const QJsonArray &json);
        void LoadMetadata(const QJsonObject &json);
        void ParseFile(const QString &filename);

    private:
        QString m_FilePath;
        QHash<QString, ImageMetadata> m_ImagesMetadata;
    };
}

#endif // KEYWORDSSOURCE_H
