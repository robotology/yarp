#include "applicationviewwidget.h"
#include "ui_applicationviewwidget.h"
#include <QMessageBox>
#include <QMenu>
#include <QFileDialog>
#include "localbroker.h"

ApplicationViewWidget::ApplicationViewWidget(Application *app,Manager *lazyManager,yarp::os::Property* config,QWidget *parent) :
    GenericViewWidget(parent), ApplicationEvent(),
    ui(new Ui::ApplicationViewWidget)
{
    ui->setupUi(this);
    lazy = lazyManager;
    this->app = app;
    m_pConfig = config;

    type = APPLICATION;

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

    createModulesViewContextMenu();
    createConnectionsViewContextMenu();
    createResourcesViewContextMenu();



}

ApplicationViewWidget::~ApplicationViewWidget()
{
    delete ui;
}

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
}

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

    connYarpViewAction = new QAction("yarpview",connSubMenu);
    connYarpReadAction = new QAction("yarpread",connSubMenu);
    connYarpHearAction = new QAction("yarphear",connSubMenu);
    connYarpScopeAction = new QAction("yarpscope",connSubMenu);

    connSelectAllAction->setIcon(QIcon(":/images/selectall_ico.png"));
    connRefreshAction->setIcon(QIcon(":/images/progress_ico.png"));

    connect(connConnectAction,SIGNAL(triggered()),this,SLOT(onConnect()));
    connect(connDisconnectAction,SIGNAL(triggered()),this,SLOT(onDisconnect()));
    connect(connRefreshAction,SIGNAL(triggered()),this,SLOT(onRefresh()));
    connect(connSelectAllAction,SIGNAL(triggered()),this,SLOT(selectAllConnections()));

    connect(connYarpViewAction,SIGNAL(triggered()),this,SLOT(onYarpView()));
    connect(connYarpReadAction,SIGNAL(triggered()),this,SLOT(onYarpRead()));
    connect(connYarpHearAction,SIGNAL(triggered()),this,SLOT(onYarpHear()));
    connect(connYarpScopeAction,SIGNAL(triggered()),this,SLOT(onYarpScope()));

    connContex->addAction(connConnectAction);
    connContex->addAction(connDisconnectAction);
    connContex->addAction(connSeparatorAction);
    connContex->addAction(connRefreshAction);
    connContex->addAction(connSelectAllAction);
    connContex->addAction(conn1SeparatorAction);
    connContex->addMenu(connSubMenu);

    connSubMenu->addAction(connYarpViewAction);
    connSubMenu->addAction(connYarpReadAction);
    connSubMenu->addAction(connYarpHearAction);
    connSubMenu->addAction(connYarpScopeAction);


    ui->connectionList->setContextMenu(connContex);

}

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

void ApplicationViewWidget::onResourceItemSelectionChanged()
{
    if(ui->resourcesList->currentItem() == NULL){
        resRefreshAction->setEnabled(false);
    }else{
        resRefreshAction->setEnabled(true);
    }
}

void ApplicationViewWidget::prepareManagerFrom(Manager* lazy)
{

    manager.prepare(lazy, m_pConfig,
                    dynamic_cast<ApplicationEvent*>(this));

    // loading application
    if(manager.loadApplication(app->getName())){
       updateApplicationWindow();
    }

    reportErrors();
}

