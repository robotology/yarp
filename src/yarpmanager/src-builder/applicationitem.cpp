/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "applicationitem.h"
#include "moduleitem.h"
#include "arrow.h"
#include "sourceportitem.h"
#include "destinationportitem.h"
#include "builderscene.h"
#include <QGraphicsDropShadowEffect>
#include <QDebug>

using namespace std;

ApplicationItem::ApplicationItem(Application* application, Manager *manager,  QList <int> *usedIds,bool isInApp,
                                 bool editingMode,
                                 int *connectionsId,
                                 BuilderItem *parent) :
    BuilderItem(parent)
{
    sigHandler = new ItemSignalHandler();
    this->itemType = ApplicationItemType;
    this->itemName = QString("%1").arg(application->getName());
    this->application = application;
    this->editingMode = editingMode;
    this->mainAppManager = manager;
    this->connectionsId = connectionsId;
    index = 0;
    minx = miny = maxw = maxh = -1000;
    this->nestedInApp = isInApp;

    customFont.setPointSize(font.pointSize() + 5);
    customFont.setBold(true);
    QFontMetrics fontMetric(customFont);
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
    textWidth = fontMetric.horizontalAdvance(itemName);
#else
    textWidth = fontMetric.width(itemName);
#endif
    usedModulesId = usedIds;
}

ApplicationItem::~ApplicationItem()
{
    if(editingMode && mainAppManager){
        Application* mainApplication = mainAppManager->getKnowledgeBase()->getApplication();
        mainAppManager->getKnowledgeBase()->removeIApplicationFromApplication(mainApplication,application->getName());
        application = nullptr;
    }
}

Application* ApplicationItem::getInnerApplication()
{
    return application;
}



