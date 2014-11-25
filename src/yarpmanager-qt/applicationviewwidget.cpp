/*
 * Copyright (C) 2014 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Davide Perrone
 * Date: Feb 2014
 * email:   dperrone@aitek.it
 * website: www.aitek.it
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include "applicationviewwidget.h"
#include "ui_applicationviewwidget.h"
#include <QMessageBox>
#include <QMenu>
#include <QFileDialog>
#include <yarp/manager/localbroker.h>
#include "yscopewindow.h"
#include <QTreeWidgetItem>

ApplicationViewWidget::ApplicationViewWidget(yarp::manager::Application *app,yarp::manager::Manager *lazyManager,yarp::os::Property* config,QWidget *parent) :
    GenericViewWidget(parent), ApplicationEvent(),
    ui(new Ui::ApplicationViewWidget)
{
    ui->setupUi(this);
    lazy = lazyManager;
    this->app = app;
    m_pConfig = config;

    type = yarp::manager::APPLICATION;

    prepareManagerFrom(lazy);

    ui->moduleList->resizeColumnToContents(0);
    ui->moduleList->resizeColumnToContents(1);
    ui->moduleList->resizeColumnToContents(2);
    ui->moduleList->resizeColumnToContents(3);
    ui->moduleList->resizeColumnToContents(4);
    ui->moduleList->resizeColumnToContents(5);
    ui->moduleList->resizeColumnToContents(6);
    ui->moduleList->resizeColumnToContents(7);

    ui->resourcesList->resizeColumnToContents(0);
    ui->resourcesList->resizeColumnToContents(1);
    ui->resourcesList->resizeColumnToContents(2);
    ui->resourcesList->resizeColumnToContents(3);


    ui->connectionList->resizeColumnToContents(0);
    ui->connectionList->resizeColumnToContents(1);
    ui->connectionList->resizeColumnToContents(2);
    ui->connectionList->resizeColumnToContents(3);
    ui->connectionList->resizeColumnToContents(4);
    ui->connectionList->resizeColumnToContents(5);


    connect(ui->moduleList,SIGNAL(itemSelectionChanged()),this,SLOT(onModuleItemSelectionChanged()));
    connect(ui->moduleList,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(onItemDoubleClicked(QTreeWidgetItem*,int)));

    connect(ui->resourcesList,SIGNAL(itemSelectionChanged()),this,SLOT(onResourceItemSelectionChanged()));
    connect(ui->resourcesList,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(onItemDoubleClicked(QTreeWidgetItem*,int)));

    connect(ui->connectionList,SIGNAL(itemSelectionChanged()),this,SLOT(onConnectionItemSelectionChanged()));
    connect(ui->connectionList,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(onItemDoubleClicked(QTreeWidgetItem*,int)));

    connect(this,SIGNAL(selfSafeLoadBolance()),this,SLOT(onSelfSafeLoadBalance()),Qt::QueuedConnection);
    connect(this,SIGNAL(selfConnect(int)),this,SLOT(onSelfConnect(int)),Qt::QueuedConnection);
    connect(this,SIGNAL(selfDisconnect(int)),this,SLOT(onSelfDisconnect(int)),Qt::QueuedConnection);
    connect(this,SIGNAL(selfResAvailable(int)),this,SLOT(onSelfResAvailable(int)),Qt::QueuedConnection);
    connect(this,SIGNAL(selfResUnavailable(int)),this,SLOT(onSelfResUnavailable(int)),Qt::QueuedConnection);
    connect(this,SIGNAL(selfStart(int)),this,SLOT(onSelfStart(int)),Qt::QueuedConnection);
    connect(this,SIGNAL(selfStop(int)),this,SLOT(onSelfStop(int)),Qt::QueuedConnection);
    createModulesViewContextMenu();
    createConnectionsViewContextMenu();
    createResourcesViewContextMenu();



}

ApplicationViewWidget::~ApplicationViewWidget()
{
    delete ui;
}

/*! \brief Create the context menu for the modules tree. */
void ApplicationViewWidget::createModulesViewContextMenu()
{
    modRunAction = new QAction("Run",this);
    modStopAction = new QAction("Stop",this);
    modkillAction = new QAction("Kill",this);
    modSeparator = new QAction(this);
    modSeparator->setSeparator(true);
    modRefreshAction = new QAction("Refresh Status",this);
    modSelectAllAction = new QAction("Select All Modules",this);
    modAttachAction = new QAction("Attach to stdout",this);
    modAssignAction = new QAction("Assign Hosts",this);

    modRunAction->setIcon(QIcon(":/images/run_ico.png"));
    modStopAction->setIcon(QIcon(":/images/stop_ico.png"));
    modkillAction->setIcon(QIcon(":/images/kill_ico.png"));
    modRefreshAction->setIcon(QIcon(":/images/progress_ico.png"));
    modSelectAllAction->setIcon(QIcon(":/images/selectall_ico.png"));
    modAssignAction->setIcon(QIcon(":/images/yesres_ico.png"));


    ui->moduleList->addAction(modRunAction);
    ui->moduleList->addAction(modStopAction);
    ui->moduleList->addAction(modkillAction);
    ui->moduleList->addAction(modSeparator);
    ui->moduleList->addAction(modRefreshAction);
    ui->moduleList->addAction(modSelectAllAction);
    ui->moduleList->addAction(modAttachAction);
    ui->moduleList->addAction(modAssignAction);

    connect(modRunAction,SIGNAL(triggered()),this,SLOT(onRun()));
    connect(modStopAction,SIGNAL(triggered()),this,SLOT(onStop()));
    connect(modkillAction,SIGNAL(triggered()),this,SLOT(onKill()));
    connect(modRefreshAction,SIGNAL(triggered()),this,SLOT(onRefresh()));
    connect(modSelectAllAction,SIGNAL(triggered()),this,SLOT(selectAllModule()));
    connect(modAttachAction,SIGNAL(triggered()),this,SLOT(onAttachStdout()));
    connect(modAssignAction,SIGNAL(triggered()),this,SLOT(onAssignHost()));
}

