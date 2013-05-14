/*
 *  Yarp Modules Manager
 *  Copyright: 2011 (C) Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#if defined(WIN32)
    #pragma warning (disable : 4250)
    #pragma warning (disable : 4520)
#endif

#include "application_window.h"
#include "main_window.h"
#include "icon_res.h"

using namespace std;

ApplicationWindow::ApplicationWindow(const char* szAppName, Manager* lazy, 
                                    yarp::os::Property* config, MainWindow* parent)
{
    dummy_h = dummy_w = 0;
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
    //set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    add(m_VPaned);
    m_ScrollModView.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    m_ScrollModView.add(m_TreeModView);
    m_ScrollConView.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    m_ScrollConView.add(m_TreeConView);
    m_ScrollResView.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    m_ScrollResView.add(m_TreeResView);

    m_VPaned.add1(m_ScrollModView);
    m_VPaned.add2(m_HPaned);
    m_HPaned.add1(m_ScrollConView);
    m_HPaned.add2(m_ScrollResView);

    /* create list store */
    m_refTreeModModel = Gtk::TreeStore::create(m_modColumns);
    m_TreeModView.set_model(m_refTreeModModel);
    m_TreeModView.set_rules_hint(true);
    m_refTreeConModel = Gtk::TreeStore::create(m_conColumns);
    m_TreeConView.set_model(m_refTreeConModel);
    m_TreeConView.set_rules_hint(true);
    m_refTreeResModel = Gtk::TreeStore::create(m_resColumns);
    m_TreeResView.set_model(m_refTreeResModel);
    m_TreeResView.set_rules_hint(true);


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

    m_refPixUnknown = Gdk::Pixbuf::create_from_data(res_comp_unknown_ico.pixel_data, 
                                Gdk::COLORSPACE_RGB,
                                true,
                                8,
                                res_comp_unknown_ico.width,
                                res_comp_unknown_ico.height,
                                res_comp_unknown_ico.bytes_per_pixel*res_comp_unknown_ico.width);

    m_refPixComputer = Gdk::Pixbuf::create_from_data(computer_ico.pixel_data, 
                                Gdk::COLORSPACE_RGB,
                                true,
                                8,
                                computer_ico.width,
                                computer_ico.height,
                                computer_ico.bytes_per_pixel*computer_ico.width);

    m_refPixPort = Gdk::Pixbuf::create_from_data(port_ico.pixel_data, 
                                Gdk::COLORSPACE_RGB,
                                true,
                                8,
                                port_ico.width,
                                port_ico.height,
                                port_ico.bytes_per_pixel*port_ico.width);

    m_refPixPortAvaibable = Gdk::Pixbuf::create_from_data(port_avail_ico.pixel_data, 
                                Gdk::COLORSPACE_RGB,
                                true,
                                8,
                                port_avail_ico.width,
                                port_avail_ico.height,
                                port_avail_ico.bytes_per_pixel*port_avail_ico.width);

    m_refPixPortUnAvailable = Gdk::Pixbuf::create_from_data(port_unavail_ico.pixel_data, 
                                Gdk::COLORSPACE_RGB,
                                true,
                                8,
                                port_unavail_ico.width,
                                port_unavail_ico.height,
                                port_unavail_ico.bytes_per_pixel*port_unavail_ico.width);
    m_refPixPortUnknown = Gdk::Pixbuf::create_from_data(port_unknown_ico.pixel_data, 
                                Gdk::COLORSPACE_RGB,
                                true,
                                8,
                                port_unknown_ico.width,
                                port_unknown_ico.height,
                                port_unknown_ico.bytes_per_pixel*port_unknown_ico.width);

    //Add the Model’s column to the Module View’s columns:  
    Gtk::TreeViewColumn* col = Gtk::manage(new Gtk::TreeViewColumn("Module"));
    Gtk::CellRendererText* cellText = Gtk::manage(new Gtk::CellRendererText());
    Gtk::CellRendererPixbuf* cellPix = Gtk::manage(new Gtk::CellRendererPixbuf());
    col->pack_start(*cellPix, false);
    col->pack_start(*cellText, true);
    col->add_attribute(*cellText, "text", 1);
    col->add_attribute(*cellPix, "pixbuf", 0);
    m_TreeModView.append_column(*col);

    m_TreeModView.get_column(0)->set_sort_column(m_modColumns.m_col_name);
    m_TreeModView.get_column(0)->set_resizable(true);

    m_TreeModView.append_column("ID", m_modColumns.m_col_id);
    m_TreeModView.get_column(1)->set_sort_column(m_modColumns.m_col_id);
    m_TreeModView.get_column(1)->set_resizable(true);

    Gtk::CellRendererText* statusRenderer = Gtk::manage(new Gtk::CellRendererText());
    statusRenderer->property_editable() = false;
    Gtk::TreeViewColumn* statusCol = Gtk::manage(new Gtk::TreeViewColumn("Status", *statusRenderer));
    statusCol->add_attribute(*statusRenderer, "foreground-gdk", m_modColumns.m_col_color);
    statusCol->add_attribute(*statusRenderer, "text", m_modColumns.m_col_status);
    statusCol->set_sort_column(m_modColumns.m_col_status);
    statusCol->set_resizable(true);
    m_TreeModView.append_column(*statusCol);


    m_TreeModView.append_column_editable("Host", m_modColumns.m_col_host);
    m_TreeModView.get_column(3)->set_sort_column(m_modColumns.m_col_host);
    m_TreeModView.get_column(3)->set_resizable(true);
    m_TreeModView.get_column(3)->add_attribute(*m_TreeModView.get_column_cell_renderer(3),
                                                     "editable", 
                                                     m_modColumns.m_col_editable);
    m_TreeModView.append_column_editable("Parameters", m_modColumns.m_col_param);
    m_TreeModView.get_column(4)->set_sort_column(m_modColumns.m_col_param);
    m_TreeModView.get_column(4)->set_resizable(true);
    m_TreeModView.get_column(4)->add_attribute(*m_TreeModView.get_column_cell_renderer(4),
                                                     "editable", 
                                                     m_modColumns.m_col_editable);

    m_TreeModView.append_column_editable("Stdio", m_modColumns.m_col_stdio);
    m_TreeModView.get_column(5)->set_sort_column(m_modColumns.m_col_stdio);
    m_TreeModView.get_column(5)->set_resizable(true);
    m_TreeModView.get_column(5)->add_attribute(*m_TreeModView.get_column_cell_renderer(5),
                                                     "editable", 
                                                     m_modColumns.m_col_editable);

    m_TreeModView.append_column_editable("Work Dir", m_modColumns.m_col_wdir);
    m_TreeModView.get_column(6)->set_sort_column(m_modColumns.m_col_env);
    m_TreeModView.get_column(6)->set_resizable(true);
    m_TreeModView.get_column(6)->add_attribute(*m_TreeModView.get_column_cell_renderer(6),
                                                     "editable", 
                                                     m_modColumns.m_col_editable);

    m_TreeModView.append_column_editable("Environment", m_modColumns.m_col_env);
    m_TreeModView.get_column(7)->set_sort_column(m_modColumns.m_col_env);
    m_TreeModView.get_column(7)->set_resizable(true);
    m_TreeModView.get_column(7)->add_attribute(*m_TreeModView.get_column_cell_renderer(7),
                                                     "editable", 
                                                     m_modColumns.m_col_editable);


    //Add the Model’s column to the connection View’s columns:  
    Gtk::TreeViewColumn* ccol = Gtk::manage(new Gtk::TreeViewColumn("Connection"));
    Gtk::CellRendererText* ccellText = Gtk::manage(new Gtk::CellRendererText());
    Gtk::CellRendererPixbuf* ccellPix = Gtk::manage(new Gtk::CellRendererPixbuf());
    ccol->pack_start(*ccellPix, false);
    ccol->pack_start(*ccellText, true);
    ccol->add_attribute(*ccellText, "text", 1);
    ccol->add_attribute(*ccellPix, "pixbuf", 0);
    m_TreeConView.append_column(*ccol);
    
    m_TreeConView.append_column("ID", m_conColumns.m_col_id);

    Gtk::CellRendererText* statusRenderer2 = Gtk::manage(new Gtk::CellRendererText());
    statusRenderer2->property_editable() = false;
    Gtk::TreeViewColumn* statusCol2 = Gtk::manage(new Gtk::TreeViewColumn("Status", *statusRenderer2));
    statusCol2->add_attribute(*statusRenderer2, "foreground-gdk", m_conColumns.m_col_color);
    statusCol2->add_attribute(*statusRenderer2, "text", m_conColumns.m_col_status);
    statusCol2->set_sort_column(m_conColumns.m_col_status);
    statusCol2->set_resizable(true);
    m_TreeConView.append_column(*statusCol2);

    int idx = m_TreeConView.append_column_editable("From", m_conColumns.m_col_from) - 1;
    Gtk::CellRendererText* fromRenderer = 
            dynamic_cast<Gtk::CellRendererText*>(m_TreeConView.get_column_cell_renderer(idx));
    if(fromRenderer)
    {
        m_TreeConView.get_column(idx)->add_attribute(*fromRenderer, 
                                                     "editable", 
                                                     m_conColumns.m_col_editable);
        m_TreeConView.get_column(idx)->add_attribute(*fromRenderer, 
                                                     "foreground-gdk", 
                                                     m_conColumns.m_col_from_color);
        m_TreeConView.get_column(idx)->add_attribute(*fromRenderer, 
                                                     "text", 
                                                     m_conColumns.m_col_from);
        m_TreeConView.get_column(idx)->set_sort_column(m_conColumns.m_col_from);
        m_TreeConView.get_column(idx)->set_resizable(true);
    }

    idx = m_TreeConView.append_column_editable("To", m_conColumns.m_col_to) - 1 ;
    Gtk::CellRendererText* toRenderer = 
            dynamic_cast<Gtk::CellRendererText*>(m_TreeConView.get_column_cell_renderer(idx));
    if(toRenderer)
    {
        //toRenderer->property_editable() = m_conColumns.m_col_editable;
        m_TreeConView.get_column(idx)->add_attribute(*toRenderer, 
                                                     "editable", 
                                                     m_conColumns.m_col_editable);
        m_TreeConView.get_column(idx)->add_attribute(*toRenderer, 
                                                     "foreground-gdk", 
                                                     m_conColumns.m_col_to_color);
        m_TreeConView.get_column(idx)->add_attribute(*toRenderer, 
                                                     "text", 
                                                     m_conColumns.m_col_to);
        m_TreeConView.get_column(idx)->set_sort_column(m_conColumns.m_col_to);
        m_TreeConView.get_column(idx)->set_resizable(true);
    }

    m_TreeConView.append_column_editable("Carrier", m_conColumns.m_col_carrier);
    m_TreeConView.get_column(5)->add_attribute(*m_TreeConView.get_column_cell_renderer(5),
                                                     "editable", 
                                                     m_conColumns.m_col_editable);

    m_TreeConView.get_column(0)->set_sort_column(m_conColumns.m_col_type);
    m_TreeConView.get_column(0)->set_resizable(true);
    m_TreeConView.get_column(1)->set_sort_column(m_conColumns.m_col_id);
    m_TreeConView.get_column(1)->set_resizable(true);
    m_TreeConView.get_column(2)->set_sort_column(m_conColumns.m_col_status);
    m_TreeConView.get_column(2)->set_resizable(true);
    //m_TreeConView.get_column(3)->set_sort_column(m_conColumns.m_col_from);
    //m_TreeConView.get_column(3)->set_resizable(true);
    m_TreeConView.get_column(4)->set_sort_column(m_conColumns.m_col_to);
    m_TreeConView.get_column(4)->set_resizable(true);
    m_TreeConView.get_column(5)->set_sort_column(m_conColumns.m_col_carrier);
    m_TreeConView.get_column(5)->set_resizable(true);

    //Add the Model’s column to the resource View’s columns:    
    Gtk::TreeViewColumn* rcol = Gtk::manage(new Gtk::TreeViewColumn("Resource"));
    Gtk::CellRendererText* rcellText = Gtk::manage(new Gtk::CellRendererText());
    Gtk::CellRendererPixbuf* rcellPix = Gtk::manage(new Gtk::CellRendererPixbuf());
    rcol->pack_start(*rcellPix, false);
    rcol->pack_start(*rcellText, true);
    rcol->add_attribute(*rcellText, "text", 1);
    rcol->add_attribute(*rcellPix, "pixbuf", 0);
    m_TreeResView.append_column(*rcol);

    m_TreeResView.append_column("ID", m_resColumns.m_col_id);
    
    Gtk::CellRendererText* statusRenderer3 = Gtk::manage(new Gtk::CellRendererText()); 
    statusRenderer3->property_editable() = false;
    Gtk::TreeViewColumn* statusCol3 = Gtk::manage(new Gtk::TreeViewColumn("Status", *statusRenderer3));
    statusCol3->add_attribute(*statusRenderer3, "foreground-gdk", m_resColumns.m_col_color);
    statusCol3->add_attribute(*statusRenderer3, "text", m_resColumns.m_col_status);
    statusCol3->set_sort_column(m_resColumns.m_col_status);
    statusCol3->set_resizable(true);
    m_TreeResView.append_column(*statusCol3);

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
//  m_refActionGroup->add(Gtk::Action::create("PopupModules", "PopupModules"));
    m_refActionGroup->add( Gtk::Action::create("PManageRun", Gtk::StockID("YRUN"), "_Run", "Run Application"),
                            sigc::mem_fun(*this, &ApplicationWindow::onPMenuRun) );
    m_refActionGroup->add( Gtk::Action::create("PManageStop", Gtk::StockID("YSTOP") ,"_Stop", "Stop Application"),
                            sigc::mem_fun(*this, &ApplicationWindow::onPMenuStop) );
    m_refActionGroup->add( Gtk::Action::create("PManageKill", Gtk::StockID("YKILL"),"_Kill", "Kill Application"),
                            sigc::mem_fun(*this, &ApplicationWindow::onPMenuKill) );
    m_refActionGroup->add( Gtk::Action::create("PManageLoadBalance", Gtk::StockID("YPROCESSOR"), "Assign _hosts", "Assign hosts to modules"),
                            sigc::mem_fun(*this, &ApplicationWindow::onPMenuLoadBalance) );
    m_refActionGroup->add( Gtk::Action::create("PManageConnect", Gtk::Stock::CONNECT, "_Connect", "Connect links"),
                            sigc::mem_fun(*this, &ApplicationWindow::onPMenuConnect) );
    m_refActionGroup->add( Gtk::Action::create("PManageDisconnect", Gtk::Stock::DISCONNECT, "_Disconnect", "Disconnect links"),
                            sigc::mem_fun(*this, &ApplicationWindow::onPMenuDisconnect) );
    m_refActionGroup->add( Gtk::Action::create("PManageRefresh", Gtk::Stock::REFRESH, "Re_fresh Status", "Refresh Modules/connections Status"),
                            sigc::mem_fun(*this, &ApplicationWindow::onPMenuRefresh) );
    m_refActionGroup->add( Gtk::Action::create("PManageStdout", Gtk::Stock::NETWORK, "_Attach to stdout", "Attach to standard output"),
                            sigc::mem_fun(*this, &ApplicationWindow::onPMenuAttachStdout) );        
    m_refActionGroup->add( Gtk::Action::create("PModuleSelAll", Gtk::StockID("YSELECTALL"), "Select _all modules", "Select all modules"),
                            sigc::mem_fun(*this, &ApplicationWindow::onPModuleSelectAll) );
    m_refActionGroup->add( Gtk::Action::create("PConenctionSelAll", Gtk::StockID("YSELECTALL"), "_Select all connections", "Select all connections"),
                            sigc::mem_fun(*this, &ApplicationWindow::onPConnectionSelectAll) );
    m_refActionGroup->add( Gtk::Action::create("PReseourceSelAll", Gtk::StockID("YSELECTALL"), "_Select all resources", "Select all resources"),
                            sigc::mem_fun(*this, &ApplicationWindow::onPResourceSelectAll) );

    m_refUIManager = Gtk::UIManager::create();
    m_refUIManager->insert_action_group(m_refActionGroup);
    m_pParent->add_accel_group(m_refUIManager->get_accel_group());
    Glib::ustring ui_info =
        "<ui>"
        " <popup name='PopupModules'>"
        "      <menuitem action='PManageRun'/>"
        "      <menuitem action='PManageStop'/>"
        "      <menuitem action='PManageKill'/>"
        "      <separator/>"
        "      <menuitem action='PManageRefresh'/>"
        "      <menuitem action='PModuleSelAll'/>"
        "      <menuitem action='PManageStdout'/>"
        "      <menuitem action='PManageLoadBalance'/>"
        " </popup>"
        " <popup name='PopupConnections'>"
        "      <menuitem action='PManageConnect'/>"
        "      <menuitem action='PManageDisconnect'/>"
        "      <separator/>"
        "      <menuitem action='PManageRefresh'/>"
        "      <menuitem action='PConenctionSelAll'/>"
        " </popup>"
        " <popup name='PopupResources'>"
        "      <menuitem action='PManageRefresh'/>"
        "      <menuitem action='PReseourceSelAll'/>"
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
        std::cerr << "building popup menus failed: " << ex->what();
    }   