void ApplicationItem::init()
{
    setFlag(ItemIsMovable,!nestedInApp);
    setFlag(ItemIsSelectable,true);
    setFlag(ItemSendsGeometryChanges,!nestedInApp);

    pressed = false;
    moved = false;

    setToolTip(itemName);


    QFontMetrics fontMetric(font);
    //int textWidth = fontMetric.width(itemName);


    /*********************************************************/
    ModulePContainer modules = mainAppManager->getKnowledgeBase()->getModules(application);
    CnnContainer connections = mainAppManager->getKnowledgeBase()->getConnections(application);
    ApplicaitonPContainer applications = mainAppManager->getKnowledgeBase()->getApplications(application);
//    ExecutablePContainer exes = mainAppManager->getExecutables();
    yarp::manager::ExecutablePIterator moditr;
    /*************************************/

    ApplicationPIterator appItr;
    for(appItr=applications.begin(); appItr!=applications.end(); appItr++)
    {
        Application* application = (*appItr);
        auto* appItem = new ApplicationItem(application,mainAppManager,usedModulesId,true,editingMode,connectionsId,this);
        QObject::connect(appItem->signalHandler(),SIGNAL(moduleSelected(QGraphicsItem*)),
                         sigHandler,SLOT(onModuleSelected(QGraphicsItem*)));
        QObject::connect(appItem->signalHandler(),SIGNAL(connectctionSelected(QGraphicsItem*)),
                         sigHandler,SLOT(onConnectionSelected(QGraphicsItem*)));
        //QObject::connect(appItem->signalHandler(),SIGNAL(applicationSelected(QGraphicsItem*)),
        //                 sigHandler,SLOT(onApplicationSelected(QGraphicsItem*)));
        appItem->setZValue(zValue());
        appItem->init();

        if(application->getModelBase().points.size()>0){
            appItem->setPos(application->getModelBase().points[0].x,
                    application->getModelBase().points[0].y);
        } else {
            appItem->setPos(index%900+10,
                            (index/900)*100+10);
            index += 300;
        }
        //itemsList.append(appItem);
        updateSizes(appItem);

    }

    ModulePIterator itr;
    //int n = application->sucCount();
    //for(int i=0;i<n;i++){
    for(itr=modules.begin(); itr!=modules.end(); itr++){
        Module* module1 = (*itr); //dynamic_cast<Module*>(application->getLinkAt(i).to());

        if(!editingMode){
            ExecutablePContainer exes = mainAppManager->getExecutables();
            for(moditr=exes.begin(); moditr<exes.end(); moditr++){

                Module* module = (*moditr)->getModule();

                if(module == module1){
                    QString id = QString("%1").arg((*moditr)->getID());
                    usedModulesId->append(id.toInt());
                    addModule(module,id.toInt());
                    break;
                }
            }
        }else{
            addModule(module1,-1);
        }
    }



    index = (index/900)*100+50;
    CnnIterator citr;
    ModulePContainer allModules = mainAppManager->getKnowledgeBase()->getSelModules();
    //int id = 0;
    for(citr=connections.begin(); citr<connections.end(); citr++){
        Connection baseCon = *citr;
        //            if(baseCon.owner()->getLabel() != application->getLabel()){
        //                continue;
        //            }
        GraphicModel model = baseCon.getModelBase();
        InputData* input = nullptr;
        OutputData* output = nullptr;
        BuilderItem *source = nullptr;
        BuilderItem *dest = nullptr;
        QString inModulePrefix,outModulePrefix;
        findInputOutputData((*citr), /*!editingMode ?*/ allModules/* : modules*/, input, output);
        if(output){
            source = findModelFromOutput(output);
        }else{
            bool bExist = false;
            SourcePortItem *sourcePort = nullptr;
            for(int i=0;i<childItems().count() && !bExist;i++){
                if(childItems().at(i)->type() == (QGraphicsItem::UserType + SourcePortItemType)){
                    SourcePortItem *auxSourceport = (SourcePortItem*)childItems().at(i);
                    if(auxSourceport->getItemName() == baseCon.from()){
                        source = auxSourceport;
                        bExist = true;
                        break;
                    }

                }
                if(childItems().at(i)->type() == (QGraphicsItem::UserType + ApplicationItemType)){
                    // TODO
                }
            }
            if(!bExist){
                sourcePort = new SourcePortItem((*citr).from(),true,false,application,this);
                QObject::connect(sourcePort->signalHandler(),SIGNAL(requestNewConnection(QPointF,QGraphicsItem*)),((BuilderScene*)scene()),SLOT(onNewConnectionRequested(QPointF,QGraphicsItem*)));
                //itemsList.append(sourcePort);
                //scene->addItem(sourcePort);
                source = sourcePort;

                if(model.points.size() > 1){
                    source->setPos(model.points[1].x /*+ source->boundingRect().width()/2*/,
                            model.points[1].y/* + source->boundingRect().height()/2*/);
                }else{
                    source->setPos(10 + source->boundingRect().width()/2, index);
                }
                updateSizes(source);
            }


            index += 40;
        }
        if(input){
            dest = findModelFromInput(input);
        }else{
            bool bExist = false;
            DestinationPortItem *destPort = nullptr;
            for(int i=0;i<childItems().count() && !bExist;i++){
                if(childItems().at(i)->type() == (QGraphicsItem::UserType + DestinationPortItemType)){
                    DestinationPortItem *auxDestPort = (DestinationPortItem*)childItems().at(i);
                    if(auxDestPort->getItemName() == baseCon.to()){
                        dest = auxDestPort;
                        bExist = true;
                        break;
                    }
                }
                if(childItems().at(i)->type() == (QGraphicsItem::UserType + ApplicationItemType)){
                    // TODO
                }
            }
            if(!bExist){
                destPort = new DestinationPortItem((*citr).to(),true,false,application,this);
                QObject::connect(destPort->signalHandler(),SIGNAL(addNewConnection(QPointF,QGraphicsItem*)),((BuilderScene*)scene()),SLOT(onNewConnectionAdded(QPointF,QGraphicsItem*)));
                //itemsList.append(destPort);
                dest = destPort;

                //scene->addItem(destPort);
                size_t size = model.points.size();
                if(size > 2){
                    dest->setPos(model.points[size-1].x /*+ dest->boundingRect().width()/2*/,
                            model.points[size-1].y /*+ dest->boundingRect().height()/2*/);
                }else{
                    dest->setPos(400 + dest->boundingRect().width()/2, index);
                }
                updateSizes(dest);
            }


        }

        Arrow *arrow;
        // check for arbitrators
        string strCarrier = baseCon.carrier();
        if((strCarrier.find("recv.priority") != std::string::npos)){
            // TODO
        }else{
            if(source && dest){

                arrow = new Arrow(source, dest,*connectionsId,nullptr,true,editingMode,this);
                arrow->setConnection(baseCon);
                QObject::connect(arrow->signalHandler(),SIGNAL(connectctionSelected(QGraphicsItem*)),sigHandler,SLOT(onConnectionSelected(QGraphicsItem*)));
                arrow->setColor(QColor(Qt::red));
                source->addArrow(arrow);
                dest->addArrow(arrow);
                //scene->addItem(arrow);
                arrow->setZValue(zValue()+1);
                arrow->updatePosition();
                updateSizes(arrow);
                //itemsList.append(arrow);
            }
        }
        *connectionsId += 1;
        qDebug() << "*connectionsId " << *connectionsId;
    }



    /***********************************************************/




    if(!nestedInApp){
        auto* effect = new QGraphicsDropShadowEffect();
        effect->setColor(QColor(80,80,80,80));
        effect->setBlurRadius(5);
        setGraphicsEffect(effect);
    }

    allowInputs = true;
    allowOutputs = true;
    updateBoundingRect(childItems());

    update();


}


