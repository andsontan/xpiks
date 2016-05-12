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
#include <sstream>
#include "../xpiks-qt/Models/artworkmetadata.h"
#include "../xpiks-qt/Common/defines.h"
#include "../xpiks-qt/Helpers/stringhelper.h"
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

    Helpers::ExifEncoding getExifEncoding(Exiv2::ExifData &exifData) {
        Helpers::ExifEncoding encoding = Helpers::ExifEncodingUnknown;

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
                encoding = Helpers::ExifEncodingUtf8;
            }
            else if (charset == "\"Jis\"") {
                encoding = Helpers::ExifEncodingJis;
            }
            else if (charset == "\"Ascii\"") {
                encoding = Helpers::ExifEncodingAscii;
            }
        }

        return encoding;
    }

    bool getXmpDescription(Exiv2::XmpData &xmpData, const QString &langAlt, QString &description) {
        bool anyFound = false;

        Exiv2::XmpKey key("Xmp.dc.description");
        Exiv2::XmpData::iterator it = xmpData.findKey(key);
        if (it != xmpData.end()) {
            Q_ASSERT(it->typeId() == Exiv2::langAlt);
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
                        description = text.trimmed();
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
                description = anyValue;
            }
        }

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

    bool getIptcDescription(Exiv2::IptcData &iptcData, const QString &charset, QString &description) {
        bool foundDesc = false;

        Exiv2::IptcKey key("Iptc.Application2.Caption");

        Exiv2::IptcData::iterator it = iptcData.findKey(key);
        if (it != iptcData.end()) {
            std::ostringstream os;
            os << *it;
            std::string str = os.str();

            QString value;

            if (charset == QLatin1String("UTF-8") ||
                    charset == QLatin1String("UTF8") ||
                    Helpers::isUtf8(str.c_str())) {
                value = QString::fromUtf8(str.c_str()).trimmed();
            } else {
                value = QString::fromLocal8Bit(str.c_str()).trimmed();
            }

            if (!value.isEmpty()) {
                description = value;
                foundDesc = true;
            }
        }

        return foundDesc;
    }

    bool getExifDescription(Exiv2::ExifData &exifData, Helpers::ExifEncoding exifEncoding, QString &description) {
        bool foundDesc = false;

        QString value;

        Exiv2::ExifKey key("Exif.Image.ImageDescription");
        Exiv2::ExifData::iterator it = exifData.findKey(key);

        if (it != exifData.end()) {
            std::ostringstream os;
            os << *it;
            std::string str = os.str();

            switch (exifEncoding) {
                case Helpers::ExifEncodingUtf8: {
                    value = QString::fromUtf8(str.c_str()).trimmed();
                    break;
                }
                case Helpers::ExifEncodingJis: {
                    QTextCodec* const codec = QTextCodec::codecForName("JIS7");
                    value = codec->toUnicode(str.c_str());
                    break;
                }
                case Helpers::ExifEncodingAscii: {
                    value = QString::fromLatin1(str.c_str()).trimmed();
                    break;
                }
                default: {
                    if (Helpers::isUtf8(str.c_str())) {
                        value = QString::fromUtf8(str.c_str()).trimmed();
                    } else {
                        value = QString::fromLocal8Bit(str.c_str()).trimmed();
                    }
                }
            }

            if (!value.isEmpty()) {
                description = value;
                foundDesc = true;
            }
        }

        return foundDesc;
    }

    QString retrieveDescription(Exiv2::XmpData &xmpData, Exiv2::ExifData &exifData, Exiv2::IptcData &iptcData,
                                const QString &iptcEncoding, Helpers::ExifEncoding exifEncoding) {
        QString description;
        bool success = false;
        success = getXmpDescription(xmpData, QString(), description);
        success = success || getIptcDescription(iptcData, iptcEncoding, description);
        success = success || getExifDescription(exifData, exifEncoding, description);
        return description;
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

        Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(filepath.toStdWString());
        image->readMetadata();

        Exiv2::XmpData &xmpData = image->xmpData();
        Exiv2::ExifData &exifData = image->exifData();
        Exiv2::IptcData &iptcData = image->iptcData();

        QString iptcEncoding = getIptcCharset(iptcData);
        Helpers::ExifEncoding exifEncoding = getExifEncoding(exifData);

        importResult.FilePath = filepath;
        importResult.Description = retrieveDescription(xmpData, exifData, iptcData, iptcEncoding, exifEncoding);

        m_ImportResult.insert(importResult.FilePath, importResult);

        // BUMP
        return false;
    }
}
