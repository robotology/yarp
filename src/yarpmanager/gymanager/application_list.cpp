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


#include "application_list.h"
#include "icon_res.h"

#include <string>
#include <iostream>

#include <yarp/manager/ymm-dir.h>

using namespace std;


ApplicationList::ApplicationList()
{

    /* Create a new scrolled window, with scrollbars only if needed */
    set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    add(m_TreeView);

    /* create list store */
    m_refTreeModel = Gtk::TreeStore::create(m_appColumns);
    m_TreeView.set_model(m_refTreeModel);

    m_refPixComputer = Gdk::Pixbuf::create_from_data(computer_ico.pixel_data, 
                                Gdk::COLORSPACE_RGB,
                                true,
                                8,
                                computer_ico.width,
                                computer_ico.height,
                                computer_ico.bytes_per_pixel*computer_ico.width);

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


    m_appRow = *(m_refTreeModel->append());
    m_appRow[m_appColumns.m_col_type] = NODE_OTHER;
    m_appRow[m_appColumns.m_col_name] = "Applications";
    m_appRow.set_value(0, Gdk::Pixbuf::create_from_data(folderapp_ico.pixel_data, 
                                            Gdk::COLORSPACE_RGB,
                                            true,
                                            8,
                                            folder_ico.width,
                                            folder_ico.height,
                                            folder_ico.bytes_per_pixel*folder_ico.width));
                                                               
    m_modRow = *(m_refTreeModel->append());
    m_modRow[m_appColumns.m_col_type] = NODE_OTHER;
    m_modRow[m_appColumns.m_col_name] = "Modules";
    m_modRow.set_value(0, Gdk::Pixbuf::create_from_data(foldermod_ico.pixel_data, 
                                            Gdk::COLORSPACE_RGB,
                                            true,
                                            8,
                                            folder_ico.width,
                                            folder_ico.height,
                                            folder_ico.bytes_per_pixel*folder_ico.width));
                                                               
    m_resRow = *(m_refTreeModel->append());
    m_resRow[m_appColumns.m_col_type] = NODE_OTHER;
    m_resRow[m_appColumns.m_col_name] = "Resources";
    m_resRow.set_value(0, Gdk::Pixbuf::create_from_data(folderres_ico.pixel_data, 
                                            Gdk::COLORSPACE_RGB,
                                            true,
                                            8,
                                            folder_ico.width,
                                            folder_ico.height,
                                            folder_ico.bytes_per_pixel*folder_ico.width));


    m_tempRow = *(m_refTreeModel->append());
    m_tempRow[m_appColumns.m_col_type] = NODE_OTHER;
    m_tempRow[m_appColumns.m_col_name] = "Templates";
    m_tempRow.set_value(0, Gdk::Pixbuf::create_from_data(folder_ico.pixel_data, 
                                            Gdk::COLORSPACE_RGB,
                                            true,
                                            8,
                                            folder_ico.width,
                                            folder_ico.height,
                                            folder_ico.bytes_per_pixel*folder_ico.width));

    show_all_children();
}


ApplicationList::~ApplicationList()
{
}


bool ApplicationList::removeApplication(const char* szAppName)
{
    typedef Gtk::TreeModel::Children type_children;
    type_children appchild = m_appRow->children();
    for(type_children::iterator itrapp = appchild.begin(); itrapp!= appchild.end(); ++itrapp)
    {
        Gtk::TreeModel::Row childrow = *itrapp;
        if(childrow[m_appColumns.m_col_name] == Glib::ustring(szAppName))
        {
            m_refTreeModel->erase(childrow);    
            return true;
        }
    }
    return false;
}


bool ApplicationList::removeResource(const char* szResName)
{
    typedef Gtk::TreeModel::Children type_children;
    type_children reschild = m_resRow->children();
    for(type_children::iterator itr = reschild.begin(); itr!= reschild.end(); ++itr)
    {
        Gtk::TreeModel::Row childrow = *itr;
        if(childrow[m_appColumns.m_col_name] == Glib::ustring(szResName))
        {
            m_refTreeModel->erase(childrow);
            return true;
        }
    }
    return false;
}


bool ApplicationList::removeModule(const char* szModName)
{
    typedef Gtk::TreeModel::Children type_children;
    type_children modchild = m_modRow->children();
    for(type_children::iterator itr = modchild.begin(); itr!= modchild.end(); ++itr)
    {
        Gtk::TreeModel::Row childrow = *itr;
        if(childrow[m_appColumns.m_col_name] == Glib::ustring(szModName))
        {
            m_refTreeModel->erase(childrow);
            return false;
        }
    }
    return true;
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
    childrow[m_appColumns.m_col_filename] = app->getXmlFile();
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
        fname = fpath.substr(pos+1);
    else
        fname = fpath;
     fname = fname + string(" (") + fpath + string(")"); 
    
    Gtk::TreeModel::Row descrow = *(m_refTreeModel->append(childrow.children()));
    descrow[m_appColumns.m_col_type] = NODE_FILENAME;
    descrow[m_appColumns.m_col_name] = fname;
    descrow[m_appColumns.m_col_filename] = app->getXmlFile();
    descrow.set_value(0, Gdk::Pixbuf::create_from_data(document_ico.pixel_data, 
                                            Gdk::COLORSPACE_RGB,
                                            true,
                                            8,
                                            document_ico.width,
                                            document_ico.height,
                                            document_ico.bytes_per_pixel*document_ico.width));
    return true;
}