/*! \brief Create the context menu for the connections tree. */
void ApplicationViewWidget::createConnectionsViewContextMenu()
{
    connContex = new QMenu(ui->connectionList);
    connSubMenu =  new QMenu("Inspect",connContex);

    connConnectAction = new QAction("Connect",connContex);
    connDisconnectAction = new QAction("Disconnect",connContex);
    connSeparatorAction = new QAction(connContex);
    connSeparatorAction->setSeparator(true);
    connRefreshAction = new QAction("Refresh Status",connContex);
    connSelectAllAction = new QAction("Select All Connections",connContex);
    conn1SeparatorAction = new QAction(connContex);
    conn1SeparatorAction->setSeparator(connContex);

    connYARPViewAction = new QAction("yarpview",connSubMenu);
    connYARPReadAction = new QAction("yarpread",connSubMenu);
    connYARPHearAction = new QAction("yarphear",connSubMenu);
    connYARPScopeAction = new QAction("yarpscope",connSubMenu);

    connSelectAllAction->setIcon(QIcon(":/images/selectall_ico.png"));
    connRefreshAction->setIcon(QIcon(":/images/progress_ico.png"));
    connConnectAction->setIcon(QIcon(":/images/connect_ico.png"));
    connDisconnectAction->setIcon(QIcon(":/images/disconnect_ico.png"));

    connect(connConnectAction,SIGNAL(triggered()),this,SLOT(onConnect()));
    connect(connDisconnectAction,SIGNAL(triggered()),this,SLOT(onDisconnect()));
    connect(connRefreshAction,SIGNAL(triggered()),this,SLOT(onRefresh()));
    connect(connSelectAllAction,SIGNAL(triggered()),this,SLOT(selectAllConnections()));

    connect(connYARPViewAction,SIGNAL(triggered()),this,SLOT(onYARPView()));
    connect(connYARPReadAction,SIGNAL(triggered()),this,SLOT(onYARPRead()));
    connect(connYARPHearAction,SIGNAL(triggered()),this,SLOT(onYARPHear()));
    connect(connYARPScopeAction,SIGNAL(triggered()),this,SLOT(onYARPScope()));

    connContex->addAction(connConnectAction);
    connContex->addAction(connDisconnectAction);
    connContex->addAction(connSeparatorAction);
    connContex->addAction(connRefreshAction);
    connContex->addAction(connSelectAllAction);
    connContex->addAction(conn1SeparatorAction);
    connContex->addMenu(connSubMenu);

    connSubMenu->addAction(connYARPViewAction);
    connSubMenu->addAction(connYARPReadAction);
    connSubMenu->addAction(connYARPHearAction);
    connSubMenu->addAction(connYARPScopeAction);


    ui->connectionList->setContextMenu(connContex);

}

/*! \brief Create the context menu for the resources tree. */
void ApplicationViewWidget::createResourcesViewContextMenu()
{
    resRefreshAction = new QAction("Refresh Status", this);
    resSelectAllAction = new QAction("Select All Resources", this);

    resRefreshAction->setIcon(QIcon(":/images/progress_ico.png"));
    resSelectAllAction->setIcon(QIcon(":/images/selectall_ico.png"));

    ui->resourcesList->addAction(resRefreshAction);
    ui->resourcesList->addAction(resSelectAllAction);

    connect(resRefreshAction,SIGNAL(triggered()),this,SLOT(onRefresh()));
    connect(resSelectAllAction,SIGNAL(triggered()),this,SLOT(selectAllResources()));
}

/*! \brief Called when an item of the connections tree has been selected. */
void ApplicationViewWidget::onConnectionItemSelectionChanged()
{
    if(ui->connectionList->currentItem() == NULL){
        connConnectAction->setEnabled(false);
        connDisconnectAction->setEnabled(false);
        connSeparatorAction->setEnabled(false);
        connRefreshAction->setEnabled(false);
    }else{
        connConnectAction->setEnabled(true);
        connDisconnectAction->setEnabled(true);
        connSeparatorAction->setEnabled(true);
        connRefreshAction->setEnabled(true);
    }
}

/*! \brief Called when an item of the modules tree has been selected. */
void ApplicationViewWidget::onModuleItemSelectionChanged()
{
    if(ui->moduleList->currentItem() == NULL){
        modRunAction->setEnabled(false);
        modStopAction->setEnabled(false);
        modkillAction->setEnabled(false);
        modAttachAction->setEnabled(false);
        modAssignAction->setEnabled(false);
        modRefreshAction->setEnabled(false);
    }else{
        modRunAction->setEnabled(true);
        modStopAction->setEnabled(true);
        modkillAction->setEnabled(true);
        modAttachAction->setEnabled(true);
        modAssignAction->setEnabled(true);
        modRefreshAction->setEnabled(true);
    }
}

/*! \brief Called when an item of the resources tree has been selected. */
void ApplicationViewWidget::onResourceItemSelectionChanged()
{
    if(ui->resourcesList->currentItem() == NULL){
        resRefreshAction->setEnabled(false);
    }else{
        resRefreshAction->setEnabled(true);
    }
}

void ApplicationViewWidget::prepareManagerFrom(yarp::manager::Manager* lazy)
{

    manager.prepare(lazy, m_pConfig,dynamic_cast<ApplicationEvent*>(this));

    // loading application
    if(manager.loadApplication(app->getName())){
       updateApplicationWindow();
    }

    reportErrors();
}