#endif //GLIBMM_EXCEPTIONS_ENABLED

    m_TreeModView.setMouseEventCallback(this, 
                &ApplicationWindow::onModuleTreeButtonPressed);
    m_TreeConView.setMouseEventCallback(this, 
                &ApplicationWindow::onConnectionTreeButtonPressed);
    m_TreeResView.setMouseEventCallback(this, 
                &ApplicationWindow::onResourceTreeButtonPressed);

}

void ApplicationWindow::setupSignals(void)
{
//  m_TreeModView.signal_button_press_event().connect_notify(sigc::mem_fun(*this,
//            &ApplicationWindow::onModuleTreeButtonPressed) );

//  m_TreeConView.signal_button_press_event().connect_notify(sigc::mem_fun(*this,
//            &ApplicationWindow::onConnectionTreeButtonPressed) );

//  m_TreeResView.signal_button_press_event().connect_notify(sigc::mem_fun(*this,
//            &ApplicationWindow::onResourceTreeButtonPressed) );
//

/*
#if (GTKMM_MAJOR_VERSION == 2 && GTKMM_MINOR_VERSION >= 6)
    for(unsigned int i=3; i<m_TreeModView.get_columns().size(); i++)
    {
        Gtk::CellRendererText* render = 
            dynamic_cast<Gtk::CellRendererText*>(m_TreeModView.get_column_cell_renderer(i));
        if(render)
        {
            render->signal_editing_started().connect( sigc::mem_fun(*this,
                &ApplicationWindow::onModuleEditingStarted) );               
            //render->signal_edited().connect( sigc::mem_fun(*this,
            //    &ApplicationWindow::onModuleEdited) );               
        }
    }

    for(unsigned int i=3; i<m_TreeConView.get_columns().size(); i++)
    {
        Gtk::CellRendererText* render = 
            dynamic_cast<Gtk::CellRendererText*>(m_TreeConView.get_column_cell_renderer(i));
        if(render)
        {
            render->signal_editing_started().connect( sigc::mem_fun(*this,
                &ApplicationWindow::onConnectionEditingStarted) );
        }
    }
#endif
*/
}

