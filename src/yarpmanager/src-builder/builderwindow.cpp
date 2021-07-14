/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "builderwindow.h"
//#include "ui_builderwindow.h"
#include <QHBoxLayout>
#include <QSplitter>

using namespace std;

BuilderWindow::BuilderWindow(Application *app, Manager *lazyManager, SafeManager *safeManager, bool editingMode, QWidget *parent) :
    QWidget(parent)/*,
    ui(new Ui::BuilderWindow)*/
{

    //ui->setupUi(this);
    this->editingMode = editingMode;
    this->lazyManager = lazyManager;
    this->app = app;
    this->safeManager = safeManager;


    init();

}

void BuilderWindow::init()
{
    scene = new BuilderScene(this);
    scene->setSceneRect(QRectF(0, 0, 2000, 2000));
    scene->setBackgroundBrush(QPixmap(":/images/background2.png"));
    connect(scene,SIGNAL(addedModule(void*,QPointF)),this,SLOT(onAddModule(void*,QPointF)));
    connect(scene,SIGNAL(addedApplication(void*,QPointF)),this,SLOT(onAddedApplication(void*,QPointF)));
    connect(scene,SIGNAL(addNewConnection(void*,void*)),this,SLOT(onAddNewConnection(void*,void*)));

    view = new CustomView(this);
    connect(view,SIGNAL(pressedNullItem()),this,SLOT(initApplicationTab()));
    connect(view,SIGNAL(addSourcePort(QString,QPointF)),this,SLOT(onAddSourcePort(QString,QPointF)));
    connect(view,SIGNAL(addDestinationPort(QString,QPointF)),this,SLOT(onAddDestinationPort(QString,QPointF)));
    connect(view,SIGNAL(addModule(void*,QPointF)),this,SLOT(onAddModule(void*,QPointF)));
    connect(view,SIGNAL(addNewConnection(void*,void*)),this,SLOT(onAddNewConnection(void*,void*)));
    connect(view,SIGNAL(modified()),this,SLOT(onModified()));

    view->setMouseTracking(true);
    view->setScene(scene);
    view->centerOn(scene->sceneRect().width()/2,scene->sceneRect().height()/2);
    view->setEditingMode(editingMode);

    QAction *zoomIn = builderToolbar.addAction("Zoom In");
    QAction *zoomOut = builderToolbar.addAction("Zoom Out");
    QAction *restoreZoom = builderToolbar.addAction("Reset Zoom");
    builderToolbar.addSeparator();
    QAction *showGrid = builderToolbar.addAction("Show Grid");
    QAction *snapToGrid = builderToolbar.addAction("Snap to Grid");
    builderToolbar.addSeparator();

    snapToGrid->setCheckable(true);
    showGrid->setCheckable(true);
    showGrid->setChecked(true);
    connect(snapToGrid,SIGNAL(triggered(bool)),this,SLOT(onSnap(bool)));
    connect(restoreZoom,SIGNAL(triggered()),this,SLOT(onRestoreZoom()));
    connect(showGrid,SIGNAL(triggered(bool)),this,SLOT(onShowGrid(bool)));
    connect(zoomIn,SIGNAL(triggered(bool)),this,SLOT(onZoomIn()));
    connect(zoomOut,SIGNAL(triggered(bool)),this,SLOT(onZoomOut()));

    if(editingMode){
       prepareManagerFrom(lazyManager,app->getName());
    }


    auto* layout = new QVBoxLayout;
    if(editingMode){
        propertiesTab = new PropertiesTable(&manager);
        connect(propertiesTab,SIGNAL(modified()),this,SLOT(onModified()));
        propertiesTab->showApplicationTab(manager.getKnowledgeBase()->getApplication());
        splitter = new QSplitter;
        splitter->addWidget(view);
        splitter->addWidget(propertiesTab);
        layout->addWidget(splitter);
    }else{
        layout->addWidget(view);
    }


    layout->setMargin(0);
    view->centerOn(0,0);
    setLayout(layout);


    m_modified = false;
    emit modified(false);

    //onRestoreZoom();

}

bool BuilderWindow::save()
{
    if(!editingMode){
        return true;
    }
    Application* application = manager.getKnowledgeBase()->getApplication();
    if (!application) {
        return true;
    }

    foreach (QGraphicsItem *it, scene->items()) {
        if(it->type() == QGraphicsItem::UserType+SourcePortItemType){
            if(((SourcePortItem*)it)->isErrorState()){
                return false;
            }
        }
        if(it->type() == QGraphicsItem::UserType+DestinationPortItemType){
            if(((DestinationPortItem*)it)->isErrorState()){
                return false;
            }
        }
    }

    m_modified = !manager.saveApplication(application->getName());
    emit modified(m_modified);
    return !m_modified;
}

QString BuilderWindow::getFileName()
{
    Application* application = manager.getKnowledgeBase()->getApplication();
    if (!application) {
        return {};
    };
    return QString(application->getXmlFile());
}
void BuilderWindow::setFileName(QString filename)
{
    Application* application = manager.getKnowledgeBase()->getApplication();
    if (application) {
        application->setXmlFile(filename.toStdString().c_str());
    }
    return;
}
QString BuilderWindow::getAppName()
{
    Application* application = manager.getKnowledgeBase()->getApplication();
    if (!application) {
        return {};
    };
    return QString(application->getName());
}
void BuilderWindow::setAppName(QString appName)
{
    Application* application = manager.getKnowledgeBase()->getApplication();
    if (application) {
        application->setName(appName.toStdString().c_str());
    }
    return;
}

void BuilderWindow::prepareManagerFrom(Manager* lazy,
                                           const char* szAppName)
{

    // making manager from lazy manager
    KnowledgeBase* lazy_kb = lazy->getKnowledgeBase();

    ModulePContainer mods =  lazy_kb->getModules();
    for (auto& mod : mods) {
        manager.getKnowledgeBase()->addModule(mod);
    }

    ResourcePContainer res =  lazy_kb->getResources();
    for (auto& re : res) {
        manager.getKnowledgeBase()->addResource(re);
    }

    ApplicaitonPContainer apps =  lazy_kb->getApplications();
    for (auto& app : apps) {
        manager.getKnowledgeBase()->addApplication(app);
    }

    // loading application
    manager.loadApplication(szAppName);


}


BuilderWindow::~BuilderWindow()
{
    //delete ui;
    for(int i=0;i<scene->items().count();i++){
        QGraphicsItem *it = scene->items().at(i);
        if(it->type() == QGraphicsItem::UserType + (int)ModuleItemType){
            auto* bItem  = (BuilderItem*)it;
            disconnect(bItem->signalHandler(),SIGNAL(moduleSelected(QGraphicsItem*)),this,SLOT(onModuleSelected(QGraphicsItem*)));
            disconnect(bItem->signalHandler(),SIGNAL(requestNewConnection(QPointF,QGraphicsItem*)),scene,SLOT(onNewConnectionRequested(QPointF,QGraphicsItem*)));
            disconnect(bItem->signalHandler(),SIGNAL(addNewConnection(QPointF,QGraphicsItem*)),scene,SLOT(onNewConnectionAdded(QPointF,QGraphicsItem*)));
            disconnect(bItem->signalHandler(),SIGNAL(modified()),this,SLOT(onModified()));
            disconnect(bItem->signalHandler(),SIGNAL(moved()),this,SLOT(onMoved()));
        }
        if(it->type() == QGraphicsItem::UserType + (int)ApplicationItemType){
            auto* bItem  = (BuilderItem*)it;
            disconnect(bItem->signalHandler(),SIGNAL(moduleSelected(QGraphicsItem*)),this,SLOT(onModuleSelected(QGraphicsItem*)));
            disconnect(bItem->signalHandler(),SIGNAL(connectctionSelected(QGraphicsItem*)),this,SLOT(onConnectionSelected(QGraphicsItem*)));
            disconnect(bItem->signalHandler(),SIGNAL(applicationSelected(QGraphicsItem*)),this,SLOT(onApplicationSelected(QGraphicsItem*)));
            disconnect(bItem->signalHandler(),SIGNAL(modified()),this,SLOT(onModified()));
            disconnect(bItem->signalHandler(),SIGNAL(moved()),this,SLOT(onMoved()));
        }
    }
    view->close();
    scene->clear();

    //view->deleteAllItems();
}


