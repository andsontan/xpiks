/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2016 Taras Kushnir <kushnirTV@gmail.com>
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

#include "warningsmanager.h"
#include <QtConcurrent>
#include <QFuture>
#include <QDebug>
#include "warningsinfo.h"
#include "../Helpers/globalimageprovider.h"
#include "../Helpers/appsettings.h"
#include "../Models/artiteminfo.h"
#include "../Commands/commandmanager.h"
#include "settingsmodel.h"
#include "../Common/defines.h"

namespace Models {
    WarningsManager::WarningsManager(QObject *parent) :
        QAbstractListModel(parent),
        Common::BaseEntity()
    {
        m_CheckingWatcher = new QFutureWatcher<QVector<WarningsInfo*> >();
        QObject::connect(m_CheckingWatcher, SIGNAL(finished()),
                         this, SLOT(warningsCheckFinished()));
    }

    WarningsManager::~WarningsManager() {
        qDeleteAll(m_WarningsList);
        delete m_CheckingWatcher;
    }

    void WarningsManager::onCheckWarnings(const QVector<ArtItemInfo *> &artworks) {
        clearModel();
        checkForWarnings(artworks);
    }

    void WarningsManager::warningsCheckFinished() {
        qInfo() << "Warnings check finished. Updating main model...";
        QVector<WarningsInfo*> items = m_CheckingWatcher->result();

        beginResetModel();

        qDeleteAll(m_WarningsList);
        m_WarningsList.clear();

        int length = items.length();
        for (int i = 0; i < length; ++i) {
            WarningsInfo *item = items[i];
            if  (!item->hasWarnings()) {
                delete item;
            } else {
                m_WarningsList.append(item);
            }
        }

        endResetModel();

        emit warningsCountChanged();
    }

    void WarningsManager::checkForWarnings(const QVector<ArtItemInfo *> &artworks) {
        qDebug() << "Check for warnings" << artworks.length() << "item(s)";
        QVector<WarningsInfo*> warningsBufferList;

        warningsBufferList.reserve(artworks.length());

        foreach (ArtItemInfo *itemInfo, artworks) {
            warningsBufferList.append(new WarningsInfo(itemInfo));
        }

        initConstraintsFromSettings();

        QFuture<QVector<WarningsInfo*> > future = QtConcurrent::run<QVector<WarningsInfo*> >(this, &WarningsManager::doCheckItems, warningsBufferList);
        m_CheckingWatcher->setFuture(future);

        m_CommandManager->reportUserAction(Conectivity::UserActionWarningsCheck);
    }

    void WarningsManager::recheckItem(int itemIndex) {
        if (itemIndex < 0 || itemIndex >= m_WarningsList.length()) {
            return;
        }

        qInfo() << "Rechecking item for warnings with index" << itemIndex;

        WarningsInfo *info = m_WarningsList.at(itemIndex);
        info->updateData();
        info->clearWarnings();

        if (!checkItem(info)) {
            beginRemoveRows(QModelIndex(), itemIndex, itemIndex);
            m_WarningsList.removeAt(itemIndex);
            endRemoveRows();
        } else {
            emit dataChanged(this->index(itemIndex), this->index(itemIndex));
        }
    }

    void WarningsManager::clearModel() {
        qDebug() << "Clearing warnings...";
        beginResetModel();

        qDeleteAll(m_WarningsList);
        m_WarningsList.clear();

        endResetModel();
    }

    QVector<WarningsInfo *> WarningsManager::doCheckItems(const QVector<WarningsInfo *> &items) const {
        int length = items.length();
        for (int i = 0; i < length; ++i) {
            checkItem(items[i]);
        }

        return items;
    }

    bool WarningsManager::checkItem(WarningsInfo *wi) const {
        bool hasWarnings = false;

        hasWarnings = checkDimensions(wi) || hasWarnings;
        hasWarnings = checkKeywordsCount(wi) || hasWarnings;
        hasWarnings = checkDescriptionLength(wi) || hasWarnings;
        hasWarnings = checkTitleWordsCount(wi) || hasWarnings;
        hasWarnings = checkSpellCheckErrors(wi) || hasWarnings;

        return hasWarnings;
    }

