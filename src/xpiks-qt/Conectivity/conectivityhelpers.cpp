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

#include "conectivityhelpers.h"
#include <memory>
#include <QVector>
#include "../Models/artworkmetadata.h"
#include "../Models/uploadinfo.h"
#include "../Encryption/secretsmanager.h"
#include "uploadcontext.h"
#include "uploadbatch.h"
#include "../Helpers/filenameshelpers.h"
#include "../Models/imageartwork.h"
#include "../Commands/commandmanager.h"
#include "../Models/settingsmodel.h"
#include "../Models/proxysettings.h"

#define RETRIES_COUNT 3

namespace Conectivity {
    void extractFilePathes(const QVector<Models::ArtworkMetadata *> &artworkList,
                           QStringList &filePathes,
                           QStringList &zipsPathes) {

        int size = artworkList.length();
        filePathes.reserve(size);
        zipsPathes.reserve(size);
        LOG_DEBUG << "Generating filepathes for" << size << "item(s)";

        for (int i = 0; i < size; ++i) {
            Models::ArtworkMetadata *metadata = artworkList.at(i);
            QString filepath = metadata->getFilepath();
            filePathes.append(filepath);

            Models::ImageArtwork *image = dynamic_cast<Models::ImageArtwork*>(metadata);

            if (image != NULL && image->hasVectorAttached()) {
                filePathes.append(image->getAttachedVectorPath());

                QString zipPath = Helpers::getArchivePath(filepath);
                zipsPathes.append(zipPath);
            } else {
                zipsPathes.append(filepath);
            }
        }
    }

    void generateUploadContexts(const std::vector<std::shared_ptr<Models::UploadInfo> > &uploadInfos,
                                std::vector<std::shared_ptr<UploadContext> > &contexts,
                                Encryption::SecretsManager *secretsManager,
                                Models::SettingsModel *settingsModel) {
        size_t size = uploadInfos.size();
        contexts.reserve(size);

        Models::ProxySettings *proxySettings = settingsModel->getProxySettings();
        int timeoutSeconds = settingsModel->getUploadTimeout();
        bool useProxy = settingsModel->getUseProxy();

        for (size_t i = 0; i < size; ++i) {
            std::shared_ptr<UploadContext> context(new UploadContext());
            auto &info = uploadInfos.at(i);

            context->m_Title = info->getTitle();
            context->m_Host = info->getHost();
            context->m_Username = info->getUsername();
            context->m_Password = secretsManager->decodePassword(info->getPassword());
            context->m_UsePassiveMode = !info->getDisableFtpPassiveMode();
            context->m_UseEPSV = !info->getDisableEPSV();
            context->m_UseProxy = useProxy;
            context->m_ProxySettings = proxySettings;
            context->m_TimeoutSeconds = timeoutSeconds;
            // TODO: move to configs/options
            context->m_RetriesCount = RETRIES_COUNT;

            if (context->m_Host.contains("dreamstime")) {
                context->m_DirForVectors = "additional";
            }

            if (context->m_Host.contains("alamy")) {
                context->m_DirForImages = "Stock";
                context->m_DirForVectors = "Vector";
            }

            contexts.emplace_back(context);
        }
    }

    std::vector<std::shared_ptr<UploadBatch> > generateUploadBatches(const QVector<Models::ArtworkMetadata *> &artworksToUpload,
                                                const std::vector<std::shared_ptr<Models::UploadInfo> > &uploadInfos,
                                                Encryption::SecretsManager *secretsManager,
                                                Models::SettingsModel *settingsModel) {
        LOG_DEBUG << artworksToUpload.length() << "file(s)";
        std::vector<std::shared_ptr<UploadBatch> > batches;

        QStringList filePathes;
        QStringList zipFilePathes;
        extractFilePathes(artworksToUpload, filePathes, zipFilePathes);

        std::vector<std::shared_ptr<UploadContext> > contexts;
        generateUploadContexts(uploadInfos, contexts, secretsManager, settingsModel);

        size_t size = contexts.size();
        batches.reserve(size);

        for (size_t i = 0; i < size; ++i) {
            auto &context = contexts.at(i);

            if (uploadInfos[i]->getZipBeforeUpload()) {
                batches.emplace_back(new UploadBatch(context, zipFilePathes));
            } else {
                batches.emplace_back(new UploadBatch(context, filePathes));
            }
        }

        return batches;
    }
}
