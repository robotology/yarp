/*
 * Copyright (C) 2014 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Davide Perrone
 * Date: Feb 2014
 * email:   dperrone@aitek.it
 * website: www.aitek.it
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <yarp/manager/ymm-dir.h>
#include <yarp/manager/xmlapploader.h>
#include <yarp/manager/xmltemploader.h>
#include <yarp/manager/localbroker.h>

#include "moduleviewwidget.h"
#include "applicationviewwidget.h"
#include "resourceviewwidget.h"
#include "genericviewwidget.h"
#include "template_res.h"

#include <QGridLayout>
#include <QMessageBox>
#include <QFileDialog>
#include <QDesktopServices>

#if defined(WIN32)
    #pragma warning (disable : 4250)
    #pragma warning (disable : 4520)
#else
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/wait.h>
    #include <errno.h>
    #include <sys/types.h>
    #include <signal.h>
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


    connect(ui->entitiesTree,SIGNAL(viewResource(yarp::manager::Computer*)),this,SLOT(viewResource(yarp::manager::Computer*)));
    connect(ui->entitiesTree,SIGNAL(viewModule(yarp::manager::Module*)),this,SLOT(viewModule(yarp::manager::Module*)));
    connect(ui->entitiesTree,SIGNAL(viewApplication(yarp::manager::Application*)),this,SLOT(viewApplication(yarp::manager::Application*)));
    connect(ui->entitiesTree,SIGNAL(openFiles()),this,SLOT(onOpen()));
    connect(ui->entitiesTree,SIGNAL(importFiles()),this,SLOT(onImportFiles()));
    connect(ui->entitiesTree,SIGNAL(removeApplication(QString)),this,SLOT(onRemoveApplication(QString)));
    connect(ui->entitiesTree,SIGNAL(removeModule(QString)),this,SLOT(onRemoveModule(QString)));
    connect(ui->entitiesTree,SIGNAL(removeResource(QString)),this,SLOT(onRemoveResource(QString)));
    connect(ui->entitiesTree,SIGNAL(reopenApplication(QString,QString)),this,SLOT(onReopenApplication(QString,QString)));
    connect(ui->entitiesTree,SIGNAL(reopenModule(QString,QString)),this,SLOT(onReopenModule(QString,QString)));
    connect(ui->entitiesTree,SIGNAL(reopenResource(QString,QString)),this,SLOT(onReopenResource(QString,QString)));

    connect(ui->actionRun,SIGNAL(triggered()),this,SLOT(onRun()));
    connect(ui->actionStop,SIGNAL(triggered()),this,SLOT(onStop()));
    connect(ui->actionKill,SIGNAL(triggered()),this,SLOT(onKill()));
    connect(ui->actionConnect,SIGNAL(triggered()),this,SLOT(onConnect()));
    connect(ui->actionDisconnect,SIGNAL(triggered()),this,SLOT(onDisconnect()));
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
    connect(ui->actionHelp,SIGNAL(triggered()),this,SLOT(onHelp()));



    onTabChangeItem(-1);

}

MainWindow::~MainWindow()
{
    delete ui;
}

/*! \brief Init the application with the current configuration.
 *
 *  \param config the configuration
 */
