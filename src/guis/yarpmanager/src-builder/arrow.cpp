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


#include "arrow.h"
#include "destinationportitem.h"
#include "sourceportitem.h"

#include <cmath>

#include <QPen>
#include <QPainter>
#include <QGraphicsDropShadowEffect>
#include <QStyleOptionGraphicsItem>
#include <QDebug>

#define AUTOSNIPE_MARGINE 5

const qreal Pi = 3.14;



Arrow::Arrow(BuilderItem *startItem, BuilderItem *endItem,int id, Manager *safeManager,
             bool isInApp,bool editingMode, BuilderItem *parent) :
    BuilderItem(parent),
    manager(safeManager),
    externalSelection(false),
    editingMode(editingMode),
    connected(false),
    myStartItem(startItem),
    myEndItem(endItem),
    myColor(Qt::black),
    textLbl("",this),
    textWidth(0),
    id(id)
{
    itemType = ConnectionItemType;
    nestedInApp = isInApp;
    sigHandler = new ItemSignalHandler();
    setFlag(ItemIsSelectable,true);
    setFlag(ItemClipsToShape,false);
    setToolTip(QString("%1 --> %2").arg(myStartItem->itemName).arg(myEndItem->itemName));
    textLbl.setFlag(ItemIsMovable,!nestedInApp);
    textLbl.setFlag(ItemSendsGeometryChanges,!nestedInApp);
}

Arrow::~Arrow()
{
    hide();
    setToolTip("");
    startItem()->removeArrow(this);
    endItem()->removeArrow(this);
    handleList.clear();

    if(editingMode && manager){
        Application* mainApplication = manager->getKnowledgeBase()->getApplication();
        manager->getKnowledgeBase()->removeConnectionFromApplication(mainApplication, connection);
    }
}

GraphicModel* Arrow::getModel()
{
    return &model;
}

void Arrow::setConnection(const Connection& conn)
{
    connection = conn;
    QString label = connection.carrier();
    if(!label.isEmpty()){
        textLbl.setText(label);
    }
    QFontMetrics fontMetric(font);
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
    textWidth = fontMetric.horizontalAdvance(label);
#else
    textWidth = fontMetric.width(label);
#endif

    GraphicModel mod = connection.getModelBase();
    if(mod.points.size() > 0){
        GyPoint p = mod.points[0];
        if(p.x >= 0 && p.y >= 0){
            textLbl.setHasMoved(true);
            textLbl.setPos(p.x,p.y);
        }
        if(mod.points.size() > 3){
            // Handles
            startP = mapFromItem(myStartItem,myStartItem->connectionPoint());
            endP = mapFromItem(myEndItem,myEndItem->connectionPoint());
            polyline.clear();
            polyline.prepend(startP);

            for(unsigned int i=2; i<mod.points.size() - 1;i++){
                GyPoint p = mod.points[i];
                QPointF point(p.x,p.y);
                auto* handle = new LineHandle(point,this);
                handleList.append(handle);
                polyline.append(point);
                qDebug() << "APPENDING " << handle;
            }
            polyline.append(endP);
            boundingPolyline = polyline;
        }
    }

    update();
}

void Arrow::updateCarrier(QString carrier)
{
    if(!editingMode){
        return;
    }
    Application* mainApplication = nullptr;
    mainApplication = manager->getKnowledgeBase()->getApplication();
    manager->getKnowledgeBase()->removeConnectionFromApplication(mainApplication, connection);

    connection.setCarrier(carrier.toLatin1().data());

    connection = manager->getKnowledgeBase()->addConnectionToApplication(mainApplication, connection);
    emit sigHandler->modified();
}

void Arrow::updateConnectionFrom(QString from)
{
    if(!editingMode){
        return;
    }
    Application* mainApplication = nullptr;
    mainApplication = manager->getKnowledgeBase()->getApplication();
    Connection updatedCon = connection;
    updatedCon.setFrom(from.toLatin1().data());

    if(manager->getKnowledgeBase()->updateConnectionOfApplication(mainApplication, connection,updatedCon)){
        connection = updatedCon;
    }

//    connection.setFrom(from.toLatin1().data());

//    connection = manager->getKnowledgeBase()->addConnectionToApplication(mainApplication, connection);
    emit sigHandler->modified();
}

