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
#include "../Models/artworkmetadata.h"
#include "../Common/defines.h"
#include <exiv2/exiv2.hpp>
#include <string>

#define X_DEFAULT QString::fromLatin1("x-default")

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
                const std::string &txt((*it).toUtf8().constData());
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
                    const std::string &val((*it).toUtf8().constData());
                    xmpTxtVal->read(val);
                }
            }
        }

        QString txtLangAlt = QString(QString::fromLatin1("lang=%1 %2")).arg(langAlt).arg(text);
        const std::string &txt(txtLangAlt.toUtf8().constData());
        xmpTxtVal->read(txt);

        removeXmpTag(xmpData, propertyName);
        xmpData.add(Exiv2::XmpKey(propertyName), xmpTxtVal.get());
    }

    void setXmpKeywords(Exiv2::XmpData &xmpData, const QStringList &keywords) {
        try {
            setXmpTagStringBag(xmpData, "Xmp.dc.subject", keywords);
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
            setXmpTagStringLangAlt(xmpData, "Xmp.dc.description", X_DEFAULT, description);
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
            setXmpTagStringLangAlt(xmpData, "Xmp.dc.title", X_DEFAULT, title);
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
        iptcData["Iptc.Envelope.CharacterSet"] = "\33%G";
    }

    void setIptcString(Exiv2::IptcData &iptcData, const char *propertyName, const QString &value) {
        iptcData[propertyName] = std::string(value.toUtf8().constData());
        setIptcEncodingUtf8(iptcData);
    }

    Exiv2WritingWorker::Exiv2WritingWorker(int index, QVector<Models::ArtworkMetadata *> itemsToWrite, QObject *parent) :
        QObject(parent),
        m_ItemsToWrite(itemsToWrite),
        m_WorkerIndex(index),
        m_Stopped(false)
    {
        Q_ASSERT(!itemsToWrite.isEmpty());
        LOG_INFO << "Worker [" << index << "]:" << itemsToWrite.size() << "items to read";
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

    }
}