QRectF ApplicationItem::boundingRect() const
{
    QRectF br = QRectF(boundingR.x() - BORDERWIDTH,
                       boundingR.y() - BORDERWIDTH -30 ,
                       boundingR.width() + (2*BORDERWIDTH),
                       boundingR.height() + (2*BORDERWIDTH) + 30);
    //qDebug() << br;
    return br;
}


int ApplicationItem::type() const
{
    return (int)(QGraphicsItem::UserType + (int)itemType);
}

QPointF ApplicationItem::connectionPoint()
{
    QPointF startIngPoint;

    return startIngPoint;
}

void ApplicationItem::setModulesAction(QList<QAction *> act)
{
    modulesAction = act;
}

BuilderItem * ApplicationItem::addModule(Module *module,int moduleId)
{
    auto* it = new ModuleItem(module,moduleId,true,editingMode,mainAppManager,this);
    it->setActions(modulesAction);
    QObject::connect(it->signalHandler(),SIGNAL(moduleSelected(QGraphicsItem*)),sigHandler,SLOT(onModuleSelected(QGraphicsItem*)));
    QObject::connect(it->signalHandler(),SIGNAL(requestNewConnection(QPointF,QGraphicsItem*)),((BuilderScene*)scene()),SLOT(onNewConnectionRequested(QPointF,QGraphicsItem*)));
    QObject::connect(it->signalHandler(),SIGNAL(addNewConnection(QPointF,QGraphicsItem*)),((BuilderScene*)scene()),SLOT(onNewConnectionAdded(QPointF,QGraphicsItem*)));

    it->setZValue(zValue()+2);
    if(module->getModelBase().points.size()>0){
        it->setPos(module->getModelBase().points[0].x ,
                module->getModelBase().points[0].y );
    }else{
        it->setPos(index%900+10 + it->boundingRect().width()/2,
                   (index/900)*100+10 + it->boundingRect().height()/2);
        index += 300;
    }

    updateSizes(it);

    return it;
}

void ApplicationItem::updateSizes(QGraphicsItem *it,QGraphicsItem *parent)
{
    QRectF bRect = it->boundingRect();
    if(it->type() == QGraphicsItem::UserType + ConnectionItemType){

        foreach (QGraphicsItem *child, it->childItems()) {
            updateSizes(child,it);
        }
        return;
    }
    if(it->type() == QGraphicsItem::UserType + ArrowLabelItemType){
        auto* l = (Label*)it;
        if(!l->hasBeenMoved()){
            return;
        }
    }
    QPointF pos = it->pos();
    if(parent && (it->type() == QGraphicsItem::UserType + HandleItemType ||
            it->type() == QGraphicsItem::UserType + ArrowLabelItemType)){
        pos = mapFromItem(parent,pos);

    }
    QPointF p = mapFromItem(it,QPointF(bRect.x(),bRect.y() + bRect.height()/2));
    if(minx == -1000 || p.x()  < minx){
        //minx = pos.x() - bRect.width()/2;
        minx = p.x();
    }
    QPointF pp = mapFromItem(it,QPointF(bRect.x() + bRect.width()/2,bRect.y()));
    if(miny == -1000 || pp.y()  < miny){
        miny = pp.y();
    }
    QPointF p1 = mapFromItem(it,QPointF(bRect.x() + bRect.width() ,bRect.y() + bRect.height()/2));
    if(maxw == -1000 ||  p1.x() > maxw){
       // maxw = pos.x() + bRect.width() /2;

        maxw = p1.x();
    }
    QPointF pp1 = mapFromItem(it,QPointF(bRect.x() + bRect.width()/2, bRect.y() + bRect.height()));
    if(maxh == -1000 || pp1.y()  > maxh){
        maxh = pp1.y();
    }

}

