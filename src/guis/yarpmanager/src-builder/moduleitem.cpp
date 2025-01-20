/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "moduleitem.h"
#include <QGraphicsDropShadowEffect>
#include <QDebug>
#include <QCursor>
#include <QGraphicsScene>
#include <cmath>

#define TRIANGLEH   (double)((double)PORT_LINE_WIDTH * (double)sqrt(3.0) ) / 2.0


ModuleItem::ModuleItem(Module *module, int moduleId, bool isInApp, bool editingMode, Manager *manager, BuilderItem * parent) : BuilderItem(parent)
{
    sigHandler = new ItemSignalHandler();
    this->itemType = ModuleItemType;
    this->module = module;
    this->moduleId = moduleId;
    this->nestedInApp = isInApp;
    this->editingMode = editingMode;
    this->manager = manager;
    running = false;
    externalSelection = false;
    startingPoint = QPointF(10,10);
    itemName = QString("%1").arg(module->getName());

//    for(int i=0; i<module->inputCount();i++){

//        inputPorts << QString("%1").arg(module->getInputAt(i).getName());
//    }
//    for(int i=0; i<module->outputCount();i++){
//        OutputData out = module->getOutputAt(i);
//        outputPorts << QString("%1").arg(out.getName());
//    }

//    if(module->getModelBase().points.size()>0){
//        startingPoint = QPointF(module->getModelBase().points[0].x,module->getModelBase().points[0].y);
//    }



    init();


}

int ModuleItem::getId()
{
    return moduleId;
}


void ModuleItem::setRunning(bool running)
{
    this->running = running;
    update();
}

void ModuleItem::init()
{

    setFlag(ItemIsMovable,!nestedInApp);
    setFlag(ItemIsSelectable,true);
    setFlag(ItemSendsGeometryChanges,!nestedInApp);

    pressed = false;
    moved = false;

    setToolTip(itemName);


    QFontMetrics fontMetric(font);
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
    int textWidth = fontMetric.horizontalAdvance(itemName);
#else
    int textWidth = fontMetric.width(itemName);
#endif
    int mod = textWidth % 16;
    textWidth+=mod;

    prepareGeometryChange();
    mainRect = QRectF(-((2*PORT_TEXT_WIDTH) + textWidth)/2,
                     -PORT_LINE_WIDTH,
                     ((2*PORT_TEXT_WIDTH) + textWidth),
                     2*PORT_LINE_WIDTH);

    boundingR = QRectF(mainRect);

    if(module->inputCount() > 0){

        for(int i=0;i<module->inputCount();i++){
            iPorts.append(new PortItem(&module->getInputAt(i),this));
        }
        prepareGeometryChange();
        boundingR.setX((boundingR.x() - (PORT_LINE_WIDTH + TRIANGLEH / 2.0) ));
//        boundingR.setWidth(boundingR.width() +  PORT_LINE_WIDTH + TRIANGLEH );



        if(module->inputCount() > 1){
            int newH = (module->inputCount() * 2 * PORT_LINE_WIDTH) ;
            if(mainRect.height() <= newH){

                mainRect.setY(-newH/2);
                mainRect.setHeight(newH);
                boundingR.setY(mainRect.y());
                boundingR.setHeight(mainRect.height());
            }
        }

    }

    if(module->outputCount() > 0){
        for(int i=0;i<module->outputCount();i++){
            oPorts.append(new PortItem(&module->getOutputAt(i),this));
        }
        prepareGeometryChange();
        boundingR.setWidth(boundingR.width() +  (PORT_LINE_WIDTH +TRIANGLEH/2.0) );

        if(module->outputCount() > 1){
            int newH = (module->outputCount() * 2 * PORT_LINE_WIDTH) ;
            if(mainRect.height() <= newH){

                mainRect.setY(-newH/2);
                mainRect.setHeight(newH);
                boundingR.setY(mainRect.y());
                boundingR.setHeight(mainRect.height());
            }
        }
    }

    qDebug() << mainRect;


    if(!nestedInApp){
        auto* effect = new QGraphicsDropShadowEffect();
        effect->setColor(QColor(80,80,80,80));
        effect->setBlurRadius(5);
        setGraphicsEffect(effect);
    }

    allowInputs = true;
    allowOutputs = true;
}

