/*
 *  Yarp Modules Manager
 *  Copyright: 2011 (C) Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include "application_window.h"
#include "main_window.h"
#include "icon_res.h"

#include <sstream>

using namespace std;

ApplicationWindow::ApplicationWindow(const char* szAppName, Manager* lazy, 
									yarp::os::Property* config, MainWindow* parent) 
									: RateThread(100)
{
	m_bShouldRun = false;
	m_pConfig = config;
	m_pParent = parent;
	m_pAction = NULL;

	createWidgets();
	setupSignals();
	show_all_children();
	prepareManagerFrom(lazy, szAppName);
}


ApplicationWindow::~ApplicationWindow()
{
}

bool ApplicationWindow::threadInit() { return true; }


void ApplicationWindow::afterStart(bool s) {}

void ApplicationWindow::threadRelease() {}

void ApplicationWindow::run()
{
	if(m_pAction)
		(this->*m_pAction)();
	RateThread::stop();
}

void ApplicationWindow::createWidgets(void)
{
	/* Create a new scrolled window, with scrollbars only if needed */
	set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	add(m_VPaned);
	m_VPaned.add1(m_TreeModView);
	m_VPaned.add2(m_HPaned);
	m_HPaned.add1(m_TreeConView);
	m_HPaned.add2(m_TreeResView);

	/* create list store */
	m_refTreeModModel = Gtk::TreeStore::create(m_modColumns);
	m_TreeModView.set_model(m_refTreeModModel);
	m_refTreeConModel = Gtk::TreeStore::create(m_conColumns);
	m_TreeConView.set_model(m_refTreeConModel);
	m_refTreeResModel = Gtk::TreeStore::create(m_resColumns);
	m_TreeResView.set_model(m_refTreeResModel);


	 m_refPixSuspended =  Gdk::Pixbuf::create_from_data(suspended_ico.pixel_data, 
								Gdk::COLORSPACE_RGB,
								90,
								8,
								suspended_ico.height,
								suspended_ico.width,
								suspended_ico.bytes_per_pixel*suspended_ico.width);

	m_refPixRunning = Gdk::Pixbuf::create_from_data(runnin_ico.pixel_data, 
								Gdk::COLORSPACE_RGB,
								90,
								8,
								runnin_ico.height,
								runnin_ico.width,
								runnin_ico.bytes_per_pixel*runnin_ico.width);
	
	m_refPixWaiting = Gdk::Pixbuf::create_from_data(progress_ico.pixel_data, 
								Gdk::COLORSPACE_RGB,
								90,
								8,
								progress_ico.height,
								progress_ico.width,
								progress_ico.bytes_per_pixel*progress_ico.width);

	m_refPixConnected = Gdk::Pixbuf::create_from_data(connected_ico.pixel_data, 
								Gdk::COLORSPACE_RGB,
								90,
								8,
								connected_ico.height,
								connected_ico.width,
								connected_ico.bytes_per_pixel*connected_ico.width);

	m_refPixDisconnected = Gdk::Pixbuf::create_from_data(disconnected_ico.pixel_data, 
								Gdk::COLORSPACE_RGB,
								90,
								8,
								disconnected_ico.height,
								disconnected_ico.width,
								disconnected_ico.bytes_per_pixel*disconnected_ico.width);

	m_refPixAvailable = Gdk::Pixbuf::create_from_data(yesres_ico.pixel_data, 
								Gdk::COLORSPACE_RGB,
								90,
								8,
								yesres_ico.height,
								yesres_ico.width,
								yesres_ico.bytes_per_pixel*yesres_ico.width);

	m_refPixUnAvailable = Gdk::Pixbuf::create_from_data(nores_ico.pixel_data, 
								Gdk::COLORSPACE_RGB,
								90,
								8,
								nores_ico.height,
								nores_ico.width,
								nores_ico.bytes_per_pixel*nores_ico.width);

	//Add the Model’s column to the Module View’s columns:	
	Gtk::TreeViewColumn col("Module");
	Gtk::CellRendererText cellText;
	Gtk::CellRendererPixbuf cellPix;
	col.pack_start(cellPix, false);
	col.pack_start(cellText, true);
	col.add_attribute(cellText, "text", 1);
	col.add_attribute(cellPix, "pixbuf", 0);
	m_TreeModView.append_column(col);

	m_TreeModView.get_column(0)->set_sort_column(m_modColumns.m_col_name);
	m_TreeModView.get_column(0)->set_resizable(true);

	m_TreeModView.append_column("ID", m_modColumns.m_col_id);
	m_TreeModView.get_column(1)->set_sort_column(m_modColumns.m_col_id);
	m_TreeModView.get_column(1)->set_resizable(true);

