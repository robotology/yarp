/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <yarp/conf/version.h>
#include <yarp/conf/filesystem.h>
#include <yarp/os/Log.h>
#include <yarp/os/ResourceFinder.h>
#include <dirent.h>
#include <yarp/manager/xmlapploader.h>
#include <yarp/manager/xmltemploader.h>
#include <yarp/manager/localbroker.h>
#include <yarp/profiler/NetworkProfiler.h>

#include "moduleviewwidget.h"
#include "applicationviewwidget.h"
#include "resourceviewwidget.h"
#include "yarpbuilderlib.h"

#include "template_res.h"
#include "aboutdlg.h"

#include <QGridLayout>
#include <QMessageBox>
#include <QFileDialog>
#include <QDesktopServices>
#include <QInputDialog>

#include <QWizardPage>
#include <QLabel>
#include <QLineEdit>



#if defined(_WIN32)
# pragma warning (disable : 4250)
# pragma warning (disable : 4520)
#else
# include <unistd.h>
# include <cerrno>
# include <csignal>
# if defined(YARP_HAS_SYS_TYPES_H)
#  include <sys/types.h>
# endif
# if defined(YARP_HAS_SYS_WAIT_H)
#  include <sys/wait.h>
# endif
#endif

#ifndef APP_NAME
 #define APP_NAME "yarpmanager"
#endif

using namespace std;


bool isAbsolute(const char *path) {  //copied from yarp_OS ResourceFinder.cpp
   if (path[0]=='/'||path[0]=='\\') {
       return true;
   }
   std::string str(path);
   if (str.length()>1) {
       if (str[1]==':') {
           return true;
       }
   }
   return false;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qRegisterMetaType< QVector<int> >("QVector<int>");
    ui->mainToolBar->setContextMenuPolicy(Qt::PreventContextMenu);
    ui->menuBar->setContextMenuPolicy(Qt::PreventContextMenu);
    auto label = new QLabel("", ui->statusBar);
    auto yarpino = new QLabel("", ui->statusBar);
    QPixmap pixmap(QPixmap(":/yarp-robot-22.png").scaledToHeight(ui->statusBar->height()));
    yarpino->setPixmap(pixmap);
    label->setText("Powered by");
    ui->statusBar->insertPermanentWidget(0,label);
    ui->statusBar->insertPermanentWidget(1,yarpino);
    prevWidget = nullptr;

    watcher = new QFileSystemWatcher(this);



    connect(ui->entitiesTree,SIGNAL(viewResource(yarp::manager::Computer*)),this,SLOT(viewResource(yarp::manager::Computer*)));
    connect(ui->entitiesTree,SIGNAL(viewModule(yarp::manager::Module*)),this,SLOT(viewModule(yarp::manager::Module*)));
    connect(ui->entitiesTree,SIGNAL(viewApplication(yarp::manager::Application*,bool)),this,SLOT(viewApplication(yarp::manager::Application*,bool)));
    connect(ui->entitiesTree,SIGNAL(openFiles()),this,SLOT(onOpen()));
    connect(ui->entitiesTree,SIGNAL(importFiles()),this,SLOT(onImportFiles()));
    connect(ui->entitiesTree,SIGNAL(removeApplication(QString, QString)),this,SLOT(onRemoveApplication(QString, QString)));
    connect(ui->entitiesTree,SIGNAL(removeModule(QString)),this,SLOT(onRemoveModule(QString)));
    connect(ui->entitiesTree,SIGNAL(removeResource(QString)),this,SLOT(onRemoveResource(QString)));
    connect(ui->entitiesTree,SIGNAL(reopenApplication(QString,QString)),this,SLOT(onReopenApplication(QString,QString)),Qt::DirectConnection);
    connect(ui->entitiesTree,SIGNAL(reopenModule(QString,QString)),this,SLOT(onReopenModule(QString,QString)),Qt::DirectConnection);
    connect(ui->entitiesTree,SIGNAL(reopenResource(QString,QString)),this,SLOT(onReopenResource(QString,QString)),Qt::DirectConnection);

    connect(ui->actionRun_all,SIGNAL(triggered()),this,SLOT(onRun()));
    connect(ui->actionStop_all,SIGNAL(triggered()),this,SLOT(onStop()));
    connect(ui->actionKill_all,SIGNAL(triggered()),this,SLOT(onKill()));
    connect(ui->actionConnect_all,SIGNAL(triggered()),this,SLOT(onConnect()));
    connect(ui->actionDisconnect_all,SIGNAL(triggered()),this,SLOT(onDisconnect()));

    connect(ui->actionRun,SIGNAL(triggered()),this,SLOT(onRunSelected()));
    connect(ui->actionStop,SIGNAL(triggered()),this,SLOT(onStopSelected()));
    connect(ui->actionKill,SIGNAL(triggered()),this,SLOT(onKillSelected()));
    connect(ui->actionConnect,SIGNAL(triggered()),this,SLOT(onConnectSelected()));
    connect(ui->actionDisconnect,SIGNAL(triggered()),this,SLOT(onDisconnectSelected()));

    connect(ui->actionRefresh_Status,SIGNAL(triggered()),this,SLOT(onRefresh()));
    connect(ui->actionSelect_All,SIGNAL(triggered()),this,SLOT(onSelectAll()));
    connect(ui->actionExport_Graph,SIGNAL(triggered()),this,SLOT(onExportGraph()));
    connect(ui->actionNew_Application,SIGNAL(triggered()),this,SLOT(onNewApplication()));
    connect(ui->actionImport_Files,SIGNAL(triggered()),this,SLOT(onImportFiles()));
    connect(ui->mainTabs,SIGNAL(tabCloseRequested(int)),this,SLOT(onTabClose(int)));
    connect(ui->mainTabs,SIGNAL(currentChanged(int)),this,SLOT(onTabChangeItem(int)));
    connect(ui->actionNew_Module,SIGNAL(triggered()),this,SLOT(onNewModule()));
    connect(ui->actionNew_Resource,SIGNAL(triggered()),this,SLOT(onNewResource()));
    connect(ui->actionClose,SIGNAL(triggered()),this,SLOT(onClose()));
    connect(ui->actionQuit,SIGNAL(triggered()),this,SLOT(close()));
    connect(ui->actionOpen_File,SIGNAL(triggered()),this,SLOT(onOpen()));
    connect(ui->actionSave,SIGNAL(triggered()),this,SLOT(onSave()));
    connect(ui->actionSave_As,SIGNAL(triggered()),this,SLOT(onSaveAs()));
    connect(ui->actionHelp,SIGNAL(triggered()),this,SLOT(onHelp()));
    connect(ui->actionAbout,SIGNAL(triggered()),this,SLOT(onAbout()));
    connect(ui->actionYarpClean, SIGNAL(triggered()),this, SLOT(onYarpClean()));
    connect(ui->actionYarpNameList, SIGNAL(triggered()),this, SLOT(onYarpNameList()));

    connect(this,SIGNAL(selectItem(QString, bool)),ui->entitiesTree,SLOT(onSelectItem(QString, bool)));

    connect(watcher, SIGNAL(fileChanged(const QString &)), this, SLOT(onFileChanged(const QString &)));

    //Adding actions for making the window listen key events(shortcuts)
    this->addAction(ui->actionQuit);
    this->addAction(ui->actionSave);
    this->addAction(ui->actionSave_As);
    this->addAction(ui->actionClose);
    this->addAction(ui->actionRefresh_Status);
    onTabChangeItem(-1);

    ui->actionConnect->setEnabled(false);
    ui->actionDisconnect->setEnabled(false);
    ui->actionRun->setEnabled(false);
    ui->actionStop->setEnabled(false);
    ui->actionKill->setEnabled(false);

    ui->actionAuto_reload->setChecked(true);

    ui->action_Manager_Window->setChecked(true);
#ifdef WIN32
    ui->tabWidgetLeft->tabBar()->hide();
#else
    yarp::os::ResourceFinder& rf = yarp::os::ResourceFinder::getResourceFinderSingleton();

    std::string confFile = rf.findFileByName("cluster-config.xml");
    if (!confFile.empty())
    {
        ui->clusterWidget->setConfigFile(confFile);
        ui->clusterWidget->init();
        connect(ui->clusterWidget, SIGNAL(logError(QString)), this, SLOT(onLogError(QString)));
        connect(ui->clusterWidget, SIGNAL(logMessage(QString)), this, SLOT(onLogMessage(QString)));
    }
    else
    {
        ui->tabWidgetLeft->tabBar()->hide();
    }
#endif

}

