/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "destinationportitem.h"
#include <QGraphicsDropShadowEffect>
#include <QDebug>
#include <QCursor>
#include <QGraphicsScene>
#include <QLineEdit>


DestinationPortItem::DestinationPortItem(QString itemName, bool isInApp,
                                         bool editOnStart, Application *app, BuilderItem *parent) : BuilderItem(parent)
{
    itemType = DestinationPortItemType;
    this->itemName = itemName;
    errorState = false;
    portAvailable = false;
    sigHandler = new ItemSignalHandler((QGraphicsItem*)this,DestinationPortItemType,nullptr);
    pressed = false;
    moved = false;
    this->nestedInApp = isInApp;
    this->parent = parent;
    this->app = app;

    QFontMetrics fontMetric(font);
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
    int textWidth = fontMetric.horizontalAdvance(itemName);
#else
    int textWidth = fontMetric.width(itemName);
#endif

    prepareGeometryChange();
    mainRect = QRectF(-((2*PORT_TEXT_WIDTH) + textWidth)/2,-16,(2*PORT_TEXT_WIDTH) + textWidth,32);

    boundingR = QRectF(mainRect);

    setToolTip(itemName);

    setFlag(ItemIsMovable,!isInApp);
    setFlag(ItemIsSelectable,true);
    setFlag(ItemSendsGeometryChanges,!isInApp);

    if(!isInApp){
        auto* effect = new QGraphicsDropShadowEffect();
        effect->setColor(QColor(80,80,80,80));
        effect->setBlurRadius(5);
        setGraphicsEffect(effect);
    }

    allowInputs = true;
    allowOutputs= false;

    lineEditWidget = new QGraphicsProxyWidget(this);
    auto* lineEdit = new QLineEdit();
    QObject::connect(lineEdit,SIGNAL(editingFinished()),signalHandler(),SLOT(onEditingFinished()));
    QObject::connect(lineEdit,SIGNAL(returnPressed()),signalHandler(),SLOT(onEditingFinished()));
    lineEdit->setText(itemName);
    lineEditWidget->setWidget(lineEdit);
    if(editOnStart){
        lineEditWidget->setVisible(true);
    }else{
        lineEditWidget->setVisible(false);
    }
    QRectF geo = lineEditWidget->geometry();
    geo.setWidth(textWidth);
    lineEditWidget->setGeometry(geo);
    lineEditWidget->setPos(-textWidth/2,-lineEditWidget->geometry().height()/2);
}

DestinationPortItem::~DestinationPortItem()
{
    hide();
    removeArrows();
    delete sigHandler;
    //scene()->removeItem(lineEditWidget);
    delete lineEditWidget;
    //scene()->removeItem(this);
}


int DestinationPortItem::type() const
{
    return (int)(UserType + (int)itemType);
}
void DestinationPortItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    //painter->fillRect(boundingRect(),QBrush(Qt::red));

    QBrush availableBrush;
    if(!portAvailable){
        availableBrush = QBrush(QColor("#BF0303"));
    }else{
        availableBrush = QBrush(QColor("#00E400"));
    }
    if(!nestedInApp){
        painter->setPen(QPen(availableBrush,BORDERWIDTH));
    }else{
        painter->setPen(QPen(availableBrush,BORDERWIDTH,Qt::DashLine));
    }
    QPainterPath path;
    path.moveTo(mainRect.x(),mainRect.y());
    path.lineTo(mainRect.x() + mainRect.width(), mainRect.y());
    path.lineTo(mainRect.x() + mainRect.width(),mainRect.y() + mainRect.height());
    path.lineTo(mainRect.x(),mainRect.y() + mainRect.height());
    path.lineTo(mainRect.x() +10 ,mainRect.y() + mainRect.height()/2);
    path.lineTo(mainRect.x(),mainRect.y());
    if (isSelected()) {
        painter->setBrush(QBrush(QColor(220,220,220)));
    }else{
        painter->setBrush(QBrush(QColor(245,245,245)));
    }
    painter->drawPath(path);

    //TEXT
    if(!lineEditWidget->isVisible()){
        painter->setPen(QPen(QBrush(QColor(Qt::black)),1));
        painter->drawText(mainRect,Qt::AlignCenter,itemName);
    }else{
        if(!isSelected()){
            painter->setPen(QPen(QBrush(QColor(Qt::black)),1));
            painter->drawText(mainRect,Qt::AlignCenter,itemName);
        }
    }

}

