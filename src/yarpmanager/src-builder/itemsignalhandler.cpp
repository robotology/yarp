/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * LGPLv2.1+ license. See the accompanying LICENSE file for details.
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
