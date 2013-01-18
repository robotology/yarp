/*
 *  Yarp Modules Manager
 *  Copyright: 2011 (C) Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _APLICATION_PROPERTY_WINDOW_H_
#define _APLICATION_PROPERTY_WINDOW_H_

#include <gtkmm.h>
#include "application.h"
#include "manager.h"

class MainWindow;
class ApplicationWindow;

class ApplicationPropItemColumns : public Gtk::TreeModel::ColumnRecord
{
public:

    ApplicationPropItemColumns() { 
        add(m_col_name); 
        add(m_col_value);
        add(m_col_color_value);
        add(m_col_editable);
    }

    Gtk::TreeModelColumn<Glib::ustring> m_col_name;
    Gtk::TreeModelColumn<Glib::ustring> m_col_value;
    Gtk::TreeModelColumn<Gdk::Color> m_col_color_value;
    Gtk::TreeModelColumn<bool> m_col_editable;

};


class ApplicationPropertyWindow: public Gtk::ScrolledWindow
{
public:
    ApplicationPropertyWindow(MainWindow* parent, Manager* manager, ApplicationWindow* appWnd=NULL);
    virtual ~ApplicationPropertyWindow();

    void onTabCloseRequest();
    void onRefresh();

    void update(Application* application);
    Application* getApplication(void) { return m_pApplication; }
    void release(void) { m_pApplication = NULL; } 

protected:
    void onCellData(Gtk::CellRenderer*, const Gtk::TreeModel::iterator& iter);
    void onCellEdited(const Glib::ustring& path_string, 
                    const Glib::ustring& new_text);
protected:
    ApplicationPropItemColumns m_Columns;  
    Glib::RefPtr<Gtk::TreeStore> m_refTreeModel;
    Gtk::TreeView m_TreeView;

private:
    void updateApplication(const char* item, const char* value);

    MainWindow* m_pParent;
    ApplicationWindow* m_pAppWindow;
    Application* m_pApplication;
    Manager* m_pManager;
};


#endif //_APLICATION_PROPERTY_WINDOW_H_