/*! \brief Refresh the widget. */
void ApplicationViewWidget::updateApplicationWindow()
{
    ui->moduleList->clear();
    ui->connectionList->clear();
    ui->resourcesList->clear();

    yarp::manager::ExecutablePContainer modules = manager.getExecutables();
    yarp::manager::CnnContainer connections  = manager.getConnections();
    yarp::manager::ExecutablePIterator moditr;
    yarp::manager::CnnIterator cnnitr;

    int id = 0;
    for(moditr=modules.begin(); moditr<modules.end(); moditr++)
    {

        QString id = QString("%1").arg((*moditr)->getID());
        QString command = QString("%1").arg((*moditr)->getCommand());
        QString host = QString("%1").arg((*moditr)->getHost());
        QString param = QString("%1").arg((*moditr)->getParam());
        QString stdio = QString("%1").arg((*moditr)->getStdio());
        QString workDir = QString("%1").arg((*moditr)->getWorkDir());
        QString env = QString("%1").arg((*moditr)->getEnv());

        QStringList l;
        l << command << id << "stopped" << host << param << stdio << workDir << env;
        CustomTreeWidgetItem *it = new CustomTreeWidgetItem(ui->moduleList,l);

        if (host=="localhost")
        {
            it->setTextColor(3,QColor("#A0A0A0"));
        }

        //it->setFlags(it->flags() | Qt::ItemIsEditable);
        it->setData(0,Qt::UserRole,yarp::manager::MODULE);
        it->setIcon(0,QIcon(":/images/suspended_ico.png"));
        it->setTextColor(2,QColor("#BF0303"));
        ui->moduleList->addTopLevelItem(it);
    }

    id = 0;
    for(cnnitr=connections.begin(); cnnitr<connections.end(); cnnitr++)
    {
        QString type;
        if((*cnnitr).isPersistent())
            type = "Persistent";
        else
        {
            if((*cnnitr).isExternalFrom() || (*cnnitr).isExternalTo())
                type = "External";
            else
                type = "Internal";
        }
        QString sId = QString("%1").arg(id);
        QString from = QString("%1").arg((*cnnitr).from());
        QString to = QString("%1").arg((*cnnitr).to());
        QString carrier = QString("%1").arg((*cnnitr).carrier());
        QString status = "disconnected";


        QStringList l;
        l << type << sId << status << from << to << carrier;
        CustomTreeWidgetItem *it = new CustomTreeWidgetItem(ui->connectionList,l);
        ui->moduleList->addTopLevelItem(it);
        it->setData(0,Qt::UserRole,yarp::manager::INOUTD);
        it->setIcon(0,QIcon(":/images/disconnected_ico.png"));
        it->setTextColor(2,QColor("#BF0303"));
        id++;
    }

    id = 0;
    yarp::manager::ResourcePIterator itrS;
    for(itrS=manager.getResources().begin(); itrS!=manager.getResources().end(); itrS++)
    {
        //m_resRow = *(m_refTreeResModel->append());
        QString sId = QString("%1").arg(id);
        QString type;
        if(dynamic_cast<yarp::manager::Computer*>(*itrS))
        {
            type = "computer";
        }
        else if(dynamic_cast<yarp::manager::ResYarpPort*>(*itrS))
        {
            type = "port";
        }
        QString res = QString("%1").arg((*itrS)->getName());
        QString status = "unknown";
        //m_resRow[m_resColumns.m_col_color] = Gdk::Color("#00000");

        QStringList l;
        l << res << sId << type << status ;
        CustomTreeWidgetItem *it = new CustomTreeWidgetItem(ui->resourcesList,l);
        ui->moduleList->addTopLevelItem(it);
        it->setData(0,Qt::UserRole,yarp::manager::RESOURCE);
        if(type == "computer"){
            it->setIcon(0,QIcon(":/images/nores_ico.png"));
            it->setTextColor(3,QColor("#BF0303"));
        }else{
            it->setIcon(0,QIcon(":/images/port_unavail_ico.png"));
            it->setTextColor(3,QColor("#BF0303"));
        }
        id++;
    }

}

/*! \brief Called when an item has been double clicked */
void ApplicationViewWidget::onItemDoubleClicked(QTreeWidgetItem *it,int col)
{
    Qt::ItemFlags tmp = it->flags();
    if (isEditable(it, col)) {
        it->setFlags(tmp | Qt::ItemIsEditable);
    } else if (tmp & Qt::ItemIsEditable) {
        it->setFlags(tmp ^ Qt::ItemIsEditable);
    }
}

/*! \brief Return the editable state of an item
    \param it The QtreeWidgetItem clicked
    \param col the column
*/
bool ApplicationViewWidget::isEditable(QTreeWidgetItem *it,int col)
{
    yarp::manager::NodeType type = (yarp::manager::NodeType)it->data(0,Qt::UserRole).toInt();

    switch (type) {
    case yarp::manager::MODULE:{
           if(col == 4 || col == 5 || col == 6 || col == 7){
               if(it->text(2) == "stopped"){
                    return true;
               }
           }
           if(col == 3){
                if(it->text(3) != "localhost" && it->text(2) == "stopped"){
                    return true;
                }
           }
        break;
    }
    case yarp::manager::INOUTD:{
           if(col == 3 || col == 4 || col == 5 ){
               if(it->text(2) == "disconnected"){
                    return true;
               }
           }
        break;
    }
    case yarp::manager::RESOURCE:{
           return false;
        break;
    }
    default:
        break;
    }

    return false;
}

void ApplicationViewWidget::onCloseStdOut(int id)
{
    for(int i=0;i<ui->moduleList->topLevelItemCount();i++){
        QTreeWidgetItem *it = ui->moduleList->topLevelItem(i);
        if(it->text(1).toInt() == id){
            StdoutWindow *stdouWin = qvariant_cast<StdoutWindow *>(it->data(0,Qt::UserRole));
            if(stdouWin && stdouWin->getId() == id){
                delete stdouWin;
                it->setData(0,Qt::UserRole,QVariant::fromValue(NULL));
                return;
            }
        }
    }
}

