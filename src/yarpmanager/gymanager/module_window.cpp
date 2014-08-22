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

#include "module_window.h"
#include "icon_res.h"
#include "main_window.h"

#include <yarp/manager/ymm-dir.h>

using namespace yarp::manager;

ModuleWindow::ModuleWindow(Module* module, MainWindow* parent,
                               Manager* manager)
{
    m_pModule = module;
    m_pParent = parent;
    m_pManager = manager;

    /* Create a new scrolled window, with scrollbars only if needed */
    set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    add(m_TreeView);

    /* create tree store */
    m_refTreeModel = Gtk::TreeStore::create(m_Columns);
    m_TreeView.set_model(m_refTreeModel);

    //Add the Model’s column to the View’s columns:
    Gtk::CellRendererText* cellText = Gtk::manage(new Gtk::CellRendererText());
    Gtk::CellRendererPixbuf* cellPix = Gtk::manage(new Gtk::CellRendererPixbuf());
    Gtk::TreeViewColumn* col = Gtk::manage(new Gtk::TreeViewColumn("Item"));
    col->pack_start(*cellPix, false);
    col->pack_start(*cellText, true);
    col->add_attribute(*cellText, "text", 1);
    col->add_attribute(*cellPix, "pixbuf", 0);
    m_TreeView.append_column(*col);
    m_TreeView.get_column(0)->set_resizable(true);

    m_TreeView.append_column("Value", m_Columns.m_col_value);
    m_TreeView.get_column(1)->set_resizable(true);

    updateWidget();
    show_all_children();
}


ModuleWindow::~ModuleWindow()
{
}

void ModuleWindow::onRefresh()
{
    updateWidget();
    m_pParent->reportErrors();
}

void ModuleWindow::onTabCloseRequest()
{
    m_pParent->onTabCloseRequest(this);
}

void ModuleWindow::updateWidget()
{
    Module* m_pModule = ModuleWindow::m_pModule;
    m_refTreeModel->clear();

    Gtk::TreeModel::Row row;
    Gtk::TreeModel::Row childrow;
    Gtk::TreeModel::Row cchildrow;

    row = *(m_refTreeModel->append());
    row[m_Columns.m_col_name] = "Name";
    row[m_Columns.m_col_value] = m_pModule->getName();

    row = *(m_refTreeModel->append());
    row[m_Columns.m_col_name] = "Version";
    row[m_Columns.m_col_value] = m_pModule->getVersion();

    row = *(m_refTreeModel->append());
    row[m_Columns.m_col_name] = "Description";
    row[m_Columns.m_col_value] = m_pModule->getDescription();

    row = *(m_refTreeModel->append());
    row[m_Columns.m_col_name] = "Parameters";
    row[m_Columns.m_col_value] = "";
    for(int i=0; i<m_pModule->argumentCount(); i++)
    {
        childrow = *(m_refTreeModel->append(row.children()));
        childrow[m_Columns.m_col_name] = m_pModule->getArgumentAt(i).getParam();
        childrow[m_Columns.m_col_value] = m_pModule->getArgumentAt(i).getDescription();
    }

    row = *(m_refTreeModel->append());
    row[m_Columns.m_col_name] = "Authors";
    row[m_Columns.m_col_value] = "";
    for(int i=0; i<m_pModule->authorCount(); i++)
    {
        childrow = *(m_refTreeModel->append(row.children()));
        childrow[m_Columns.m_col_name] = m_pModule->getAuthorAt(i).getName();
        childrow[m_Columns.m_col_value] = m_pModule->getAuthorAt(i).getEmail();
    }

    row = *(m_refTreeModel->append());
    row[m_Columns.m_col_name] = "Inputs";
    row[m_Columns.m_col_value] = "";
    for(int i=0; i<m_pModule->inputCount(); i++)
    {
        InputData in = m_pModule->getInputAt(i);

        childrow = *(m_refTreeModel->append(row.children()));
        childrow[m_Columns.m_col_name] = "Type";
        childrow[m_Columns.m_col_value] = in.getName();

        cchildrow = *(m_refTreeModel->append(childrow.children()));
        cchildrow[m_Columns.m_col_name] = "Port";
        cchildrow[m_Columns.m_col_value] = in.getPort();

        cchildrow = *(m_refTreeModel->append(childrow.children()));
        cchildrow[m_Columns.m_col_name] = "Description";
        cchildrow[m_Columns.m_col_value] = in.getDescription();

        childrow = *(m_refTreeModel->append(childrow.children()));
        childrow[m_Columns.m_col_name] = "Required";
        childrow[m_Columns.m_col_value] = (in.isRequired())?"Yes":"No";
    }

    row = *(m_refTreeModel->append());
    row[m_Columns.m_col_name] = "Outputs";
    row[m_Columns.m_col_value] = "";
    for(int i=0; i<m_pModule->outputCount(); i++)
    {
        OutputData out = m_pModule->getOutputAt(i);

        childrow = *(m_refTreeModel->append(row.children()));
        childrow[m_Columns.m_col_name] = "Type";
        childrow[m_Columns.m_col_value] = out.getName();

        cchildrow = *(m_refTreeModel->append(childrow.children()));
        cchildrow[m_Columns.m_col_name] = "Port";
        cchildrow[m_Columns.m_col_value] = out.getPort();

        cchildrow = *(m_refTreeModel->append(childrow.children()));
        cchildrow[m_Columns.m_col_name] = "Description";
        cchildrow[m_Columns.m_col_value] = out.getDescription();
    }

}