void MainWindow::init(yarp::os::Property config)
{
    this->config = config;

    string basepath=config.check("ymanagerini_dir", yarp::os::Value("")).asString().c_str();

    if(config.check("modpath")){
        string strPath;
        string modPaths(config.find("modpath").asString().c_str());
        while (modPaths!=""){
            string::size_type pos=modPaths.find(";");
            strPath=modPaths.substr(0, pos);
            yarp::manager::trimString(strPath);
            if (!isAbsolute(strPath.c_str()))
                strPath=basepath+strPath;
            if((strPath.rfind(PATH_SEPERATOR)==string::npos) ||
            (strPath.rfind(PATH_SEPERATOR)!=strPath.size()-1))
                strPath = strPath + string(PATH_SEPERATOR);
            lazyManager.addModules(strPath.c_str());
            if (pos==string::npos || pos==0)
                break;
            modPaths=modPaths.substr(pos+1);
        }
    }

    if(config.check("respath")){
        string strPath;
        string resPaths(config.find("respath").asString().c_str());
        while (resPaths!=""){
            string::size_type pos=resPaths.find(";");
            strPath=resPaths.substr(0, pos);
            yarp::manager::trimString(strPath);
            if (!isAbsolute(strPath.c_str()))
                strPath=basepath+strPath;

            if((strPath.rfind(PATH_SEPERATOR)==string::npos) ||
            (strPath.rfind(PATH_SEPERATOR)!=strPath.size()-1))
                strPath = strPath + string(PATH_SEPERATOR);

            lazyManager.addResources(strPath.c_str());
            if (pos==string::npos)
                break;
            resPaths=resPaths.substr(pos+1);
        }
    }

    yarp::manager::ErrorLogger* logger  = yarp::manager::ErrorLogger::Instance();


    if(config.check("apppath")){
        string strPath;
        string appPaths(config.find("apppath").asString().c_str());
        while (appPaths!=""){
            string::size_type pos=appPaths.find(";");
            strPath=appPaths.substr(0, pos);
            yarp::manager::trimString(strPath);
            if (!isAbsolute(strPath.c_str())){
                strPath=basepath+strPath;
            }

            if((strPath.rfind(PATH_SEPERATOR)==string::npos) ||
                (strPath.rfind(PATH_SEPERATOR)!=strPath.size()-1)){
                    strPath = strPath + string(PATH_SEPERATOR);
            }

            if(config.find("load_subfolders").asString() == "yes"){
                if(!loadRecursiveApplications(strPath.c_str())){
                    logger->addError("Cannot load the applications from  " + strPath);
                }
                loadRecursiveTemplates(strPath.c_str());
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
        string templPaths(config.find("templpath").asString().c_str());
        while (templPaths!=""){
            string::size_type pos=templPaths.find(";");
            strPath=templPaths.substr(0, pos);
            yarp::manager::trimString(strPath);
            if (!isAbsolute(strPath.c_str())){
                strPath=basepath+strPath;
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
#if defined(WIN32)
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
void MainWindow::syncApplicationList()
{
    ui->entitiesTree->clearApplication();
    ui->entitiesTree->clearModules();
    ui->entitiesTree->clearResources();
    //ui->entitiesTree->clearTemplates();

    yarp::manager::KnowledgeBase* kb = lazyManager.getKnowledgeBase();
    yarp::manager::ApplicaitonPContainer apps =  kb->getApplications();
    unsigned int cnt = 0;
    for(yarp::manager::ApplicationPIterator itr=apps.begin(); itr!=apps.end(); itr++){
        cnt++;
        yarp::manager::Application *app = dynamic_cast<yarp::manager::Application*>(*itr);
        if(app){
            ui->entitiesTree->addApplication(app);
        }
    }

    yarp::manager::ResourcePContainer resources = kb->getResources();
    for(yarp::manager::ResourcePIterator itr=resources.begin(); itr!=resources.end(); itr++){
        yarp::manager::Computer* comp = dynamic_cast<yarp::manager::Computer*>(*itr);
        if(comp){
            ui->entitiesTree->addComputer(comp);
        }
    }

    yarp::manager::ModulePContainer modules = kb->getModules();
    for(yarp::manager::ModulePIterator itr=modules.begin(); itr!=modules.end(); itr++){
        yarp::manager::Module *mod = dynamic_cast<yarp::manager::Module*>(*itr);
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
    string strPath = szPath;
    if((strPath.rfind(PATH_SEPERATOR)==string::npos) ||
            (strPath.rfind(PATH_SEPERATOR)!=strPath.size()-1))
            strPath = strPath + string(PATH_SEPERATOR);

    DIR *dir;
    struct dirent *entry;
    if ((dir = opendir(strPath.c_str())) == NULL)
        return false;

    // loading from current folder
    yarp::manager::AppTemplate* tmp;
    yarp::manager::XmlTempLoader tempload(strPath.c_str(), NULL);
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
    string strPath = szPath;
    if((strPath.rfind(PATH_SEPERATOR)==string::npos) ||
            (strPath.rfind(PATH_SEPERATOR)!=strPath.size()-1))
            strPath = strPath + string(PATH_SEPERATOR);

    DIR *dir;
    struct dirent *entry;
    if ((dir = opendir(strPath.c_str())) == NULL)
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

    ResourceViewWidget *w = new ResourceViewWidget(res,ui->mainTabs);
    int index = ui->mainTabs->addTab(w,res->getName());
    ui->mainTabs->setTabIcon(index,QIcon(":/images/computer_ico.png"));
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

    ModuleViewWidget *w = new ModuleViewWidget(module,ui->mainTabs);
    int index = ui->mainTabs->addTab(w,module->getName());
    ui->mainTabs->setTabIcon(index,QIcon(":/images/module_ico.png"));
    ui->mainTabs->setCurrentIndex(index);
}

/*! \brief Load the Application on the MainWindow
    \param app the Application
 */
void MainWindow::viewApplication(yarp::manager::Application *app)
{
    for(int i=0;i<ui->mainTabs->count();i++){
        if(ui->mainTabs->tabText(i) == app->getName()){
            ui->mainTabs->setCurrentIndex(i);
            return;
        }
    }

    ApplicationViewWidget *w = new ApplicationViewWidget(app,&lazyManager,&config,ui->mainTabs);
    connect(w,SIGNAL(logError(QString)),this,SLOT(onLogError(QString)));
    connect(w,SIGNAL(logWarning(QString)),this,SLOT(onLogWarning(QString)));
    int index = ui->mainTabs->addTab(w,app->getName());
    ui->mainTabs->setTabIcon(index,QIcon(":/images/application_ico.png"));
    ui->mainTabs->setCurrentIndex(index);
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
        ApplicationViewWidget *ww = (ApplicationViewWidget*)w;
        ww->exportGraph();
    }
}

/*! \brief When Run is clicked the applications in the current Application Tab will go on run state
 */
void MainWindow::onRun()
{
    QWidget *w = ui->mainTabs->currentWidget();
    if(!w){
        return;
    }
    yarp::manager::NodeType type = ((GenericViewWidget*)w)->getType();
    if(type == yarp::manager::APPLICATION){
        ApplicationViewWidget *ww = (ApplicationViewWidget*)w;
        ww->runApplicationSet();
    }
}

/*! \brief When Stop is clicked the applications running in the current Application Tab will go on stop state
 */
void MainWindow::onStop()
{
    QWidget *w = ui->mainTabs->currentWidget();
    if(!w){
        return;
    }
    yarp::manager::NodeType type = ((GenericViewWidget*)w)->getType();
    if(type == yarp::manager::APPLICATION){
        ApplicationViewWidget *ww = (ApplicationViewWidget*)w;
        ww->stopApplicationSet();
    }
}

/*! \brief When Stop is clicked the applications running in the current Application Tab will be killed
 */
void MainWindow::onKill()
{
    QWidget *w = ui->mainTabs->currentWidget();
    if(!w){
        return;
    }
    yarp::manager::NodeType type = ((GenericViewWidget*)w)->getType();
    if(type == yarp::manager::APPLICATION){
        ApplicationViewWidget *ww = (ApplicationViewWidget*)w;
        ww->killApplicationSet();
    }
}

/*! \brief When Connect is clicked the applications ports will be connected
 */
void MainWindow::onConnect()
{
    QWidget *w = ui->mainTabs->currentWidget();
    if(!w){
        return;
    }
    yarp::manager::NodeType type = ((GenericViewWidget*)w)->getType();
    if(type == yarp::manager::APPLICATION){
        ApplicationViewWidget *ww = (ApplicationViewWidget*)w;
        ww->connectConnectionSet();
    }
}
/*! \brief When Disconnect is clicked the applications ports will be disconnected
 */
void MainWindow::onDisconnect()
{
    QWidget *w = ui->mainTabs->currentWidget();
    if(!w){
        return;
    }
    yarp::manager::NodeType type = ((GenericViewWidget*)w)->getType();
    if(type == yarp::manager::APPLICATION){
        ApplicationViewWidget *ww = (ApplicationViewWidget*)w;
        ww->disconnectConnectionSet();
    }
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
        ApplicationViewWidget *ww = (ApplicationViewWidget*)w;
        ww->refresh();
    }

    if(type == yarp::manager::RESOURCE){
        ResourceViewWidget *ww = (ResourceViewWidget*)w;
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
        ApplicationViewWidget *ww = (ApplicationViewWidget*)w;
        ww->selectAll();
    }
}

/*! \brief Close the tab of index index
    \param index the index of the tab
 */
void MainWindow::onTabClose(int index)
{
    GenericViewWidget *w = (GenericViewWidget*)ui->mainTabs->widget(index);
    if(!w){
        return;
    }
    if(w->getType() == yarp::manager::APPLICATION){
        ApplicationViewWidget *aw = ((ApplicationViewWidget*)w);
        if(aw->isRunning()){
            if( QMessageBox::warning(this,QString("Closing %1").arg(ui->mainTabs->tabText(index)),"You have some running module. After closing the application window you might not be able to recover them. Are you sure?",QMessageBox::Yes,QMessageBox::No) == QMessageBox::No){
                return;
            }
        }
    }
    ui->mainTabs->removeTab(index);
    delete w;
}

/*! \brief Logs an error message
    \param msg the message
 */
void MainWindow::onLogError(QString msg)
{
    QString text = QString ("[ERR] %1").arg(msg);
    ui->logWidget->addItem(text);
    ui->logWidget->item(ui->logWidget->count() - 1)->setBackgroundColor(QColor("#F9CCCA"));
    ui->logWidget->setCurrentRow(ui->logWidget->count() - 1);
}

/*! \brief Logs an warning message
    \param msg the message
 */
void MainWindow::onLogWarning(QString msg)
{
    QString text = QString ("[WAR] %1").arg(msg);
    ui->logWidget->addItem(text);
    ui->logWidget->item(ui->logWidget->count() - 1)->setBackgroundColor(QColor("#FFF6C8"));
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

    GenericViewWidget *w = (GenericViewWidget*)ui->mainTabs->widget(index);
    if(w && w->getType() == yarp::manager::APPLICATION){
        ui->actionSelect_All->setEnabled(true);
        ui->actionRefresh_Status->setEnabled(true);
        ui->actionExport_Graph->setEnabled(true);
        ui->actionConnect->setEnabled(true);
        ui->actionDisconnect->setEnabled(true);
        ui->actionRun->setEnabled(true);
        ui->actionStop->setEnabled(true);
        ui->actionKill->setEnabled(true);
    }else{
        if(w && w->getType() == yarp::manager::RESOURCE){
            ui->actionRefresh_Status->setEnabled(true);
        }else{
            ui->actionRefresh_Status->setEnabled(false);
        }
        ui->actionSelect_All->setEnabled(false);
        ui->actionExport_Graph->setEnabled(false);
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
    yarp::manager::ErrorLogger* logger  = yarp::manager::ErrorLogger::Instance();

    QString fileName = QFileDialog::getSaveFileName(this,"Create new Application description file",QApplication::applicationDirPath(),
                                 "Application description files (*.xml)");

    QFile f(fileName);
    bool b = f.open(QIODevice::ReadWrite);
    if(b){
        f.write(str_app_template.c_str());
        f.flush();
        f.close();
    }else{
        QString err = QString("Cannot create %1").arg(fileName);
        logger->addError(err.toLatin1().data());
        reportErrors();
        return;
    }

    yarp::manager::LocalBroker launcher;
    if(launcher.init(ext_editor.c_str(), fileName.toLatin1().data(), NULL, NULL, NULL, NULL)){
        if(!launcher.start() && strlen(launcher.error())){
            QString msg = QString("Error while launching %1. %2").arg(ext_editor.c_str()).arg(launcher.error());
            logger->addError(msg.toLatin1().data());
            reportErrors();
        }
    }
}

/*! \brief Create a new Resource */
void MainWindow::onNewResource()
{
    yarp::manager::ErrorLogger* logger  = yarp::manager::ErrorLogger::Instance();

    QString fileName = QFileDialog::getSaveFileName(this,"Create new Resource description file",QApplication::applicationDirPath(),
                                 "Resource description files (*.xml)");

    QFile f(fileName);
    bool b = f.open(QIODevice::ReadWrite);
    if(b){
        f.write(str_res_template.c_str());
        f.flush();
        f.close();
    }else{
        QString err = QString("Cannot create %1").arg(fileName);
        logger->addError(err.toLatin1().data());
        reportErrors();
        return;
    }

    yarp::manager::LocalBroker launcher;
    if(launcher.init(ext_editor.c_str(), fileName.toLatin1().data(), NULL, NULL, NULL, NULL)){
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

    QString fileName = QFileDialog::getSaveFileName(this,"Create new Module description file",QApplication::applicationDirPath(),
                                 "Module description files (*.xml)");

    QFile f(fileName);
    bool b = f.open(QIODevice::ReadWrite);
    if(b){
        f.write(str_mod_template.c_str());
        f.flush();
        f.close();
    }else{
        QString err = QString("Cannot create %1").arg(fileName);
        logger->addError(err.toLatin1().data());
        reportErrors();
        return;
    }

    yarp::manager::LocalBroker launcher;
    if(launcher.init(ext_editor.c_str(), fileName.toLatin1().data(), NULL, NULL, NULL, NULL)){
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

/*! \brief Open File (Modules, Applications, Resources) */
void MainWindow::onOpen()
{
    QString fileName = QFileDialog::getOpenFileName(this,"Please choose a File",QCoreApplication::applicationDirPath(),
                                 "Application description files (*.xml);;Modules description files (*.xml);;Resource description files (*.xml);;Any files (*.xml)");

    if(fileName.isEmpty()){
        return;
    }

    if(lazyManager.addApplication(fileName.toLatin1().data())){
        syncApplicationList();
    }

    if(lazyManager.addResource(fileName.toLatin1().data())){
        syncApplicationList();
    }
    if(lazyManager.addModule(fileName.toLatin1().data())){
        syncApplicationList();
    }
    reportErrors();

}

/*! \brief Opens the Help in the default browser */

void MainWindow::onHelp()
{
    QDesktopServices::openUrl(QUrl("http://wiki.icub.org/yarpdoc/yarpmanager.html"));
}

void MainWindow::onRemoveApplication(QString appName)
{
    lazyManager.removeApplication(appName.toLatin1().data());
    syncApplicationList();
}


void MainWindow::onReopenApplication(QString appName,QString fileName)
{
    lazyManager.removeApplication(appName.toLatin1().data());
    lazyManager.addApplication(fileName.toLatin1().data(),appName.toLatin1().data());
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
