/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "applicationviewwidget.h"
#include "ui_applicationviewwidget.h"
#include <QMessageBox>
#include <QMenu>
#include <QFileDialog>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Carriers.h>
#include <yarp/manager/localbroker.h>
#include "yscopewindow.h"
#include <QTreeWidgetItem>
#include <QMainWindow>
#include <QMdiArea>


ApplicationViewWidget::ApplicationViewWidget(yarp::manager::Application *app,
                                             yarp::manager::Manager *lazyManager,
                                             yarp::os::Property* config,
                                             bool editingMode,
                                             QWidget *parent) :
    GenericViewWidget(parent), ApplicationEvent(),
    ui(new Ui::ApplicationViewWidget),
    modRunAction(nullptr),
    modStopAction(nullptr),
    modkillAction(nullptr),
    modSeparator(nullptr),
    modRefreshAction(nullptr),
    modSelectAllAction(nullptr),
    modAttachAction(nullptr),
    modAssignAction(nullptr),
    connContex(nullptr),
    connSubMenu(nullptr),
    connConnectAction(nullptr),
    connDisconnectAction(nullptr),
    connSeparatorAction(nullptr),
    connRefreshAction(nullptr),
    connSelectAllAction(nullptr),
    conn1SeparatorAction(nullptr),
    connInspectAction(nullptr),
    connYARPViewAction(nullptr),
    connYARPReadAction(nullptr),
    connYARPHearAction(nullptr),
    connYARPScopeAction(nullptr),
    resRefreshAction(nullptr),
    resSelectAllAction(nullptr)
{
    ui->setupUi(this);
    lazy = lazyManager;
    this->app = app;
    m_pConfig = config;
    this->editingMode = editingMode;
    this->m_modified = false;
    type = yarp::manager::APPLICATION;

    QGridLayout *l;

    if (!editingMode) {
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

        l = new QGridLayout(ui->builderContainer);
        ui->builderContainer->setLayout(l);


    } else {
        layout()->removeWidget(ui->mainSplietter);
        delete ui->mainSplietter;
        l = (QGridLayout*)layout();

    }


    l->setMargin(0);


    builderWindowContainer = new QMainWindow(nullptr,Qt::Widget);
    builderWindowContainer->setDockNestingEnabled(true);
    l->addWidget(builderWindowContainer);

    builderWidget = new QDockWidget("YARP Builder",builderWindowContainer);
    builderWindowContainer->addDockWidget(Qt::TopDockWidgetArea,builderWidget);
    builder = YarpBuilderLib::getBuilder(this->app,lazyManager,&safeManager,editingMode);
    builderWidget->setWidget(builder);
    //builderWidget->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    builderWidget->setFeatures(QDockWidget::NoDockWidgetFeatures);
    connect(builderWidget,SIGNAL(topLevelChanged(bool)),this,SLOT(onBuilderFloatChanged(bool)));

    //builderWidget->pos()

    if (!editingMode) {
        //builder->addAction(modRunAction);
        //builder->addAction(modStopAction);
        //builder->addAction(modkillAction);
        //builder->addAction(modSeparator);
        //builder->addAction(modRefreshAction);
        builder->addAction(modSelectAllAction);
        //builder->addAction(modAttachAction);
        //builder->addAction(modAssignAction);
        builder->addAction(modSeparator);
        //builder->addAction(connConnectAction);
        //builder->addAction(connDisconnectAction);
        //builder->addAction(connSeparatorAction);
        //builder->addAction(connRefreshAction);
        builder->addAction(connSelectAllAction);
        //builder->addAction(conn1SeparatorAction);


        builder->addModulesAction(modRunAction);
        builder->addModulesAction(modStopAction);
        builder->addModulesAction(modkillAction);
        builder->addModulesAction(modSeparator);
        builder->addModulesAction(modRefreshAction);
        builder->addModulesAction(modSelectAllAction);
        builder->addModulesAction(modAttachAction);
        builder->addModulesAction(modAssignAction);

        builder->addConnectionsAction(connConnectAction);
        builder->addConnectionsAction(connDisconnectAction);
        builder->addConnectionsAction(connSeparatorAction);
        builder->addConnectionsAction(connRefreshAction);
        builder->addConnectionsAction(connSelectAllAction);
        builder->addConnectionsAction(conn1SeparatorAction);


        connect(builder,SIGNAL(refreshApplication()),
                this,SLOT(onRefreshApplication()),Qt::DirectConnection);
        connect(builder,SIGNAL(setModuleSelected(QList<int>)),this,SLOT(onModuleSelected(QList<int>)));
        connect(builder,SIGNAL(setConnectionSelected(QList<int>)),this,SLOT(onConnectionSelected(QList<int>)));

    } else {
        connect(builder,SIGNAL(modified(bool)),this,SLOT(onModified(bool)));
    }


    builder->load();
    builderToolBar = builder->getToolBar();
}

bool ApplicationViewWidget::save()
{
    bool ret = true;
    if (builder) {
        ret = builder->save();
    }
    return ret;
}

QString ApplicationViewWidget::getFileName()
{
    if (builder)
        return builder->getFileName();
    else
        return {};
}

void ApplicationViewWidget::setFileName(QString filename)
{
    if (builder)
        builder->setFileName(filename);
    return;
}

QString ApplicationViewWidget::getAppName()
{
    if (builder) {
        return builder->getAppName();
    }
    else
        return {};
}

void ApplicationViewWidget::setAppName(QString appName)
{
    if (builder)
        builder->setAppName(appName);
    return;
}

ApplicationViewWidget::~ApplicationViewWidget()
{
    delete ui;
}

void ApplicationViewWidget::showBuilder(bool show)
{
    builderWidget->setVisible(show);
}

bool ApplicationViewWidget::anyModuleSelected()
{
    return !ui->moduleList->selectedItems().isEmpty();
}
bool ApplicationViewWidget::anyConnectionSelected()
{
    return !ui->connectionList->selectedItems().isEmpty();
}

CustomTreeWidget* ApplicationViewWidget::getModuleList()
{
    return ui->moduleList;
}

CustomTreeWidget* ApplicationViewWidget::getConnectionList()
{
    return ui->connectionList;
}

QToolBar* ApplicationViewWidget::getBuilderToolBar()
{
    return builderToolBar;
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

    modRunAction->setIcon(QIcon(":/play22.svg"));
    modStopAction->setIcon(QIcon(":/stop22.svg"));
    modkillAction->setIcon(QIcon(":/kill22.svg"));
    modRefreshAction->setIcon(QIcon(":/refresh22.svg"));
    modSelectAllAction->setIcon(QIcon(":/select-all22.svg"));
    modAssignAction->setIcon(QIcon(":/computer-available22.svg"));


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

    onModuleItemSelectionChanged();
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

    connSelectAllAction->setIcon(QIcon(":/select-all22.svg"));
    connRefreshAction->setIcon(QIcon(":/refresh22.svg"));
    connConnectAction->setIcon(QIcon(":/connect22.svg"));
    connDisconnectAction->setIcon(QIcon(":/disconnect22.svg"));

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

    onConnectionItemSelectionChanged();

}

/*! \brief Create the context menu for the resources tree. */
void ApplicationViewWidget::createResourcesViewContextMenu()
{
    resRefreshAction = new QAction("Refresh Status", this);
    resSelectAllAction = new QAction("Select All Resources", this);

    resRefreshAction->setIcon(QIcon(":/refresh22.svg"));
    resSelectAllAction->setIcon(QIcon(":/select-all22.svg"));

    ui->resourcesList->addAction(resRefreshAction);
    ui->resourcesList->addAction(resSelectAllAction);

    connect(resRefreshAction,SIGNAL(triggered()),this,SLOT(onRefresh()));
    connect(resSelectAllAction,SIGNAL(triggered()),this,SLOT(selectAllResources()));
}

