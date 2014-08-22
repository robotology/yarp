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

#include "module_property_window.h"
#include "icon_res.h"
#include "main_window.h"
#include "arrow_model.h"
#include "application_window.h"
#include <yarp/manager/ymm-dir.h>
using namespace yarp::manager;

ModulePropertyWindow::ModulePropertyWindow(MainWindow* parent,
                               Manager* manager, ApplicationWindow* appWnd) : m_pModule(NULL)
{
    m_pParent = parent;
    m_pManager = manager;
    m_pModule = NULL;
    m_pAppWindow = appWnd;


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
    itemCol->add_attribute(*itemRenderer, "foreground-gdk", m_Columns.m_col_color_item);
    itemCol->add_attribute(*itemRenderer, "text", m_Columns.m_col_name);
    itemCol->set_resizable(true);
    m_TreeView.append_column(*itemCol);


    Gtk::CellRendererCombo* valueRenderer = Gtk::manage(new Gtk::CellRendererCombo());
    Gtk::TreeView::Column* valueCol = Gtk::manage(new Gtk::TreeView::Column("Value"));
    valueCol->pack_start(*valueRenderer);
    valueCol->add_attribute(*valueRenderer, "foreground-gdk", m_Columns.m_col_color_value);
    //valueCol->set_resizable(true);
    valueCol->add_attribute(*valueRenderer, "editable", m_Columns.m_col_editable);

  //Make this View column represent the m_col_itemchosen model column:
#ifdef GLIBMM_PROPERTIES_ENABLED
  valueCol->add_attribute(valueRenderer->property_text(), m_Columns.m_col_value);
#else
  pColumn->add_attribute(*valueRenderer, "text", m_Columns.m_col_value);
#endif

#ifdef GLIBMM_PROPERTIES_ENABLED
    valueCol->add_attribute(valueRenderer->property_model(), m_Columns.m_col_choices);
#else
    valueCol->add_attribute(*valueRenderer, "model", m_Columns.m_col_choices);
#endif

#ifdef GLIBMM_PROPERTIES_ENABLED
    valueRenderer->property_text_column() = 0;
#else
    valueRenderer->set_property("text_column", 0);
#endif
    valueRenderer->signal_edited().connect( sigc::mem_fun(*this,
              &ModulePropertyWindow::onCellEdited) );
    m_TreeView.append_column(*valueCol);

    m_TreeView.set_grid_lines(Gtk::TREE_VIEW_GRID_LINES_BOTH);
    m_TreeView.set_rules_hint(true);

    show_all_children();
}


ModulePropertyWindow::~ModulePropertyWindow()
{
}

void ModulePropertyWindow::onRefresh()
{
    if(!m_pModule)
        return;
    update(m_pModule);
    m_pParent->reportErrors();
}

void ModulePropertyWindow::onTabCloseRequest()
{
    m_pParent->onTabCloseRequest(this);
}

