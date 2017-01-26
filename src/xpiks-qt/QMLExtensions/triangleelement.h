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

#ifndef TRIANGLEELEMENT_H
#define TRIANGLEELEMENT_H

#include <QtQuick/QQuickItem>
#include <QBrush>
#include <QSGGeometry>
#include <QSGFlatColorMaterial>

namespace QMLExtensions {
    class TriangleElement : public QQuickItem
    {
        Q_OBJECT
        Q_PROPERTY(QColor color READ getColor WRITE setColor NOTIFY colorChanged)
        Q_PROPERTY(bool isFlipped READ getIsFlipped WRITE setIsFlipped NOTIFY isFlippedChanged)
        Q_PROPERTY(bool isVertical READ getIsVertical WRITE setIsVertical NOTIFY isVerticalChanged)

    public:
        TriangleElement(QQuickItem *parent = 0);

        const QColor &getColor() const { return m_Color; }
        bool getIsFlipped() const { return m_IsFlipped; }
        bool getIsVertical() const { return m_IsVertical; }

    public slots:
        void setColor(const QColor &color);
        void setIsFlipped(bool value);
        void setIsVertical(bool value);

    signals:
        void colorChanged(const QColor &color);
        void isFlippedChanged(bool value);
        void isVerticalChanged(bool value);

    protected:
        virtual QSGNode *updatePaintNode(QSGNode*, UpdatePaintNodeData *data) override;

    private:
        void updateTriangle(QSGGeometry *geometry);

    private:
        QSGGeometry m_Geometry;
        QSGFlatColorMaterial m_Material;
        QColor m_Color;
        bool m_IsFlipped;
        bool m_IsVertical;
    };
}

#endif // TRIANGLEELEMENT_H