void ApplicationItem::updateBoundingRect(QList<QGraphicsItem *> items)
{

    foreach (QGraphicsItem *it, items) {

        if(it->type() == QGraphicsItem::UserType + ArrowLabelItemType ||
                it->type() == QGraphicsItem::UserType + HandleItemType){
            continue;
        }

        if(minx < 25){
            qreal diff =  25 - minx;
            it->moveBy(diff,0);
            mainRect.setWidth(maxw + 25 + diff);
        }else{
            qreal diff = minx - 25;
            it->moveBy(-diff,0);
            mainRect.setWidth(maxw + 25 - diff);
        }

        if(miny < 25){
            qreal diff =  25 - miny;
            it->moveBy(0,diff);
            mainRect.setHeight(maxh + 25 + diff);
        }else{
            qreal diff = miny - 25 ;
            it->moveBy(0,-diff);
            mainRect.setHeight(maxh + 25 - diff);
        }
    }

    prepareGeometryChange();
    boundingR = QRectF(mainRect);
    update();
}

void ApplicationItem::findInputOutputData(Connection& cnn,  ModulePContainer &modules,
                                          InputData* &input_, OutputData* &output_)
{
    input_ = nullptr;
    output_ = nullptr;
    string strTo = cnn.to();
    string strFrom = cnn.from();

    ModulePIterator itr;
    for(itr=modules.begin(); itr!=modules.end(); itr++)
    {
        Module* module = (*itr);
        //if(module->owner() == application)
        {
            for(int i=0; i<module->inputCount(); i++)
            {
                InputData &input = module->getInputAt(i);
                string strInput = string(module->getPrefix()) + string(input.getPort());
                if(strTo == strInput)
                {
                    input_ = &input;
                    break;
                }
            }

            for(int i=0; i<module->outputCount(); i++)
            {
                OutputData &output = module->getOutputAt(i);
                string strOutput = string(module->getPrefix()) + string(output.getPort());
                if(strFrom == strOutput)
                {
                    output_ = &output;
                    break;
                }
            }
        }
    }
}

PortItem* ApplicationItem::findModelFromOutput(OutputData* output)
{
    for(int i=0; i<scene()->items().count(); i++)
    {
        QGraphicsItem *it = scene()->items().at(i);
        if(it->type() == (QGraphicsItem::UserType + (int)ApplicationItemType)){
            auto* application = (ApplicationItem*)it;
            for(int j=0; j<application->getModulesList().count(); j++){
                if(application->getModulesList().at(j)->type() == QGraphicsItem::UserType + ModuleItemType){
                    ModuleItem *module = (ModuleItem*)application->getModulesList().at(j);
                    for(int k=0;k<module->oPorts.count();k++){
                        PortItem *port = module->oPorts.at(k);
                        //QString prefix = QString("%1%2").arg(application->getInnerApplication()->getPrefix()).arg(module->getInnerModule()->getBasePrefix());

                        //if(!strcmp(port->outData.getPort(), (*output).getPort()) && modulePrefix == prefix)  {
                        if (port->outData == output) {
                            return port;
                        }
                    }
                }
            }
        }



        if(it->type() == (QGraphicsItem::UserType + (int)ModuleItemType)){
            auto* module = (ModuleItem*)it;
            for(int k=0;k<module->oPorts.count();k++){
                PortItem *port = module->oPorts.at(k);
                //if(!strcmp(port->outData.getPort(), (*output).getPort()) && modulePrefix == QString("%1").arg(module->getInnerModule()->getPrefix()))  {
                if(port->outData == output){
                    return port;
                }
            }

        }
    }



    return nullptr;
}