void Arrow::updateConnectionTo(QString to)
{
    if(!editingMode){
        return;
    }
    Application* mainApplication = nullptr;
    mainApplication = manager->getKnowledgeBase()->getApplication();
    Connection updatedCon = connection;
    updatedCon.setTo(to.toLatin1().data());

    if(manager->getKnowledgeBase()->updateConnectionOfApplication(mainApplication, connection,updatedCon)){
        connection = updatedCon;
    }

//    connection.setTo(to.toLatin1().data());
//    connection = manager->getKnowledgeBase()->addConnectionToApplication(mainApplication, connection);
    emit sigHandler->modified();
}

void Arrow::updateGraphicModel()
{
    GyPoint startPoint;
    GyPoint endPoint;
    GyPoint labelPoint;

    if(!textLbl.hasBeenMoved()){
        labelPoint.x = -1;
        labelPoint.y = -1;
    }else{
        QPointF p = textLbl.pos();
        labelPoint.x = p.x();
        labelPoint.y = p.y();
    }
    startPoint.x = (myStartItem->pos()).x();
    startPoint.y = (myStartItem->pos()).y();
    endPoint.x = (myEndItem->pos()).x();
    endPoint.y = (myEndItem->pos()).y();

    Application* mainApplication = nullptr;
    mainApplication = manager->getKnowledgeBase()->getApplication();
    manager->getKnowledgeBase()->removeConnectionFromApplication(mainApplication, connection);

    model.points.clear();
    model.points.push_back(labelPoint);
    model.points.push_back(startPoint);

    foreach (LineHandle *handle, handleList) {
        GyPoint point;
        point.x = handle->handlePoint().x();
        point.y = handle->handlePoint().y();
        model.points.push_back(point);
    }

    model.points.push_back(endPoint);
    connection.setModel(&model);
    connection.setModelBase(model);

    connection = manager->getKnowledgeBase()->addConnectionToApplication(mainApplication, connection);

}

void Arrow::updateModel()
{
    if(!editingMode || nestedInApp){
        return;
    }

    updateGraphicModel();
    updatePosition();

    emit sigHandler->modified();
    emit signalHandler()->moved();
}

void Arrow::setConnectionSelected(bool selected)
{
    externalSelection = true;
    setSelected(selected);
}

QPointF Arrow::connectionPoint()
{
    QPointF startIngPoint;

    return startIngPoint;
}

QString Arrow::getFrom()
{
    return QString("%1").arg(connection.from());
}

QString Arrow::getTo()
{
    return QString("%1").arg(connection.to());
}

void Arrow::setConnected(bool connected)
{
    this->connected = connected;
    update();
}

int Arrow::getId()
{
    return id;
}

QRectF Arrow::boundingRect() const
{
    qreal extra = (/*pen().width()*/2 + 30) / 2.0;

    QRectF bRect =  QRectF(boundingPolyline.boundingRect())
            .normalized()
            .adjusted(-extra, -extra, extra, extra);

    return bRect.united(textLbl.boundingRect());
}

QPainterPath Arrow::shape() const
{
    QPainterPath path;// = QGraphicsItem::shape();
    QPainterPathStroker pathStroke;
    pathStroke.setWidth(8);
    for(int i=0;i<boundingPolyline.count();i++){
        QPointF p = boundingPolyline.at(i);
        if(i==0){
            path.moveTo(p);
            continue;
        }
        path.lineTo(p.x(),p.y());
    }

    //path.addPolygon(arrowHead);
    QPainterPath ret = pathStroke.createStroke(path);
    return ret;
}

void Arrow::updatePosition()
{

    if(boundingPolyline.isEmpty()){
        boundingPolyline.prepend(mapFromItem(myStartItem,myStartItem->connectionPoint()));
        boundingPolyline.append( mapFromItem(myEndItem,myEndItem->connectionPoint()));
    }
    update();
}

