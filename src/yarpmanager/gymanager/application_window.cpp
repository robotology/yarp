/*
 *  Yarp Modules Manager
 *  Copyright: 2011 (C) Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#if defined(WIN32) || defined(WIN64)
	#pragma warning (disable : 4250)
	#pragma warning (disable : 4520)
#endif

#include "application_window.h"
#include "main_window.h"
#include "icon_res.h"

#include <sstream>

using namespace std;

ApplicationWindow::ApplicationWindow(const char* szAppName, Manager* lazy, 
									yarp::os::Property* config, MainWindow* parent)
{
	m_bShouldRun = false;
	m_pConfig = config;
	m_pParent = parent;
	m_strAppName = szAppName;

	createWidgets();
	setupSignals();
	show_all_children();
	prepareManagerFrom(lazy, szAppName);
}


ApplicationWindow::~ApplicationWindow()
{
	releaseApplication();
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
								true,
								8,
								suspended_ico.width,
								suspended_ico.height,
								suspended_ico.bytes_per_pixel*suspended_ico.width);

	m_refPixRunning = Gdk::Pixbuf::create_from_data(runnin_ico.pixel_data, 
								Gdk::COLORSPACE_RGB,
								true,
								8,
								runnin_ico.width,
								runnin_ico.height,
								runnin_ico.bytes_per_pixel*runnin_ico.width);
	
	m_refPixWaiting = Gdk::Pixbuf::create_from_data(progress_ico.pixel_data, 
								Gdk::COLORSPACE_RGB,
								true,
								8,
								progress_ico.width,
								progress_ico.height,
								progress_ico.bytes_per_pixel*progress_ico.width);

	m_refPixConnected = Gdk::Pixbuf::create_from_data(connected_ico.pixel_data, 
								Gdk::COLORSPACE_RGB,
								true,
								8,
								connected_ico.width,
								connected_ico.height,
								connected_ico.bytes_per_pixel*connected_ico.width);

	m_refPixDisconnected = Gdk::Pixbuf::create_from_data(disconnected_ico.pixel_data, 
								Gdk::COLORSPACE_RGB,
								true,
								8,
								disconnected_ico.width,
								disconnected_ico.height,
								disconnected_ico.bytes_per_pixel*disconnected_ico.width);

	m_refPixAvailable = Gdk::Pixbuf::create_from_data(yesres_ico.pixel_data, 
								Gdk::COLORSPACE_RGB,
								true,
								8,
								yesres_ico.width,
								yesres_ico.height,
								yesres_ico.bytes_per_pixel*yesres_ico.width);

	m_refPixUnAvailable = Gdk::Pixbuf::create_from_data(nores_ico.pixel_data, 
								Gdk::COLORSPACE_RGB,
								true,
								8,
								nores_ico.width,
								nores_ico.height,
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

	Gtk::CellRendererText statusRenderer;
	statusRenderer.property_editable() = false;
	Gtk::TreeViewColumn statusCol("Status", statusRenderer);
	statusCol.add_attribute(statusRenderer, "foreground-gdk", m_modColumns.m_col_color);
	statusCol.add_attribute(statusRenderer, "text", m_modColumns.m_col_status);
	statusCol.set_sort_column(m_modColumns.m_col_status);
	statusCol.set_resizable(true);
	m_TreeModView.append_column(statusCol);


	m_TreeModView.append_column_editable("Host", m_modColumns.m_col_host);
	m_TreeModView.get_column(3)->set_sort_column(m_modColumns.m_col_host);
	m_TreeModView.get_column(3)->set_resizable(true);
	m_TreeModView.append_column_editable("Parameters", m_modColumns.m_col_param);
	m_TreeModView.get_column(4)->set_sort_column(m_modColumns.m_col_param);
	m_TreeModView.get_column(4)->set_resizable(true);

	m_TreeModView.append_column_editable("Stdio", m_modColumns.m_col_stdio);
	m_TreeModView.get_column(5)->set_sort_column(m_modColumns.m_col_stdio);
	m_TreeModView.get_column(5)->set_resizable(true);

	m_TreeModView.append_column_editable("Work Dir", m_modColumns.m_col_wdir);
	m_TreeModView.get_column(6)->set_sort_column(m_modColumns.m_col_env);
	m_TreeModView.get_column(6)->set_resizable(true);

	m_TreeModView.append_column_editable("Environment", m_modColumns.m_col_env);
	m_TreeModView.get_column(7)->set_sort_column(m_modColumns.m_col_env);
	m_TreeModView.get_column(7)->set_resizable(true);


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

	Gtk::CellRendererText statusRenderer2;
	statusRenderer2.property_editable() = false;
	Gtk::TreeViewColumn statusCol2("Status", statusRenderer2);
	statusCol2.add_attribute(statusRenderer2, "foreground-gdk", m_conColumns.m_col_color);
	statusCol2.add_attribute(statusRenderer2, "text", m_conColumns.m_col_status);
	statusCol2.set_sort_column(m_conColumns.m_col_status);
	statusCol2.set_resizable(true);
	m_TreeConView.append_column(statusCol2);

	m_TreeConView.append_column_editable("From", m_conColumns.m_col_from);
	m_TreeConView.append_column_editable("To", m_conColumns.m_col_to);
	m_TreeConView.append_column_editable("Carrier", m_conColumns.m_col_carrier);


	m_TreeConView.get_column(0)->set_sort_column(m_conColumns.m_col_type);
	m_TreeConView.get_column(0)->set_resizable(true);
	m_TreeConView.get_column(1)->set_sort_column(m_conColumns.m_col_id);
	m_TreeConView.get_column(1)->set_resizable(true);
	m_TreeConView.get_column(2)->set_sort_column(m_conColumns.m_col_status);
	m_TreeConView.get_column(2)->set_resizable(true);
	m_TreeConView.get_column(3)->set_sort_column(m_conColumns.m_col_from);
	m_TreeConView.get_column(3)->set_resizable(true);
	m_TreeConView.get_column(4)->set_sort_column(m_conColumns.m_col_to);
	m_TreeConView.get_column(4)->set_resizable(true);
	m_TreeConView.get_column(5)->set_sort_column(m_conColumns.m_col_carrier);
	m_TreeConView.get_column(5)->set_resizable(true);

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
	
	Gtk::CellRendererText statusRenderer3;
	statusRenderer3.property_editable() = false;
	Gtk::TreeViewColumn statusCol3("Status", statusRenderer3);
	statusCol3.add_attribute(statusRenderer3, "foreground-gdk", m_resColumns.m_col_color);
	statusCol3.add_attribute(statusRenderer3, "text", m_resColumns.m_col_status);
	statusCol3.set_sort_column(m_resColumns.m_col_status);
	statusCol3.set_resizable(true);
	m_TreeResView.append_column(statusCol3);

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
	m_refActionGroup->add( Gtk::Action::create("PManageStop", Gtk::Stock::STOP ,"_Stop", "Stop Application"),
							sigc::mem_fun(*this, &ApplicationWindow::onPMenuStop) );
	m_refActionGroup->add( Gtk::Action::create("PManageKill", Gtk::Stock::CLOSE,"_Kill", "Kill Application"),
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
	
	manager.prepare(lazy, m_pConfig, 
					dynamic_cast<ApplicationEvent*>(this));

	// loading application
	if(manager.loadApplication(szAppName))
	{

		ExecutablePContainer modules = manager.getExecutables();
		CnnContainer connections  = manager.getConnections();
		ExecutablePIterator moditr;
		CnnIterator cnnitr;

		int id = 0;	 
		for(moditr=modules.begin(); moditr<modules.end(); moditr++)
		{
			m_modRow = *(m_refTreeModModel->append());
			m_modRow[m_modColumns.m_col_id] = (*moditr)->getID();
			m_modRow[m_modColumns.m_col_name] = (*moditr)->getCommand();
			m_modRow.set_value(0, m_refPixSuspended);
			m_modRow[m_modColumns.m_col_status] = "stopped";
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
			m_conRow.set_value(0, m_refPixDisconnected);
			
			if((*cnnitr).isExternalFrom() || (*cnnitr).isExternalTo())
				m_conRow[m_conColumns.m_col_type] = "External";
			else
				m_conRow[m_conColumns.m_col_type] = "Internal";

			m_conRow[m_conColumns.m_col_from] = (*cnnitr).from();
			m_conRow[m_conColumns.m_col_to] = (*cnnitr).to();
			m_conRow[m_conColumns.m_col_carrier] = carrierToStr((*cnnitr).carrier());
			m_conRow[m_conColumns.m_col_status] = "disconnected";
			m_conRow[m_conColumns.m_col_color] = Gdk::Color("#BF0303");
		}

		
		id = 0;
		ResourcePIterator itrS;
		for(itrS=manager.getResources().begin(); itrS!=manager.getResources().end(); itrS++)
		{
			m_resRow = *(m_refTreeResModel->append());
			m_resRow[m_resColumns.m_col_id] = id++;
			m_resRow.set_value(0, m_refPixUnAvailable);
			m_resRow[m_resColumns.m_col_res] = (*itrS)->getPort();
			m_resRow[m_resColumns.m_col_status] = "unknown";
			m_resRow[m_resColumns.m_col_color] = Gdk::Color("#00000");
		}
	}
	reportErrors();
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

void ApplicationWindow::setCellsEditable(void)
{
	bool bAllModOk = true;	
	typedef Gtk::TreeModel::Children type_children;
	type_children children = m_refTreeModModel->children();
	for(type_children::iterator iter = children.begin(); iter!=children.end(); ++iter)
	{
		if((*iter)[m_modColumns.m_col_status] != Glib::ustring("stopped"))
		{
			bAllModOk = false;
			break;
		}
	}

	// check for connection status 
	bool bAllConOk = true;	
	children = m_refTreeConModel->children();
	for(type_children::iterator iter = children.begin(); iter!=children.end(); ++iter)
	{
		if((*iter)[m_conColumns.m_col_status] != Glib::ustring("disconnected"))
		{
			bAllConOk = false;
			break;
		}
	}


	for(unsigned int i=3; i<m_TreeModView.get_columns().size(); i++)
	{
		Gtk::CellRendererText* render = 
			dynamic_cast<Gtk::CellRendererText*>(m_TreeModView.get_column_cell_renderer(i));
		if(render)
		{
			#ifdef GLIBMM_PROPERTIES_ENABLED
				render->property_editable() = (bAllModOk && bAllConOk);
			#else
				render->_validated.set_property("editable", (bAllModOk && bAllConOk));
			#endif		
		}
	}

	for(unsigned int i=3; i<m_TreeConView.get_columns().size(); i++)
	{
		Gtk::CellRendererText* render = 
			dynamic_cast<Gtk::CellRendererText*>(m_TreeConView.get_column_cell_renderer(i));
		if(render)
		{
			#ifdef GLIBMM_PROPERTIES_ENABLED
				render->property_editable() = (bAllModOk && bAllConOk);
			#else
				render->_validated.set_property("editable", (bAllModOk && bAllConOk));
			#endif		
		}
	}

}


bool ApplicationWindow::onRun(void)
{
	if(manager.checkSemaphore())
	{
		m_ModuleIDs.clear();
		m_refTreeModSelection = m_TreeModView.get_selection();
		m_refTreeModSelection->selected_foreach_iter(
    		sigc::mem_fun(*this, &ApplicationWindow::selectedModuleCallback) );
				
		// changing icons and updating Executables with cell paramters
		for(unsigned int i=0; i<m_ModuleIDs.size(); i++)
		{
			Gtk::TreeModel::Row row;
			if(getModRowByID(m_ModuleIDs[i], &row))
			{
				manager.updateExecutable(m_ModuleIDs[i], 
					Glib::ustring(row[m_modColumns.m_col_param]).c_str(),
					Glib::ustring(row[m_modColumns.m_col_host]).c_str(), 
					Glib::ustring(row[m_modColumns.m_col_stdio]).c_str(),
					Glib::ustring(row[m_modColumns.m_col_wdir]).c_str(), 
					Glib::ustring(row[m_modColumns.m_col_env]).c_str() );

				row[m_modColumns.m_col_status] = "waiting";
				row[m_modColumns.m_col_color] = Gdk::Color("#000000");
				row.set_value(0, m_refPixWaiting);
			}
		}
		setCellsEditable();	
		manager.postSemaphore();
		manager.safeRun(m_ModuleIDs);
	}
	return true;	
}


bool ApplicationWindow::onStop(void)
{
	if(manager.checkSemaphore())
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
				row.set_value(0, m_refPixWaiting);
			}
		}

		manager.postSemaphore();
		manager.safeStop(m_ModuleIDs);
	}

	return true;	
}



bool ApplicationWindow::onKill(void)
{
	if(manager.checkSemaphore())
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
				row.set_value(0, m_refPixWaiting);
			}
		}
		manager.postSemaphore();
		manager.safeKill(m_ModuleIDs);
	}
	return true;	
}



bool ApplicationWindow::onConnect(void) 
{ 
	if(manager.checkSemaphore())
	{
		m_ConnectionIDs.clear();
		m_refTreeConSelection= m_TreeConView.get_selection();
		m_refTreeConSelection->selected_foreach_iter(
    		sigc::mem_fun(*this, &ApplicationWindow::selectedConnectionCallback) );

		// updating connections with modified cell values 
		for(unsigned int i=0; i<m_ConnectionIDs.size(); i++)
		{
			Gtk::TreeModel::Row row;
			if(getConRowByID(m_ConnectionIDs[i], &row))
			{
				manager.updateConnection(m_ConnectionIDs[i], 
					Glib::ustring(row[m_conColumns.m_col_from]).c_str(), 
					Glib::ustring(row[m_conColumns.m_col_to]).c_str(), 
					Glib::ustring(row[m_conColumns.m_col_carrier]).c_str() );

				row[m_conColumns.m_col_status] = "waiting";
				row[m_conColumns.m_col_color] = Gdk::Color("#000000");
				row.set_value(0, m_refPixWaiting);
			}
		}
		setCellsEditable();
		manager.postSemaphore();
		manager.safeConnect(m_ConnectionIDs);
	}
	return true;	
}


bool ApplicationWindow::onDisconnect(void) 
{
	if(manager.checkSemaphore())
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
				row[m_conColumns.m_col_status] = "waiting";
				row[m_conColumns.m_col_color] = Gdk::Color("#000000");
				row.set_value(0, m_refPixWaiting);
			}
		}
		manager.postSemaphore();
		manager.safeDisconnect(m_ConnectionIDs);
	}
	return true;	
}


bool ApplicationWindow::onRefresh(void)
{
	if(manager.checkSemaphore())
	{		
		m_ModuleIDs.clear();
		m_refTreeModSelection = m_TreeModView.get_selection();
		m_refTreeModSelection->selected_foreach_iter(
    		sigc::mem_fun(*this, &ApplicationWindow::selectedModuleCallback) );
		m_ConnectionIDs.clear();
		m_refTreeConSelection= m_TreeConView.get_selection();
		m_refTreeConSelection->selected_foreach_iter(
    		sigc::mem_fun(*this, &ApplicationWindow::selectedConnectionCallback) );
		m_ResourceIDs.clear();
		m_refTreeResSelection= m_TreeResView.get_selection();
		m_refTreeResSelection->selected_foreach_iter(
    		sigc::mem_fun(*this, &ApplicationWindow::selectedResourceCallback) );

		for(unsigned int i=0; i<m_ModuleIDs.size(); i++)
		{
			Gtk::TreeModel::Row row;
			if(getModRowByID(m_ModuleIDs[i], &row))
			{
				row[m_modColumns.m_col_status] = "waiting";
				row[m_modColumns.m_col_color] = Gdk::Color("#000000");
				row.set_value(0, m_refPixWaiting);
			}
		}

		for(unsigned int i=0; i<m_ConnectionIDs.size(); i++)
		{
			Gtk::TreeModel::Row row;
			if(getConRowByID(m_ConnectionIDs[i], &row))
			{
				row[m_conColumns.m_col_status] = "waiting";
				row[m_conColumns.m_col_color] = Gdk::Color("#000000");
				row.set_value(0, m_refPixWaiting);
			}
		}

		for(unsigned int i=0; i<m_ResourceIDs.size(); i++)
		{
			Gtk::TreeModel::Row row;
			if(getResRowByID(m_ResourceIDs[i], &row))
			{
				row[m_resColumns.m_col_status] = "waiting";
				row[m_resColumns.m_col_color] = Gdk::Color("#000000");
				row.set_value(0, m_refPixWaiting);
			}
		}
		
		manager.postSemaphore();
		manager.safeRefresh(m_ModuleIDs, 
							m_ConnectionIDs, 
							m_ResourceIDs);
	}
	return true;	
}


void ApplicationWindow::onTabCloseRequest() 
{ 
	m_pParent->onTabCloseRequest(this);	
}


bool ApplicationWindow::onClose(void) 
{
	bool bAllStoped = true;	
	typedef Gtk::TreeModel::Children type_children;
	type_children children = m_refTreeModModel->children();
	for(type_children::iterator iter = children.begin(); iter!=children.end(); ++iter)
	{
		if((*iter)[m_modColumns.m_col_status] !=  Glib::ustring("stopped"))
		{
			bAllStoped = false;
			break;
		}
	}

	if(bAllStoped)
	{
		releaseApplication();
		return true;
	}

	ostringstream msg;
	msg<<"Closing "<<getApplicationName()<<"!";
	Gtk::MessageDialog dialog(msg.str(), false, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_YES_NO);
	dialog.set_secondary_text( "Running modules will be killed on close. Are you sure?");

	if(dialog.run() != Gtk::RESPONSE_YES)
		return false;

	// killing all application
	manager.kill();
	yarp::os::Time::delay(2);
	releaseApplication();
	return true;
} 


bool ApplicationWindow::onSelectAll(void)
{
	m_refTreeModSelection->select_all();
	m_refTreeConSelection->select_all();
	m_refTreeResSelection->select_all();
	return true;
}



void ApplicationWindow::onModStart(int which) 
{
	Gtk::TreeModel::Row row;
	if(getModRowByID(which, &row))
	{
		row[m_modColumns.m_col_status] = "running";
		row[m_modColumns.m_col_color] = Gdk::Color("#008C00");
		row.set_value(0, m_refPixRunning);
	}
	reportErrors();
}


void ApplicationWindow::onModStop(int which) 
{
	Gtk::TreeModel::Row row;
	if(getModRowByID(which, &row))
	{
		row[m_modColumns.m_col_status] = "stopped";
		row[m_modColumns.m_col_color] = Gdk::Color("#BF0303");
		row.set_value(0, m_refPixSuspended);
	}
	setCellsEditable();
	reportErrors();
}

void ApplicationWindow::onConConnect(int which) 
{
	Gtk::TreeModel::Row row;
	if(getConRowByID(which, &row))
	{
		row[m_conColumns.m_col_status] = "connected";
		row[m_conColumns.m_col_color] = Gdk::Color("#008C00");
		row.set_value(0, m_refPixConnected);
	}
	reportErrors();
}


void ApplicationWindow::onConDisconnect(int which) 
{
	Gtk::TreeModel::Row row;
	if(getConRowByID(which, &row))
	{
		row[m_conColumns.m_col_status] = "disconnected";
		row[m_conColumns.m_col_color] = Gdk::Color("#BF0303");
		row.set_value(0, m_refPixDisconnected);
	}
	setCellsEditable();
	reportErrors();
}


void ApplicationWindow::onResAvailable(int which)
{
	Gtk::TreeModel::Row row;
	if(getResRowByID(which, &row))
	{
		row[m_resColumns.m_col_status] = "available";
		row[m_resColumns.m_col_color] = Gdk::Color("#008C00");
		row.set_value(0, m_refPixAvailable);
	}
	setCellsEditable();
	reportErrors();
}

void ApplicationWindow::onResUnAvailable(int which) 
{
	Gtk::TreeModel::Row row;
	if(getResRowByID(which, &row))
	{
		row[m_resColumns.m_col_status] = "unavailable";
		row[m_resColumns.m_col_color] = Gdk::Color("#BF0303");
		row.set_value(0, m_refPixUnAvailable);
	}
	setCellsEditable();
	reportErrors();
}

void ApplicationWindow::onError(void) 
{
	reportErrors();
}


void ApplicationWindow::releaseApplication(void)
{
	m_refPixSuspended.reset();
	m_refPixRunning.reset();
	m_refPixWaiting.reset();
	m_refPixConnected.reset();
	m_refPixDisconnected.reset();
	m_refPixAvailable.reset();
	m_refPixUnAvailable.reset();
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