void ApplicationWindow::prepareManagerFrom(Manager* lazy, const char* szAppName)
{
    
    manager.prepare(lazy, m_pConfig, 
                    dynamic_cast<ApplicationEvent*>(this));

    // loading application
    if(manager.loadApplication(szAppName))
       updateApplicationWindow();
    
    reportErrors();
}

void ApplicationWindow::updateApplicationWindow(void)
{
    
    m_refTreeModModel->clear();
    m_refTreeConModel->clear();
    m_refTreeResModel->clear();

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
        m_modRow[m_modColumns.m_col_editable] = true;
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
        
        
        if((*cnnitr).isPersistent())
            m_conRow[m_conColumns.m_col_type] = "Persistent";
        else
        {
            if((*cnnitr).isExternalFrom() || (*cnnitr).isExternalTo())
                m_conRow[m_conColumns.m_col_type] = "External";
            else
                m_conRow[m_conColumns.m_col_type] = "Internal";
        }
        m_conRow[m_conColumns.m_col_from] = (*cnnitr).from();
        m_conRow[m_conColumns.m_col_to] = (*cnnitr).to();
        m_conRow[m_conColumns.m_col_carrier] = (*cnnitr).carrier();
        m_conRow[m_conColumns.m_col_status] = "disconnected";
        m_conRow[m_conColumns.m_col_editable] = true;
        m_conRow[m_conColumns.m_col_color] = Gdk::Color("#BF0303");
        //m_conRow[m_conColumns.m_col_from_color] = Gdk::Color("#BF0303");
    }
    
    id = 0;
    ResourcePIterator itrS;
    for(itrS=manager.getResources().begin(); itrS!=manager.getResources().end(); itrS++)
    {
        m_resRow = *(m_refTreeResModel->append());
        m_resRow[m_resColumns.m_col_id] = id++;
        if(dynamic_cast<Computer*>(*itrS))
        {
            m_resRow.set_value(0, m_refPixUnknown);
            m_resRow[m_resColumns.m_col_type] = "computer";
        }            
        else if(dynamic_cast<ResYarpPort*>(*itrS))
        {
            m_resRow.set_value(0, m_refPixPortUnknown);
            m_resRow[m_resColumns.m_col_type] = "port";
        }            
        m_resRow[m_resColumns.m_col_res] = (*itrS)->getName();
        m_resRow[m_resColumns.m_col_status] = "unknown";
        m_resRow[m_resColumns.m_col_color] = Gdk::Color("#00000");
    }

}