MainWindow::~MainWindow()
{

    delete ui;
}
void MainWindow::closeEvent(QCloseEvent *event)
{
    for(int i=0;i<ui->mainTabs->count();i++){
        if(!onTabClose(i)){
            event->ignore();
            break;
        }
    }

}

void MainWindow::onWizardError(QString err)
{
    yarp::manager::ErrorLogger* logger  = yarp::manager::ErrorLogger::Instance();
    logger->addError(err.toLatin1().data());
    reportErrors();
}

/*! \brief Init the application with the current configuration.
 *
 *  \param config the configuration
 */
void MainWindow::init(yarp::os::Property config)
{
    this->config = config;
    const std::string directorySeparator{yarp::conf::filesystem::preferred_separator};

    string basepath=config.check("ymanagerini_dir", yarp::os::Value("")).asString();

    if(config.check("modpath")){
        string strPath;
        string modPaths(config.find("modpath").asString());
        while (modPaths!=""){
            string::size_type pos=modPaths.find(';');
            strPath=modPaths.substr(0, pos);
            yarp::manager::trimString(strPath);
            if (!isAbsolute(strPath.c_str()))
                strPath.insert(0, basepath);
            if((strPath.rfind(directorySeparator)==string::npos) ||
            (strPath.rfind(directorySeparator)!=strPath.size()-1))
                strPath.append(directorySeparator);
            lazyManager.addModules(strPath.c_str());
            if (pos==string::npos || pos==0)
                break;
            modPaths=modPaths.substr(pos+1);
        }
    }

    if(config.check("respath")){
        string strPath;
        string resPaths(config.find("respath").asString());
        while (resPaths!=""){
            string::size_type pos=resPaths.find(';');
            strPath=resPaths.substr(0, pos);
            yarp::manager::trimString(strPath);
            if (!isAbsolute(strPath.c_str()))
                strPath.insert(0, basepath);

            if((strPath.rfind(directorySeparator)==string::npos) ||
            (strPath.rfind(directorySeparator)!=strPath.size()-1))
                strPath.append(directorySeparator);

            lazyManager.addResources(strPath.c_str());
            if (pos==string::npos)
                break;
            resPaths=resPaths.substr(pos+1);
        }
    }

    yarp::manager::ErrorLogger* logger  = yarp::manager::ErrorLogger::Instance();


    if(config.check("apppath")){
        string strPath;
        string appPaths(config.find("apppath").asString());
        while (appPaths!=""){
            string::size_type pos=appPaths.find(';');
            strPath=appPaths.substr(0, pos);
            yarp::manager::trimString(strPath);
            if (!isAbsolute(strPath.c_str())){
                strPath.insert(0, basepath);
            }

            if((strPath.rfind(directorySeparator)==string::npos) ||
                (strPath.rfind(directorySeparator)!=strPath.size()-1)){
                    strPath.append(directorySeparator);
            }

            if(config.find("load_subfolders").asString() == "yes"){
                if(!loadRecursiveApplications(strPath.c_str())){
                    logger->addError("Cannot load the applications from  " + strPath);
                }
                loadRecursiveTemplates(strPath.c_str());
                ui->entitiesTree->header()->setSortIndicator(0,Qt::SortOrder::AscendingOrder);
            }
            else{
                lazyManager.addApplications(strPath.c_str());
            }
            if (pos==string::npos){
                break;
            }
            appPaths=appPaths.substr(pos+1);
        }
    }

    if (config.check("templpath")){
        string strPath;
        string templPaths(config.find("templpath").asString());
        while (templPaths!=""){
            string::size_type pos=templPaths.find(';');
            strPath=templPaths.substr(0, pos);
            yarp::manager::trimString(strPath);
            if (!isAbsolute(strPath.c_str())){
                strPath.insert(0, basepath);
            }

            if(!loadRecursiveTemplates(strPath.c_str())){
                logger->addError("Cannot load the templates from  " + strPath);
            }

            if (pos==string::npos){
                break;
            }
            templPaths=templPaths.substr(pos+1);
        }
    }

    if(config.check("external_editor")){
        ext_editor = config.find("external_editor").asString();
        ((EntitiesTreeWidget*)ui->entitiesTree)->setExtEditor(ext_editor);
    }else{
#if defined(_WIN32)
        ext_editor = "notepad.exe";
#else
        ext_editor = "xdg-open";
#endif
    }

    reportErrors();

    syncApplicationList();

    if(config.check("application")){
        yarp::manager::XmlAppLoader appload(config.find("application").asString().c_str());
        if(!appload.init()){
            return;
        }
        yarp::manager::Application* application = appload.getNextApplication();
        if(!application){
            return;  // TODO far ritornare valore per chiudere in caso di errore
        }
        // add this application to the manager if does not exist
        if(!lazyManager.getKnowledgeBase()->getApplication(application->getName())){
            lazyManager.getKnowledgeBase()->addApplication(application);
            syncApplicationList();
        }
        //manageApplication(application->getName());
    }
        onYarpNameList();
}

