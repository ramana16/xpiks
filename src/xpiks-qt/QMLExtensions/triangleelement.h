/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
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