void ModulePropertyWindow::update(Module* module)
{
    m_pModule = module;
    m_refTreeModel->clear();
    m_refModelCombos.clear();

    Gtk::TreeModel::Row row;
    Gtk::TreeModel::Row childrow;
//    Gtk::TreeModel::Row cchildrow;

    Glib::RefPtr<Gtk::ListStore> m_refCombo = Gtk::ListStore::create(m_ColumnsCombo);
    //row = *(m_refCombo->append());
    //row[m_ColumnsCombo.m_col_choice] = "---";
    m_refModelCombos.push_back(m_refCombo);


    row = *(m_refTreeModel->append());
    row[m_Columns.m_col_name] = "Name";
    row[m_Columns.m_col_value] = m_pModule->getName();
    row[m_Columns.m_col_color_value] = Gdk::Color("#888888");
    row[m_Columns.m_col_editable] = false;
    row[m_Columns.m_col_choices] = m_refModelCombos.back();

    /*
    row = *(m_refTreeModel->append());
    row[m_Columns.m_col_name] = "Description";
    row[m_Columns.m_col_value] = m_pModule->getDescription();
    row[m_Columns.m_col_color_value] = Gdk::Color("#888888");
    row[m_Columns.m_col_editable] = false;
    row[m_Columns.m_col_choices] = m_refModelCombos.back();
    */

    //node
    m_refCombo = Gtk::ListStore::create(m_ColumnsCombo);
    row = *(m_refCombo->append());
    row[m_ColumnsCombo.m_col_choice] = "localhost";
    ResourcePContainer resources = m_pManager->getKnowledgeBase()->getResources();
    for(ResourcePIterator itr=resources.begin(); itr!=resources.end(); itr++)
    {
        Computer* comp = dynamic_cast<Computer*>(*itr);
        if(comp && !compareString(comp->getName(), "localhost"))
        {
            row = *(m_refCombo->append());
            row[m_ColumnsCombo.m_col_choice] = comp->getName();
        }
    }

    m_refModelCombos.push_back(m_refCombo);

    row = *(m_refTreeModel->append());
    row[m_Columns.m_col_name] = "Node";
    row[m_Columns.m_col_value] = m_pModule->getHost();
    row[m_Columns.m_col_editable] = true;
    row[m_Columns.m_col_choices] = m_refModelCombos.back();

    row = *(m_refTreeModel->append());
    row[m_Columns.m_col_name] = "Stdio";
    row[m_Columns.m_col_value] = m_pModule->getStdio();
    row[m_Columns.m_col_editable] = true;
    row[m_Columns.m_col_choices] = m_refModelCombos.back();

    m_refCombo = Gtk::ListStore::create(m_ColumnsCombo);
    m_refModelCombos.push_back(m_refCombo);

    row = *(m_refTreeModel->append());
    row[m_Columns.m_col_name] = "Workdir";
    row[m_Columns.m_col_value] = m_pModule->getWorkDir();
    row[m_Columns.m_col_editable] = true;
    row[m_Columns.m_col_choices] = m_refModelCombos.back();

    row = *(m_refTreeModel->append());
    row[m_Columns.m_col_name] = "Prefix";
    row[m_Columns.m_col_value] = m_pModule->getBasePrefix();
    row[m_Columns.m_col_editable] = true;
    row[m_Columns.m_col_choices] = m_refModelCombos.back();

    //Deployer
    m_refCombo = Gtk::ListStore::create(m_ColumnsCombo);
    if(compareString(m_pModule->getBroker(), "yarpdev"))
    {
        row = *(m_refCombo->append());
        row[m_ColumnsCombo.m_col_choice] = "yarpdev";
    }
    else if(compareString(m_pModule->getBroker(), "icubmoddev"))
    {
        row = *(m_refCombo->append());
        row[m_ColumnsCombo.m_col_choice] = "icubmoddev";
    }
    else
    {
        row = *(m_refCombo->append());
        row[m_ColumnsCombo.m_col_choice] = "local";
        row = *(m_refCombo->append());
        row[m_ColumnsCombo.m_col_choice] = "yarprun";
    }
    m_refModelCombos.push_back(m_refCombo);

    row = *(m_refTreeModel->append());
    row[m_Columns.m_col_name] = "Deployer";
    if(strlen(m_pModule->getBroker()))
        row[m_Columns.m_col_value] = m_pModule->getBroker();
    else if(compareString(m_pModule->getHost(), "localhost"))
        row[m_Columns.m_col_value] = "local";

    if(m_pModule->getNeedDeployer())
    {
        row[m_Columns.m_col_editable] = false;
        row[m_Columns.m_col_color_value] = Gdk::Color("#888888");
    }
    else
        row[m_Columns.m_col_editable] = true;
    row[m_Columns.m_col_choices] = m_refModelCombos.back();

    row = *(m_refTreeModel->append());
    row[m_Columns.m_col_name] = "Parameters";
    row[m_Columns.m_col_value] = m_pModule->getParam();
    row[m_Columns.m_col_color_value] = Gdk::Color("#888888");
    row[m_Columns.m_col_editable] = false;
    row[m_Columns.m_col_choices] = m_refModelCombos.back();

    for(int i=0; i<m_pModule->argumentCount(); i++)
    {
        Gtk::TreeModel::Row comboRow;
        m_refCombo = Gtk::ListStore::create(m_ColumnsCombo);
        comboRow = *(m_refCombo->append());
        if(!m_pModule->getArgumentAt(i).isSwitch())
            comboRow[m_ColumnsCombo.m_col_choice] = m_pModule->getArgumentAt(i).getDefault();
        else
        {
            comboRow[m_ColumnsCombo.m_col_choice] = "on";
            comboRow = *(m_refCombo->append());
            comboRow[m_ColumnsCombo.m_col_choice] = "off";
        }
        m_refModelCombos.push_back(m_refCombo);

        childrow = *(m_refTreeModel->append(row.children()));
        childrow[m_Columns.m_col_name] = m_pModule->getArgumentAt(i).getParam();
        childrow[m_Columns.m_col_value] = m_pModule->getArgumentAt(i).getValue();
        Glib::ustring strval = childrow[m_Columns.m_col_value];
        if(m_pModule->getArgumentAt(i).isRequired()
            && !strval.size())
            childrow[m_Columns.m_col_color_item] = Gdk::Color("#BF0303");
        childrow[m_Columns.m_col_editable] = true;
        childrow[m_Columns.m_col_choices] = m_refModelCombos.back();
    }

    //updateParamteres();

    m_TreeView.expand_all();

}


