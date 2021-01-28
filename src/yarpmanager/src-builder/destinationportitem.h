/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef DESTINATIONPORTITEM_H
#define DESTINATIONPORTITEM_H

#include <QObject>
#include <QPainter>
#include <QGraphicsPolygonItem>

#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>

#include "commons.h"
#include "arrow.h"
#include "itemsignalhandler.h"
#include "builderitem.h"

class ItemSignalHandler;
class Arrow;

class DestinationPortItem : public BuilderItem
{
    friend class Arrow;
public:
    DestinationPortItem(QString itemName,bool nestedInApp = false,
                        bool editOnStart = false, Application *app = NULL,BuilderItem * parent = 0);
    ~DestinationPortItem();
    QRectF boundingRect() const override;
    QPointF connectionPoint() override;
    int type() const override;
    void setAvailable(bool);
    void editingFinished();
    bool isErrorState();


private:
    void updateConnectionsTo(QString to);

private:
    Application *app;
    BuilderItem *parent;
    bool portAvailable;
    QGraphicsProxyWidget *lineEditWidget;
    bool errorState;


protected:
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *e) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *e) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *e) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *e) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
};

#endif // DESTINATIONPORTITEM_H
