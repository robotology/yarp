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
#endif

#include "con_property_window.h"
#include "icon_res.h"
#include "ymm-dir.h"

#include "main_window.h"
#include "ext_port_model.h"
#include "port_abitrator_model.h"
#include "application_window.h"

#include <yarp/os/Property.h>
#include <yarp/os/Bottle.h>

using namespace yarp::os;

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
    Gtk::TreeModel::Row childrow;
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
    carrierRow = row; 
    string carrier = m_pConnection->carrier();
    if(carrier.find("recv.priority") != std::string::npos)
    {
        //printf("%s\n", getCarrierOptions(m_pConnection->carrier()));
        //string carrier = m_pConnection->carrier();
        string protocol;
        if(carrier.find("+") == std::string::npos)
            protocol = carrier;
        else
            protocol = carrier.substr(0, carrier.find("+"));
        
        bool dbg = (carrier.find("debug") != std::string::npos);
        bool aux = (carrier.find("virtual") != std::string::npos) ||  
                   (carrier.find("auxilary") != std::string::npos);
        
        double st = 1.0;
        int tc = 1;
        // extracting st
        size_t start = carrier.find("+st.");
        if(start != std::string::npos)
        {
            size_t end = carrier.find("+", start+1);
            if(end == std::string::npos)
                end = carrier.length();
            string dummy = carrier.substr(start+4, end-(start+4));
            if(dummy.length())
                st = atof(dummy.c_str())/10.0;
        }

        // extracting tc
        start = carrier.find("+tc.");
        if(start != std::string::npos)
        {
            size_t end = carrier.find("+", start+1);
            if(end == std::string::npos)
                end = carrier.length();
            string dummy = carrier.substr(start+4, end-(start+4));
            if(dummy.length())
                tc = atoi(dummy.c_str());
        }

        /*
        string stimulation;
        size_t start = carrier.find("+(ex");
        if(start != std::string::npos)
            stimulation = carrier.substr(0, start);
        else
            stimulation = carrier;

        printf("%s\n", getCarrierOptions(stimulation.c_str()));
        Property options;
        options.fromString(getCarrierOptions(stimulation.c_str())); 
        //double st = (options.check("st",Value(10)).asInt())/10.0;
        double st = options.find("st").asInt()/10.0;
        int tc = (int) options.check("tc",Value(1)).asInt();
        printf("st:%.2f, tc:%d\n", st, tc);
        */

        char dummy[128]; 
        row[m_Columns.m_col_color_value] = Gdk::Color("#888888");
        row[m_Columns.m_col_editable] = false;
        
        childrow = *(m_refTreeModel->append(row.children()));
        childrow[m_Columns.m_col_name] = "Protocol";
        childrow[m_Columns.m_col_value] = protocol.c_str();
        childrow[m_Columns.m_col_editable] = true;

        childrow = *(m_refTreeModel->append(row.children()));
        childrow[m_Columns.m_col_name] = "Stimulation";
        sprintf(dummy, "%.2lf", st);
        childrow[m_Columns.m_col_value] = dummy;
        childrow[m_Columns.m_col_editable] = true;

        childrow = *(m_refTreeModel->append(row.children()));
        childrow[m_Columns.m_col_name] = "Decay time";        
        sprintf(dummy, "%d", tc);
        childrow[m_Columns.m_col_value] = dummy;
        childrow[m_Columns.m_col_editable] = true;

        childrow = *(m_refTreeModel->append(row.children()));
        childrow[m_Columns.m_col_name] = "Auxilary";        
        childrow[m_Columns.m_col_value] = (aux) ? "yes" : "no";
        childrow[m_Columns.m_col_editable] = true;

        childrow = *(m_refTreeModel->append(row.children()));
        childrow[m_Columns.m_col_name] = "Debug mode";
        childrow[m_Columns.m_col_value] = (dbg) ? "yes" : "no";
        childrow[m_Columns.m_col_editable] = true;       
    }
    else
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
            string strOldFrom = m_pConnection->from();
            // updating source port name in model
            Glib::RefPtr<ExternalPortModel> port = Glib::RefPtr<ExternalPortModel>::cast_dynamic(m_pArrow->getSource());
            if(port)
                port->setPort(new_text.c_str());

            Glib::RefPtr<PortArbitratorModel> arbPort = Glib::RefPtr<PortArbitratorModel>::cast_dynamic(m_pArrow->getDestination());
            if(arbPort)
                arbPort->updateExcitation(m_pArrow.operator->(), strOldFrom.c_str(), new_text.c_str());
        }
        else
        if(strName == "To")
        {
            // updating source port name in model
            Glib::RefPtr<ExternalPortModel> port = Glib::RefPtr<ExternalPortModel>::cast_dynamic(m_pArrow->getDestination());
            if(port)
                port->setPort(new_text.c_str());
            Glib::RefPtr<PortArbitratorModel> arbPort = Glib::RefPtr<PortArbitratorModel>::cast_dynamic(m_pArrow->getDestination());
            if(arbPort)
                arbPort->setPort(new_text.c_str());
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
        else
        {
            // create carrier params
            string carrier = Glib::ustring(carrierRow[m_Columns.m_col_value]).c_str(); 

            string exictation;
            // extract bias
            size_t start = carrier.find("+bs");
            if(start != std::string::npos)
            {
                size_t end = carrier.find("+", start+1);
                if(end == std::string::npos)
                    end = carrier.length();
                exictation = carrier.substr(start, end-start);
            }

            //extract exictatory
            start = carrier.find("+(ex");
            if(start != std::string::npos)
            {
                size_t end = carrier.find("+", start+1);
                if(end == std::string::npos)
                    end = carrier.length();
                exictation += carrier.substr(start, end-start);
            }

            //printf("exec: %s\n", exictation.c_str());
    
            typedef Gtk::TreeModel::Children type_children;
            type_children children = carrierRow->children();
            Glib::ustring param; 
            for(type_children::iterator iter = children.begin(); iter != children.end(); ++iter)
            {
                Gtk::TreeModel::Row row = *iter;
                if(row[m_Columns.m_col_name] == "Protocol")
                    param = row[m_Columns.m_col_value] + "+recv.priority";
                else if(row[m_Columns.m_col_name] == "Stimulation")
                {
                    // scaling up by 10 
                    string strSt = Glib::ustring(row[m_Columns.m_col_value]).c_str();
                    double st = atof(strSt.c_str());
                    char dummy[64];
                    sprintf(dummy, "%d", (int)(st*10));
                    param += string("+st.") + string(dummy);
                }                    
                else if(row[m_Columns.m_col_name] == "Decay time")
                {
                    // removing dot e.g. 2.0 -> 2
                    string strTc = Glib::ustring(row[m_Columns.m_col_value]).c_str();
                    int tc = atoi(strTc.c_str());
                    char dummy[64];
                    sprintf(dummy, "%d", tc);
                    param += string("+tc.") + string(dummy);
                }                    
                else if((row[m_Columns.m_col_name] == "Auxilary") 
                        && compareString(Glib::ustring(row[m_Columns.m_col_value]).c_str(), "yes" ) )
                    param += "+virtual";
                else if((row[m_Columns.m_col_name] == "Debug mode") 
                        && compareString(Glib::ustring(row[m_Columns.m_col_value]).c_str(), "yes" ) )
                    param += "+debug";
            }
            param += exictation;
            //printf("param: %s\n", param.c_str());
            carrierRow[m_Columns.m_col_value] = param;

            Connection con = *m_pConnection;
            con.setCarrier(param.c_str());
            m_pManager->getKnowledgeBase()->updateConnectionOfApplication(application, *m_pConnection, con);
            m_pConnection->setCarrier(param.c_str());
            m_pArrow->setLabel(param.c_str());
       }
   }
}

// copied from yarp Protocol.cpp
const char* ConnectionPropertyWindow::getCarrierOptions(const char* opt) 
{
    string carrier = opt;
    string from;
    unsigned start = carrier.find("+");
    if(carrier.find("+") == std::string::npos)
        return from.c_str();
         //from = " (protocol " + carrier + ") ";
    //{
        //from = " (protocol \"" + carrier.substr(0, start) + "\")";
    from += " (";
    for ( unsigned i=start+1; 
         i<(unsigned)carrier.length(); 
         i++) {
        char ch = carrier[i];
        if (ch=='+') {
            from += ") (";
        } else if (ch=='.') {
            from += " ";
        } else {
            from += ch;
        }
    }
    from += ")";
    //}
    return from.c_str();
}

