/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 *
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef RESOURCE_WINDOW_H
#define RESOURCE_WINDOW_H

#include <gtkmm.h>
#include <yarp/manager/primresource.h>
#include <yarp/manager/manager.h>

class MainWindow;

class ResourceItemColumns : public Gtk::TreeModel::ColumnRecord
{
public:

    ResourceItemColumns() {
        add(m_col_refPix);
        add(m_col_name);
        add(m_col_value);
    }

    Gtk::TreeModelColumn< Glib::RefPtr<Gdk::Pixbuf> > m_col_refPix;
    Gtk::TreeModelColumn<Glib::ustring> m_col_name;
    Gtk::TreeModelColumn<Glib::ustring> m_col_value;
};


class ResourceWindow: public Gtk::ScrolledWindow
{
public:
    ResourceWindow(yarp::manager::Computer* computer, MainWindow* parent, yarp::manager::Manager* manager);
    virtual ~ResourceWindow();

    const char* getResourceName(void) { return m_pComputer->getName(); }
    void onTabCloseRequest();
    void onRefresh();
protected:
    ResourceItemColumns m_Columns;
    Glib::RefPtr<Gtk::TreeStore> m_refTreeModel;
    Gtk::TreeView m_TreeView;

private:
    MainWindow* m_pParent;
    yarp::manager::Computer* m_pComputer;
    yarp::manager::Manager* m_pManager;
    void updateWidget(void);
};

#endif //_RESOURCE_WINDOW_H_