/*
	m_TreeModView.append_column("Status", m_modColumns.m_col_status);
	m_TreeModView.get_column(2)->set_sort_column(m_modColumns.m_col_status);
	m_TreeModView.get_column(2)->set_resizable(true);
*/

	Gtk::CellRendererText* statusRenderer = manage(new Gtk::CellRendererText());
	statusRenderer->property_editable() = false;
	Gtk::TreeViewColumn *statusCol = manage(new Gtk::TreeViewColumn("Status", *statusRenderer));

	statusCol->add_attribute(*statusRenderer, "foreground-gdk", m_modColumns.m_col_color);
	statusCol->add_attribute(*statusRenderer, "text", m_modColumns.m_col_status);
	statusCol->set_sort_column(m_modColumns.m_col_status);
	statusCol->set_resizable(true);
	m_TreeModView.append_column(*statusCol);


	m_TreeModView.append_column("Host", m_modColumns.m_col_host);
	m_TreeModView.get_column(3)->set_sort_column(m_modColumns.m_col_host);
	m_TreeModView.get_column(3)->set_resizable(true);

	m_TreeModView.append_column("Parameters", m_modColumns.m_col_param);
	m_TreeModView.get_column(4)->set_sort_column(m_modColumns.m_col_param);
	m_TreeModView.get_column(4)->set_resizable(true);

	m_TreeModView.append_column("Stdio", m_modColumns.m_col_stdio);
	m_TreeModView.get_column(5)->set_sort_column(m_modColumns.m_col_stdio);
	m_TreeModView.get_column(5)->set_resizable(true);

	m_TreeModView.append_column("Work Dir", m_modColumns.m_col_wdir);
	m_TreeModView.get_column(6)->set_sort_column(m_modColumns.m_col_env);
	m_TreeModView.get_column(6)->set_resizable(true);

	m_TreeModView.append_column("Environment", m_modColumns.m_col_env);
	m_TreeModView.get_column(7)->set_sort_column(m_modColumns.m_col_env);
	m_TreeModView.get_column(7)->set_resizable(true);

	/*
*/


	//Add the Model’s column to the connection View’s columns:	
	Gtk::TreeViewColumn ccol("Connection");
	Gtk::CellRendererText ccellText;
	Gtk::CellRendererPixbuf ccellPix;
	ccol.pack_start(ccellPix, false);
	ccol.pack_start(ccellText, true);
	ccol.add_attribute(ccellText, "text", 1);
	ccol.add_attribute(ccellPix, "pixbuf", 0);
	m_TreeConView.append_column(ccol);


	m_TreeConView.append_column("ID", m_conColumns.m_col_id);
	m_TreeConView.append_column("To", m_conColumns.m_col_to);
	m_TreeConView.append_column("Carrier", m_conColumns.m_col_carrier);

	//m_TreeConView.append_column("Status", m_conColumns.m_col_status);

	statusRenderer = manage(new Gtk::CellRendererText());
	statusRenderer->property_editable() = false;
	statusCol = manage(new Gtk::TreeViewColumn("Status", *statusRenderer));

	statusCol->add_attribute(*statusRenderer, "foreground-gdk", m_conColumns.m_col_color);
	statusCol->add_attribute(*statusRenderer, "text", m_conColumns.m_col_status);
	statusCol->set_sort_column(m_conColumns.m_col_status);
	statusCol->set_resizable(true);
	m_TreeConView.append_column(*statusCol);


	m_TreeConView.get_column(0)->set_sort_column(m_conColumns.m_col_from);
	m_TreeConView.get_column(0)->set_resizable(true);
	m_TreeConView.get_column(1)->set_sort_column(m_conColumns.m_col_id);
	m_TreeConView.get_column(1)->set_resizable(true);
	m_TreeConView.get_column(2)->set_sort_column(m_conColumns.m_col_to);
	m_TreeConView.get_column(2)->set_resizable(true);
	m_TreeConView.get_column(3)->set_sort_column(m_conColumns.m_col_carrier);
	m_TreeConView.get_column(3)->set_resizable(true);
	//m_TreeConView.get_column(4)->set_sort_column(m_conColumns.m_col_status);
	//m_TreeConView.get_column(4)->set_resizable(true);

	//Add the Model’s column to the resource View’s columns:	
	Gtk::TreeViewColumn rcol("Resource");
	Gtk::CellRendererText rcellText;
	Gtk::CellRendererPixbuf rcellPix;
	rcol.pack_start(rcellPix, false);
	rcol.pack_start(rcellText, true);
	rcol.add_attribute(rcellText, "text", 1);
	rcol.add_attribute(rcellPix, "pixbuf", 0);
	m_TreeResView.append_column(rcol);

	m_TreeResView.append_column("ID", m_resColumns.m_col_id);
	//m_TreeResView.append_column("Status", m_resColumns.m_col_status);
	statusRenderer = manage(new Gtk::CellRendererText());
	statusRenderer->property_editable() = false;
	statusCol = manage(new Gtk::TreeViewColumn("Status", *statusRenderer));

	statusCol->add_attribute(*statusRenderer, "foreground-gdk", m_resColumns.m_col_color);
	statusCol->add_attribute(*statusRenderer, "text", m_resColumns.m_col_status);
	statusCol->set_sort_column(m_resColumns.m_col_status);
	statusCol->set_resizable(true);
	m_TreeResView.append_column(*statusCol);

	m_TreeResView.get_column(0)->set_sort_column(m_resColumns.m_col_res);
	m_TreeResView.get_column(0)->set_resizable(true);
	m_TreeResView.get_column(1)->set_sort_column(m_resColumns.m_col_id);
	m_TreeResView.get_column(1)->set_resizable(true);
	//m_TreeResView.get_column(2)->set_sort_column(m_resColumns.m_col_status);
	//m_TreeResView.get_column(2)->set_resizable(true);

	// enable multiple selections
	m_refTreeModSelection = m_TreeModView.get_selection();
	m_refTreeConSelection = m_TreeConView.get_selection();
	m_refTreeResSelection = m_TreeResView.get_selection();
	m_refTreeModSelection->set_mode(Gtk::SELECTION_MULTIPLE);
	m_refTreeConSelection->set_mode(Gtk::SELECTION_MULTIPLE);
	m_refTreeResSelection->set_mode(Gtk::SELECTION_MULTIPLE);



	// adding popup menubar
	m_refActionGroup = Gtk::ActionGroup::create();
