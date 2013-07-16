/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _APPLICATION_LIST_
#define _APPLICATION_LIST_


#include <gtkmm.h>
#include "application.h"
#include "primresource.h"
#include "xmltemploader.h"

class MainWindow;

//Tree model columns:
class AppModelColumns : public Gtk::TreeModel::ColumnRecord
{
public:

    AppModelColumns() { 
        add(m_col_refPix); 
        add(m_col_name); 
        add(m_col_type);
        add(m_col_filename);
        }
    //Gtk::TreeModelColumn<int> m_col_id;
    Gtk::TreeModelColumn< Glib::RefPtr<Gdk::Pixbuf> > m_col_refPix;
    Gtk::TreeModelColumn<Glib::ustring> m_col_name;
    Gtk::TreeModelColumn<NodeType> m_col_type;
    Gtk::TreeModelColumn<Glib::ustring> m_col_filename;
};


class ApplicationList: public Gtk::ScrolledWindow
{

public:
    ApplicationList(MainWindow* parent);
    virtual ~ApplicationList();
    bool addApplication(Application* app);
    bool addComputer(Computer* comp);
    bool addModule(Module* mod);
    bool addAppTemplate(AppTemplate* temp);

    bool removeApplication(const char* szAppName);
    bool removeModule(const char* szModName);
    bool removeResource(const char* szResName);
    
    Glib::RefPtr<Gtk::TreeStore> m_refTreeModel;
    AppModelColumns m_appColumns;
    AppModelColumns m_resColumns;

    Gtk::TreeView* getTreeView(void) { return &m_TreeView;}

protected:
    void onDragBegin(const Glib::RefPtr<Gdk::DragContext>& context);
    void onDragDataGet(const Glib::RefPtr<Gdk::DragContext>& context, 
                             Gtk::SelectionData& data, guint info, guint time);
protected:
    Gtk::TreeView m_TreeView; //The Tree View.
    Gtk::TreeModel::Row m_appRow;
    Gtk::TreeModel::Row m_modRow;
    Gtk::TreeModel::Row m_resRow;
    Gtk::TreeModel::Row m_tempRow;
    Glib::RefPtr<Gdk::Pixbuf> m_refPixComputer;
};


#endif //_APPLICATION_LIST_

