#ifndef IMAGEMETADATA_H
#define IMAGEMETADATA_H

#include <QStringListModel>
#include <QStringList>
#include <QString>
#include <QSet>

namespace Models {
    class ArtworkMetadata : public QObject {
        Q_OBJECT
    public:
        ArtworkMetadata(const QString &filepath) :
            m_ArtworkFilepath(filepath),
            m_IsModified(false),
            m_IsSelected(false)
        { }

    public:
        void initialize(const QString &author, const QString &title,
                        const QString &description, const QString &rawKeywords);

    public:
        const QString &getAuthor() const { return m_ArtworkAuthor; }
        const QString &getTitle() const { return m_ArtworkTitle; }
        const QString &getDescription() const { return m_ArtworkDescription; }
        const QString &getFilepath() const { return m_ArtworkFilepath; }
        const QStringList &getKeywords() const { return m_KeywordsList; }
        const QSet<QString> &getKeywordsSet() const { return m_KeywordsSet; }
        QString getKeywordsString() const { return m_KeywordsList.join(','); }
        bool isInDirectory(const QString &directory) const { return m_ArtworkFilepath.startsWith(directory); }
        bool isModified() const { return m_IsModified; }
        bool getIsSelected() const { return m_IsSelected; }

    public:
        void setDescription(const QString &value) {
            if (m_ArtworkDescription != value) {
                m_ArtworkDescription = value;
                setModified();
            }
        }

        void setAuthor(const QString &value) {
            if (m_ArtworkAuthor != value) {
                m_ArtworkAuthor = value;
                setModified();
            }
        }

        void setTitle(const QString &value) {
            if (m_ArtworkTitle != value) {
                m_ArtworkTitle = value;
                setModified();
            }
        }

        void resetModified() { m_IsModified = false; emit modifiedChanged(m_IsModified); }
        void setIsSelected(bool value) { m_IsSelected = value; }

    public:
        bool removeKeywordAt(int index);
        bool removeLastKeyword() { return removeKeywordAt(m_KeywordsList.length() - 1); }
        bool appendKeyword(const QString &keyword);

    public:
        void setKeywords(const QStringList &keywordsList) { resetKeywords(); appendKeywords(keywordsList); }
        void appendKeywords(const QStringList &keywordsList);

    private:
        void resetKeywords();
        void setModified() { m_IsModified = true; emit modifiedChanged(m_IsModified); }

    public:
        void addKeywords(const QString& rawKeywords);
        void unsetModified() { m_IsModified = false; }

    signals:
         void modifiedChanged(bool newValue);

    private:
        QStringList m_KeywordsList;
        QSet<QString> m_KeywordsSet;
        QString m_ArtworkFilepath;
        QString m_ArtworkDescription;
        QString m_ArtworkTitle;
        QString m_ArtworkAuthor;
        bool m_IsModified;
        bool m_IsSelected;
    };
}

#endif // IMAGEMETADATA_H
