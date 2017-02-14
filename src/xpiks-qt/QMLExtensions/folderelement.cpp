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

#include "folderelement.h"
#include <QPainter>
#include <QSGGeometryNode>
#include <QRect>
#include <QPointF>
#include <QDebug>
#include <QColor>
#include <QBrush>

namespace QMLExtensions {
    FolderElement::FolderElement(QQuickItem *parent):
        QQuickItem (parent),
        m_Geometry(QSGGeometry::defaultAttributes_Point2D(), 12),
        m_Thickness(2.0),
        m_Scale(1.0)
    {
        setFlag(ItemHasContents);
        m_Material.setColor(m_Color);
        m_Geometry.setDrawingMode(GL_LINES);
        m_Geometry.setLineWidth(m_Thickness * m_Scale);

        QObject::connect(this, SIGNAL(windowChanged(QQuickWindow*)), this, SLOT(windowChangedHandler(QQuickWindow*)));
    }

    void FolderElement::setColor(const QColor &color) {
        if (m_Color != color) {
            m_Color = color;
            m_Material.setColor(m_Color);
            update();
            emit colorChanged(color);
        }
    }

    void FolderElement::setThickness(qreal value) {
        if (m_Thickness != value) {
            m_Thickness = value;
            update();
            emit thicknessChanged(value);
        }
    }

    void FolderElement::windowChangedHandler(QQuickWindow *window) {
        if (window != nullptr) {
            m_Scale = window->devicePixelRatio();
            update();
        }
    }

    QSGNode *FolderElement::updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *) {
        QSGGeometryNode *node = 0;

        if (oldNode == nullptr) {
            node = new QSGGeometryNode();
            node->setGeometry(&m_Geometry);
            node->setMaterial(&m_Material);
        } else {
            node = static_cast<QSGGeometryNode *>(oldNode);
        }

        float lineWidth = (float)(m_Thickness * m_Scale);
        m_Geometry.setLineWidth(lineWidth);
        updateView(&m_Geometry);

        node->markDirty(QSGNode::DirtyGeometry);
        node->markDirty(QSGNode::DirtyMaterial);

        return node;
    }

    void FolderElement::itemChange(QQuickItem::ItemChange item, const QQuickItem::ItemChangeData &data) {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
        if (item == QQuickItem::ItemDevicePixelRatioHasChanged) {
            m_Scale = data.realValue;
            update();
        }
#endif

        QQuickItem::itemChange(item, data);
    }

    void FolderElement::updateView(QSGGeometry *geometry) {
        QSGGeometry::Point2D *geometryPoints = geometry->vertexDataAsPoint2D();
        const QRectF rect = boundingRect();

        QSGGeometry::Point2D v[7];

        v[0].x = rect.left();
        v[0].y = rect.top();
        v[1].x = rect.left() + rect.width()/2.5;
        v[1].y = rect.top();
        v[2].x = rect.left() + rect.width()/2.5;
        v[2].y = rect.top() + rect.height()/7.0;
        v[3].x = rect.right();
        v[3].y = v[2].y;
        v[4].x = rect.right();
        v[4].y = rect.bottom();
        v[5].x = rect.left();
        v[5].y = rect.bottom();
        // fictious first point (for the loop)
        v[6].x = v[0].x;
        v[6].y = v[0].y;

        for (int i = 0; i < 6; ++i) {
            geometryPoints[2*i] = v[i];
            geometryPoints[2*i + 1] = v[i + 1];
        }

        const qreal delta = m_Thickness / 2.0;

        geometryPoints[0].x -= delta;
        geometryPoints[5].x += delta;
        geometryPoints[6].y -= delta;
        geometryPoints[7].y += delta;
        geometryPoints[8].x += delta;
        geometryPoints[9].x -= delta;
        geometryPoints[10].y += delta;
        geometryPoints[11].y -= delta;
    }
}