void ApplicationWindow::onModuleTreeButtonPressed(GdkEventButton* event)
{
    //if it's a mouse click 
    if(event->type == GDK_BUTTON_PRESS)
    {   
#if (GTKMM_MAJOR_VERSION == 2 && GTKMM_MINOR_VERSION >= 16)
        Gtk::TreeModel::Path path;
        bool bOnItem = m_TreeModView.get_path_at_pos((int)event->x, 
                                                     (int)event->y, path);
        // if it's not a free click
        if(bOnItem)
        {
            m_refActionGroup->get_action("PManageRun")->set_sensitive(true);
            m_refActionGroup->get_action("PManageStop")->set_sensitive(true);
            m_refActionGroup->get_action("PManageKill")->set_sensitive(true);
            m_refActionGroup->get_action("PManageStdout")->set_sensitive(true);
            m_refActionGroup->get_action("PManageRefresh")->set_sensitive(true);
        }
        else
        {
            m_refTreeModSelection->unselect_all();
            m_refActionGroup->get_action("PManageRun")->set_sensitive(false);
            m_refActionGroup->get_action("PManageStop")->set_sensitive(false);
            m_refActionGroup->get_action("PManageKill")->set_sensitive(false);
            m_refActionGroup->get_action("PManageStdout")->set_sensitive(false);
            m_refActionGroup->get_action("PManageRefresh")->set_sensitive(false);
        }
#endif 
        // if it's a right click 
        if(event->button == 3)
        {
            Gtk::Menu* pMenu = dynamic_cast<Gtk::Menu*>(
                        m_refUIManager->get_widget("/PopupModules"));
            if(pMenu)
                pMenu->popup(event->button, event->time);
        }
    }
}

