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

#include "triangleelement.h"
#include <QPainter>
#include <QSGGeometryNode>
#include <QRect>
#include <QPointF>
#include <QDebug>
#include <QColor>
#include <QBrush>

namespace QMLExtensions {
    TriangleElement::TriangleElement(QQuickItem *parent) :
        QQuickItem(parent),
        m_Geometry(QSGGeometry::defaultAttributes_Point2D(), 3),
        m_IsFlipped(false)
    {
        setFlag(ItemHasContents);
        m_Material.setColor(m_Color);
    }

    QSGNode* TriangleElement::updatePaintNode(QSGNode* n, UpdatePaintNodeData*) {
        if (n == NULL) {
            n = new QSGNode;
        }

        QSGGeometryNode* geomnode = new QSGGeometryNode();

        QSGGeometry::Point2D* v = m_Geometry.vertexDataAsPoint2D();
        const QRectF rect = boundingRect();

        if (m_IsFlipped) {
            v[0].x = rect.left();
            v[0].y = rect.top();
            v[1].x = rect.left() + rect.width()/2;
            v[1].y = rect.bottom();
            v[2].x = rect.right();
            v[2].y = rect.top();
        } else {
            v[0].x = rect.left();
            v[0].y = rect.bottom();
            v[1].x = rect.left() + rect.width()/2;
            v[1].y = rect.top();
            v[2].x = rect.right();
            v[2].y = rect.bottom();
        }

        geomnode->setGeometry(&m_Geometry);
        geomnode->setMaterial(&m_Material);

        n->appendChildNode(geomnode);
        return n;
    }

    void TriangleElement::setColor(const QColor &color) {
        if (m_Color != color) {
            m_Color = color;
            m_Material.setColor(m_Color);
            update();
            emit colorChanged(color);
        }
    }
}