/*! \brief Called when an item of the connections tree has been selected. */
void ApplicationViewWidget::onConnectionItemSelectionChanged()
{
    if (ui->connectionList->selectedItems().isEmpty()) {
        connConnectAction->setEnabled(false);
        connDisconnectAction->setEnabled(false);
        connSeparatorAction->setEnabled(false);
        connRefreshAction->setEnabled(false);
    } else {
        connConnectAction->setEnabled(true);
        connDisconnectAction->setEnabled(true);
        connSeparatorAction->setEnabled(true);
        connRefreshAction->setEnabled(true);
    }

    if (ui->connectionList->hasFocus()) {
        QList<int>selectedIds;
        QList<QTreeWidgetItem*> selectedItems = ui->connectionList->selectedItems();
        foreach(QTreeWidgetItem *it,selectedItems) {
            QString id = it->text(1);
            selectedIds.append(id.toInt());
        }

        builder->setSelectedConnections(selectedIds);
    }
}

/*! \brief Called when an item of the modules tree has been selected. */
void ApplicationViewWidget::onModuleItemSelectionChanged()
{

    if (ui->moduleList->selectedItems().isEmpty()) {
        modRunAction->setEnabled(false);
        modStopAction->setEnabled(false);
        modkillAction->setEnabled(false);
        modAttachAction->setEnabled(false);
        modAssignAction->setEnabled(false);
        modRefreshAction->setEnabled(false);
    } else {
        modRunAction->setEnabled(true);
        modStopAction->setEnabled(true);
        modkillAction->setEnabled(true);

        bool all = true;
        foreach (QTreeWidgetItem *it, ui->moduleList->selectedItems()) {
            if (it->text(3) != "localhost") {
                 modAttachAction->setEnabled(false);
                 all = false;
                 break;
            }
        }
        if (all) {
            modAttachAction->setEnabled(true);
        }

        modAssignAction->setEnabled(true);
        modRefreshAction->setEnabled(true);



        if (ui->moduleList->hasFocus()) {

            QList<int>selectedIds;
            QList<QTreeWidgetItem*> selectedItems = ui->moduleList->selectedItems();
            foreach(QTreeWidgetItem *it,selectedItems) {
                if (it->data(0,Qt::UserRole).toInt() == APPLICATION) {
                    for(int j=0;j<it->childCount();j++) {
                        QTreeWidgetItem *child = it->child(j);
                        child->setSelected(true);
                    }

                }

            }
            selectedItems = ui->moduleList->selectedItems();
            foreach(QTreeWidgetItem *it,selectedItems) {
                if (it->data(0,Qt::UserRole).toInt() == APPLICATION) {
                    continue;
                }
                QString id = it->text(1);
                selectedIds.append(id.toInt());
            }

            builder->setSelectedModules(selectedIds);
        }
    }


}

/*! \brief Called when an item of the resources tree has been selected. */
void ApplicationViewWidget::onResourceItemSelectionChanged()
{
    if (ui->resourcesList->currentItem() == nullptr) {
        resRefreshAction->setEnabled(false);
    } else {
        resRefreshAction->setEnabled(true);
    }
}

void ApplicationViewWidget::prepareManagerFrom(yarp::manager::Manager* lazy)
{

    safeManager.prepare(lazy, m_pConfig,dynamic_cast<ApplicationEvent*>(this));

    // loading application
    if (safeManager.loadApplication(app->getName())) {
       updateApplicationWindow();
    }

    reportErrors();
}

void ApplicationViewWidget::onRefreshApplication()
{
    //manager.close();

    // loading application

    if (safeManager.loadApplication(app->getName())) {
        updateApplicationWindow();
    }

    reportErrors();
}



void ApplicationViewWidget::onConnectionSelected(QList<int> id)
{
    disconnect(ui->connectionList,SIGNAL(itemSelectionChanged()),this,SLOT(onConnectionItemSelectionChanged()));
    for(int i=0;i<ui->connectionList->topLevelItemCount();i++) {
        ui->connectionList->topLevelItem(i)->setSelected(false);
    }
    for(int i=0;i<ui->connectionList->topLevelItemCount();i++) {
        for(int j=0;j<id.count();j++) {
            if (ui->connectionList->topLevelItem(i)->text(1).toInt() == id.at(j)) {
                ui->connectionList->topLevelItem(i)->setSelected(true);
            }
        }

    }
    onConnectionItemSelectionChanged();
    connect(ui->connectionList,SIGNAL(itemSelectionChanged()),this,SLOT(onConnectionItemSelectionChanged()));
}

void ApplicationViewWidget::onModuleSelected(QList<int> id)
{
    disconnect(ui->moduleList,SIGNAL(itemSelectionChanged()),this,SLOT(onModuleItemSelectionChanged()));
    for(int i=0;i<ui->moduleList->topLevelItemCount();i++) {
        ui->moduleList->topLevelItem(i)->setSelected(false);
        if (ui->moduleList->topLevelItem(i)->data(0,Qt::UserRole) == APPLICATION ) {
            for(int k=0;k<ui->moduleList->topLevelItem(i)->childCount();k++) {
                ui->moduleList->topLevelItem(i)->child(k)->setSelected(false);
            }
        }
    }

    for(int i=0;i<ui->moduleList->topLevelItemCount();i++) {
        if (ui->moduleList->topLevelItem(i)->data(0,Qt::UserRole) == APPLICATION ) {
            for(int k=0;k<ui->moduleList->topLevelItem(i)->childCount();k++) {
                for(int j=0;j<id.count();j++) {
                    if (ui->moduleList->topLevelItem(i)->child(k)->text(1).toInt() == id.at(j)) {
                        ui->moduleList->topLevelItem(i)->child(k)->setSelected(true);
                    }
                }


            }

        } else {
            for(int j=0;j<id.count();j++) {
                if (ui->moduleList->topLevelItem(i)->text(1).toInt() == id.at(j)) {
                    ui->moduleList->topLevelItem(i)->setSelected(true);
                }
            }
        }


    }
    onModuleItemSelectionChanged();
    connect(ui->moduleList,SIGNAL(itemSelectionChanged()),this,SLOT(onModuleItemSelectionChanged()));
}