bool ModulePropertyWindow::getRowByName(const char* name, Gtk::TreeModel::Row* row)
{
    typedef Gtk::TreeModel::Children type_children;
    type_children children = m_refTreeModel->children();
    for(type_children::iterator iter = children.begin(); iter!=children.end(); ++iter)
    {
        Glib::ustring strName = (*iter)[m_Columns.m_col_name];
        if( strName == name)
        {
            *row = (*iter);
            return true;
        }
    }
    return false;
}

/*
Connection* ModulePropertyWindow::findConnection( CnnContainer& connections, const char* szPort, bool from)
{
    CnnIterator jtr;
    for(jtr=connections.begin(); jtr!=connections.end(); jtr++)
    {
        if(from && (string((*jtr).from()) == string(szPort)))
            return &(*jtr);
        if(!from && (string((*jtr).to()) == string(szPort)))
            return &(*jtr);
    }

    return NULL;
}
*/

void ModulePropertyWindow::updateModule(const char* item, const char* value)
{
    if(!m_pModule)
        return;

    // updating modules
    if(strcmp(item, "Node") == 0)
    {
        m_pModule->setHost(value);
    }
    else if(strcmp(item,"Stdio") == 0)
    {
        m_pModule->setStdio(value);

    }
    else if(strcmp(item, "Workdir") == 0)
    {
        m_pModule->setWorkDir(value);

    }
    else if(strcmp(item, "Prefix") == 0)
    {
        m_pModule->setBasePrefix(value);
        string strPrefix;
        Application* application = m_pManager->getKnowledgeBase()->getApplication();
        if(application)
        {
            strPrefix = string(application->getPrefix()) + string(value);
            for(int j=0; j<m_pModule->outputCount(); j++)
            {
                OutputData *output = &m_pModule->getOutputAt(j);

                for(int i=0; i<application->connectionCount(); i++)
                {
                    Connection con = application->getConnectionAt(i);
                    string strOldFrom = con.from();
                    Connection updatedCon = con;
                    if(con.getCorOutputData())
                    {
                        if(con.getCorOutputData() == output)
                        {
                            string strFrom = strPrefix + string(output->getPort());
                            updatedCon.setFrom(strFrom.c_str());
                            m_pManager->getKnowledgeBase()->updateConnectionOfApplication(application,
                                                        con, updatedCon);
                            // updating arrow's connection
                            ArrowModel* arrow = dynamic_cast<ArrowModel*>(con.getModel());
                            if(arrow)
                            {
                                arrow->setConnection(updatedCon);
                                // updating excitatory links from other connections
                                Glib::RefPtr<PortArbitratorModel> arbPort = Glib::RefPtr<PortArbitratorModel>::cast_dynamic(arrow->getDestination());
                                if(arbPort)
                                    arbPort->updateExcitation(arrow, strOldFrom.c_str(), strFrom.c_str());
                            }
                        }
                    }
                }
            }

            for(int j=0; j<m_pModule->inputCount(); j++)
            {
                InputData *input = &m_pModule->getInputAt(j);

                for(int i=0; i<application->connectionCount(); i++)
                {
                    Connection con = application->getConnectionAt(i);
                    Connection updatedCon = con;
                    if(con.getCorInputData())
                    {
                        if(con.getCorInputData() == input)
                        {
                            string strTo = strPrefix + string(input->getPort());
                            updatedCon.setTo(strTo.c_str());
                            m_pManager->getKnowledgeBase()->updateConnectionOfApplication(application,
                                                        con, updatedCon);
                            // updating arrow's connection
                            if(dynamic_cast<ArrowModel*>(con.getModel()))
                                dynamic_cast<ArrowModel*>(con.getModel())->setConnection(updatedCon);
                        }
                    }
                }
            }
            // updating module prefix.
            m_pManager->getKnowledgeBase()->setModulePrefix(m_pModule, strPrefix.c_str(), false);
        }
    }
    else if(strcmp(item, "Parameters") == 0)
    {
        m_pModule->setParam(value);
    }

    else if(strcmp(item, "Deployer") == 0)
    {
        m_pModule->setBroker(value);
    }
}

