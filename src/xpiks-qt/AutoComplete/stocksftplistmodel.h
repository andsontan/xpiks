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

#ifndef STOCKSFTPLISTMODEL_H
#define STOCKSFTPLISTMODEL_H

#include <QHash>
#include <QList>
#include <QJsonDocument>
#include <QJsonArray>
#include "../Models/abstractconfigupdatermodel.h"

namespace AutoComplete {
    class StocksFtpListModel: public Models::AbstractConfigUpdaterModel
    {
        Q_OBJECT
    public:
        StocksFtpListModel();

    public:
        void initializeConfigs();
        QString getFtpAddress(const QString &stockName) const { return m_StocksHash.value(stockName, ""); }
        const QStringList &getStockNamesList() const { return m_StockNames; }

        // AbstractConfigUpdaterModel interface
    protected:
        virtual void processRemoteConfig(const QJsonDocument &remoteDocument, bool overwriteLocal) override;
        virtual bool parseConfig(const QJsonDocument &document) override;

    private:
        void parseFtpArray(const QJsonArray &array);

    signals:
        void stocksListUpdated();

        // CompareValuesJson interface
    public:
        virtual int operator ()(const QJsonObject &val1, const QJsonObject &val2) override;

    private:
        QHash<QString, QString> m_StocksHash;
        QStringList m_StockNames;
    };
}

#endif // STOCKSFTPLISTMODEL_H
