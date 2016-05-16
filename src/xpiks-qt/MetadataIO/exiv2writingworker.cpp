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

#include "exiv2writingworker.h"
#include <QStringList>
#include <QTextCodec>
#include "../Models/artworkmetadata.h"
#include "../Common/defines.h"
#include "../Helpers/stringhelper.h"
#include "exiv2tagnames.h"
#include <exiv2/exiv2.hpp>
#include <string>

#define X_DEFAULT QString::fromLatin1("x-default")
#define IPTC_MAX_DESCRIPTION_LEN 2000
#define IPTC_MAX_TITLE_LEN 64

namespace MetadataIO {
    typedef QMap<QString, QString> AltLangMap;

    void removeXmpTag(Exiv2::XmpData &xmpData, const char* propertyName) {
        Exiv2::XmpKey xmpKey(propertyName);
        Exiv2::XmpData::iterator it = xmpData.findKey(xmpKey);

        if (it != xmpData.end()) {
            xmpData.erase(it);
        }
    }

    void setXmpTagStringBag(Exiv2::XmpData &xmpData, const char* propertyName, const QStringList& bag) {
        if (bag.isEmpty()) {
            removeXmpTag(xmpData, propertyName);
        } else {
            Exiv2::Value::AutoPtr xmpTxtBag = Exiv2::Value::create(Exiv2::xmpBag);
            QStringList::const_iterator end = bag.constEnd();
            for (QStringList::const_iterator it = bag.constBegin(); it != end; ++it) {
                std::string txt((*it).toUtf8().constData());
                xmpTxtBag->read(txt);
            }

            xmpData[propertyName].setValue(xmpTxtBag.get());
        }
    }

    bool getXmpTagStringListLangAlt(Exiv2::XmpData &xmpData, const char* propertyName, AltLangMap &map) {
        bool success = false;
        Exiv2::XmpKey xmpKey(propertyName);
        Exiv2::XmpData::iterator it = xmpData.findKey(xmpKey);

        if (it != xmpData.end()) {
            const Exiv2::LangAltValue &value = static_cast<const Exiv2::LangAltValue &>(it->value());

            Exiv2::LangAltValue::ValueType::const_iterator end = value.value_.end();
            Exiv2::LangAltValue::ValueType::const_iterator it2 = value.value_.begin();
            for (; it2 != end; ++it2) {
                QString lang = QString::fromUtf8(it2->first.c_str());
                QString text = QString::fromUtf8(it2->second.c_str());
                map.insert(lang, text);
                success = true;
            }
        }

        return success;
    }

    void setXmpTagStringLangAlt(Exiv2::XmpData &xmpData, const char* propertyName,
                                const QString& langAlt, const QString& text) {
        Exiv2::Value::AutoPtr xmpTxtVal = Exiv2::Value::create(Exiv2::langAlt);

        AltLangMap map;

        if (getXmpTagStringListLangAlt(xmpData, propertyName, map)) {
            AltLangMap::iterator end = map.end();
            for (AltLangMap::iterator it = map.begin(); it != end; ++it) {
                if (it.key() != langAlt) {
                    std::string val((*it).toUtf8().constData());
                    xmpTxtVal->read(val);
                }
            }
        }

        QString txtLangAlt = QString("lang=%1 %2").arg(langAlt).arg(text);
        std::string txt(txtLangAlt.toUtf8().constData());
        xmpTxtVal->read(txt);

        removeXmpTag(xmpData, propertyName);
        xmpData.add(Exiv2::XmpKey(propertyName), xmpTxtVal.get());
    }

    void setXmpKeywords(Exiv2::XmpData &xmpData, const QStringList &keywords) {
        try {
            setXmpTagStringBag(xmpData, XMP_KEYWORDS, keywords);
        }
        catch (Exiv2::Error &e) {
            LOG_WARNING << "Exiv2 error:" << e.what();
        }
        catch (...) {
            LOG_WARNING << "Exception";
#ifdef QT_DEBUG
            throw;
#endif
        }
    }

    void setXmpDescription(Exiv2::XmpData &xmpData, const QString &description) {
        try {
            setXmpTagStringLangAlt(xmpData, XMP_DESCRIPTION, X_DEFAULT, description);
        }
        catch (Exiv2::Error &e) {
            LOG_WARNING << "Exiv2 error:" << e.what();
        }
        catch (...) {
            LOG_WARNING << "Exception";
#ifdef QT_DEBUG
            throw;
#endif
        }
    }