//	m_refActionGroup->add(Gtk::Action::create("PopupModules", "PopupModules"));
	m_refActionGroup->add( Gtk::Action::create("PManageRun", Gtk::Stock::EXECUTE, "_Run", "Run Application"),
							sigc::mem_fun(*this, &ApplicationWindow::onPMenuRun) );
	m_refActionGroup->add( Gtk::Action::create("PManageStop", Gtk::Stock::CLOSE ,"_Stop", "Stop Application"),
							sigc::mem_fun(*this, &ApplicationWindow::onPMenuStop) );
	m_refActionGroup->add( Gtk::Action::create("PManageKill", Gtk::Stock::STOP,"_Kill", "Kill Application"),
							sigc::mem_fun(*this, &ApplicationWindow::onPMenuKill) );
	m_refActionGroup->add( Gtk::Action::create("PManageConnect", Gtk::Stock::CONNECT, "_Connect", "Connect links"),
							sigc::mem_fun(*this, &ApplicationWindow::onPMenuConnect) );
	m_refActionGroup->add( Gtk::Action::create("PManageDisconnect", Gtk::Stock::DISCONNECT, "_Disconnect", "Disconnect links"),
							sigc::mem_fun(*this, &ApplicationWindow::onPMenuDisconnect) );
	m_refActionGroup->add( Gtk::Action::create("PManageRefresh", Gtk::Stock::REFRESH, "Re_fresh Status", "Refresh Modules/connections Status"),
							sigc::mem_fun(*this, &ApplicationWindow::onPMenuRefresh) );

	m_refUIManager = Gtk::UIManager::create();
	m_refUIManager->insert_action_group(m_refActionGroup);
	//::add_accel_group(m_refUIManager->get_accel_group());
	Glib::ustring ui_info =
		"<ui>"
		" <popup name='PopupModules'>"
        "      <menuitem action='PManageRun'/>"
        "      <menuitem action='PManageStop'/>"
        "      <menuitem action='PManageKill'/>"
        "      <separator/>"
        "      <menuitem action='PManageRefresh'/>"
		" </popup>"
		" <popup name='PopupConnections'>"
        "      <menuitem action='PManageConnect'/>"
        "      <menuitem action='PManageDisconnect'/>"
        "      <separator/>"
        "      <menuitem action='PManageRefresh'/>"
		" </popup>"
		" <popup name='PopupResources'>"
        "      <menuitem action='PManageRefresh'/>"
		" </popup>"
		"</ui>";


