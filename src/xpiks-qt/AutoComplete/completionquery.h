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

#ifndef COMPLETIONQUERY_H
#define COMPLETIONQUERY_H

#include <QObject>
#include <QString>
#include <QStringList>
#include "autocompletemodel.h"
#include "../Common/defines.h"

namespace AutoComplete {
    class CompletionQuery : public QObject {
        Q_OBJECT
    public:
        CompletionQuery(const QString &prefix, AutoCompleteModel *autoCompleteModel) :
            m_Prefix(prefix),
            m_AutoCompleteModel(autoCompleteModel)
        {
            Q_ASSERT(autoCompleteModel != nullptr);
        }

        const QString &getPrefix() const { return m_Prefix; }

        void setCompletions(const QStringList &completions) {
            m_AutoCompleteModel->setCompletions(completions);
            emit completionsAvailable();
        }

    signals:
        void completionsAvailable();

    private:
        QString m_Prefix;
        AutoCompleteModel *m_AutoCompleteModel;
    };
}

#endif // COMPLETIONQUERY_H
