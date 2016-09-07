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

#ifndef ISUGGESTIONQUERYENGINE_H
#define ISUGGESTIONQUERYENGINE_H

#include <QStringList>
#include <vector>
#include <memory>
#include <QObject>
#include "suggestionartwork.h"

namespace Suggestion {

    class SuggestionQueryEngineBase: public QObject {
        Q_OBJECT
    public:
        SuggestionQueryEngineBase(int ID, QObject *parent = 0):
            QObject(parent),
            m_EngineID(ID)
        { }

        virtual ~SuggestionQueryEngineBase() { }

        virtual void submitQuery(const QStringList &queryKeywords) = 0;
        virtual QString getName() const = 0;

    public:
        int getID() const { return m_EngineID; }
        void cancelQueries() { emit cancelAllQueries(); }
        std::vector<std::shared_ptr<SuggestionArtwork> > &getLastResults() { return m_LastResults; }

        void setResults(std::vector<std::shared_ptr<SuggestionArtwork> > &results) {
            m_LastResults = std::move(results);
        }

    signals:
        void resultsAvailable();
        void cancelAllQueries();
        void errorReceived(const QString &error);

    private:
        std::vector<std::shared_ptr<SuggestionArtwork> > m_LastResults;
        int m_EngineID;
    };
}

#endif // ISUGGESTIONQUERYENGINE_H
