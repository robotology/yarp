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

#include "con_property_window.h"
#include "icon_res.h"
#include "ymm-dir.h"

#include "main_window.h"
#include "ext_port_model.h"
#include "application_window.h"

ConnectionPropertyWindow::ConnectionPropertyWindow(MainWindow* parent, 
                               Manager* manager, ApplicationWindow* appWnd)
{   
    m_pParent = parent;
    m_pManager = manager;
    m_pAppWindow = appWnd;
    m_pArrow.clear();

    /* Create a new scrolled window, with scrollbars only if needed */
    set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    add(m_TreeView);

    /* create tree store */
    m_refTreeModel = Gtk::TreeStore::create(m_Columns);
    m_TreeView.set_model(m_refTreeModel);

    //Add the Model’s column to the View’s columns: 
    Gtk::CellRendererText* itemRenderer = Gtk::manage(new Gtk::CellRendererText());
    itemRenderer->property_editable() = false;
    Gtk::TreeViewColumn* itemCol = Gtk::manage(new Gtk::TreeViewColumn("Property", *itemRenderer));
    //itemCol->add_attribute(*itemRenderer, "background-gdk", m_Columns.m_col_color);
    itemCol->add_attribute(*itemRenderer, "text", m_Columns.m_col_name);
    itemCol->set_resizable(true);
    m_TreeView.append_column(*itemCol);



    Gtk::CellRendererText* valueRenderer = Gtk::manage(new Gtk::CellRendererText());
    valueRenderer->property_editable() = false;
    Gtk::TreeViewColumn* valueCol = Gtk::manage(new Gtk::TreeViewColumn("Value", *valueRenderer));
    valueCol->add_attribute(*valueRenderer, "foreground-gdk", m_Columns.m_col_color_value);
    valueCol->add_attribute(*valueRenderer, "text", m_Columns.m_col_value);
    valueCol->set_resizable(true);
    valueRenderer->property_editable() = true;
    //valueCol->set_cell_data_func(*valueRenderer, sigc::mem_fun(*this,
    //    &ConnectionPropertyWindow::onCellData) );
    valueRenderer->signal_edited().connect( sigc::mem_fun(*this,
              &ConnectionPropertyWindow::onCellEdited) );
    m_TreeView.append_column(*valueCol);
    valueCol->add_attribute(*valueRenderer, "editable", m_Columns.m_col_editable);   


    //m_TreeView.append_column_editable("Value", m_Columns.m_col_value);
    //m_TreeView.get_column(1)->set_resizable(true);
       //Tell the view column how to render the model values:
    m_TreeView.set_property("enable_grid_lines", true);
    show_all_children();
    m_TreeView.set_grid_lines(Gtk::TREE_VIEW_GRID_LINES_BOTH);
    m_TreeView.set_rules_hint(true);
}


ConnectionPropertyWindow::~ConnectionPropertyWindow()
{
}

void ConnectionPropertyWindow::onRefresh()
{
    if(!m_pArrow)
        return;
    update(m_pArrow);
    m_pParent->reportErrors();
}

void ConnectionPropertyWindow::onTabCloseRequest() 
{ 
    m_pParent->onTabCloseRequest(this); 
}

void ConnectionPropertyWindow::release(void)
{
    m_pArrow.clear();
}

void ConnectionPropertyWindow::update(Glib::RefPtr<ArrowModel> &arrow)
{
    m_pArrow = arrow;
    Connection* m_pConnection = arrow->getConnection();
    m_refTreeModel->clear();


    Gtk::TreeModel::Row row;
//    Gtk::TreeModel::Row childrow;
//    Gtk::TreeModel::Row cchildrow;

    row = *(m_refTreeModel->append());
    row[m_Columns.m_col_name] = "From";
    row[m_Columns.m_col_value] = m_pConnection->from();
    Glib::RefPtr<ExternalPortModel> port = Glib::RefPtr<ExternalPortModel>::cast_dynamic(m_pArrow->getSource());
    bool bNested = port && (port->get_parent() != m_pAppWindow->getRootModel());
    if(m_pConnection->getCorOutputData() || bNested)
    {
        row[m_Columns.m_col_color_value] = Gdk::Color("#888888");
        row[m_Columns.m_col_editable] = false;
    }
    else
        row[m_Columns.m_col_editable] = true;

    row = *(m_refTreeModel->append());
    row[m_Columns.m_col_name] = "To";
    row[m_Columns.m_col_value] = m_pConnection->to();
    port = Glib::RefPtr<ExternalPortModel>::cast_dynamic(m_pArrow->getDestination());
    bNested = port && (port->get_parent() != m_pAppWindow->getRootModel());
    if(m_pConnection->getCorInputData() || bNested)
    {
        row[m_Columns.m_col_color_value] = Gdk::Color("#888888");
        row[m_Columns.m_col_editable] = false;
    }
    else
        row[m_Columns.m_col_editable] = true;

    row = *(m_refTreeModel->append());
    row[m_Columns.m_col_name] = "Carrier";
    row[m_Columns.m_col_value] = m_pConnection->carrier();
    row[m_Columns.m_col_editable] = true;

    row = *(m_refTreeModel->append());
    row[m_Columns.m_col_name] = "Persistent";
    row[m_Columns.m_col_value] = (m_pConnection->isPersistent()) ? "yes" : "no";
    row[m_Columns.m_col_editable] = true;

    m_TreeView.expand_all();
}

void ConnectionPropertyWindow::onCellEdited(const Glib::ustring& path_string, 
                    const Glib::ustring& new_text)
{
    if(!m_pArrow)
        return;

    Connection* m_pConnection = m_pArrow->getConnection();

    Gtk::TreePath path(path_string);

    //Get the row from the path:
    Gtk::TreeModel::iterator iter = m_refTreeModel->get_iter(path);
    if(iter)
    {
        if(m_pAppWindow)
            m_pAppWindow->setModified();
        Gtk::TreeModel::Row row = *iter;
        //Put the new value in the model:
        Glib::ustring strName = Glib::ustring(row[m_Columns.m_col_name]);
    
        row[m_Columns.m_col_value] = new_text;

        Application* application = m_pManager->getKnowledgeBase()->getApplication();
        if(strName == "From")
        {
            // updating source port name in model
            Glib::RefPtr<ExternalPortModel> port = Glib::RefPtr<ExternalPortModel>::cast_dynamic(m_pArrow->getSource());
            if(port)
                port->setPort(new_text.c_str());
        }
        else
        if(strName == "To")
        {
            // updating source port name in model
            Glib::RefPtr<ExternalPortModel> port = Glib::RefPtr<ExternalPortModel>::cast_dynamic(m_pArrow->getDestination());
            if(port)
                port->setPort(new_text.c_str());
        }
        else
        if(strName == "Carrier")
        {
            Connection con = *m_pConnection;
            con.setCarrier(new_text.c_str());
            m_pManager->getKnowledgeBase()->updateConnectionOfApplication(application, *m_pConnection, con);
            m_pConnection->setCarrier(new_text.c_str());
            m_pArrow->setLabel(new_text.c_str());
        }
        else
        if(strName == "Persistent")
        {
            Connection con = *m_pConnection;
            if(compareString(new_text.c_str(), "yes")) 
                con.setPersistent(true);
            else
                con.setPersistent(false);
            m_pManager->getKnowledgeBase()->updateConnectionOfApplication(application, *m_pConnection, con);
            if(compareString(new_text.c_str(), "yes")) 
                m_pConnection->setPersistent(true);
            else    
                m_pConnection->setPersistent(false);
        }
   }
}


