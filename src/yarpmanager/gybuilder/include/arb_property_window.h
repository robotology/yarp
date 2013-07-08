/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _ARBITRATOR_PROPERTY_WINDOW_H_
#define _ARBITRATOR_PROPERTY_WINDOW_H_

#include <gtkmm.h>
#include "application.h"
#include "manager.h"
#include "arrow_model.h"
#include "port_abitrator_model.h"

class MainWindow;
class ApplicationWindow;

class ArbitratorPropItemColumns : public Gtk::TreeModel::ColumnRecord
{
public:

    ArbitratorPropItemColumns() { 
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


class ArbitratorPropertyWindow: public Gtk::ScrolledWindow
{
public:
    ArbitratorPropertyWindow(MainWindow* parent, Manager* manager, ApplicationWindow* appWnd=NULL);
    virtual ~ArbitratorPropertyWindow();

    void onTabCloseRequest();
    void onRefresh();

    void update(Glib::RefPtr<PortArbitratorModel>& arbPort);
    void release(void);
    
protected:
    void onCellData(Gtk::CellRenderer*, const Gtk::TreeModel::iterator& iter);
    void onCellEdited(const Glib::ustring& path_string, 
                    const Glib::ustring& new_text);
    void setExcitation(ArrowModel* arrow, const char* szLink, int value);

protected:
    ArbitratorPropItemColumns m_Columns;  
    Glib::RefPtr<Gtk::TreeStore> m_refTreeModel;
    Gtk::TreeView m_TreeView;

private:
    MainWindow* m_pParent;
    Manager* m_pManager;
    ApplicationWindow* m_pAppWindow;
    Glib::RefPtr<PortArbitratorModel> m_arbPort;
    Gtk::TreeModel::Row rulesRow;
};


#endif //_ARBITRATOR_PROPERTY_WINDOW_H_