    void setXmpTitle(Exiv2::XmpData &xmpData, const QString &title) {
        try {
            setXmpTagStringLangAlt(xmpData, XMP_TITLE, X_DEFAULT, title);
        }
        catch (Exiv2::Error &e) {
            LOG_WARNING << "Exiv2 error:" << e.what();
        }
        catch (...) {
            LOG_WARNING << "Exception";
#ifdef QT_DEBUG
            throw;
#endif
        }
    }

    void removeIptcTag(Exiv2::IptcData &iptcData, const char *propertyName) {
        Exiv2::IptcKey key(propertyName);

        Exiv2::IptcData::iterator it = iptcData.findKey(key);
        if (it != iptcData.end()) {
            iptcData.erase(it);
        }
    }

    void setIptcEncodingUtf8(Exiv2::IptcData &iptcData) {
        iptcData[IPTC_CHARSET] = "\033%G";
    }

    void setIptcString(Exiv2::IptcData &iptcData, const char *propertyName, const QString &value) {
        iptcData[propertyName] = std::string(value.toUtf8().constData());
        setIptcEncodingUtf8(iptcData);
    }

    void setIptcDescription(Exiv2::IptcData &iptcData, const QString &description) {
        try {
            setIptcString(iptcData, IPTC_DESCRIPTION, description.mid(0, IPTC_MAX_DESCRIPTION_LEN));
        }
        catch (Exiv2::Error &e) {
            LOG_WARNING << "Exiv2 error:" << e.what();
        }
        catch (...) {
            LOG_WARNING << "Exception";
#ifdef QT_DEBUG
            throw;
#endif
        }
    }

    void setIptcTitle(Exiv2::IptcData &iptcData, const QString &title) {
        try {
            setIptcString(iptcData, IPTC_TITLE, title.mid(0, IPTC_MAX_TITLE_LEN));
        }
        catch (Exiv2::Error &e) {
            LOG_WARNING << "Exiv2 error:" << e.what();
        }
        catch (...) {
            LOG_WARNING << "Exception";
#ifdef QT_DEBUG
            throw;
#endif
        }
    }

    void removeExistingIptcKeywords(Exiv2::IptcData &iptcData) {
        Exiv2::IptcData::iterator it = iptcData.begin();        
        QString keywordsKey = QString::fromLatin1(IPTC_KEYWORDS);

        while (it != iptcData.end()) {
            QString key = QString::fromLocal8Bit(it->key().c_str());

            if (key == keywordsKey) {
                it = iptcData.erase(it);
            } else {
                ++it;
            }
        }
    }

    void addIptcKeywords(Exiv2::IptcData &iptcData, const QStringList &keywords) {
        Exiv2::IptcKey keywordsKey(IPTC_KEYWORDS);
        int size = keywords.size();

        for (int i = 0; i < size; ++i) {
            QString key = keywords.at(i);
            key.truncate(64);

            Exiv2::Value::AutoPtr iptcKeyword = Exiv2::Value::create(Exiv2::string);
            iptcKeyword->read(key.toUtf8().constData());
            iptcData.add(keywordsKey, iptcKeyword.get());
        }

        setIptcEncodingUtf8(iptcData);
    }

    void setIptcKeywords(Exiv2::IptcData &iptcData, const QStringList &keywords) {
        try {
            removeExistingIptcKeywords(iptcData);
            addIptcKeywords(iptcData, keywords);
        }
        catch (Exiv2::Error &e) {
            LOG_WARNING << "Exiv2 error:" << e.what();
        }
        catch (...) {
            LOG_WARNING << "Exception";
#ifdef QT_DEBUG
            throw;
#endif
        }
    }

    void setExifLatin1String(Exiv2::ExifData &exifData, const char *propertyName, const QString &value) {
        exifData[propertyName] = std::string(value.toLatin1().constData());
    }

    void setExifUtf8String(Exiv2::ExifData &exifData, const char *propertyName, const QString &value) {
        exifData[propertyName] = std::string(value.toUtf8().constData());
    }