/*! \brief Reports tge error on the log window.
 */
void MainWindow::reportErrors()
{
    yarp::manager::ErrorLogger* logger  = yarp::manager::ErrorLogger::Instance();
    if(logger->errorCount() || logger->warningCount()){
        const char* err;
        while((err=logger->getLastError())){
            onLogError(err);
        }
        while((err=logger->getLastWarning())){
            onLogWarning(err);
        }
    }
}

/*! \brief Synchs the application list on filesystem with the application tree.
 */
void MainWindow::syncApplicationList(QString selectNodeForEditing, bool open)
{
    if (!listOfAppFiles.isEmpty())
    {
        watcher->removePaths(listOfAppFiles);
    }
    listOfAppFiles.clear();
    ui->entitiesTree->clearApplications();
    ui->entitiesTree->clearModules();
    ui->entitiesTree->clearResources();
    //ui->entitiesTree->clearTemplates();
    yarp::manager::KnowledgeBase* kb = lazyManager.getKnowledgeBase();
    yarp::manager::ApplicaitonPContainer apps =  kb->getApplications();
    unsigned int cnt = 0;
    for(auto& itr : apps){
        cnt++;
        auto* app = dynamic_cast<yarp::manager::Application*>(itr);
        if(app){
            ui->entitiesTree->addApplication(app);
            if(strcmp(selectNodeForEditing.toLatin1().data(),app->getName())==0){
                emit selectItem(selectNodeForEditing, open);
            }
            listOfAppFiles.push_back(app->getXmlFile());
        }
    }

    watcher->addPaths(listOfAppFiles);

    yarp::manager::ResourcePContainer resources = kb->getResources();
    for(auto& resource : resources) {
        auto* comp = dynamic_cast<yarp::manager::Computer*>(resource);
        if(comp){
            ui->entitiesTree->addComputer(comp);
        }
    }

    yarp::manager::ModulePContainer modules = kb->getModules();
    for(auto& module : modules) {
        auto* mod = dynamic_cast<yarp::manager::Module*>(module);
        if(mod){
            ui->entitiesTree->addModule(mod);
        }
    }


    if(cnt){
        QString msg = QString("%1 applications are loaded successfully.").arg(cnt);
        onLogMessage(msg);
    }
    else{
        QString msg = QString("No application is loaded!");
        onLogMessage(msg);
    }
}

/*! \brief Load the Application Templates recursively.
    \param szPath the path to the directory
 */
bool MainWindow::loadRecursiveTemplates(const char* szPath)
{
    const std::string directorySeparator{yarp::conf::filesystem::preferred_separator};
    string strPath = szPath;
    if((strPath.rfind(directorySeparator)==string::npos) ||
            (strPath.rfind(directorySeparator)!=strPath.size()-1)) {
        strPath.append(directorySeparator);
    }

    DIR *dir;
    struct dirent *entry;
    if ((dir = opendir(strPath.c_str())) == nullptr)
        return false;

    // loading from current folder
    yarp::manager::AppTemplate* tmp;
    yarp::manager::XmlTempLoader tempload(strPath.c_str(), nullptr);
    if(tempload.init())
    {
        while((tmp = tempload.getNextAppTemplate())){
            ui->entitiesTree->addAppTemplate(tmp);
        }
    }

    while((entry = readdir(dir)))
    {
        if((string(entry->d_name) != string("."))
        && (string(entry->d_name) != string("..")))
        {

            string name = strPath + string(entry->d_name);
            loadRecursiveTemplates(name.c_str());
        }
    }
    closedir(dir);

    return true;
}

/*! \brief Load the Applications recursively.
    \param szPath the path to the directory
 */
bool MainWindow::loadRecursiveApplications(const char* szPath)
{
    const std::string directorySeparator{yarp::conf::filesystem::preferred_separator};
    string strPath = szPath;
    if((strPath.rfind(directorySeparator)==string::npos) ||
            (strPath.rfind(directorySeparator)!=strPath.size()-1))
            strPath = strPath + directorySeparator;

    DIR *dir;
    struct dirent *entry;
    if ((dir = opendir(strPath.c_str())) == nullptr)
        return false;

    lazyManager.addApplications(strPath.c_str());

    while((entry = readdir(dir)))
    {
        if((string(entry->d_name) != string("."))
        && (string(entry->d_name) != string("..")))
        {
            string name = strPath + string(entry->d_name);
            loadRecursiveApplications(name.c_str());
        }
    }
    closedir(dir);
    return true;
}