QToolBar *BuilderWindow::getToolBar()
{
    return &builderToolbar;
}

void BuilderWindow::removeToolBar()
{
    layout()->removeWidget(&builderToolbar);
    builderToolbar.hide();
}

void BuilderWindow::addToolBar()
{
    layout()->removeWidget(&builderToolbar);
    if(editingMode){
        layout()->removeWidget(splitter);
    }else{
        layout()->removeWidget(view);
    }

    layout()->addWidget(&builderToolbar);
    if(editingMode){
        layout()->addWidget(splitter);
    }else{
        layout()->addWidget(view);
    }

    builderToolbar.show();
}

void BuilderWindow::addAction(QAction *act)
{
   builderActions.append(act);

}

void BuilderWindow::addModulesAction(QAction *act)
{
    modulesAction.append(act);
}

void BuilderWindow::addConnectionsAction(QAction *act)
{
    connectionsAction.append(act);
}

void BuilderWindow::setModuleRunning(bool running, int id)
{
    for(int i=0;i<scene->items().count();i++){
        if(scene->items().at(i)->type() == (QGraphicsItem::UserType + ModuleItemType)){
            ModuleItem *m = (ModuleItem *)scene->items().at(i);
            if(m->getId() == id){
                m->setRunning(running);
                break;
            }
        }
        if(scene->items().at(i)->type() == (QGraphicsItem::UserType + ApplicationItemType)){
            ApplicationItem *a = (ApplicationItem *)scene->items().at(i);
            a->setModuleRunning(running,id);
        }
    }
}

void BuilderWindow::setConnectionConnected(bool connected, QString from, QString to)
{
    for(int i=0;i<scene->items().count();i++){
        if(scene->items().at(i)->type() == (QGraphicsItem::UserType + ConnectionItemType)){
            Arrow *m = (Arrow *)scene->items().at(i);
            if(m->getFrom() == from && m->getTo() == to){
                m->setConnected(connected);
                break;
            }
        }
        if(scene->items().at(i)->type() == (QGraphicsItem::UserType + ApplicationItemType)){
            ApplicationItem *a = (ApplicationItem *)scene->items().at(i);
            a->setConnectionConnected(connected,from,to);
        }
    }
}



bool BuilderWindow::isApplicationPresent(Application *application)
{
    for(int i=0;i<scene->items().count();i++){
        BuilderItem *it = (BuilderItem*)scene->items().at(i);
        if(it->type() == QGraphicsItem::UserType + ApplicationItemType){
            auto* appItem = (ApplicationItem*)it;
            if(appItem->getInnerApplication() == application){
                return true;
            }
        }
    }
    return false;
}

bool BuilderWindow::isModulePresent(Module *module)
{
    for(int i=0;i<scene->items().count();i++){
        BuilderItem *it = (BuilderItem*)scene->items().at(i);
        if(it->type() == QGraphicsItem::UserType + ModuleItemType){
            auto* appItem = (ModuleItem*)it;
            if(appItem->getInnerModule() == module){
                return true;
            }
        }
    }
    return false;
}




void BuilderWindow::load(bool refresh)
{
    index = 0;


    usedModulesId.clear();
    connectionsId = 0;

    bool ret = true;//safeManager->loadApplication(app->getName());
    if(ret){
        Application* mainApplication;
        CnnContainer connections;
        ApplicaitonPContainer applications;
        ModulePContainer modules;
        ExecutablePContainer exes;


        if(!editingMode){
            mainApplication = safeManager->getKnowledgeBase()->getApplication();
            connections = safeManager->getKnowledgeBase()->getConnections(mainApplication);
            applications = safeManager->getKnowledgeBase()->getApplications(mainApplication);
            exes = safeManager->getExecutables();
        }else{
            mainApplication = manager.getKnowledgeBase()->getApplication();
            connections = manager.getKnowledgeBase()->getConnections(mainApplication);
            applications = manager.getKnowledgeBase()->getApplications(mainApplication);
            modules = manager.getKnowledgeBase()->getModules(mainApplication);
        }

        //ModulePContainer modules = safeManager->getKnowledgeBase()->getModules(mainApplication);

        /*************************************/
        ExecutablePIterator exeitr;
        ModulePIterator moditr;
        /*************************************/



        ApplicationPIterator appItr;
        for(appItr=applications.begin(); appItr!=applications.end(); appItr++)
        {
            Application* application = (*appItr);
            addApplication(application,&connectionsId);
        }


        if(!editingMode){
            for(exeitr=exes.begin(); exeitr<exes.end(); exeitr++){
                Module* module = (*exeitr)->getModule();

                QString id = QString("%1").arg((*exeitr)->getID());
                bool idFound = false;
                foreach (int usedId, usedModulesId) {
                    if(usedId == id.toInt()){
                        idFound = true;
                        break;
                    }
                }
                if(!idFound){
                    usedModulesId.append(id.toInt());
                    addModule(module,id.toInt());
                }

            }
        }else{
            for(moditr=modules.begin(); moditr<modules.end(); moditr++){
                auto* module = (Module*)(*moditr);
                addModule(module,-1);
            }
        }



        index = (index/900)*100+50;
        CnnIterator citr;
        ModulePContainer allModules;
        if(!editingMode){
            allModules = safeManager->getKnowledgeBase()->getSelModules();
        }else{
            allModules = manager.getKnowledgeBase()->getSelModules();
        }

        for(citr=connections.begin(); citr<connections.end(); citr++){
            Connection baseCon = *citr;
//            if(baseCon.owner()->getLabel() != mainApplication->getLabel()){
//                continue;
//            }
            GraphicModel model = baseCon.getModelBase();
            InputData* input = nullptr;
            OutputData* output = nullptr;
            BuilderItem *source = nullptr;
            BuilderItem *dest = nullptr;
            QString inModulePrefix,outModulePrefix;
            findInputOutputData((*citr), allModules, input, output,&inModulePrefix,&outModulePrefix);
            if(output){
                source = findModelFromOutput(output,outModulePrefix);
            }else{
                bool bExist = false;
                SourcePortItem *sourcePort = nullptr;
                for(int i=0;i<scene->items().count() && !bExist;i++){
                    if(scene->items().at(i)->type() == (QGraphicsItem::UserType + SourcePortItemType)){
                        SourcePortItem *auxSourceport = (SourcePortItem*)scene->items().at(i);
                        if(auxSourceport->getItemName() == baseCon.from()){
                            source = auxSourceport;
                            bExist = true;
                            break;
                        }

                    }
                    if(scene->items().at(i)->type() == (QGraphicsItem::UserType + ApplicationItemType)){
                        // TODO
                        qDebug() << "APPLICATION CONN";
                    }
                }
                if(!bExist){
                    sourcePort = (SourcePortItem*)addSourcePort((*citr).from());
                    source = sourcePort;

                    if(model.points.size() > 1){
                        source->setPos(model.points[1].x /*+ source->boundingRect().width()/2*/,
                                       model.points[1].y /*+ source->boundingRect().height()/2*/);
                    }else{
                        source->setPos(10 + source->boundingRect().width()/2, index);
                    }
                }


                index += 40;
            }
            if(input){
                dest = findModelFromInput(input,inModulePrefix);
            }else{
                bool bExist = false;
                for(int i=0;i<scene->items().count() && !bExist;i++){
                    if(scene->items().at(i)->type() == (QGraphicsItem::UserType + DestinationPortItemType)){
                        DestinationPortItem *auxDestPort = (DestinationPortItem*)scene->items().at(i);
                        if(auxDestPort->getItemName() == baseCon.to()){
                            dest = auxDestPort;
                            bExist = true;
                            break;
                        }
                    }
                    if(scene->items().at(i)->type() == (QGraphicsItem::UserType + ApplicationItemType)){
                        // TODO
                        qDebug() << "APPLICATION CONN";
                    }
                }
                if(!bExist){
                    dest = (DestinationPortItem*)addDestinantionPort((*citr).to());

                    size_t size = model.points.size();
                    if(size > 2){
                        dest->setPos(model.points[size-1].x /*+ dest->boundingRect().width()/2*/,
                                       model.points[size-1].y/* + dest->boundingRect().height()/2*/);
                    }else{
                        dest->setPos(400 + dest->boundingRect().width()/2, index);
                    }
                }


             }

            //Arrow *arrow;
            // check for arbitrators
            string strCarrier = baseCon.carrier();
            if((strCarrier.find("recv.priority") != std::string::npos)){
                // TODO
            }else{
                if(source && dest){
//                    Arrow *arrow =(Arrow*)addConnection(source,dest,id);
//                    arrow->setConnection(baseCon);
                    auto* arrow =(Arrow*)addConnection(source,dest,connectionsId);
                    arrow->setConnection(baseCon);

//                    arrow = new Arrow(source, dest, baseCon,id,!editingMode ? safeManager : &manager,false,editingMode);
//                    arrow->setActions(connectionsAction);
//                    connect(arrow->signalHandler(),SIGNAL(connectctionSelected(QGraphicsItem*)),this,SLOT(onConnectionSelected(QGraphicsItem*)));
//                    arrow->setColor(QColor(Qt::red));
//                    source->addArrow(arrow);
//                    dest->addArrow(arrow);
//                    scene->addItem(arrow);
//                    arrow->setZValue(1);
//                    arrow->updatePosition();
                    //itemsList.append(arrow);
                }
            }
            connectionsId++;
        }
    }

    QRectF rr = itemsBoundingRect();
    view->fitInView(rr,Qt::KeepAspectRatio);


}

