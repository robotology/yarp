/*
 *  Yarp Modules Manager
 *  Copyright: 2011 (C) Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
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

#include "manager.h"

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

protected:

	//Signal handlers:
	virtual bool onExposeEvent(GdkEventExpose* event);
	virtual void onMenuFileQuit();
	virtual void onMenuFileNewApp();
	virtual void onMenuFileNewMod();
	virtual void onMenuFileOpen();
	virtual void onMenuFileClose();
	virtual void onMenuFileSave();
	virtual void onMenuFileSaveAs();
	virtual void onMenuFileImport();
	virtual void onMenuHelpAbout();
	virtual void onMenuHelpOnlineHelp();
	virtual void onMenuManageRun();
	virtual void onMenuManageStop();
	virtual void onMenuManageKill();
	virtual void onMenuManageConnect();
	virtual void onMenuManageDisconnect();
	virtual void onMenuManageRefresh();
	virtual void onMenuEditSellAll();
	virtual void onAppListRowActivated(const Gtk::TreeModel::Path& path, 
				Gtk::TreeViewColumn* column);
	virtual void onAppListButtonPressed(GdkEventButton* event);

	virtual void onNotebookSwitchPage(GtkNotebookPage* page, guint page_num);	
	virtual bool onDeleteEvent(GdkEventAny* event);

	virtual void onPAppMenuLoad();
	virtual void onPAppMenuRemove();


private:
	Manager lazyManager;
	yarp::os::Property m_config;

	//Child widgets:
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
	bool safeExit(void);
	void closeTab(int page_num);
	void createWidgets(void);
	void setupStocks(void);
	void setupActions(void);
	void setupSignals(void);
	bool loadRecursiveApplications(const char* szPath);
	void reportErrors(void);
	void syncApplicationList(void);
};



#endif //_MAIN_WINDOW_