bool MainWindow::initializeFile(string _class)
{
    QFile f(fileName);
    bool b = f.open(QIODevice::ReadWrite);
    QString appTemplate  = "<application>\n"
                 "      <name>" + currentAppName + "</name>\n"
                 "      <description>" + currentAppDescription + "</description>\n"
                 "      <version>" + currentAppVersion + "</version>\n"
                 "      <authors>\n"
                 "      </authors>\n"
                 "</application>\n";
    if(b){
        if(_class == "Resource")
            f.write(str_res_template.c_str());
        else if(_class == "Module")
            f.write(str_mod_template.c_str());
        else if(_class == "Application")
            f.write(appTemplate.toStdString().c_str());
        else
            return false;
        f.flush();
        f.close();
        return true;
    }else{
        yarp::manager::ErrorLogger* logger  = yarp::manager::ErrorLogger::Instance();
        QString err = QString("Cannot create %1").arg(fileName);
        logger->addError(err.toLatin1().data());
        reportErrors();
        return false;
    }
}

int MainWindow::getAppTabIndex(QString appName)
{
    for (int i=0; i<ui->mainTabs->count(); i++){
        if (ui->mainTabs->tabText(i) == appName){
            return i;
        }
    }
    return -1;
}

QString MainWindow::getAppNameFromXml(QString fileName)
{
    QString appName("");
    yarp::manager::KnowledgeBase* kb = lazyManager.getKnowledgeBase();
    yarp::manager::ApplicaitonPContainer apps =  kb->getApplications();
    for(auto& itr : apps)
    {
        auto* app = dynamic_cast<yarp::manager::Application*>(itr);
        if(app)
        {
            if(app->getXmlFile() == fileName.toStdString())
            {
                return app->getName();
            }
        }
    }
    return appName;
}

/*! \brief Load the Resource on the MainWindow
    \param res the resource
 */
void MainWindow::viewResource(yarp::manager::Computer *res)
{
    for(int i=0;i<ui->mainTabs->count();i++){
        if(ui->mainTabs->tabText(i) == res->getName()){
            ui->mainTabs->setCurrentIndex(i);
            return;
        }
    }

    auto* w = new ResourceViewWidget(res,ui->mainTabs);
    int index = ui->mainTabs->addTab(w,res->getName());
    ui->mainTabs->setTabIcon(index,QIcon(":/computer22.svg"));
    ui->mainTabs->setCurrentIndex(index);
}
/*! \brief Load the Module on the MainWindow
    \param module the module
 */

void MainWindow::viewModule(yarp::manager::Module *module)
{
    for(int i=0;i<ui->mainTabs->count();i++){
        if(ui->mainTabs->tabText(i) == module->getName()){
            ui->mainTabs->setCurrentIndex(i);
            return;
        }
    }

    auto* w = new ModuleViewWidget(module,ui->mainTabs);
    int index = ui->mainTabs->addTab(w,module->getName());
    ui->mainTabs->setTabIcon(index,QIcon(":/module22.svg"));
    ui->mainTabs->setCurrentIndex(index);
}

/*! \brief Load the Application on the MainWindow
    \param app the Application
 */
void MainWindow::viewApplication(yarp::manager::Application *app,bool editingMode)
{

    for (int i=0; i<ui->mainTabs->count(); i++){
        if (ui->mainTabs->tabText(i) == app->getName()){
            ui->mainTabs->setCurrentIndex(i);
            return;
        }
    }

    auto* w = new ApplicationViewWidget(app,&lazyManager,&config,editingMode,ui->mainTabs);
    connect(w,SIGNAL(logError(QString)),this,SLOT(onLogError(QString)));
    connect(w,SIGNAL(logWarning(QString)),this,SLOT(onLogWarning(QString)));
    connect(w,SIGNAL(modified(bool)),this,SLOT(onModified(bool)));
    int index = ui->mainTabs->addTab(w,app->getName());
    ui->mainTabs->setTabIcon(index,QIcon(":/run22.svg"));
    ui->mainTabs->setCurrentIndex(index);
    if(!editingMode)
    {
        connect(w->getConnectionList(),SIGNAL(itemSelectionChanged()),this,SLOT(onApplicationSelectionChanged()));
        connect(w->getModuleList(),SIGNAL(itemSelectionChanged()),this,SLOT(onApplicationSelectionChanged()));
    }
    if(editingMode){
        ui->actionSelect_All->setEnabled(false);
        ui->actionRefresh_Status->setEnabled(false);
        ui->actionExport_Graph->setEnabled(false);
        ui->actionConnect_all->setEnabled(false);
        ui->actionDisconnect_all->setEnabled(false);
        ui->actionRun_all->setEnabled(false);
        ui->actionStop_all->setEnabled(false);
        ui->actionKill_all->setEnabled(false);
        ui->actionConnect->setEnabled(false);
        ui->actionDisconnect->setEnabled(false);
        ui->actionRun->setEnabled(false);
        ui->actionStop->setEnabled(false);
        ui->actionKill->setEnabled(false);
    }else{
        ui->actionSelect_All->setEnabled(true);
        ui->actionRefresh_Status->setEnabled(true);
        ui->actionExport_Graph->setEnabled(true);
        ui->actionConnect_all->setEnabled(true);
        ui->actionDisconnect_all->setEnabled(true);
        ui->actionRun_all->setEnabled(true);
        ui->actionStop_all->setEnabled(true);
        ui->actionKill_all->setEnabled(true);
        onRefresh();
    }
}

/*! \brief Exports the actual Graph
 */
void MainWindow::onExportGraph()
{
    QWidget *w = ui->mainTabs->currentWidget();
    if(!w){
        return;
    }
    yarp::manager::NodeType type = ((GenericViewWidget*)w)->getType();
    if(type == yarp::manager::APPLICATION){
        auto* ww = (ApplicationViewWidget*)w;
        ww->exportGraph();
    }
}

