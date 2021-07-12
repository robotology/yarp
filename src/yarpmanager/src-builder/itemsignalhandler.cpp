/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
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