ModuleItem::~ModuleItem()
{
    hide();
    delete sigHandler;
    foreach (PortItem *port, iPorts) {
        port->removeArrows();
        //scene()->removeItem(port);
        delete port;
    }
    foreach (PortItem *port, oPorts) {
        port->removeArrows();
        //scene()->removeItem(port);
        delete port;
    }

    //scene()->removeItem(this);

    if(manager && editingMode){
        Application* mainApplication = manager->getKnowledgeBase()->getApplication();
        manager->getKnowledgeBase()->removeIModuleFromApplication(mainApplication,module->getLabel());
        module = nullptr;
    }

}



int ModuleItem::type() const
{
    return (int)(QGraphicsItem::UserType + (int)itemType);
}
void ModuleItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    qreal partialH ;
    //painter->fillRect(boundingRect(),QBrush(Qt::red));

    //Input Ports


    if(module->inputCount() <= 1){
        partialH = (qreal)mainRect.height()/(qreal)((qreal)module->inputCount() + 1.0);
        for(int i=0; i < module->inputCount(); i++){
            painter->setPen(QPen(QBrush(QColor(Qt::black)),BORDERWIDTH));
            painter->drawLine(QPointF(mainRect.x() - PORT_LINE_WIDTH, mainRect.y() + ((i+1) * partialH )),
                              QPointF(mainRect.x() , mainRect.y() + ((i+1) * partialH )));
            PortItem *it = iPorts.at(i);
            it->setPos(mainRect.x() - PORT_LINE_WIDTH,mainRect.y() + ((i+1) * partialH ));
        }
    }else{
        partialH = PORT_LINE_WIDTH;
        for(int i=0; i < module->inputCount(); i++){
            painter->setPen(QPen(QBrush(QColor(Qt::black)),BORDERWIDTH));
            painter->drawLine(QPointF(mainRect.x() - PORT_LINE_WIDTH, mainRect.y() + (partialH )),
                              QPointF(mainRect.x() , mainRect.y() + (partialH )));
            PortItem *it = iPorts.at(i);
            it->setPos(mainRect.x() - PORT_LINE_WIDTH,mainRect.y() + (partialH ));
            partialH += 2 * PORT_LINE_WIDTH;
        }
    }

    //Output Ports ?????????
    if(module->outputCount() <= 1){
        partialH = (qreal)mainRect.height()/(qreal)((qreal)module->outputCount() + 1.0);
        for(int i=0; i < module->outputCount(); i++){
            PortItem *it = oPorts.at(i);
            painter->setPen(QPen(QBrush(QColor(Qt::black)),BORDERWIDTH));

            if(it->outData->getPortType() == SERVICE_PORT){
                painter->drawLine(QPointF(mainRect.x() + mainRect.width() , mainRect.y() + ((i+1) * partialH )),
                                  QPointF(mainRect.x() + mainRect.width() + PORT_LINE_WIDTH/2 - BORDERWIDTH/2 , mainRect.y() + ((i+1) * partialH )));
            }else{
                painter->drawLine(QPointF(mainRect.x() + mainRect.width() , mainRect.y() + ((i+1) * partialH )),
                                  QPointF(mainRect.x() + mainRect.width() + PORT_LINE_WIDTH , mainRect.y() + ((i+1) * partialH )));
            }

            it->setPos(mainRect.x() + mainRect.width() + PORT_LINE_WIDTH,mainRect.y() + ((i+1) * partialH ));
        }
    }else{
        partialH = PORT_LINE_WIDTH;
        for(int i=0; i < module->outputCount(); i++){
            PortItem *it = oPorts.at(i);

            painter->setPen(QPen(QBrush(QColor(Qt::black)),BORDERWIDTH));

            if(it->outData->getPortType() == SERVICE_PORT){
                painter->drawLine(QPointF(mainRect.x() + mainRect.width() , mainRect.y() + (partialH )),
                                  QPointF(mainRect.x() + mainRect.width() + PORT_LINE_WIDTH/2 - BORDERWIDTH/2 , mainRect.y() + (partialH )));
            }else{
                painter->drawLine(QPointF(mainRect.x() + mainRect.width() , mainRect.y() + (partialH )),
                                  QPointF(mainRect.x() + mainRect.width() + PORT_LINE_WIDTH , mainRect.y() + (partialH )));
            }

            it->setPos(mainRect.x() + mainRect.width() + PORT_LINE_WIDTH,mainRect.y() + (partialH ));
            partialH += 2 * PORT_LINE_WIDTH;
        }
    }




    if(!nestedInApp){
        if(!running){
            painter->setPen(QPen(QBrush(QColor("#BF0303")),BORDERWIDTH ));
        }else{
            painter->setPen(QPen(QBrush(QColor("#00E400")),BORDERWIDTH ));
        }
    }else{
        if(!running){
            painter->setPen(QPen(QBrush(QColor("#BF0303")),BORDERWIDTH, Qt::DashLine ));
        }else{
            painter->setPen(QPen(QBrush(QColor("#00E400")),BORDERWIDTH , Qt::DashLine));
        }
    }
    QPainterPath path;
    path.addRoundedRect( mainRect, 5.0, 5.0 );
    if (isSelected()) {\
        painter->setBrush(QBrush(QColor(220,220,220)));
    }else{
        painter->setBrush(QBrush(QColor(Qt::white)));
    }


    painter->drawPath(path);



    //TEXT
    painter->setPen(QPen(QBrush(QColor(Qt::black)),1));
    painter->drawText(mainRect,Qt::AlignCenter,itemName);

    //painter->fillRect(-4,-4,8,8,QBrush(Qt::yellow));


}