/*! \brief When Run is clicked the applications in the current Application Tab will go on run state
 */
void MainWindow::onRun(bool onlySelected)
{
    QWidget *w = ui->mainTabs->currentWidget();
    if(!w){
        return;
    }
    yarp::manager::NodeType type = ((GenericViewWidget*)w)->getType();
    if(type == yarp::manager::APPLICATION){
        auto* ww = (ApplicationViewWidget*)w;
        ww->runApplicationSet(onlySelected);
    }
}

/*! \brief When Stop is clicked the applications running in the current Application Tab will go on stop state
 */
void MainWindow::onStop(bool onlySelected)
{
    QWidget *w = ui->mainTabs->currentWidget();
    if(!w){
        return;
    }
    yarp::manager::NodeType type = ((GenericViewWidget*)w)->getType();
    if(type == yarp::manager::APPLICATION){
        auto* ww = (ApplicationViewWidget*)w;
        ww->stopApplicationSet(onlySelected);
    }
}

/*! \brief When Stop is clicked the applications running in the current Application Tab will be killed
 */
void MainWindow::onKill(bool onlySelected)
{
    QWidget *w = ui->mainTabs->currentWidget();
    if(!w){
        return;
    }
    yarp::manager::NodeType type = ((GenericViewWidget*)w)->getType();
    if(type == yarp::manager::APPLICATION){
        auto* ww = (ApplicationViewWidget*)w;
        ww->killApplicationSet(onlySelected);
    }
}

/*! \brief When Connect is clicked the applications ports will be connected
 */
void MainWindow::onConnect(bool onlySelected)
{
    QWidget *w = ui->mainTabs->currentWidget();
    if(!w){
        return;
    }
    yarp::manager::NodeType type = ((GenericViewWidget*)w)->getType();
    if(type == yarp::manager::APPLICATION){
        auto* ww = (ApplicationViewWidget*)w;
        ww->connectConnectionSet(onlySelected);
    }
}
/*! \brief When Disconnect is clicked the applications ports will be disconnected
 */
void MainWindow::onDisconnect(bool onlySelected)
{
    QWidget *w = ui->mainTabs->currentWidget();
    if(!w){
        return;
    }
    yarp::manager::NodeType type = ((GenericViewWidget*)w)->getType();
    if(type == yarp::manager::APPLICATION){
        auto* ww = (ApplicationViewWidget*)w;
        ww->disconnectConnectionSet(onlySelected);
    }
}

void MainWindow::onRunSelected()
{
    onRun(true);
}
void MainWindow::onStopSelected()
{
    onStop(true);
}
void MainWindow::onKillSelected()
{
    onKill(true);
}
void MainWindow::onConnectSelected()
{
    onConnect(true);
}
void MainWindow::onDisconnectSelected()
{
    onDisconnect(true);
}
/*! \brief Refresh the applications state
 */
void MainWindow::onRefresh()
{
    QWidget *w = ui->mainTabs->currentWidget();
    if(!w){
        return;
    }
    yarp::manager::NodeType type = ((GenericViewWidget*)w)->getType();
    if(type == yarp::manager::APPLICATION){
        auto* ww = (ApplicationViewWidget*)w;
        ww->refresh();
    }

    if(type == yarp::manager::RESOURCE){
        auto* ww = (ResourceViewWidget*)w;
        ww->refresh();
    }
}

/*! \brief Select all items in Application tab
 */
void MainWindow::onSelectAll()
{
    QWidget *w = ui->mainTabs->currentWidget();
    if(!w){
        return;
    }
    yarp::manager::NodeType type = ((GenericViewWidget*)w)->getType();
    if(type == yarp::manager::APPLICATION){
        auto* ww = (ApplicationViewWidget*)w;
        ww->selectAll();
    }
}

/*! \brief Close the tab of index index
    \param index the index of the tab
 */
bool MainWindow::onTabClose(int index)
{
    auto* w = (GenericViewWidget*)ui->mainTabs->widget(index);
    if(!w){
        return false;
    }
    if(w->getType() == yarp::manager::APPLICATION){
        ApplicationViewWidget *aw = ((ApplicationViewWidget*)w);

        if(aw && aw->isRunning()){
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Icon::Warning);
            msgBox.setWindowTitle(QString("Closing %1").arg(ui->mainTabs->tabText(index)));
            msgBox.setText(tr("You have some running module. After closing the application window you might not be able to recover them. Are you sure?"));
            QPushButton* noButton = msgBox.addButton(tr("No"), QMessageBox::NoRole);
            QPushButton* pstopAndClose = msgBox.addButton(tr("Yes and Stop"), QMessageBox::YesRole);
            msgBox.addButton(tr("Yes"), QMessageBox::YesRole);
            msgBox.setDefaultButton(noButton);
            msgBox.exec();

            if (msgBox.clickedButton() == noButton)
            {
                return false;
            }
            else if(msgBox.clickedButton() == pstopAndClose)
            {
                onStop();
            }
        }

        QString fileOfCurrentApp = aw->getFileName();

        if(aw->isModified() && aw->isEditingMode()){
            QMessageBox::StandardButton btn = QMessageBox::question(this,"Save",QString("%1 has been modified\nDo you want to save it before closing?").arg(aw->getAppName().toLatin1().data()),
                                                                    QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
            if(btn == QMessageBox::Yes){
                bool ret = aw->save();
                if(ret){
                    onReopenApplication(aw->getAppName(),fileOfCurrentApp);
                    ui->mainTabs->removeTab(index);
                    delete w;
                    return true;

                }else{
                    QMessageBox::critical(this,"Error",QString("Error Saving the file"));
                    return false;
                }
            }
            if(btn == QMessageBox::Cancel){
                return false;
            }
            if (btn == QMessageBox::No){
                QFile file(fileOfCurrentApp);
                if(!file.exists()){
                    ui->entitiesTree->setCurrentItem(ui->entitiesTree->getWidgetItemByFilename(fileOfCurrentApp));
                    ui->entitiesTree->onRemove();
                    aw->closeManager();
                    ui->mainTabs->removeTab(index);
                    delete w;
                    return true;
                }
            }

        }
        aw->closeManager();
    }
    ui->mainTabs->removeTab(index);
    delete w;
    return true;
}