bool DestinationPortItem::isErrorState()
{
    return errorState;
}


void DestinationPortItem::editingFinished()
{
    QString text = ((QLineEdit*)lineEditWidget->widget())->text();

    for (int i=0;i<scene()->items().count();i++){
        QGraphicsItem *it = scene()->items().at(i);
        if((it->type() == QGraphicsItem::UserType + DestinationPortItemType || it->type() == QGraphicsItem::UserType + SourcePortItemType) && (it != this)){
            if(((DestinationPortItem*)it)->getItemName() == text){
                ((QLineEdit*)lineEditWidget->widget())->setStyleSheet("QLineEdit { background-color: red;}");
                ((QLineEdit*)lineEditWidget->widget())->setToolTip(tr("Duplicate Entry"));
                allowOutputs = false;
                errorState = true;
                return;
            }
        }
    }
    errorState = false;
    allowOutputs = true;

    ((QLineEdit*)lineEditWidget->widget())->setStyleSheet("QLineEdit { background-color: white;}");
    ((QLineEdit*)lineEditWidget->widget())->setToolTip("");

    this->itemName = text;
    lineEditWidget->setVisible(false);

    QFontMetrics fontMetric(font);
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
    int textWidth = fontMetric.horizontalAdvance(itemName);
#else
    int textWidth = fontMetric.width(itemName);
#endif

    prepareGeometryChange();
    mainRect = QRectF(-((2*PORT_TEXT_WIDTH) + textWidth)/2,-15,(2*PORT_TEXT_WIDTH) + textWidth,30);
    boundingR = QRectF(mainRect);
    setToolTip(itemName);

    QRectF geo = lineEditWidget->geometry();
    geo.setWidth(textWidth);
    lineEditWidget->setGeometry(geo);
    lineEditWidget->setPos(-textWidth/2,-lineEditWidget->geometry().height()/2);
    emit signalHandler()->modified();

    updateConnections();
    updateConnectionsTo(this->itemName);
    update();
}


void DestinationPortItem::updateConnectionsTo(QString to){
    foreach (Arrow *arrow, arrows) {
        if(arrow){
            arrow->updateConnectionTo(to);
        }
    }
}



void DestinationPortItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    lineEditWidget->setVisible(true);
    update();
    //QGraphicsItem::mouseDoubleClickEvent(event);
}

QRectF DestinationPortItem::boundingRect() const
{
    QRectF br = QRectF(boundingR.x() - BORDERWIDTH, boundingR.y() - BORDERWIDTH , boundingR.width() + (2*BORDERWIDTH), boundingR.height() + (2*BORDERWIDTH));
    return br;
}




void DestinationPortItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    moved = true;
//    if(pressed){
//        ((DestinationPortItem*)this)->updateConnections();
//    }
    QGraphicsItem::mouseMoveEvent(event);
}

void DestinationPortItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    pressed = true;
    //setZValue(zValue() + 10);
    QGraphicsItem::mousePressEvent(event);
}

QPointF DestinationPortItem::connectionPoint()
{
    QPointF startIngPoint;
    startIngPoint = QPointF(mainRect.x() +10 ,mainRect.y() + mainRect.height()/2);
    return startIngPoint;
}

void DestinationPortItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(moved && !nestedInApp){
        emit sigHandler->modified();
        foreach (Arrow *arrow, arrows) {
            arrow->updateModel();
        }
    }
    if(!moved && event->modifiers() == Qt::NoModifier && event->button() == Qt::LeftButton){
        signalHandler()->newConnectionAdded(connectionPoint(),this);
    }
    pressed = false;
    moved = false;
    //setZValue(zValue() - 10);

    QGraphicsItem::mouseReleaseEvent(event);
}



QVariant DestinationPortItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemPositionChange) {
        foreach (Arrow *arrow, arrows) {
            arrow->updatePosition();
        }
        if(snap && !nestedInApp){
            QPointF newPos = value.toPointF();
            QPointF closestPoint = computeTopLeftGridPoint(newPos);
            return closestPoint+=offset;
        }
    }

    return value;
}

void DestinationPortItem::setAvailable(bool available)
{
    portAvailable = available;
    update();
}