    bool WarningsManager::checkDimensions(WarningsInfo *wi) const {
        bool hasWarnings = false;

        QSize size = wi->getSize();

        double currentProd = size.width() * size.height() / 1000000.0;
        if (currentProd < m_MinimumMegapixels) {
            QString warning = QString("Image size (%1 Megapixels) is less than minimum (%2 Megapixels)")
                    .arg(currentProd, 2)
                    .arg(m_MinimumMegapixels);
            wi->addWarning(warning);
            hasWarnings = true;
        }

        return hasWarnings;
    }

    bool WarningsManager::checkKeywordsCount(WarningsInfo *wi) const {
        bool hasWarnings = false;
        int keywordsCount = wi->getKeywordsCount();

        if (keywordsCount == 0) {
            wi->addWarning("Image has no keywords");
            hasWarnings = true;
        }

        if (keywordsCount > m_MaximumKeywordsCount) {
            QString warning = QString("Keywords count (%1) exceeds maximum (%2)")
                    .arg(keywordsCount)
                    .arg(m_MaximumKeywordsCount);
            wi->addWarning(warning);
            hasWarnings = true;
        }

        return hasWarnings;
    }

    bool WarningsManager::checkDescriptionLength(WarningsInfo *wi) const {
        bool hasWarnings = false;
        int descriptionLength = wi->getDescriptionLength();

        if (descriptionLength == 0) {
            wi->addWarning("Description is empty");
            hasWarnings = true;
        } else {
            if (descriptionLength > m_MaximumDescriptionLength) {
                QString warning = QString("Description length (%1) exceeds maximum (%2)")
                        .arg(descriptionLength)
                        .arg(m_MaximumDescriptionLength);
                wi->addWarning(warning);
                hasWarnings = true;
            }

            int wordsLength = wi->getDescriptionWordsCount();
            if (wordsLength < 3) {
                QString warning = QString("Description should contain at least three words");
                wi->addWarning(warning);
                hasWarnings = true;
            }
        }

        return hasWarnings;
    }

    bool WarningsManager::checkTitleWordsCount(WarningsInfo *wi) const {
        bool hasWarnings = false;

        if (wi->getTitleLength() > 0) {
            int partsLength = wi->getTitleWordsCount();

            if (partsLength < 3) {
                QString warning = QString("Title should contain at least three words");
                wi->addWarning(warning);
                hasWarnings = true;
            }

            if (partsLength > 10) {
                QString warning = QString("Title should not contain more than ten words");
                wi->addWarning(warning);
                hasWarnings = true;
            }
        } else {
            wi->addWarning("Title is empty and will be copied from description");
            hasWarnings = true;
        }

        return hasWarnings;
    }

    bool WarningsManager::checkSpellCheckErrors(WarningsInfo *wi) const {
        bool hasWarnings = false;

        if (wi->hasKeywordsSpellErrors()) {
            wi->addWarning("Item has spell errors in keywords");
            hasWarnings = true;
        }

        if (wi->hasDescriptionSpellErrors()) {
            wi->addWarning("Item has spell errors in description");
            hasWarnings = true;
        }

        if (wi->hasTitleSpellErrors()) {
            wi->addWarning("Item has spell errors in title");
            hasWarnings = true;
        }

        return hasWarnings;
    }

    void WarningsManager::initConstraintsFromSettings() {
        SettingsModel *settingsModel = m_CommandManager->getSettingsModel();
        m_MaximumDescriptionLength = settingsModel->getMaxDescriptionLength();
        m_MinimumMegapixels = settingsModel->getMinMegapixelCount();
        m_MaximumKeywordsCount = settingsModel->getMaxKeywordsCount();
    }

    int WarningsManager::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return m_WarningsList.count();
    }

    QVariant WarningsManager::data(const QModelIndex &index, int role) const {
        int row = index.row();
        if (row < 0 || row >= m_WarningsList.count())
            return QVariant();

        WarningsInfo *warningsInfo = m_WarningsList.at(row);

        switch (role) {
        case ImagePathRole:
            return warningsInfo->getFilePath();
        case WarningsListRole:
            return warningsInfo->getWarnings();
        case ItemIndexRole:
            return warningsInfo->getIndex();
        default:
            return QVariant();
        }
    }

    QHash<int, QByteArray> WarningsManager::roleNames() const {
        QHash<int, QByteArray> roles;
        roles[ImagePathRole] = "filename";
        roles[WarningsListRole] = "warnings";
        roles[ItemIndexRole] = "itemindex";
        return roles;
    }
}
