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
    #pragma warning (disable : 4099)
#endif


#include "arb_property_window.h"
#include "icon_res.h"
#include "main_window.h"
#include "ext_port_model.h"
#include "application_window.h"

#include <yarp/manager/ymm-dir.h>
#include <yarp/manager/binexparser.h>

//#include <yarp/os/Property.h>
//#include <yarp/os/Bottle.h>

using namespace yarp::os;
using namespace yarp::manager;

ArbitratorPropertyWindow::ArbitratorPropertyWindow(MainWindow* parent, 
                               Manager* manager, ApplicationWindow* appWnd)
{   
    m_pParent = parent;
    m_pManager = manager;
    m_pAppWindow = appWnd;
    m_arbPort.clear();

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
    //    &ArbitratorPropertyWindow::onCellData) );
    valueRenderer->signal_edited().connect( sigc::mem_fun(*this,
              &ArbitratorPropertyWindow::onCellEdited) );
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


ArbitratorPropertyWindow::~ArbitratorPropertyWindow()
{
}

void ArbitratorPropertyWindow::onRefresh()
{
    if(!m_arbPort)
        return;
    update(m_arbPort);
    m_pParent->reportErrors();
}

void ArbitratorPropertyWindow::onTabCloseRequest() 
{ 
    m_pParent->onTabCloseRequest(this); 
}

void ArbitratorPropertyWindow::release(void)
{
    m_arbPort.clear();
}

void ArbitratorPropertyWindow::update(Glib::RefPtr<PortArbitratorModel> &arbPort)
{
    m_arbPort = arbPort;
    Arbitrator& arbitrator = m_arbPort->getArbitrator();
    m_refTreeModel->clear();

    Gtk::TreeModel::Row row;
    Gtk::TreeModel::Row childrow;
    
    row = *(m_refTreeModel->append());
    row[m_Columns.m_col_name] = "Port";
    row[m_Columns.m_col_value] = arbitrator.getPort();
    row[m_Columns.m_col_color_value] = Gdk::Color("#888888");
    row[m_Columns.m_col_editable] = false;

    row = *(m_refTreeModel->append());
    rulesRow = row;
    row[m_Columns.m_col_name] = "Rules";
    row[m_Columns.m_col_color_value] = Gdk::Color("#888888");
    row[m_Columns.m_col_editable] = false;

    std::map<string, string> &rules = arbitrator.getRuleMap();
    std::map<string, string>::iterator itr;
    for(itr=rules.begin(); itr!=rules.end(); itr++)
    {
        childrow = *(m_refTreeModel->append(row.children()));
        childrow[m_Columns.m_col_name] = (itr->first).c_str();
        childrow[m_Columns.m_col_value] = (itr->second).c_str();
        childrow[m_Columns.m_col_editable] = true;
        if(!arbitrator.trainWeights((itr->first).c_str()))
            childrow[m_Columns.m_col_color_value] = Gdk::Color("#FF0000");
        else
            childrow[m_Columns.m_col_color_value] = Gdk::Color("#000000");
    }

    m_arbPort->setErrorMode(!arbitrator.validate());

    m_TreeView.expand_all();
    m_pParent->reportErrors();
}

void ArbitratorPropertyWindow::onCellEdited(const Glib::ustring& path_string, 
                    const Glib::ustring& new_text)
{
    if(!m_arbPort)
        return;

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
        row[m_Columns.m_col_color_value] = Gdk::Color("#000000");

        Arbitrator& arbitrator = m_arbPort->getArbitrator();

        string strRule = new_text.c_str();

        Application* application = m_pManager->getKnowledgeBase()->getApplication();
        ArrowModel* curArrow = m_arbPort->getArrowModel(strName.c_str());
        Connection curCon = *curArrow->getConnection();

        strRule.erase(std::remove_if(strRule.begin(), strRule.end(), ::isspace), strRule.end());
        if(!strRule.size())
        {
            arbitrator.addRule(strName.c_str(), "");
            string carrier = curCon.carrier();
            // setting bias = 1.0
            size_t start = carrier.find("+bs");
            if(start != std::string::npos)
            {
                size_t end = carrier.find("+", start+1);
                if(end == std::string::npos)
                    end = carrier.length();
                carrier.erase(start, end-start);
            }
            carrier += "+bs.10";
            // erase excitatory links to this connection
            std::map<string, string> &rules = arbitrator.getRuleMap();
            std::map<string, string>::iterator itr;
            for(itr=rules.begin(); itr!=rules.end(); itr++)
            {
               ArrowModel* arw = m_arbPort->getArrowModel((itr->first).c_str());
               if(arw && (arw != curArrow))
                   setExcitation(arw, curCon.from(), 0);
            }

            // updating carrier
            curCon.setCarrier(carrier.c_str());
            m_pManager->getKnowledgeBase()->updateConnectionOfApplication(application, *curArrow->getConnection(), curCon);
            curArrow->getConnection()->setCarrier(carrier.c_str());
            curArrow->setLabel(carrier.c_str());
        }          
        else
        {
            string oldRule = arbitrator.getRule(strName.c_str()); 
            arbitrator.addRule(strName.c_str(), new_text.c_str());
            if(arbitrator.trainWeights(strName.c_str()))
            {
                std::map<std::string, double>& alphas = arbitrator.getAlphas(strName.c_str());
                double bias = arbitrator.getBias(strName.c_str());

                string carrier = curCon.carrier();                    
                // clearing bias
                size_t start = carrier.find("+bs");
                if(start != std::string::npos)
                {
                    size_t end = carrier.find("+", start+1);
                    if(end == std::string::npos)
                        end = carrier.length();
                    carrier.erase(start, end-start);
                }
                // setting bias
                char dummy[64];
                sprintf(dummy, "+bs.%d", (int)(bias*10));
                carrier += dummy;

                // erase excitatory links to this connection
                std::map<string, string> &rules = arbitrator.getRuleMap();
                std::map<string, string>::iterator jtr;
                for(jtr=rules.begin(); jtr!=rules.end(); jtr++)
                {
                   ArrowModel* arw = m_arbPort->getArrowModel((jtr->first).c_str());
                   if(arw && (arw != curArrow))
                       setExcitation(arw, curCon.from(), 0);
                }

                std::map<std::string, double>::iterator itr;
                for(itr=alphas.begin(); itr!=alphas.end(); itr++)
                {
                    ArrowModel* arw = m_arbPort->getArrowModel((itr->first).c_str());
                    if(arw && (arw != curArrow))
                    {
                        //printf("excitation : %s to %s , value:%d\n", arw->getConnection()->from(), curCon.from(), (int)(trainer.getAlphas()[i]*10));
                        setExcitation(arw, curCon.from(), (int)(itr->second*10));
                    }                           
                }
        
                // updating carrier
                curCon.setCarrier(carrier.c_str());
                m_pManager->getKnowledgeBase()->updateConnectionOfApplication(application, *curArrow->getConnection(), curCon);
                curArrow->getConnection()->setCarrier(carrier.c_str());
                curArrow->setLabel(carrier.c_str());

            }
            else
            {
                arbitrator.addRule(strName.c_str(), oldRule.c_str());
                row[m_Columns.m_col_color_value] = Gdk::Color("#FF0000");
            }

        } 
        m_arbPort->setArbitrator(arbitrator);
        m_arbPort->setErrorMode(!arbitrator.validate());
        m_pParent->reportErrors();

        /*
        typedef Gtk::TreeModel::Children type_children;
        type_children children = rulesRow->children();
        Glib::ustring param; 
        m_arbPort->setErrorMode(false);
        for(type_children::iterator iter = children.begin(); iter != children.end(); ++iter)
        {
            Gtk::TreeModel::Row row = *iter;
            if(row[m_Columns.m_col_color_value] == Gdk::Color("#FF0000"))
            {
                m_arbPort->setErrorMode(true);
                break;
            }
        } 
        */
   }
}


