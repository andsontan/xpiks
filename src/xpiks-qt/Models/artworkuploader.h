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

#ifndef ARTWORKUPLOADER_H
#define ARTWORKUPLOADER_H

#include <QAbstractListModel>
#include <QStringList>
#include <QFutureWatcher>
#include "artworksprocessor.h"
#include "../Conectivity/testconnection.h"
#include "../AutoComplete/stringfilterproxymodel.h"
#include "../AutoComplete/stocksftplistmodel.h"
#include "../Conectivity/uploadwatcher.h"

namespace Helpers {
    class TestConnectionResult;
}

namespace Conectivity {
    class IFtpCoordinator;
}

namespace Commands {
    class CommandManager;
}

namespace Models {
    class ArtworkMetadata;

    class ArtworkUploader:
        public ArtworksProcessor
    {
    Q_OBJECT

    public:
        ArtworkUploader(Conectivity::IFtpCoordinator *ftpCoordinator, QObject *parent=0);
        virtual ~ArtworkUploader();

    public:
        AutoComplete::StringFilterProxyModel *getStocksCompletionSource() { return &m_StocksCompletionSource; }
        virtual void setCommandManager(Commands::CommandManager *commandManager);

    signals:
        void percentChanged();
        void credentialsChecked(bool result, const QString &url);

    public:
        int getPercent() const { return m_Percent; }

    public slots:
        void onUploadStarted();
        void allFinished(bool anyError);
        void credentialsTestingFinished();

    private slots:
        void uploaderPercentChanged(double percent);
        void stocksListUpdated();

    public:
#ifndef CORE_TESTS
        Q_INVOKABLE void uploadArtworks();
        Q_INVOKABLE void checkCredentials(const QString &host, const QString &username,
                                          const QString &password, bool disablePassiveMode, bool disableEPSV) const;

#endif
        Q_INVOKABLE bool needCreateArchives() const;

        Q_INVOKABLE QString getFtpAddress(const QString &stockName) const { return m_StocksFtpList.getFtpAddress(stockName); }
#ifndef CORE_TESTS
        Q_INVOKABLE QString getFtpName(const QString &stockAddress) const;

#endif
        Q_INVOKABLE QObject *getUploadWatcher() {
            auto *model = &m_UploadWatcher;
            QQmlEngine::setObjectOwnership(model, QQmlEngine::CppOwnership);

            return model;
        }

        void initializeStocksList();

    private:
#ifndef CORE_TESTS
        void doUploadArtworks(const QVector<ArtworkMetadata *> &artworkList);

#endif

    protected:
        virtual void cancelProcessing();

        virtual void innerResetModel() { m_Percent = 0; }

    private:
        Conectivity::UploadWatcher m_UploadWatcher;
        Conectivity::IFtpCoordinator *m_FtpCoordinator;
        AutoComplete::StringFilterProxyModel m_StocksCompletionSource;
        AutoComplete::StocksFtpListModel m_StocksFtpList;
#ifndef CORE_TESTS
        QFutureWatcher<Conectivity::ContextValidationResult> *m_TestingCredentialWatcher;
#endif
        int m_Percent;
    };
}

#endif // ARTWORKUPLOADER_H
