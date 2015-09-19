/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2015 Taras Kushnir <kushnirTV@gmail.com>
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
#include "../Helpers/curlwrapper.h"
#include "../Helpers/uploadcoordinator.h"
#include "../Helpers/ziphelper.h"
#include "../Models/artworkmetadata.h"
#include "../Helpers/testconnectionresult.h"
#include "../Helpers/uploadcoordinator.h"
#include "../Commands/commandmanager.h"

namespace Models {
    ArtworkUploader::ArtworkUploader(int maxParallelUploads) :
        ArtworksProcessor(),
        m_IncludeVector(false),
        m_Percent(0)
    {
        m_UploadCoordinator = new Helpers::UploadCoordinator(maxParallelUploads);

        QObject::connect(m_UploadCoordinator, SIGNAL(uploadStarted()), this, SLOT(onUploadStarted()));
        QObject::connect(m_UploadCoordinator, SIGNAL(uploadFinished(bool)), this, SLOT(allFinished(bool)));
        QObject::connect(m_UploadCoordinator, SIGNAL(itemFinished(bool)), this, SLOT(artworkUploaded(bool)));
        QObject::connect(m_UploadCoordinator, SIGNAL(percentChanged(double)), this, SLOT(uploaderPercentChanged(double)));

        m_TestingCredentialWatcher = new QFutureWatcher<Helpers::TestConnectionResult>(this);
        connect(m_TestingCredentialWatcher, SIGNAL(finished()), SLOT(credentialsTestingFinished()));
    }

    ArtworkUploader::~ArtworkUploader() {
        delete m_TestingCredentialWatcher;
        delete m_UploadCoordinator;
    }

    void ArtworkUploader::onUploadStarted()
    {
        beginProcessing();
        m_Percent = 0;
        updateProgress();
    }

    void ArtworkUploader::artworkUploaded(bool status)
    {
        artworkUploadedHandler(status);
    }

    void ArtworkUploader::allFinished(bool status)
    {
        Q_UNUSED(status);
        endProcessing();
        m_Percent = 100;
        updateProgress();
    }

    void ArtworkUploader::credentialsTestingFinished()
    {
        Helpers::TestConnectionResult result = m_TestingCredentialWatcher->result();
        emit credentialsChecked(result.getResult(), result.getUrl());
    }

    void ArtworkUploader::uploaderPercentChanged(double percent)
    {
        m_Percent = (int)(percent);
        emit percentChanged();
        UploadInfoRepository *uploadInfoRepository = m_CommandManager->getUploadInfoRepository();
        uploadInfoRepository->updatePercentages();
    }

    void ArtworkUploader::artworkUploadedHandler(bool success) {
        if (!success) {
            setIsError(true);
        }
    }

    void ArtworkUploader::uploadArtworks() { doUploadArtworks(getArtworkList()); }

    void ArtworkUploader::checkCredentials(const QString &host, const QString &username, const QString &password) const
    {
        m_TestingCredentialWatcher->setFuture(QtConcurrent::run(isConnectionValid, host, username, password));
    }

    bool ArtworkUploader::needCreateArchives() const {
        bool anyZipNeeded = false;
        const UploadInfoRepository *uploadInfoRepository = m_CommandManager->getUploadInfoRepository();
        const QList<Models::UploadInfo *> &infos = uploadInfoRepository->getUploadInfos();
        foreach (Models::UploadInfo *info, infos) {
            if (info->getIsSelected() && info->getZipBeforeUpload()) {
                anyZipNeeded = true;
                break;
            }
        }

        bool needCreate = false;

        if (anyZipNeeded) {
            const QList<ArtworkMetadata*> &artworkList = this->getArtworkList();
            foreach (ArtworkMetadata *metadata, artworkList) {
                const QString &filepath = metadata->getFilepath();
                QString archivePath = Helpers::getArchivePath(filepath);
                QFileInfo fi(archivePath);

                if (!fi.exists()) {
                    needCreate = true;
                    break;
                }
            }
        }

        return needCreate;
    }

    void ArtworkUploader::doUploadArtworks(const QList<ArtworkMetadata *> &artworkList) {
        int artworksCount = artworkList.length();
        if (artworksCount == 0) {
            return;
        }

        UploadInfoRepository *uploadInfoRepository = m_CommandManager->getUploadInfoRepository();
        const QList<Models::UploadInfo *> &infos = uploadInfoRepository->getUploadInfos();
        const Encryption::SecretsManager *secretsManager = m_CommandManager->getSecretsManager();

        uploadInfoRepository->resetPercents();
        m_UploadCoordinator->uploadArtworks(artworkList, infos, m_IncludeVector, secretsManager);
    }

    void ArtworkUploader::cancelProcessing() {
        m_UploadCoordinator->cancelUpload();
    }
}
