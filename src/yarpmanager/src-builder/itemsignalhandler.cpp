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

#include "itemsignalhandler.h"
#include "arrow.h"
#include "sourceportitem.h"
#include "destinationportitem.h"

ItemSignalHandler::ItemSignalHandler(QObject *parent) : QObject(parent),
    parentItem(nullptr),
    type(ModuleItemType)
{}

ItemSignalHandler::ItemSignalHandler(QGraphicsItem *it,ItemType type,QObject *parent)  : QObject(parent)
{
    parentItem = (QGraphicsItem*)it;
    this->type = type;
}

void ItemSignalHandler::newConnectionRequested(QPointF p, QGraphicsItem *it)
{
    emit requestNewConnection(p,it);
}

void ItemSignalHandler::newConnectionAdded(QPointF p, QGraphicsItem *it)
{
    emit addNewConnection(p,it);
}

void ItemSignalHandler::onConnectionSelected(QGraphicsItem *it)
{
    emit connectctionSelected(it);
}

void ItemSignalHandler::onModuleSelected(QGraphicsItem *it)
{
    emit moduleSelected(it);
}

void ItemSignalHandler::onApplicationSelected(QGraphicsItem *it)
{
    emit applicationSelected(it);
}


void ItemSignalHandler::onConnectionComboChanged(QString text)
{
    if(parentItem && type == ArrowLabelItemType){
        ((Label*)parentItem)->currentComboTextChanged(text);
    }
}


void ItemSignalHandler::onEditingFinished()
{
    if(parentItem && type == SourcePortItemType){
        ((SourcePortItem*)parentItem)->editingFinished();
    }
    if(parentItem && type == DestinationPortItemType){
        ((DestinationPortItem*)parentItem)->editingFinished();
    }
}
