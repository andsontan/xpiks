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

#include "exiv2readingworker.h"
#include <QVector>
#include <QTextCodec>
#include <QDateTime>
#include <sstream>
#include "../xpiks-qt/Models/artworkmetadata.h"
#include "../xpiks-qt/Common/defines.h"
#include "../xpiks-qt/Helpers/stringhelper.h"
#include "saverworkerjobitem.h"
#include <exiv2/exiv2.hpp>

namespace MetadataIO {
    QString getIptcCharset(Exiv2::IptcData &iptcData) {
        const char *charsetPtr = iptcData.detectCharset();
        QString iptcCharset = "";
        if (charsetPtr != NULL) {
            iptcCharset = QString::fromLatin1(charsetPtr).toUpper();
        }

        return iptcCharset;
    }

    bool getXmpLangAltValue(Exiv2::XmpData &xmpData, const char *propertyName,
                            const QString &langAlt, QString &resultValue) {
        bool anyFound = false;

        Exiv2::XmpKey key(propertyName);
        Exiv2::XmpData::iterator it = xmpData.findKey(key);
        if ((it != xmpData.end()) && (it->typeId() == Exiv2::langAlt)) {
            const Exiv2::LangAltValue &value = static_cast<const Exiv2::LangAltValue &>(it->value());

            QString anyValue;

            Exiv2::LangAltValue::ValueType::const_iterator it2 = value.value_.begin();
            Exiv2::LangAltValue::ValueType::const_iterator end = value.value_.end();
            for (; it2 != end; ++it2) {
                QString lang = QString::fromUtf8(it2->first.c_str());

                if (langAlt == lang) {
                    QString text = QString::fromUtf8(it2->second.c_str()).trimmed();
                    if (!text.isEmpty()) {
                        anyFound = true;
                        resultValue = text.trimmed();
                        break;
                    }
                }

                if (anyValue.isEmpty()) {
                    QString text = QString::fromUtf8(it2->second.c_str());
                    anyValue = text.trimmed();
                }
            }

            if (!anyFound && !anyValue.isEmpty()) {
                anyFound = true;
                resultValue = anyValue;
            }
        }

        return anyFound;
    }

    bool getXmpDescription(Exiv2::XmpData &xmpData, const QString &langAlt, QString &description) {
        bool anyFound = false;

        anyFound = getXmpLangAltValue(xmpData, "Xmp.dc.description", langAlt, description);

        if (!anyFound || description.isEmpty()) {
            Exiv2::XmpKey psKey("Xmp.photoshop.Headline");
            Exiv2::XmpData::iterator xmpIt = xmpData.findKey(psKey);
            if (xmpIt != xmpData.end()) {
                const Exiv2::XmpTextValue &value = static_cast<const Exiv2::XmpTextValue &>(xmpIt->value());
                QString headline = QString::fromUtf8(value.value_.c_str()).trimmed();

                if (!headline.isEmpty()) {
                    anyFound = true;
                    description = headline;
                }
            }
        }

        return anyFound;
    }

    bool getXmpTitle(Exiv2::XmpData &xmpData, const QString &langAlt, QString &title) {
        bool anyFound = getXmpLangAltValue(xmpData, "Xmp.dc.title", langAlt, title);
        return anyFound;
    }

    bool getXmpTagStringBag(Exiv2::XmpData &xmpData, const char *propertyName, QStringList &bag) {
        bool found = false;

        Exiv2::XmpKey key(propertyName);
        Exiv2::XmpData::iterator it = xmpData.findKey(key);

        if ((it != xmpData.end()) && (it->typeId() == Exiv2::xmpBag)) {
            found = true;
            int count = it->count();
            bag.reserve(count);

            for (int i = 0; i < count; i++) {
                QString bagValue = QString::fromUtf8(it->toString(i).c_str());
                bag.append(bagValue);
            }
        }

        return found;
    }

    bool getXmpDateTime(Exiv2::XmpData &xmpData, QDateTime &dateTime) {
        bool found = false;

        Exiv2::XmpKey psKey("Xmp.photoshop.DateCreated");
        Exiv2::XmpData::iterator xmpIt = xmpData.findKey(psKey);

        if (xmpIt != xmpData.end()) {
            dateTime = QDateTime::fromString(QString::fromLatin1(xmpIt->toString().c_str()), Qt::ISODate);
            found = dateTime.isValid();
        }

        return found;
    }

    bool getXmpKeywords(Exiv2::XmpData &xmpData, QStringList &keywords) {
        return getXmpTagStringBag(xmpData, "Xmp.dc.subject", keywords);
    }