void ApplicationWindow::onConnectionTreeButtonPressed(GdkEventButton* event)
{
    //if it's a mouse click 
    if(event->type == GDK_BUTTON_PRESS)
    {

#if (GTKMM_MAJOR_VERSION == 2 && GTKMM_MINOR_VERSION >= 16)
        Gtk::TreeModel::Path path;
        bool bOnItem = m_TreeConView.get_path_at_pos((int)event->x, 
                                                     (int)event->y, path);
        // if it's not a free click
        if(bOnItem)
        {
            m_refActionGroup->get_action("PManageConnect")->set_sensitive(true);
            m_refActionGroup->get_action("PManageDisconnect")->set_sensitive(true);
            m_refActionGroup->get_action("PManageRefresh")->set_sensitive(true);
        }
        else
        {
            m_refTreeConSelection->unselect_all();
            m_refActionGroup->get_action("PManageConnect")->set_sensitive(false);
            m_refActionGroup->get_action("PManageDisconnect")->set_sensitive(false);
            m_refActionGroup->get_action("PManageRefresh")->set_sensitive(false);
        }
#endif      
        // if it's a right click 
        if(event->button == 3)
        {
            Gtk::Menu* pMenu = dynamic_cast<Gtk::Menu*>(
                        m_refUIManager->get_widget("/PopupConnections"));
            if(pMenu)
                pMenu->popup(event->button, event->time);
        }
    }
}


