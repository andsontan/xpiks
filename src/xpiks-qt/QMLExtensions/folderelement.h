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

#ifndef FOLDERELEMENT_H
#define FOLDERELEMENT_H

#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickWindow>
#include <QBrush>
#include <QSGGeometry>
#include <QSGFlatColorMaterial>

namespace QMLExtensions {
    class FolderElement : public QQuickItem
    {
        Q_OBJECT
        Q_PROPERTY(QColor color READ getColor WRITE setColor NOTIFY colorChanged)
        Q_PROPERTY(qreal thickness READ getThickness WRITE setThickness NOTIFY thicknessChanged)
    public:
        FolderElement(QQuickItem *parent=0);

    public:
        const QColor &getColor() const { return m_Color; }
        qreal getThickness() const { return m_Thickness; }

    public slots:
        void setColor(const QColor &color);
        void setThickness(qreal value);

    private slots:
        void windowChangedHandler(QQuickWindow *window);

    signals:
        void colorChanged(const QColor &color);
        void thicknessChanged(qreal value);

    protected:
        virtual QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *);
        virtual void itemChange(ItemChange item, const ItemChangeData &data);

    private:
        void updateView(QSGGeometry *geometry);

    private:
        QSGGeometry m_Geometry;
        QSGFlatColorMaterial m_Material;
        QColor m_Color;
        qreal m_Thickness;
        qreal m_Scale;
    };
}

#endif // FOLDERELEMENT_H