QRectF BuilderWindow::itemsBoundingRect()
{
    QRectF rr;
    foreach(QGraphicsItem *it, scene->items()){
        if(it->parentItem() != nullptr && it->parentItem()->type() != QGraphicsItem::UserType + ConnectionItemType){
            continue;
        }
        if(it->type() == QGraphicsItem::UserType + ModuleItemType ||
                it->type() == QGraphicsItem::UserType + ApplicationItemType ||
                it->type() == QGraphicsItem::UserType + SourcePortItemType ||
                it->type() == QGraphicsItem::UserType + DestinationPortItemType){
            QRectF bRect = it->mapToScene(it->boundingRect()).boundingRect();
            rr = rr.united(bRect);
        }

        if(it->type() == QGraphicsItem::UserType + ConnectionItemType){
            auto* bItem = (Arrow *)it;
            foreach (LineHandle *i, bItem->handles()) {
                rr = rr.united(bItem->mapToScene(bItem->mapFromItem(i,i->boundingRect()).boundingRect()).boundingRect());
            }
        }
    }
    return rr;
}

BuilderItem *BuilderWindow::onAddSourcePort(QString name,QPointF pos)
{
    BuilderItem *it = addSourcePort(name,true);
    it->snapToGrid(scene->snap);
    it->setSelected(true);
    it->setPos(pos);
    return it;
}

BuilderItem *BuilderWindow::onAddDestinationPort(QString name,QPointF pos)
{
    BuilderItem *it = addDestinantionPort(name,true);
    it->snapToGrid(scene->snap);
    it->setSelected(true);
    it->setPos(pos);
    return it;
}

BuilderItem *BuilderWindow::onAddNewConnection(void *startItem ,void *endItem, int connectionId)
{
    Manager *manager = !editingMode ? safeManager : &this->manager;

    Application* mainApplication = nullptr;
    mainApplication = manager->getKnowledgeBase()->getApplication();
    Connection connection;
    auto* myStartItem = (BuilderItem*)startItem;
    auto* myEndItem = (BuilderItem*)endItem;
    GyPoint p,p1,fakeLblPoint;
    QString label;
    setToolTip(QString("%1 --> %2").arg(myStartItem->getItemName()).arg(myEndItem->getItemName()));

    bool bExternTo = false;
    bool bExternFrom = false;

    InputData* input = nullptr;
    OutputData* output = nullptr;

    string strFrom,strTo ;

    // Source
    if(myStartItem->type() == (QGraphicsItem::UserType + (int)ModulePortItemType)){
        PortItem *port = ((PortItem*)myStartItem);
        auto* module = (ModuleItem *)port->parentItem();
        int portType = port->getPortType();
        if(portType == OUTPUT_PORT){
            output = port->getOutputData();
            strFrom = string(module->getInnerModule()->getPrefix()) + string(port->getOutputData()->getPort());
            label = QString("%1").arg(port->getOutputData()->getCarrier());

            if(((ModuleItem*)port->parentItem())->getInnerModule()->owner() != mainApplication){
                bExternFrom = true;
            }
        }

    }else if(myStartItem->type() == (QGraphicsItem::UserType + (int)SourcePortItemType)){
        strFrom = string(myStartItem->getItemName().toLatin1().data());
        bExternFrom = true;
    }

    // Destination
    if(myEndItem->type() == (QGraphicsItem::UserType + (int)ModulePortItemType)){
        PortItem *port = ((PortItem*)myEndItem);
        auto* module = (ModuleItem *)port->parentItem();
        int portType = port->getPortType();
        if(portType == INPUT_PORT){
            input = port->getInputData();
            strTo = string(module->getInnerModule()->getPrefix()) + string(port->getInputData()->getPort());
            label = QString("%1").arg(port->getInputData()->getCarrier());

            if(((ModuleItem*)port->parentItem())->getInnerModule()->owner() != mainApplication){
                bExternTo = true;
            }
        }

    }else if(myEndItem->type() == (QGraphicsItem::UserType + (int)DestinationPortItemType)){
        strTo = string(myEndItem->getItemName().toLatin1().data());
        bExternTo = true;

    }



    if(label.isEmpty()){
        label = "udp";
    }

    auto* arrow = (Arrow*)addConnection((BuilderItem*)startItem, (BuilderItem*)endItem,connectionId);

    connection.setFrom(strFrom.c_str());
    connection.setTo(strTo.c_str());
    connection.setCarrier(label.toLatin1().data());
    connection.setFromExternal(bExternFrom);
    connection.setToExternal(bExternTo);
    connection.setCorOutputData(output);
    connection.setCorInputData(input);


    fakeLblPoint.x = -1;
    fakeLblPoint.y = -1;
    p.x = (myStartItem->pos()).x();
    p.y = (myStartItem->pos()).y();
    p1.x = (myEndItem->pos()).x();
    p1.y = (myEndItem->pos()).y();
    arrow->getModel()->points.clear();
    arrow->getModel()->points.push_back(fakeLblPoint);
    arrow->getModel()->points.push_back(p);
    arrow->getModel()->points.push_back(p1);
    connection.setModel(arrow->getModel());
    connection.setModelBase(*arrow->getModel());
    connection.setPersistent(false);
    connection = manager->getKnowledgeBase()->addConnectionToApplication(mainApplication, connection);

    arrow->setConnection(connection);
    //arrow->updateModel(p,p1,fakeLblPoint);
//    Arrow *arrow = new Arrow((BuilderItem*)startItem, (BuilderItem*)endItem, &manager,false,editingMode);
//    arrow->setActions(connectionsAction);
//    connect(arrow->signalHandler(),SIGNAL(connectctionSelected(QGraphicsItem*)),this,SLOT(onConnectionSelected(QGraphicsItem*)));
//    arrow->setColor(QColor(Qt::red));
//    ((BuilderItem*)startItem)->addArrow(arrow);
//    ((BuilderItem*)endItem)->addArrow(arrow);
//    scene->addItem(arrow);
//    arrow->setZValue(-1000.0);
//    arrow->updatePosition();
//    scene->items().append(arrow);

    m_modified = true;
    emit modified(true);
    return arrow;


}



