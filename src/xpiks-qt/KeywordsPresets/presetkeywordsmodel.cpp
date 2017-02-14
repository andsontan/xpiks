/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * Xpiks is distributed under the GNU General Public License, version 3.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "presetkeywordsmodel.h"
#include "../Commands/commandmanager.h"
#include "../Helpers/stringhelper.h"

namespace KeywordsPresets {
    PresetKeywordsModel::PresetKeywordsModel(QObject *parent):
        QAbstractListModel(parent),
        Common::BaseEntity()
    {}

    PresetKeywordsModel::~PresetKeywordsModel() {
        for (auto *x: m_Finalizers) {
            delete x;
        }
    }

    bool PresetKeywordsModel::tryGetNameFromIndex(int index, QString &name) {
        if (index < 0 || index >= getPresetsCount()) {
            return false;
        }

        name = m_PresetsList[index]->m_PresetName;
        return true;
    }

    bool PresetKeywordsModel::tryGetPreset(int presetIndex, QStringList &keywords) {
        if (presetIndex < 0 || presetIndex >= getPresetsCount()) {
            return false;
        }

        auto *preset = m_PresetsList[presetIndex];
        auto &keywordsModel = preset->m_KeywordsModel;
        keywords = keywordsModel.getKeywords();
        return true;
    }

    void PresetKeywordsModel::setName(int presetIndex, const QString &name) {
        if (presetIndex < 0 || presetIndex >= getPresetsCount()) {
            return;
        }

        m_PresetsList[presetIndex]->m_PresetName = name;
    }

    bool PresetKeywordsModel::tryFindSinglePresetByName(const QString &name, bool strictMatch, int &index) {
        LOG_INFO << name;
        int foundIndex = -1;
        size_t size = m_PresetsList.size();
        bool anyError = false;

        if (!strictMatch) {
            for (size_t i = 0; i < size; ++i) {
                PresetModel *preset = m_PresetsList[i];
                if (preset->m_PresetName.contains(name, Qt::CaseInsensitive)) {
                    if (foundIndex != -1) {
                        anyError = true;
                        foundIndex = -1;
                        break;
                    } else {
                        foundIndex = (int)i;
                    }
                }
            }
        } else {
            for (size_t i = 0; i < size; ++i) {
                PresetModel *preset = m_PresetsList[i];
                if (preset->m_PresetName == name) {
                    if (foundIndex != -1) {
                        anyError = true;
                        foundIndex = -1;
                        break;
                    } else {
                        foundIndex = (int)i;
                    }
                }
            }
        }

        bool found = !anyError && (foundIndex != -1);

        if (found) {
            index = foundIndex;
            LOG_INFO << "found [" << m_PresetsList[foundIndex]->m_PresetName << "] with index" << foundIndex;
        }

        return found;
    }

    void PresetKeywordsModel::findPresetsByName(const QString &name, QVector<QPair<int, QString> > &results) {
        LOG_INFO << name;
        size_t size = m_PresetsList.size();

        for (size_t i = 0; i < size; ++i) {
            PresetModel *preset = m_PresetsList[i];

            if (preset->m_PresetName.contains(name, Qt::CaseInsensitive)) {
                results.push_back(qMakePair((int)i, preset->m_PresetName));
            }
        }
    }

    bool PresetKeywordsModel::findOrRegisterPreset(const QString &name, const QStringList &keywords, int &index) {
        LOG_INFO << name;

        bool found = false;

        int existingIndex = -1;
        if (!tryFindSinglePresetByName(name, true, existingIndex)) {
            int lastIndex = getPresetsCount();

            beginInsertRows(QModelIndex(), lastIndex, lastIndex);
            m_PresetsList.push_back(new PresetModel(name, keywords));
            endInsertRows();

            index = lastIndex;
        } else {
            found = true;
            index = existingIndex;
        }

        return found;
    }

    void PresetKeywordsModel::removeItem(int row) {
        if (row < 0 || row >= getPresetsCount()){
            return;
        }

        beginRemoveRows(QModelIndex(), row, row);
        removeInnerItem(row);
        endRemoveRows();
    }

    void PresetKeywordsModel::addItem() {
        LOG_DEBUG << "#";
        int lastIndex = getPresetsCount();

        beginInsertRows(QModelIndex(), lastIndex, lastIndex);
        m_PresetsList.push_back(new PresetModel());
        endInsertRows();
    }

