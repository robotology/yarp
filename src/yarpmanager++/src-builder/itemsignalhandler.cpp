#include "itemsignalhandler.h"
#include "arrow.h"
#include "sourceportitem.h"
#include "destinationportitem.h"

ItemSignalHandler::ItemSignalHandler(QObject *parent)  : QObject(parent)
{
    //parentItem = it;
}

ItemSignalHandler::ItemSignalHandler(QGraphicsItem *it,ItemType type,QObject *parent)  : QObject(parent)
{
    parentItem = (QGraphicsItem*)it;
    this->type = type;
}

void ItemSignalHandler::newConnectionRequested(QPointF p, QGraphicsItem *it)
{
    requestNewConnection(p,it);
}

void ItemSignalHandler::newConnectionAdded(QPointF p, QGraphicsItem *it)
{
    addNewConnection(p,it);
}

void ItemSignalHandler::onConnectionSelected(QGraphicsItem *it)
{
    connectctionSelected(it);
}

void ItemSignalHandler::onModuleSelected(QGraphicsItem *it)
{
    moduleSelected(it);
}

void ItemSignalHandler::onApplicationSelected(QGraphicsItem *it)
{
    applicationSelected(it);
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