BuilderItem * BuilderWindow::addConnection(void *startItem ,void *endItem, int connectionId)
{
   auto* arrow = new Arrow((BuilderItem*)startItem,(BuilderItem*)endItem,connectionId,!editingMode ? safeManager : &manager,false,editingMode);
   arrow->setActions(connectionsAction);
   connect(arrow->signalHandler(),SIGNAL(connectctionSelected(QGraphicsItem*)),this,SLOT(onConnectionSelected(QGraphicsItem*)));
   connect(arrow->signalHandler(),SIGNAL(modified()),this,SLOT(onModified()));
   connect(arrow->signalHandler(),SIGNAL(moved()),this,SLOT(onMoved()));
   arrow->setColor(QColor(Qt::red));
   ((BuilderItem*)startItem)->addArrow(arrow);
   ((BuilderItem*)endItem)->addArrow(arrow);
   scene->addItem(arrow);
   //arrow->setZValue(1000.0);
   arrow->updatePosition();
   scene->items().append(arrow);



   return arrow;
}

BuilderItem * BuilderWindow::addDestinantionPort(QString name, bool editOnStart)
{
    Application *mainApplication = nullptr;
    if(!editingMode){
        mainApplication = safeManager->getKnowledgeBase()->getApplication();
    }else{
        mainApplication = manager.getKnowledgeBase()->getApplication();
    }
    DestinationPortItem *destPort = new DestinationPortItem(name,false,editOnStart,mainApplication);
    connect(destPort->signalHandler(),SIGNAL(addNewConnection(QPointF,QGraphicsItem*)),scene,SLOT(onNewConnectionAdded(QPointF,QGraphicsItem*)));
    connect(destPort->signalHandler(),SIGNAL(modified()),this,SLOT(onModified()));
    //itemsList.append(destPort);

    scene->addItem(destPort);
    return destPort;
}

void BuilderWindow::onMoved()
{
    foreach(QGraphicsItem *it, scene->selectedItems()){
        if(it->parentItem() != nullptr && it->parentItem()->type() != QGraphicsItem::UserType + ConnectionItemType){
            continue;
        }
        if(it->type() == QGraphicsItem::UserType + ModuleItemType){
            auto* bItem = (ModuleItem *)it;
            bItem->updateGraphicModel();
        }
        if(it->type() == QGraphicsItem::UserType + ApplicationItemType){
            auto* bItem = (ApplicationItem *)it;
            bItem->updateGraphicModel();
        }
        if(it->type() == QGraphicsItem::UserType + ConnectionItemType){
            auto* bItem = (Arrow *)it;
            bItem->updateGraphicModel();
        }
        if(it->type() == QGraphicsItem::UserType + HandleItemType){
            auto* bItem = (Arrow *)it->parentItem();
            bItem->updateGraphicModel();
        }
        if(it->type() == QGraphicsItem::UserType + ArrowLabelItemType){
            auto* bItem = (Arrow *)it->parentItem();
            bItem->updateGraphicModel();
        }
    }
}

void BuilderWindow::onModified()
{
    m_modified = true;
    emit modified(true);
}

BuilderItem * BuilderWindow::addSourcePort(QString name, bool editOnStart)
{
    Application *mainApplication = nullptr;
    if(!editingMode){
        mainApplication = safeManager->getKnowledgeBase()->getApplication();
    }else{
        mainApplication = manager.getKnowledgeBase()->getApplication();
    }
    SourcePortItem *sourcePort = new SourcePortItem(name,false,editOnStart,mainApplication);
    connect(sourcePort->signalHandler(),SIGNAL(requestNewConnection(QPointF,QGraphicsItem*)),scene,SLOT(onNewConnectionRequested(QPointF,QGraphicsItem*)));
    connect(sourcePort->signalHandler(),SIGNAL(modified()),this,SLOT(onModified()));

    //itemsList.append(sourcePort);
    scene->addItem(sourcePort);
    return sourcePort;
}

BuilderItem * BuilderWindow::addModule(Module *module,int moduleId)
{
    auto* it = new ModuleItem(module,moduleId,false,editingMode,!editingMode ? safeManager : &manager);
    it->setActions(modulesAction);
    connect(it->signalHandler(),SIGNAL(moduleSelected(QGraphicsItem*)),this,SLOT(onModuleSelected(QGraphicsItem*)));
    connect(it->signalHandler(),SIGNAL(requestNewConnection(QPointF,QGraphicsItem*)),scene,SLOT(onNewConnectionRequested(QPointF,QGraphicsItem*)));
    connect(it->signalHandler(),SIGNAL(addNewConnection(QPointF,QGraphicsItem*)),scene,SLOT(onNewConnectionAdded(QPointF,QGraphicsItem*)));
    connect(it->signalHandler(),SIGNAL(modified()),this,SLOT(onModified()));
    connect(it->signalHandler(),SIGNAL(moved()),this,SLOT(onMoved()));
    //itemsList.append(it);
    scene->addItem(it);
    it->setZValue(2);
    if(module->getModelBase().points.size()>0){
        it->setPos(module->getModelBase().points[0].x /*+ it->boundingRect().width()/2*/,
                module->getModelBase().points[0].y /*+ it->boundingRect().height()/2*/);
    }else{
        it->setPos(index%900+10 + it->boundingRect().width()/2,
                   (index/900)*100+10 + it->boundingRect().height()/2);
        index += 300;
    }
    return it;
}

ApplicationItem* BuilderWindow::addApplication(Application *application, int *connectionsId)
{

    auto* appItem = new ApplicationItem(application,!editingMode ? safeManager : &manager,&usedModulesId, false,editingMode,connectionsId);

    if(!editingMode){
        connect(appItem->signalHandler(),SIGNAL(moduleSelected(QGraphicsItem*)),this,SLOT(onModuleSelected(QGraphicsItem*)));
        connect(appItem->signalHandler(),SIGNAL(connectctionSelected(QGraphicsItem*)),this,SLOT(onConnectionSelected(QGraphicsItem*)));
        appItem->setModulesAction(modulesAction);
    }else{
        connect(appItem->signalHandler(),SIGNAL(modified()),this,SLOT(onModified()));
        connect(appItem->signalHandler(),SIGNAL(moved()),this,SLOT(onMoved()));
    }

    connect(appItem->signalHandler(),SIGNAL(applicationSelected(QGraphicsItem*)),this,SLOT(onApplicationSelected(QGraphicsItem*)));


    scene->addItem(appItem);
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
    return appItem;

}

void BuilderWindow::onAddedApplication(void *app,QPointF pos)
{
    if(!editingMode){
        return;
    }

    ApplicationInterface iapp((const char*)((Application*)app)->getName());
    GraphicModel modBase;
    GyPoint p;
    p.x = pos.x();
    p.y = pos.y();
    modBase.points.push_back(p);
    iapp.setModelBase(modBase);

    Application* mainApplication = manager.getKnowledgeBase()->getApplication();
    if (!mainApplication) {
        return;
    }

    string strPrefix = "/";
    string  uniqeId = manager.getKnowledgeBase()->getUniqueAppID(mainApplication, iapp.getName());

    strPrefix += (uniqeId);
    iapp.setPrefix(strPrefix.c_str());
    Application* application  = manager.getKnowledgeBase()->addIApplicationToApplication(mainApplication, iapp);
    if(application){
        addApplication(application,&connectionsId);

    }
    m_modified = true;
    emit modified(true);
}

