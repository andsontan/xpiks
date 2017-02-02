/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
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

#include <QQuickItem>
#include <QQmlComponent>
#include <QQuickWindow>
#include <QObject>
#include <QUrl>

namespace Models {
    class UIManager;
}

namespace Plugins {
    class UIProvider : public QObject
    {
        Q_OBJECT
    public:
        UIProvider(QObject *parent=0);
        virtual ~UIProvider() {}

    public:
        Models::UIManager *getUIManager() const { return m_UiManager; }

    public:
        void setQmlEngine(QQmlEngine *engine) { m_QmlEngine = engine; }
        void setRoot(QQuickItem *root) { m_Root = root; }
        void setUIManager(Models::UIManager *manager) { m_UiManager = manager; }

        // IUIProvider interface
    public:
        void openWindow(const QUrl &rcPath, const QHash<QString, QObject*> &contextModels = QHash<QString, QObject*>()) const;

    private slots:
        void viewStatusChanged(QQmlComponent::Status status);
        void windowDestroyed(QObject *object);
        void contextDestroyed(QObject *object);
        void windowClosing(QQuickCloseEvent* closeEvent);

    private:
        QQmlEngine *m_QmlEngine;
        QQuickItem *m_Root;
        Models::UIManager *m_UiManager;
    };
}

#endif // UIPROVIDER_H