QRectF ModuleItem::boundingRect() const
{
    QRectF br = QRectF(boundingR.x() - BORDERWIDTH,
                       boundingR.y() - BORDERWIDTH ,
                       boundingR.width() + (2*BORDERWIDTH),
                       boundingR.height() + (2*BORDERWIDTH));
    //qDebug() << br;
    return br;
}




void ModuleItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    moved = true;
//    if(pressed){
//        foreach (PortItem *port, iPorts) {
//            port->updateConnections();
//        }
//        foreach (PortItem *port, oPorts) {
//            port->updateConnections();
//        }
//    }
    QGraphicsItem::mouseMoveEvent(event);
}

void ModuleItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    pressed = true;
    setZValue(zValue() + 10);
    //sigHandler->moduleSelected(this);
    if(nestedInApp && isSelected()){
        parentItem()->setSelected(true);
    }
    QGraphicsItem::mousePressEvent(event);
}

QPointF ModuleItem::connectionPoint()
{
    QPointF startIngPoint;

    return startIngPoint;
}

void ModuleItem::updateGraphicModel()
{
    GraphicModel modBase;
    GyPoint p;
    p.x = pos().x();
    p.y = pos().y();
    modBase.points.push_back(p);
    module->setModelBase(modBase);
}

void ModuleItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{

    if(moved && editingMode && !nestedInApp){
        //updateGraphicModel();
        emit signalHandler()->modified();
        emit signalHandler()->moved();
    }
    pressed = false;
    moved = false;
    setZValue(zValue() - 10);

    if(nestedInApp && isSelected()){
        parentItem()->setSelected(true);
    }

    QGraphicsItem::mouseReleaseEvent(event);
}

void ModuleItem::setModuleSelected(bool selected)
{
    externalSelection = true;
    setSelected(selected);
}

QVariant ModuleItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemPositionChange) {
        foreach (PortItem *port, iPorts) {
            port->updateConnections();
        }
        foreach (PortItem *port, oPorts) {
            port->updateConnections();
        }
        if(snap  && !nestedInApp){
            QPointF newPos = value.toPointF();
            QPointF closestPoint = computeTopLeftGridPoint(newPos);
            return closestPoint+=offset;
        }
    }
    if (change == QGraphicsItem::ItemSelectedHasChanged) {
        //bool selected = value.toBool();
        if(!externalSelection){
            emit sigHandler->moduleSelected(this);
        }
        externalSelection = false;
    }

    return value;
}

void ModuleItem::portPressed(PortItem *port,QGraphicsSceneMouseEvent *e)
{
    pressed = true;
}

Module *ModuleItem::getInnerModule()
{
    return module;
}

void ModuleItem::portReleased(PortItem *port,QGraphicsSceneMouseEvent *e)
{
//    if(!moved){
//        sigHandler->newConnection(mapFromItem(port,port->connectionPoint()),this);
//    }
//    pressed = false;
//    moved = false;
    qDebug() << "MODULE RELEASED";
}

void ModuleItem::portMoved(PortItem *port,QGraphicsSceneMouseEvent *e)
{
//    if(!moved){
//        sigHandler->newConnection(mapFromItem(port,port->connectionPoint()),this);
//    }
//    pressed = false;
//    moved = false;
}



/********************************************************************************************************************/
/********************************************************************************************************************/
/********************************************************************************************************************/
/********************************************************************************************************************/
/********************************************************************************************************************/
/********************************************************************************************************************/

//PortItem::PortItem(QString portName, int type, BuilderItem *parent) : BuilderItem(parent)
//{
//    triangleH = (PORT_LINE_WIDTH/2)* sqrt(3.0);
//    portAvailable = unknown;

