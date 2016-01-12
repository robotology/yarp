/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 *
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef CONNECTION_PROPERTY_WINDOW_H
#define CONNECTION_PROPERTY_WINDOW_H

#include <gtkmm.h>
#include <yarp/manager/application.h>
#include <yarp/manager/manager.h>
#include "arrow_model.h"

class MainWindow;
class ApplicationWindow;

class ConnectionPropItemColumns : public Gtk::TreeModel::ColumnRecord
{
public:

    ConnectionPropItemColumns() {
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


class ConnectionPropertyWindow: public Gtk::ScrolledWindow
{
public:
    ConnectionPropertyWindow(MainWindow* parent, yarp::manager::Manager* manager, ApplicationWindow* appWnd=NULL);
    virtual ~ConnectionPropertyWindow();

    void onTabCloseRequest();
    void onRefresh();

    void update(Glib::RefPtr<ArrowModel>& arrow);
    void release(void);

    Glib::RefPtr<ArrowModel>& getArrow(void) { return m_pArrow; }

protected:
    void onCellData(Gtk::CellRenderer*, const Gtk::TreeModel::iterator& iter);
    void onCellEdited(const Glib::ustring& path_string,
                    const Glib::ustring& new_text);
protected:
    ConnectionPropItemColumns m_Columns;
    Glib::RefPtr<Gtk::TreeStore> m_refTreeModel;
    Gtk::TreeView m_TreeView;

private:
    const char* getCarrierOptions(const char* opt);

private:
    MainWindow* m_pParent;
//    Connection* m_pConnection;
    yarp::manager::Manager* m_pManager;
    ApplicationWindow* m_pAppWindow;
    Glib::RefPtr<ArrowModel> m_pArrow;
    Gtk::TreeModel::Row carrierRow;

};


#endif //_CONNECTION_PROPERTY_WINDOW_H_

