/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 *
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef APLICATION_PROPERTY_WINDOW_H
#define APLICATION_PROPERTY_WINDOW_H

#include <gtkmm.h>
#include <yarp/manager/application.h>
#include <yarp/manager/manager.h>

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
    ApplicationPropertyWindow(MainWindow* parent, yarp::manager::Manager* manager, ApplicationWindow* appWnd=NULL);
    virtual ~ApplicationPropertyWindow();

    void onTabCloseRequest();
    void onRefresh();

    void update(yarp::manager::Application* application);
    yarp::manager::Application* getApplication(void) { return m_pApplication; }
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
    yarp::manager::Application* m_pApplication;
    yarp::manager::Manager* m_pManager;
};


#endif //_APLICATION_PROPERTY_WINDOW_H_