void ApplicationViewWidget::updateApplicationWindow()
{

    ui->moduleList->clear();
    ui->connectionList->clear();
    ui->resourcesList->clear();



    ExecutablePContainer modules = manager.getExecutables();
    CnnContainer connections  = manager.getConnections();
    ExecutablePIterator moditr;
    CnnIterator cnnitr;

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
        QTreeWidgetItem *it = new QTreeWidgetItem(ui->moduleList,l);
        //it->setFlags(it->flags() | Qt::ItemIsEditable);
        it->setData(0,Qt::UserRole,MODULE);
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
        QTreeWidgetItem *it = new QTreeWidgetItem(ui->connectionList,l);
        ui->moduleList->addTopLevelItem(it);
        it->setData(0,Qt::UserRole,INOUTD);
        it->setIcon(0,QIcon(":/images/disconnected_ico.png"));
        it->setTextColor(2,QColor("#BF0303"));
        id++;
    }

    id = 0;
    ResourcePIterator itrS;
    for(itrS=manager.getResources().begin(); itrS!=manager.getResources().end(); itrS++)
    {
        //m_resRow = *(m_refTreeResModel->append());
        QString sId = QString("%1").arg(id);
        QString type;
        if(dynamic_cast<Computer*>(*itrS))
        {
            type = "computer";
        }
        else if(dynamic_cast<ResYarpPort*>(*itrS))
        {
            type = "port";
        }
        QString res = QString("%1").arg((*itrS)->getName());
        QString status = "unknown";
        //m_resRow[m_resColumns.m_col_color] = Gdk::Color("#00000");

        QStringList l;
        l << res << sId << type << status ;
        QTreeWidgetItem *it = new QTreeWidgetItem(ui->resourcesList,l);
        ui->moduleList->addTopLevelItem(it);
        it->setData(0,Qt::UserRole,RESOURCE);
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
void ApplicationViewWidget::onItemDoubleClicked(QTreeWidgetItem *it,int col)
{
    Qt::ItemFlags tmp = it->flags();
    if (isEditable(it, col)) {
        it->setFlags(tmp | Qt::ItemIsEditable);
    } else if (tmp & Qt::ItemIsEditable) {
        it->setFlags(tmp ^ Qt::ItemIsEditable);
    }
}

bool ApplicationViewWidget::isEditable(QTreeWidgetItem *it,int col)
{
    NodeType type = (NodeType)it->data(0,Qt::UserRole).toInt();

    switch (type) {
    case MODULE:{
           if(col == 3 || col == 4 || col == 5 || col == 6 || col == 7){
               if(it->text(2) == "stopped"){
                    return true;
               }
           }
        break;
    }
    case INOUTD:{
           if(col == 3 || col == 4 || col == 5 ){
               if(it->text(2) == "disconnected"){
                    return true;
               }
           }
        break;
    }
    case RESOURCE:{
           return false;
        break;
    }
    default:
        break;
    }

    return false;
}


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
            it->setIcon(0,QIcon(":/images/progress_ico.png"));
            it->setTextColor(2,QColor("#000000"));

        }
    }


    manager.safeRun(MIDs);
    yarp::os::Time::delay(0.1);
    selectAllModule(false);
    return true;
}

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
            it->setIcon(0,QIcon(":/images/progress_ico.png"));
            it->setTextColor(2,QColor("#000000"));

        }
    }


    manager.safeStop(MIDs);
    yarp::os::Time::delay(0.1);
    selectAllModule(false);
    return true;
}

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
}

void ApplicationViewWidget::selectAllModule()
{
    selectAllModule(true);
}

void ApplicationViewWidget::selectAllConnections()
{
    selectAllConnections(true);
}

void ApplicationViewWidget::selectAllResources()
{
   selectAllResources(true);
}

void ApplicationViewWidget::selectAllModule(bool check)
{
    for(int i=0;i<ui->moduleList->topLevelItemCount();i++){
        QTreeWidgetItem *it = ui->moduleList->topLevelItem(i);
        it->setSelected(check);
    }
}

void ApplicationViewWidget::selectAllConnections(bool check)
{
    for(int i=0;i<ui->connectionList->topLevelItemCount();i++){
        QTreeWidgetItem *it = ui->connectionList->topLevelItem(i);
        it->setSelected(check);
    }
}

void ApplicationViewWidget::selectAllResources(bool check)
{
    for(int i=0;i<ui->resourcesList->topLevelItemCount();i++){
        QTreeWidgetItem *it = ui->resourcesList->topLevelItem(i);
        it->setSelected(check);
    }
}

void ApplicationViewWidget::selectAll()
{
    selectAllConnections(true);
    selectAllModule(true);
    selectAllResources(true);
}


void ApplicationViewWidget::runApplicationSet()
{
    selectAllModule(true);
    onRun();
}

void ApplicationViewWidget::stopApplicationSet()
{
    selectAllModule(true);
    onStop();
}

void ApplicationViewWidget::killApplicationSet()
{
    selectAllModule(true);
    onKill();
}

void ApplicationViewWidget::connectConnectionSet()
{
    selectAllConnections(true);
    onConnect();
}

void ApplicationViewWidget::disconnectConnectionSet()
{
    selectAllConnections(true);
    onDisconnect();
}

void ApplicationViewWidget::refresh()
{
    selectAllConnections(true);
    selectAllModule(true);
    selectAllResources(true);

    onRefresh();
}


