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
#include "uploadinforepository.h"
#include "uploadinfo.h"
#include "../Common/defines.h"
#include "../Helpers/ziphelper.h"
#include "../Models/artworkmetadata.h"
#include "../Commands/commandmanager.h"
#include "../Models/settingsmodel.h"
#include "../Helpers/filenameshelpers.h"
#include "../Conectivity/iftpcoordinator.h"
#include "../Conectivity/testconnection.h"
#include "../Conectivity/uploadcontext.h"

#ifndef TESTS
#include "../Conectivity/ftpcoordinator.h"
#endif

namespace Models {
    ArtworkUploader::ArtworkUploader(Conectivity::IFtpCoordinator *ftpCoordinator, QObject *parent) :
        ArtworksProcessor(parent),
        m_FtpCoordinator(ftpCoordinator),
        m_Percent(0)
    {
#ifndef TESTS
        Conectivity::FtpCoordinator *coordinator = dynamic_cast<Conectivity::FtpCoordinator*>(ftpCoordinator);
        QObject::connect(coordinator, SIGNAL(uploadStarted()), this, SLOT(onUploadStarted()));
        QObject::connect(coordinator, SIGNAL(uploadFinished(bool)), this, SLOT(allFinished(bool)));
        QObject::connect(coordinator, SIGNAL(overallProgressChanged(double)), this, SLOT(uploaderPercentChanged(double)));

        m_TestingCredentialWatcher = new QFutureWatcher<Conectivity::ContextValidationResult>(this);
        QObject::connect(m_TestingCredentialWatcher, SIGNAL(finished()), SLOT(credentialsTestingFinished()));
#endif
    }

    ArtworkUploader::~ArtworkUploader() {
#ifndef TESTS
        delete m_TestingCredentialWatcher;
#endif

        if (m_FtpCoordinator != NULL) {
            delete m_FtpCoordinator;
        }
    }

    void ArtworkUploader::setCommandManager(Commands::CommandManager *commandManager) {
        Common::BaseEntity::setCommandManager(commandManager);
#ifndef TESTS
        Conectivity::FtpCoordinator *coordinator = dynamic_cast<Conectivity::FtpCoordinator*>(m_FtpCoordinator);
        coordinator->setCommandManager(commandManager);
#endif
    }

    void ArtworkUploader::onUploadStarted() {
        LOG_DEBUG << "#";
        beginProcessing();
        m_Percent = 0;
        updateProgress();
    }

    void ArtworkUploader::allFinished(bool anyError) {
        LOG_INFO << "anyError =" << anyError;
        setIsError(anyError);
        endProcessing();
        m_Percent = 100;
        updateProgress();
    }

    void ArtworkUploader::credentialsTestingFinished() {
#ifndef TESTS
        Conectivity::ContextValidationResult result = m_TestingCredentialWatcher->result();
        emit credentialsChecked(result.m_Result, result.m_Host);
#endif
    }

    void ArtworkUploader::uploaderPercentChanged(double percent) {
        m_Percent = (int)(percent);
        LOG_DEBUG << "Overall progress =" << percent;
        updateProgress();
#ifndef TESTS
        UploadInfoRepository *uploadInfoRepository = m_CommandManager->getUploadInfoRepository();
        uploadInfoRepository->updatePercentages();
#endif
    }

#ifndef TESTS
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
#endif

    bool ArtworkUploader::needCreateArchives() const {
        bool anyZipNeeded = false;
        const UploadInfoRepository *uploadInfoRepository = m_CommandManager->getUploadInfoRepository();
        const QVector<Models::UploadInfo *> &infos = uploadInfoRepository->getUploadInfos();
        foreach (Models::UploadInfo *info, infos) {
            if (info->getIsSelected() && info->getZipBeforeUpload()) {
                anyZipNeeded = true;
                LOG_DEBUG << "at least for" << info->getHost();
                break;
            }
        }

        bool needCreate = false;

        if (anyZipNeeded) {
            const QVector<ArtworkMetadata*> &artworkList = this->getArtworkList();
            foreach (ArtworkMetadata *metadata, artworkList) {
                if (!metadata->hasVectorAttached()) { continue; }

                const QString &filepath = metadata->getFilepath();
                QString archivePath = Helpers::getArchivePath(filepath);
                QFileInfo fi(archivePath);

                if (!fi.exists()) {
                    needCreate = true;
                    LOG_DEBUG << "Zip needed at least for" << archivePath;
                    break;
                }
            }
        }

        return needCreate;
    }

#ifndef TESTS
    void ArtworkUploader::doUploadArtworks(const QVector<ArtworkMetadata *> &artworkList) {
        int artworksCount = artworkList.length();
        if (artworksCount == 0) {
            return;
        }

        UploadInfoRepository *uploadInfoRepository = m_CommandManager->getUploadInfoRepository();
        QVector<Models::UploadInfo *> selectedInfos = uploadInfoRepository->retrieveSelectedUploadInfos();

        uploadInfoRepository->resetPercents();
        uploadInfoRepository->updatePercentages();

        m_FtpCoordinator->uploadArtworks(artworkList, selectedInfos);
        m_CommandManager->reportUserAction(Conectivity::UserActionUpload);
    }
#endif

    void ArtworkUploader::cancelProcessing() {
        m_FtpCoordinator->cancelUpload();
    }


    void ArtworkUploader::removeUnavailableItems(){
        const QVector<ArtworkMetadata*> & ArtworksListOld=getArtworkList();
        QVector<ArtworkMetadata*> ArtworksListNew;
        int size = ArtworksListOld.size();
        for (int i = 0; i < size; ++i){
             ArtworkMetadata* ArtItemInfoElement=ArtworksListOld[i];
            if (!ArtItemInfoElement->getIsUnavailable()){
                ArtworksListNew.append(ArtItemInfoElement);
            }
        }
        setArtworks(ArtworksListNew);
        if (ArtworksListNew.size()==0){
            emit closeWindow();
        }
            emit numberItemsChanged();
    }
}