/*! \brief Logs an error message
    \param msg the message
 */
void MainWindow::onLogError(QString msg)
{
    QString text = QString ("[ERR] %1").arg(msg);
    ui->logWidget->addItem(text);
    ui->logWidget->item(ui->logWidget->count() - 1)->setBackground(QColor("#F9CCCA"));
    ui->logWidget->setCurrentRow(ui->logWidget->count() - 1);
}

/*! \brief Logs an warning message
    \param msg the message
 */
void MainWindow::onLogWarning(QString msg)
{
    QString text = QString ("[WAR] %1").arg(msg);
    ui->logWidget->addItem(text);
    ui->logWidget->item(ui->logWidget->count() - 1)->setBackground(QColor("#FFF6C8"));
    ui->logWidget->setCurrentRow(ui->logWidget->count() - 1);
}

/*! \brief Logs a message
    \param msg the message
 */
void MainWindow::onLogMessage(QString msg)
{
    QString text = QString ("[MSG] %1").arg(msg);
    ui->logWidget->addItem(text);
    ui->logWidget->setCurrentRow(ui->logWidget->count() - 1);
}


/*! \brief Called when a tab has been pressed
    \param index the index of the tab
 */
void MainWindow::onTabChangeItem(int index)
{

    auto* w = (GenericViewWidget*)ui->mainTabs->widget(index);
    if(w && w->getType() == yarp::manager::APPLICATION){
        auto* aw = (ApplicationViewWidget*)w;
        if(aw->isEditingMode()){
            ui->actionSelect_All->setEnabled(false);
            ui->actionRefresh_Status->setEnabled(false);
            ui->actionExport_Graph->setEnabled(false);
            ui->actionConnect_all->setEnabled(false);
            ui->actionDisconnect_all->setEnabled(false);
            ui->actionRun_all->setEnabled(false);
            ui->actionStop_all->setEnabled(false);
            ui->actionKill_all->setEnabled(false);
            ui->actionConnect->setEnabled(false);
            ui->actionDisconnect->setEnabled(false);
            ui->actionRun->setEnabled(false);
            ui->actionStop->setEnabled(false);
            ui->actionKill->setEnabled(false);
        }else{
            ui->actionSelect_All->setEnabled(true);
            ui->actionRefresh_Status->setEnabled(true);
            ui->actionExport_Graph->setEnabled(true);
            ui->actionConnect_all->setEnabled(true);
            ui->actionDisconnect_all->setEnabled(true);
            ui->actionRun_all->setEnabled(true);
            ui->actionStop_all->setEnabled(true);
            ui->actionKill_all->setEnabled(true);
            onApplicationSelectionChanged();
        }


        if(w->isModified()){
            ui->actionSave->setEnabled(true);
            ui->actionSave_As->setEnabled(true);
        }else{
            ui->actionSave->setEnabled(false);
            ui->actionSave_As->setEnabled(false);
        }
    }
    else {

        if(w && w->getType() == yarp::manager::RESOURCE){
            ui->actionRefresh_Status->setEnabled(true);
            prevWidget = w;
        }else{
            ui->actionRefresh_Status->setEnabled(false);
            prevWidget = nullptr;
        }

        ui->actionSelect_All->setEnabled(false);
        ui->actionExport_Graph->setEnabled(false);
        ui->actionConnect_all->setEnabled(false);
        ui->actionDisconnect_all->setEnabled(false);
        ui->actionRun_all->setEnabled(false);
        ui->actionStop_all->setEnabled(false);
        ui->actionKill_all->setEnabled(false);
        ui->actionConnect->setEnabled(false);
        ui->actionDisconnect->setEnabled(false);
        ui->actionRun->setEnabled(false);
        ui->actionStop->setEnabled(false);
        ui->actionKill->setEnabled(false);
    }
}

/*! \brief Create a new Application */
void MainWindow::onNewApplication()
{
    auto* newApplicationWizard = new NewApplicationWizard(&config);
    newApplicationWizard->setWindowTitle("Create New Application");
    if (newApplicationWizard->exec() == QDialog::Accepted){

        currentAppName = newApplicationWizard->name;
        currentAppDescription = newApplicationWizard->description;
        currentAppVersion = newApplicationWizard->version;
        fileName = newApplicationWizard->fileName;
        size_t len = newApplicationWizard->name.toLatin1().size();
        char* appName;
        appName = new char [len+1];
        strncpy(appName, newApplicationWizard->name.toLatin1().data(), len);
        appName[len] = '\0';
        if (newApplicationWizard->alreadyExists)
        {
            if (!lazyManager.removeApplication(newApplicationWizard->fileName.toLatin1().data(),
                                      appName))
            {
                reportErrors();
                if(appName)
                {
                    delete [] appName;
                    appName = nullptr;
                }
                return;
            }
        }
        initializeFile("Application");

        if (lazyManager.addApplication(newApplicationWizard->fileName.toLatin1().data(),
                                      &appName,true))
        {
            QString newApp(appName);
            syncApplicationList(newApp);

        }
        else
        {
            reportErrors();
        }

        if(appName)
        {
            delete [] appName;
            appName = nullptr;
        }
        delete newApplicationWizard;
        QFile f(fileName);
        f.remove();
        onModified(true);
        return;
    }
    delete newApplicationWizard;
    qDebug() << "Rejected";
}