void Arrow::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
          QWidget *)
{
    qreal arrowSize = 10;

    /************** Polyline *********************/
    startP = mapFromItem(myStartItem,myStartItem->connectionPoint());
    endP = mapFromItem(myEndItem,myEndItem->connectionPoint());
    polyline.clear();

    polyline.prepend(startP);
    foreach (LineHandle *handle, handleList) {
        QPointF handlePoint = handle->handlePoint();
        polyline.append(handlePoint);
        if(isSelected() && !isNestedInApp()){
            handle->show();
        }else{
            handle->hide();
        }

    }
    polyline.append(endP);
    boundingPolyline = polyline;
    /************** Polyline *********************/


    QLineF lastLine(polyline.at(polyline.count()-2),
                      polyline.last());
    QPointF p1 = lastLine.p1();
    QPointF p2 = lastLine.p2();

    QLineF line = QLineF(p2,p1);

    double angle = ::acos(line.dx() / line.length());
    if (line.dy() >= 0){
        angle = (Pi * 2) - angle;
    }

    QPointF arrowP1 = line.p1() + QPointF(sin(angle + Pi / 3) * arrowSize,
                                    cos(angle + Pi / 3) * arrowSize);
    QPointF arrowP2 = line.p1() + QPointF(sin(angle + Pi - Pi / 3) * arrowSize,
                                    cos(angle + Pi - Pi / 3) * arrowSize);

    arrowHead.clear();
    arrowHead << line.p1() << arrowP1 << arrowP2;
    if (isSelected()) {
        painter->setPen(QPen(Qt::blue, 1, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin));
    }else{
        if(connected){
            painter->setPen(QPen(Qt::green, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        }else{
            painter->setPen(QPen(Qt::red, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        }


    }
    if(connected){
        painter->setBrush(Qt::green);
    }else{
        painter->setBrush(Qt::red);
    }
    painter->drawPolyline(polyline);
    painter->drawPolygon(arrowHead);


    if(!textLbl.hasBeenMoved()){
        QPointF centerP = QPointF((p1.x() + p2.x())/2 - textWidth/2,((p1.y() + p2.y())/2) - 5);
        textLbl.setPos(centerP);


    }



}

void Arrow::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    pressed = true;
    if(!nestedInApp){
        setFlag(ItemIsMovable,true);
    }
    BuilderItem::mousePressEvent(event);

}


void Arrow::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{

    pressed = false;
    if(!nestedInApp){
        setFlag(ItemIsMovable,false);
    }
    BuilderItem::mouseReleaseEvent(event);
}
void Arrow::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{

    moved = true;
    if(!nestedInApp){
        setFlag(ItemIsMovable,false);
    }
    BuilderItem::mouseMoveEvent(event);
}



void Arrow::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if(!nestedInApp){
        QPointF clickPos = event->pos();
        addHandle(clickPos);

        updatePosition();
    }

    QGraphicsItem::mouseDoubleClickEvent(event);
}

void Arrow::addHandle(QPointF clickPos)
{
    auto* handle = new LineHandle(clickPos,this);
    handle->setSelected(true);
    if(handleList.isEmpty()){
        handleList.append(handle);
    }else{
        bool inserted = false;
        for(int i=0;i<handleList.count();i++){
            LineHandle *hndl = handleList.at(i);
            if(hndl->x() > clickPos.x()){
                handleList.insert(i,handle);
                inserted = true;
                break;
            }
        }
        if(!inserted){
            handleList.append(handle);
        }
    }
    updateModel();
}

void Arrow::removeHandle(LineHandle *handle)
{
    handleList.removeOne(handle);
    updateModel();
}
QList <LineHandle*> Arrow::handles()
{
    return handleList;
}

QVariant Arrow::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemPositionChange) {
        if(pressed){
            setFlag(ItemIsMovable,false);
        }
    }
    if (change == QGraphicsItem::ItemSelectedHasChanged) {
        if(!externalSelection){
            emit sigHandler->connectctionSelected(this);
        }
        externalSelection = false;
    }

    return value;
}

int Arrow::getHandleIndex(LineHandle *handle)
{
    for(int i=0;i<handleList.count();i++) {
        LineHandle *h = handleList.at(i);
        if(h == handle){
            return i;
        }
    }

    return -1;
}

LineHandle *Arrow::getLineHandle(int index)
{
    if(index >= 0 && index <= handleList.count() - 1){
        return handleList.at(index);
    }
    return nullptr;
}