/*! \brief Refresh the widget. */
void ApplicationViewWidget::updateApplicationWindow()
{
    ui->moduleList->clear();
    ui->connectionList->clear();
    ui->resourcesList->clear();
    listOfResourceNames.clear();

    disconnect(ui->moduleList,SIGNAL(itemChanged(QTreeWidgetItem*,int)),this,SLOT(onModuleItemChanged(QTreeWidgetItem*,int)));

    yarp::manager::ExecutablePContainer modules = safeManager.getExecutables();
    yarp::manager::CnnContainer connections  = safeManager.getConnections();
    yarp::manager::ExecutablePIterator moditr;
    yarp::manager::CnnIterator cnnitr;

    int id = 0;
    for(moditr=modules.begin(); moditr<modules.end(); moditr++)
    {

        Module *mod = (*moditr)->getModule();
        CustomTreeWidgetItem *appNode = nullptr;
        QString modLabel = mod->owner()->getLabel();
        QString appLabel = app->getLabel();
        if (modLabel != appLabel) {

            for(int i=0; i < ui->moduleList->topLevelItemCount();i++) {
                if (ui->moduleList->topLevelItem(i)->data(0,Qt::UserRole + 1).toString() == modLabel) {
                    appNode = (CustomTreeWidgetItem*) ui->moduleList->topLevelItem(i);
                    break;
                }
            }
            if (!appNode) {
                QStringList l;
                l << ((Application*)mod->owner())->getName();

                appNode = new CustomTreeWidgetItem(ui->moduleList,l);
                appNode->setData(0,Qt::UserRole + 1,modLabel);
                appNode->setData(0,Qt::UserRole,APPLICATION);
                appNode->setExpanded(true);
                appNode->setIcon(0,QIcon(":/run22.svg"));
            }
        }
        QString id = QString("%1").arg((*moditr)->getID());
        QString command = QString("%1").arg((*moditr)->getCommand());
        QString host = QString("%1").arg((*moditr)->getHost());
        QString param = QString("%1").arg((*moditr)->getParam());
        QString stdio = QString("%1").arg((*moditr)->getStdio());
        QString workDir = QString("%1").arg((*moditr)->getWorkDir());
        QString env = QString("%1").arg((*moditr)->getEnv());

        // The default host is "localhost" if the <node> is not specified.
        if(host.isEmpty())
        {
            host = "localhost";
        }

        QStringList l;
        l << command << id << "stopped" << host << param << stdio << workDir << env;

        CustomTreeWidgetItem *it;
        if (!appNode)
            it = new CustomTreeWidgetItem(ui->moduleList,l);
        else
            it = new CustomTreeWidgetItem(appNode,l);

        //it->setFlags(it->flags() | Qt::ItemIsEditable);
        it->setData(0,Qt::UserRole,yarp::manager::MODULE);
        it->setIcon(0,QIcon(":/close.svg"));
        it->setForeground(2,QColor("#BF0303"));
        ui->moduleList->addTopLevelItem(it);
    }

    id = 0;
    for(cnnitr=connections.begin(); cnnitr<connections.end(); cnnitr++)
    {
        QString type;
        if ((*cnnitr).isPersistent())
            type = "Persistent";
        else
        {
            if ((*cnnitr).isExternalFrom() || (*cnnitr).isExternalTo())
                type = "External";
            else
                type = "Internal";
        }
        QString sId = QString("%1").arg(id);
        QString from = QString("%1").arg((*cnnitr).from());
        QString to = QString("%1").arg((*cnnitr).to());
        QString carrier = QString("%1").arg((*cnnitr).carrier());
        QString status = "disconnected";
        QString modifier="";
        // The default carrier is "tcp" if <protocol> is not specified.
        if(carrier.isEmpty())
        {
            carrier = "tcp";
        }
        size_t pos = carrier.toStdString().find('+');
        if(pos != std::string::npos)
        {
            modifier = carrier.mid(pos);
            QStringList myStringList = carrier.split('+');
            carrier = myStringList.first();
        }


        QStringList l;
        l << type << sId << status << from << to << carrier << modifier;
        auto* it = new CustomTreeWidgetItem(ui->connectionList,l);
        ui->moduleList->addTopLevelItem(it);

        //scanning available carriers:
        scanAvailableCarriers(carrier,false);

        auto* comboBox = new QComboBox(this);
        comboBox->addItems(stringLst);
        comboBox->setEditable(true);
        ui->connectionList->setItemWidget((QTreeWidgetItem *) it,5, comboBox);
        it->setData(0,Qt::UserRole,yarp::manager::INOUTD);
        it->setIcon(0,QIcon(":/disconnect22.svg"));
        it->setForeground(2,QColor("#BF0303"));
        id++;
    }

    id = 0;
    yarp::manager::ResourcePIterator itrS;
    for(itrS=safeManager.getResources().begin(); itrS!=safeManager.getResources().end(); itrS++)
    {
        //m_resRow = *(m_refTreeResModel->append());
        QString sId = QString("%1").arg(id);
        QString type;
        if (dynamic_cast<yarp::manager::Computer*>(*itrS))
        {
            type = "computer";
        }
        else if (dynamic_cast<yarp::manager::ResYarpPort*>(*itrS))
        {
            type = "port";
        }
        QString res = QString("%1").arg((*itrS)->getName());
        if (std::find(listOfResourceNames.begin(),
                      listOfResourceNames.end(),
                      res.toStdString()) == listOfResourceNames.end())
        {
          listOfResourceNames.push_back(res.toStdString());
        }
        else
        {
            // The resource has been already added
            continue;
        }
        QString status = "unknown";
        //m_resRow[m_resColumns.m_col_color] = Gdk::Color("#00000");

        QStringList l;
        l << res << sId << type << status ;
        auto* it = new CustomTreeWidgetItem(ui->resourcesList,l);
        ui->moduleList->addTopLevelItem(it);
        it->setData(0,Qt::UserRole,yarp::manager::RESOURCE);
        if (type == "computer") {
            it->setIcon(0,QIcon(":/computer-unavailable22.svg"));
            it->setForeground(3,QColor("#BF0303"));
        } else {
            it->setIcon(0,QIcon(":/port-unavailable22.svg"));
            it->setForeground(3,QColor("#BF0303"));
        }
        id++;
    }
    connect(ui->moduleList,SIGNAL(itemChanged(QTreeWidgetItem*,int)),this,SLOT(onModuleItemChanged(QTreeWidgetItem*,int)));
}