BuilderItem *BuilderWindow::onAddModule(void *mod,QPointF pos)
{

    BuilderItem *modIt = nullptr;
    if(!editingMode){
        return modIt;
    }


    ModuleInterface imod((const char*)((Module*)mod)->getName());
    GraphicModel modBase;
    GyPoint p;
    p.x = pos.x();
    p.y = pos.y();
    modBase.points.push_back(p);
    imod.setModelBase(modBase);

    Application* mainApplication = manager.getKnowledgeBase()->getApplication();
    if(!mainApplication){
        return modIt;
    }

    Module* module = manager.getKnowledgeBase()->addIModuleToApplication(mainApplication, imod, true);

    if(module){
        string strPrefix = string("/") + module->getLabel();
        module->setBasePrefix(strPrefix.c_str());
        //module->setBasePrefix(((Module*)mod)->getBasePrefix());
        module->setBroker(((Module*)mod)->getBroker());
        module->setHost(((Module*)mod)->getHost());
        module->setParam(((Module*)mod)->getParam());
        module->setWorkDir(((Module*)mod)->getWorkDir());
        module->setStdio(((Module*)mod)->getStdio());


        string strAppPrefix = mainApplication->getBasePrefix();
        string prefix = strAppPrefix+module->getBasePrefix();
        manager.getKnowledgeBase()->setModulePrefix(module, prefix.c_str(), false);

        modIt = addModule(module,-1);
        modIt->setSelected(true);
        modIt->snapToGrid(scene->snap);


        //load(true);
    }
    m_modified = true;
    emit modified(true);
    return modIt;

}

