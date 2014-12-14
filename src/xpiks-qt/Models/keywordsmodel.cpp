#include "keywordsmodel.h"

namespace Models {
    KeywordsModel::KeywordsModel(QObject *parent) {
    }



    int KeywordsModel::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return m_KeywordsList.count();
    }

    QVariant KeywordsModel::data(const QModelIndex &index, int role) const {
        if (index.row() < 0 || index.row() >= m_KeywordsList.count()) {
            return QVariant();
        }

        QString keyword = m_KeywordsList[index.row()];
        if (role == KeywordRole) {
            return keyword;
        }

        return QVariant();
    }

    QHash<int, QByteArray> KeywordsModel::roleNames() const {
        QHash<int, QByteArray> roles;
        roles[KeywordRole] = "keyword";
        return roles;
    }
}