bool ApplicationViewWidget::areAllShutdown()
{
    for(int i=0;i<ui->moduleList->topLevelItemCount();i++){
        QTreeWidgetItem *it = ui->moduleList->topLevelItem(i);
        if(it->text(2) != "stopped"){
            return false;
        }
    }

    for(int i=0;i<ui->connectionList->topLevelItemCount();i++){
        QTreeWidgetItem *it = ui->connectionList->topLevelItem(i);
        if(it->text(2) != "disconnected"){
            return false;
        }
    }

    return true;
}

void ApplicationViewWidget::onSelfSafeLoadBalance()
{
    updateApplicationWindow();
    reportErrors();
}

void ApplicationViewWidget::onAssignHost()
{
    if(areAllShutdown() && !manager.busy()){
        for(int i=0;i<ui->moduleList->topLevelItemCount();i++){
            QTreeWidgetItem *it = ui->moduleList->topLevelItem(i);
            if(it->isSelected()){
                //QString waitHost = QString("%1").arg("?");
                it->setText(2,"waiting");
                it->setText(3,"?");
            }

        }
        manager.safeLoadBalance();
        yarp::os::Time::delay(0.1);

        /*typedef Gtk::TreeModel::Children type_children;
        type_children children = m_refTreeModModel->children();
        for(type_children::iterator iter = children.begin(); iter!=children.end(); ++iter)
        {
            Gtk::TreeModel::Row row = (*iter);
            row[m_modColumns.m_col_status] = "waiting";
            row[m_modColumns.m_col_editable] = false;
            row[m_modColumns.m_col_host] = "?";
            row[m_modColumns.m_col_color] = Gdk::Color("#000000");

            row.set_value(0, m_refPixWaiting);
        }

        manager.safeLoadBalance();
        */
    }else{
        yarp::manager::ErrorLogger* logger  = yarp::manager::ErrorLogger::Instance();
        logger->addError("Running modules should be stopped before assigning hosts.");
        reportErrors();
    }
}

void ApplicationViewWidget::onAttachStdout()
{
    if(manager.busy()){
        return;
    }

    std::vector<int> MIDs;
    for(int i=0;i<ui->moduleList->topLevelItemCount();i++){
        QTreeWidgetItem *it = ui->moduleList->topLevelItem(i);
        if(it->isSelected()){
            bool found = false;
            int id = it->text(1).toInt();

            StdoutWindow *stdouWin = qvariant_cast<StdoutWindow *>(it->data(0,Qt::UserRole));
            if(stdouWin && stdouWin->getId() == id){
                found = true;
                continue;
            }

            MIDs.push_back(it->text(1).toInt());

            QString name = QString("%1").arg(app->getName());
            QString strTitle = name + ":" + it->text(0) + ":" + it->text(1);

            StdoutWindow *stdOutWindow = new StdoutWindow(id,strTitle);
            connect(stdOutWindow,SIGNAL(closeStdOut(int)),this,SLOT(onCloseStdOut(int)));
            //stdoutWinList.append(stdOutWindow);
            it->setData(0,Qt::UserRole,QVariant::fromValue(stdOutWindow));
            stdOutWindow->show();
        }

    }
    manager.safeAttachStdout(MIDs);

    //return true;
}

/*! \brief Called when the Run button has been pressed */
bool ApplicationViewWidget::onRun()
{
    if(manager.busy()){
        return false;
    }

    std::vector<int> MIDs;
    for(int i=0;i<ui->moduleList->topLevelItemCount();i++){
        QTreeWidgetItem *it = ui->moduleList->topLevelItem(i);
        if(it->isSelected()){
            MIDs.push_back(it->text(1).toInt());
            manager.updateExecutable(it->text(1).toInt(),
                                     it->text(4).toLatin1().data(),
                                     it->text(3).toLatin1().data(),
                                     it->text(5).toLatin1().data(),
                                     it->text(6).toLatin1().data(),
                                     it->text(7).toLatin1().data());

            it->setText(2,"waiting");
            it->setIcon(0,QIcon(":/images/refresh_ico.png"));
            it->setTextColor(2,QColor("#000000"));

        }
    }


    manager.safeRun(MIDs);
    yarp::os::Time::delay(0.1);
    selectAllModule(false);
    return true;
}

/*! \brief Called when the Stop button has been pressed */
bool ApplicationViewWidget::onStop()
{
    if(manager.busy()){
        return false;
    }

    std::vector<int> MIDs;
    for(int i=0;i<ui->moduleList->topLevelItemCount();i++){
        QTreeWidgetItem *it = ui->moduleList->topLevelItem(i);
        if(it->isSelected()){
            //moduluesIDs.append(it->text(1).toInt());
            MIDs.push_back(it->text(1).toInt());
            manager.updateExecutable(it->text(1).toInt(),
                                     it->text(4).toLatin1().data(),
                                     it->text(3).toLatin1().data(),
                                     it->text(5).toLatin1().data(),
                                     it->text(6).toLatin1().data(),
                                     it->text(7).toLatin1().data());

            it->setText(2,"waiting");
            it->setIcon(0,QIcon(":/images/refresh_ico.png"));
            it->setTextColor(2,QColor("#000000"));

        }
    }


    manager.safeStop(MIDs);
    yarp::os::Time::delay(0.1);
    selectAllModule(false);
    return true;
}