PortItem*  ApplicationItem::findModelFromInput(InputData* input)
{
    for(int i=0; i<scene()->items().count(); i++)
    {
        QGraphicsItem *it = scene()->items().at(i);

        if(it->type() == (QGraphicsItem::UserType + (int)ApplicationItemType)){
            auto* application = (ApplicationItem*)it;
            for(int j=0;j<application->getModulesList().count();j++){
                if(application->getModulesList().at(j)->type() == QGraphicsItem::UserType + ModuleItemType){
                    ModuleItem *module = (ModuleItem*)application->getModulesList().at(j);
                    for(int k=0;k<module->iPorts.count();k++){
                        PortItem *port = module->iPorts.at(k);
                        //QString prefix = QString("%1%2").arg(application->getInnerApplication()->getPrefix()).arg(module->getInnerModule()->getBasePrefix());
                        //if(!strcmp(port->inData.getPort(),(*input).getPort()) && modulePrefix == prefix)  {
                        if(port->inData == input){
                            return port;
                        }
                    }
                }
            }

        }

        if(it->type() == (QGraphicsItem::UserType + (int)ModuleItemType)){
            auto* module = (ModuleItem*)it;
            for(int k=0;k<module->iPorts.count();k++){
                PortItem *port = module->iPorts.at(k);
                //if(!strcmp(port->inData.getPort(),(*input).getPort()) && modulePrefix == QString("%1").arg(module->getInnerModule()->getPrefix())){
                if(port->inData == input){
                    return port;
                }
            }

        }
    }


    return nullptr;
}


void ApplicationItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    //QGraphicsItem::paint(painter,option,widget);
    painter->save();
    QPainterPath path;
    path.moveTo(mainRect.x(),mainRect.y());
    path.lineTo(mainRect.x(),mainRect.y() - 30);
    path.lineTo(mainRect.x() + textWidth + 50,mainRect.y() - 30);
    path.lineTo(mainRect.x() + textWidth + 50,mainRect.y());
    path.lineTo(mainRect.x() + mainRect.width(),mainRect.y());
    path.lineTo(mainRect.x() + mainRect.width(),mainRect.y() + mainRect.height());
    path.lineTo(mainRect.x(),mainRect.y() + mainRect.height());
    path.lineTo(mainRect.x(),mainRect.y());
    //path.addRoundedRect( mainRect, 5.0, 5.0 );



    painter->setFont(customFont);
    painter->setPen(QPen(QBrush(QColor(Qt::black)),1));
    painter->drawText(QRectF(mainRect.x(),mainRect.y() - 30,
                             textWidth + 50,30
                             ),Qt::AlignCenter,itemName);

    if(isSelected()){
        painter->setBrush(QBrush(QColor(0,0,220,80)));
    }else{
        painter->setBrush(QBrush(QColor(156,156,255,80)));
    }
    painter->drawPath(path);
    painter->restore();

}

QList<QGraphicsItem *> ApplicationItem::getModulesList()
{
    return childItems();
}

void ApplicationItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    moved = true;
    QGraphicsItem::mouseMoveEvent(event);
}

void ApplicationItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    pressed = true;
    //setZValue(zValue() + 10);
    emit sigHandler->applicationSelected(this);
    QGraphicsItem::mousePressEvent(event);
}

void ApplicationItem::updateGraphicModel()
{
    GraphicModel modBase;
    GyPoint p;
    p.x = pos().x();
    p.y = pos().y();
    modBase.points.push_back(p);
    application->setModelBase(modBase);
}

void ApplicationItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{

    if(moved && editingMode && !nestedInApp){
        //updateGraphicModel();
        emit signalHandler()->modified();
        emit signalHandler()->moved();
    }
    pressed = false;
    moved = false;
    //setZValue(zValue() - 10);
    QGraphicsItem::mouseReleaseEvent(event);
}


QVariant ApplicationItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemPositionChange) {
        foreach (QGraphicsItem *it, childItems()) {
            if(it->type() == QGraphicsItem::UserType + ModuleItemType){
                ModuleItem *mIt = ((ModuleItem*)it);
                foreach (PortItem *p, mIt->oPorts) {
                    p->updateConnections();
                }
                foreach (PortItem *p, mIt->iPorts) {
                    p->updateConnections();
                }
            }
            if(it->type() == QGraphicsItem::UserType + SourcePortItemType){
                SourcePortItem *sIt = ((SourcePortItem *)it);
                sIt->updateConnections();
            }
            if(it->type() == QGraphicsItem::UserType + DestinationPortItemType){
                DestinationPortItem *dIt = ((DestinationPortItem *)it);
                dIt->updateConnections();
            }

        }

        if(snap && !nestedInApp){
            QPointF newPos = value.toPointF();
            QPointF closestPoint = computeTopLeftGridPoint(newPos);
            return closestPoint+=offset;
        }
    }

    return value;
}


void ApplicationItem::setModuleRunning(bool running, int id)
{
    for(int i=0;i<childItems().count();i++){
        if(childItems().at(i)->type() == (QGraphicsItem::UserType + ModuleItemType)){
            ModuleItem *m = (ModuleItem *)childItems().at(i);
            if(m->getId() == id){
                m->setRunning(running);
                break;
            }
        }
        if(childItems().at(i)->type() == (QGraphicsItem::UserType + ApplicationItemType)){
            ApplicationItem *a = (ApplicationItem *)childItems().at(i);
            a->setModuleRunning(running,id);
        }
    }
}

