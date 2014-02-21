/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


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


#include <gtkmm.h>

#include <iostream>
#include <fstream>
#include <string>
#include <yarp/manager/ymm-dir.h>

#include <yarp/manager/localbroker.h>
#include <yarp/manager/xmltemploader.h>
#include <yarp/manager/xmlapploader.h>

#include "main_window.h"
#include "application_window.h"
#include "resource_window.h"
#include "module_window.h"
#include "icon_res.h"
#include "template_res.h"

using namespace std;
using namespace yarp::manager;

#define WND_DEF_HEIGHT      600
#define WND_DEF_WIDTH       800

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

MainWindow::MainWindow( yarp::os::Property &config)
{
    m_config = config;

    //fullscreen();
    set_title("Yarp module manager");
    //set_border_width(3);
    set_default_size(WND_DEF_WIDTH, WND_DEF_HEIGHT);

    m_refMessageList = new MessagesList(this);
    m_refMessageList->enableTimeStamp();

    setupStocks();
    setupActions();
    setupSignals();
    
    createWidgets();
    
    std::string basepath=config.check("ymanagerini_dir", yarp::os::Value("")).asString().c_str();

    if(config.check("modpath"))
    {
        string strPath;
        string modPaths(config.find("modpath").asString().c_str());
        while (modPaths!="")
        {
            string::size_type pos=modPaths.find(";");
            strPath=modPaths.substr(0, pos);
            trimString(strPath);
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

    if(config.check("respath"))
    {
        string strPath;
        string resPaths(config.find("respath").asString().c_str());
        while (resPaths!="")
        {
            string::size_type pos=resPaths.find(";");
            strPath=resPaths.substr(0, pos);
            trimString(strPath);
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

    ErrorLogger* logger  = ErrorLogger::Instance(); 

    if(config.check("apppath"))
    {
               string strPath;
        string appPaths(config.find("apppath").asString().c_str());
        while (appPaths!="")
        {
            string::size_type pos=appPaths.find(";");
            strPath=appPaths.substr(0, pos);
            trimString(strPath);
            if (!isAbsolute(strPath.c_str()))
                strPath=basepath+strPath;

            if((strPath.rfind(PATH_SEPERATOR)==string::npos) ||
                (strPath.rfind(PATH_SEPERATOR)!=strPath.size()-1))
                    strPath = strPath + string(PATH_SEPERATOR);

            if(config.find("load_subfolders").asString() == "yes")
            {
                if(!loadRecursiveApplications(strPath.c_str()))
                    logger->addError("Cannot load the applications from  " + strPath);
                loadRecursiveTemplates(strPath.c_str());
            }
            else
                lazyManager.addApplications(strPath.c_str());

            if (pos==string::npos)
                break;
            appPaths=appPaths.substr(pos+1);
        }
    }

    if (config.check("templpath"))
    {
        string strPath;
        string templPaths(config.find("templpath").asString().c_str());
        while (templPaths!="")
        {
            string::size_type pos=templPaths.find(";");
            strPath=templPaths.substr(0, pos);
            trimString(strPath);
            if (!isAbsolute(strPath.c_str()))
                strPath=basepath+strPath;

            if(!loadRecursiveTemplates(strPath.c_str()))
                    logger->addError("Cannot load the templates from  " + strPath);

            if (pos==string::npos)
                break;
            templPaths=templPaths.substr(pos+1);
        }
    }

    reportErrors();

    syncApplicationList();
    show_all_children();

    if(config.check("application"))
    {
        XmlAppLoader appload(config.find("application").asString().c_str());
        if(!appload.init())
            return;
        Application* application = appload.getNextApplication();
        if(!application)
            return;
        // add this application to the manager if does not exist    
        if(!lazyManager.getKnowledgeBase()->getApplication(application->getName()))
        {
            lazyManager.getKnowledgeBase()->addApplication(application);
            syncApplicationList();
        }
        manageApplication(application->getName());
    }    
}



MainWindow::~MainWindow()
{
    delete m_refMessageList;
#if (GLIBMM_MAJOR_VERSION == 2 && GLIBMM_MINOR_VERSION >= 16)
    m_factory.reset();
#endif
}

void MainWindow::on_size_allocate(Gtk::Allocation& allocation)
{
    if(allocation.get_height() != dummy_h)
    {
        dummy_h = allocation.get_height();
        m_VPaned.set_position((int)(allocation.get_height()-allocation.get_height()/3.0));
    }

    if(m_VPaned.get_position()<50)
        m_VPaned.set_position(50);

    if(m_VPaned.get_position()>(allocation.get_height()-120))
        m_VPaned.set_position(allocation.get_height()-120);

    Gtk::Window::on_size_allocate(allocation);
}

void MainWindow::createWidgets(void)
{

     set_icon(Gdk::Pixbuf::create_from_data(ymanager_ico.pixel_data, 
                        Gdk::COLORSPACE_RGB,
                        true,
                        8,
                        ymanager_ico.width,
                        ymanager_ico.height,
                        ymanager_ico.bytes_per_pixel*ymanager_ico.width));

    add(m_VBox);

    m_refUIManager = Gtk::UIManager::create();
    m_refUIManager->insert_action_group(m_refActionGroup);
    add_accel_group(m_refUIManager->get_accel_group());

    Glib::ustring ui_info =
        "<ui>"
        " <menubar name='MenuBar'>"
        "    <menu action='FileMenu'>"
        "      <menu action='FileNew'>"
        "        <menuitem action='FileNewApp'/>"
        "        <menuitem action='FileNewMod'/>"
        "        <menuitem action='FileNewRes'/>"      
        "      </menu>"
        "      <menuitem action='FileOpen'/>"
        "      <menuitem action='FileClose'/>"
        "      <separator/>"
        "      <menuitem action='FileSave'/>"
        "      <menuitem action='FileSaveAs'/>"
        "      <separator/>"
        "      <menuitem action='FileImport'/>"
        "      <separator/>"
        "      <menuitem action='FileQuit'/>"
        "    </menu>"
        "    <menu action='EditMenu'>"
        "      <menuitem action='EditSelAll'/>"
        "      <menuitem action='EditExportGraph'/>"
        "    </menu>"
        "    <menu action='ManageMenu'>"
        "      <menuitem action='ManageRun'/>"
        "      <menuitem action='ManageStop'/>"
        "      <menuitem action='ManageKill'/>"
        "      <separator/>"
        "      <menuitem action='ManageConnect'/>"
        "      <menuitem action='ManageDisconnect'/>"
        "      <separator/>"
        "      <menuitem action='ManageRefresh'/>"
        "    </menu>"
        "    <menu action='HelpMenu'>"
        "      <menuitem action='HelpOnline'/>"
        "      <menuitem action='HelpAbout'/>"
        "    </menu>"
        " </menubar>"
        " <toolbar name='ToolBar'>"
        "    <toolitem action='FileOpen'/>"
        "    <toolitem action='FileImport'/>"
        "    <separator/>"
        "    <toolitem action='EditSelAll'/>"
        "    <toolitem action='ManageRefresh'/>"
        "    <separator/>"
        "    <toolitem action='ManageRun'/>"
        "    <toolitem action='ManageConnect'/>"
        "    <toolitem action='ManageDisconnect'/>"
        "    <toolitem action='ManageStop'/>"
        "    <toolitem action='ManageKill'/>"
        "    <separator/>"
        "    <toolitem action='HelpOnline'/>"
        " </toolbar>"
        " <popup name='PopupManageModules'>"
        "      <menuitem action='ManageRun'/>"
        "      <menuitem action='ManageStop'/>"
        "      <menuitem action='ManageKill'/>"
        "      <separator/>"
        "      <menuitem action='ManageRefresh'/>"
        " </popup>"
        " <popup name='PopupGeneral'>"
        "      <menuitem action='FileOpen'/>"
        "      <menuitem action='FileImport'/>"
        "      <menuitem action='PAppRemove'/>"
        "      <menuitem action='PAppReopen'/>"
        " </popup>"
        " <popup name='PopupFolders'>"
        "      <menuitem action='FileOpen'/>"
        "      <menuitem action='FileImport'/>"
        " </popup>"
        " <popup name='PopupFile'>"
        "      <menuitem action='PEditFile'/>"
        " </popup>"
        " <popup name='PopupApplication'>"
        "      <menuitem action='PAppLoad'/>"   
        "      <separator/>"
        "      <menuitem action='PAppReopen'/>"
        "      <menuitem action='PAppRemove'/>"
        " </popup>"
        " <popup name='PopupModule'>"
        "      <menuitem action='PModLoad'/>"   
        "      <separator/>"
        "      <menuitem action='PAppReopen'/>"
        "      <menuitem action='PAppRemove'/>"
        " </popup>"
        " <popup name='PopupResource'>"
        "      <menuitem action='PResLoad'/>"   
        "      <separator/>"
        "      <menuitem action='PAppReopen'/>"
        "      <menuitem action='PAppRemove'/>"
        " </popup>"
        "</ui>";


#ifdef GLIBMM_EXCEPTIONS_ENABLED
    try
    {
        m_refUIManager->add_ui_from_string(ui_info);
    }
    catch(const Glib::Error& ex)
    {
        std::cerr << "building menus failed: " << ex.what();
    }
#else
    std::auto_ptr<Glib::Error> ex;
    m_refUIManager->add_ui_from_string(ui_info, ex);
    if(ex.get())
    {
        std::cerr << "building menus failed: " << ex->what();
    }
#endif //GLIBMM_EXCEPTIONS_ENABLED

    Gtk::Widget* pMenubar = m_refUIManager->get_widget("/MenuBar");
    if(pMenubar)
        m_VBox.pack_start(*pMenubar, Gtk::PACK_SHRINK);
    Gtk::Widget* pToolbar = m_refUIManager->get_widget("/ToolBar") ;
    if(pToolbar)
    {
#if (GTKMM_MAJOR_VERSION == 2 && GTKMM_MINOR_VERSION >= 12)
        ((Gtk::Toolbar*)pToolbar)->set_icon_size(Gtk::IconSize(Gtk::ICON_SIZE_SMALL_TOOLBAR));
#endif
        m_VBox.pack_start(*pToolbar, Gtk::PACK_SHRINK);
    }


    m_refCommandBuffer = Gtk::TextBuffer::create();
    m_refCommandBuffer->set_text("<<Yellow notes>>\n");
    m_commandView.set_buffer(m_refCommandBuffer);

    m_bottomTab.set_tab_pos(Gtk::POS_BOTTOM);
    m_bottomTab.set_border_width(0);
    m_bottomTab.append_page(*m_refMessageList, "Messages");
    m_bottomTab.append_page(m_commandView, "Notes");

    //m_Notebook.signal_switch_page().connect(sigc::mem_fun(*this,
    //        &ExampleWindow::on_notebook_switch_page) );


    m_mainTab.set_border_width(0);
    m_mainTab.set_show_border(false);
    m_mainTab.set_scrollable(true);
    m_applicationList.set_border_width(3);  
    m_HPaned.add1(m_applicationList);
    m_HPaned.add2(m_mainTab);
    m_HPaned.set_position(WND_DEF_WIDTH/3);
    
    m_VPaned.add1(m_HPaned);
    m_VPaned.add2(m_bottomTab);
    m_VPaned.set_position(WND_DEF_HEIGHT - WND_DEF_HEIGHT/3);
    m_VPaned.set_size_request(-1, 300);
    m_VBox.pack_start(m_VPaned);
    m_VBox.pack_start(m_Statusbar, Gtk::PACK_SHRINK);
    

}

void MainWindow::setupSignals(void)
{
    //Connect signal:
    
    m_applicationList.getTreeView()->signal_row_activated().connect(sigc::mem_fun(*this,
                        &MainWindow::onAppListRowActivated) );
    
    m_applicationList.getTreeView()->signal_button_press_event().connect_notify(sigc::mem_fun(*this,
            &MainWindow::onAppListButtonPressed) );

    m_mainTab.signal_switch_page().connect(sigc::mem_fun(*this,
            &MainWindow::onNotebookSwitchPage) );

    signal_delete_event().connect(sigc::mem_fun(*this, 
            &MainWindow::onDeleteEvent));

//  signal_expose_event().connect(sigc::mem_fun(*this,
//            &MainWindow::onExposeEvent) );    
}

void MainWindow::setupStocks(void)
{
    m_factory = Gtk::IconFactory::create();
    m_factory->add_default();

    Gtk::StockID killID = Gtk::StockID("YKILL");
    Gtk::StockID stopID = Gtk::StockID("YSTOP");
    Gtk::StockID runID = Gtk::StockID("YRUN");
    Gtk::StockID procID = Gtk::StockID("YPROCESSOR");
    Gtk::StockID importID = Gtk::StockID("YIMPORT");
    Gtk::StockID selallID = Gtk::StockID("YSELECTALL");
    Gtk::StockID connectID = Gtk::StockID("YCONNECT");
    Gtk::StockID disconnectID = Gtk::StockID("YDISCONNECT");
    Gtk::StockID refreshID = Gtk::StockID("YREFRESH");

    Gtk::StockItem killStock(killID, "YKILL");
    Gtk::StockItem stopStock(killID, "YSTOP");
    Gtk::StockItem runStock(runID, "YRUN");
    Gtk::StockItem procStock(procID, "YPROCESSOR");
    Gtk::StockItem importStock(importID, "YIMPORT");
    Gtk::StockItem selallStock(selallID, "YSELECTALL");
    Gtk::StockItem connectStock(connectID, "YCONNECT");
    Gtk::StockItem disconnectStock(disconnectID, "YDISCONNECT");
    Gtk::StockItem refreshStock(refreshID, "YREFRESH");

    Gtk::Stock::add(killStock);
    Gtk::Stock::add(stopStock);
    Gtk::Stock::add(runStock);
    Gtk::Stock::add(procStock);
    Gtk::Stock::add(importStock);
    Gtk::Stock::add(selallStock);
    Gtk::Stock::add(connectStock);
    Gtk::Stock::add(disconnectStock);
    Gtk::Stock::add(refreshStock);

    Gtk::IconSet killIcon(Gdk::Pixbuf::create_from_data(kill_ico.pixel_data, 
                        Gdk::COLORSPACE_RGB,
                        true,
                        8,
                        kill_ico.width,
                        kill_ico.height,
                        kill_ico.bytes_per_pixel*kill_ico.width));

    Gtk::IconSet stopIcon(Gdk::Pixbuf::create_from_data(stop_ico.pixel_data, 
                        Gdk::COLORSPACE_RGB,
                        true,
                        8,
                        stop_ico.width,
                        stop_ico.height,
                        stop_ico.bytes_per_pixel*stop_ico.width));

    Gtk::IconSet runIcon(Gdk::Pixbuf::create_from_data(run_ico.pixel_data, 
                        Gdk::COLORSPACE_RGB,
                        true,
                        8,
                        run_ico.width,
                        run_ico.height,
                        run_ico.bytes_per_pixel*run_ico.width));

    Gtk::IconSet procIcon(Gdk::Pixbuf::create_from_data(yesres_ico.pixel_data, 
                        Gdk::COLORSPACE_RGB,
                        true,
                        8,
                        yesres_ico.width,
                        yesres_ico.height,
                        yesres_ico.bytes_per_pixel*yesres_ico.width));

    Gtk::IconSet importIcon(Gdk::Pixbuf::create_from_data(import_ico.pixel_data, 
                        Gdk::COLORSPACE_RGB,
                        true,
                        8,
                        import_ico.width,
                        import_ico.height,
                        import_ico.bytes_per_pixel*import_ico.width));

    Gtk::IconSet selallIcon(Gdk::Pixbuf::create_from_data(selectall_ico.pixel_data, 
                        Gdk::COLORSPACE_RGB,
                        true,
                        8,
                        selectall_ico.width,
                        selectall_ico.height,
                        selectall_ico.bytes_per_pixel*selectall_ico.width));

    Gtk::IconSet connectIcon(Gdk::Pixbuf::create_from_data(connected_ico.pixel_data, 
                        Gdk::COLORSPACE_RGB,
                        true,
                        8,
                        connected_ico.width,
                        connected_ico.height,
                        connected_ico.bytes_per_pixel*connected_ico.width));

    Gtk::IconSet disconnectIcon(Gdk::Pixbuf::create_from_data(disconnected_ico.pixel_data, 
                        Gdk::COLORSPACE_RGB,
                        true,
                        8,
                        disconnected_ico.width,
                        disconnected_ico.height,
                        disconnected_ico.bytes_per_pixel*disconnected_ico.width));

    Gtk::IconSet refreshIcon(Gdk::Pixbuf::create_from_data(progress_ico.pixel_data, 
                        Gdk::COLORSPACE_RGB,
                        true,
                        8,
                        progress_ico.width,
                        progress_ico.height,
                        progress_ico.bytes_per_pixel*progress_ico.width));


    m_factory->add(killID, killIcon);
    m_factory->add(stopID, stopIcon);
    m_factory->add(runID, runIcon);
    m_factory->add(procID, procIcon);
    m_factory->add(importID, importIcon);
    m_factory->add(selallID, selallIcon);
    m_factory->add(connectID, connectIcon);
    m_factory->add(disconnectID, disconnectIcon);
    m_factory->add(refreshID, refreshIcon);

}



void MainWindow::setupActions(void)
{

    //Create actions for menus and toolbars:
    m_refActionGroup = Gtk::ActionGroup::create();

    //File|New sub menu:
    m_refActionGroup->add(Gtk::Action::create("FileMenu", "File"));
    m_refActionGroup->add(Gtk::Action::create("FileNew", Gtk::Stock::NEW));
    m_refActionGroup->add(Gtk::Action::create("FileNewApp",
                Gtk::Stock::NEW, "New _Application", "Create a new Application"),
                sigc::mem_fun(*this, &MainWindow::onMenuFileNewApp));
    m_refActionGroup->add(Gtk::Action::create("FileNewMod",
                Gtk::Stock::NEW, "New _Module", "Create a new Module"),
                sigc::mem_fun(*this, &MainWindow::onMenuFileNewMod));
    m_refActionGroup->add(Gtk::Action::create("FileNewRes",
                Gtk::Stock::NEW, "New _Resource", "Create a new Resource"),
                sigc::mem_fun(*this, &MainWindow::onMenuFileNewRes));
               
    m_refActionGroup->add( Gtk::Action::create("FileOpen", Gtk::Stock::OPEN  ,"_Open File", "Open xml file"),
                        sigc::mem_fun(*this, &MainWindow::onMenuFileOpen) );
    m_refActionGroup->add( Gtk::Action::create("FileClose", Gtk::Stock::CLOSE),
                        sigc::mem_fun(*this, &MainWindow::onMenuFileClose) );

    m_refActionGroup->add( Gtk::Action::create("FileSave", Gtk::Stock::SAVE),
                        sigc::mem_fun(*this, &MainWindow::onMenuFileSave) );
    m_refActionGroup->add( Gtk::Action::create("FileSaveAs", Gtk::Stock::SAVE_AS),
                        sigc::mem_fun(*this, &MainWindow::onMenuFileSaveAs) );
    m_refActionGroup->add( Gtk::Action::create("FileImport", Gtk::StockID("YIMPORT") ,"_Import Files...", "Import xml files"),
                        sigc::mem_fun(*this, &MainWindow::onMenuFileImport) );

    m_refActionGroup->add(Gtk::Action::create("FileQuit", Gtk::Stock::QUIT),
          sigc::mem_fun(*this, &MainWindow::onMenuFileQuit));
    
    //Edit menu:
    m_refActionGroup->add( Gtk::Action::create("EditMenu", "Edit") );
    m_refActionGroup->add( Gtk::Action::create("EditSelAll", Gtk::StockID("YSELECTALL"), "Select All", "Select all"),
                        sigc::mem_fun(*this, &MainWindow::onMenuEditSellAll) );
    m_refActionGroup->add( Gtk::Action::create("EditExportGraph", Gtk::Stock::CONVERT, "Export graph...", "Export graph"),
                        sigc::mem_fun(*this, &MainWindow::onMenuEditExportGraph) );

    //Manage menu:
    m_refActionGroup->add( Gtk::Action::create("ManageMenu", "Manage") );
    m_refActionGroup->add( Gtk::Action::create("ManageRun", Gtk::StockID("YRUN"), "_Run", "Run Application"),
                            sigc::mem_fun(*this, &MainWindow::onMenuManageRun) );
    m_refActionGroup->add( Gtk::Action::create("ManageStop", Gtk::StockID("YSTOP") ,"_Stop", "Stop Application"),
                            sigc::mem_fun(*this, &MainWindow::onMenuManageStop) );
    m_refActionGroup->add( Gtk::Action::create("ManageKill", Gtk::StockID("YKILL"),"_Kill", "Kill Application"),
                            sigc::mem_fun(*this, &MainWindow::onMenuManageKill) );
    m_refActionGroup->add( Gtk::Action::create("ManageConnect", Gtk::StockID("YCONNECT"), "_Connect", "Connect links"),
                            sigc::mem_fun(*this, &MainWindow::onMenuManageConnect) );
    m_refActionGroup->add( Gtk::Action::create("ManageDisconnect", Gtk::StockID("YDISCONNECT"), "_Disconnect", "Disconnect links"),
                            sigc::mem_fun(*this, &MainWindow::onMenuManageDisconnect) );
    m_refActionGroup->add( Gtk::Action::create("ManageRefresh", Gtk::StockID("YREFRESH"), "Re_fresh Status", "Refresh Status"),
                            sigc::mem_fun(*this, &MainWindow::onMenuManageRefresh) );

    //Help menu:
    m_refActionGroup->add( Gtk::Action::create("HelpMenu", "Help") );
    m_refActionGroup->add( Gtk::Action::create("HelpOnline", Gtk::Stock::HELP),
                            sigc::mem_fun(*this, &MainWindow::onMenuHelpOnlineHelp) );
    m_refActionGroup->add( Gtk::Action::create("HelpAbout", Gtk::Stock::ABOUT),
                            sigc::mem_fun(*this, &MainWindow::onMenuHelpAbout) );
    
    //Popup Application
    m_refActionGroup->add( Gtk::Action::create("PAppRemove", Gtk::Stock::REMOVE, "_Remove", "Remove"),
                            sigc::mem_fun(*this, &MainWindow::onPAppMenuRemove) );
    m_refActionGroup->add( Gtk::Action::create("PAppReopen", Gtk::Stock::REFRESH, "R_eopen", "Reopen"),
                            sigc::mem_fun(*this, &MainWindow::onPAppMenuReopen) );
                           
    m_refActionGroup->add( Gtk::Action::create("PAppLoad", Gtk::Stock::APPLY, "_Load Application", "Load Application"),
                            sigc::mem_fun(*this, &MainWindow::onPAppMenuLoad) );
    m_refActionGroup->add( Gtk::Action::create("PModLoad", Gtk::Stock::APPLY, "_Load Module", "Load Module"),
                            sigc::mem_fun(*this, &MainWindow::onPAppMenuLoad) );
    m_refActionGroup->add( Gtk::Action::create("PResLoad", Gtk::Stock::APPLY, "_Load Resource", "Load Resource"),
                            sigc::mem_fun(*this, &MainWindow::onPAppMenuLoad) );
    m_refActionGroup->add( Gtk::Action::create("PEditFile", Gtk::Stock::EDIT , "_Edit File", "Edit xml file"),
                        sigc::mem_fun(*this, &MainWindow::onPAppMenuLoad) );

    // initial sensitivity
    //m_refActionGroup->get_action("FileNew")->set_sensitive(false);
    m_refActionGroup->get_action("FileClose")->set_sensitive(false);
    m_refActionGroup->get_action("FileSave")->set_sensitive(false);
    m_refActionGroup->get_action("FileSaveAs")->set_sensitive(false);

    m_refActionGroup->get_action("ManageRun")->set_sensitive(false);
    m_refActionGroup->get_action("ManageStop")->set_sensitive(false);
    m_refActionGroup->get_action("ManageKill")->set_sensitive(false);
    m_refActionGroup->get_action("ManageConnect")->set_sensitive(false);
    m_refActionGroup->get_action("ManageDisconnect")->set_sensitive(false);
    m_refActionGroup->get_action("EditSelAll")->set_sensitive(false);
    m_refActionGroup->get_action("EditExportGraph")->set_sensitive(false);
    m_refActionGroup->get_action("ManageRefresh")->set_sensitive(false);

}


void MainWindow::reportErrors(void)
{
    ErrorLogger* logger  = ErrorLogger::Instance(); 
    if(logger->errorCount() || logger->warningCount())
    {
        const char* err;
        while((err=logger->getLastError()))
            m_refMessageList->addError(err);

        while((err=logger->getLastWarning()))
            m_refMessageList->addWarning(err);
    }
}

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
    AppTemplate* tmp;
    XmlTempLoader tempload(strPath.c_str(), NULL);
    if(tempload.init())
    {
        while((tmp = tempload.getNextAppTemplate()))
            m_applicationList.addAppTemplate(tmp);
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



void MainWindow::syncApplicationList(void)
{
    KnowledgeBase* kb = lazyManager.getKnowledgeBase();
    ApplicaitonPContainer apps =  kb->getApplications();    
    unsigned int cnt = 0;
    for(ApplicationPIterator itr=apps.begin(); itr!=apps.end(); itr++)
    {
        cnt++;
        m_applicationList.addApplication((*itr));
    }

    ResourcePContainer resources = kb->getResources();
    for(ResourcePIterator itr=resources.begin(); itr!=resources.end(); itr++)
    {
        Computer* comp = dynamic_cast<Computer*>(*itr);
        if(comp)
            m_applicationList.addComputer(comp);
    }
 
    ModulePContainer modules = kb->getModules();
    for(ModulePIterator itr=modules.begin(); itr!=modules.end(); itr++)
           m_applicationList.addModule(*itr);

    if(cnt)
    {
        OSTRINGSTREAM msg;
        msg<< (int) cnt<<" "<<"applications are loaded successfully.";
        m_Statusbar.push(msg.str().c_str());
    }
    else
        m_Statusbar.push("No application is loaded!");
}


bool MainWindow::onDeleteEvent(GdkEventAny* event)
{
    return !safeExit();
}

void MainWindow::onMenuFileQuit()
{
    if(safeExit())
        hide();
}


bool MainWindow::safeExit(void)
{
    bool bSafe = true;  
    for(int i=0; i<m_mainTab.get_n_pages(); i++)
    {
        ApplicationWindow* appWnd = 
                dynamic_cast<ApplicationWindow*>( m_mainTab.get_nth_page(i));
        if(appWnd)
        {
            bSafe &= appWnd->onClose();
            if(!bSafe)
                return false;
        }
    }
    return bSafe;
}


void MainWindow::onMenuFileNewApp() 
{
    ErrorLogger* logger  = ErrorLogger::Instance(); 
    std::string ext_editor;
    if(m_config.check("external_editor"))
        ext_editor = m_config.find("external_editor").asString();
    else
#if defined(WIN32)
        ext_editor = "notepad.exe";
#else
        ext_editor = "gedit";
#endif
    Gtk::FileChooserDialog dialog("Create new Application description file");
    dialog.set_transient_for(*this);
    dialog.set_action(Gtk::FILE_CHOOSER_ACTION_SAVE);
    dialog.set_do_overwrite_confirmation(true);

    //Add response buttons the the dialog:
    dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    dialog.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);

    //Add filters, so that only certain file types can be selected:
    Gtk::FileFilter filter_app;
    filter_app.set_name("Application description files (xml)");
    filter_app.add_mime_type("text/xml");
    dialog.add_filter(filter_app);

    if(dialog.run() == Gtk::RESPONSE_OK)
    {
        string fname = dialog.get_filename();
        ofstream ser(fname.c_str());
        if(ser.is_open())
        {
            ser<<str_app_template<<endl;
            ser.close();
        }
        else
        {
            logger->addError(string("Cannot create ") + fname);
            reportErrors();
            return;
        }

        LocalBroker launcher;
        if(launcher.init(ext_editor.c_str(), fname.c_str(), NULL, NULL, NULL, NULL))
            if(!launcher.start() && strlen(launcher.error()))
            {
                OSTRINGSTREAM msg;
                msg<<"Error while launching "<<ext_editor.c_str();
                msg<<". "<<launcher.error();
                logger->addError(msg);
                reportErrors();
            }           
    }

}

void MainWindow::onMenuFileNewMod() 
{
    ErrorLogger* logger  = ErrorLogger::Instance(); 
    std::string ext_editor;
    if(m_config.check("external_editor"))
        ext_editor = m_config.find("external_editor").asString();
    else
#if defined(WIN32)
        ext_editor = "notepad.exe";
#else
        ext_editor = "gedit";
#endif

    Gtk::FileChooserDialog dialog("Create new Module description file");
    dialog.set_transient_for(*this);
    dialog.set_action(Gtk::FILE_CHOOSER_ACTION_SAVE);
    dialog.set_do_overwrite_confirmation(true);

    //Add response buttons the the dialog:
    dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    dialog.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);

    //Add filters, so that only certain file types can be selected:
    Gtk::FileFilter filter_app;
    filter_app.set_name("Module description files (xml)");
    filter_app.add_mime_type("text/xml");
    dialog.add_filter(filter_app);

    if(dialog.run() == Gtk::RESPONSE_OK)
    {
        string fname = dialog.get_filename();
        ofstream ser(fname.c_str());
        if(ser.is_open())
        {
            ser<<str_mod_template<<endl;
            ser.close();
        }
        else
        {
            logger->addError(string("Cannot create ") + fname);
            reportErrors();
            return;
        }

        LocalBroker launcher;
        if(launcher.init(ext_editor.c_str(), fname.c_str(), NULL, NULL, NULL, NULL))
            if(!launcher.start() && strlen(launcher.error()))
            {
                OSTRINGSTREAM msg;
                msg<<"Error while launching "<<ext_editor.c_str();
                msg<<". "<<launcher.error();
                logger->addError(msg);
                reportErrors();
            }           
    }
}


void MainWindow::onMenuFileNewRes() 
{
    ErrorLogger* logger  = ErrorLogger::Instance(); 
    std::string ext_editor;
    if(m_config.check("external_editor"))
        ext_editor = m_config.find("external_editor").asString();
    else
#if defined(WIN32)
        ext_editor = "notepad.exe";
#else
        ext_editor = "gedit";
#endif

   Gtk::FileChooserDialog dialog("Create new Resource description file");
    dialog.set_transient_for(*this);
    dialog.set_action(Gtk::FILE_CHOOSER_ACTION_SAVE);
    dialog.set_do_overwrite_confirmation(true);

    //Add response buttons the the dialog:
    dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    dialog.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);

    //Add filters, so that only certain file types can be selected:
    Gtk::FileFilter filter_app;
    filter_app.set_name("Resource description files (xml)");
    filter_app.add_mime_type("text/xml");
    dialog.add_filter(filter_app);
    
    if(dialog.run() == Gtk::RESPONSE_OK)
    {
        string fname = dialog.get_filename();
        ofstream ser(fname.c_str());
        if(ser.is_open())
        {
            ser<<str_res_template<<endl;
            ser.close();
        }
        else
        {
            logger->addError(string("Cannot create ") + fname);
            reportErrors();
            return;
        }

        LocalBroker launcher;
        if(launcher.init(ext_editor.c_str(), fname.c_str(), NULL, NULL, NULL, NULL))
            if(!launcher.start() && strlen(launcher.error()))
            {
                OSTRINGSTREAM msg;
                msg<<"Error while launching "<<ext_editor.c_str();
                msg<<". "<<launcher.error();
                logger->addError(msg);
                reportErrors();
            }           
    }
}


void MainWindow::onTabCloseRequest(Widget* wdg)
{
    closeTab(m_mainTab.page_num(*wdg));
}


void MainWindow::onMenuFileClose()
{
    closeTab(m_mainTab.get_current_page());
}


void MainWindow::closeTab(int page_num)
{
    ApplicationWindow* appWnd = 
            dynamic_cast<ApplicationWindow*>(m_mainTab.get_nth_page(page_num));
    ResourceWindow* resWnd = 
            dynamic_cast<ResourceWindow*>(m_mainTab.get_nth_page(page_num));
    ModuleWindow* modWnd = 
            dynamic_cast<ModuleWindow*>(m_mainTab.get_nth_page(page_num));

    if(appWnd)
    {
        if(appWnd->onClose())
        {
            m_mainTab.remove_page(page_num);
            delete appWnd;
        }
    }
    else if(resWnd)
    {
        m_mainTab.remove_page(page_num);
        delete resWnd;
    }
    else if(modWnd)
    {
        m_mainTab.remove_page(page_num);
        delete modWnd;
    }

    if(!m_mainTab.get_n_pages())
    {
        m_Statusbar.push("No application is loaded");
        m_refActionGroup->get_action("FileClose")->set_sensitive(false);
        m_refActionGroup->get_action("ManageRun")->set_sensitive(false);
        m_refActionGroup->get_action("ManageStop")->set_sensitive(false);
        m_refActionGroup->get_action("ManageKill")->set_sensitive(false);
        m_refActionGroup->get_action("ManageConnect")->set_sensitive(false);
        m_refActionGroup->get_action("ManageDisconnect")->set_sensitive(false);
        m_refActionGroup->get_action("ManageRefresh")->set_sensitive(false);
        m_refActionGroup->get_action("EditSelAll")->set_sensitive(false);
        m_refActionGroup->get_action("EditExportGraph")->set_sensitive(false);
    }
}

void MainWindow::onMenuFileOpen() 
{
    Gtk::FileChooserDialog dialog("Please choose a folder");
    dialog.set_transient_for(*this);

    //Add response buttons the the dialog:
    dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    dialog.add_button("Open", Gtk::RESPONSE_OK);


    //Add filters, so that only certain file types can be selected:
    Gtk::FileFilter filter_app;
    filter_app.set_name("Application description files");
    filter_app.add_mime_type("text/xml");
    dialog.add_filter(filter_app);

    Gtk::FileFilter filter_mod;
    filter_mod.set_name("Modules description files");
    filter_mod.add_mime_type("text/xml");
    dialog.add_filter(filter_mod);

    Gtk::FileFilter filter_res;
    filter_res.set_name("Resource description files");
    filter_res.add_mime_type("text/xml");
    dialog.add_filter(filter_res);

    Gtk::FileFilter filter_any;
    filter_any.set_name("Any files");
    filter_any.add_pattern("*");
    dialog.add_filter(filter_any);

    if(dialog.run() == Gtk::RESPONSE_OK)
    {
        string fname = dialog.get_filename();
        if(lazyManager.addApplication(fname.c_str()))
        {
            syncApplicationList();
        }
        if(lazyManager.addResource(fname.c_str()))
        {
            syncApplicationList();
        }
        if(lazyManager.addModule(fname.c_str()))
        {
            syncApplicationList();
        }
        reportErrors();
    }

}

void MainWindow::onMenuFileSave() { }
void MainWindow::onMenuFileSaveAs() { }

void MainWindow::onMenuFileImport() 
{
    Gtk::FileChooserDialog dialog("Please choose a folder", Gtk::FILE_CHOOSER_ACTION_SELECT_FOLDER);
    dialog.set_transient_for(*this);

    //Add response buttons the the dialog:
    dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    dialog.add_button("Import", Gtk::RESPONSE_OK);

    if(dialog.run() == Gtk::RESPONSE_OK)
    {
        string fname = dialog.get_filename();
        
        if(m_config.find("load_subfolders").asString() == "yes")
        {
            if(loadRecursiveApplications(fname.c_str()))
                syncApplicationList();
        }
        else
        {
            if(lazyManager.addApplications(fname.c_str()))
                syncApplicationList();
        }

        if(lazyManager.addResources(fname.c_str()))
            syncApplicationList();

        if(lazyManager.addModules(fname.c_str()))
            syncApplicationList();

        reportErrors();
    }

}

void MainWindow::onPAppMenuLoad()
{
    Gtk::TreeModel::iterator iter = 
        m_applicationList.getTreeView()->get_selection()->get_selected();
    if(iter) //If anything is selected
    {
        Gtk::TreeModel::Row row = *iter;
        //Do something with the row.
        if(row[m_applicationList.m_appColumns.m_col_type] == APPLICATION)
        {
            Glib::ustring strName = row[m_applicationList.m_appColumns.m_col_name];
            manageApplication(strName.c_str());
        }
        else if(row[m_applicationList.m_appColumns.m_col_type] == MODULE)
        {
            Glib::ustring strName = row[m_applicationList.m_appColumns.m_col_name];
            manageModule(strName.c_str());
        }
        else if(row[m_applicationList.m_appColumns.m_col_type] == RESOURCE)
        {
            Glib::ustring strName = row[m_applicationList.m_appColumns.m_col_name];
            manageResource(strName.c_str());
        }
        else if(row[m_applicationList.m_appColumns.m_col_type] == NODE_FILENAME)
        {
            Glib::ustring name = row[m_applicationList.m_appColumns.m_col_filename];
            ErrorLogger* logger  = ErrorLogger::Instance(); 
            std::string ext_editor;
            if(m_config.check("external_editor"))
                ext_editor = m_config.find("external_editor").asString();
            else
#if defined(WIN32)
                ext_editor = "notepad.exe";
#else
                ext_editor = "gedit";
#endif
                LocalBroker launcher;
                if(launcher.init(ext_editor.c_str(), name.c_str(), NULL, NULL, NULL, NULL))
                    if(!launcher.start() && strlen(launcher.error()))
                    {
                        OSTRINGSTREAM msg;
                        msg<<"Error while launching "<<m_config.find("external_editor").asString().c_str();
                        msg<<". "<<launcher.error();
                        logger->addError(msg);
                        reportErrors();
                    }
        }

    }
}


void MainWindow::onPAppMenuRemove()
{
    OSTRINGSTREAM msg;
    Gtk::MessageDialog dialog("Removing!", false, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_YES_NO);
    dialog.set_secondary_text("Are you sure to remove this item?");
    if(dialog.run() != Gtk::RESPONSE_YES)
        return;

    Gtk::TreeModel::iterator iter = 
        m_applicationList.getTreeView()->get_selection()->get_selected();
    if(iter) //If anything is selected
    {
        Gtk::TreeModel::Row row = *iter;
        //Do something with the row.
        if(row[m_applicationList.m_appColumns.m_col_type] == APPLICATION)
        {
            Glib::ustring strName = row[m_applicationList.m_appColumns.m_col_name];
            if(lazyManager.removeApplication(strName.c_str()))
                m_applicationList.removeApplication(strName.c_str());
        }
        else if(row[m_applicationList.m_appColumns.m_col_type] == MODULE)
        {
            Glib::ustring strName = row[m_applicationList.m_appColumns.m_col_name];
            int page_num = -1;
            for(int i=0; i<m_mainTab.get_n_pages(); i++)
            {
                Glib::ustring pageName;
                ModuleWindow* pModWnd = 
                        dynamic_cast<ModuleWindow*>(m_mainTab.get_nth_page(i));
                if(pModWnd)
                    pageName = pModWnd->getModuleName();           
                if(pageName == strName)
                {
                    page_num = i;
                    break;
                }
            }

            if(page_num>=0)
                closeTab(page_num);
            if(lazyManager.removeModule(strName.c_str()))
                m_applicationList.removeModule(strName.c_str());
        }
        else if(row[m_applicationList.m_appColumns.m_col_type] == RESOURCE)
        {
            Glib::ustring strName = row[m_applicationList.m_appColumns.m_col_name];
            int page_num = -1;
            for(int i=0; i<m_mainTab.get_n_pages(); i++)
            {
                Glib::ustring pageName;
                ResourceWindow* pResWnd = 
                        dynamic_cast<ResourceWindow*>(m_mainTab.get_nth_page(i));
                if(pResWnd)
                    pageName = pResWnd->getResourceName();           
                if(pageName == strName)
                {
                    page_num = i;
                    break;
                }
            }

            if(page_num>=0)         
                closeTab(page_num);

            if(lazyManager.removeResource(strName.c_str()))
                m_applicationList.removeResource(strName.c_str());
        }
    }
}


void MainWindow::onPAppMenuReopen()
{
    Gtk::TreeModel::iterator iter = 
        m_applicationList.getTreeView()->get_selection()->get_selected();
    if(iter) //If anything is selected
    {
        Gtk::TreeModel::Row row = *iter;
        //Do something with the row.
        if(row[m_applicationList.m_appColumns.m_col_type] == APPLICATION)
        {
            Glib::ustring strName = row[m_applicationList.m_appColumns.m_col_name];
            Glib::ustring strFileName = row[m_applicationList.m_appColumns.m_col_filename];
            lazyManager.removeApplication(strName.c_str());
            if(lazyManager.addApplication(strFileName.c_str()))
                m_Statusbar.push(strFileName + Glib::ustring(" reopened."));
            reportErrors();
        }
        else if(row[m_applicationList.m_appColumns.m_col_type] == MODULE)
        {
            Glib::ustring strName = row[m_applicationList.m_appColumns.m_col_name];
            int page_num = -1;
            for(int i=0; i<m_mainTab.get_n_pages(); i++)
            {
                Glib::ustring pageName;
                ModuleWindow* pModWnd = 
                        dynamic_cast<ModuleWindow*>(m_mainTab.get_nth_page(i));
                if(pModWnd)
                    pageName = pModWnd->getModuleName();           
                if(pageName == strName)
                {
                    page_num = i;
                    break;
                }
            }

            if(page_num>=0)
                closeTab(page_num);

            Glib::ustring strFileName = row[m_applicationList.m_appColumns.m_col_filename];
            lazyManager.removeModule(strName.c_str());
            if(lazyManager.addModule(strFileName.c_str()))
            {
                if(page_num>=0)  
                    manageModule(strName.c_str());
                m_Statusbar.push(strFileName + Glib::ustring(" reopened."));
            }
            reportErrors();
        }
        else if(row[m_applicationList.m_appColumns.m_col_type] == RESOURCE)
        {
            Glib::ustring strName = row[m_applicationList.m_appColumns.m_col_name];
            int page_num = -1;
            for(int i=0; i<m_mainTab.get_n_pages(); i++)
            {
                Glib::ustring pageName;
                ResourceWindow* pResWnd = 
                        dynamic_cast<ResourceWindow*>(m_mainTab.get_nth_page(i));
                if(pResWnd)
                    pageName = pResWnd->getResourceName();           
                if(pageName == strName)
                {
                    page_num = i;
                    break;
                }
            }

            if(page_num>=0)         
                closeTab(page_num);

            Glib::ustring strFileName = row[m_applicationList.m_appColumns.m_col_filename];
            lazyManager.removeResource(strName.c_str());            
            if(lazyManager.addResource(strFileName.c_str()))
            {
                if(page_num>=0)  
                    manageResource(strName.c_str());
                m_Statusbar.push(strFileName + Glib::ustring(" reopened."));
            }
            reportErrors();
            syncApplicationList();
        }
    }
}




void MainWindow::onMenuHelpOnlineHelp() 
{
 #if (GTKMM_MAJOR_VERSION == 2 && GTKMM_MINOR_VERSION >= 16)  
    GError *error = NULL;
    gtk_show_uri(gdk_screen_get_default(), 
                 "http://wiki.icub.org/yarpdoc/yarpmanager.html",
                 gtk_get_current_event_time(), &error);
    if(error)
    {
        Gtk::MessageDialog dialog("Cannot open online help!", false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_CLOSE);
        dialog.set_secondary_text("http://wiki.icub.org/yarpdoc/yarpmanager.html");
        dialog.run();
    }
#else
    Gtk::MessageDialog dialog("Please visit the following link.", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_CLOSE);
    dialog.set_secondary_text("http://wiki.icub.org/yarpdoc/yarpmanager.html");
    dialog.run();

#endif    
}

void MainWindow::onMenuHelpAbout()
{
    Gtk::AboutDialog dialog; 
#if (GTKMM_MAJOR_VERSION == 2 && GTKMM_MINOR_VERSION >= 12)
    dialog.set_program_name("YARP Manager");
#endif

    dialog.set_version("1.2");
    dialog.set_copyright(
            "(C) 2011 Robotics, Brain and Cognitive Sciences\n"
            "Italian Institute of Technology (IIT)");
    dialog.set_license("Released under the terms of the LGPLv2.1 or later.");
    dialog.set_website("http://wiki.icub.org/yarp");
    std::vector<Glib::ustring> authors;
    authors.push_back("Ali Paikan <ali.paikan@iit.it>");
    authors.push_back("Elena Ceseracciu <elena.ceseracciu@iit.it>");
    dialog.set_authors(authors);

    std::vector<Glib::ustring> artists;
    artists.push_back("Alessandro Roncone <Alessandro.Roncone@iit.it>");
    dialog.set_artists(artists);


    dialog.set_logo(Gdk::Pixbuf::create_from_data(ymanager_ico.pixel_data, 
                        Gdk::COLORSPACE_RGB,
                        true,
                        8,
                        ymanager_ico.width,
                        ymanager_ico.height,
                        ymanager_ico.bytes_per_pixel*ymanager_ico.width));

    dialog.run();
}

void MainWindow::onMenuManageRun() 
{
    int page_num = m_mainTab.get_current_page();
    ApplicationWindow* appWnd = dynamic_cast<ApplicationWindow*>( m_mainTab.get_nth_page(page_num));
    if(appWnd)
    {
        appWnd->onPModuleSelectAll();
        appWnd->onRun();
    }
}

void MainWindow::onMenuManageStop() 
{
    int page_num = m_mainTab.get_current_page();
    ApplicationWindow* appWnd = dynamic_cast<ApplicationWindow*>( m_mainTab.get_nth_page(page_num));
    if(appWnd)
    {
        appWnd->onPModuleSelectAll();
        appWnd->onStop();
    }
}

void MainWindow::onMenuManageKill()
{
    int page_num = m_mainTab.get_current_page();
    ApplicationWindow* appWnd = dynamic_cast<ApplicationWindow*>( m_mainTab.get_nth_page(page_num));
    if(appWnd)
    {
        appWnd->onPModuleSelectAll();
        appWnd->onKill();
    }
}

void MainWindow::onMenuManageConnect() 
{
    int page_num = m_mainTab.get_current_page();
    ApplicationWindow* appWnd = dynamic_cast<ApplicationWindow*>( m_mainTab.get_nth_page(page_num));
    if(appWnd)
    {
        appWnd->onPConnectionSelectAll();
        appWnd->onConnect();
    }
}


void MainWindow::onMenuManageDisconnect() 
{
    int page_num = m_mainTab.get_current_page();
    ApplicationWindow* appWnd = dynamic_cast<ApplicationWindow*>( m_mainTab.get_nth_page(page_num));
    if(appWnd)
    {
        appWnd->onPConnectionSelectAll();
        appWnd->onDisconnect();
    }
}

void MainWindow::onMenuManageRefresh() 
{
    int page_num = m_mainTab.get_current_page();
    ApplicationWindow* appWnd = dynamic_cast<ApplicationWindow*>( m_mainTab.get_nth_page(page_num));
    ResourceWindow* resWnd = dynamic_cast<ResourceWindow*>( m_mainTab.get_nth_page(page_num));
    if(appWnd)
    {
        appWnd->onSelectAll();
        appWnd->onRefresh();
    }
    else if(resWnd)
        resWnd->onRefresh();
}

void MainWindow::onMenuEditSellAll() 
{
    int page_num = m_mainTab.get_current_page();
    ApplicationWindow* appWnd = 
            dynamic_cast<ApplicationWindow*>(m_mainTab.get_nth_page(page_num));
    if(appWnd)
        appWnd->onSelectAll();
}

void MainWindow::onMenuEditExportGraph() 
{
    int page_num = m_mainTab.get_current_page();
    ApplicationWindow* appWnd = 
            dynamic_cast<ApplicationWindow*>(m_mainTab.get_nth_page(page_num));
    if(appWnd)
        appWnd->onExportGraph();
}


void MainWindow::onAppListButtonPressed(GdkEventButton* event)
{
    //if it's a mouse click 
    if(event->type == GDK_BUTTON_PRESS)
    {
#if (GTKMM_MAJOR_VERSION == 2 && GTKMM_MINOR_VERSION >= 16)
        Gtk::TreeModel::Path path;
        bool bOnItem = m_applicationList.getTreeView()->get_path_at_pos(
                                                    (int)event->x, 
                                                    (int)event->y, path);
        bool bOnAppItem = false;
        bool bOnModItem = false;
        bool bOnResItem = false;
        bool bOnFileItem = false;
        bool bOnFolderItem = false;
        if(path)
        {
            Gtk::TreeModel::iterator iter = 
                    m_applicationList.m_refTreeModel->get_iter(path);
            if((*iter)[m_applicationList.m_appColumns.m_col_type] == APPLICATION)
                bOnAppItem = true;
            if((*iter)[m_applicationList.m_appColumns.m_col_type] == MODULE)
                bOnModItem = true;
            if((*iter)[m_applicationList.m_appColumns.m_col_type] == RESOURCE)
                bOnResItem = true;
             if((*iter)[m_applicationList.m_appColumns.m_col_type] == NODE_FILENAME)
                bOnFileItem = true;             
             if((*iter)[m_applicationList.m_appColumns.m_col_type] == NODE_OTHER)
                bOnFolderItem = true;
       }
            //Do something with the row.

        // if it's not a free click
        if(bOnItem && bOnAppItem && (event->button == 3))
        {   
            Gtk::Menu* pMenu = dynamic_cast<Gtk::Menu*>(
                        m_refUIManager->get_widget("/PopupApplication"));
            if(pMenu)
                pMenu->popup(event->button, event->time);
        }
        else if(bOnItem && bOnModItem && (event->button == 3))
        {
            Gtk::Menu* pMenu = dynamic_cast<Gtk::Menu*>(
                        m_refUIManager->get_widget("/PopupModule"));
            if(pMenu)
                pMenu->popup(event->button, event->time);
        }
        else if(bOnItem && bOnResItem && (event->button == 3))
        {
            Gtk::Menu* pMenu = dynamic_cast<Gtk::Menu*>(
                        m_refUIManager->get_widget("/PopupResource"));
            if(pMenu)
                pMenu->popup(event->button, event->time);
        }
        else if(bOnItem && bOnFolderItem && (event->button == 3))        
        {
            Gtk::Menu* pMenu = dynamic_cast<Gtk::Menu*>(
                        m_refUIManager->get_widget("/PopupFolders"));
            if(pMenu)
                pMenu->popup(event->button, event->time);
        }        
        else if(bOnItem && bOnFileItem && (event->button == 3))
        {
             Gtk::Menu* pMenu = dynamic_cast<Gtk::Menu*>(
                        m_refUIManager->get_widget("/PopupFile"));
            if(pMenu)
                pMenu->popup(event->button, event->time);          
        }
#else
        // if it's a right click 
        if(event->button == 3)
        {
            Gtk::Menu* pMenu = dynamic_cast<Gtk::Menu*>(
                        m_refUIManager->get_widget("/PopupGeneral"));
            if(pMenu)
                pMenu->popup(event->button, event->time);
        }
#endif        
    }
}

void MainWindow::onAppListRowActivated(const Gtk::TreeModel::Path& path, 
            Gtk::TreeViewColumn* column)
{
    Gtk::TreeModel::iterator iter = m_applicationList.m_refTreeModel->get_iter(path);
    if(iter && ((*iter)[m_applicationList.m_appColumns.m_col_type] == APPLICATION)) 
    {
        Gtk::TreeModel::Row row = *iter;
        Glib::ustring name = row[m_applicationList.m_appColumns.m_col_name];
        manageApplication(name.c_str());
    }
    else if(iter && ((*iter)[m_applicationList.m_appColumns.m_col_type] == RESOURCE ))
    {
        Gtk::TreeModel::Row row = *iter;
        Glib::ustring name = row[m_applicationList.m_appColumns.m_col_name];
        manageResource(name.c_str());
    }
    else if(iter && ((*iter)[m_applicationList.m_appColumns.m_col_type] == MODULE ))
    {
        Gtk::TreeModel::Row row = *iter;
        Glib::ustring name = row[m_applicationList.m_appColumns.m_col_name];
        manageModule(name.c_str());
    }

    else if(iter && ((*iter)[m_applicationList.m_appColumns.m_col_type] == NODE_FILENAME ))
    {
        Gtk::TreeModel::Row row = *iter;
        Glib::ustring name = row[m_applicationList.m_appColumns.m_col_filename];
        ErrorLogger* logger  = ErrorLogger::Instance();
        std::string ext_editor;
       if(m_config.check("external_editor"))
            ext_editor = m_config.find("external_editor").asString();
       else
#if defined(WIN32)
            ext_editor = "notepad.exe";
#else
            ext_editor = "gedit";
#endif
        LocalBroker launcher;
        if(launcher.init(ext_editor.c_str(), name.c_str(), NULL, NULL, NULL, NULL))
            if(!launcher.start() && strlen(launcher.error()))
            {
                OSTRINGSTREAM msg;
                msg<<"Error while launching "<<ext_editor.c_str();
                msg<<". "<<launcher.error();
                logger->addError(msg);
                reportErrors();
            }
    }

}


void MainWindow::manageApplication(const char* szName)
{
    Glib::ustring name = szName;

    int page_num = -1;
    for(int i=0; i<m_mainTab.get_n_pages(); i++)
    {
        Glib::ustring pageName;
        ApplicationWindow* pAppWnd = 
                dynamic_cast<ApplicationWindow*>(m_mainTab.get_nth_page(i));
        if(pAppWnd)
            pageName = pAppWnd->getApplicationName();           
        if(pageName == name)
        {
            page_num = i;
            break;
        }
    }

    if(page_num>=0)
        m_mainTab.set_current_page(page_num);
    else
    {
        ApplicationWindow* pAppWnd = new ApplicationWindow(name.c_str(), 
                                        &lazyManager, &m_config, this);
//          m_mainTab.append_page(*pAppWnd, name);
        Gtk::HBox* hb = Gtk::manage( new Gtk::HBox());
        Gtk::Label* lb = Gtk::manage(new Gtk::Label(name));
        lb->set_text(name);
        //Gtk::EventBox* ev = Gtk::manage(new Gtk::EventBox);
        //ev->add(*lb);
        hb->pack_start(*lb);
        Gtk::Button* bt = Gtk::manage(new Gtk::Button());
        Gtk::Image* ico = Gtk::manage(new Gtk::Image(Gdk::Pixbuf::create_from_data(close_ico.pixel_data, 
                    Gdk::COLORSPACE_RGB,
                    true,
                    8,
                    close_ico.width,
                    close_ico.height,
                    close_ico.bytes_per_pixel*close_ico.width)));
        bt->add(*ico);
        bt->set_size_request(24,24);
        bt->set_relief(Gtk::RELIEF_NONE);
        bt->signal_clicked().connect(sigc::mem_fun(*pAppWnd, 
                                    &ApplicationWindow::onTabCloseRequest));
        hb->set_spacing(1);
        hb->set_border_width(0);
        hb->pack_start(*bt, Gtk::PACK_SHRINK);
        Gtk::Label* ml = Gtk::manage(new Gtk::Label);
        ml->set_text(name);
        m_mainTab.append_page(*pAppWnd, *hb, *ml);
        hb->show_all_children();
#if (GTKMM_MAJOR_VERSION == 2 && GTKMM_MINOR_VERSION >= 10)
        m_mainTab.set_tab_reorderable(*pAppWnd);
#endif        
        m_mainTab.show_all_children();
        m_mainTab.set_current_page(m_mainTab.get_n_pages()-1);
    }
    
    m_refActionGroup->get_action("FileClose")->set_sensitive(true);
    m_refActionGroup->get_action("ManageRun")->set_sensitive(true);
    m_refActionGroup->get_action("ManageStop")->set_sensitive(true);
    m_refActionGroup->get_action("ManageKill")->set_sensitive(true);
    m_refActionGroup->get_action("ManageConnect")->set_sensitive(true);
    m_refActionGroup->get_action("ManageDisconnect")->set_sensitive(true);
    m_refActionGroup->get_action("ManageRefresh")->set_sensitive(true);
    m_refActionGroup->get_action("EditSelAll")->set_sensitive(true);
    m_refActionGroup->get_action("EditExportGraph")->set_sensitive(true);
}

void MainWindow::manageResource(const char* szName)
{
    Glib::ustring name = szName;
    int page_num = -1;
    for(int i=0; i<m_mainTab.get_n_pages(); i++)
    {
        Glib::ustring pageName;
        ResourceWindow* pResWnd = 
                dynamic_cast<ResourceWindow*>(m_mainTab.get_nth_page(i));
        if(pResWnd)
            pageName = pResWnd->getResourceName();           
        if(pageName == name)
        {
            page_num = i;
            break;
        }
    }

    if(page_num>=0)
        m_mainTab.set_current_page(page_num);
    else
    {
        Computer* comp = 
            dynamic_cast<Computer*>(lazyManager.getKnowledgeBase()->getResource(name.c_str()));
        if(!comp)
            return;

        ResourceWindow* pResWnd = new ResourceWindow(comp,  this, &lazyManager);
        Gtk::HBox* hb = Gtk::manage( new Gtk::HBox());
        Gtk::Label* lb = Gtk::manage(new Gtk::Label(name));
        lb->set_text(name);
        hb->pack_start(*lb);
        Gtk::Button* bt = Gtk::manage(new Gtk::Button());
        Gtk::Image* ico = Gtk::manage(new Gtk::Image(Gdk::Pixbuf::create_from_data(close_ico.pixel_data, 
                    Gdk::COLORSPACE_RGB,
                    true,
                    8,
                    close_ico.width,
                    close_ico.height,
                    close_ico.bytes_per_pixel*close_ico.width)));
        bt->add(*ico);
        bt->set_size_request(24,24);
        bt->set_relief(Gtk::RELIEF_NONE);
        
        bt->signal_clicked().connect(sigc::mem_fun(*pResWnd, 
                                     &ResourceWindow::onTabCloseRequest));       
        hb->set_spacing(1);
        hb->set_border_width(0);
        hb->pack_start(*bt, Gtk::PACK_SHRINK);
        Gtk::Label* ml = Gtk::manage(new Gtk::Label);
        ml->set_text(name);
        m_mainTab.append_page(*pResWnd, *hb, *ml);
        hb->show_all_children();
#if (GTKMM_MAJOR_VERSION == 2 && GTKMM_MINOR_VERSION >= 10)
        m_mainTab.set_tab_reorderable(*pResWnd);
#endif        
        m_mainTab.show_all_children();
        m_mainTab.set_current_page(m_mainTab.get_n_pages()-1);
    }
    m_refActionGroup->get_action("FileClose")->set_sensitive(true);
    m_refActionGroup->get_action("ManageRun")->set_sensitive(false);
    m_refActionGroup->get_action("ManageStop")->set_sensitive(false);
    m_refActionGroup->get_action("ManageKill")->set_sensitive(false);
    m_refActionGroup->get_action("ManageConnect")->set_sensitive(false);
    m_refActionGroup->get_action("ManageDisconnect")->set_sensitive(false);
    m_refActionGroup->get_action("ManageRefresh")->set_sensitive(true);
    m_refActionGroup->get_action("EditSelAll")->set_sensitive(false);
    m_refActionGroup->get_action("EditExportGraph")->set_sensitive(false);
}

void MainWindow::manageModule(const char* szName)
{
    Glib::ustring name = szName;
    int page_num = -1;
    for(int i=0; i<m_mainTab.get_n_pages(); i++)
    {
        Glib::ustring pageName;
        ModuleWindow* pModWnd = 
                dynamic_cast<ModuleWindow*>(m_mainTab.get_nth_page(i));
        if(pModWnd)
            pageName = pModWnd->getModuleName();
        if(pageName == name)
        {
            page_num = i;
            break;
        }
    }
    if(page_num>=0)
        m_mainTab.set_current_page(page_num);
    else
    {
        Module* module = 
            dynamic_cast<Module*>(lazyManager.getKnowledgeBase()->getModule(name.c_str()));
        if(!module)
            return;

        ModuleWindow* pModWnd = new ModuleWindow(module,  this, &lazyManager);
        Gtk::HBox* hb = Gtk::manage( new Gtk::HBox());
        Gtk::Label* lb = Gtk::manage(new Gtk::Label(name));
        lb->set_text(name);
        hb->pack_start(*lb);
        Gtk::Button* bt = Gtk::manage(new Gtk::Button());
        Gtk::Image* ico = Gtk::manage(new Gtk::Image(Gdk::Pixbuf::create_from_data(close_ico.pixel_data, 
                    Gdk::COLORSPACE_RGB,
                    true,
                    8,
                    close_ico.width,
                    close_ico.height,
                    close_ico.bytes_per_pixel*close_ico.width)));
        bt->add(*ico);
        bt->set_size_request(24,24);
        bt->set_relief(Gtk::RELIEF_NONE);
        
        bt->signal_clicked().connect(sigc::mem_fun(*pModWnd, 
                                     &ModuleWindow::onTabCloseRequest));       
        hb->set_spacing(1);
        hb->set_border_width(0);
        hb->pack_start(*bt, Gtk::PACK_SHRINK);
        Gtk::Label* ml = Gtk::manage(new Gtk::Label);
        ml->set_text(name);
        m_mainTab.append_page(*pModWnd, *hb, *ml);
        hb->show_all_children();
#if (GTKMM_MAJOR_VERSION == 2 && GTKMM_MINOR_VERSION >= 10)
        m_mainTab.set_tab_reorderable(*pModWnd);
#endif
        m_mainTab.show_all_children();
        m_mainTab.set_current_page(m_mainTab.get_n_pages()-1);
    }

    m_refActionGroup->get_action("FileClose")->set_sensitive(true);
    m_refActionGroup->get_action("ManageRun")->set_sensitive(false);
    m_refActionGroup->get_action("ManageStop")->set_sensitive(false);
    m_refActionGroup->get_action("ManageKill")->set_sensitive(false);
    m_refActionGroup->get_action("ManageConnect")->set_sensitive(false);
    m_refActionGroup->get_action("ManageDisconnect")->set_sensitive(false);
    m_refActionGroup->get_action("ManageRefresh")->set_sensitive(false);
    m_refActionGroup->get_action("EditSelAll")->set_sensitive(false);
    m_refActionGroup->get_action("EditExportGraph")->set_sensitive(false);
}


void MainWindow::onNotebookSwitchPage(GtkNotebookPage* page, guint page_num)
{
    ApplicationWindow* pAppWnd = dynamic_cast<ApplicationWindow*>(m_mainTab.get_nth_page(page_num));
    ResourceWindow* pResWnd = dynamic_cast<ResourceWindow*>(m_mainTab.get_nth_page(page_num));
    ModuleWindow* pModWnd = dynamic_cast<ModuleWindow*>(m_mainTab.get_nth_page(page_num));
   
    if(pAppWnd)
    {
        OSTRINGSTREAM msg;
        msg<<"Current application: "<<pAppWnd->getApplicationName();
        m_Statusbar.push(msg.str());

        m_refActionGroup->get_action("FileClose")->set_sensitive(true);
        m_refActionGroup->get_action("ManageRun")->set_sensitive(true);
        m_refActionGroup->get_action("ManageStop")->set_sensitive(true);
        m_refActionGroup->get_action("ManageKill")->set_sensitive(true);
        m_refActionGroup->get_action("ManageConnect")->set_sensitive(true);
        m_refActionGroup->get_action("ManageDisconnect")->set_sensitive(true);
        m_refActionGroup->get_action("ManageRefresh")->set_sensitive(true);
        m_refActionGroup->get_action("EditSelAll")->set_sensitive(true);
        m_refActionGroup->get_action("EditExportGraph")->set_sensitive(true);      
    }
    else if(pResWnd)
    {
        OSTRINGSTREAM msg;
        msg<<"Current Resource: "<<pResWnd->getResourceName();
        m_Statusbar.push(msg.str());

        m_refActionGroup->get_action("FileClose")->set_sensitive(true);
        m_refActionGroup->get_action("ManageRun")->set_sensitive(false);
        m_refActionGroup->get_action("ManageStop")->set_sensitive(false);
        m_refActionGroup->get_action("ManageKill")->set_sensitive(false);
        m_refActionGroup->get_action("ManageConnect")->set_sensitive(false);
        m_refActionGroup->get_action("ManageDisconnect")->set_sensitive(false);
        m_refActionGroup->get_action("ManageRefresh")->set_sensitive(true);
        m_refActionGroup->get_action("EditSelAll")->set_sensitive(false);
        m_refActionGroup->get_action("EditExportGraph")->set_sensitive(false);      
    }
    else if(pModWnd)
    {
        OSTRINGSTREAM msg;
        msg<<"Current Module: "<<pModWnd->getModuleName();
        m_Statusbar.push(msg.str());

        m_refActionGroup->get_action("FileClose")->set_sensitive(true);
        m_refActionGroup->get_action("ManageRun")->set_sensitive(false);
        m_refActionGroup->get_action("ManageStop")->set_sensitive(false);
        m_refActionGroup->get_action("ManageKill")->set_sensitive(false);
        m_refActionGroup->get_action("ManageConnect")->set_sensitive(false);
        m_refActionGroup->get_action("ManageDisconnect")->set_sensitive(false);
        m_refActionGroup->get_action("ManageRefresh")->set_sensitive(false);
        m_refActionGroup->get_action("EditSelAll")->set_sensitive(false);
        m_refActionGroup->get_action("EditExportGraph")->set_sensitive(false);      
    }
}


bool MainWindow::onExposeEvent( GdkEventExpose* event)
{
//  cout<<get_height()<<endl;
    m_VPaned.set_position(get_height() - get_height()/3);

    return Gtk::Window::on_expose_event(event); 
}