/******************************************************************/

LineHandle::LineHandle(QPointF center, Arrow *parent) : QGraphicsRectItem(parent)
{
    this->parent = parent;
    setFlag(ItemIsSelectable,true);
    setFlag(ItemIsMovable,true);
    setFlag(ItemSendsGeometryChanges,true);

    rectSize = 8;
    setRect( - rectSize/2, -rectSize/2, rectSize,rectSize);
    setPos(center);
    setPen(QPen(QColor(Qt::black),1));
    setBrush(QBrush(QColor(Qt::red)));


    offset = QPointF(0,0);
    pressed = false;
    ctrlPressed = false;
    this->center = center;
    qDebug() << "CENTER CREATED IN " << center;
}

LineHandle::~LineHandle() = default;

QPointF LineHandle::computeTopLeftGridPoint(const QPointF &pointP){
    int gridSize = 16;
    qreal xV = gridSize/2 + floor(pointP.x()/gridSize)*gridSize;
    qreal yV = gridSize/2 + floor(pointP.y()/gridSize)*gridSize;
    return {xV, yV};
}


QPointF LineHandle::handlePoint()
{
    return pos();
}

void LineHandle::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    pressed = true;
    QPainterPath newPath;

    if(event->modifiers() == Qt::ControlModifier){
        ctrlPressed = true;
    }else{
        ctrlPressed = false;
    }

    newPath.addRect(pos().x() + rect().x() - 2,
                    pos().y() + rect().y() - 2 ,
                    rect().width() + 4,
                    rect().height()+4);

    scene()->setSelectionArea(newPath);
    parent->update();
    QGraphicsRectItem::mousePressEvent(event);
}

void LineHandle::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(pressed){

        if(event->modifiers() == Qt::ControlModifier){
            ctrlPressed = true;
        }else{
            ctrlPressed = false;
        }

        parent->updatePosition();
    }

    QGraphicsRectItem::mouseMoveEvent(event);
}

void LineHandle::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    pressed = false;
    ctrlPressed = false;
    parent->setSelected(true);
    parent->updateModel();
    QGraphicsRectItem::mouseReleaseEvent(event);
}

QVariant LineHandle::itemChange(GraphicsItemChange change, const QVariant &value)
{
    QVariant val = value;
    if (change == QGraphicsItem::ItemPositionChange && pressed) {
        if(parent->snap){
            QPointF newPos = parent->mapToScene(value.toPointF());
            QPointF closestPoint = computeTopLeftGridPoint(newPos);
            QPointF ret = parent->mapFromScene(closestPoint);

            val = ret;
        }
        if(ctrlPressed){
            int index = parent->getHandleIndex(this);
            if( index >= 0){
                QPointF current_point, base_point;
                float dist;
                bool modified = false;

                current_point = mapToItem(parent,val.toPointF());
                // adjusting to the previous point
                LineHandle *h = parent->getLineHandle(index-1);
                if(h){
                    base_point = mapToItem(parent,h->pos());
                }else{
                    base_point = mapToItem(parent,parent->mapFromItem(parent->startItem(),parent->startItem()->connectionPoint()));
                }

                dist =  current_point.y() -  base_point.y();

                if (::fabs(dist) <= AUTOSNIPE_MARGINE) {
                    modified = true;
                    current_point = QPointF(current_point.x(), current_point.y() -dist);
                }
                dist =  current_point.x() -  base_point.x();
                if (::fabs(dist) <= AUTOSNIPE_MARGINE) {
                    modified = true;
                    //moveBy(-dist, 0);
                    current_point = QPointF(current_point.x()-dist, current_point.y());
                }



                // adjusting to the next point
                h = parent->getLineHandle(index+1);
                if(h){
                    base_point = mapToItem(parent,h->pos());
                }else{
                    base_point = mapToItem(parent,parent->mapFromItem(parent->endItem(),parent->endItem()->connectionPoint()));
                }
                dist =  current_point.y() -  base_point.y();
                if (::fabs(dist) <= AUTOSNIPE_MARGINE) {
                    modified = true;
                    current_point = QPointF(current_point.x(), current_point.y() -dist);
                }
                dist =  current_point.x() -  base_point.x();
                if (::fabs(dist) <= AUTOSNIPE_MARGINE) {
                    modified = true;
                    //moveBy(-dist, 0);
                    current_point = QPointF(current_point.x()-dist, current_point.y());
                }




                if(modified){
                    val = mapFromItem(parent,current_point);
                }
            }
        }
    }

    return val;
}