    void PresetKeywordsModel::editKeyword(int index, int keywordIndex, const QString &replacement) {
        LOG_INFO << "item" << index << "keyword" << keywordIndex << "replacement" << replacement;

        if (0 <= index && index < getPresetsCount()) {
            auto &keywordsModel = m_PresetsList.at(index)->m_KeywordsModel;
            if (keywordsModel.editKeyword(keywordIndex, replacement)) {
                m_CommandManager->submitKeywordForSpellCheck(&keywordsModel, keywordIndex);
            }
        }
    }

    void PresetKeywordsModel::removeKeywordAt(int index, int keywordIndex) {
        LOG_INFO << "index" << index << "keyword" << keywordIndex;
        QString keyword;

        if (0 <= index && index < getPresetsCount()) {
            auto &keywordsModel = m_PresetsList.at(index)->m_KeywordsModel;
            if (keywordsModel.removeKeywordAt(keywordIndex, keyword)) {
                LOG_INFO << "Removed keyword:" << keyword;

                QModelIndex indexToUpdate = this->index(index);
                emit dataChanged(indexToUpdate, indexToUpdate, QVector<int>() << KeywordsCountRole);
            }
        }
    }

    void PresetKeywordsModel::removeLastKeyword(int index) {
        LOG_INFO << "index" << index;
        QString keyword;

        if (0 <= index && index < getPresetsCount()) {
            auto &keywordsModel = m_PresetsList.at(index)->m_KeywordsModel;
            if (keywordsModel.removeLastKeyword(keyword)) {
                LOG_INFO << "Removed keyword:" << keyword;

                QModelIndex indexToUpdate = this->index(index);
                emit dataChanged(indexToUpdate, indexToUpdate, QVector<int>() << KeywordsCountRole);
            }
        }
    }

    void PresetKeywordsModel::pasteKeywords(int index, const QStringList &keywords) {
        LOG_INFO << "index" << index << "keywords:" << keywords;

        if (0 <= index && index < getPresetsCount()) {
            auto *preset = m_PresetsList.at(index);
            auto &keywordsModel = preset->m_KeywordsModel;
            keywordsModel.appendKeywords(keywords);

            QModelIndex indexToUpdate = this->index(index);
            emit dataChanged(indexToUpdate, indexToUpdate, QVector<int>() << KeywordsCountRole);
        }
    }

    void PresetKeywordsModel::plainTextEdit(int index, const QString &rawKeywords, bool spaceIsSeparator) {
        LOG_INFO << "index" << index;

        if (0 <= index && index < getPresetsCount()) {
            auto *preset = m_PresetsList.at(index);
            Common::BasicKeywordsModel &keywordsModel = preset->m_KeywordsModel;

            QVector<QChar> separators;
            separators << QChar(',');
            if (spaceIsSeparator) { separators << QChar(' '); }
            QStringList keywords;
            Helpers::splitKeywords(rawKeywords.trimmed(), separators, keywords);

            keywordsModel.setKeywords(keywords);

            QModelIndex indexToUpdate = this->index(index);
            emit dataChanged(indexToUpdate, indexToUpdate, QVector<int>() << KeywordsCountRole);
        }
    }

    void PresetKeywordsModel::appendKeyword(int index, const QString &keyword) {
        LOG_INFO << "index" << index << "keyword" << keyword;

        if (0 <= index && index < getPresetsCount()) {            
            auto *preset = m_PresetsList.at(index);
            auto &keywordsModel = preset->m_KeywordsModel;
            keywordsModel.appendKeyword(keyword);
            m_CommandManager->submitKeywordForSpellCheck(&keywordsModel, keywordsModel.getKeywordsCount() - 1);

            QModelIndex indexToUpdate = this->index(index);
            emit dataChanged(indexToUpdate, indexToUpdate, QVector<int>() << KeywordsCountRole);
        }
    }

    QObject *PresetKeywordsModel::getKeywordsModel(int index) {
        QObject *result = nullptr;

        if (0 <= index && index < getPresetsCount()) {
            result = &m_PresetsList[index]->m_KeywordsModel;
            QQmlEngine::setObjectOwnership(result, QQmlEngine::CppOwnership);
        }

        return result;
    }

    void PresetKeywordsModel::saveToConfig() {        
#ifndef CORE_TESTS
        auto *presetConfig = m_CommandManager->getPresetsModelConfig();
        presetConfig->saveFromModel(m_PresetsList);
#endif
    }

    void PresetKeywordsModel::loadModelFromConfig() {
        beginResetModel();
        doLoadFromConfig();
        endResetModel();
    }