#ifdef GLIBMM_EXCEPTIONS_ENABLED
	try
	{
		m_refUIManager->add_ui_from_string(ui_info);
	}
	catch(const Glib::Error& ex)
	{
		std::cerr << "building popup menus failed: " << ex.what();
	}
#else
	std::auto_ptr<Glib::Error> ex;
	m_refUIManager->add_ui_from_string(ui_info, ex);
	if(ex.get())
	{
  		std::cerr << "building popu menus failed: " << ex->what();
	}	
#endif //GLIBMM_EXCEPTIONS_ENABLED

}

void ApplicationWindow::setupSignals(void)
{
	m_TreeModView.signal_button_press_event().connect_notify(sigc::mem_fun(*this,
            &ApplicationWindow::onModuleTreeButtonPressed) );

	m_TreeConView.signal_button_press_event().connect_notify(sigc::mem_fun(*this,
            &ApplicationWindow::onConnectionTreeButtonPressed) );

	m_TreeResView.signal_button_press_event().connect_notify(sigc::mem_fun(*this,
            &ApplicationWindow::onResourceTreeButtonPressed) );

}

void ApplicationWindow::prepareManagerFrom(Manager* lazy, const char* szAppName)
{
	
	if(m_pConfig->find("watchdog").asString() == "yes")
		enableWatchDog();
	else
		disableWatchod();

	if(m_pConfig->find("auto_dependency").asString() == "yes")
		enableAutoDependency();
	else
		disableAutoDependency();

	if(m_pConfig->find("auto_connect").asString() == "yes")
		enableAutoConnect();
	else
		disableAutoConnect();

	// making manager from lazy manager
	KnowledgeBase* lazy_kb = lazy->getKnowledgeBase();
	ApplicaitonPContainer apps =  lazy_kb->getApplications();
	for(ApplicationPIterator itr=apps.begin(); itr!=apps.end(); itr++)
		getKnowledgeBase()->addApplication((*itr));

	// loading application
	loadApplication(szAppName);

	
	ExecutablePContainer modules = getExecutables();
	CnnContainer connections  = getConnections();
	ExecutablePIterator moditr;
	CnnIterator cnnitr;

	int id = 0;	 
	for(moditr=modules.begin(); moditr<modules.end(); moditr++)
	{
		m_modRow = *(m_refTreeModModel->append());
		m_modRow[m_modColumns.m_col_id] = (*moditr)->getID();
		m_modRow[m_modColumns.m_col_name] = (*moditr)->getCommand();
		//m_modRow[m_modColumns.m_col_refPix] = Gtk::IconTheme::get_default()->load_icon("process-working", 16);			
		m_modRow[m_modColumns.m_col_refPix]	= m_refPixSuspended;
		m_modRow[m_modColumns.m_col_status] = "suspended";
		m_modRow[m_modColumns.m_col_color] = Gdk::Color("#BF0303");
		m_modRow[m_modColumns.m_col_host] = (*moditr)->getHost();
		m_modRow[m_modColumns.m_col_param] = (*moditr)->getParam();
		m_modRow[m_modColumns.m_col_stdio] = (*moditr)->getStdio();
		m_modRow[m_modColumns.m_col_wdir] = (*moditr)->getWorkDir();
		m_modRow[m_modColumns.m_col_env] = (*moditr)->getEnv();
	}

	id = 0;	 
	for(cnnitr=connections.begin(); cnnitr<connections.end(); cnnitr++)
	{
		m_conRow = *(m_refTreeConModel->append());
		m_conRow[m_conColumns.m_col_id] = id++;
		//m_conRow[m_conColumns.m_col_refPix] = Gtk::IconTheme::get_default()->load_icon("document-properties", 16);	
		m_conRow[m_conColumns.m_col_refPix]	= m_refPixDisconnected;
		m_conRow[m_conColumns.m_col_from] = (*cnnitr).from();
		m_conRow[m_conColumns.m_col_to] = (*cnnitr).to();
		m_conRow[m_conColumns.m_col_carrier] = carrierToStr((*cnnitr).carrier());
		m_conRow[m_conColumns.m_col_status] = "disconnected";
		m_conRow[m_conColumns.m_col_color] = Gdk::Color("#BF0303");
	}

	reportErrors();

	
	id = 0;
	ResourcePIterator itrS;
	for(itrS=getResources().begin(); itrS!=getResources().end(); itrS++)
	{
		m_resRow = *(m_refTreeResModel->append());
		m_resRow[m_resColumns.m_col_id] = id++;
		//m_conRow[m_conColumns.m_col_refPix] = Gtk::IconTheme::get_default()->load_icon("document-properties", 16);	
		m_resRow[m_resColumns.m_col_refPix]	= m_refPixUnAvailable;
		m_resRow[m_resColumns.m_col_res] = (*itrS)->getPort();
		m_resRow[m_resColumns.m_col_status] = "unknown";
		m_resRow[m_resColumns.m_col_color] = Gdk::Color("#00000");
	}

}



