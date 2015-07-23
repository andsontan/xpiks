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

#include "commandmanager.h"
#include "../Models/artworksrepository.h"
#include "../Models/artitemsmodel.h"
#include "../Models/combinedartworksmodel.h"
#include "../Models/iptcprovider.h"
#include "../Models/artworkuploader.h"
#include "../Models/uploadinforepository.h"
#include "../Models/warningsmanager.h"
#include "../Encryption/secretsmanager.h"
#include "../UndoRedo/undoredomanager.h"
#include "../Models/ziparchiver.h"
#include "../Suggestion/keywordssuggestor.h"
#include "../Commands/addartworkscommand.h"

void Commands::CommandManager::InjectDependency(Models::ArtworksRepository *artworkRepository) {
    Q_ASSERT(artworkRepository != NULL); m_ArtworksRepository = artworkRepository;
    m_ArtworksRepository->setCommandManager(this);
}

void Commands::CommandManager::InjectDependency(Models::ArtItemsModel *artItemsModel) {
    Q_ASSERT(artItemsModel != NULL); m_ArtItemsModel = artItemsModel;
    m_ArtItemsModel->setCommandManager(this);
}

void Commands::CommandManager::InjectDependency(Models::CombinedArtworksModel *combinedArtworksModel) {
    Q_ASSERT(combinedArtworksModel != NULL); m_CombinedArtworksModel = combinedArtworksModel;
    m_CombinedArtworksModel->setCommandManager(this);
}

void Commands::CommandManager::InjectDependency(Models::IptcProvider *iptcProvider) {
    Q_ASSERT(iptcProvider != NULL); m_IptcProvider = iptcProvider;
    m_IptcProvider->setCommandManager(this);
}

void Commands::CommandManager::InjectDependency(Models::ArtworkUploader *artworkUploader) {
    Q_ASSERT(artworkUploader != NULL); m_ArtworkUploader = artworkUploader;
    m_ArtworkUploader->setCommandManager(this);
}

void Commands::CommandManager::InjectDependency(Models::UploadInfoRepository *uploadInfoRepository) {
    Q_ASSERT(uploadInfoRepository != NULL); m_UploadInfoRepository = uploadInfoRepository;
    m_UploadInfoRepository->setCommandManager(this);
}

void Commands::CommandManager::InjectDependency(Models::WarningsManager *warningsManager) {
    Q_ASSERT(warningsManager != NULL); m_WarningsManager = warningsManager;
    m_WarningsManager->setCommandManager(this);
}

void Commands::CommandManager::InjectDependency(Encryption::SecretsManager *secretsManager) {
    Q_ASSERT(secretsManager != NULL); m_SecretsManager = secretsManager;
    m_SecretsManager->setCommandManager(this);
}

void Commands::CommandManager::InjectDependency(UndoRedo::UndoRedoManager *undoRedoManager) {
    Q_ASSERT(undoRedoManager != NULL); m_UndoRedoManager = undoRedoManager;
    m_UndoRedoManager->setCommandManager(this);
}

void Commands::CommandManager::InjectDependency(Models::ZipArchiver *zipArchiver)
{
    Q_ASSERT(zipArchiver != NULL); m_ZipArchiver = zipArchiver;
    m_ZipArchiver->setCommandManager(this);
}

void Commands::CommandManager::InjectDependency(Suggestion::KeywordsSuggestor *keywordsSuggestor)
{
    Q_ASSERT(keywordsSuggestor != NULL); m_KeywordsSuggestor = keywordsSuggestor;
    m_KeywordsSuggestor->setCommandManager(this);
}

Commands::CommandResult *Commands::CommandManager::processCommand(Commands::CommandBase *command) const
{
    Commands::CommandResult *result = command->execute(this);
    delete command;
    return result;
}

void Commands::CommandManager::recordHistoryItem(UndoRedo::HistoryItem *historyItem) const
{
    if (m_UndoRedoManager) {
        m_UndoRedoManager->recordHistoryItem(historyItem);
    }
}

void Commands::CommandManager::connectEntitiesSignalsSlots() const
{
    QObject::connect(m_SecretsManager, SIGNAL(beforeMasterPasswordChange(QString,QString)),
                     m_UploadInfoRepository, SLOT(onBeforeMasterPasswordChanged(QString,QString)));

    QObject::connect(m_ArtItemsModel, SIGNAL(needCheckItemsForWarnings(QList<ArtworkMetadata*>)),
                     m_WarningsManager, SLOT(onCheckWarnings(QList<ArtworkMetadata*>)));
}

void Commands::CommandManager::recodePasswords(const QString &oldMasterPassword,
                                                  const QString &newMasterPassword,
                                                  const QList<Models::UploadInfo*> &uploadInfos) const
{
    foreach (Models::UploadInfo *info, uploadInfos) {
        if (info->hasPassword()) {
            QString newPassword = m_SecretsManager->recodePassword(
                        info->getPassword(), oldMasterPassword, newMasterPassword);
            info->setPassword(newPassword);
        }
    }
}

void Commands::CommandManager::combineArtworks(const QList<Models::ArtItemInfo *> &artworks) const
{
    if (m_CombinedArtworksModel) {
        m_CombinedArtworksModel->initArtworks(artworks);
        m_CombinedArtworksModel->recombineArtworks();
    }
}

void Commands::CommandManager::setArtworksForIPTCProcessing(const QList<Models::ArtworkMetadata*> &artworks) const
{
    if (m_IptcProvider) {
        m_IptcProvider->setArtworks(artworks);
    }
}

void Commands::CommandManager::setArtworksForUpload(const QList<Models::ArtworkMetadata *> &artworks) const
{
    if (m_ArtworkUploader) {
        m_ArtworkUploader->setArtworks(artworks);
    }
}

void Commands::CommandManager::setArtworksForZipping(const QList<Models::ArtworkMetadata *> &artworks) const {
    if (m_ZipArchiver) {
        m_ZipArchiver->setArtworks(artworks);
    }
}

/*virtual*/
void Commands::CommandManager::connectArtworkSignals(Models::ArtworkMetadata *metadata) const
{
    if (m_ArtItemsModel) {
        QObject::connect(metadata, SIGNAL(modifiedChanged(bool)),
                         m_ArtItemsModel, SLOT(itemModifiedChanged(bool)));
        QObject::connect(metadata, SIGNAL(selectedChanged(bool)),
                         m_ArtItemsModel, SLOT(itemSelectedChanged(bool)));
    }

    if (m_ArtworksRepository) {
        QObject::connect(metadata, SIGNAL(fileSelectedChanged(QString,bool)),
                         m_ArtworksRepository, SLOT(fileSelectedChanged(QString,bool)));
    }
}

void Commands::CommandManager::updateArtworks(const QList<int> &indices) const
{
    if (m_ArtItemsModel) {
        m_ArtItemsModel->updateItemsAtIndices(indices);
    }
}

#ifdef QT_DEBUG
void Commands::CommandManager::addInitialArtworks(const QStringList &artworksFilepathes)
{
    Commands::AddArtworksCommand *command = new Commands::AddArtworksCommand(artworksFilepathes);
    CommandResult *result = this->processCommand(command);
    delete result;
}
#endif
