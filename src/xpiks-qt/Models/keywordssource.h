#include <QString>

#ifndef KEYWORDSSOURCE_H
#define KEYWORDSSOURCE_H

namespace Models
{
    class KeywordsSource
    {
    public:
        KeywordsSource(QString filePath);

        const QString &GetFilePath() const { return m_FilePath; }

    private:
        QString m_FilePath;
    };
}

#endif // KEYWORDSSOURCE_H
