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

#ifndef BUILDERITEM_H
#define BUILDERITEM_H

#include <QObject>
#include <QAction>
#include <QPainter>
#include <QGraphicsPolygonItem>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <yarp/manager/manager.h>

#include "commons.h"

using namespace yarp::manager;

class PortItem;
class ItemSignalHandler;
class Arrow;



class BuilderItem :   public QGraphicsObject
{
    friend class PortItem;
    friend class Arrow;
    friend class Label;

public:

    BuilderItem(QGraphicsObject * parent = 0) : QGraphicsObject(parent),
        itemType(ModuleItemType),
        offset(QPointF(0,0)),
        pressed(false),
        moved(false),
        creatingNewConnection(false),
        allowInputs(false),
        allowOutputs(false),
        snap(false),
        nestedInApp(false),
        sigHandler(nullptr)
    {}

    QRectF boundingRect() const override = 0;
    virtual QPointF connectionPoint() = 0;
    int type() const override = 0;

    QString getItemName();
    QPointF getStartingPoint();
    ItemSignalHandler *signalHandler();
    void removeArrow(Arrow *arrow);
    void removeArrows();
    void addArrow(Arrow *arrow);
    QList<Arrow *>* getArrows();

    bool arrowAlreadyPresent(BuilderItem *endItem);
    void updateConnections();
    bool allowInputConnections();
    bool allowOutputConnections();
    void snapToGrid(bool snap);
    QPointF computeTopLeftGridPoint(const QPointF &pointP);
    QList<QAction*> getActions();
    void setActions(QList<QAction*>);
    bool isNestedInApp();


protected:
    QFont font;
    ItemType itemType;
    QString itemName;

    QRectF mainRect;
    QRectF boundingR;

    QPointF offset;

    bool pressed;
    bool moved;
    bool creatingNewConnection;
    bool allowInputs;
    bool allowOutputs;
    bool snap;
    bool nestedInApp;

    ItemSignalHandler *sigHandler;
    QList<Arrow *> arrows;
    QList<QAction*> actions;

    QPointF startingPoint;

};

#endif // BUILDERITEM_H
