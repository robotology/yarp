/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "builderscene.h"
#include <QDebug>
#include <QMimeData>
#include "moduleitem.h"
#include "sourceportitem.h"
#include "destinationportitem.h"
#include <QCursor>
#include "arrow.h"
#include <QGraphicsView>
#include <QGraphicsSceneWheelEvent>

BuilderScene::BuilderScene(QObject *parent) :
    QGraphicsScene(parent),
    currentLine(nullptr),
    startConnectionItem(nullptr),
    endConnectionItem(nullptr),
    snap(false),
    editingMode(false)
{
    //connect(this,SIGNAL(changed(QList<QRectF>)),this,SLOT(onSceneChanged(QList<QRectF>)));
    setStickyFocus(true);
}



void BuilderScene::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    qDebug() << "Enter Drag";
    QString itemType = event->mimeData()->text();
    if(itemType == "module" ||
       itemType == "application"){
        event->setAccepted(true);
        qDebug() << "Enter Drag ACCEPTED";
    }else{
        event->setAccepted(false);
        qDebug() << "Enter Drag REJECTED";
    }



}

void BuilderScene::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    qDebug() << "dropEvent";
    qlonglong pointer = event->mimeData()->data("pointer").toLongLong();
    QString itemType = event->mimeData()->text();

    // Deselect all
    foreach (QGraphicsItem *it, selectedItems()) {
        it->setSelected(false);
    }

    if(itemType == "module" ){
        auto* mod = (yarp::manager::Module*)pointer;
        emit addedModule((void*)mod,event->scenePos());
    }
    if(itemType == "application" ){
        auto* app = (yarp::manager::Application*)pointer;
        emit addedApplication((void*)app,event->scenePos());
    }

}

void BuilderScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    if(!editingMode){
        event->setAccepted(false);
        return;
    }
    event->setAccepted(true);
}

void BuilderScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF p = event->scenePos();
    QGraphicsItem *it = itemAt(p,QTransform());
    if(currentLine && !it){
        removeItem(currentLine);
        delete currentLine;
        currentLine = nullptr;
    }

    QGraphicsScene::mousePressEvent(event);


    //startConnectionItem = NULL;
}

//void BuilderScene::wheelEvent(QGraphicsSceneWheelEvent *event)
//{
//    qDebug() << event->pos();

//    if(event->modifiers() == Qt::CTRL){
//        if(event->delta() > 0){
//            views().first()->scale(1.1,1.1);
//        }else{
//            views().first()->scale(0.9,0.9);
//        }

//        views().first()->centerOn(event->pos());

//    }
//}

void BuilderScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseReleaseEvent(event);
}

void BuilderScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    //qDebug() << "mouseMoveEvent";
    if(currentLine){
        //qDebug() << startingPoint.x() << " || " << startingPoint.y() << " || " << QCursor::pos().x() << " || " << QCursor::pos().y();
        currentLine->setLine(startingPoint.x(),startingPoint.y(),event->scenePos().x()-1,event->scenePos().y()-1);
    }
    QGraphicsScene::mouseMoveEvent(event);
}

void BuilderScene::onNewConnectionRequested(QPointF p,QGraphicsItem *item)
{

    startConnectionItem = nullptr;
    if(!editingMode || !((BuilderItem*)item)->allowOutputConnections()){
        return;
    }

    if(!currentLine){
        startConnectionItem = item;
        startingPoint = item->mapToScene(p);
        currentLine = new QGraphicsLineItem();
        addItem(currentLine);
    }else{
        removeItem(currentLine);
        delete currentLine;
        currentLine = nullptr;
    }



}

void BuilderScene::onNewConnectionAdded(QPointF p,QGraphicsItem *item)
{
    qDebug() << "onNewConnectionAdded";
    if(!editingMode || !item){
        return;
    }
    if(startConnectionItem){
        if(currentLine){
            removeItem(currentLine);
            delete currentLine;
            currentLine = nullptr;
        }

            auto* startItem = (BuilderItem*)startConnectionItem;
            auto* endItem = (BuilderItem*)item;

            if(!startItem || !endItem){
                return;
            }

            if(startItem->type() == QGraphicsItem::UserType + (int)ModuleItemType || endItem->type() == QGraphicsItem::UserType + (int)ModuleItemType){
                return;
            }

            // controllo di non cambiare una applicazione nested.
            if(startItem->isNestedInApp() && endItem->isNestedInApp()){

                BuilderItem *startParent;
                BuilderItem *endParent;

                if(startItem->type() == QGraphicsItem::UserType + (int)ModulePortItemType){
                    startParent = (BuilderItem *)startItem->parentItem()->parentItem();
                }else{
                    startParent = (BuilderItem *)startItem->parentItem();
                }

                if(endItem->type() == QGraphicsItem::UserType + (int)ModulePortItemType){
                    endParent = (BuilderItem *)endItem->parentItem()->parentItem();
                }else{
                    endParent = (BuilderItem *)endItem->parentItem();
                }

                if(startParent && endParent &&
                   startParent->type() == QGraphicsItem::UserType + (int)ApplicationItemType &&
                   endParent->type() == QGraphicsItem::UserType + (int)ApplicationItemType){
                    if(startParent == endParent){
                        return;
                    }
                }
            }



           if (startItem->allowOutputConnections() &&
               endItem->allowInputConnections()) {
               if(!startItem->arrowAlreadyPresent(endItem)){
                   emit addNewConnection(startItem,endItem);
               }
           }

        startConnectionItem = nullptr;
    }

}

void BuilderScene::onSceneChanged(QList<QRectF> rects)
{
    foreach (QRectF r, rects) {
        QList<QGraphicsItem *> startItems = items(r);

        for(int i=0;i<startItems.count();i++){
            QGraphicsItem *gIt = startItems.at(i);
            if(gIt){
                if(gIt == currentLine){
                    return;
                }
                if(gIt->type() == QGraphicsItem::UserType + (int) ConnectionItemType){

                    return;

                }
                auto* it = (BuilderItem*)gIt;
                it->updateConnections();
                //qDebug() << "UPDATE";
            }
        }
    }
}

void BuilderScene::snapToGrid(bool snap)
{
    this->snap = snap;
    foreach (QGraphicsItem *it, items()) {
        if(it->type() == QGraphicsItem::UserType + (int)HandleItemType ||
            it->type() == QGraphicsItem::UserType + (int)ArrowLabelItemType){
            continue;
        }

        ((BuilderItem*)it)->snapToGrid(snap);
    }
}
