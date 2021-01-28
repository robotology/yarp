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
