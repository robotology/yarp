/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
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