void ArbitratorPropertyWindow::setExcitation(ArrowModel* arrow, const char* szLink, int value)
{
    Connection con = *arrow->getConnection();
    string carrier = con.carrier();    

    //printf("Connection: %s\n",arrow->getId());
    string excitation;
    size_t start = carrier.find("+(ex");
    if(start != std::string::npos)
    {
        size_t end = carrier.find("+", start+1);
        if(end == std::string::npos)
            end = carrier.length();
        excitation = carrier.substr(start+1, end-(start+1));
        carrier.erase(start, end-start);        
    }

    //printf("\tvalue:%d, excitation: %s\n", value, excitation.c_str());
    Property options;
    options.fromString(excitation.c_str()); 
    Bottle exc = options.findGroup("ex");
    string strLink = "+(ex";
    bool bEmpty = true;
    for(int i=0; i<exc.size(); i++)
    {
        Value v = exc.get(i);
        if(v.isList() && (v.asList()->size()>=2))
        {
            Bottle* b = v.asList();
            if(!compareString(b->get(0).asString().c_str(), szLink))
            {
                strLink += string(" (") + b->get(0).asString().c_str();
                char dummy[64];
                sprintf(dummy, " %d)", (int)b->get(1).asDouble());
                strLink += dummy; 
                bEmpty = false;
            }
        }
    }

    if(value != 0)
    {
        strLink += string(" (") + szLink;
        char dummy[64];
        sprintf(dummy, " %d)", value);
        strLink += dummy;
        bEmpty = false;
    }
    strLink += ")";

    if(!bEmpty)    
        carrier += strLink;

    // updating carrier
    con.setCarrier(carrier.c_str());
    Application* application = m_pManager->getKnowledgeBase()->getApplication();
    m_pManager->getKnowledgeBase()->updateConnectionOfApplication(application, *arrow->getConnection(), con);
    arrow->getConnection()->setCarrier(carrier.c_str());
    arrow->setLabel(carrier.c_str());
}