void ApplicationWindow::onModuleTreeButtonPressed(GdkEventButton* event)
{
	if((event->type == GDK_BUTTON_PRESS) && (event->button == 3))
	{
		Gtk::Menu* pMenu = dynamic_cast<Gtk::Menu*>(
        			m_refUIManager->get_widget("/PopupModules"));
		if(pMenu)
	  	pMenu->popup(event->button, event->time);
	}
}

void ApplicationWindow::onConnectionTreeButtonPressed(GdkEventButton* event)
{
	if((event->type == GDK_BUTTON_PRESS) && (event->button == 3))
	{
		Gtk::Menu* pMenu = dynamic_cast<Gtk::Menu*>(
        			m_refUIManager->get_widget("/PopupConnections"));
		if(pMenu)
	  		pMenu->popup(event->button, event->time);
	}
}


void ApplicationWindow::onResourceTreeButtonPressed(GdkEventButton* event)
{
	if((event->type == GDK_BUTTON_PRESS) && (event->button == 3))
	{
		Gtk::Menu* pMenu = dynamic_cast<Gtk::Menu*>(
        			m_refUIManager->get_widget("/PopupResources"));
		if(pMenu)
	  		pMenu->popup(event->button, event->time);
	}
}


void ApplicationWindow::selectedModuleCallback(const Gtk::TreeModel::iterator& iter)
{
	Gtk::TreeModel::Row row = *iter;
	m_ModuleIDs.push_back(row[m_modColumns.m_col_id]);
}

void ApplicationWindow::selectedConnectionCallback(const Gtk::TreeModel::iterator& iter)
{
	Gtk::TreeModel::Row row = *iter;
	m_ConnectionIDs.push_back(row[m_conColumns.m_col_id]);
}

void ApplicationWindow::selectedResourceCallback(const Gtk::TreeModel::iterator& iter)
{
	Gtk::TreeModel::Row row = *iter;
	m_ResourceIDs.push_back(row[m_resColumns.m_col_id]);
}


bool ApplicationWindow::getModRowByID(int id, Gtk::TreeModel::Row* row )
{
	typedef Gtk::TreeModel::Children type_children;
	type_children children = m_refTreeModModel->children();
	for(type_children::iterator iter = children.begin(); iter!=children.end(); ++iter)
	{
		if((*iter)[m_modColumns.m_col_id] == id)
		{
			*row = (*iter);
			return true;
		}
	}
	return false;
}


bool ApplicationWindow::getConRowByID(int id, Gtk::TreeModel::Row* row )
{
	typedef Gtk::TreeModel::Children type_children;
	type_children children = m_refTreeConModel->children();
	for(type_children::iterator iter = children.begin(); iter!=children.end(); ++iter)
	{
		if((*iter)[m_conColumns.m_col_id] == id)
		{
			*row = (*iter);
			return true;
		}
	}
	return false;
}


bool ApplicationWindow::getResRowByID(int id, Gtk::TreeModel::Row* row )
{
	typedef Gtk::TreeModel::Children type_children;
	type_children children = m_refTreeResModel->children();
	for(type_children::iterator iter = children.begin(); iter!=children.end(); ++iter)
	{
		if((*iter)[m_resColumns.m_col_id] == id)
		{
			*row = (*iter);
			return true;
		}
	}
	return false;
}



bool ApplicationWindow::onRun(void)
{
	m_pAction = &ApplicationWindow::doRun;
	RateThread::start();
	return true;	
}


