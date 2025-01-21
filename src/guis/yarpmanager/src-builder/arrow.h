/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

/**
 * Original license follows:
 */

/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef ARROW_H
#define ARROW_H

#include <QStaticText>
#include <QGraphicsLineItem>
#include <QGraphicsProxyWidget>
#include <QComboBox>
#include "commons.h"
#include "itemsignalhandler.h"
#include "builderitem.h"



QT_BEGIN_NAMESPACE
class QGraphicsPolygonItem;
class QGraphicsLineItem;
class QGraphicsScene;
class QRectF;
class QGraphicsSceneMouseEvent;
class QPainterPath;
QT_END_NAMESPACE

class LineHandle;
class Label : public QGraphicsTextItem
{
public:
    Label(QString label, QGraphicsItem *parent = 0);
    ~Label();
    int type() const override { return UserType + (int)ArrowLabelItemType; }
    bool hasBeenMoved();
    void setHasMoved(bool);
    void currentComboTextChanged(QString text);
    QString labelText();
    void setText(QString);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

    QPointF computeTopLeftGridPoint(const QPointF &pointP);
protected:
    QGraphicsProxyWidget *comboWidget;
    QString text;
    ItemSignalHandler *sigHandler;
    bool moved;
    bool hasMoved;
    bool pressed;
    Arrow *parentArrow;
    QPointF offset;
};
//class ItemSignalHandler;

class Arrow : public BuilderItem
{
public:

    friend class LineHandle;
    friend class Label;

    Arrow(BuilderItem *startItem, BuilderItem *endItem,
          int id, Manager *safeManager, bool nestedInApp = false, bool editingMode = false, BuilderItem *parent = 0);

    ~Arrow();
    int type() const override { return (int)QGraphicsItem::UserType + (int)itemType; }

    QPointF connectionPoint() override;
    void setConnected(bool);
    int getId();
    QString getFrom();
    QString getTo();
    void setConnection(const Connection& conn);
    void setConnectionSelected(bool selected);
    void updateModel();
    void updateCarrier(QString carrier);
    GraphicModel* getModel();

    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void setColor(const QColor &color) { myColor = color; }
    BuilderItem *startItem() const { return myStartItem; }
    BuilderItem *endItem() const { return myEndItem; }

    void addHandle(QPointF clickPos);
    void updatePosition();
    QList <LineHandle*> handles();
    void removeHandle(LineHandle*);
    int getHandleIndex(LineHandle *handle);
    LineHandle *getLineHandle(int index);
    void updateConnectionFrom(QString from);
    void updateConnectionTo(QString to);
    void updateGraphicModel();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *e) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *e) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *e) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;


private:
    Manager *manager;
    GraphicModel model;
    bool externalSelection;

    bool editingMode;
    bool connected;
    yarp::manager::Connection connection;
    BuilderItem *myStartItem;
    BuilderItem *myEndItem;
    QColor myColor;
    QPolygonF arrowHead;
    Label textLbl;
    int textWidth;
    QFont font;
    //QLineF line;
    QPolygonF polyline;
    QPolygonF boundingPolyline;
    int id;


    QPointF startP;
    QPointF endP;

    QList <LineHandle*> handleList;
};


class LineHandle : public QGraphicsRectItem
{
public:
    LineHandle(QPointF center, Arrow *parent = 0);
    ~LineHandle();
    QPointF handlePoint();
    int type() const override { return UserType + (int)HandleItemType; }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    QPointF computeTopLeftGridPoint(const QPointF &pointP);
private:
    QPointF center;
    Arrow *parent;
    bool pressed;
    int rectSize;
    QPointF offset;
    bool ctrlPressed;
};




#endif // ARROW_H