    void setExifCommentValue(Exiv2::ExifData &exifData, const char *propertyName, const QString &comment) {
        // Write as Unicode only when necessary.
        QTextCodec* latin1Codec = QTextCodec::codecForName("iso8859-1");

        bool dataWritten = false;

        if (latin1Codec->canEncode(comment)) {
            // We know it's in the ISO-8859-1 8bit range.
            // Check if it's in the ASCII 7bit range
            if (Helpers::is7BitAscii(comment.toLatin1())) {
                // write as ASCII
                std::string exifComment("charset=\"Ascii\" ");
                exifComment += comment.toLatin1().constData();
                exifData[propertyName] = exifComment;
                dataWritten = true;
            }
        }

        if (!dataWritten) {
            // write as Unicode (UCS-2)
            std::string exifComment("charset=\"Unicode\" ");
            exifComment += comment.toUtf8().constData();

            exifData[propertyName] = exifComment;
            dataWritten = true;
        }
    }

    void setExifDescription(Exiv2::ExifData &exifData, const QString &description) {
        try {
            setExifUtf8String(exifData, EXIF_DESCRIPTION, description);
            setExifCommentValue(exifData, EXIF_USERCOMMENT, description);
        }
        catch (Exiv2::Error &e) {
            LOG_WARNING << "Exiv2 error:" << e.what();
        }
        catch (...) {
            LOG_WARNING << "Exception";
#ifdef QT_DEBUG
            throw;
#endif
        }
    }

    void setArtworkDescription(Exiv2::XmpData &xmpData, Exiv2::ExifData &exifData,
                             Exiv2::IptcData &iptcData, const QString &description) {
        setXmpDescription(xmpData, description);
        setIptcDescription(iptcData, description);
        setExifDescription(exifData, description);
    }

    void setArtworkTitle(Exiv2::XmpData &xmpData, Exiv2::ExifData &exifData,
                         Exiv2::IptcData &iptcData, const QString &title) {
        setXmpTitle(xmpData, title);
        setIptcTitle(iptcData, title);
        Q_UNUSED(exifData);
    }

    void setArtworkKeywords(Exiv2::XmpData &xmpData, Exiv2::ExifData &exifData,
                            Exiv2::IptcData &iptcData, const QStringList &keywords) {
        setXmpKeywords(xmpData, keywords);
        setIptcKeywords(iptcData, keywords);
        Q_UNUSED(exifData);
    }

    Exiv2WritingWorker::Exiv2WritingWorker(int index, QVector<Models::ArtworkMetadata *> itemsToWrite, QObject *parent) :
        QObject(parent),
        m_ItemsToWrite(itemsToWrite),
        m_WorkerIndex(index),
        m_Stopped(false)
    {
        Q_ASSERT(!itemsToWrite.isEmpty());
        LOG_INFO << "Worker [" << index << "]:" << itemsToWrite.size() << "items to write";
    }

    void Exiv2WritingWorker::process() {
        bool anyError = false;
        int size = m_ItemsToWrite.size();

        for (int i = 0; i < size; ++i) {
            if (m_Stopped) { break; }

            Models::ArtworkMetadata *artwork = m_ItemsToWrite.at(i);

            try {
                writeMetadata(artwork);
            }
            catch(Exiv2::Error &error) {
                anyError = true;
                LOG_WARNING << "Worker" << m_WorkerIndex << "Exiv2 error:" << error.what();
            }
            catch(...) {
                anyError = true;
                LOG_WARNING << "Worker" << m_WorkerIndex << "Writing error for item" << artwork->getFilepath();
            }
        }

        LOG_INFO << "Worker #" << m_WorkerIndex << "finished";

        emit finished(anyError);
    }

    void Exiv2WritingWorker::cancel() {
        m_Stopped = true;
    }

    void Exiv2WritingWorker::writeMetadata(Models::ArtworkMetadata *artwork) {
        const QString &filepath = artwork->getFilepath();
#if defined(Q_OS_WIN)
        Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(filepath.toStdWString());
#else
        Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(filepath.toStdString());
#endif
        Q_ASSERT(image.get() != NULL);

        image->readMetadata();

        Exiv2::XmpData &xmpData = image->xmpData();
        Exiv2::ExifData &exifData = image->exifData();
        Exiv2::IptcData &iptcData = image->iptcData();

        QString description = artwork->getDescription();
        setArtworkDescription(xmpData, exifData, iptcData, description);

        QString title = artwork->getTitle();
        setArtworkTitle(xmpData, exifData, iptcData, title);

        QStringList keywords = artwork->getKeywords();
        setArtworkKeywords(xmpData, exifData, iptcData, keywords);

        image->writeMetadata();
    }
}