void ApplicationWindow::doRun(void) 
{
	m_ModuleIDs.clear();
	m_refTreeModSelection = m_TreeModView.get_selection();
	m_refTreeModSelection->selected_foreach_iter(
    	sigc::mem_fun(*this, &ApplicationWindow::selectedModuleCallback) );

	// changing icons 
	for(unsigned int i=0; i<m_ModuleIDs.size(); i++)
	{
		Gtk::TreeModel::Row row;
		if(getModRowByID(m_ModuleIDs[i], &row))
		{
			row[m_modColumns.m_col_status] = "waiting";
			row[m_modColumns.m_col_color] = Gdk::Color("#000000");
			row[m_modColumns.m_col_refPix] =  m_refPixWaiting;
		}
	}

	for(unsigned int i=0; i<m_ModuleIDs.size(); i++)
		Manager::run(m_ModuleIDs[i], true);
	
	reportErrors();
}


bool ApplicationWindow::onStop(void)
{
	m_pAction = &ApplicationWindow::doStop;
	RateThread::start();
	return true;	
}


void ApplicationWindow::doStop(void) 
{
	m_ModuleIDs.clear();
	m_refTreeModSelection = m_TreeModView.get_selection();
	m_refTreeModSelection->selected_foreach_iter(
    	sigc::mem_fun(*this, &ApplicationWindow::selectedModuleCallback) );
	
	for(unsigned int i=0; i<m_ModuleIDs.size(); i++)
	{
		Gtk::TreeModel::Row row;
		if(getModRowByID(m_ModuleIDs[i], &row))
		{
			row[m_modColumns.m_col_status] = "waiting";
			row[m_modColumns.m_col_color] = Gdk::Color("#000000");
			row[m_modColumns.m_col_refPix] =  m_refPixWaiting;
		}
	}

	for(unsigned int i=0; i<m_ModuleIDs.size(); i++)
		Manager::stop(m_ModuleIDs[i], true);

	reportErrors();

}



bool ApplicationWindow::onKill(void)
{
	m_pAction = &ApplicationWindow::doKill;
	RateThread::start();
	return true;	
}


void ApplicationWindow::doKill(void) 
{
	m_ModuleIDs.clear();
	m_refTreeModSelection = m_TreeModView.get_selection();
	m_refTreeModSelection->selected_foreach_iter(
    	sigc::mem_fun(*this, &ApplicationWindow::selectedModuleCallback) );
	
	for(unsigned int i=0; i<m_ModuleIDs.size(); i++)
	{
		Gtk::TreeModel::Row row;
		if(getModRowByID(m_ModuleIDs[i], &row))
		{
			row[m_modColumns.m_col_status] = "waiting";
			row[m_modColumns.m_col_color] = Gdk::Color("#000000");
			row[m_modColumns.m_col_refPix] =  m_refPixWaiting;
		}
	}
	
	for(unsigned int i=0; i<m_ModuleIDs.size(); i++)
		Manager::kill(m_ModuleIDs[i], true);

	reportErrors();
}


bool ApplicationWindow::onConnect(void) 
{ 
	m_pAction = &ApplicationWindow::doConnect;
	RateThread::start();
	return true;	
}

void ApplicationWindow::doConnect(void) 
{
	m_ConnectionIDs.clear();
	m_refTreeConSelection= m_TreeConView.get_selection();
	m_refTreeConSelection->selected_foreach_iter(
    	sigc::mem_fun(*this, &ApplicationWindow::selectedConnectionCallback) );
	
	for(unsigned int i=0; i<m_ConnectionIDs.size(); i++)
	{
		Gtk::TreeModel::Row row;
		if(getConRowByID(m_ConnectionIDs[i], &row))
		{
			if(connect(m_ConnectionIDs[i]))
			{
				row[m_conColumns.m_col_status] = "connected";
				row[m_conColumns.m_col_color] = Gdk::Color("#008C00");
				row[m_conColumns.m_col_refPix] =  m_refPixConnected;
			}
		}

	}
	
	reportErrors();
}



bool ApplicationWindow::onDisconnect(void) 
{
	m_pAction = &ApplicationWindow::doDisconnect;
	RateThread::start();
	return true;	

}

void ApplicationWindow::doDisconnect(void) 
{
	m_ConnectionIDs.clear();
	m_refTreeConSelection= m_TreeConView.get_selection();
	m_refTreeConSelection->selected_foreach_iter(
    	sigc::mem_fun(*this, &ApplicationWindow::selectedConnectionCallback) );
	
	for(unsigned int i=0; i<m_ConnectionIDs.size(); i++)
	{
		Gtk::TreeModel::Row row;
		if(getConRowByID(m_ConnectionIDs[i], &row))
		{
			if(disconnect(m_ConnectionIDs[i]))
			{
				row[m_conColumns.m_col_status] = "disconnected";
				row[m_conColumns.m_col_color] = Gdk::Color("#BF0303");
				row[m_conColumns.m_col_refPix] =  m_refPixDisconnected;
			}
		}

	}
	
	reportErrors();
}