/*! \brief Called when the Kill button has been pressed */
bool ApplicationViewWidget::onKill()
{
    if(manager.busy()){
        return false;
    }

    if(QMessageBox::question(this,"Killing modules!","Are you sure?") != QMessageBox::Yes){
        return true;
    }

    std::vector<int> MIDs;
    for(int i=0;i<ui->moduleList->topLevelItemCount();i++){
        QTreeWidgetItem *it = ui->moduleList->topLevelItem(i);
        if(it->isSelected()){
            //moduluesIDs.append(it->text(1).toInt());
            MIDs.push_back(it->text(1).toInt());
            manager.updateExecutable(it->text(1).toInt(),
                                     it->text(4).toLatin1().data(),
                                     it->text(3).toLatin1().data(),
                                     it->text(5).toLatin1().data(),
                                     it->text(6).toLatin1().data(),
                                     it->text(7).toLatin1().data());

            it->setText(2,"waiting");
            it->setIcon(0,QIcon(":/images/progress_ico.png"));
            it->setTextColor(2,QColor("#000000"));

        }
    }


    manager.safeKill(MIDs);
    yarp::os::Time::delay(0.1);
    selectAllModule(false);
    return true;
}

/*! \brief Called when the Conenct button has been pressed */
bool ApplicationViewWidget::onConnect()
{
    if(manager.busy()){
        return false;
    }


    std::vector<int> MIDs;
    for(int i=0;i<ui->connectionList->topLevelItemCount();i++){
        QTreeWidgetItem *it = ui->connectionList->topLevelItem(i);
        if(it->isSelected()){
            MIDs.push_back(it->text(1).toInt());
            manager.updateConnection(it->text(1).toInt(),
                                     it->text(3).toLatin1().data(),
                                     it->text(4).toLatin1().data(),
                                     it->text(5).toLatin1().data());

            it->setText(2,"waiting");
            it->setIcon(0,QIcon(":/images/progress_ico.png"));
            it->setTextColor(2,QColor("#000000"));
        }
    }


    manager.safeConnect(MIDs);
    yarp::os::Time::delay(0.1);
    selectAllConnections(false);
    return true;
}

/*! \brief Called when the Disconnect button has been pressed */
bool ApplicationViewWidget::onDisconnect()
{
    if(manager.busy()){
        return false;
    }


    std::vector<int> MIDs;
    for(int i=0;i<ui->connectionList->topLevelItemCount();i++){
        QTreeWidgetItem *it = ui->connectionList->topLevelItem(i);
        if(it->isSelected()){
            MIDs.push_back(it->text(1).toInt());
            manager.updateConnection(it->text(1).toInt(),
                                     it->text(3).toLatin1().data(),
                                     it->text(4).toLatin1().data(),
                                     it->text(5).toLatin1().data());

            it->setText(2,"waiting");
            it->setIcon(0,QIcon(":/images/progress_ico.png"));
            it->setTextColor(2,QColor("#000000"));

        }
    }


    manager.safeDisconnect(MIDs);
    yarp::os::Time::delay(0.1);
    selectAllConnections(false);
    return true;
}

/*! \brief Called when the Refresh button has been pressed */
bool ApplicationViewWidget::onRefresh()
{
    if(manager.busy()){
        return false;
    }

    std::vector<int> modulesIDs;
    std::vector<int> connectionsIDs;
    std::vector<int> resourcesIDs;

    for(int i=0;i<ui->moduleList->topLevelItemCount();i++){
        QTreeWidgetItem *it = ui->moduleList->topLevelItem(i);
        if(it->isSelected()){
            modulesIDs.push_back(it->text(1).toInt());
            it->setText(2,"waiting");
            it->setIcon(0,QIcon(":/images/progress_ico.png"));
            it->setTextColor(2,QColor("#000000"));
        }
    }


    for(int i=0;i<ui->connectionList->topLevelItemCount();i++){
        QTreeWidgetItem *it = ui->connectionList->topLevelItem(i);
        if(it->isSelected()){
            connectionsIDs.push_back(it->text(1).toInt());
            it->setText(2,"waiting");
            it->setIcon(0,QIcon(":/images/progress_ico.png"));
            it->setTextColor(2,QColor("#000000"));
        }
    }

    for(int i=0;i<ui->resourcesList->topLevelItemCount();i++){
        QTreeWidgetItem *it = ui->resourcesList->topLevelItem(i);
        if(it->isSelected()){
            resourcesIDs.push_back(it->text(1).toInt());
            it->setText(3,"waiting");
            it->setIcon(0,QIcon(":/images/progress_ico.png"));
            it->setTextColor(3,QColor("#000000"));
        }
    }

    manager.safeRefresh(modulesIDs,
                        connectionsIDs,
                        resourcesIDs);
    yarp::os::Time::delay(0.1);
    selectAllConnections(false);
    selectAllModule(false);
    selectAllResources(false);

    return true;
}

/*! \brief Select all modules */
void ApplicationViewWidget::selectAllModule()
{
    selectAllModule(true);
}

/*! \brief Select all connections */
void ApplicationViewWidget::selectAllConnections()
{
    selectAllConnections(true);
}

/*! \brief Select all resources */
void ApplicationViewWidget::selectAllResources()
{
   selectAllResources(true);
}

/*! \brief Select/deselect all modules
    \param check
*/
void ApplicationViewWidget::selectAllModule(bool check)
{
    for(int i=0;i<ui->moduleList->topLevelItemCount();i++){
        QTreeWidgetItem *it = ui->moduleList->topLevelItem(i);
        it->setSelected(check);
    }
}

/*! \brief Select/deselect all connections
    \param check
*/
void ApplicationViewWidget::selectAllConnections(bool check)
{
    for(int i=0;i<ui->connectionList->topLevelItemCount();i++){
        QTreeWidgetItem *it = ui->connectionList->topLevelItem(i);
        it->setSelected(check);
    }
}

/*! \brief Select/deselect all resources
    \param check
*/
void ApplicationViewWidget::selectAllResources(bool check)
{
    for(int i=0;i<ui->resourcesList->topLevelItemCount();i++){
        QTreeWidgetItem *it = ui->resourcesList->topLevelItem(i);
        it->setSelected(check);
    }
}

/*! \brief Select all element in the widget (modules, connections, resources)
*/
void ApplicationViewWidget::selectAll()
{
    selectAllConnections(true);
    selectAllModule(true);
    selectAllResources(true);
}

