#ifndef PRESETKEYWORDSMODEL_H
#define PRESETKEYWORDSMODEL_H

#include "../Common/basickeywordsmodel.h"
#include "../Common/baseentity.h"
#include "../Common/abstractlistmodel.h"
#include <QAbstractListModel>
#include <QSortFilterProxyModel>

namespace KeywordsPreset {
    struct Preset {
        using Model = Common::BasicKeywordsModel;
        using ModelPtr = std::shared_ptr<Model>;
        Preset() = default;
        Preset(QString name):
            m_KeywordsModel(new Model(m_HoldPlaceholder)),
            m_PresetName(name)
        {}
        ModelPtr m_KeywordsModel;
        QString m_PresetName;
        Common::Hold m_HoldPlaceholder;
    };

    class PresetKeywordsModel:
        public QAbstractListModel,
        public Common::BaseEntity
    {
    Q_OBJECT

    public:
        PresetKeywordsModel(QObject *parent=0);
        size_t getPresetsCount() const {return m_PresetsList.size(); }
        bool tryGetIndexFromName(const QString &presetName, int &result);
        bool tryGetNameFromIndex(int index, QString &name);
        bool tryGetPreset(int presetIndex, QStringList &keywords);
        void setName(int presetIndex, const QString &name);

    public:
        enum PresetKeywords_Roles {
            NameRole = Qt::UserRole + 1
        };

        virtual int rowCount(const QModelIndex &parent=QModelIndex()) const;
        virtual QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
        virtual bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);

    protected:
        virtual QHash<int, QByteArray> roleNames() const;
        virtual void removeInnerItem(int row);

    public:
        Q_INVOKABLE void removeItem(int row);
        Q_INVOKABLE void addItem();
        Q_INVOKABLE void editKeyword(int index, int keywordIndex, const QString &replacement);
        Q_INVOKABLE QString removeKeywordAt(int index, int keywordIndex);
        Q_INVOKABLE void appendKeyword(int index, const QString &keyword);
        Q_INVOKABLE QObject *getKeywordsModel(int index);
        Q_INVOKABLE void saveToConfig();
        Q_INVOKABLE void loadFromConfigModel();

    public slots:
        void onPresetsUpdated() { LOG_INFO << "loading Model"; loadFromConfigModel(); }

    private:
        std::vector<Preset> m_PresetsList;
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
        const QString &getSearchTerm() const;
        void setSearchTerm(const QString &value);
    signals:
        void searchTermChanged(const QString &searchTerm);
        // QSortFilterProxyModel interface
    protected:
        virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
    private:
        PresetKeywordsModel *getPresetsModel() const;
    private:
        QString m_SearchTerm;
    };
}
#endif // PRESETKEYWORDSMODEL_H