void ApplicationItem::setSelectedConnections(QList<int>selectedIds)
{
    foreach (QGraphicsItem *it, childItems()) {
        if(it->type() == QGraphicsItem::UserType + ConnectionItemType){
            auto* arrow = (Arrow*)it;
            if(selectedIds.contains(arrow->getId())){
                arrow->setConnectionSelected(true);
            }else{
                arrow->setConnectionSelected(false);
            }
        }else
            if(it->type() == QGraphicsItem::UserType + ApplicationItemType){
                auto* app = (ApplicationItem*)it;
                app->setSelectedConnections(selectedIds);
            }
    }
}

void ApplicationItem::setSelectedModules(QList<int>selectedIds)
{
    foreach (QGraphicsItem *it, childItems()) {
        if(it->type() == QGraphicsItem::UserType + ModuleItemType){
            auto* mod = (ModuleItem*)it;
            if(selectedIds.contains(mod->getId())){
                mod->setModuleSelected(true);
            }else{
                mod->setModuleSelected(false);
            }
        }else
            if(it->type() == QGraphicsItem::UserType + ApplicationItemType){
                auto* app = (ApplicationItem*)it;
                app->setSelectedModules(selectedIds);
            }
    }
}


void ApplicationItem::setConnectionConnected(bool connected, QString from, QString to)
{
    for(int i=0;i<childItems().count();i++){
        if(childItems().at(i)->type() == (QGraphicsItem::UserType + ConnectionItemType)){
            Arrow *m = (Arrow *)childItems().at(i);
            if(m->getFrom() == from && m->getTo() == to){
                m->setConnected(connected);
                break;
            }
        }
        if(childItems().at(i)->type() == (QGraphicsItem::UserType + ApplicationItemType)){
            ApplicationItem *a = (ApplicationItem *)childItems().at(i);
            a->setConnectionConnected(connected,from,to);
        }
    }
}

void ApplicationItem::setOutputPortAvailable(QString oData, bool available)
{
    foreach (QGraphicsItem *it, childItems()) {
        auto* item = (BuilderItem *)it;
        if(item->type() == QGraphicsItem::UserType + ModuleItemType){
            auto* mod = (ModuleItem*)item;

            foreach (PortItem *oPort, mod->oPorts) {
                QString strPort = QString("%1%2").arg(mod->getInnerModule()->getPrefix()).arg(oPort->outData->getPort());

                if(strPort == oData ){
                    oPort->setAvailable((available)? PortItem::availbale : PortItem::unavailable);
                }
            }


        }else
            if(item->type() == QGraphicsItem::UserType + ApplicationItemType){
                auto* app = (ApplicationItem*)item;
                app->setOutputPortAvailable(oData, available);
            }else
                if(item->type() == QGraphicsItem::UserType + SourcePortItemType){
                    auto* source = (SourcePortItem*)item;
                    QString strPort = QString("%1").arg(source->getItemName());
                    if(strPort == oData){
                        source->setAvailable(available);
                    }
                }
    }
}

void ApplicationItem::setInputPortAvailable(QString iData, bool available)
{


    foreach (QGraphicsItem *it, childItems()) {
        auto* item = (BuilderItem *)it;
        if(item->type() == QGraphicsItem::UserType + ModuleItemType){
            auto* mod = (ModuleItem*)item;

            foreach (PortItem *iPort, mod->iPorts) {
                QString strPort = QString("%1%2").arg(mod->getInnerModule()->getPrefix()).arg(iPort->inData->getPort());
                if(strPort == iData){
                    iPort->setAvailable((available)? PortItem::availbale : PortItem::unavailable);
                }
            }


        }else
            if(item->type() == QGraphicsItem::UserType + ApplicationItemType){
                auto* app = (ApplicationItem*)item;
                app->setInputPortAvailable(iData,available);
            }
            else{
                if(item->type() == QGraphicsItem::UserType + DestinationPortItemType){
                    auto* dest = (DestinationPortItem*)item;

                    QString strPort = QString("%1").arg(dest->getItemName());
                    if(strPort == iData){
                        dest->setAvailable(available);
                    }
                }
            }
    }
}