/*! \brief Create a new Resource */
void MainWindow::onNewResource()
{
    yarp::manager::ErrorLogger* logger  = yarp::manager::ErrorLogger::Instance();

    fileName = QFileDialog::getSaveFileName(this,"Create new Resource description file",QApplication::applicationDirPath(),
                                 "Resource description files (*.xml)");

    if(!initializeFile("Resource"))
    {
        QString msg = QString("Error while initializing %1.").arg(fileName.toStdString().c_str());
        logger->addError(msg.toLatin1().data());
        reportErrors();
        return;
    }

    yarp::manager::LocalBroker launcher;
    if(launcher.init(ext_editor.c_str(), fileName.toLatin1().data(), nullptr, nullptr, nullptr, nullptr)){
        if(!launcher.start() && strlen(launcher.error())){
            QString msg = QString("Error while launching %1. %2").arg(ext_editor.c_str()).arg(launcher.error());
            logger->addError(msg.toLatin1().data());
            reportErrors();
        }
    }

}

/*! \brief Create a new Module */
void MainWindow::onNewModule()
{
    yarp::manager::ErrorLogger* logger  = yarp::manager::ErrorLogger::Instance();

    fileName = QFileDialog::getSaveFileName(this,"Create new Module description file",QApplication::applicationDirPath(),
                                 "Module description files (*.xml)");

    if(!initializeFile("Module"))
    {
        QString msg = QString("Error while initializing %1.").arg(fileName.toStdString().c_str());
        logger->addError(msg.toLatin1().data());
        reportErrors();
        return;
    }

    yarp::manager::LocalBroker launcher;
    if(launcher.init(ext_editor.c_str(), fileName.toLatin1().data(), nullptr, nullptr, nullptr, nullptr)){
        if(!launcher.start() && strlen(launcher.error())){
            QString msg = QString("Error while launching %1. %2").arg(ext_editor.c_str()).arg(launcher.error());
            logger->addError(msg.toLatin1().data());
            reportErrors();
        }
    }

}

/*! \brief Import Files (Modules, Applications, Resources) */
void MainWindow::onImportFiles()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Please choose a folder"),
                                                    QApplication::applicationDirPath(),
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    if(!dir.isEmpty()){
        if(config.find("load_subfolders").asString() == "yes"){
            if(loadRecursiveApplications(dir.toLatin1().data()))
                syncApplicationList();
        }
        else{
            if(lazyManager.addApplications(dir.toLatin1().data()))
                syncApplicationList();
        }

        if(lazyManager.addResources(dir.toLatin1().data())){
            syncApplicationList();
        }

        if(lazyManager.addModules(dir.toLatin1().data())){
            syncApplicationList();
        }

        reportErrors();
    }
}

/*! \brief Called when a tab is going to be closed */
void MainWindow::onClose()
{
    int index = ui->mainTabs->currentIndex();
    if(index < 0){
        return;
    }
    onTabClose(index);
}

void MainWindow::onModified(bool mod)
{
    ui->actionSave->setEnabled(mod);
    ui->actionSave_As->setEnabled(mod);
    int index = ui->mainTabs->currentIndex();
    auto* gw = (GenericViewWidget*)ui->mainTabs->currentWidget();
    if(gw->getType() == yarp::manager::APPLICATION){
        auto* w = (ApplicationViewWidget*)gw;
        if(mod){
            ui->mainTabs->setTabText(index,w->getAppName() + "*");
            gw->setModified(mod);
        }else{
            ui->mainTabs->setTabText(index,w->getAppName());
        }
    }
}

void MainWindow::onFileChanged(const QString &path)
{

    watcher->addPaths(listOfAppFiles);
    if (!ui->actionAuto_reload->isChecked())
    {
        return;
    }

    // get the app name from the file name
    QString appName = getAppNameFromXml(path);

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "File changed", "Xml file '" + path +
                                  "' changed.\nDo you want to reload the application? If open, the respective tab will be closed",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::No)
        return;

    Application* app = (Application*) lazyManager.getNode(appName.toStdString());
    if (app)
    {
        int index = getAppTabIndex(appName);
        if (index >= 0)
        {
            if (!onTabClose(index))
            {
                return;
            }
            else
            {
                // refresh it in the application list
                onReopenApplication(appName, path);
                // the reference has been changed reopening the application
                app = (Application*) lazyManager.getNode(appName.toStdString());
                // is it already open in the tab? if so close it and reopen it after the refresh
                if (app)
                {
                    viewApplication(app, false);
                }
                return;            }
        }
        else
        {
            // refresh it in the application list
            onReopenApplication(appName, path);
            return;
        }
    }
    return;
}

void MainWindow::onYarpClean()
{
    if(!yarp::os::Network::checkNetwork())
    {
        onLogWarning(QString::fromLatin1("yarpserver is not running"));
        return;
    }
    QInputDialog* inputDialog = new QInputDialog(this);
    inputDialog->setOptions(QInputDialog::NoButtons);

    bool ok=false;

    float timeout =  inputDialog->getDouble(nullptr ,"Running yarp clean",
                                          "Be aware that yarp clean with a little timetout could\n"
                                          "unregister ports that are actually open.\n\n"
                                           "Timeout(seconds):", 0.3, 0, 2147483647, 1, &ok);
    if (ok)
    {
        onLogMessage(QString("YARP clean: cleaning death ports..."));
        yarp::profiler::NetworkProfiler::yarpClean(timeout);
        onYarpNameList();
    }

}

void MainWindow::onYarpNameList()
{
    if(!yarp::os::Network::checkNetwork())
    {
        onLogWarning(QString::fromLatin1("yarpserver is not running"));
        return;
    }
    ui->entitiesTree->clearPorts();
    yarp::profiler::NetworkProfiler::ports_name_set ports;
    yarp::profiler::NetworkProfiler::yarpNameList(ports, true);
    for(auto& port : ports)
    {
        std::string portName = port.find("name").asString();
        std::string portIp   = port.find("ip").asString() + " port " +
                    std::to_string(port.find("port_number").asInt32());
        ui->entitiesTree->addPort(QStringList() << QString(portName.c_str())
                                  << QString(portIp.c_str()));
    }
    onLogMessage(QString::fromLatin1("Running yarp name list...found %1 ports").arg(ports.size()));
}