/*! \brief Run all modules in the application */
void ApplicationViewWidget::runApplicationSet()
{
    selectAllModule(true);
    onRun();
}

/*! \brief Stop all modules in the application */
void ApplicationViewWidget::stopApplicationSet()
{
    selectAllModule(true);
    onStop();
}

/*! \brief Kill all running modules in the application */
void ApplicationViewWidget::killApplicationSet()
{
    selectAllModule(true);
    onKill();
}

/*! \brief Connect all modules in the application to their ports using connections list*/
void ApplicationViewWidget::connectConnectionSet()
{
    selectAllConnections(true);
    onConnect();
}

/*! \brief Disconnect all modules in the application to their ports using connections list*/
void ApplicationViewWidget::disconnectConnectionSet()
{
    selectAllConnections(true);
    onDisconnect();
}

/*! \brief Refresh all*/
void ApplicationViewWidget::refresh()
{
    selectAllConnections(true);
    selectAllModule(true);
    selectAllResources(true);

    onRefresh();
}

/*! \brief Report all errors*/
void ApplicationViewWidget::reportErrors()
{
    yarp::manager::ErrorLogger* logger  = yarp::manager::ErrorLogger::Instance();
    if(logger->errorCount() || logger->warningCount())
    {
        const char* err;
        while((err=logger->getLastError()))
        {
            QString msg = QString("(%1) %2").arg(app->getName()).arg(err);
            logError(msg);
        }

        while((err=logger->getLastWarning()))
        {
            QString msg = QString("(%1) %2").arg(app->getName()).arg(err);
            logWarning(msg);
        }
    }
}


/*! \brief Export the current Graph*/
void ApplicationViewWidget::exportGraph()
{
    QString fileName = QFileDialog::getSaveFileName(this,"Export Graph",QApplication::applicationDirPath(),"GraphViz format (*.dot)");

    if(!fileName.isEmpty()){
        if(!manager.exportDependencyGraph(fileName.toLatin1().data()))
        {
            yarp::manager::ErrorLogger* logger  = yarp::manager::ErrorLogger::Instance();
            logger->addError("Cannot export graph");
            reportErrors();
            return;
        }
    }
}


/*! \brief Launch YARPView Inspection modality*/
void ApplicationViewWidget::onYARPView()
{
    if(manager.busy()){
        return;
    }
    yarp::manager::ErrorLogger* logger  = yarp::manager::ErrorLogger::Instance();

    for(int i=0;i<ui->connectionList->topLevelItemCount();i++){
        QTreeWidgetItem *it = ui->connectionList->topLevelItem(i);
        if(it->isSelected()){
            QString from = it->text(3);
            QString to = QString("/inspect").arg(from);
            QString env = QString("YARP_PORT_PREFIX=%1").arg(to);
            to += "/yarpview/img:i";

            yarp::manager::LocalBroker launcher;
            if(launcher.init("yarpview", NULL, NULL, NULL, NULL, env.toLatin1().data()))
            {
                if(!launcher.start() && strlen(launcher.error()))
                {
                    QString msg;
                    msg = QString("Error while launching yarpview. %1").arg(launcher.error());
                    logger->addError(msg.toLatin1().data());
                    reportErrors();
                }
                else
                {
                    // waiting for the port to get open
                    double base = yarp::os::Time::now();
                    while(!timeout(base, 3.0)){
                        if(launcher.exists(to.toLatin1().data())){
                            break;
                        }
                    }
                    if(!launcher.connect(from.toLatin1().data(), to.toLatin1().data(), "udp")){
                        QString msg;
                        msg = QString("Cannot inspect '%1' : %2").arg(from).arg(launcher.error());
                        logger->addError(msg.toLatin1().data());
                        launcher.stop();
                        reportErrors();
                    }
                }
            }
        }
    }


    yarp::os::Time::delay(0.1);
}

/*! \brief Launch YARPHear Inspection modality*/
void ApplicationViewWidget::onYARPHear()
{
    if(manager.busy()){
        return;
    }
    yarp::manager::ErrorLogger* logger  = yarp::manager::ErrorLogger::Instance();

    for(int i=0;i<ui->connectionList->topLevelItemCount();i++){
        QTreeWidgetItem *it = ui->connectionList->topLevelItem(i);
        if(it->isSelected()){
            QString from = it->text(3);
            QString to = QString("/inspect/hear%1").arg(from);

#if defined(WIN32)
            QString cmd = "cmd.exe";
            QString param;
            param = QString("/C yarphear --name %1").arg(to);

#else
            QString cmd = "xterm";
            QString param;
            param = QString("-hold -title %1 -e yarphear --nodevice --name %2").arg(from).arg(to);
#endif

            yarp::manager::LocalBroker launcher;
            launcher.showConsole(true);
            if(launcher.init(cmd.toLatin1().data(), param.toLatin1().data(), NULL, NULL, NULL, NULL))
            {
                if(!launcher.start() && strlen(launcher.error()))
                {
                    QString  msg;
                    msg = QString("Error while launching yarpread. %1").arg(launcher.error());
                    logger->addError(msg.toLatin1().data());
                    reportErrors();
                }
                else
                {
                    // waiting for the port to get open
                    double base = yarp::os::Time::now();
                    while(!timeout(base, 3.0)){
                        if(launcher.exists(to.toLatin1().data())){
                            break;
                        }
                    }
                    if(!launcher.exists(to.toLatin1().data())){
                        QString msg;
                        msg = QString("Cannot inspect '%1' : %2. Did you build yarp with 'portaudio' module?").arg(from).arg(launcher.error());
                        logger->addError(msg.toLatin1().data());
                        launcher.stop();
                        reportErrors();
                    }else if(!launcher.connect(from.toLatin1().data(), to.toLatin1().data(), "udp")){
                        QString msg;
                        msg = QString("Cannot inspect '%1' : %2").arg(from).arg(launcher.error());
                        logger->addError(msg.toLatin1().data());
                        launcher.stop();
                        reportErrors();
                    }
                }
            }

        }
    }
    yarp::os::Time::delay(0.1);



}