void LineHandle::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
          QWidget *)
{
    QRectF myRect = rect();

    if(isSelected()){
        painter->setPen(QPen(QColor(Qt::black),1,Qt::DashLine));
        painter->setBrush(QBrush(QColor(Qt::blue)));
    }else{
        painter->setPen(QPen(QColor(Qt::black),1));
        painter->setBrush(QBrush(QColor(Qt::red)));
    }


    painter->drawRect(myRect);
}


/**********************************************************************/

Label::Label(QString label, QGraphicsItem *parent) : QGraphicsTextItem(label,parent)
{

    sigHandler = new ItemSignalHandler((QGraphicsItem*)this,ArrowLabelItemType,nullptr);
    comboWidget = new QGraphicsProxyWidget(this);
    auto* combo = new QComboBox();
    combo->setEditable(true);
    parentArrow = (Arrow*)parent;
    QObject::connect(combo,SIGNAL(activated(QString)),
                     sigHandler,SLOT(onConnectionComboChanged(QString)));

    combo->addItem("tcp");
    combo->addItem("udp");

    comboWidget->setWidget(combo);
    comboWidget->setVisible(false);

    setFlag(ItemIsMovable,!parentArrow->nestedInApp);
    setFlag(ItemIsSelectable,true);
    setFlag(ItemSendsGeometryChanges,!parentArrow->nestedInApp);


    pressed = false;
    moved = false;
    hasMoved = false;
    offset = QPointF(0,0);

    setText(label);

    parentArrow->update();

}

Label::~Label() = default;

void Label::setHasMoved(bool moved)
{
    hasMoved = moved;
}

bool Label::hasBeenMoved()
{
    return hasMoved;
}

QString Label::labelText()
{
    return text;
}
void Label::setText(QString label)
{
    this->text = label;
    setPlainText(label);
    if(((QComboBox*)comboWidget->widget())->findText(label) == -1){
        ((QComboBox*)comboWidget->widget())->addItem(label);
    }

}

void Label::currentComboTextChanged(QString text){
    setPlainText(text);
    comboWidget->setVisible(false);
    parentArrow->updateCarrier(text);
}
void Label::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event){
    if(!parentArrow->editingMode || parentArrow->nestedInApp){
        return;
    }
    if(comboWidget->isVisible()){
        comboWidget->setVisible(false);
    }else{
        comboWidget->setVisible(true);
    }
    QGraphicsTextItem::mouseDoubleClickEvent(event);
}

void Label::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    pressed = true;
    parentArrow->setSelected(true);
    QGraphicsItem::mousePressEvent(event);
}
void Label::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{

    if(moved){
        parentArrow->updateModel();
    }
    parentArrow->setSelected(true);
    pressed = false;
    moved = false;


    QGraphicsItem::mouseReleaseEvent(event);
}

void Label::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(pressed){
        moved = true;
        hasMoved = true;

    }
    QGraphicsItem::mouseMoveEvent(event);
}

QVariant Label::itemChange(GraphicsItemChange change, const QVariant &value)
{

    if (change == QGraphicsItem::ItemPositionChange) {
        if(parentArrow->snap && hasMoved){
            QPointF newPos = value.toPointF();
                QPointF closestPoint = computeTopLeftGridPoint(newPos);

            return closestPoint+=offset;
        }

    }
    if (change == QGraphicsItem::ItemSelectedHasChanged) {
        bool selected = isSelected();
        parentArrow->setConnectionSelected(selected);

        if(!selected){
            comboWidget->setVisible(false);
        }
    }

    return value;
}


QPointF Label::computeTopLeftGridPoint(const QPointF &pointP){
    int gridSize = 16;
    qreal xV = gridSize/2 + floor(pointP.x()/gridSize)*gridSize;
    qreal yV = gridSize/2 + floor(pointP.y()/gridSize)*gridSize;
    return {xV, yV};
}