bool ApplicationWindow::onRefresh(void)
{
	m_pAction = &ApplicationWindow::doRefresh;
	RateThread::start();
	return true;	
}

void ApplicationWindow::doRefresh(void) 
{

	/**
	 * Refreshing modules status 
	 */ 
	m_ModuleIDs.clear();
	m_refTreeModSelection = m_TreeModView.get_selection();
	m_refTreeModSelection->selected_foreach_iter(
    	sigc::mem_fun(*this, &ApplicationWindow::selectedModuleCallback) );

	for(unsigned int i=0; i<m_ModuleIDs.size(); i++)
	{
		Gtk::TreeModel::Row row;
		if(getModRowByID(m_ModuleIDs[i], &row))
		{
			row[m_modColumns.m_col_status] = "waiting";
			row[m_modColumns.m_col_color] = Gdk::Color("#000000");
			row[m_modColumns.m_col_refPix] =  m_refPixWaiting;
		}
	}

	for(unsigned int i=0; i<m_ModuleIDs.size(); i++)
	{
		Gtk::TreeModel::Row row;
		if(getModRowByID(m_ModuleIDs[i], &row))
		{
			if (Manager::running(m_ModuleIDs[i]))
			{
				row[m_modColumns.m_col_status] = "running";
				row[m_modColumns.m_col_color] = Gdk::Color("#008C00");
				row[m_modColumns.m_col_refPix] = m_refPixRunning;
			}
			else
			{
				row[m_modColumns.m_col_status] = "suspended";
				row[m_modColumns.m_col_color] = Gdk::Color("#BF0303");
				row[m_modColumns.m_col_refPix] = m_refPixSuspended; 
			}
		}
	}
	
	/*
	 * Refreshing connection status
	 */
	m_ConnectionIDs.clear();
	m_refTreeConSelection= m_TreeConView.get_selection();
	m_refTreeConSelection->selected_foreach_iter(
    	sigc::mem_fun(*this, &ApplicationWindow::selectedConnectionCallback) );
	
	for(unsigned int i=0; i<m_ConnectionIDs.size(); i++)
	{
		Gtk::TreeModel::Row row;
		if(getConRowByID(m_ConnectionIDs[i], &row))
		{
			if(!connected(m_ConnectionIDs[i]))
			{
				row[m_conColumns.m_col_status] = "disconnected";
				row[m_conColumns.m_col_color] = Gdk::Color("#BF0303");
				row[m_conColumns.m_col_refPix] = m_refPixDisconnected; 
			}
			else
			{
				row[m_conColumns.m_col_status] = "connected";
				row[m_conColumns.m_col_color] = Gdk::Color("#008C00");
				row[m_conColumns.m_col_refPix] = m_refPixConnected;
			}

		}
	}


	/*
	 * Refreshing resources status
	 */
	m_ResourceIDs.clear();
	m_refTreeResSelection= m_TreeResView.get_selection();
	m_refTreeResSelection->selected_foreach_iter(
    	sigc::mem_fun(*this, &ApplicationWindow::selectedResourceCallback) );
	
	for(unsigned int i=0; i<m_ResourceIDs.size(); i++)
	{
		Gtk::TreeModel::Row row;
		if(getResRowByID(m_ResourceIDs[i], &row))
		{
			if(exist(m_ResourceIDs[i]))
			{
				row[m_resColumns.m_col_status] = "available";
				row[m_resColumns.m_col_color] = Gdk::Color("#008C00");
				row[m_resColumns.m_col_refPix] = m_refPixAvailable;
			}
			else
			{
				row[m_resColumns.m_col_status] = "not available";
				row[m_resColumns.m_col_color] = Gdk::Color("#BF0303");
				row[m_resColumns.m_col_refPix] = m_refPixUnAvailable;
			}
		}
	}

	reportErrors();
} 