void MainWindow::onSave()
{
    auto* w = (GenericViewWidget *)ui->mainTabs->currentWidget();
    if(!w){
        return;
    }
    yarp::manager::NodeType type = ((GenericViewWidget*)w)->getType();
    if(type == yarp::manager::APPLICATION){
        auto* ww = (ApplicationViewWidget*)w;
        bool ret = ww->save();
        if(ret){
            onReopenApplication(ww->getAppName(),ww->getFileName());
        }else{
            QMessageBox::critical(this,"Error",QString("Error Saving the file"));
        }
    }
}

void MainWindow::onSaveAs()
{
    auto* newApplicationWizard = new NewApplicationWizard(&config,true);
    newApplicationWizard->setWindowTitle("Save Application as");
    if(newApplicationWizard->exec() == QDialog::Accepted){
        fileName = newApplicationWizard->fileName;
        delete newApplicationWizard;
    }
    else
    {
        delete newApplicationWizard;
        return;
    }
    if(fileName.trimmed().size() == 0 || fileName.contains(" ")){
        QMessageBox::critical(nullptr, QObject::tr("Error"), QObject::tr(string("Invalid file name " + fileName.toStdString()).c_str()));
        return;
    }

    auto* w = (GenericViewWidget *)ui->mainTabs->currentWidget();
    if(!w){
        return;
    }
    yarp::manager::NodeType type = ((GenericViewWidget*)w)->getType();
    if(type == yarp::manager::APPLICATION){
        auto* ww = (ApplicationViewWidget*)w;
        QString oldAppName = ww->getAppName();
        QString oldFileName = ww->getFileName();
        ww->setFileName(fileName);
        size_t it1 = fileName.toStdString().find(".xml");
        if(it1 == string::npos)
        {
            yError("yarpmanager: '.xml' not present in filename");
            return;
        }
        QString appName = fileName.toStdString().substr(0,it1).c_str();
        size_t it2 =appName.toStdString().find_last_of('/');
        if(it2 != string::npos)
        {
            currentAppName = appName.toStdString().substr(it2+1).c_str();
        }
        else
        {
            currentAppName = appName;
        }
        ww->setAppName(currentAppName);
        if(fileName.isEmpty()){
            return;
        }
        reportErrors();
        onSave();
        ww->setAppName(oldAppName);
        ww->setFileName(oldFileName);
        onTabClose(ui->mainTabs->currentIndex());
        syncApplicationList();
        Application* newApp = (Application*) lazyManager.getNode(currentAppName.toStdString());
        if (newApp)
        {
            viewApplication(newApp,true);
        }
    }
}

/*! \brief Open File (Modules, Applications, Resources) */
void MainWindow::onOpen()
{
    QString fileName = QFileDialog::getOpenFileName(this,"Please choose a File",QCoreApplication::applicationDirPath(),
                                 "Application description files (*.xml);;Modules description files (*.xml);;Resource description files (*.xml);;Any files (*.xml)");

    if(fileName.isEmpty()){
        return;
    }

    char* name = nullptr;

    if(lazyManager.addApplication(fileName.toLatin1().data(), &name, true)){
        QString appName(name);
        syncApplicationList(appName,true);
    }
    if(name)
    {
        delete [] name;
        name = nullptr;
    }

    if(lazyManager.addResource(fileName.toLatin1().data())){
        syncApplicationList();
    }
    if(lazyManager.addModule(fileName.toLatin1().data())){
        syncApplicationList();
    }
    reportErrors();

}
/*! \brief Opens the About Dialog */
void MainWindow::onAbout()
{
    QString copyright = "Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)";
    QString name = APP_NAME;
    QString version = YARP_VERSION;
    AboutDlg dlg(name,version,copyright,"https://www.iit.it/");
    dlg.exec();
}

/*! \brief Opens the Help in the default browser */

void MainWindow::onHelp()
{
    QDesktopServices::openUrl(QUrl("http://www.yarp.it/yarpmanager.html"));
}

void MainWindow::onRemoveApplication(QString xmlFile, QString appName)
{
    lazyManager.removeApplication(xmlFile.toLatin1().data(),appName.toLatin1().data());
    syncApplicationList();
}


void MainWindow::onReopenApplication(QString appName,QString fileName)
{
    lazyManager.removeApplication(fileName.toLatin1().data(), appName.toLatin1().data());
    char * appNamePtr = appName.toLatin1().data();
    lazyManager.addApplication(fileName.toLatin1().data(), &appNamePtr);
    syncApplicationList();
}

void MainWindow::onRemoveModule(QString modName)
{
    lazyManager.removeModule(modName.toLatin1().data());
    syncApplicationList();
}

void MainWindow::onReopenModule(QString modName,QString fileName)
{
    lazyManager.removeModule(modName.toLatin1().data());
    lazyManager.addModule(fileName.toLatin1().data());
    syncApplicationList();
}

void MainWindow::onRemoveResource(QString resName)
{
    lazyManager.removeResource(resName.toLatin1().data());
    syncApplicationList();
}

void MainWindow::onReopenResource(QString resName,QString fileName)
{
    lazyManager.removeResource(resName.toLatin1().data());
    lazyManager.addResource(fileName.toLatin1().data());
    syncApplicationList();
}

void MainWindow::onApplicationSelectionChanged()
{
    QWidget *w = ui->mainTabs->currentWidget();
    if(!w){
        return;
    }
    yarp::manager::NodeType type = ((GenericViewWidget*)w)->getType();
    if(type == yarp::manager::APPLICATION){
        auto* ww = (ApplicationViewWidget*)w;
        ui->actionRun->setEnabled(ww->anyModuleSelected());
        ui->actionStop->setEnabled(ww->anyModuleSelected());
        ui->actionKill->setEnabled(ww->anyModuleSelected());
        ui->actionConnect->setEnabled(ww->anyConnectionSelected());
        ui->actionDisconnect->setEnabled(ww->anyConnectionSelected());
    }
}
