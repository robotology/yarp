/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _MODULE_WINDOW_H_
#define _MODULE_WINDOW_H_

#include <gtkmm.h>
#include <yarp/manager/module.h>
#include <yarp/manager/manager.h>

class MainWindow;

class ModuleItemColumns : public Gtk::TreeModel::ColumnRecord
{
public:

    ModuleItemColumns() { 
        add(m_col_refPix); 
        add(m_col_name); 
        add(m_col_value);
    }

    Gtk::TreeModelColumn< Glib::RefPtr<Gdk::Pixbuf> > m_col_refPix;
    Gtk::TreeModelColumn<Glib::ustring> m_col_name;
    Gtk::TreeModelColumn<Glib::ustring> m_col_value;
};


class ModuleWindow: public Gtk::ScrolledWindow
{
public:
    ModuleWindow(yarp::manager::Module* module, MainWindow* parent, yarp::manager::Manager* manager);
    virtual ~ModuleWindow();

    const char* getModuleName(void) { return m_pModule->getName(); }    
    void onTabCloseRequest();
    void onRefresh();
protected:
    ModuleItemColumns m_Columns;  
    Glib::RefPtr<Gtk::TreeStore> m_refTreeModel;
    Gtk::TreeView m_TreeView;

private:
    MainWindow* m_pParent;
    yarp::manager::Module* m_pModule;
    yarp::manager::Manager* m_pManager;
    void updateWidget(void);
};


#endif //_MODULE_WINDOW_H_