void BuilderWindow::setSelectedConnections(QList<int>selectedIds)
{
    if(editingMode){
        return;
    }
    foreach (QGraphicsItem *it, scene->items()) {
        if(it->parentItem() != nullptr){
            continue;
        }
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

void BuilderWindow::setSelectedModules(QList<int>selectedIds)
{
    if(editingMode){
        return;
    }
    foreach (QGraphicsItem *it, scene->items()) {
        if(it->parentItem() != nullptr){
            continue;
        }
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

void BuilderWindow::onConnectionSelected(QGraphicsItem *it)
{
    //initModuleTab((ModuleItem*)it);
    Q_UNUSED(it);
    if(editingMode){
        return;
    }

    QList<int> selectedModules;
    foreach (QGraphicsItem *item , scene->selectedItems()) {

        if(item->type() == QGraphicsItem::UserType + (int)ConnectionItemType){
            auto* arrow = (Arrow*)item;
            selectedModules.append(arrow->getId());
        }
    }
    emit setConnectionSelected(selectedModules);
}

void BuilderWindow::onModuleSelected(QGraphicsItem *it)
{

    Q_UNUSED(it);
    if(editingMode){
        if(scene->selectedItems().count() > 1){
            propertiesTab->addModules(((ModuleItem*)it));
            return;
        }

        propertiesTab->showModuleTab(((ModuleItem*)it));
        return;
    }

    QList<int> selectedModules;
    foreach (QGraphicsItem *item , scene->selectedItems()) {

        if(item->type() == QGraphicsItem::UserType + (int)ModuleItemType){
            auto* mod = (ModuleItem*)item;
            selectedModules.append(mod->getId());
        }
    }
    emit setModuleSelected(selectedModules);
}

void BuilderWindow::onApplicationSelected(QGraphicsItem* it)
{
    if(!editingMode){
        return;
    }
    propertiesTab->showApplicationTab(((ApplicationItem*)it)->getInnerApplication());
}

void BuilderWindow::initApplicationTab()
{
    if(!editingMode){
        return;
    }

    propertiesTab->showApplicationTab(manager.getKnowledgeBase()->getApplication());


}

void BuilderWindow::initModuleTab(ModuleItem *it)
{
//    propertiesTab->clear();
//    propertiesTab->addTab(moduleProperties,"Module Properties");
//    propertiesTab->addTab(moduleDescription,"Description");
//    moduleProperties->clear();
//    moduleDescription->clear();

//    QTreeWidgetItem *modName = new QTreeWidgetItem(moduleProperties,QStringList() << "Name" << it->getInnerModule()->getName());
//    QTreeWidgetItem *modNode = new QTreeWidgetItem(moduleProperties,QStringList() << "Node" << it->getInnerModule()->getHost());
//    QTreeWidgetItem *modStdio = new QTreeWidgetItem(moduleProperties,QStringList() << "Stdio" << it->getInnerModule()->getStdio());
//    QTreeWidgetItem *modWorkDir = new QTreeWidgetItem(moduleProperties,QStringList() << "Workdir" << it->getInnerModule()->getWorkDir());
//    QTreeWidgetItem *modPrefix = new QTreeWidgetItem(moduleProperties,QStringList() << "Prefix" << it->getInnerModule()->getPrefix());
//    QTreeWidgetItem *modDeployer = new QTreeWidgetItem(moduleProperties,QStringList() << "Deployer");
//    QTreeWidgetItem *modParams = new QTreeWidgetItem(moduleProperties,QStringList() << "Parameters" << it->getInnerModule()->getParam());

//    modNode->setFlags(modNode->flags() | Qt::ItemIsEditable);
//    modStdio->setFlags(modStdio->flags() | Qt::ItemIsEditable);

//    moduleProperties->addTopLevelItem(modName);
//    moduleProperties->addTopLevelItem(modNode);
//    moduleProperties->addTopLevelItem(modStdio);
//    moduleProperties->addTopLevelItem(modWorkDir);
//    moduleProperties->addTopLevelItem(modPrefix);
//    moduleProperties->addTopLevelItem(modDeployer);
//    moduleProperties->addTopLevelItem(modParams);


//    QTreeWidgetItem *nameItem = new QTreeWidgetItem(moduleDescription,QStringList() << "Name" << it->getInnerModule()->getName());
//    QTreeWidgetItem *versionItem = new QTreeWidgetItem(moduleDescription,QStringList() << "Version" << it->getInnerModule()->getVersion());
//    QTreeWidgetItem *descriptionItem = new QTreeWidgetItem(moduleDescription,QStringList() << "Description" << it->getInnerModule()->getDescription());
//    QTreeWidgetItem *parametersItem = new QTreeWidgetItem(moduleDescription,QStringList() << "Parameters");
//    for(int i=0;i<it->getInnerModule()->argumentCount();i++){
//        Argument a = it->getInnerModule()->getArgumentAt(i);
//        QTreeWidgetItem *it = new QTreeWidgetItem(parametersItem,QStringList() << a.getParam() << a.getDescription());
//        Q_UNUSED(it);
//    }

//    QTreeWidgetItem *authorsItem = new QTreeWidgetItem(moduleDescription,QStringList() << "Authors" << it->getInnerModule()->getName());
//    for(int i=0;i<it->getInnerModule()->authorCount();i++){
//        Author a = it->getInnerModule()->getAuthorAt(i);
//        QTreeWidgetItem *it = new QTreeWidgetItem(authorsItem,QStringList() << a.getName() << a.getEmail());
//        Q_UNUSED(it);
//    }

//    QTreeWidgetItem *inputsItem = new QTreeWidgetItem(moduleDescription,QStringList() << "Inputs" << it->getInnerModule()->getName());
//    for(int i=0;i<it->getInnerModule()->inputCount();i++){
//        InputData a = it->getInnerModule()->getInputAt(i);

//        QTreeWidgetItem *type = new QTreeWidgetItem(inputsItem,QStringList() << "Type" << a.getName());
//        QTreeWidgetItem *port = new QTreeWidgetItem(type,QStringList() << "Port" << a.getPort());
//        QTreeWidgetItem *desc = new QTreeWidgetItem(type,QStringList() << "Description" << a.getDescription());
//        QTreeWidgetItem *req = new QTreeWidgetItem(type,QStringList() << "Required" << (a.isRequired() ? "yes" : "no"));
//        Q_UNUSED(port);
//        Q_UNUSED(desc);
//        Q_UNUSED(req);
//    }

//    QTreeWidgetItem *outputsItem = new QTreeWidgetItem(moduleDescription,QStringList() << "Outputs" << it->getInnerModule()->getName());
//    for(int i=0;i<it->getInnerModule()->outputCount();i++){
//        OutputData a = it->getInnerModule()->getOutputAt(i); //TODO controllare

//        QTreeWidgetItem *type = new QTreeWidgetItem(outputsItem,QStringList() << "Type" << a.getName());
//        QTreeWidgetItem *port = new QTreeWidgetItem(type,QStringList() << "Port" << a.getPort());
//        QTreeWidgetItem *desc = new QTreeWidgetItem(type,QStringList() << "Description" << a.getDescription());
//        Q_UNUSED(port);
//        Q_UNUSED(desc);
//    }

//    moduleDescription->addTopLevelItem(nameItem);
//    moduleDescription->addTopLevelItem(versionItem);
//    moduleDescription->addTopLevelItem(descriptionItem);
//    moduleDescription->addTopLevelItem(parametersItem);
//    moduleDescription->addTopLevelItem(authorsItem);
//    moduleDescription->addTopLevelItem(inputsItem);
//    moduleDescription->addTopLevelItem(outputsItem);
}

void BuilderWindow::onRestoreZoom()
{
    //view->resetMatrix();
    QRectF rr = itemsBoundingRect();
    view->fitInView(rr,Qt::KeepAspectRatio);
}

void BuilderWindow::onZoomIn()
{
    view->scale(1.1,1.1);
}

void BuilderWindow::onZoomOut()
{
    view->scale(0.9,0.9);
}

void BuilderWindow::onSnap(bool checked)
{
    scene->snapToGrid(checked);
}

void BuilderWindow::onShowGrid(bool checked)
{
    if(checked){
        scene->setBackgroundBrush(QPixmap(":/images/background2.png"));
    }else{
        scene->setBackgroundBrush(QBrush(QColor("#f8f8ff")));
    }
}

void BuilderWindow::findInputOutputData(Connection& cnn,  ModulePContainer &modules,
                                            InputData* &input_, OutputData* &output_, QString *inModulePrefix,QString *outModulePrefix)
{
    input_ = nullptr;
    output_ = nullptr;
    string strTo = cnn.to();
    string strFrom = cnn.from();
    qDebug() << "CONNECTION FROM " << strFrom.data() << " TO " << strTo.data();

    ModulePIterator itr;
    for(itr=modules.begin(); itr!=modules.end(); itr++)
    {
        Module* module = (*itr);
        for(int i=0; i<module->inputCount(); i++)
        {
            InputData &input = module->getInputAt(i);
            string strInput = string(module->getPrefix()) + string(input.getPort());
            if(strTo == strInput)
            {
                input_ = &input;
                *inModulePrefix = QString("%1").arg(module->getPrefix());
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
                *outModulePrefix = QString("%1").arg(module->getPrefix());
                break;
            }
        }
    }
}

PortItem* BuilderWindow::findModelFromOutput(OutputData* output,QString modulePrefix)
{
    for(int i=0; i<scene->items().count(); i++)
    {
        QGraphicsItem *it = scene->items().at(i);
        if(it->type() == (QGraphicsItem::UserType + (int)ApplicationItemType)){
            auto* application = (ApplicationItem*)it;
            for(int j=0; j<application->getModulesList().count(); j++){
                if(application->getModulesList().at(j)->type() == QGraphicsItem::UserType + ModuleItemType){
                    ModuleItem *module = (ModuleItem*)application->getModulesList().at(j);
                    for(int k=0;k<module->oPorts.count();k++){
                        PortItem *port = module->oPorts.at(k);
                        //QString prefix = QString("%1%2").arg(application->getInnerApplication()->getPrefix()).arg(module->getInnerModule()->getBasePrefix());
                        //if(!strcmp(port->outData.getPort(), (*output).getPort()) && modulePrefix == prefix)  {
                        if(port->outData == output){
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

PortItem*  BuilderWindow::findModelFromInput(InputData* input,QString modulePrefix)
{
    for(int i=0; i<scene->items().count(); i++)
    {
        QGraphicsItem *it = scene->items().at(i);

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




void BuilderWindow::setOutputPortAvailable(QString oData, bool available)
{
    foreach (QGraphicsItem *it, scene->items()) {

        if(it->type() == QGraphicsItem::UserType + ModuleItemType){
            auto* mod = (ModuleItem*)it;

            foreach (PortItem *oPort, mod->oPorts) {
                QString strPort = QString("%1%2").arg(mod->getInnerModule()->getPrefix()).arg(oPort->outData->getPort());

                if(strPort == oData ){
                    oPort->setAvailable((available)? PortItem::availbale : PortItem::unavailable);
                }
            }


        }else
        if(it->type() == QGraphicsItem::UserType + ApplicationItemType){
            auto* app = (ApplicationItem*)it;
            app->setOutputPortAvailable(oData, available);
        }else
            if(it->type() == QGraphicsItem::UserType + SourcePortItemType){
                auto* source = (SourcePortItem*)it;

                QString strPort = QString("%1").arg(source->getItemName());
                if(strPort == oData){
                    source->setAvailable(available);
                }

            }
    }
}

void BuilderWindow::setInputPortAvailable(QString iData, bool available)
{


    foreach (QGraphicsItem *it, scene->items()) {
        if(it->type() == QGraphicsItem::UserType + ModuleItemType){
            auto* mod = (ModuleItem*)it;

            foreach (PortItem *iPort, mod->iPorts) {
                QString strPort = QString("%1%2").arg(mod->getInnerModule()->getPrefix()).arg(iPort->inData->getPort());
                if(strPort == iData){
                    iPort->setAvailable((available)? PortItem::availbale : PortItem::unavailable);
                }
            }


        }else
            if(it->type() == QGraphicsItem::UserType + ApplicationItemType){
                auto* app = (ApplicationItem*)it;
                app->setInputPortAvailable(iData,available);
            }
            else{
                if(it->type() == QGraphicsItem::UserType + DestinationPortItemType){
                    auto* dest = (DestinationPortItem*)it;

                    QString strPort = QString("%1").arg(dest->getItemName());
                    if(strPort == iData){
                        dest->setAvailable(available);
                    }
                }
            }
    }
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/

CustomView::CustomView(BuilderWindow *builder,QGraphicsView *parent) :
    QGraphicsView(parent),
    builder(builder),
    editingMode(false),
    m_rubberBandActive(false),
    mousepressed(false),
    rubberBand(nullptr)
{
    setInteractive(true);

//    QGLWidget *viewport = new QGLWidget(QGLFormat(QGL::SampleBuffers));
//    setViewport(viewport);

    setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    setDragMode(QGraphicsView::RubberBandDrag);
    setOptimizationFlags(QGraphicsView::DontSavePainterState |QGraphicsView::DontAdjustForAntialiasing);
    setViewportUpdateMode(/*QGraphicsView::FullViewportUpdate*/QGraphicsView::BoundingRectViewportUpdate);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

}

void CustomView::setEditingMode(bool editing)
{
    editingMode = editing;
    if(scene()){
        ((BuilderScene*)scene())->editingMode = editingMode;
    }
}

bool CustomView::getEditingMode()
{
    return editingMode;
}


void CustomView::wheelEvent(QWheelEvent *event){
    if(event->modifiers() == Qt::CTRL){
        if(event->angleDelta().y() > 0){
            scale(1.1,1.1);
        }else{
            scale(0.9,0.9);
        }

        return;
    }
    QGraphicsView::wheelEvent(event);
}

void CustomView::keyPressEvent(QKeyEvent *event){
    if(event->key() == Qt::Key_A && event->modifiers() == Qt::CTRL){
        foreach (QGraphicsItem *it, items()) {
            it->setSelected(true);
        }
    }
    if(editingMode && event->key() == Qt::Key_Delete){
        deleteSelectedItems();
        QGraphicsView::keyPressEvent(event);
        return;
    }
    if(editingMode && event->key() == Qt::Key_C && event->modifiers() == Qt::CTRL){
        QGraphicsView::keyPressEvent(event);
        copySelectedItems();
        return;
    }
    if(editingMode && event->key() == Qt::Key_V && event->modifiers() == Qt::CTRL){
        QGraphicsView::keyPressEvent(event);
        pasteSelectedItems(QPoint());
        return;
    }
    QGraphicsView::keyPressEvent(event);
}

void CustomView::mouseReleaseEvent(QMouseEvent* event)
{

    QGraphicsView::mouseReleaseEvent(event);
}

void CustomView::mousePressEvent(QMouseEvent* event)
{

    if(event->button()==Qt::LeftButton){
        if(event->modifiers()==Qt::ControlModifier){
            setDragMode(QGraphicsView::ScrollHandDrag);

            QGraphicsView::mousePressEvent(event);
            return;
        }else{
            if(itemAt(event->pos()) == nullptr){
                setDragMode(QGraphicsView::RubberBandDrag);
                emit pressedNullItem();
                QGraphicsView::mousePressEvent(event);
                return;
            }else{
                setDragMode(QGraphicsView::NoDrag);
                QGraphicsView::mousePressEvent(event);
                return;
            }
        }
    }

    setDragMode(QGraphicsView::NoDrag);
    QGraphicsView::mousePressEvent(event);

}

void CustomView::contextMenuEvent(QContextMenuEvent *event)
{

    QMenu menu(this);
    QGraphicsItem *it = itemAt(event->pos());
    QAction *copyAction = nullptr;
    QAction *pasteAction = nullptr;
    QAction *deleteAction = nullptr;

    if(editingMode){
        if(!it){
            QAction *addSourcePortAction = menu.addAction("Add Source Port");
            QAction *addDestinationPortAction = menu.addAction("Add Destination Port");
            menu.addSeparator();
            pasteAction = menu.addAction("Paste");

            if(!copiedItems.isEmpty()){
                pasteAction->setEnabled(true);
            }else{
                pasteAction->setEnabled(false);
            }


            QAction *act = menu.exec(event->globalPos());
            if(act == addSourcePortAction){
                emit addSourcePort("Source",mapToScene(event->pos()));
            }
            if(act == addDestinationPortAction){
                emit addDestinationPort("Destination",mapToScene(event->pos()));
            }
            if(act == pasteAction){
                pasteSelectedItems(event->pos());
            }
            QGraphicsView::contextMenuEvent(event);
            return;
        }

        if(!((BuilderItem*)it)->isNestedInApp() && it->type() != QGraphicsItem::UserType + ApplicationItemType){
            copyAction = menu.addAction("Copy");
            pasteAction = menu.addAction("Paste");
            deleteAction = menu.addAction("Delete");

            menu.addSeparator();

            if(!copiedItems.isEmpty()){
                pasteAction->setEnabled(true);
            }else{
                pasteAction->setEnabled(false);
            }
            if(it->type() == QGraphicsItem::UserType + ConnectionItemType){
                copyAction->setEnabled(false);
                pasteAction->setEnabled(false);
            }
        }



    }else{
        if(it){
            if(it->type() == QGraphicsItem::UserType + ModuleItemType ||
               it->type() == QGraphicsItem::UserType + ConnectionItemType){
                auto* bItem = (BuilderItem*)it;

                foreach(QAction *act, bItem->getActions()){
                    menu.addAction(act);
                }
            }
        }else{
            foreach(QAction *act, builder->builderActions){
                menu.addAction(act);
            }
        }


    }





    if(menu.isEmpty()){
        QGraphicsView::contextMenuEvent(event);
        return;
    }

    QAction *act = menu.exec(event->globalPos());
    if(!act){
        QGraphicsView::contextMenuEvent(event);
        return;
    }

    if(act == deleteAction){
        if(it && it->type() == QGraphicsItem::UserType + (int)HandleItemType){
            deleteSelectedItems(it);
        }else{
            deleteSelectedItems();
        }
    }
    if(act == copyAction){
        copySelectedItems();
    }
    if(act == pasteAction){
        pasteSelectedItems(event->pos());
    }

    update();
    QGraphicsView::contextMenuEvent(event);

}


void CustomView::deleteAllItems(){
    QList <QGraphicsItem*>selectedItems;


    qDebug() << "Left Items in scene " << items().count();
    foreach (QGraphicsItem* item, items()) {
        if(item->type() != QGraphicsItem::UserType + (int)ModulePortItemType &&
                item->type() != QGraphicsItem::UserType + (int)ArrowLabelItemType &&
                item->type() != QGraphicsItem::UserType + (int)HandleItemType &&
                item->parentItem() == nullptr){
            selectedItems.append(item);
        }
    }
    foreach (QGraphicsItem* item, selectedItems) {

        if(item->type() == QGraphicsItem::UserType + (int)ConnectionItemType){
            if(selectedItems.removeOne(item)){
                //builder->itemsList.removeOne(item);
                delete item;
            }
        }

    }

    foreach (QGraphicsItem* item, selectedItems) {
        if(item->type() == QGraphicsItem::UserType + (int)ModuleItemType ||
           item->type() == QGraphicsItem::UserType + (int)SourcePortItemType ||
           item->type() == QGraphicsItem::UserType + (int)DestinationPortItemType ||
           item->type() == QGraphicsItem::UserType + (int)ApplicationItemType){
            if(selectedItems.removeOne(item)){
                //builder->itemsList.removeOne(item);
                delete item;
            }
        }
    }
    emit modified();
}


void CustomView::deleteSelectedItems(QGraphicsItem *it){
    copiedItems.clear();
    QList <QGraphicsItem*>selectedItems;
    //bool deleteOnlyHandle = false;
    if(it && it->type() == QGraphicsItem::UserType + (int)HandleItemType){
        //deleteOnlyHandle  = true;
        auto* arrow = (Arrow*)it->parentItem();
        arrow->removeHandle((LineHandle*)it);
        delete it;
        return;
    }

    foreach (QGraphicsItem* item, items()) {
        if(item->isSelected() &&
                item->type() != QGraphicsItem::UserType + (int)ModulePortItemType &&
                item->type() != QGraphicsItem::UserType + (int)ArrowLabelItemType &&
                item->type() != QGraphicsItem::UserType + (int)HandleItemType &&
                item->parentItem() == nullptr){
            selectedItems.append(item);
        }
    }

//    foreach (QGraphicsItem* item, selectedItems) {
//        if(item->type() == QGraphicsItem::UserType + (int)HandleItemType){
//            selectedItems.removeOne(item);
//            //builder->itemsList.removeOne(item);
//            delete item;
//        }

//    }

//    if(deleteOnlyHandle){
//        return;
//    }

    foreach (QGraphicsItem* item, selectedItems) {

        if(item->type() == QGraphicsItem::UserType + (int)ConnectionItemType){
            if(selectedItems.removeOne(item)){
                //builder->itemsList.removeOne(item);
               // scene()->removeItem(item);
                delete item;
            }
        }

    }

    foreach (QGraphicsItem* item, selectedItems) {
        if(item->type() == QGraphicsItem::UserType + (int)ModuleItemType ||
           item->type() == QGraphicsItem::UserType + (int)SourcePortItemType ||
           item->type() == QGraphicsItem::UserType + (int)DestinationPortItemType ||
           item->type() == QGraphicsItem::UserType + (int)ApplicationItemType){
            if(selectedItems.removeOne(item)){
                //builder->itemsList.removeOne(item);
                //scene()->removeItem(item);
                delete item;
            }

        }

    }
    emit modified();
}

void CustomView::copySelectedItems(){
    copiedItems.clear();

    foreach (QGraphicsItem* item, items()) {
        if(item->isSelected()){
            if(item->type() == QGraphicsItem::UserType + (int)SourcePortItemType ||
                    item->type() == QGraphicsItem::UserType + (int)DestinationPortItemType ||
                    item->type() == QGraphicsItem::UserType + (int)ModuleItemType ||
                    item->type() == QGraphicsItem::UserType + (int)ConnectionItemType){

                copiedItems.append(item);
            }
        }
    }
}

void CustomView::pasteSelectedItems(QPoint pos){

    foreach (QGraphicsItem* item, items()) {
        item->setSelected(false);
    }

    bool firstAdded = true;
    QSize offset;

    QMap <QGraphicsItem*,QGraphicsItem*> pastedItems;
    QList <QGraphicsItem*> itemsToRemove;
    foreach (QGraphicsItem* item, copiedItems) {
        if(item->type() == QGraphicsItem::UserType + (int)ConnectionItemType){
            Arrow *arrow = ((Arrow*)item);
            BuilderItem *startItem = arrow->startItem();
            BuilderItem *endItem = arrow->endItem();

            BuilderItem *copiedStartItem = nullptr;
            BuilderItem *copiedEndItem = nullptr;

            PortItem *startPort = nullptr;
            PortItem *endPort = nullptr;

            if(startItem->type() == QGraphicsItem::UserType + (int)ModulePortItemType){
                startPort = (PortItem*)startItem;
                startItem = (ModuleItem *)startPort->parentItem();
            }

            if(endItem->type() == QGraphicsItem::UserType + (int)ModulePortItemType){
                endPort = (PortItem*)endItem;
                endItem = (ModuleItem *)endPort->parentItem();
            }

            for(int i=0;i<itemsToRemove.count();i++){
                if(itemsToRemove.at(i) == startItem){
                    copiedStartItem = (BuilderItem*)pastedItems.value(startItem);
                }
                if(itemsToRemove.at(i) == endItem){
                    copiedEndItem = (BuilderItem*)pastedItems.value(endItem);
                }
            }

            bool startContained = false;
            bool endContained = false;
            for(int i=0;i<copiedItems.count();i++){
                if(copiedItems.at(i) == startItem){
                    startContained = true;
                }
                if(copiedItems.at(i) == endItem){
                    endContained = true;
                }
            }



            if(startContained && endContained){

                BuilderItem *startConnection = nullptr;
                BuilderItem *endConnection = nullptr;

                if(startItem->type() == QGraphicsItem::UserType + (int)ModuleItemType){
                    // Start is a Port Module
                    if(!copiedStartItem){
                        copiedStartItem = pasteItem(startItem,&offset,&firstAdded,pos);
                        itemsToRemove.append(startItem);
                        pastedItems.insert(startItem,copiedStartItem);
                    }

                    int portIndex = ((ModuleItem *)startItem)->oPorts.indexOf(startPort);
                    PortItem *newPort = ((ModuleItem *)copiedStartItem)->oPorts.at(portIndex);
                    startConnection = newPort;



                }else{
                    if(!copiedStartItem){
                        copiedStartItem = pasteItem(startItem,&offset,&firstAdded,pos);
                        itemsToRemove.append(startItem);
                        pastedItems.insert(startItem,copiedStartItem);
                    }
                    startConnection = copiedStartItem;

                }

                if(endItem->type() == QGraphicsItem::UserType + (int)ModuleItemType){
                    // Destination is a Port Module
                    if(!copiedEndItem){
                        copiedEndItem = pasteItem(endItem,&offset,&firstAdded,pos);
                        itemsToRemove.append(endItem);
                        pastedItems.insert(endItem,copiedEndItem);
                    }


                    int portIndex = ((ModuleItem *)endItem)->iPorts.indexOf(endPort);
                    PortItem *newPort = ((ModuleItem *)copiedEndItem)->iPorts.at(portIndex);
                    endConnection = newPort;


                }else{
                    if(!copiedEndItem){
                        copiedEndItem = pasteItem(endItem,&offset,&firstAdded,pos);
                        itemsToRemove.append(endItem);
                        pastedItems.insert(endItem,copiedEndItem);
                    }
                    endConnection = copiedEndItem;

                }


                if(startConnection && endConnection){
                    builder->onAddNewConnection(startConnection,endConnection);
                }

                itemsToRemove.append(item);

            }
        }
    }




    while(!itemsToRemove.isEmpty()) {
        //item->setSelected(false);
        copiedItems.removeOne(itemsToRemove.takeLast());
    }


    foreach (QGraphicsItem* item, copiedItems) {
        pasteItem(item,&offset,&firstAdded,pos);
    }


    hide();
    show();

}


BuilderItem *CustomView::pasteItem(QGraphicsItem *item, QSize *offset, bool *firstAdded,QPoint pos)
{
    BuilderItem *ret = nullptr;
    ModuleItem *mod = nullptr;
    SourcePortItem *sPort = nullptr;
    DestinationPortItem *dPort = nullptr;
    switch (item->type()) {
    case QGraphicsItem::UserType + (int)SourcePortItemType:
        sPort = ((SourcePortItem*)item);
        if(pos.isNull()){
            ret = builder->onAddSourcePort(QString("%1_copy").arg(sPort->getItemName().toLatin1().data()),
                          QPointF(sPort->pos().x() + 20, sPort->pos().y() + 20));
        }else{
            if(copiedItems.count() <= 1){
                 ret = builder->onAddSourcePort(QString("%1_copy").arg(sPort->getItemName().toLatin1().data()),
                               QPointF(mapToScene(pos).x(), mapToScene(pos).y()));
            }else{
                if(*firstAdded){
                     ret = builder->onAddSourcePort(QString("%1_copy").arg(sPort->getItemName().toLatin1().data()),
                                   QPointF(mapToScene(pos).x(), mapToScene(pos).y()));
                    *offset = QSize(mapToScene(pos).x() - sPort->pos().x(), mapToScene(pos).y() - sPort->pos().y());
                }else{
                     ret = builder->onAddSourcePort(QString("%1_copy").arg(sPort->getItemName().toLatin1().data()),
                                   QPointF(offset->width() + sPort->pos().x(), offset->height() + sPort->pos().y()));
                }

            }
        }
        break;
    case QGraphicsItem::UserType + (int)DestinationPortItemType:
        dPort = ((DestinationPortItem*)item);
        if(pos.isNull()){
            ret = builder->onAddDestinationPort(QString("%1_copy").arg(dPort->getItemName().toLatin1().data()),
                          QPointF(dPort->pos().x() + 20, dPort->pos().y() + 20));
        }else{
            if(copiedItems.count() <= 1){
                 ret = builder->onAddDestinationPort(QString("%1_copy").arg(dPort->getItemName().toLatin1().data()),
                               QPointF(mapToScene(pos).x(), mapToScene(pos).y()));
            }else{
                if(*firstAdded){
                     ret = builder->onAddDestinationPort(QString("%1_copy").arg(dPort->getItemName().toLatin1().data()),
                                   QPointF(mapToScene(pos).x(), mapToScene(pos).y()));
                    *offset = QSize(mapToScene(pos).x() - dPort->pos().x(), mapToScene(pos).y() - dPort->pos().y());
                }else{
                     ret = builder->onAddDestinationPort(QString("%1_copy").arg(dPort->getItemName().toLatin1().data()),
                                   QPointF(offset->width() + dPort->pos().x(), offset->height() + dPort->pos().y()));
                }

            }
        }
        break;
    case QGraphicsItem::UserType + (int)ModuleItemType:
        mod = ((ModuleItem*)item);
        if(pos.isNull()){
            ret = builder->onAddModule(mod->getInnerModule(),QPointF(mod->pos().x() + 20, mod->pos().y() + 20));
        }else{
            if(copiedItems.count() <= 1){
                ret = builder->onAddModule(mod->getInnerModule(),QPointF(mapToScene(pos).x(), mapToScene(pos).y()));
            }else{
                if(*firstAdded){
                    ret = builder->onAddModule(mod->getInnerModule(),QPointF(mapToScene(pos).x(), mapToScene(pos).y()));
                    *offset = QSize(mapToScene(pos).x() - mod->pos().x(), mapToScene(pos).y() - mod->pos().y());
                }else{
                    ret = builder->onAddModule(mod->getInnerModule(),QPointF(offset->width() + mod->pos().x(), offset->height() + mod->pos().y()));
                }

            }

        }
        break;
    default:
        break;
    }


    *firstAdded = false;
    return ret;
}