    bool getIptcString(Exiv2::IptcData &iptcData, const char *propertyName, bool isIptcUtf8, QString &resultValue) {
        bool anyFound = false;

        Exiv2::IptcKey key(propertyName);

        Exiv2::IptcData::iterator it = iptcData.findKey(key);
        if (it != iptcData.end()) {
            std::ostringstream os;
            os << *it;
            std::string str = os.str();

            QString value;

            if (isIptcUtf8 || Helpers::isUtf8(str.c_str())) {
                value = QString::fromUtf8(str.c_str()).trimmed();
            } else {
                value = QString::fromLocal8Bit(str.c_str()).trimmed();
            }

            if (!value.isEmpty()) {
                resultValue = value;
                anyFound = true;
            }
        }

        return anyFound;
    }

    bool getIptcDescription(Exiv2::IptcData &iptcData, bool isIptcUtf8, QString &description) {
        return getIptcString(iptcData, "Iptc.Application2.Caption", isIptcUtf8, description);
    }

    bool getIptcTitle(Exiv2::IptcData &iptcData, bool isIptcUtf8, QString &title) {
        return getIptcString(iptcData, "Iptc.Application2.ObjectName", isIptcUtf8, title);
    }

    bool getIptcKeywords(Exiv2::IptcData &iptcData, bool isIptcUtf8, QStringList &keywords) {
        bool anyAdded = false;
        QString keywordsTagName = QString::fromLatin1("Iptc.Application2.Keywords");

        for (Exiv2::IptcData::iterator it = iptcData.begin(); it != iptcData.end(); ++it) {
            QString key = QString::fromLocal8Bit(it->key().c_str());

            if (key == keywordsTagName) {
                QString tag;
                if (isIptcUtf8) {
                    tag = QString::fromUtf8(it->toString().c_str());
                } else {
                    tag = QString::fromLocal8Bit(it->toString().c_str());
                }

                keywords.append(tag);
                anyAdded = true;
            }
        }

        return anyAdded;
    }

    QString getExifUserComment(Exiv2::ExifData &exifData) {
        QString result;

        Exiv2::ExifKey key("Exif.Photo.UserComment");
        Exiv2::ExifData::iterator it = exifData.findKey(key);
        if (it != exifData.end()) {
            const Exiv2::Exifdatum& exifDatum = *it;

            std::string comment;
            std::string charset;

            comment = exifDatum.toString();

            // libexiv2 will prepend "charset=\"SomeCharset\" " if charset is specified
            // Before conversion to QString, we must know the charset, so we stay with std::string for a while
            if (comment.length() > 8 && comment.substr(0, 8) == "charset=") {
                // the prepended charset specification is followed by a blank
                std::string::size_type pos = comment.find_first_of(' ');

                if (pos != std::string::npos) {
                    // extract string between the = and the blank
                    charset = comment.substr(8, pos-8);
                    // get the rest of the string after the charset specification
                    comment = comment.substr(pos+1);
                }
            }

            if (charset == "\"Unicode\"") {
                result = QString::fromUtf8(comment.data());
            }
            else if (charset == "\"Jis\"") {
                QTextCodec* const codec = QTextCodec::codecForName("JIS7");
                result = codec->toUnicode(comment.c_str());
            }
            else if (charset == "\"Ascii\"") {
                result = QString::fromLatin1(comment.c_str());
            } else {
                result = Helpers::detectEncodingAndDecode(comment);
            }
        }

        return result;
    }

    bool getExifDescription(Exiv2::ExifData &exifData, QString &description) {
        bool foundDesc = false;

        Exiv2::ExifKey key("Exif.Image.ImageDescription");
        Exiv2::ExifData::iterator it = exifData.findKey(key);

        QString value;

        if (it != exifData.end()) {
            std::ostringstream os;
            os << *it;
            value = QString::fromLatin1(os.str().c_str());
        } else {
            value = getExifUserComment(exifData);
        }

        if (!value.isEmpty()) {
            description = value;
            foundDesc = true;
        }

        return foundDesc;
    }

    bool getExifDateTime(Exiv2::ExifData &exifData, QDateTime &dateTime) {
        bool found = false;

        Exiv2::ExifKey key("Exif.Photo.DateTimeOriginal");
        Exiv2::ExifData::iterator it = exifData.findKey(key);
        if (it != exifData.end()) {
            dateTime = QDateTime::fromString(QString::fromLatin1(it->toString().c_str()), Qt::ISODate);
            found = dateTime.isValid();
        }

        if (!found) {
            Exiv2::ExifKey imageKey("Exif.Image.DateTimeOriginal");
            Exiv2::ExifData::iterator imageIt = exifData.findKey(imageKey);
            if (imageIt != exifData.end()) {
                dateTime = QDateTime::fromString(QString::fromLatin1(imageIt->toString().c_str()), Qt::ISODate);
                found = dateTime.isValid();
            }
        }

        return found;
    }

    QString retrieveDescription(Exiv2::XmpData &xmpData, Exiv2::ExifData &exifData, Exiv2::IptcData &iptcData,
                                bool isIptcUtf8) {
        QString description;
        bool success = false;
        success = getXmpDescription(xmpData, QString::fromLatin1("x-default"), description);
        success = success || getIptcDescription(iptcData, isIptcUtf8, description);
        success = success || getExifDescription(exifData, description);
        return description;
    }