    void PresetKeywordsModel::doLoadFromConfig() {
#ifndef CORE_TESTS
        auto *presetConfig = m_CommandManager->getPresetsModelConfig();
        auto &presetData = presetConfig->m_PresetData;

        removeAllPresets();

        for (auto &item: presetData) {
            auto &keywords = item.m_Keywords;
            auto &name = item.m_Name;

            PresetModel *model = new PresetModel(name);
            model->m_KeywordsModel.setKeywords(keywords);
            m_PresetsList.push_back(model);
            m_CommandManager->submitItemForSpellCheck(&model->m_KeywordsModel, Common::SpellCheckFlags::Keywords);
        }
#endif
    }

    void PresetKeywordsModel::removeAllPresets() {
        for (auto *p: m_PresetsList) {
            if (p->release()) {
                delete p;
            } else {
                m_Finalizers.push_back(p);
            }
        }

        m_PresetsList.clear();
    }

    int PresetKeywordsModel::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return (int)m_PresetsList.size();
    }

    QVariant PresetKeywordsModel::data(const QModelIndex &index, int role) const {
        int row = index.row();

        if (row < 0 || row >= getPresetsCount()) {
            return QVariant();
        }

        auto *item = m_PresetsList[row];

        switch (role) {
        case NameRole:
            return item->m_PresetName;
        case KeywordsCountRole:
            return item->m_KeywordsModel.getKeywordsCount();
        case KeywordsStringRole:
            return item->m_KeywordsModel.getKeywordsString();
        default:
            return QVariant();
        }
    }

    bool PresetKeywordsModel::setData(const QModelIndex &index, const QVariant &value, int role) {
        int row = index.row();

        if (row < 0 || row >= getPresetsCount()) {
            return false;
        }

        switch (role) {
        case EditNameRole: {
            auto &name = m_PresetsList[row]->m_PresetName;
            auto newName = value.toString();
            QString sanitized = newName.simplified();

            if (name != sanitized) {
                LOG_INFO << "Preset" << name << "renamed to" << sanitized;
                m_PresetsList[row]->m_PresetName = sanitized;
                emit dataChanged(index, index);
                return true;
            }
        }
        }

        return false;
    }

    QHash<int, QByteArray> PresetKeywordsModel::roleNames() const {
        QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
        roles[NameRole] = "name";
        roles[EditNameRole] = "editname";
        roles[KeywordsCountRole] = "keywordscount";
        roles[KeywordsStringRole] = "keywordsstring";
        return roles;
    }

    void PresetKeywordsModel::removeInnerItem(int row) {
        Q_ASSERT(row >= 0 && row < getPresetsCount());
        PresetModel *item = m_PresetsList[row];
        m_PresetsList.erase(m_PresetsList.begin() + row);
        if (item->release()) {
            delete item;
        } else {
            m_Finalizers.push_back(item);
        }
    }

    int FilteredPresetKeywordsModel::getOriginalIndex(int index) {
        QModelIndex originalIndex = mapToSource(this->index(index, 0));
        int row = originalIndex.row();
        return row;
    }

    QString FilteredPresetKeywordsModel::getName(int index) {
        int originalIndex = getOriginalIndex(index);
        PresetKeywordsModel *presetsModel = getPresetsModel();
        QString name;

        if (!presetsModel->tryGetNameFromIndex(originalIndex, name)) {
            name = QObject::tr("Untitled");
        }

        return name;
    }

    void FilteredPresetKeywordsModel::setSearchTerm(const QString &value) {
        LOG_INFO << value;

        if (value != m_SearchTerm) {
            m_SearchTerm = value;
            emit searchTermChanged(value);
        }

        invalidateFilter();
    }

    bool FilteredPresetKeywordsModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
        Q_UNUSED(sourceParent);

        if (m_SearchTerm.trimmed().isEmpty()) {
            return true;
        }

        bool result = false;

        PresetKeywordsModel *presetsModel = getPresetsModel();
        QString name;
        if (presetsModel->tryGetNameFromIndex(sourceRow, name)) {
            result = name.contains(m_SearchTerm, Qt::CaseInsensitive);
        }

        return result;
    }

    PresetKeywordsModel *FilteredPresetKeywordsModel::getPresetsModel() const {
        QAbstractItemModel *sourceItemModel = sourceModel();
        PresetKeywordsModel *presetsModel = dynamic_cast<PresetKeywordsModel *>(sourceItemModel);
        Q_ASSERT(presetsModel != nullptr);
        return presetsModel;
    }
}