void ApplicationWindow::onResourceTreeButtonPressed(GdkEventButton* event)
{
    //if it's a mouse click 
    if(event->type == GDK_BUTTON_PRESS)
    {

#if (GTKMM_MAJOR_VERSION == 2 && GTKMM_MINOR_VERSION >= 16)
        Gtk::TreeModel::Path path;
        bool bOnItem = m_TreeResView.get_path_at_pos((int)event->x, 
                                                     (int)event->y, path);
        // if it's not a free click
        if(bOnItem)
        {
            m_refActionGroup->get_action("PManageRefresh")->set_sensitive(true);
        }
        else
        {
            m_refTreeResSelection->unselect_all();
            m_refActionGroup->get_action("PManageRefresh")->set_sensitive(false);
        }
#endif      
        // if it's a right click 
        if(event->button == 3)
        {
            Gtk::Menu* pMenu = dynamic_cast<Gtk::Menu*>(
                        m_refUIManager->get_widget("/PopupResources"));
            if(pMenu)
                pMenu->popup(event->button, event->time);
        }
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

bool ApplicationWindow::areAllShutdown(void)
{
    typedef Gtk::TreeModel::Children type_children;
    type_children children = m_refTreeModModel->children();
    for(type_children::iterator iter = children.begin(); iter!=children.end(); ++iter)
    {
        if((*iter)[m_modColumns.m_col_status] != Glib::ustring("stopped"))
            return false;
    }

    // check for connection status 
    children = m_refTreeConModel->children();
    for(type_children::iterator iter = children.begin(); iter!=children.end(); ++iter)
    {
        if((*iter)[m_conColumns.m_col_status] != Glib::ustring("disconnected"))
            return false;
    }

    return true;
}


/*
void ApplicationWindow::onModuleEditingStarted(Gtk::CellEditable* cell_editable, 
                                const Glib::ustring& path_string)
{
    Gtk::TreePath path(path_string);
    //Get the row from the path:
    Gtk::TreeModel::iterator iter = m_refTreeModModel->get_iter(path);
    if(iter)
    {
        Gtk::TreeModel::Row row = *iter;
        //Put the new value in the model:
        Glib::ustring strStatus = Glib::ustring(row[m_modColumns.m_col_status]);
        if(strStatus != Glib::ustring("stopped"))
        {
            //Tell the user:          
            OSTRINGSTREAM msg;
            msg<<"Editing "<<Glib::ustring(row[m_modColumns.m_col_name])<<"!";
            Gtk::MessageDialog dialog(msg.str(), false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK);
            dialog.set_secondary_text("Please stop the module before modifying it.");
            dialog.run();
        }
    }
}


void ApplicationWindow::onConnectionEditingStarted(Gtk::CellEditable* cell_editable, 
                                const Glib::ustring& path_string)
{
    Gtk::TreePath path(path_string);
    //Get the row from the path:
    Gtk::TreeModel::iterator iter = m_refTreeConModel->get_iter(path);
    if(iter)
    {
        Gtk::TreeModel::Row row = *iter;
        //Put the new value in the model:
        Glib::ustring strStatus = Glib::ustring(row[m_conColumns.m_col_status]);
        if(strStatus != Glib::ustring("disconnected"))
        {
            //Tell the user:          
            OSTRINGSTREAM msg;
            msg<<"Editing connections!";
            Gtk::MessageDialog dialog(msg.str(), false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK);
            dialog.set_secondary_text("Please stop the connection before modifying it.");
            dialog.run();
        }
    }
}
*/

void ApplicationWindow::setCellsEditable(void)
{
/*
#if (GTKMM_MAJOR_VERSION == 2 && GTKMM_MINOR_VERSION >= 16)
    // for the gtkmm > 2.6 we do not use SetCellEditable and 
    // the corresponding functinality will be taken care by 
    // signal_editing_started()
    //  	
#else
    bool bFlag = areAllShutdown();

    for(unsigned int i=3; i<m_TreeModView.get_columns().size(); i++)
    {
        Gtk::CellRendererText* render = 
            dynamic_cast<Gtk::CellRendererText*>(m_TreeModView.get_column_cell_renderer(i));
        if(render)
        {
            #ifdef GLIBMM_PROPERTIES_ENABLED
                render->property_editable() = bFlag;
            #else
                render->_validated.set_property("editable", bFlag);
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
                render->property_editable() = bFlag;
            #else
                render->_validated.set_property("editable", bFlag);
            #endif      
        }
    }
#endif    
*/
}


bool ApplicationWindow::onRun(void)
{
    if(manager.busy()) return false;

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
             row[m_modColumns.m_col_editable] = false;
             row[m_modColumns.m_col_color] = Gdk::Color("#000000");
             row.set_value(0, m_refPixWaiting);
        }
    }
    setCellsEditable(); 
    manager.safeRun(m_ModuleIDs);
    yarp::os::Time::delay(0.1);
    m_refTreeModSelection->unselect_all();
    return true;    
}


bool ApplicationWindow::onStop(void)
{
    if(manager.busy()) return false;

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
            row[m_modColumns.m_col_editable] = false;
            row[m_modColumns.m_col_color] = Gdk::Color("#000000");
            row.set_value(0, m_refPixWaiting);
        }
    }
    manager.safeStop(m_ModuleIDs);
    yarp::os::Time::delay(0.1);
    m_refTreeModSelection->unselect_all();
    return true;    
}



bool ApplicationWindow::onKill(void)
{
    if(manager.busy()) return false;

    m_ModuleIDs.clear();
    m_refTreeModSelection = m_TreeModView.get_selection();
    m_refTreeModSelection->selected_foreach_iter(
        sigc::mem_fun(*this, &ApplicationWindow::selectedModuleCallback) );

    //if(m_ModuleIDs.size() > 1)
    //{
        Gtk::MessageDialog dialog("Killing modules!", false, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_YES_NO);
        dialog.set_secondary_text("Are you sure?");
        if(dialog.run() != Gtk::RESPONSE_YES)
        {        
            m_refTreeModSelection->unselect_all();
            return false;
        }
   // }   

    for(unsigned int i=0; i<m_ModuleIDs.size(); i++)
    {
        Gtk::TreeModel::Row row;
        if(getModRowByID(m_ModuleIDs[i], &row))
        {
            row[m_modColumns.m_col_status] = "waiting";
            row[m_modColumns.m_col_editable] = false;
            row[m_modColumns.m_col_color] = Gdk::Color("#000000");
            row.set_value(0, m_refPixWaiting);
        }
    }

    // kill selected modules
    manager.safeKill(m_ModuleIDs);
    yarp::os::Time::delay(0.1);
    m_refTreeModSelection->unselect_all();
    return true;    
}



bool ApplicationWindow::onConnect(void) 
{ 
    if(manager.busy()) return false;

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
                row[m_conColumns.m_col_editable] = false;
                row[m_conColumns.m_col_color] = Gdk::Color("#000000");
                row.set_value(0, m_refPixWaiting);
        }
    }

    setCellsEditable();
    manager.safeConnect(m_ConnectionIDs);
    yarp::os::Time::delay(0.1);
    m_refTreeConSelection->unselect_all();
    return true;    
}


bool ApplicationWindow::onDisconnect(void) 
{
    if(manager.busy()) return false;

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
            row[m_conColumns.m_col_editable] = false;
            row[m_conColumns.m_col_color] = Gdk::Color("#000000");
            row.set_value(0, m_refPixWaiting);
        }
    }
    manager.safeDisconnect(m_ConnectionIDs);
    yarp::os::Time::delay(0.1);
    m_refTreeConSelection->unselect_all();
    return true;    
}


