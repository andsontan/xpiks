#ifndef PRESETKEYWORDSMODEL_H
#define PRESETKEYWORDSMODEL_H

#include "../Common/basickeywordsmodel.h"
#include "../Common/baseentity.h"
#include "../Common/abstractlistmodel.h"
#include <QAbstractListModel>
#include <QSortFilterProxyModel>

namespace KeywordsPresets {
    struct PresetModel {
        PresetModel():
            m_KeywordsModel(m_Hold),
            m_PresetName(QObject::tr("Untitled"))
        {
        }

        PresetModel(const QString &name):
            m_KeywordsModel(m_Hold),
            m_PresetName(name)
        {}

        void acquire() { m_Hold.acquire(); }
        bool release() { return m_Hold.release(); }

        Common::BasicKeywordsModel m_KeywordsModel;
        QString m_PresetName;
        Common::Hold m_Hold;
    };

    class PresetKeywordsModel:
        public QAbstractListModel,
        public Common::BaseEntity
    {
        Q_OBJECT

    public:
        PresetKeywordsModel(QObject *parent=0);
        virtual ~PresetKeywordsModel();

    public:
        int getPresetsCount() const { return (int)m_PresetsList.size(); }
        bool tryGetNameFromIndex(int index, QString &name);
        bool tryGetPreset(int presetIndex, QStringList &keywords);
        void setName(int presetIndex, const QString &name);
        bool tryFindSinglePresetByName(const QString &name, int &index) const;

    private:
        enum PresetKeywords_Roles {
            NameRole = Qt::UserRole + 1,
            EditNameRole,
            KeywordsCountRole
        };

    public:
        virtual int rowCount(const QModelIndex &parent=QModelIndex()) const override;
        virtual QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const override;
        virtual bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole) override;

    protected:
        virtual QHash<int, QByteArray> roleNames() const override;
        virtual void removeInnerItem(int row);

    public:
        Q_INVOKABLE void removeItem(int row);
        Q_INVOKABLE void addItem();
        Q_INVOKABLE void editKeyword(int index, int keywordIndex, const QString &replacement);
        Q_INVOKABLE void removeKeywordAt(int index, int keywordIndex);
        Q_INVOKABLE void removeLastKeyword(int index);
        Q_INVOKABLE void appendKeyword(int index, const QString &keyword);
        Q_INVOKABLE void pasteKeywords(int index, const QStringList &keywords);
        Q_INVOKABLE QObject *getKeywordsModel(int index);
        Q_INVOKABLE void saveToConfig();
        /*Q_INVOKABLE*/ void loadModelFromConfig();

    public slots:
        void onPresetsUpdated() { LOG_INFO << "loading Model"; loadModelFromConfig(); }

    private:
        void doLoadFromConfig();
        void removeAllPresets();

    private:
        std::vector<PresetModel *> m_PresetsList;
        std::vector<PresetModel *> m_Finalizers;
    };

    class FilteredPresetKeywordsModel:
        public QSortFilterProxyModel
    {
    Q_OBJECT
    Q_PROPERTY(QString searchTerm READ getSearchTerm WRITE setSearchTerm NOTIFY searchTermChanged)
    public:
        Q_INVOKABLE int getOriginalIndex(int index);
        Q_INVOKABLE int getItemsCount() const { return rowCount(); }
        Q_INVOKABLE QString getName(int index);
    public:
        const QString &getSearchTerm() const { return m_SearchTerm; }
        void setSearchTerm(const QString &value);
    signals:
        void searchTermChanged(const QString &searchTerm);
        // QSortFilterProxyModel interface
    protected:
        virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    private:
        PresetKeywordsModel *getPresetsModel() const;
    private:
        QString m_SearchTerm;
    };
}
#endif // PRESETKEYWORDSMODEL_H