/*! \brief Launch YARPRead Inspection modality*/
void ApplicationViewWidget::onYARPRead()
{
    if(manager.busy()){
        return;
    }
    yarp::manager::ErrorLogger* logger  = yarp::manager::ErrorLogger::Instance();

    for(int i=0;i<ui->connectionList->topLevelItemCount();i++){
        QTreeWidgetItem *it = ui->connectionList->topLevelItem(i);
        if(it->isSelected()){
            QString from = it->text(3);
            QString to = QString("/inspect/read%1").arg(from);

#if defined(WIN32)
            QString cmd = "cmd.exe";
            QString param;
            param = QString("/C yarp read %1").arg(to);

#else
            QString cmd = "xterm";
            QString param;
            param = QString("-hold -title %1 -e yarp read %2").arg(from).arg(to);
#endif

            yarp::manager::LocalBroker launcher;
            launcher.showConsole(true);
            if(launcher.init(cmd.toLatin1().data(), param.toLatin1().data(), NULL, NULL, NULL, NULL))
            {
                if(!launcher.start() && strlen(launcher.error()))
                {
                    QString msg;
                    msg = QString("Error while launching yarpread. %1").arg(launcher.error());
                    logger->addError(msg.toLatin1().data());
                    reportErrors();
                }
                else
                {
                    // waiting for the port to get open
                    double base = yarp::os::Time::now();
                    while(!timeout(base, 3.0)){
                        if(launcher.exists(to.toLatin1().data())){
                            break;
                        }
                    }
                    if(!launcher.connect(from.toLatin1().data(), to.toLatin1().data(), "udp")){
                        QString msg;
                        msg = QString("Cannot inspect '%1' : %2").arg(from).arg(launcher.error());
                        logger->addError(msg.toLatin1().data());
                        launcher.stop();
                        reportErrors();
                    }
                }
            }
        }
    }


    yarp::os::Time::delay(0.1);

}

/*! \brief Launch YARPScope Inspection modality*/
void ApplicationViewWidget::onYARPScope()
{
    if(manager.busy()){
        return;
    }
    yarp::manager::ErrorLogger* logger  = yarp::manager::ErrorLogger::Instance();

    YscopeWindow dlg;
    dlg.setModal(true);
    if(dlg.exec() != QDialog::Accepted){
        return;
    }
    int strIndex = dlg.getIndex();




    for(int i=0;i<ui->connectionList->topLevelItemCount();i++){
        QTreeWidgetItem *it = ui->connectionList->topLevelItem(i);
        if(it->isSelected()){
            QString from = it->text(3);
            QString to = QString("/inspect").arg(from);
            QString env = QString("YARP_PORT_PREFIX=%1").arg(to);
            to += "/yarpscope";

            QString param;
            param = QString("--title %1:%2 --bgcolor white --color blue --graph_size 2 --index %2").arg(from).arg(strIndex);


            yarp::manager::LocalBroker launcher;
            if(launcher.init("yarpscope", param.toLatin1().data(), NULL, NULL, NULL, env.toLatin1().data())){
                if(!launcher.start() && strlen(launcher.error())){
                    QString msg;
                    msg = QString("Error while launching yarpscope. %1").arg(launcher.error());
                    logger->addError(msg.toLatin1().data());
                    reportErrors();
                }
                else{
                    // waiting for the port to get open
                    double base = yarp::os::Time::now();
                    while(!timeout(base, 3.0))
                        if(launcher.exists(to.toLatin1().data())) break;
                    if(!launcher.connect(from.toLatin1().data(), to.toLatin1().data(), "udp")){
                        QString msg;
                        msg = QString("Cannot inspect '%1' : %2").arg(from).arg(launcher.error());
                        logger->addError(msg.toLatin1().data());
                        launcher.stop();
                        reportErrors();
                    }
                }
            }

        }
    }
    yarp::os::Time::delay(0.1);
}



bool ApplicationViewWidget::timeout(double base, double timeout)
{
    yarp::os::Time::delay(1.0);
    if((yarp::os::Time::now()-base) > timeout)
        return true;
    return false;
}




/*! \brief Tells if a modules is in running state*/
bool ApplicationViewWidget::isRunning()
{
    for(int i=0; i< ui->moduleList->topLevelItemCount();i++){
        if(ui->moduleList->topLevelItem(i)->text(2) == "running"){
            return true;
        }
    }
    return false;
}


void ApplicationViewWidget::onSelfConnect(int which)
{
    QTreeWidgetItem *it = ui->connectionList->topLevelItem(which);
    if(it){
        it->setText(2,"connected");
        it->setIcon(0,QIcon(":/images/connected_ico.png"));
        it->setTextColor(2,QColor("#008C00"));
    }

           /* row[m_conColumns.m_col_status] = "connected";
            row[m_conColumns.m_col_editable] = false;
            row[m_conColumns.m_col_color] = Gdk::Color("#008C00");
            row.set_value(0, m_refPixConnected);*/

    reportErrors();
}

void ApplicationViewWidget::onSelfDisconnect(int which)
{
    QTreeWidgetItem *it = ui->connectionList->topLevelItem(which);
    if(it){
        it->setText(2,"disconnected");
        it->setIcon(0,QIcon(":/images/disconnected_ico.png"));
        it->setTextColor(2,QColor("#BF0303"));
    }
    reportErrors();
}

