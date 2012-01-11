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



#include <sstream>
#include <string>
#include <iostream>

#include "application_list.h"
#include "icon_res.h"
#include "ymm-dir.h"

using namespace std;


ApplicationList::ApplicationList()
{

    /* Create a new scrolled window, with scrollbars only if needed */
    set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    add(m_TreeView);

    /* create list store */
    m_refTreeModel = Gtk::TreeStore::create(m_appColumns);
    m_TreeView.set_model(m_refTreeModel);


    //Add the Model’s column to the View’s columns: 
    Gtk::CellRendererText* cellText = Gtk::manage(new Gtk::CellRendererText());
    Gtk::CellRendererPixbuf* cellPix = Gtk::manage(new Gtk::CellRendererPixbuf()); 
    Gtk::TreeViewColumn* col = Gtk::manage(new Gtk::TreeViewColumn("Entities"));
    col->pack_start(*cellPix, false);
    col->pack_start(*cellText, true);
    col->add_attribute(*cellText, "text", 1);
    col->add_attribute(*cellPix, "pixbuf", 0);
    m_TreeView.append_column(*col);
    //m_TreeView.expand_all();


//  m_modRow = *(m_refTreeModel->append());
//  m_modRow[m_appColumns.m_col_name] = "Module";
//  m_modRow[m_appColumns.m_col_refPix] = Gtk::IconTheme::get_default()->load_icon("folder", Gtk::ICON_LOOKUP_USE_BUILTIN);
//  m_appRow.set_value(0,Gtk::IconTheme::get_default()->load_icon("folder", 16, Gtk::ICON_LOOKUP_FORCE_SIZE));


    m_appRow = *(m_refTreeModel->append());
    m_appRow[m_appColumns.m_col_type] = NODE_OTHER;
    m_appRow[m_appColumns.m_col_name] = "Applications";
    m_appRow.set_value(0,Gtk::IconTheme::get_default()->load_icon("folder", 16, Gtk::ICON_LOOKUP_USE_BUILTIN));

    show_all_children();
}


ApplicationList::~ApplicationList()
{
}


bool ApplicationList::removeApplication(const char* szAppName)
{
    typedef Gtk::TreeModel::Children type_children;

    type_children children = m_refTreeModel->children();
    for(type_children::iterator iter = children.begin(); iter != children.end(); ++iter)
    {
      Gtk::TreeModel::Row row = *iter;
      if(row[m_appColumns.m_col_name] == Glib::ustring("Applications"))
      {
        type_children appchild = row->children();
        for(type_children::iterator itrapp = appchild.begin(); itrapp!= appchild.end(); ++itrapp)
        {
            Gtk::TreeModel::Row childrow = *itrapp;
            if(childrow[m_appColumns.m_col_name] == Glib::ustring(szAppName))
            {
                m_refTreeModel->erase(childrow);    
                return true;
            }
        }
      }
    }
    return false;
}



bool ApplicationList::addApplication(Application* app)
{
    typedef Gtk::TreeModel::Children type_children;

    type_children children = m_refTreeModel->children();
    for(type_children::iterator iter = children.begin(); iter != children.end(); ++iter)
    {
      Gtk::TreeModel::Row row = *iter;
      if(row[m_appColumns.m_col_name] == Glib::ustring("Applications"))
      {
        type_children appchild = row->children();
        for(type_children::iterator itrapp = appchild.begin(); itrapp!= appchild.end(); ++itrapp)
        {
            Gtk::TreeModel::Row childrow = *itrapp;
            if(childrow[m_appColumns.m_col_name] == Glib::ustring(app->getName()))
                return false;
        }
      }
    }

    Gtk::TreeModel::Row childrow = *(m_refTreeModel->append(m_appRow.children()));
    childrow[m_appColumns.m_col_type] = APPLICATION;
    childrow[m_appColumns.m_col_name] = app->getName();
    
    /*
    childrow[m_appColumns.m_col_refPix] = 
             Gdk::Pixbuf::create_from_data(application_ico.pixel_data, 
                                            Gdk::COLORSPACE_RGB,
                                            true,
                                            8,
                                            application_ico.width,
                                            application_ico.height,
                                            application_ico.bytes_per_pixel*application_ico.width);
    */
    childrow.set_value(0, Gdk::Pixbuf::create_from_data(application_ico.pixel_data, 
                                            Gdk::COLORSPACE_RGB,
                                            true,
                                            8,
                                            application_ico.width,
                                            application_ico.height,
                                            application_ico.bytes_per_pixel*application_ico.width));

    string fname;
    string fpath = app->getXmlFile();
    size_t pos = fpath.rfind(PATH_SEPERATOR);
    if(pos!=string::npos)
        fname = fpath.substr(pos);
    else
        fname = fpath;
     //fname = "..." + fname; 

    Gtk::TreeModel::Row descrow = *(m_refTreeModel->append(childrow.children()));
    descrow[m_appColumns.m_col_type] = NODE_FILENAME;
    descrow[m_appColumns.m_col_name] = fname;

#if defined(WIN32)
        //check it
#else
#if (GTKMM_MAJOR_VERSION == 2 && GTKMM_MINOR_VERSION >= 4)
    descrow.set_value(0,Gtk::IconTheme::get_default()->load_icon("document", 16, Gtk::ICON_LOOKUP_USE_BUILTIN));
#endif
#endif

    return true;
}