bool ApplicationWindow::onRefresh(void)
{
    if(manager.busy()) return false;

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
            row[m_modColumns.m_col_editable] = false;
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
            row[m_conColumns.m_col_editable] = false;
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
    
    manager.safeRefresh(m_ModuleIDs, 
                        m_ConnectionIDs, 
                        m_ResourceIDs);
    yarp::os::Time::delay(0.1);

    m_refTreeModSelection->unselect_all();
    m_refTreeConSelection->unselect_all();
    m_refTreeResSelection->unselect_all();

    return true;    

}


bool ApplicationWindow::onAttachStdout()
{ 
    if(manager.busy()) return false;

    m_ModuleIDs.clear();
    m_refTreeModSelection = m_TreeModView.get_selection();
    m_refTreeModSelection->selected_foreach_iter(
        sigc::mem_fun(*this, &ApplicationWindow::selectedModuleCallback) );

    for(unsigned int i=0; i<m_ModuleIDs.size(); i++)
    {
        int id = m_ModuleIDs[i];
        std::map<int, StdoutWindow*>::iterator itr = m_MapstdWnds.find(id); 
        if(itr == m_MapstdWnds.end())
        {
            Gtk::TreeModel::Row row;
            OSTRINGSTREAM strTitle;
            if(getModRowByID(id, &row))
            {
                string name = Glib::ustring(row[m_modColumns.m_col_name]).c_str();
                strTitle<<getApplicationName()<<":"<<name<<":"<<id;
            }                
            m_MapstdWnds[id] = new StdoutWindow(this, id, strTitle.str().c_str());
        }
        m_MapstdWnds[id]->show();
        m_MapstdWnds[id]->raise();
    }
    manager.safeAttachStdout(m_ModuleIDs);

    return true;
}

bool ApplicationWindow::onDetachStdout(void)
{
    if(manager.busy()) return false;

    m_ModuleIDs.clear();
    m_refTreeModSelection = m_TreeModView.get_selection();
    m_refTreeModSelection->selected_foreach_iter(
        sigc::mem_fun(*this, &ApplicationWindow::selectedModuleCallback) );
    manager.safeDetachStdout(m_ModuleIDs);
    return true;
}

void ApplicationWindow::onPMenuLoadBalance(void)
{
    if(areAllShutdown() && !manager.busy())
    {
        typedef Gtk::TreeModel::Children type_children;
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
        yarp::os::Time::delay(0.1);
    }
    else
    {
        ErrorLogger* logger  = ErrorLogger::Instance(); 
        logger->addError("Running modules should be stopped before assigning hosts.");
        reportErrors();
    }
}


void ApplicationWindow::onTabCloseRequest() 
{ 
    m_pParent->onTabCloseRequest(this); 
}


bool ApplicationWindow::onClose(void) 
{
    if(manager.busy()) return false;

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

    OSTRINGSTREAM msg;
    msg<<"Closing "<<getApplicationName()<<"!";
    Gtk::MessageDialog dialog(msg.str(), false, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_YES_NO);
    dialog.set_secondary_text("You have some running module. After closing the application window you might not be able to recover them. Are you sure?");

    if(dialog.run() != Gtk::RESPONSE_YES)
        return false;

    // killing all application
    //manager.kill();
    //yarp::os::Time::delay(2);
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

bool ApplicationWindow::onExportGraph(void)
{
    Gtk::FileChooserDialog dialog("Export to GraphViz");
    dialog.set_transient_for(*m_pParent);
    dialog.set_action(Gtk::FILE_CHOOSER_ACTION_SAVE);
    dialog.set_do_overwrite_confirmation(true);

    //Add response buttons the the dialog:
    dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    dialog.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);


    //Add filters, so that only certain file types can be selected:
    Gtk::FileFilter filter_any;
    filter_any.set_name("GraphViz format (dot)");
    filter_any.add_pattern("*.dot");
    dialog.add_filter(filter_any);

    if(dialog.run() == Gtk::RESPONSE_OK)
    {
        string fname = dialog.get_filename();
        if(!manager.exportDependencyGraph(fname.c_str()))
        {
            ErrorLogger* logger  = ErrorLogger::Instance(); 
            logger->addError("Cannot export graph");
            reportErrors();
            return false;
        }
    }
    return true;
}


void ApplicationWindow::onModStart(int which) 
{
    GDK_THREADS_ENTER();
    Gtk::TreeModel::Row row;
    if(getModRowByID(which, &row))
    {
        row[m_modColumns.m_col_status] = "running";
        row[m_modColumns.m_col_editable] = false;
        row[m_modColumns.m_col_color] = Gdk::Color("#008C00");
        row.set_value(0, m_refPixRunning);
    }
    reportErrors();
    GDK_THREADS_LEAVE();
}


void ApplicationWindow::onModStop(int which) 
{
    GDK_THREADS_ENTER();
    Gtk::TreeModel::Row row;
    if(getModRowByID(which, &row))
    {
        row[m_modColumns.m_col_status] = "stopped";
        row[m_modColumns.m_col_editable] = true;
        row[m_modColumns.m_col_color] = Gdk::Color("#BF0303");
        row.set_value(0, m_refPixSuspended);
    }
    setCellsEditable();
    reportErrors();
    GDK_THREADS_LEAVE();
}