void ApplicationViewWidget::onSelfResAvailable(int which)
{
    QTreeWidgetItem *it = ui->resourcesList->topLevelItem(which);
    if(it){
        it->setText(3,"available");
        if(it->text(2) == "computer"){
            it->setIcon(0,QIcon(":/images/computer_ico.png"));
            it->setTextColor(3,QColor("#008C00"));
        }else{
            it->setIcon(0,QIcon(":/images/port_avail_ico.png"));
            it->setTextColor(3,QColor("#008C00"));
        }
        /*if(row[m_resColumns.m_col_type] == Glib::ustring("computer"))
            row.set_value(0, m_refPixAvailable);
        else
            row.set_value(0, m_refPixPortAvaibable);*/
    }


    reportErrors();
}

void ApplicationViewWidget::onSelfResUnavailable(int which)
{
    QTreeWidgetItem *it = ui->resourcesList->topLevelItem(which);
    if(it){
        it->setText(3,"unavailable");
        if(it->text(2) == "computer"){
            it->setIcon(0,QIcon(":/images/nores_ico.png"));
            it->setTextColor(3,QColor("#BF0303"));
        }else{
            it->setIcon(0,QIcon(":/images/port_unavail_ico.png"));
            it->setTextColor(3,QColor("#BF0303"));
        }
    }
    reportErrors();
}

void ApplicationViewWidget::onSelfStart(int which)
{
    int row;
    getModRowByID(which,&row);
    QTreeWidgetItem *it = ui->moduleList->topLevelItem(row);

    if(it){
        it->setText(2,"running");
        it->setIcon(0,QIcon(":/images/runnin_ico.png"));
        it->setTextColor(2,QColor("#008C00"));
        //row[m_modColumns.m_col_editable] = false;
        //row[m_modColumns.m_col_color] = Gdk::Color("#008C00");
        //row.set_value(0, m_refPixRunning);
    }
    reportErrors();
}

void ApplicationViewWidget::onSelfStop(int which)
{
    int row;
    getModRowByID(which,&row);
    QTreeWidgetItem *it = ui->moduleList->topLevelItem(row);
    if(it){
        it->setText(2,"stopped");
        it->setIcon(0,QIcon(":/images/suspended_ico.png"));
        it->setTextColor(2,QColor("#BF0303"));
    }
    reportErrors();
}

/*! \brief Called when a modlue has been started
    \param which
*/
void ApplicationViewWidget::onModStart(int which)
{
    selfStart(which);
}

/*! \brief Called when a modlue has been stopped
    \param which
*/
void ApplicationViewWidget::onModStop(int which)
{
    selfStop(which);
}
/*! \brief Called when a modlue has writes on stdout
    \param which
    \param msg the message
*/
void ApplicationViewWidget::onModStdout(int which, const char* msg)
{
    for(int j=0;j<ui->moduleList->topLevelItemCount();j++){
        //StdoutWindow *stdouWin = stdoutWinList.at(j);
        QTreeWidgetItem *it = ui->moduleList->topLevelItem(j);
        StdoutWindow *stdouWin = qvariant_cast<StdoutWindow *>(it->data(0,Qt::UserRole));
        if(stdouWin && stdouWin->getId() == which){
            QString s = QString("%1").arg(msg);
            stdouWin->addMessage(s);
            break;
        }
    }
}

/*! \brief Called when a connection has been performed
    \param which
*/
void ApplicationViewWidget::onConConnect(int which)
{
   selfConnect(which);

}

/*! \brief Called when a disconnection has been performed
    \param which
*/
void ApplicationViewWidget::onConDisconnect(int which)
{
    selfDisconnect(which);
}

/*! \brief Called when a resource became avaible
    \param which
*/
void ApplicationViewWidget::onResAvailable(int which)
{

    selfResAvailable(which);
}

/*! \brief Called when a resource become unavaible
    \param which
*/
void ApplicationViewWidget::onResUnAvailable(int which)
{
    selfResUnavailable(which);
}

/*! \brief Called when a connection become avaible
    \param which
*/
void ApplicationViewWidget::onConAvailable(int from, int to)
{
    if(from >= 0){
        int row;
        if(getConRowByID(from, &row))
            ui->connectionList->topLevelItem(row)->setTextColor(3,QColor("#008C00"));
    }

    if(to >= 0){
        int row;
        if(getConRowByID(to, &row))
            ui->connectionList->topLevelItem(row)->setTextColor(4,QColor("#008C00"));
    }
    reportErrors();
}

/*! \brief Called when a connection become unavaible
    \param which
*/
void ApplicationViewWidget::onConUnAvailable(int from, int to)
{
    if(from >= 0){
        int row;
        if(getConRowByID(from, &row))
            ui->connectionList->topLevelItem(row)->setTextColor(3,QColor("#BF0303"));
    }

    if(to >= 0){
        int row;
        if(getConRowByID(to, &row))
            ui->connectionList->topLevelItem(row)->setTextColor(4,QColor("#BF0303"));
    }
    reportErrors();
}

/*! \brief Called when an error occurred

*/
void ApplicationViewWidget::onError(void)
{
    reportErrors();
}

/*! \brief Refresh all and reports errors
*/
void ApplicationViewWidget::onLoadBalance(void)
{
    selfSafeLoadBolance();
}



void ApplicationViewWidget::onDetachStdout(void)
{

}

/*! \brief Get the connection row by id
    \param id the requested id
    \param the output row
*/
bool ApplicationViewWidget::getConRowByID(int id, int *row)
{
    for(int i=0;i< ui->connectionList->topLevelItemCount();i++){
        QTreeWidgetItem *it = ui->connectionList->topLevelItem(i);

        if(it->text(1).toInt() == id){
            *row = i;
            return true;
        }
    }
    return false;
}
/*! \brief Get the Module row by id
    \param id the requested id
    \param the output row
*/
bool ApplicationViewWidget::getModRowByID(int id, int *row)
{
    for(int i=0;i< ui->moduleList->topLevelItemCount();i++){
        QTreeWidgetItem *it = ui->moduleList->topLevelItem(i);

        if(it->text(1).toInt() == id){
            *row = i;
            return true;
        }
    }
    return false;
}