bool ApplicationList::addComputer(Computer* comp)
{
    typedef Gtk::TreeModel::Children type_children;

    //type_children children = m_refTreeModel->children();
    type_children reschild = m_resRow->children();
    for(type_children::iterator itr = reschild.begin(); itr!= reschild.end(); ++itr)
    {
        Gtk::TreeModel::Row childrow = *itr;
        if(childrow[m_appColumns.m_col_name] == Glib::ustring(comp->getName()))
            return false;
    }


    Gtk::TreeModel::Row childrow = *(m_refTreeModel->append(m_resRow.children()));
    childrow[m_appColumns.m_col_type] = RESOURCE;
    childrow[m_appColumns.m_col_name] = comp->getName();
    childrow[m_appColumns.m_col_filename] = comp->getXmlFile();

    if(comp->getDisable())
        childrow.set_value(0, Gdk::Pixbuf::create_from_data(computer_disable_ico.pixel_data, 
                                            Gdk::COLORSPACE_RGB,
                                            true,
                                            8,
                                            computer_disable_ico.width,
                                            computer_disable_ico.height,
                                            computer_disable_ico.bytes_per_pixel*computer_disable_ico.width)); 
    else
        childrow.set_value(0, Gdk::Pixbuf::create_from_data(computer_ico.pixel_data, 
                                            Gdk::COLORSPACE_RGB,
                                            true,
                                            8,
                                            computer_ico.width,
                                            computer_ico.height,
                                            computer_ico.bytes_per_pixel*computer_ico.width));
    
    string fname;
    string fpath = comp->getXmlFile();
    size_t pos = fpath.rfind(PATH_SEPERATOR);
    if(pos!=string::npos)
        fname = fpath.substr(pos+1);
    else
        fname = fpath;
     fname = fname + string(" (") + fpath + string(")"); 
 
    Gtk::TreeModel::Row descrow = *(m_refTreeModel->append(childrow.children()));
    descrow[m_appColumns.m_col_type] = NODE_FILENAME;
    descrow[m_appColumns.m_col_name] = fname;
    descrow[m_appColumns.m_col_filename] = comp->getXmlFile();
    descrow.set_value(0, Gdk::Pixbuf::create_from_data(document_ico.pixel_data, 
                                            Gdk::COLORSPACE_RGB,
                                            true,
                                            8,
                                            document_ico.width,
                                            document_ico.height,
                                            document_ico.bytes_per_pixel*document_ico.width));
                                                                   
    return true;
}


bool ApplicationList::addModule(Module* mod)
{
    typedef Gtk::TreeModel::Children type_children;

    type_children modchild = m_modRow->children();
    for(type_children::iterator itr = modchild.begin(); itr!= modchild.end(); ++itr)
    {
        Gtk::TreeModel::Row childrow = *itr;
        if(childrow[m_appColumns.m_col_name] == Glib::ustring(mod->getName()))
            return false;
    }


    Gtk::TreeModel::Row childrow = *(m_refTreeModel->append(m_modRow.children()));
    childrow[m_appColumns.m_col_type] = MODULE;
    childrow[m_appColumns.m_col_name] = mod->getName();
    childrow[m_appColumns.m_col_filename] = mod->getXmlFile();
    childrow.set_value(0, Gdk::Pixbuf::create_from_data(module_ico.pixel_data, 
                                            Gdk::COLORSPACE_RGB,
                                            true,
                                            8,
                                            module_ico.width,
                                            module_ico.height,
                                            module_ico.bytes_per_pixel*module_ico.width));
    
    string fname;
    string fpath = mod->getXmlFile();
    size_t pos = fpath.rfind(PATH_SEPERATOR);
    if(pos!=string::npos)
        fname = fpath.substr(pos+1);
    else
        fname = fpath;
     fname = fname + string(" (") + fpath + string(")"); 

    Gtk::TreeModel::Row descrow = *(m_refTreeModel->append(childrow.children()));
    descrow[m_appColumns.m_col_type] = NODE_FILENAME;
    descrow[m_appColumns.m_col_name] = fname;
    descrow[m_appColumns.m_col_filename] = mod->getXmlFile();
    descrow.set_value(0, Gdk::Pixbuf::create_from_data(document_ico.pixel_data, 
                                            Gdk::COLORSPACE_RGB,
                                            true,
                                            8,
                                            document_ico.width,
                                            document_ico.height,
                                            document_ico.bytes_per_pixel*document_ico.width));
    return true;
}


bool ApplicationList::addAppTemplate(AppTemplate* temp)
{ 
    typedef Gtk::TreeModel::Children type_children;
    string fname = temp->name + string(" (") + temp->tmpFileName + string(")"); 
    Gtk::TreeModel::Row descrow = *(m_refTreeModel->append(m_tempRow.children()));
    descrow[m_appColumns.m_col_type] = NODE_FILENAME;
    descrow[m_appColumns.m_col_name] = fname;
    descrow[m_appColumns.m_col_filename] = temp->tmpFileName;
    descrow.set_value(0, Gdk::Pixbuf::create_from_data(document_ico.pixel_data, 
                                            Gdk::COLORSPACE_RGB,
                                            true,
                                            8,
                                            document_ico.width,
                                            document_ico.height,
                                            document_ico.bytes_per_pixel*document_ico.width));   
    return true;
}