void ApplicationViewWidget::reportErrors()
{
    ErrorLogger* logger  = ErrorLogger::Instance();
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



void ApplicationViewWidget::exportGraph()
{
    QString fileName = QFileDialog::getSaveFileName(this,"Export Graph",QApplication::applicationDirPath(),"GraphViz format (*.dot)");

    if(!fileName.isEmpty()){
        if(!manager.exportDependencyGraph(fileName.toLatin1().data()))
        {
            ErrorLogger* logger  = ErrorLogger::Instance();
            logger->addError("Cannot export graph");
            reportErrors();
            return;
        }
    }
}



void ApplicationViewWidget::onYarpView()
{
    if(manager.busy()){
        return;
    }
    ErrorLogger* logger  = ErrorLogger::Instance();

    for(int i=0;i<ui->connectionList->topLevelItemCount();i++){
        QTreeWidgetItem *it = ui->connectionList->topLevelItem(i);
        if(it->isSelected()){
            QString from = it->text(3);
            QString to = QString("/inspect").arg(from);
            QString env = QString("YARP_PORT_PREFIX=%1").arg(to);
            to += "/yarpview/img:i";

            LocalBroker launcher;
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

void ApplicationViewWidget::onYarpHear()
{
    if(manager.busy()){
        return;
    }
    ErrorLogger* logger  = ErrorLogger::Instance();

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

            LocalBroker launcher;
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

void ApplicationViewWidget::onYarpRead()
{
    if(manager.busy()){
        return;
    }
    ErrorLogger* logger  = ErrorLogger::Instance();

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

            LocalBroker launcher;
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

void ApplicationViewWidget::onYarpScope()
{
//    if(manager.busy()){
//        return;
//    }
//    ErrorLogger* logger  = ErrorLogger::Instance();

//    for(int i=0;i<ui->connectionList->topLevelItemCount();i++){
//        QTreeWidgetItem *it = ui->connectionList->topLevelItem(i);
//        if(it->isSelected()){
//            QString from = it->text(3);
//            QString to = QString("/inspect").arg(from);
//            QString env = QString("YARP_PORT_PREFIX=%1").arg(to);
//            to += "/yarpscope";

//            QString param;
//            param = QString("--title %1"<< from << ":" << strIndex << " --bgcolor white --color blue --graph_size 2 --index " + strIndex;
//        }
//    }

//    for(unsigned int i=0; i<m_ConnectionIDs.size(); i++)
//    {
//        Gtk::TreeModel::Row row;
//        if(getConRowByID(m_ConnectionIDs[i], &row))
//        {

//            OSTRINGSTREAM param;
//            param << "--title "<< from << ":" << strIndex << " --bgcolor white --color blue --graph_size 2 --index " + strIndex;
//            LocalBroker launcher;
//            if(launcher.init("yarpscope", param.str().c_str(), NULL, NULL, NULL, env.c_str()))
//            {
//                if(!launcher.start() && strlen(launcher.error()))
//                {
//                    OSTRINGSTREAM msg;
//                    msg<<"Error while launching yarpscope";
//                    msg<<". "<<launcher.error();
//                    logger->addError(msg);
//                    reportErrors();
//                }
//                else
//                {
//                    // waiting for the port to get open
//                    double base = yarp::os::Time::now();
//                    while(!timeout(base, 3.0))
//                        if(launcher.exists(to.c_str())) break;
//                    if(!launcher.connect(from.c_str(), to.c_str(), "udp"))
//                    {
//                        OSTRINGSTREAM msg;
//                        msg<<"Cannot inspect '"<<from<<"' : "<<launcher.error();
//                        logger->addError(msg);
//                        launcher.stop();
//                        reportErrors();
//                    }
//                }
//            }
//        }
//    }
//    yarp::os::Time::delay(0.1);
//    m_refTreeConSelection->unselect_all();
}



bool ApplicationViewWidget::timeout(double base, double timeout)
{
    yarp::os::Time::delay(1.0);
    if((yarp::os::Time::now()-base) > timeout)
        return true;
    return false;
}





bool ApplicationViewWidget::isRunning()
{
    for(int i=0; i< ui->moduleList->topLevelItemCount();i++){
        if(ui->moduleList->topLevelItem(i)->text(2) == "running"){
            return true;
        }
    }
    return false;
}

void ApplicationViewWidget::onModStart(int which)
{
    QTreeWidgetItem *it = ui->moduleList->topLevelItem(which);
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
void ApplicationViewWidget::onModStop(int which)
{
    QTreeWidgetItem *it = ui->moduleList->topLevelItem(which);
    if(it){
        it->setText(2,"stopped");
        it->setIcon(0,QIcon(":/images/suspended_ico.png"));
        it->setTextColor(2,QColor("#BF0303"));
    }
    reportErrors();
}
void ApplicationViewWidget::onModStdout(int which, const char* msg) {}
void ApplicationViewWidget::onConConnect(int which)
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
void ApplicationViewWidget::onConDisconnect(int which)
{
    QTreeWidgetItem *it = ui->connectionList->topLevelItem(which);
    if(it){
        it->setText(2,"disconnected");
        it->setIcon(0,QIcon(":/images/disconnected_ico.png"));
        it->setTextColor(2,QColor("#BF0303"));
    }
    reportErrors();
}
void ApplicationViewWidget::onResAvailable(int which)
{

    QTreeWidgetItem *it = ui->resourcesList->topLevelItem(which);
    if(it){
        it->setText(3,"avaiable");
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
void ApplicationViewWidget::onResUnAvailable(int which)
{
    QTreeWidgetItem *it = ui->resourcesList->topLevelItem(which);
    if(it){
        it->setText(3,"unavaiable");
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
void ApplicationViewWidget::onConAvailable(int from, int to) {}
void ApplicationViewWidget::onConUnAvailable(int from, int to) {}
void ApplicationViewWidget::onError(void) {}
void ApplicationViewWidget::onLoadBalance(void) {}