    QString retrieveTitle(Exiv2::XmpData &xmpData, Exiv2::ExifData &exifData, Exiv2::IptcData &iptcData,
                          bool isIptcUtf8) {
        QString title;
        bool success = false;
        success = getXmpTitle(xmpData, QString::fromLatin1("x-default"), title);
        success = success || getIptcTitle(iptcData, isIptcUtf8, title);
        Q_UNUSED(exifData);
        return title;
    }

    QStringList retrieveKeywords(Exiv2::XmpData &xmpData, Exiv2::ExifData &exifData, Exiv2::IptcData &iptcData,
                                 bool isIptcUtf8) {
        QStringList keywords;
        bool success = false;
        success = getXmpKeywords(xmpData, keywords);
        success = success || getIptcKeywords(iptcData, isIptcUtf8, keywords);
        Q_UNUSED(exifData);
        return keywords;
    }

    QDateTime retrieveDateTime(Exiv2::XmpData &xmpData, Exiv2::ExifData &exifData, Exiv2::IptcData &iptcData,
                               bool isIptcUtf8) {
        Q_UNUSED(iptcData);
        Q_UNUSED(isIptcUtf8);

        QDateTime dateTime;
        bool success = false;
        success = getXmpDateTime(xmpData, dateTime);
        success = success || getExifDateTime(exifData, dateTime);
        return dateTime;
    }

    Exiv2ReadingWorker::Exiv2ReadingWorker(int index, QVector<Models::ArtworkMetadata *> itemsToRead, QObject *parent):
        QObject(parent),
        m_ItemsToRead(itemsToRead),
        m_WorkerIndex(index),
        m_Stopped(false)
    {
        Q_ASSERT(!itemsToRead.isEmpty());
        LOG_INFO << "Worker [" << index << "]:" << itemsToRead.size() << "items to read";
    }

    Exiv2ReadingWorker::~Exiv2ReadingWorker() {
        LOG_DEBUG << "Reading worker" << m_WorkerIndex << "destroyed";
    }

    void Exiv2ReadingWorker::process() {
        LOG_DEBUG << "Worker #" << m_WorkerIndex << "started";

        bool anyError = false;

        int size = m_ItemsToRead.size();
        for (int i = 0; i < size; ++i) {
            if (m_Stopped) { break; }

            Models::ArtworkMetadata *artwork = m_ItemsToRead.at(i);
            const QString &filepath = artwork->getFilepath();
            ImportDataResult importResult;

            try {
                if (readMetadata(artwork, importResult)) {
                    Q_ASSERT(!m_ImportResult.contains(filepath));
                    m_ImportResult.insert(filepath, importResult);
                }
            }
            catch(Exiv2::Error &error) {
                anyError = true;
                LOG_WARNING << "Worker" << m_WorkerIndex << "Exiv2 error:" << error.what();
            }
            catch(...) {
                anyError = true;
                LOG_WARNING << "Worker" << m_WorkerIndex << "Reading error for item" << filepath;
            }
        }

        LOG_INFO << "Worker #" << m_WorkerIndex << "finished";

        emit finished(anyError);
    }

    void Exiv2ReadingWorker::cancel() {
        m_Stopped = true;
    }

    bool Exiv2ReadingWorker::readMetadata(Models::ArtworkMetadata *artwork, ImportDataResult &importResult) {
        const QString &filepath = artwork->getFilepath();

#if defined(Q_OS_WIN)
        Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(filepath.toStdWString());
#else
        Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(filepath.toStdString());
#endif
        image->readMetadata();

        Exiv2::XmpData &xmpData = image->xmpData();
        Exiv2::ExifData &exifData = image->exifData();
        Exiv2::IptcData &iptcData = image->iptcData();

        QString iptcEncoding = getIptcCharset(iptcData).toUpper();
        bool isIptcUtf8 = (iptcEncoding == QLatin1String("UTF-8")) || (iptcEncoding == QLatin1String("UTF8"));

        importResult.FilePath = filepath;
        importResult.Description = retrieveDescription(xmpData, exifData, iptcData, isIptcUtf8);
        importResult.Title = retrieveTitle(xmpData, exifData, iptcData, isIptcUtf8);
        importResult.Keywords = retrieveKeywords(xmpData, exifData, iptcData, isIptcUtf8);
        importResult.DateTimeOriginal = retrieveDateTime(xmpData, exifData, iptcData, isIptcUtf8);

        MetadataSavingCopy copy(artwork);
        if (copy.readFromFile()) {
            importResult.BackupDict = copy.getInfo();
        }

        QFileInfo fi(filepath);
        importResult.FileSize = fi.size();

        m_ImportResult.insert(importResult.FilePath, importResult);

        // BUMP
        return false;
    }
}