void ApplicationViewWidget::onModuleItemChanged(QTreeWidgetItem *it,int col)
{
    Qt::ItemFlags tmp = it->flags();
    if (!(tmp & Qt::ItemIsEditable)) {
        return;
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
           if (col == 4 || col == 5 || col == 6 || col == 7) {
               if (it->text(2) == "stopped") {
                    return true;
               }
           }
           if (col == 3) {
                if (it->text(2) == "stopped") {
                    return true;
                }
           }
        break;
    }
    case yarp::manager::INOUTD:{
           if (col == 3 || col == 4 || col == 5 || col == 6) {
               if (it->text(2) == "disconnected") {
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
    for(int i=0;i<ui->moduleList->topLevelItemCount();i++) {
        QTreeWidgetItem *it = ui->moduleList->topLevelItem(i);
        if (it->data(0,Qt::UserRole) == APPLICATION) {
            closeNestedApplicationStdOut(it,id);
        } else {
            if (it->text(1).toInt() == id) {
                auto* stdouWin = qvariant_cast<StdoutWindow *>(it->data(0,Qt::UserRole));
                if (stdouWin && stdouWin->getId() == id) {
                    delete stdouWin;
                    it->setData(0,Qt::UserRole,QVariant::fromValue(NULL));
                    return;
                }
            }
        }
    }
}

void ApplicationViewWidget::closeNestedApplicationStdOut(QTreeWidgetItem *it, int id)
{
    for(int j=0; j<it->childCount();j++) {
        QTreeWidgetItem *ch = it->child(j);
        if (ch->data(0,Qt::UserRole) == APPLICATION) {
            closeNestedApplicationStdOut(it,id);
        } else {
            if (ch->text(1).toInt() == id) {
                auto* stdouWin = qvariant_cast<StdoutWindow *>(ch->data(0,Qt::UserRole));
                if (stdouWin && stdouWin->getId() == id) {
                    delete stdouWin;
                    ch->setData(0,Qt::UserRole,QVariant::fromValue(NULL));
                    return;
                }
            }
        }
    }
}

bool ApplicationViewWidget::areAllShutdown()
{
    bool ret = true;
    for(int i=0;i<ui->moduleList->topLevelItemCount();i++) {
        QTreeWidgetItem *it = ui->moduleList->topLevelItem(i);
        if (it->data(0,Qt::UserRole) == APPLICATION) {
            ret = areAllNestedApplicationShutdown(it);
            if (!ret) {
                return false;
            }
        } else {
            if (it->text(2) != "stopped") {
                return false;
            }
        }
    }

    for(int i=0;i<ui->connectionList->topLevelItemCount();i++) {
        QTreeWidgetItem *it = ui->connectionList->topLevelItem(i);
        if (it->text(2) != "disconnected") {
            return false;
        }
    }

    return ret;
}

bool ApplicationViewWidget::areAllNestedApplicationShutdown(QTreeWidgetItem *it)
{
    bool ret = true;
    for(int j=0; j<it->childCount();j++) {
        QTreeWidgetItem *ch = it->child(j);
        if (ch->data(0,Qt::UserRole) == APPLICATION) {
            ret = areAllNestedApplicationShutdown(ch);
            if (!ret) {
                return false;
            }
        } else {
            if (ch->text(2) != "stopped") {
                return false;
            }
        }
    }
    return ret;
}

void ApplicationViewWidget::onSelfSafeLoadBalance()
{
    updateApplicationWindow();
    reportErrors();
}

void ApplicationViewWidget::onAssignHost()
{
    if (areAllShutdown() && !safeManager.busy()) {
        for(int i=0;i<ui->moduleList->topLevelItemCount();i++) {
            QTreeWidgetItem *it = ui->moduleList->topLevelItem(i);
            if (it->data(0,Qt::UserRole) == APPLICATION) {
                assignHostNestedApplication(it);
            } else {
                if (it->isSelected()) {
                    //QString waitHost = QString("%1").arg("?");
                    it->setText(2,"waiting");
                    it->setText(3,"?");
                }
            }


        }
        safeManager.safeLoadBalance();
        yarp::os::SystemClock::delaySystem(0.1);


    } else {
        yarp::manager::ErrorLogger* logger  = yarp::manager::ErrorLogger::Instance();
        logger->addError("Running modules should be stopped before assigning hosts.");
        reportErrors();
    }
}

void ApplicationViewWidget::assignHostNestedApplication(QTreeWidgetItem *it)
{
    for(int j=0; j<it->childCount();j++) {
        QTreeWidgetItem *ch = it->child(j);
        if (ch->data(0,Qt::UserRole) == APPLICATION) {
            assignHostNestedApplication(ch);
        } else {
            if (ch->isSelected()) {
                //QString waitHost = QString("%1").arg("?");
                ch->setText(2,"waiting");
                ch->setText(3,"?");
            }
        }
    }
}

void ApplicationViewWidget::onAttachStdout()
{
    if (safeManager.busy()) {
        return;
    }

    std::vector<int> MIDs;
    for(int i=0;i<ui->moduleList->topLevelItemCount();i++) {
        QTreeWidgetItem *it = ui->moduleList->topLevelItem(i);
        if (it->data(0,Qt::UserRole) == APPLICATION) {
            attachStdOutNestedApplication(it,&MIDs);
        } else {
            if (it->isSelected()) {
                int id = it->text(1).toInt();

                auto* stdouWin = qvariant_cast<StdoutWindow *>(it->data(0,Qt::UserRole));
                if (stdouWin && stdouWin->getId() == id) {
                    // found
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


    }
    safeManager.safeAttachStdout(MIDs);

    //return true;
}

void ApplicationViewWidget::attachStdOutNestedApplication(QTreeWidgetItem *it,std::vector<int> *MIDs)
{
    for(int j=0; j<it->childCount();j++) {
        QTreeWidgetItem *ch = it->child(j);
        if (ch->data(0,Qt::UserRole) == APPLICATION) {
            attachStdOutNestedApplication(ch,MIDs);
        } else {
            if (ch->isSelected()) {
                int id = ch->text(1).toInt();

                auto* stdouWin = qvariant_cast<StdoutWindow *>(ch->data(0,Qt::UserRole));
                if (stdouWin && stdouWin->getId() == id) {
                    // found
                    continue;
                }

                MIDs->push_back(ch->text(1).toInt());

                QString name = QString("%1").arg(app->getName());
                QString strTitle = name + ":" + ch->text(0) + ":" + ch->text(1);

                StdoutWindow *stdOutWindow = new StdoutWindow(id,strTitle);
                connect(stdOutWindow,SIGNAL(closeStdOut(int)),this,SLOT(onCloseStdOut(int)));
                //stdoutWinList.append(stdOutWindow);
                ch->setData(0,Qt::UserRole,QVariant::fromValue(stdOutWindow));
                stdOutWindow->show();
            }

        }
    }
}

/*! \brief Called when the Run button has been pressed */
bool ApplicationViewWidget::onRun()
{
    selectAllConnections(true);
    selectAllResources(true);
    if (safeManager.busy() ) {
        return false;
    }

    std::vector<int> MIDs;
    for(int i=0;i<ui->moduleList->topLevelItemCount();i++) {
        QTreeWidgetItem *it = ui->moduleList->topLevelItem(i);

        if (it->data(0,Qt::UserRole) == APPLICATION) {
            runNestedApplication(it,&MIDs);

        } else {
            if (it->isSelected()) {
                MIDs.push_back(it->text(1).toInt());
                safeManager.updateExecutable(it->text(1).toInt(),
                                         it->text(4).toLatin1().data(),
                                         it->text(3).toLatin1().data(),
                                         it->text(5).toLatin1().data(),
                                         it->text(6).toLatin1().data(),
                                         it->text(7).toLatin1().data());

                it->setText(2,"waiting");
                it->setIcon(0,QIcon(":/refresh.svg"));
                it->setForeground(2,QColor("#000000"));

        }

            //yDebug("Trying to run module ID %i param %s host %s stdio %s wdir %s env %s",it->text(1).toInt(),it->text(4).toLatin1().data(),
            //        it->text(3).toLatin1().data(),it->text(5).toLatin1().data(),it->text(6).toLatin1().data(),it->text(7).toLatin1().data());
        }
    }

    std::vector<int> CIDs;
    /*for(int i=0;i<ui->connectionList->topLevelItemCount();i++)
    {
        updateConnection(i, CIDs);
    }*/

    std::vector<int> RIDs;
    /*for(int i=0;i<ui->resourcesList->topLevelItemCount();i++) {
        QTreeWidgetItem *it = ui->resourcesList->topLevelItem(i);
        if (it->isSelected()) {
            RIDs.push_back(it->text(1).toInt());
            it->setText(3,"waiting");
            it->setIcon(0,QIcon(":/refresh22.svg"));
            it->setForeground(3,QColor("#000000"));
        }
    }*/


    safeManager.safeRun(MIDs,CIDs,RIDs);
    yarp::os::SystemClock::delaySystem(0.1);
    selectAllModule(false);
    selectAllConnections(false);
    selectAllResources(false);
    return true;
}

void ApplicationViewWidget::runNestedApplication(QTreeWidgetItem *it,std::vector<int> *MIDs)
{
    for(int j=0; j<it->childCount();j++) {
        QTreeWidgetItem *ch = it->child(j);

            if (ch->data(0,Qt::UserRole) == APPLICATION) {
                runNestedApplication(ch,MIDs);
            } else {
                if (ch->isSelected()) {
                    MIDs->push_back(ch->text(1).toInt());
                    safeManager.updateExecutable(ch->text(1).toInt(),
                                             ch->text(4).toLatin1().data(),
                                             ch->text(3).toLatin1().data(),
                                             ch->text(5).toLatin1().data(),
                                             ch->text(6).toLatin1().data(),
                                             ch->text(7).toLatin1().data());

                    ch->setText(2,"waiting");
                    ch->setIcon(0,QIcon(":/refresh.svg"));
                    ch->setForeground(2,QColor("#000000"));
                }
            }



    }
}

/*! \brief Called when the Stop button has been pressed */
bool ApplicationViewWidget::onStop()
{
    selectAllConnections(true);
    selectAllResources(true);
    if (safeManager.busy()) {
        return false;
    }

    std::vector<int> MIDs;
    for(int i=0;i<ui->moduleList->topLevelItemCount();i++) {
        QTreeWidgetItem *it = ui->moduleList->topLevelItem(i);
        if (it->data(0,Qt::UserRole) == APPLICATION) {
            stopNestedApplication(it,&MIDs);

        } else {
            if (it->isSelected()) {
                //moduluesIDs.append(it->text(1).toInt());
                MIDs.push_back(it->text(1).toInt());
                safeManager.updateExecutable(it->text(1).toInt(),
                                         it->text(4).toLatin1().data(),
                                         it->text(3).toLatin1().data(),
                                         it->text(5).toLatin1().data(),
                                         it->text(6).toLatin1().data(),
                                         it->text(7).toLatin1().data());

                it->setText(2,"waiting");
                it->setIcon(0,QIcon(":/refresh.svg"));
                it->setForeground(2,QColor("#000000"));

            }
        }

    }

    std::vector<int> CIDs;
    /*for(int i=0;i<ui->connectionList->topLevelItemCount();i++)
    {
        updateConnection(i, CIDs);
    }*/

    std::vector<int> RIDs;
    /*for(int i=0;i<ui->resourcesList->topLevelItemCount();i++) {
        QTreeWidgetItem *it = ui->resourcesList->topLevelItem(i);
        if (it->isSelected()) {
            RIDs.push_back(it->text(1).toInt());
            it->setText(3,"waiting");
            it->setIcon(0,QIcon(":/refresh22.svg"));
            it->setForeground(3,QColor("#000000"));
        }
    }*/


    safeManager.safeStop(MIDs,CIDs,RIDs);
    yarp::os::SystemClock::delaySystem(0.1);
    selectAllModule(false);
    selectAllConnections(false);
    selectAllResources(false);
    return true;
}

void ApplicationViewWidget::stopNestedApplication(QTreeWidgetItem *it,std::vector<int> *MIDs)
{
    for(int j=0; j<it->childCount();j++) {
        QTreeWidgetItem *ch = it->child(j);
            if (ch->data(0,Qt::UserRole) == APPLICATION) {
                stopNestedApplication(ch,MIDs);
            } else {
                if (ch->isSelected()) {
                    MIDs->push_back(ch->text(1).toInt());
                    safeManager.updateExecutable(ch->text(1).toInt(),
                                             ch->text(4).toLatin1().data(),
                                             ch->text(3).toLatin1().data(),
                                             ch->text(5).toLatin1().data(),
                                             ch->text(6).toLatin1().data(),
                                             ch->text(7).toLatin1().data());

                    ch->setText(2,"waiting");
                    ch->setIcon(0,QIcon(":/refresh.svg"));
                    ch->setForeground(2,QColor("#000000"));
                }
            }



    }
}

/*! \brief Called when the Kill button has been pressed */
bool ApplicationViewWidget::onKill()
{
    selectAllConnections(true);
    selectAllResources(true);
    if (safeManager.busy()) {
        return false;
    }

    if (QMessageBox::question(this,"Killing modules!","Are you sure?") != QMessageBox::Yes) {
        return true;
    }

    std::vector<int> MIDs;
    for(int i=0;i<ui->moduleList->topLevelItemCount();i++) {
        QTreeWidgetItem *it = ui->moduleList->topLevelItem(i);
        if (it->data(0,Qt::UserRole) == APPLICATION) {
            killNestedApplication(it,&MIDs);
        } else {
            if (it->isSelected()) {
                //moduluesIDs.append(it->text(1).toInt());
                MIDs.push_back(it->text(1).toInt());
                safeManager.updateExecutable(it->text(1).toInt(),
                                         it->text(4).toLatin1().data(),
                                         it->text(3).toLatin1().data(),
                                         it->text(5).toLatin1().data(),
                                         it->text(6).toLatin1().data(),
                                         it->text(7).toLatin1().data());

                it->setText(2,"waiting");
                it->setIcon(0,QIcon(":/refresh22.svg"));
                it->setForeground(2,QColor("#000000"));

            }
        }

    }

    std::vector<int> CIDs;
    /*for(int i=0;i<ui->connectionList->topLevelItemCount();i++)
    {
        updateConnection(i, CIDs);

    }*/

    std::vector<int> RIDs;
    /*for(int i=0;i<ui->resourcesList->topLevelItemCount();i++) {
        QTreeWidgetItem *it = ui->resourcesList->topLevelItem(i);
        if (it->isSelected()) {
            RIDs.push_back(it->text(1).toInt());
            it->setText(3,"waiting");
            it->setIcon(0,QIcon(":/refresh22.svg"));
            it->setForeground(3,QColor("#000000"));
        }
    }*/


    safeManager.safeKill(MIDs, CIDs, RIDs);
    yarp::os::SystemClock::delaySystem(0.1);
    selectAllModule(false);
    selectAllConnections(false);
    selectAllResources(false);
    return true;
}

void ApplicationViewWidget::killNestedApplication(QTreeWidgetItem *it,std::vector<int> *MIDs)
{
    for(int j=0; j<it->childCount();j++) {
        QTreeWidgetItem *ch = it->child(j);
            if (ch->data(0,Qt::UserRole) == APPLICATION) {
                killNestedApplication(ch,MIDs);
            } else {
                if (ch->isSelected()) {
                    MIDs->push_back(ch->text(1).toInt());
                    safeManager.updateExecutable(ch->text(1).toInt(),
                                             ch->text(4).toLatin1().data(),
                                             ch->text(3).toLatin1().data(),
                                             ch->text(5).toLatin1().data(),
                                             ch->text(6).toLatin1().data(),
                                             ch->text(7).toLatin1().data());

                    ch->setText(2,"waiting");
                    ch->setIcon(0,QIcon(":/refresh.svg"));
                    ch->setForeground(2,QColor("#000000"));
                }
            }



    }
}

/*! \brief Called when the Conenct button has been pressed */
bool ApplicationViewWidget::onConnect()
{
    if (safeManager.busy()) {
        return false;
    }


    std::vector<int> CIDs;
    for(int i=0;i<ui->connectionList->topLevelItemCount();i++)
    {
        updateConnection(i,CIDs);
    }


    safeManager.safeConnect(CIDs);
    yarp::os::SystemClock::delaySystem(0.1);
    selectAllConnections(false);
    return true;
}

/*! \brief Called when the Disconnect button has been pressed */
bool ApplicationViewWidget::onDisconnect()
{
    if (safeManager.busy()) {
        return false;
    }


    std::vector<int> CIDs;
    for(int i=0;i<ui->connectionList->topLevelItemCount();i++)
    {
        updateConnection(i, CIDs);
    }


    safeManager.safeDisconnect(CIDs);
    yarp::os::SystemClock::delaySystem(0.1);
    selectAllConnections(false);
    return true;
}


/*! \brief Called when the Refresh button has been pressed */
bool ApplicationViewWidget::onRefresh()
{
    if (safeManager.busy()) {
        return false;
    }

    std::vector<int> modulesIDs;
    std::vector<int> connectionsIDs;
    std::vector<int> resourcesIDs;

    for(int i=0;i<ui->moduleList->topLevelItemCount();i++) {
        QTreeWidgetItem *it = ui->moduleList->topLevelItem(i);
        if (it->data(0,Qt::UserRole) == APPLICATION) {
            refreshNestedApplication(it,&modulesIDs);
        } else {
            if (it->isSelected()) {
                modulesIDs.push_back(it->text(1).toInt());
                safeManager.updateExecutable(it->text(1).toInt(),
                                         it->text(4).toLatin1().data(),
                                         it->text(3).toLatin1().data(),
                                         it->text(5).toLatin1().data(),
                                         it->text(6).toLatin1().data(),
                                         it->text(7).toLatin1().data());
                it->setText(2,"waiting");
                it->setIcon(0,QIcon(":/refresh22.svg"));
                it->setForeground(2,QColor("#000000"));
            }
        }

    }

    for(int i=0;i<ui->connectionList->topLevelItemCount();i++) {
        updateConnection(i,connectionsIDs);
    }

    for(int i=0;i<ui->resourcesList->topLevelItemCount();i++) {
        QTreeWidgetItem *it = ui->resourcesList->topLevelItem(i);
        if (it->isSelected()) {
            resourcesIDs.push_back(it->text(1).toInt());
            it->setText(3,"waiting");
            it->setIcon(0,QIcon(":/refresh22.svg"));
            it->setForeground(3,QColor("#000000"));
        }
    }

    safeManager.safeRefresh(modulesIDs,
                        connectionsIDs,
                        resourcesIDs);
    yarp::os::SystemClock::delaySystem(0.1);
    selectAllConnections(false);
    selectAllModule(false);
    selectAllResources(false);

    return true;
}
void ApplicationViewWidget::refreshNestedApplication(QTreeWidgetItem *it,std::vector<int> *MIDs)
{
    for(int j=0; j<it->childCount();j++) {
        QTreeWidgetItem *ch = it->child(j);

        if (ch->data(0,Qt::UserRole) == APPLICATION) {
            refreshNestedApplication(ch,MIDs);
        } else {
            if (ch->isSelected()) {
                MIDs->push_back(ch->text(1).toInt());
                ch->setText(2,"waiting");
                ch->setIcon(0,QIcon(":/refresh22.svg"));
                ch->setForeground(2,QColor("#000000"));
            }
        }

    }
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
    disconnect(ui->moduleList,SIGNAL(itemSelectionChanged()),this,SLOT(onModuleItemSelectionChanged()));
    for(int i=0;i<ui->moduleList->topLevelItemCount();i++) {
        QTreeWidgetItem *it = ui->moduleList->topLevelItem(i);
        if (it->data(0,Qt::UserRole) == APPLICATION) {
               selectAllNestedApplicationModule(it,check);
        } else {
            it->setSelected(check);
        }
    }
    if (ui->moduleList->selectedItems().isEmpty()) {
        modRunAction->setEnabled(false);
        modStopAction->setEnabled(false);
        modkillAction->setEnabled(false);
        modAttachAction->setEnabled(false);
        modAssignAction->setEnabled(false);
        modRefreshAction->setEnabled(false);
    } else {
        modRunAction->setEnabled(true);
        modStopAction->setEnabled(true);
        modkillAction->setEnabled(true);

        bool all = true;
        foreach (QTreeWidgetItem *it, ui->moduleList->selectedItems()) {
            if (it->text(3) != "localhost") {
                 modAttachAction->setEnabled(false);
                 all = false;
                 break;
            }
        }
        if (all) {
            modAttachAction->setEnabled(true);
        }
//        if (ui->moduleList->currentItem()->text(3) == "localhost")
//        {
//            modAttachAction->setEnabled(true);
//        }
//        else
//        {
//            modAttachAction->setEnabled(false);
//        }
        modAssignAction->setEnabled(true);
        modRefreshAction->setEnabled(true);




        QList<int>selectedIds;
        QList<QTreeWidgetItem*> selectedItems = ui->moduleList->selectedItems();
        foreach(QTreeWidgetItem *it,selectedItems) {
            if (it->data(0,Qt::UserRole).toInt() == APPLICATION) {
                for(int j=0;j<it->childCount();j++) {
                    QTreeWidgetItem *child = it->child(j);
                    child->setSelected(true);
                }

            }

        }
        selectedItems = ui->moduleList->selectedItems();
        foreach(QTreeWidgetItem *it,selectedItems) {
            if (it->data(0,Qt::UserRole).toInt() == APPLICATION) {
                continue;
            }
            QString id = it->text(1);
            selectedIds.append(id.toInt());
        }

        builder->setSelectedModules(selectedIds);
    }


    connect(ui->moduleList,SIGNAL(itemSelectionChanged()),this,SLOT(onModuleItemSelectionChanged()));
}

void ApplicationViewWidget::selectAllNestedApplicationModule(QTreeWidgetItem *it,bool check)
{
    for(int j=0;j<it->childCount();j++) {
        QTreeWidgetItem *ch = it->child(j);
        if (ch->data(0,Qt::UserRole) == APPLICATION) {
            selectAllNestedApplicationModule(ch, check);
        } else {
            ch->setSelected(check);
        }
    }
}

bool ApplicationViewWidget::scanAvailableCarriers(QString carrier, bool isConnection){
    yarp::os::Bottle lst=yarp::os::Carriers::listCarriers();
    yarp::manager::ErrorLogger* logger  = yarp::manager::ErrorLogger::Instance();
    bool res=false;
    stringLst.clear();
    stringLst.push_back(carrier);
    for (size_t i=0; i<lst.size(); i++)
    {
        if (lst.get(i).asString() == carrier.toStdString())
            res = true;
        else
            stringLst.push_back(lst.get(i).asString().c_str());
    }
    if (!res && isConnection)
    {
        std::string msg = "Unable to find '"+ carrier.toStdString() +
                "' among the available carriers in this machine, the connection could fail.";
        logger->addWarning(msg.c_str());
    }
    return res;
}

void ApplicationViewWidget::updateConnection(int index, std::vector<int>& CIDs)
{
    QTreeWidgetItem *it = ui->connectionList->topLevelItem(index);
    if (it->isSelected()) {
        auto* box = qobject_cast<QComboBox*>(ui->connectionList->itemWidget((QTreeWidgetItem *)it, 5));
        QString carrier, modifier;
        if (box)
        {
            carrier = box->currentText();

        }
        else
        {
            carrier=it->text(5);
        }

        //checking if in the carrier has been added a modifier

        size_t pos = carrier.toStdString().find('+');
        if(pos != std::string::npos)
        {
            modifier = carrier.mid(pos);
            QStringList myStringList = carrier.split('+');
            carrier = myStringList.first();
            box->setCurrentText(carrier);
            it->setText(6,modifier);
        }

        // scan the available carriers in the system where yarpmanager is launched.
        scanAvailableCarriers(carrier);
        carrier = carrier + it->text(6); //adding modifier.
        CIDs.push_back(it->text(1).toInt());
        safeManager.updateConnection(it->text(1).toInt(),
                                 it->text(3).toLatin1().data(),
                                 it->text(4).toLatin1().data(),
                                 carrier.toLatin1().data());

        it->setText(2,"waiting");
        it->setIcon(0,QIcon(":/refresh22.svg"));
        it->setForeground(2,QColor("#000000"));
    }

}

/*! \brief Select/deselect all connections
    \param check
*/
void ApplicationViewWidget::selectAllConnections(bool check)
{
    disconnect(ui->connectionList,SIGNAL(itemSelectionChanged()),this,SLOT(onConnectionItemSelectionChanged()));
    for(int i=0;i<ui->connectionList->topLevelItemCount();i++) {
        QTreeWidgetItem *it = ui->connectionList->topLevelItem(i);
        it->setSelected(check);
    }

    if (ui->connectionList->selectedItems().isEmpty()) {
        connConnectAction->setEnabled(false);
        connDisconnectAction->setEnabled(false);
        connSeparatorAction->setEnabled(false);
        connRefreshAction->setEnabled(false);
    } else {
        connConnectAction->setEnabled(true);
        connDisconnectAction->setEnabled(true);
        connSeparatorAction->setEnabled(true);
        connRefreshAction->setEnabled(true);
    }

    QList<int>selectedIds;
    QList<QTreeWidgetItem*> selectedItems = ui->connectionList->selectedItems();
    foreach(QTreeWidgetItem *it,selectedItems) {
        QString id = it->text(1);
        selectedIds.append(id.toInt());
    }

    builder->setSelectedConnections(selectedIds);
    connect(ui->connectionList,SIGNAL(itemSelectionChanged()),this,SLOT(onConnectionItemSelectionChanged()));

}

/*! \brief Select/deselect all resources
    \param check
*/
void ApplicationViewWidget::selectAllResources(bool check)
{
    for(int i=0;i<ui->resourcesList->topLevelItemCount();i++) {
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
void ApplicationViewWidget::runApplicationSet(bool onlySelected)
{
    if(!onlySelected)
        selectAllModule(true);
    onRun();
}

/*! \brief Stop all modules in the application */
void ApplicationViewWidget::stopApplicationSet(bool onlySelected)
{
    if(!onlySelected)
        selectAllModule(true);
    onStop();
}

/*! \brief Kill all running modules in the application */
void ApplicationViewWidget::killApplicationSet(bool onlySelected)
{
    if(!onlySelected)
        selectAllModule(true);
    onKill();
}

/*! \brief Connect all modules in the application to their ports using connections list*/
void ApplicationViewWidget::connectConnectionSet(bool onlySelected)
{
    if(!onlySelected)
        selectAllConnections(true);
    onConnect();
}

/*! \brief Disconnect all modules in the application to their ports using connections list*/
void ApplicationViewWidget::disconnectConnectionSet(bool onlySelected)
{
    if(!onlySelected)
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
    if (logger->errorCount() || logger->warningCount())
    {
        const char* err;
        while((err=logger->getLastError()))
        {
            QString msg = QString("(%1) %2").arg(app->getName()).arg(err);
            emit logError(msg);
        }

        while((err=logger->getLastWarning()))
        {
            QString msg = QString("(%1) %2").arg(app->getName()).arg(err);
            emit logWarning(msg);
        }
    }
}


/*! \brief Export the current Graph*/
void ApplicationViewWidget::exportGraph()
{
    QString fileName = QFileDialog::getSaveFileName(this,"Export Graph",QApplication::applicationDirPath(),"GraphViz format (*.dot)");

    if (!fileName.isEmpty()) {
        if (!safeManager.exportDependencyGraph(fileName.toLatin1().data()))
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
    if (safeManager.busy()) {
        return;
    }
    yarp::manager::ErrorLogger* logger  = yarp::manager::ErrorLogger::Instance();

    for(int i=0;i<ui->connectionList->topLevelItemCount();i++) {
        QTreeWidgetItem *it = ui->connectionList->topLevelItem(i);
        if (it->isSelected()) {
            QString from = it->text(3);
            QString to = QString("/inspect").arg(from);
            QString env = QString("YARP_PORT_PREFIX=%1").arg(to);
            to += "/yarpview/img:i";

            yarp::manager::LocalBroker launcher;
            if (launcher.init("yarpview", nullptr, nullptr, nullptr, nullptr, env.toLatin1().data()))
            {
                if (!launcher.start() && strlen(launcher.error()))
                {
                    QString msg;
                    msg = QString("Error while launching yarpview. %1").arg(launcher.error());
                    logger->addError(msg.toLatin1().data());
                    reportErrors();
                }
                else
                {
                    // waiting for the port to get open
                    double base = yarp::os::SystemClock::nowSystem();
                    while(!timeout(base, 3.0)) {
                        if (launcher.exists(to.toLatin1().data())) {
                            break;
                        }
                    }
                    if (!launcher.connect(from.toLatin1().data(), to.toLatin1().data(), "udp")) {
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


    yarp::os::SystemClock::delaySystem(0.1);
}

/*! \brief Launch YARPHear Inspection modality*/
void ApplicationViewWidget::onYARPHear()
{
    if (safeManager.busy()) {
        return;
    }
    yarp::manager::ErrorLogger* logger  = yarp::manager::ErrorLogger::Instance();

    for(int i=0;i<ui->connectionList->topLevelItemCount();i++) {
        QTreeWidgetItem *it = ui->connectionList->topLevelItem(i);
        if (it->isSelected()) {
            QString from = it->text(3);
            QString to = QString("/inspect/hear%1").arg(from);

#if defined(_WIN32)
            QString cmd = "cmd.exe";
            QString param;
            param = QString("/C yarphear --name %1").arg(to);

#else
            QString cmd = "xterm";
            QString param;
            param = QString("-hold -title %1 -e yarphear --nodevice --name %2").arg(from).arg(to);
#endif

            yarp::manager::LocalBroker launcher;
            launcher.setWindowMode(yarp::manager::LocalBroker::WINDOW_VISIBLE);
            if (launcher.init(cmd.toLatin1().data(), param.toLatin1().data(), nullptr, nullptr, nullptr, nullptr))
            {
                if (!launcher.start() && strlen(launcher.error()))
                {
                    QString  msg;
                    msg = QString("Error while launching yarpread. %1").arg(launcher.error());
                    logger->addError(msg.toLatin1().data());
                    reportErrors();
                }
                else
                {
                    // waiting for the port to get open
                    double base = yarp::os::SystemClock::nowSystem();
                    while(!timeout(base, 3.0)) {
                        if (launcher.exists(to.toLatin1().data())) {
                            break;
                        }
                    }
                    if (!launcher.exists(to.toLatin1().data())) {
                        QString msg;
                        msg = QString("Cannot inspect '%1' : %2. Did you build yarp with 'portaudio' module?").arg(from).arg(launcher.error());
                        logger->addError(msg.toLatin1().data());
                        launcher.stop();
                        reportErrors();
                    }else if (!launcher.connect(from.toLatin1().data(), to.toLatin1().data(), "udp")) {
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
    yarp::os::SystemClock::delaySystem(0.1);



}

/*! \brief Launch YARPRead Inspection modality*/
void ApplicationViewWidget::onYARPRead()
{
    if (safeManager.busy()) {
        return;
    }
    yarp::manager::ErrorLogger* logger  = yarp::manager::ErrorLogger::Instance();

    for(int i=0;i<ui->connectionList->topLevelItemCount();i++) {
        QTreeWidgetItem *it = ui->connectionList->topLevelItem(i);
        if (it->isSelected()) {
            QString from = it->text(3);
            QString to = QString("/inspect/read%1").arg(from);

#if defined(_WIN32)
            QString cmd = "cmd.exe";
            QString param;
            param = QString("/C yarp read %1").arg(to);

#else
            QString cmd = "xterm";
            QString param;
            param = QString("-hold -title %1 -e yarp read %2").arg(from).arg(to);
#endif

            yarp::manager::LocalBroker launcher;
            launcher.setWindowMode(yarp::manager::LocalBroker::WINDOW_VISIBLE);
            if (launcher.init(cmd.toLatin1().data(), param.toLatin1().data(), nullptr, nullptr, nullptr, nullptr))
            {
                if (!launcher.start() && strlen(launcher.error()))
                {
                    QString msg;
                    msg = QString("Error while launching yarpread. %1").arg(launcher.error());
                    logger->addError(msg.toLatin1().data());
                    reportErrors();
                }
                else
                {
                    // waiting for the port to get open
                    double base = yarp::os::SystemClock::nowSystem();
                    while(!timeout(base, 3.0)) {
                        if (launcher.exists(to.toLatin1().data())) {
                            break;
                        }
                    }
                    if (!launcher.connect(from.toLatin1().data(), to.toLatin1().data(), "udp")) {
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


    yarp::os::SystemClock::delaySystem(0.1);

}

/*! \brief Launch YARPScope Inspection modality*/
void ApplicationViewWidget::onYARPScope()
{
    if (safeManager.busy()) {
        return;
    }
    yarp::manager::ErrorLogger* logger  = yarp::manager::ErrorLogger::Instance();

    YscopeWindow dlg;
    dlg.setModal(true);
    if (dlg.exec() != QDialog::Accepted) {
        return;
    }
    int strIndex = dlg.getIndex();




    for(int i=0;i<ui->connectionList->topLevelItemCount();i++) {
        QTreeWidgetItem *it = ui->connectionList->topLevelItem(i);
        if (it->isSelected()) {
            QString from = it->text(3);
            QString to = QString("/inspect").arg(from);
            QString env = QString("YARP_PORT_PREFIX=%1").arg(to);
            to += "/yarpscope";

            QString param;
            param = QString("--title %1:%2 --bgcolor white --color blue --graph_size 2 --index %2").arg(from).arg(strIndex);


            yarp::manager::LocalBroker launcher;
            if (launcher.init("yarpscope", param.toLatin1().data(), nullptr, nullptr, nullptr, env.toLatin1().data())) {
                if (!launcher.start() && strlen(launcher.error())) {
                    QString msg;
                    msg = QString("Error while launching yarpscope. %1").arg(launcher.error());
                    logger->addError(msg.toLatin1().data());
                    reportErrors();
                }
                else{
                    // waiting for the port to get open
                    double base = yarp::os::SystemClock::nowSystem();
                    while(!timeout(base, 3.0))
                        if (launcher.exists(to.toLatin1().data())) break;
                    if (!launcher.connect(from.toLatin1().data(), to.toLatin1().data(), "udp")) {
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
    yarp::os::SystemClock::delaySystem(0.1);
}



bool ApplicationViewWidget::timeout(double base, double timeout)
{
    yarp::os::SystemClock::delaySystem(1.0);
    if ((yarp::os::SystemClock::nowSystem()-base) > timeout)
        return true;
    return false;
}




/*! \brief Tells if a modules is in running state*/
bool ApplicationViewWidget::isRunning()
{
    if (editingMode) {
        return false;
    }
    for(int i=0; i< ui->moduleList->topLevelItemCount();i++) {
        if (ui->moduleList->topLevelItem(i)->text(2) == "running") {
            return true;
        }
    }
    return false;
}

bool ApplicationViewWidget::isEditingMode()
{
    return editingMode;
}

void ApplicationViewWidget::onSelfConnect(int which)
{
    int row;
    if (!getConRowByID(which, &row))
    {
        yError()<<"ApplicationViewWidget: unable to find row with id:"<<which;
        return;
    }

    QTreeWidgetItem *it = ui->connectionList->topLevelItem(row);
    if (it) {
        it->setText(2,"connected");
        it->setIcon(0,QIcon(":/connect22.svg"));
        it->setForeground(2,QColor("#008C00"));
        QString from = it->text(3);
        QString to = it->text(4);
        auto* box = qobject_cast<QComboBox*>(ui->connectionList->itemWidget((QTreeWidgetItem *)it, 5));
        box->setEnabled(false);
        builder->setConnectionConnected(true,from,to);
    }

           /* row[m_conColumns.m_col_status] = "connected";
            row[m_conColumns.m_col_editable] = false;
            row[m_conColumns.m_col_color] = Gdk::Color("#008C00");
            row.set_value(0, m_refPixConnected);*/

    reportErrors();
}

void ApplicationViewWidget::onSelfDisconnect(int which)
{
    int row;
    if (!getConRowByID(which, &row))
    {
        yError()<<"ApplicationViewWidget: unable to find row with id:"<<which;
        return;
    }

    QTreeWidgetItem *it = ui->connectionList->topLevelItem(row);
    if (it) {
        it->setText(2,"disconnected");
        it->setIcon(0,QIcon(":/disconnect22.svg"));
        it->setForeground(2,QColor("#BF0303"));
        QString from = it->text(3);
        QString to = it->text(4);
        auto* box = qobject_cast<QComboBox*>(ui->connectionList->itemWidget((QTreeWidgetItem *)it, 5));
        box->setEnabled(true);
        builder->setConnectionConnected(false,from,to);
    }
    reportErrors();
}

void ApplicationViewWidget::onSelfResAvailable(int which)
{
    int row;
    if (!getResRowByID(which, &row))
    {
        yError()<<"ApplicationViewWidget: unable to find row with id:"<<which;
        return;
    }

    QTreeWidgetItem *it = ui->resourcesList->topLevelItem(row);
    if (it) {
        it->setText(3,"available");
        if (it->text(2) == "computer") {
            it->setIcon(0,QIcon(":/computer-available22.svg"));
            it->setForeground(3,QColor("#008C00"));
        } else {
            it->setIcon(0,QIcon(":/port-available22.svg"));
            it->setForeground(3,QColor("#008C00"));
        }
        /*if (row[m_resColumns.m_col_type] == Glib::ustring("computer"))
            row.set_value(0, m_refPixAvailable);
        else
            row.set_value(0, m_refPixPortAvaibable);*/
    }


    reportErrors();
}

void ApplicationViewWidget::onSelfResUnavailable(int which)
{
    int row;
    if (!getResRowByID(which, &row))
    {
        yError()<<"ApplicationViewWidget: unable to find row with id:"<<which;
        return;
    }

    QTreeWidgetItem *it = ui->resourcesList->topLevelItem(row);
    if (it) {
        it->setText(3,"unavailable");
        if (it->text(2) == "computer") {
            it->setIcon(0,QIcon(":/computer-unavailable22.svg"));
            it->setForeground(3,QColor("#BF0303"));
        } else {
            it->setIcon(0,QIcon(":/port-unavailable22.svg"));
            it->setForeground(3,QColor("#BF0303"));
        }
    }
    reportErrors();
}

void ApplicationViewWidget::onSelfStart(int which)
{
    QTreeWidgetItem *it = getModRowByID(which);
    if (it) {
        it->setText(2,"running");
        it->setIcon(0,QIcon(":/apply.svg"));
        it->setForeground(2,QColor("#008C00"));
        builder->setModuleRunning(true,which);
        //row[m_modColumns.m_col_editable] = false;
        //row[m_modColumns.m_col_color] = Gdk::Color("#008C00");
        //row.set_value(0, m_refPixRunning);
    }

    reportErrors();
}

void ApplicationViewWidget::onSelfStop(int which)
{
    QTreeWidgetItem *it = getModRowByID(which);
    if (it) {
        it->setText(2,"stopped");
        it->setIcon(0,QIcon(":/close.svg"));
        it->setForeground(2,QColor("#BF0303"));
        builder->setModuleRunning(false,which);
    }
    reportErrors();
}

/*! \brief Called when a modlue has been started
    \param which
*/
void ApplicationViewWidget::onModStart(int which)
{
    emit selfStart(which);
}

/*! \brief Called when a modlue has been stopped
    \param which
*/
void ApplicationViewWidget::onModStop(int which)
{
    emit selfStop(which);
}
/*! \brief Called when a modlue has writes on stdout
    \param which
    \param msg the message
*/
void ApplicationViewWidget::onModStdout(int which, const char* msg)
{
    QString s = QString("%1").arg(msg);
    for(int j=0;j<ui->moduleList->topLevelItemCount();j++) {
        QTreeWidgetItem *it = ui->moduleList->topLevelItem(j);
        if (it->data(0,Qt::UserRole) == APPLICATION) {
            modStdOutNestedApplication(it,which,s);
        } else {
            auto* stdouWin = qvariant_cast<StdoutWindow *>(it->data(0,Qt::UserRole));
            if (stdouWin && stdouWin->getId() == which) {
                stdouWin->addMessage(s);
                break;
            }
        }

    }
}

void ApplicationViewWidget::modStdOutNestedApplication(QTreeWidgetItem *it, int id,QString s)
{
    for(int j=0; j<it->childCount();j++) {
        QTreeWidgetItem *ch = it->child(j);
        if (ch->data(0,Qt::UserRole) == APPLICATION) {
            modStdOutNestedApplication(ch,id,s);
        } else {
            auto* stdouWin = qvariant_cast<StdoutWindow *>(ch->data(0,Qt::UserRole));
            if (stdouWin && stdouWin->getId() == id) {
                stdouWin->addMessage(s);
                break;
            }
        }
    }
}

/*! \brief Called when a connection has been performed
    \param which
*/
void ApplicationViewWidget::onConConnect(int which)
{
    emit selfConnect(which);
}

/*! \brief Called when a disconnection has been performed
    \param which
*/
void ApplicationViewWidget::onConDisconnect(int which)
{
    emit selfDisconnect(which);
}

/*! \brief Called when a resource became available
    \param which
*/
void ApplicationViewWidget::onResAvailable(int which)
{
    emit selfResAvailable(which);
}

/*! \brief Called when a resource become unavailable
    \param which
*/
void ApplicationViewWidget::onResUnAvailable(int which)
{
    emit selfResUnavailable(which);
}

/*! \brief Called when a connection become available
    \param which
*/
void ApplicationViewWidget::onConAvailable(int from, int to)
{
    if (from >= 0) {
        int row;
        if (getConRowByID(from, &row)) {
            ui->connectionList->topLevelItem(row)->setForeground(3,QColor("#008C00"));
            builder->setOutputPortAvailable(ui->connectionList->topLevelItem(row)->text(3),true);
        }
    }

    if (to >= 0) {
        int row;
        if (getConRowByID(to, &row)) {
            ui->connectionList->topLevelItem(row)->setForeground(4,QColor("#008C00"));
            builder->setInputPortAvailable(ui->connectionList->topLevelItem(row)->text(4),true);
        }
    }
    reportErrors();
}

/*! \brief Called when a connection become unavailable
    \param which
*/
void ApplicationViewWidget::onConUnAvailable(int from, int to)
{
    if (from >= 0) {
        int row;
        if (getConRowByID(from, &row)) {
            ui->connectionList->topLevelItem(row)->setForeground(3,QColor("#BF0303"));
            builder->setOutputPortAvailable(ui->connectionList->topLevelItem(row)->text(3),false);
        }
    }

    if (to >= 0) {
        int row;
        if (getConRowByID(to, &row)) {
            ui->connectionList->topLevelItem(row)->setForeground(4,QColor("#BF0303"));
            builder->setInputPortAvailable(ui->connectionList->topLevelItem(row)->text(4),false);
        }
    }
    reportErrors();
}

/*! \brief Called when an error occurred

*/
void ApplicationViewWidget::onError()
{
    reportErrors();
}

/*! \brief Refresh all and reports errors
*/
void ApplicationViewWidget::onLoadBalance()
{
    emit selfSafeLoadBolance();
}



void ApplicationViewWidget::onDetachStdout()
{

}

/*! \brief Get the connection row by id
    \param id the requested id
    \param the output row
*/
bool ApplicationViewWidget::getConRowByID(int id, int *row)
{
    for(int i=0;i< ui->connectionList->topLevelItemCount();i++) {
        QTreeWidgetItem *it = ui->connectionList->topLevelItem(i);

        if (it->text(1).toInt() == id) {
            *row = i;
            return true;
        }
    }
    return false;
}

/*! \brief Get the resource row by id
    \param id the requested id
    \param the output row
*/
bool ApplicationViewWidget::getResRowByID(int id, int *row)
{
    for(int i=0;i< ui->resourcesList->topLevelItemCount();i++) {
        QTreeWidgetItem *it = ui->resourcesList->topLevelItem(i);

        if (it->text(1).toInt() == id) {
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
QTreeWidgetItem* ApplicationViewWidget::getModRowByID(int id, QTreeWidgetItem *parent)
{
    QTreeWidgetItem *ret = nullptr;
    if (!parent) {
        for(int i=0;i< ui->moduleList->topLevelItemCount();i++) {
            QTreeWidgetItem *it = ui->moduleList->topLevelItem(i);
            if (it->data(0,Qt::UserRole) == APPLICATION) {
                ret = getModRowByID(id,it);
                if (ret) {
                    break;
                }
            }

            if (it->text(1).toInt() == id) {
                return it;
            }
        }
    } else {
        for(int i=0;i< parent->childCount();i++) {
            QTreeWidgetItem *it = parent->child(i);
            if (it->data(0,Qt::UserRole) == APPLICATION) {
                ret = getModRowByID(id,it);
                if (ret) {
                    break;
                }
            }

            if (it->text(1).toInt() == id) {
                return it;
            }
        }
    }

    return ret;
}


void ApplicationViewWidget::closeManager() {
    safeManager.close();
}