//    polygon << QPointF(-triangleH/2, - PORT_LINE_WIDTH/2) << QPointF(triangleH/2, 0) << QPointF(-triangleH/2, PORT_LINE_WIDTH/2);

//    setAcceptHoverEvents(true);
//    setFlag(ItemSendsGeometryChanges,true);
//    setFlag(ItemIsSelectable,true);

//    boundingR = QRectF(-triangleH/2, - PORT_LINE_WIDTH/2,triangleH,PORT_LINE_WIDTH);


//    this->itemName = portName;
//    setToolTip(itemName);
//    this->parent = parent;
//    this->nestedInApp = parent->nestedInApp;
//    portType = type;

//    sigHandler = NULL;
//    pressed = false;
//    moved = false;
//    hovered =false;
//    if(type == INPUT_PORT){
//        allowInputs = true;
//        allowOutputs = false;
//    }
//    if(type == OUTPUT_PORT){
//        allowInputs = false;
//        allowOutputs = true;
//    }

//    itemType = ModulePortItemType;
//}
PortItem::PortItem(InputData *node, BuilderItem *parent) : BuilderItem(parent)
{
    triangleH = (PORT_LINE_WIDTH/2)* sqrt(3.0);
    outData = nullptr;
    inData = node;
    portAvailable = unknown;

    setAcceptHoverEvents(true);
    setFlag(ItemSendsGeometryChanges,true);
    setFlag(ItemIsSelectable,true);

    switch (node->getPortType()) {
    case STREAM_PORT:
        polygon << QPointF(-triangleH/2, -PORT_LINE_WIDTH/2) << QPointF(triangleH/2, 0) << QPointF(-triangleH/2, PORT_LINE_WIDTH/2);
        boundingR = QRectF(-triangleH/2, -PORT_LINE_WIDTH/2,triangleH,PORT_LINE_WIDTH);
        break;
    case EVENT_PORT:
        polygon << QPointF(-triangleH/2, -PORT_LINE_WIDTH/2) << QPointF(0,0) << QPointF(-triangleH/2, PORT_LINE_WIDTH/2) <<
                   QPointF(triangleH/2, PORT_LINE_WIDTH/2) << QPointF(triangleH/2, -PORT_LINE_WIDTH/2);
        boundingR = QRectF(-triangleH/2, -PORT_LINE_WIDTH/2,triangleH,PORT_LINE_WIDTH);
        break;
    case SERVICE_PORT:
        boundingR = QRectF(-triangleH/2, -PORT_LINE_WIDTH/2,triangleH,PORT_LINE_WIDTH);
        break;
    default:
        break;
    }

    this->itemName = node->getPort();
    setToolTip(itemName);
    this->parent = parent;
    this->nestedInApp = parent->nestedInApp;
    portType = INPUT_PORT;

    sigHandler = nullptr;
    pressed = false;
    moved = false;
    hovered =false;
    if(portType == INPUT_PORT){
        allowInputs = true;
        allowOutputs = false;
    }
    if(portType == OUTPUT_PORT){
        allowInputs = false;
        allowOutputs = true;
    }
    itemType = ModulePortItemType;
}

PortItem::PortItem(OutputData* node, BuilderItem *parent) : BuilderItem(parent)
{
    triangleH = (PORT_LINE_WIDTH/2)* sqrt(3.0);
    outData = node;
    inData = nullptr;
    portAvailable = unknown;
    setAcceptHoverEvents(true);
    setFlag(ItemSendsGeometryChanges,true);
    setFlag(ItemIsSelectable,true);

    switch (node->getPortType()) {
        case STREAM_PORT:
            polygon << QPointF(-triangleH/2, - PORT_LINE_WIDTH/2) << QPointF(triangleH/2, 0) << QPointF(-triangleH/2, PORT_LINE_WIDTH/2);
            boundingR = QRectF(-triangleH/2, - PORT_LINE_WIDTH/2,triangleH,PORT_LINE_WIDTH);
            break;
        case EVENT_PORT:
            polygon << QPointF(-triangleH/2, -PORT_LINE_WIDTH/2) << QPointF(-triangleH/2, PORT_LINE_WIDTH/2) <<
                       QPointF(0, PORT_LINE_WIDTH/2) << QPointF(triangleH/2, 0) << QPointF(0, -PORT_LINE_WIDTH/2);
            boundingR = QRectF(-triangleH/2, -PORT_LINE_WIDTH/2,triangleH,PORT_LINE_WIDTH);
            break;
        case SERVICE_PORT:
            boundingR = QRectF(-PORT_LINE_WIDTH/2, -PORT_LINE_WIDTH/2,PORT_LINE_WIDTH,PORT_LINE_WIDTH);
            break;
        default:
            break;
    }

    this->itemName = node->getPort();
    setToolTip(itemName);
    this->parent = parent;
    this->nestedInApp = parent->nestedInApp;
    portType = OUTPUT_PORT;

    sigHandler = nullptr;
    pressed = false;
    moved = false;
    hovered =false;
    if(portType == INPUT_PORT){
        allowInputs = true;
        allowOutputs = false;
    }
    if(portType == OUTPUT_PORT){
        allowInputs = false;
        allowOutputs = true;
    }
    itemType = ModulePortItemType;
}

void PortItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setPen(QPen(QBrush(QColor(Qt::black)),BORDERWIDTH/2));

    switch (portAvailable) {
    case unavailable:
        if(!hovered){
            painter->setBrush(QBrush(QColor("#F74D4D")));
        }else{
            painter->setBrush(QBrush(QColor("#BF0303")));
        }
        break;
    case availbale:
        if(!hovered){
            painter->setBrush(QBrush(QColor("#1CE61C")));
        }else{
            painter->setBrush(QBrush(QColor("#008C00")));
        }
        break;
    case unknown:
        if(!hovered){
            painter->setBrush(QBrush(QColor("#8C8c8c")));
        }else{
            painter->setBrush(QBrush(QColor("#4B4B4B")));
        }
        break;
    default:
        break;
    }

    if(portType == INPUT_PORT && inData->getPortType() == SERVICE_PORT){
        painter->drawEllipse(QPoint(0,0),(int)PORT_LINE_WIDTH/2,(int)PORT_LINE_WIDTH/2);
    } if(portType == OUTPUT_PORT && outData->getPortType() == SERVICE_PORT){
        painter->drawArc(boundingR,-90 * 16, -180 * 16);
    }else{
        painter->drawPolygon(polygon);
    }
}

void PortItem::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
    hovered = true;
    update();

}

void PortItem::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
    hovered = false;
    update();
}

int PortItem::type() const
{
    return UserType + (int)itemType;
}

void PortItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "PORT PRESSED";
    //parent->portPressed(this,event);
    pressed = true;
    QGraphicsItem::mousePressEvent(event);
}

void PortItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "PORT RELEASED";
    //parent->portReleased(this,event);
    if(!moved && event->modifiers() == Qt::NoModifier && event->button() == Qt::LeftButton){
        if(portType == OUTPUT_PORT){
            parent->signalHandler()->newConnectionRequested(connectionPoint(),this);
        }else{
            parent->signalHandler()->newConnectionAdded(connectionPoint(),this);
        }
    }
    pressed = false;
    moved = false;
    //QGraphicsItem::mouseReleaseEvent(event);
}

void PortItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "PORT MOVED";
    //parent->portReleased(this,event);
    moved = true;
    QGraphicsItem::mouseMoveEvent(event);
}

QPointF PortItem::connectionPoint()
{
    switch (portType) {
    case INPUT_PORT:

        switch (inData->getPortType()) {
        case SERVICE_PORT:
            return {-PORT_LINE_WIDTH/2,0};
            break;
        case EVENT_PORT:
            return {0,0};
            break;
        default:
            return {-triangleH/2, - 0};
        }

    case OUTPUT_PORT:
        switch (outData->getPortType()) {
        case SERVICE_PORT:
            return {-PORT_LINE_WIDTH/2,0};
            break;
        default:
            return {triangleH/2, - 0};
        }

    }
    return {0,0};
}

int PortItem::getPortType()
{
    return portType;
}


QRectF PortItem::boundingRect() const
{
    QRectF br = QRectF(boundingR.x() - BORDERWIDTH,
                       boundingR.y() - BORDERWIDTH ,
                       boundingR.width() + (2*BORDERWIDTH),
                       boundingR.height() + (2*BORDERWIDTH));
    return br;
}

InputData *PortItem::getInputData()
{
    return inData;
}

OutputData *PortItem::getOutputData()
{
    return outData;
}


QVariant PortItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
//    if (change == QGraphicsItem::ItemPositionChange) {
//        foreach (Arrow *arrow, arrows) {
//            arrow->updatePosition();
//        }
////        if(snap  && !isInApp){
////            QPointF newPos = value.toPointF();
////            QPointF closestPoint = computeTopLeftGridPoint(newPos);
////            return closestPoint+=offset;
////        }
//    }

    return value;
}

void PortItem::setAvailable(PortStatus available)
{
    portAvailable = available;
    update();
}