void ApplicationWindow::onConConnect(int which) 
{
    GDK_THREADS_ENTER();
    Gtk::TreeModel::Row row;
    if(getConRowByID(which, &row))
    {
        row[m_conColumns.m_col_status] = "connected";
        row[m_conColumns.m_col_editable] = false;
        row[m_conColumns.m_col_color] = Gdk::Color("#008C00");
        row.set_value(0, m_refPixConnected);
    }
    reportErrors();
    GDK_THREADS_LEAVE();
}


void ApplicationWindow::onConDisconnect(int which) 
{
    GDK_THREADS_ENTER();
    Gtk::TreeModel::Row row;
    if(getConRowByID(which, &row))
    {
        row[m_conColumns.m_col_status] = "disconnected";
        row[m_conColumns.m_col_editable] = true;
        row[m_conColumns.m_col_color] = Gdk::Color("#BF0303");
        row.set_value(0, m_refPixDisconnected);
    }
    setCellsEditable();
    reportErrors();
    GDK_THREADS_LEAVE();
}


void ApplicationWindow::onConAvailable(int from, int to)
{
    GDK_THREADS_ENTER();
    Gtk::TreeModel::Row row;
    if(from >= 0)
    {
        if(getConRowByID(from, &row))
            row[m_conColumns.m_col_from_color] = Gdk::Color("#008C00");
    }

    if(to >= 0)
    {
        if(getConRowByID(to, &row))
            row[m_conColumns.m_col_to_color] = Gdk::Color("#008C00");
    }
    reportErrors();
    GDK_THREADS_LEAVE();
}


void ApplicationWindow::onConUnAvailable(int from, int to)
{
    GDK_THREADS_ENTER();
    Gtk::TreeModel::Row row;
    if(from >= 0)
    {
        if(getConRowByID(from, &row))
            row[m_conColumns.m_col_from_color] = Gdk::Color("#BF0303");
    }

    if(to >= 0)
    {
        if(getConRowByID(to, &row))
            row[m_conColumns.m_col_to_color] = Gdk::Color("#BF0303");
    }
    reportErrors();
    GDK_THREADS_LEAVE();
}


void ApplicationWindow::onResAvailable(int which)
{
    GDK_THREADS_ENTER();
    Gtk::TreeModel::Row row;
    if(getResRowByID(which, &row))
    {
        row[m_resColumns.m_col_status] = "available";
        row[m_resColumns.m_col_color] = Gdk::Color("#008C00");
        if(row[m_resColumns.m_col_type] == Glib::ustring("computer"))
            row.set_value(0, m_refPixAvailable);
        else
            row.set_value(0, m_refPixPortAvaibable);
    }
    setCellsEditable();
    reportErrors();
    GDK_THREADS_LEAVE();
}

void ApplicationWindow::onResUnAvailable(int which) 
{
    GDK_THREADS_ENTER();
    Gtk::TreeModel::Row row;
    if(getResRowByID(which, &row))
    {
        row[m_resColumns.m_col_status] = "unavailable";
        row[m_resColumns.m_col_color] = Gdk::Color("#BF0303");
        if(row[m_resColumns.m_col_type] == Glib::ustring("computer"))
            row.set_value(0, m_refPixUnAvailable);
        else
            row.set_value(0, m_refPixPortUnAvailable);
    }
    setCellsEditable();
    reportErrors();
    GDK_THREADS_LEAVE();
}

void ApplicationWindow::onError(void) 
{
    GDK_THREADS_ENTER();
    reportErrors();
    GDK_THREADS_LEAVE();
}

void ApplicationWindow::onModStdout(int which, const char* msg)
{
    GDK_THREADS_ENTER();
    std::map<int, StdoutWindow*>::iterator itr = m_MapstdWnds.find(which); 
    if(itr != m_MapstdWnds.end())
    {
        StdoutWindow* wnd = (*itr).second;
        wnd->addMessage(msg);
    }
    GDK_THREADS_LEAVE();
}

void ApplicationWindow::onLoadBalance(void)
{
    GDK_THREADS_ENTER();
    updateApplicationWindow();
    reportErrors();
    GDK_THREADS_LEAVE();
}


void ApplicationWindow::releaseApplication(void)
{
    std::map<int, StdoutWindow*>::iterator itr; 
    for(itr=m_MapstdWnds.begin(); itr!=m_MapstdWnds.end(); itr++)
    {
        (*itr).second->hide();
        delete (*itr).second;
    }
    m_MapstdWnds.clear();
    
#if (GLIBMM_MAJOR_VERSION == 2 && GLIBMM_MINOR_VERSION >= 16)
    m_refPixSuspended.reset();
    m_refPixRunning.reset();
    m_refPixWaiting.reset();
    m_refPixConnected.reset();
    m_refPixDisconnected.reset();
    m_refPixAvailable.reset();
    m_refPixUnAvailable.reset();
#endif    
}


void ApplicationWindow::reportErrors(void)
{
    ErrorLogger* logger  = ErrorLogger::Instance(); 
    if(logger->errorCount() || logger->warningCount())
    {
        const char* err;
        while((err=logger->getLastError()))
        {
            OSTRINGSTREAM msg;
            msg<<"("<<getApplicationName()<<") "<<err; 
            m_pParent->m_refMessageList->addError(msg.str().c_str());
        }

        while((err=logger->getLastWarning()))
        {
            OSTRINGSTREAM msg;
            msg<<"("<<getApplicationName()<<") "<<err; 
            m_pParent->m_refMessageList->addWarning(msg.str().c_str());
        }
    }   
}


