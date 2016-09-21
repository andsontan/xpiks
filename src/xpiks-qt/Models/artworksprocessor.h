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

#ifndef ARTWORKSPROCESSOR_H
#define ARTWORKSPROCESSOR_H

#include <QObject>
#include <QVector>
#include "../Common/baseentity.h"
#include "../Helpers/ifilenotavailablemodel.h"
#include "../Models/artworkmetadata.h"

namespace Commands {
    class CommandManager;
}

namespace Models {
    class ArtworksProcessor : public QObject, public Common::BaseEntity, public Helpers::IFileNotAvailableModel
    {
        Q_OBJECT
        Q_PROPERTY(bool inProgress READ getInProgress WRITE setInProgress NOTIFY inProgressChanged)
        Q_PROPERTY(bool isError READ getIsError WRITE setIsError NOTIFY isErrorChanged)
        Q_PROPERTY(int percent READ getPercent NOTIFY percentChanged)
        Q_PROPERTY(int itemsCount READ getItemsCount NOTIFY itemsCountChanged)

    protected:
        enum { MAX_WORKER_THREADS = 10};

    public:
        ArtworksProcessor(QObject *parent=0) :
            QObject(parent),
            Common::BaseEntity(),
            m_ProcessedArtworksCount(0),
            m_ArtworksCount(0),
            m_ExistingMaxThreadsNumber(0),
            m_IsInProgress(false),
            m_IsError(false)
        {}

        virtual ~ArtworksProcessor() {}

    protected:
        void incProgress() { m_ProcessedArtworksCount++; updateProgress(); }

    public:
        bool getInProgress() const { return m_IsInProgress; }
        void setInProgress(bool value) { m_IsInProgress = value; emit inProgressChanged(); }
        virtual int getPercent() const { return m_ArtworksCount == 0 ? 0 : (m_ProcessedArtworksCount * 100 / m_ArtworksCount); }
        void updateProgress() { emit percentChanged(); }
        bool getIsError() const { return m_IsError; }
        void setIsError(bool value) { m_IsError = value; emit isErrorChanged(); }
        virtual int getItemsCount() const { return m_ArtworkList.length(); }

    public:
        Q_INVOKABLE void resetModel();
        Q_INVOKABLE void cancelOperation() { cancelProcessing(); }

    signals:
        void inProgressChanged();
        void isErrorChanged();
        void startedProcessing();
        void percentChanged();
        void finishedProcessing();
        void itemsCountChanged();
        void itemsNumberChanged();
        void requestCloseWindow();

    public:
        void setArtworks(const QVector<ArtworkMetadata*> &artworkList) { resetArtworks(); addArtworks(artworkList); }
        void addArtworks(const QVector<Models::ArtworkMetadata*> &artworkList) { m_ArtworkList << artworkList; emit itemsCountChanged(); }
        void resetArtworks() { m_ArtworkList.clear(); }
        virtual bool removeUnavailableItems();

#ifdef CORE_TESTS
    public:
#else
    protected:
#endif
        const QVector<ArtworkMetadata*> &getArtworkList() const { return m_ArtworkList; }

    protected:
        virtual void cancelProcessing() = 0;
        virtual void innerResetModel() { /*BUMP*/ }
        void beginProcessing();
        void endProcessing();
        void endAfterFirstError();
        virtual void restrictMaxThreads();

    private:
        QVector<ArtworkMetadata*> m_ArtworkList;
        volatile int m_ProcessedArtworksCount;
        volatile int m_ArtworksCount;
        volatile int m_ExistingMaxThreadsNumber;
        volatile bool m_IsInProgress;
        volatile bool m_IsError;
    };
}
#endif // ARTWORKSPROCESSOR_H
