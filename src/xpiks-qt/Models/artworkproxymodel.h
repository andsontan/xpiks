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

#ifndef ARTWORKPROXYMODEL_H
#define ARTWORKPROXYMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QQmlEngine>
#include <QStringList>
#include <QString>
#include <QList>
#include <QSet>
#include <QSize>
#include <QQuickTextDocument>
#include <memory>
#include <vector>
#include "artworkmetadata.h"
#include "../Common/basicmetadatamodel.h"
#include "../Common/flags.h"
#include "../SpellCheck/spellcheckiteminfo.h"
#include "../Common/hold.h"
#include "../Models/metadataelement.h"
#include "artworkproxybase.h"

namespace Models {
    class ArtworkProxyModel: public QObject, public ArtworkProxyBase
    {
        Q_OBJECT
        Q_PROPERTY(QString description READ getDescription WRITE setDescription NOTIFY descriptionChanged)
        Q_PROPERTY(QString title READ getTitle WRITE setTitle NOTIFY titleChanged)
        Q_PROPERTY(int keywordsCount READ getKeywordsCount NOTIFY keywordsCountChanged)
        Q_PROPERTY(QString imagePath READ getImagePath NOTIFY imagePathChanged)
        Q_PROPERTY(QString basename READ getBasename NOTIFY imagePathChanged)
        Q_PROPERTY(QString attachedVectorPath READ getAttachedVectorPath NOTIFY imagePathChanged)
        Q_PROPERTY(QSize imageSize READ retrieveImageSize NOTIFY imagePathChanged)
        Q_PROPERTY(QString fileSize READ retrieveFileSize NOTIFY imagePathChanged)
        Q_PROPERTY(QString dateTaken READ getDateTaken NOTIFY imagePathChanged)

    public:
        explicit ArtworkProxyModel(QObject *parent = 0);
        virtual ~ArtworkProxyModel();

    public:
        const QString &getImagePath() const { return m_ArtworkMetadata->getFilepath(); }
        QString getBasename() const { return m_ArtworkMetadata->getBaseFilename(); }

    public:
        virtual void setDescription(const QString &description);
        virtual void setTitle(const QString &title);

    signals:
        void imagePathChanged();
        void descriptionChanged();
        void titleChanged();
        void keywordsCountChanged();
        void completionsAvailable();
        void itemBecomeUnavailable();

    protected:
        virtual void signalDescriptionChanged() { emit descriptionChanged(); }
        virtual void signalTitleChanged() { emit titleChanged(); }
        virtual void signalKeywordsCountChanged() { emit keywordsCountChanged(); }

    public slots:
        void afterSpellingErrorsFixedHandler();
        void spellCheckErrorsChangedHandler();
        void itemUnavailableHandler(int index);

    public:
        Q_INVOKABLE void editKeyword(int index, const QString &replacement);
        Q_INVOKABLE void removeKeywordAt(int keywordIndex);
        Q_INVOKABLE void removeLastKeyword();
        Q_INVOKABLE void appendKeyword(const QString &keyword);
        Q_INVOKABLE void pasteKeywords(const QStringList &keywords);
        Q_INVOKABLE void clearKeywords();
        Q_INVOKABLE QString getKeywordsString();
        Q_INVOKABLE void suggestCorrections();
        Q_INVOKABLE void initDescriptionHighlighting(QQuickTextDocument *document);
        Q_INVOKABLE void initTitleHighlighting(QQuickTextDocument *document);
        Q_INVOKABLE void spellCheckDescription();
        Q_INVOKABLE void spellCheckTitle();
        Q_INVOKABLE void plainTextEdit(const QString &rawKeywords);
        Q_INVOKABLE bool hasTitleWordSpellError(const QString &word);
        Q_INVOKABLE bool hasDescriptionWordSpellError(const QString &word);
        Q_INVOKABLE void setSourceArtwork(QObject *artworkMetadata, int originalIndex=-1);
        Q_INVOKABLE void resetModel();
        Q_INVOKABLE QObject *getBasicModel() {
            QObject *item = getBasicMetadataModel();
            QQmlEngine::setObjectOwnership(item, QQmlEngine::CppOwnership);

            return item;
        }
        Q_INVOKABLE QSize retrieveImageSize() const;
        Q_INVOKABLE QString retrieveFileSize() const;
        Q_INVOKABLE QString getDateTaken() const;
        Q_INVOKABLE QString getAttachedVectorPath() const;

    protected:
        virtual Common::BasicMetadataModel *getBasicMetadataModel() {
            Q_ASSERT(m_ArtworkMetadata != nullptr);
            return m_ArtworkMetadata->getBasicModel();
        }

        virtual Common::IMetadataOperator *getMetadataOperator() {
            Q_ASSERT(m_ArtworkMetadata != nullptr);
            return m_ArtworkMetadata;
        }

    private:
        void updateCurrentArtwork();
        void doResetModel();
        void disconnectCurrentArtwork();

    private:
        Models::ArtworkMetadata *m_ArtworkMetadata;
        int m_ArtworkOriginalIndex;
    };
}

#endif // ARTWORKPROXYMODEL_H
