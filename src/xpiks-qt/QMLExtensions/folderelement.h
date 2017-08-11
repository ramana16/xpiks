/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
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
        virtual QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) override;
        virtual void itemChange(ItemChange item, const ItemChangeData &data) override;

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
