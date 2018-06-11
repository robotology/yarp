/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * LGPLv2.1+ license. See the accompanying LICENSE file for details.
 */

#ifndef ITEMSIGNALHANDLER_H
#define ITEMSIGNALHANDLER_H

#include <QObject>
#include <QGraphicsItem>
#include "moduleitem.h"
//#include "arrow.h"

class Label;

class ItemSignalHandler : public QObject
{
    Q_OBJECT

public:
    ItemSignalHandler(QObject *parent = NULL);
    ItemSignalHandler(QGraphicsItem *it, ItemType type, QObject *parent = NULL);
    void newConnectionRequested(QPointF, QGraphicsItem *it);
    void newConnectionAdded(QPointF, QGraphicsItem *it);

private:
       QGraphicsItem *parentItem;
       ItemType type;

signals:
    void addNewConnection(QPointF,QGraphicsItem *it);
    void requestNewConnection(QPointF,QGraphicsItem *it);
    void moduleSelected(QGraphicsItem *it);
    void connectctionSelected(QGraphicsItem *it);
    void applicationSelected(QGraphicsItem *it);
    void modified();
    void moved();

public slots:
    void onConnectionSelected(QGraphicsItem *it);
    void onModuleSelected(QGraphicsItem *it);
    void onApplicationSelected(QGraphicsItem *it);
    void onConnectionComboChanged(QString);
    void onEditingFinished();

};

#endif // ITEMSIGNALHANDLER_H
