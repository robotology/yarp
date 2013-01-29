/*
 *  Yarp Modules Manager
 *  Copyright: 2011 (C) Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _MODULE_PROPERTY_WINDOW_H_
#define _MODULE_PROPERTY_WINDOW_H_

#include <gtkmm.h>
#include "module.h"
#include "manager.h"

class MainWindow;
class ApplicationWindow;

class ModulePropItemColumns : public Gtk::TreeModel::ColumnRecord
{
public:

    ModulePropItemColumns() { 
        add(m_col_name); 
        add(m_col_value);
        add(m_col_color_item);
        add(m_col_color_value);
        add(m_col_editable);
        add(m_col_choices);
    }

    Gtk::TreeModelColumn<Glib::ustring> m_col_name;
    Gtk::TreeModelColumn<Glib::ustring> m_col_value;
    Gtk::TreeModelColumn<Gdk::Color> m_col_color_item;
    Gtk::TreeModelColumn<Gdk::Color> m_col_color_value;
    Gtk::TreeModelColumn<bool> m_col_editable;
    Gtk::TreeModelColumn< Glib::RefPtr<Gtk::TreeModel> > m_col_choices;
};


//Tree model columns for the Combo CellRenderer in the TreeView column:
class ModelColumnsCombo : public Gtk::TreeModel::ColumnRecord
{
public:

    ModelColumnsCombo() { 
        add(m_col_choice); 
        add(m_col_description); 
    }
    Gtk::TreeModelColumn<Glib::ustring> m_col_choice; //The values from which the user may choose.
    Gtk::TreeModelColumn<Glib::ustring> m_col_description; //Extra information to help the user to choose.
};



class ModulePropertyWindow: public Gtk::ScrolledWindow
{
public:
    ModulePropertyWindow(MainWindow* parent, Manager* manager, ApplicationWindow* appWnd=NULL);
    virtual ~ModulePropertyWindow();

    void onTabCloseRequest();
    void onRefresh();

    void update(Module* module);
    void release(void) { m_pModule = NULL; m_pIModule = NULL; } 
    Module* getModule(void) { return m_pModule; }

    void updateModule(const char* item, const char* value);

protected:
    void onCellData(Gtk::CellRenderer*, const Gtk::TreeModel::iterator& iter);
    void onCellEdited(const Glib::ustring& path_string, 
                    const Glib::ustring& new_text);
protected:
    ModulePropItemColumns m_Columns;  
    ModelColumnsCombo m_ColumnsCombo;
    Glib::RefPtr<Gtk::TreeStore> m_refTreeModel;
    Gtk::TreeView m_TreeView;

    std::vector< Glib::RefPtr<Gtk::ListStore> > m_refModelCombos;

private:
    void updateParamteres(void);
    bool getRowByName(const char* name, Gtk::TreeModel::Row* row);

    MainWindow* m_pParent;
    Module* m_pModule;
    ModuleInterface* m_pIModule;
    Manager* m_pManager;
    ApplicationWindow* m_pAppWindow;
};


#endif //_MODULE_PROPERTY_WINDOW_H_

