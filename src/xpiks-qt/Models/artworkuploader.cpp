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

#include "artworkuploader.h"
#include <QtConcurrent>
#include <QFileInfo>
#include <QDebug>
#include "uploadinforepository.h"
#include "uploadinfo.h"
#include "../Helpers/ziphelper.h"
#include "../Models/artworkmetadata.h"
#include "../Commands/commandmanager.h"
#include "../Models/settingsmodel.h"
#include "../Helpers/filenameshelpers.h"
#include "../Conectivity/ftpcoordinator.h"
#include "../Conectivity/testconnection.h"
#include "../Conectivity/uploadcontext.h"

namespace Models {
    ArtworkUploader::ArtworkUploader(int maxParallelUploads) :
        ArtworksProcessor(),
        m_IncludeVector(false),
        m_Percent(0)
    {
        m_FtpCoordinator = new Conectivity::FtpCoordinator(maxParallelUploads);

        QObject::connect(m_FtpCoordinator, SIGNAL(uploadStarted()), this, SLOT(onUploadStarted()));
        QObject::connect(m_FtpCoordinator, SIGNAL(uploadFinished(bool)), this, SLOT(allFinished(bool)));
        QObject::connect(m_FtpCoordinator, SIGNAL(overallProgressChanged(double)), this, SLOT(uploaderPercentChanged(double)));

        m_TestingCredentialWatcher = new QFutureWatcher<Conectivity::ContextValidationResult>(this);
        QObject::connect(m_TestingCredentialWatcher, SIGNAL(finished()), SLOT(credentialsTestingFinished()));
    }

    ArtworkUploader::~ArtworkUploader() {
        delete m_TestingCredentialWatcher;
        delete m_FtpCoordinator;
    }

    void ArtworkUploader::setCommandManager(Commands::CommandManager *commandManager) {
        ArtworksProcessor::setCommandManager(commandManager);
        m_FtpCoordinator->setCommandManager(commandManager);
    }

    void ArtworkUploader::onUploadStarted() {
        beginProcessing();
        m_Percent = 0;
        updateProgress();
    }

    void ArtworkUploader::allFinished(bool anyError) {
        qInfo() << "Upload finished with status anyError =" << anyError;
        setIsError(anyError);
        endProcessing();
        m_Percent = 100;
        updateProgress();
    }

    void ArtworkUploader::credentialsTestingFinished() {
        Conectivity::ContextValidationResult result = m_TestingCredentialWatcher->result();
        emit credentialsChecked(result.m_Result, result.m_Host);
    }

    void ArtworkUploader::uploaderPercentChanged(double percent) {
        m_Percent = (int)(percent);
        qDebug() << "Overall upload progress changed to" << percent;
        updateProgress();
        UploadInfoRepository *uploadInfoRepository = m_CommandManager->getUploadInfoRepository();
        uploadInfoRepository->updatePercentages();
    }

    void ArtworkUploader::uploadArtworks() { doUploadArtworks(getArtworkList()); }

    void ArtworkUploader::checkCredentials(const QString &host, const QString &username,
                                           const QString &password, bool disablePassiveMode) const {
        Conectivity::UploadContext *context = new Conectivity::UploadContext();
        context->m_Host = host;
        context->m_Username = username;
        context->m_Password = password;
        context->m_TimeoutSeconds = 10;
        context->m_UsePassiveMode = !disablePassiveMode;

        m_TestingCredentialWatcher->setFuture(QtConcurrent::run(Conectivity::isContextValid, context));
    }

    bool ArtworkUploader::needCreateArchives() const {
        bool anyZipNeeded = false;
        const UploadInfoRepository *uploadInfoRepository = m_CommandManager->getUploadInfoRepository();
        const QVector<Models::UploadInfo *> &infos = uploadInfoRepository->getUploadInfos();
        foreach (Models::UploadInfo *info, infos) {
            if (info->getIsSelected() && info->getZipBeforeUpload()) {
                anyZipNeeded = true;
                qDebug() << "Need create archives at least for" << info->getHost();
                break;
            }
        }

        bool needCreate = false;

        if (anyZipNeeded) {
            const QVector<ArtworkMetadata*> &artworkList = this->getArtworkList();
            foreach (ArtworkMetadata *metadata, artworkList) {
                const QString &filepath = metadata->getFilepath();
                QString archivePath = Helpers::getArchivePath(filepath);
                QFileInfo fi(archivePath);

                if (!fi.exists()) {
                    needCreate = true;
                    qDebug() << "Zip needed at least for" << archivePath;
                    break;
                }
            }
        }

        return needCreate;
    }

    void ArtworkUploader::doUploadArtworks(const QVector<ArtworkMetadata *> &artworkList) {
        int artworksCount = artworkList.length();
        if (artworksCount == 0) {
            return;
        }

        UploadInfoRepository *uploadInfoRepository = m_CommandManager->getUploadInfoRepository();
        QVector<Models::UploadInfo *> selectedInfos = uploadInfoRepository->retrieveSelectedUploadInfos();

        uploadInfoRepository->resetPercents();
        uploadInfoRepository->updatePercentages();

        m_FtpCoordinator->uploadArtworks(artworkList, selectedInfos, m_IncludeVector);
        m_CommandManager->reportUserAction(Conectivity::UserActionUpload);
    }

    void ArtworkUploader::cancelProcessing() {
        m_FtpCoordinator->cancelUpload();
    }
}
