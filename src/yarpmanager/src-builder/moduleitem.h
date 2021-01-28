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

#ifndef MODULEITEM_H
#define MODULEITEM_H


#include <QObject>
#include <QPainter>
#include <QGraphicsPolygonItem>

#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>

#include "commons.h"
#include "itemsignalhandler.h"
#include "arrow.h"
#include "builderitem.h"
#include <yarp/manager/manager.h>

class PortItem;
class ItemSignalHandler;
class Arrow;



class ModuleItem : public BuilderItem
{

    friend class Arrow;
    friend class PortItem;

public:
   // ModuleItem(QString itemName, QStringList inputPorts, QStringList outputPorts , BuilderItem * parent = 0);
     ModuleItem(Module *module, int moduleId, bool nestedInApp = false, bool editingMode = false,
                Manager *manager = NULL, BuilderItem * parent = 0);
    ~ModuleItem();
    QRectF boundingRect() const override;
    QPointF connectionPoint() override;
    int getId();
    void setRunning(bool);
    void setModuleSelected(bool selected);
    int type() const override;
    void updateGraphicModel();


    Module *getInnerModule();

    QList <PortItem*> iPorts;
    QList <PortItem*> oPorts;

private:
    void init();


private:
    Manager *manager;
    bool externalSelection;
    bool editingMode;
    int moduleId;
    Module *module;
    QStringList inputPorts;
    QStringList outputPorts;
    bool running;




protected:
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *e) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *e) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *e) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

    void portPressed(PortItem *port, QGraphicsSceneMouseEvent *e);
    void portReleased(PortItem *port, QGraphicsSceneMouseEvent *e);
    void portMoved(PortItem *port, QGraphicsSceneMouseEvent *e);

signals:
    //void moduleSelected(int);

};

class PortItem : public BuilderItem
{
    friend class Arrow;

public:
    enum PortStatus {unknown,availbale,unavailable} ;
    //PortItem(QString portName, int type, BuilderItem *parent = 0);
    PortItem(InputData*, BuilderItem *parent = 0);
    PortItem(OutputData*, BuilderItem *parent = 0);
    QRectF boundingRect() const override;
    QPointF connectionPoint() override;
    int type() const override;
    int getPortType();
    InputData *getInputData();
    OutputData *getOutputData();
    void setAvailable(PortStatus);

    OutputData *outData;
    InputData *inData;
protected:
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent * event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent * event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    PortStatus portAvailable;
    BuilderItem *parent;
    QPolygonF polygon;
    int portType;
    qreal triangleH;
    bool hovered;
};

#endif // MODULEITEM_H