bool ApplicationWindow::onClose(void) 
{
	bool bAllStoped = true;	
	typedef Gtk::TreeModel::Children type_children;
	type_children children = m_refTreeModModel->children();
	for(type_children::iterator iter = children.begin(); iter!=children.end(); ++iter)
	{
		if((*iter)[m_modColumns.m_col_status] !=  Glib::ustring("suspended"))
		{
			bAllStoped = false;
			break;
		}
	}

	if(bAllStoped)
	{
		RateThread::stop();
		return true;
	}

	ostringstream msg;
	msg<<"Closing "<<getApplicationName()<<"!";
	Gtk::MessageDialog dialog(msg.str(), false, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_YES_NO);
	dialog.set_secondary_text( "Running modules will be killed on close. Are you sure?");

	if(dialog.run() != Gtk::RESPONSE_YES)
		return false;

	// killing all application
	Manager::kill();
	return true;
} 



bool ApplicationWindow::onSelectAll(void)
{
	return true;
}



void ApplicationWindow::onExecutableStart(void* which) 
{
	Executable* exe = (Executable*) which;	
	Gtk::TreeModel::Row row;
	if(getModRowByID(exe->getID(), &row))
	{
		row[m_modColumns.m_col_status] = "running";
		row[m_modColumns.m_col_color] = Gdk::Color("#008C00");
		row[m_modColumns.m_col_refPix] = m_refPixRunning;
	}
	reportErrors();
}

void ApplicationWindow::onExecutableStop(void* which) 
{
	Executable* exe = (Executable*) which;	
	Gtk::TreeModel::Row row;
	if(getModRowByID(exe->getID(), &row))
	{
		row[m_modColumns.m_col_status] = "suspended";
		row[m_modColumns.m_col_color] = Gdk::Color("#BF0303");
		row[m_modColumns.m_col_refPix] = m_refPixSuspended;
	}

	reportErrors();
}

void ApplicationWindow::onExecutableDied(void* which) 
{
	Executable* exe = (Executable*) which;
	
	Gtk::TreeModel::Row row;
	if(getModRowByID(exe->getID(), &row))
	{
		row[m_modColumns.m_col_status] = "suspended";
		row[m_modColumns.m_col_color] = Gdk::Color("#BF0303");
		row[m_modColumns.m_col_refPix] = m_refPixSuspended;
	}
	reportErrors();
}


void ApplicationWindow::onExecutableFailed(void* which) 
{
	ErrorLogger* logger  = ErrorLogger::Instance();	
	Executable* exe = (Executable*) which;
	if(m_pConfig->find("module_failure").asString() == "prompt")
	{
		ostringstream err;
		err<<exe->getCommand()<<" from "<<exe->getHost()<<" is failed! [id:"<<exe->getID()<<"]";
		logger->addError(err);
	}

	if(m_pConfig->find("module_failure").asString() == "recover")
	{
		ostringstream err;
		err<<exe->getCommand()<<" from "<<exe->getHost()<<" is failed! [id:"<<exe->getID()<<"] (restarting...)";
		logger->addError(err);
		exe->start();
	 }

	if(m_pConfig->find("module_failure").asString() == "terminate")
	{
		ostringstream err;
		err<<exe->getCommand()<<" from "<<exe->getHost()<<" is failed! [id:"<<exe->getID()<<"] (terminating...)";
		logger->addError(err);
		Manager::stop();
	}

	reportErrors();
}


void ApplicationWindow::onCnnStablished(void* which) 
{

}


void ApplicationWindow::onCnnFailed(void* which) 
{
	ErrorLogger* logger  = ErrorLogger::Instance();	
	Connection* cnn = (Connection*) which;
	if( m_pConfig->find("connection_failure").asString() == "prompt")
	{
		ostringstream err;
		err<<"connection failed between "<<cnn->from()<<" and "<<cnn->to();
		logger->addError(err);

	}

	if(m_pConfig->find("connection_failure").asString() == "terminate")
	{
		ostringstream err;
		err<<"connection failed between "<<cnn->from()<<" and "<<cnn->to()<<" (terminating...)";
		logger->addError(err);
		Manager::stop();
	}

	reportErrors();
}


void ApplicationWindow::reportErrors(void)
{
	ErrorLogger* logger  = ErrorLogger::Instance();	
	if(logger->errorCount() || logger->warningCount())
	{
		const char* err;
		while((err=logger->getLastError()))
		{
			ostringstream msg;
			msg<<"("<<getApplicationName()<<") "<<err; 
			m_pParent->m_messageList.addError(msg.str().c_str());
		}

		while((err=logger->getLastWarning()))
		{
			ostringstream msg;
			msg<<"("<<getApplicationName()<<") "<<err; 
			m_pParent->m_messageList.addWarning(msg.str().c_str());
		}
	}	
}