void ModulePropertyWindow::onCellEdited(const Glib::ustring& path_string,
                    const Glib::ustring& new_text)
{
    if(!m_pModule)
        return;

    if(m_pAppWindow)
        m_pAppWindow->setModified();

    ErrorLogger* logger = ErrorLogger::Instance();
    Gtk::TreePath path(path_string);

    //Get the row from the path:
    Gtk::TreeModel::iterator iter = m_refTreeModel->get_iter(path);
    if(iter)
    {
        Gtk::TreeModel::Row row = *iter;
        //Put the new value in the model:
        Glib::ustring strName = Glib::ustring(row[m_Columns.m_col_name]);

        if(strName == "Deployer")
        {
            if(strlen(m_pModule->getHost()) &&
               !compareString(m_pModule->getHost(), "localhost") &&
               (new_text == "local"))
            {
                logger->addWarning("local deployer cannot be used to deploy a module on the remote host.");
                m_pParent->reportErrors();
                return;
            }
        }

        if(strName == "Node")
        {
            if(compareString(m_pModule->getBroker(), "local") &&
               new_text.size() && (new_text != "localhost"))
            {
                OSTRINGSTREAM msg;
                msg<<new_text.c_str()<<" cannot be used with local deployer!";
                logger->addWarning(msg);
                m_pParent->reportErrors();

                Gtk::TreeModel::Row row;
                if(getRowByName("Deployer", &row))
                {
                    row[m_Columns.m_col_value] = "";
                    updateModule("Deployer", "");
                }
            }
        }

        row[m_Columns.m_col_value] = new_text;
        updateParamteres();
        updateModule(strName.c_str(), new_text.c_str());

   }
}

void ModulePropertyWindow::updateParamteres()
{
    // updating parameters
    typedef Gtk::TreeModel::Children type_children;
    type_children children = m_refTreeModel->children();
    OSTRINGSTREAM strParams;
    Glib::ustring strName;
    for(type_children::iterator iter = children.begin(); iter!=children.end(); ++iter)
    {
        strName = Glib::ustring((*iter)[m_Columns.m_col_name]);
        if(strName == "Parameters")
        {
            for(type_children::iterator jter = (*iter).children().begin();
                jter!=(*iter).children().end(); ++jter)
            {
                Glib::ustring strItem = Glib::ustring((*jter)[m_Columns.m_col_name]);
                Glib::ustring strValue = Glib::ustring((*jter)[m_Columns.m_col_value]);
                for(int i=0; i<m_pModule->argumentCount(); i++)
                {
                    if(strItem == m_pModule->getArgumentAt(i).getParam())
                    {
                        if(strValue.size())
                        {
                            if(!m_pModule->getArgumentAt(i).isSwitch())
                                strParams<<"--"<<strItem<<" "<<strValue<<" ";
                            else
                            {
                                if(compareString(strValue.c_str(), "on" ))
                                    strParams<<"--"<<strItem<<" ";
                                else
                                    (*jter)[m_Columns.m_col_value] = "off";
                            }
                        }
                        else
                            if(m_pModule->getArgumentAt(i).isSwitch())
                                (*jter)[m_Columns.m_col_value] = "off";
                    }

                    if((strItem == m_pModule->getArgumentAt(i).getParam()) &&
                        m_pModule->getArgumentAt(i).isRequired())
                    {
                        if(!strValue.size())
                            (*jter)[m_Columns.m_col_color_item] = Gdk::Color("#BF0303");
                        else
                            (*jter)[m_Columns.m_col_color_item] = Gdk::Color("#000000");
                    }
                }
            }
            (*iter)[m_Columns.m_col_value] = strParams.str();
            updateModule(strName.c_str(), strParams.str().c_str());
            break;
        }
    }
}
