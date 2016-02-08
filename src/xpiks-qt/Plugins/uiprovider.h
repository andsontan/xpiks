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

#ifndef UIPROVIDER_H
#define UIPROVIDER_H

#include "iuiprovider.h"
#include <QQuickItem>
#include <QQmlComponent>
#include <QObject>
#include <QUrl>

namespace Plugins {
    class UIProvider : public QObject, public IUIProvider
    {
        Q_OBJECT
    public:
        UIProvider(QObject *parent=0);
        virtual ~UIProvider() {}

    public:
        void setQmlEngine(QQmlEngine *engine) { m_QmlEngine = engine; }
        void setRoot(QQuickItem *root) { m_Root = root; }

        // IUIProvider interface
    public:
        virtual void openWindow(const QUrl &rcPath) const;

    private slots:
        void viewStatusChanged(QQmlComponent::Status status);
        void windowDestroyed(QObject *object);

    private:
        QQmlEngine *m_QmlEngine;
        QQuickItem *m_Root;
    };
}

#endif // UIPROVIDER_H
