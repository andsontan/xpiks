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

#include "uiprovider.h"
#include <QQmlComponent>
#include <QQmlError>
#include <QQuickItem>
#include <QQmlEngine>
#include <QQuickView>
#include <QQmlProperty>
#include <QQmlContext>
#include <QDebug>

namespace Plugins {
    UIProvider::UIProvider(QObject *parent):
        QObject(parent)
    {
    }

    void UIProvider::openWindow(const QUrl &rcPath) const {
        QQmlComponent component(m_QmlEngine, rcPath);

        QObject::connect(&component, SIGNAL(statusChanged(QQmlComponent::Status)),
                         this, SLOT(viewStatusChanged(QQmlComponent::Status)));

        QQmlContext *context = new QQmlContext(m_QmlEngine);
        QObject *object = component.create(context);

        QObject::connect(object, SIGNAL(destroyed(QObject*)),
                         this, SLOT(windowDestroyed(QObject*)));

        QQmlEngine::setObjectOwnership(object, QQmlEngine::CppOwnership);

        object->setParent(m_Root);
    }

    void UIProvider::viewStatusChanged(QQmlComponent::Status status) {
        if (status == QQmlComponent::Error) {
            QQmlComponent *view = qobject_cast<QQmlComponent*>(sender());

            foreach (const QQmlError &error, view->errors()) {
                qWarning() << "UIProvider::viewStatusChanged #" << error.description();
            }
        }
    }

    void UIProvider::windowDestroyed(QObject *object) {
        Q_UNUSED(object);
        qInfo() << "UIProvider::windowDestroyed #" << "Plugin window destroyed";
    }
}
