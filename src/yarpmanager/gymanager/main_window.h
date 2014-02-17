/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#ifndef _MAIN_WINDOW_
#define _MAIN_WINDOW_

#include <gtkmm.h>
#include <yarp/os/Network.h>
#include <yarp/os/Property.h>
#include <yarp/os/ConstString.h>

#include <yarp/manager/manager.h>

#include "message_list.h"
#include "application_list.h"

class MainWindow : public Gtk::Window
{

public:
    MainWindow( yarp::os::Property &config);
    virtual ~MainWindow();

    void onTabCloseRequest(Widget* wdg); 
    //Glib::RefPtr<MessagesList> m_refMessageList;
    MessagesList* m_refMessageList;
    
    void reportErrors(void);

protected:

    //Signal handlers:
    bool onExposeEvent(GdkEventExpose* event);
    void onMenuFileQuit();
    void onMenuFileNewApp();
    void onMenuFileNewMod();
    void onMenuFileNewRes();
    void onMenuFileOpen();
    void onMenuFileClose();
    void onMenuFileSave();
    void onMenuFileSaveAs();
    void onMenuFileImport();
    void onMenuHelpAbout();
    void onMenuHelpOnlineHelp();
    void onMenuManageRun();
    void onMenuManageStop();
    void onMenuManageKill();
    void onMenuManageConnect();
    void onMenuManageDisconnect();
    void onMenuManageRefresh();
    void onMenuEditSellAll();
    void onMenuEditExportGraph();

    void onAppListRowActivated(const Gtk::TreeModel::Path& path, 
                Gtk::TreeViewColumn* column);
    void onAppListButtonPressed(GdkEventButton* event);

    void onNotebookSwitchPage(GtkNotebookPage* page, guint page_num);   
    bool onDeleteEvent(GdkEventAny* event);

    void onPAppMenuLoad();
    void onPAppMenuRemove();
    void onPAppMenuReopen();

    void on_size_allocate(Gtk::Allocation& allocation);

private:
    Manager lazyManager;
    yarp::os::Property m_config;

    //Child widgets:
    int dummy_w;
    int dummy_h;
    Gtk::VBox m_VBox;
    Gtk::VPaned m_VPaned;
    Gtk::HPaned m_HPaned;
    Gtk::Statusbar m_Statusbar;
    Glib::RefPtr<Gtk::UIManager> m_refUIManager;
    Glib::RefPtr<Gtk::ActionGroup> m_refActionGroup;
    Gtk::Notebook m_bottomTab;  
    Gtk::Notebook m_mainTab;    
    Gtk::Frame frame1;
    ApplicationList m_applicationList;
    Gtk::TextView m_commandView;
    Glib::RefPtr<Gtk::TextBuffer> m_refCommandBuffer;
    Glib::RefPtr<Gtk::IconFactory> m_factory;

    void manageApplication(const char* szName);
    void manageResource(const char* szName);
    void manageModule(const char* szName);

    bool safeExit(void);
    void closeTab(int page_num);
    void createWidgets(void);
    void setupStocks(void);
    void setupActions(void);
    void setupSignals(void);
    bool loadRecursiveApplications(const char* szPath);
    bool loadRecursiveTemplates(const char* szPath);
    void syncApplicationList(void);
};



#endif //_MAIN_WINDOW_

