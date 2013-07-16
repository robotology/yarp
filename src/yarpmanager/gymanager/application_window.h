/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _APPLICATION_WINDOW_H_
#define _APPLICATION_WINDOW_H_

#include <gtkmm.h>
#include "safe_manager.h"
#include <vector>
#include <map>

#include <yarp/os/Network.h>
#include <yarp/os/Property.h>
#include <yarp/os/ConstString.h>
#include <yarp/os/Time.h>

#include "stdout_window.h"

class MainWindow;

//Tree model columns:
class ModuleModelColumns : public Gtk::TreeModel::ColumnRecord
{
public:

    ModuleModelColumns() { 
        add(m_col_refPix);
        add(m_col_name); 
        add(m_col_id);
        add(m_col_status); 
        add(m_col_host); 
        add(m_col_param); 
        add(m_col_stdio); 
        add(m_col_wdir); 
        add(m_col_env); 
        add(m_col_color);
        add(m_col_editable);
        }
    Gtk::TreeModelColumn< Glib::RefPtr<Gdk::Pixbuf> > m_col_refPix;
    Gtk::TreeModelColumn<int> m_col_id;
    Gtk::TreeModelColumn<Glib::ustring> m_col_name;
    Gtk::TreeModelColumn<Glib::ustring> m_col_status;
    Gtk::TreeModelColumn<Glib::ustring> m_col_host;
    Gtk::TreeModelColumn<Glib::ustring> m_col_param;
    Gtk::TreeModelColumn<Glib::ustring> m_col_stdio;
    Gtk::TreeModelColumn<Glib::ustring> m_col_wdir;
    Gtk::TreeModelColumn<Glib::ustring> m_col_env;
    Gtk::TreeModelColumn<Gdk::Color> m_col_color;
    Gtk::TreeModelColumn<bool> m_col_editable;

};



class ConnectionModelColumns : public Gtk::TreeModel::ColumnRecord
{
public:

    ConnectionModelColumns() { 
        add(m_col_refPix); 
        add(m_col_type);
        add(m_col_id); 
        add(m_col_status);
        add(m_col_from); 
        add(m_col_to); 
        add(m_col_carrier); 
        add(m_col_color);
        add(m_col_from_color);
        add(m_col_to_color);
        add(m_col_editable);
        }
    Gtk::TreeModelColumn< Glib::RefPtr<Gdk::Pixbuf> > m_col_refPix;
    Gtk::TreeModelColumn<int> m_col_id;
    Gtk::TreeModelColumn<Glib::ustring> m_col_type;
    Gtk::TreeModelColumn<Glib::ustring> m_col_status;
    Gtk::TreeModelColumn<Glib::ustring> m_col_from;
    Gtk::TreeModelColumn<Glib::ustring> m_col_to;
    Gtk::TreeModelColumn<Glib::ustring> m_col_carrier;
    Gtk::TreeModelColumn<Gdk::Color> m_col_color;
    Gtk::TreeModelColumn<Gdk::Color> m_col_from_color;
    Gtk::TreeModelColumn<Gdk::Color> m_col_to_color;
    Gtk::TreeModelColumn<bool> m_col_editable;
};


class ResourceModelColumns : public Gtk::TreeModel::ColumnRecord
{
public:

    ResourceModelColumns() { 
        add(m_col_refPix); 
        add(m_col_res); 
        add(m_col_id); 
        add(m_col_status);
        add(m_col_color);
        add(m_col_type);
    }

    Gtk::TreeModelColumn< Glib::RefPtr<Gdk::Pixbuf> > m_col_refPix;
    Gtk::TreeModelColumn<int> m_col_id;
    Gtk::TreeModelColumn<Glib::ustring> m_col_res;
    Gtk::TreeModelColumn<Glib::ustring> m_col_status;
    Gtk::TreeModelColumn<Gdk::Color> m_col_color;
    Gtk::TreeModelColumn<Glib::ustring> m_col_type;
};



class ApplicationWindow; 


class MyTreeView: public Gtk::TreeView
{
public:
    MyTreeView() { 
        __pMouseEventCallback = NULL; 
        __pParent = NULL;
    }

    virtual ~MyTreeView() {}

    void setMouseEventCallback(ApplicationWindow* _parent,
                              void (ApplicationWindow::*__pCallback)(GdkEventButton*)) 
    {
        __pParent = _parent;
        __pMouseEventCallback = __pCallback;
    }

protected:
    virtual bool on_button_press_event(GdkEventButton* event )
    {
        if(event->type == GDK_BUTTON_PRESS)
        {

#if (GTKMM_MAJOR_VERSION == 2 && GTKMM_MINOR_VERSION >= 16)
            Gtk::TreeModel::Path path;
            bool bOnItem = get_path_at_pos((int)event->x, (int)event->y, path);
            // if it's not a free click
            if(!bOnItem)
                get_selection()->unselect_all();    
#endif 
            if(event->button == 3)
            {
                if(__pMouseEventCallback && __pParent)
                {
                    (__pParent->*__pMouseEventCallback)(event);
                    return true;
                }
            }
        }

        return Gtk::TreeView::on_button_press_event(event);
    }

private:
    ApplicationWindow* __pParent;
    void (ApplicationWindow::*__pMouseEventCallback)(GdkEventButton*);  
};




class ApplicationWindow: public Gtk::Frame, ApplicationEvent
{
public:
    ApplicationWindow(const char* szAppName, Manager* lazy, 
        yarp::os::Property* config, MainWindow* parent);
    virtual ~ApplicationWindow();
    const char* getApplicationName(void) { return m_strAppName.c_str(); }

    void adjustPositions(void);
    bool onClose(void);
    bool onRun(void);
    bool onStop(void);
    bool onKill(void);
    bool onConnect(void);
    bool onDisconnect(void);
    bool onRefresh(void);
    bool onSelectAll(void);
    bool onExportGraph(void);
    bool onAttachStdout(void);
    bool onDetachStdout(void);
    void onPMenuLoadBalance(void);
    void onPModuleSelectAll() { m_refTreeModSelection->select_all(); }
    void onPConnectionSelectAll() { m_refTreeConSelection->select_all(); }
    void onPResourceSelectAll() { m_refTreeResSelection->select_all(); }

    void onTabCloseRequest();
    void releaseApplication(void);


    Glib::RefPtr<Gtk::TreeStore> m_refTreeModModel;
    Glib::RefPtr<Gtk::TreeStore> m_refTreeConModel;
    Glib::RefPtr<Gtk::TreeStore> m_refTreeResModel;
    ModuleModelColumns m_modColumns;
    ConnectionModelColumns m_conColumns;
    ResourceModelColumns m_resColumns;
    Glib::RefPtr<Gtk::UIManager> m_refUIManager;
    Glib::RefPtr<Gtk::ActionGroup> m_refActionGroup;

    void onModStart(int which);
    void onModStop(int which);
    void onModStdout(int which, const char* msg);
    void onConConnect(int which);
    void onConDisconnect(int which);
    void onResAvailable(int which);
    void onResUnAvailable(int which);
    void onConAvailable(int from, int to);
    void onConUnAvailable(int from, int to);
    void onError(void);
    void onLoadBalance(void);

protected:
    void selectedModuleCallback(const Gtk::TreeModel::iterator& iter);
    void selectedConnectionCallback(const Gtk::TreeModel::iterator& iter);
    void selectedResourceCallback(const Gtk::TreeModel::iterator& iter);
    void onModuleTreeButtonPressed(GdkEventButton* event);
    void onConnectionTreeButtonPressed(GdkEventButton* event);
    void onResourceTreeButtonPressed(GdkEventButton* event);
    
    void onPMenuRun() { onRun(); }
    void onPMenuStop() { onStop(); }
    void onPMenuKill() { onKill(); }
    void onPMenuConnect() { onConnect(); }
    void onPMenuDisconnect() { onDisconnect(); }
    void onPMenuRefresh() { onRefresh(); }
    void onPMenuAttachStdout() { onAttachStdout(); }

    Gtk::VPaned m_VPaned;
    Gtk::HPaned m_HPaned;
    //Gtk::TreeView m_TreeModView;
    MyTreeView m_TreeModView;
    MyTreeView m_TreeConView;
    MyTreeView m_TreeResView;
    Gtk::ScrolledWindow m_ScrollModView;
    Gtk::ScrolledWindow m_ScrollConView;
    Gtk::ScrolledWindow m_ScrollResView;

    Gtk::TreeModel::Row m_modRow;
    Gtk::TreeModel::Row m_conRow;
    Gtk::TreeModel::Row m_resRow;
    Glib::RefPtr<Gtk::TreeSelection> m_refTreeModSelection;
    Glib::RefPtr<Gtk::TreeSelection> m_refTreeConSelection;
    Glib::RefPtr<Gtk::TreeSelection> m_refTreeResSelection;

    /*
    void onModuleEditingStarted(Gtk::CellEditable* cell_editable, 
                                const Glib::ustring& path);
    void onConnectionEditingStarted(Gtk::CellEditable* cell_editable, 
                                const Glib::ustring& path);
    */
    //void onModuleEdited(const Glib::ustring& path_string, 
    //                    const Glib::ustring& new_text);    

    int dummy_h;
    int dummy_w;
    void on_size_allocate(Gtk::Allocation& allocation)
    {
        if(!dummy_h || !dummy_w)
        {
            dummy_h = allocation.get_height();
            dummy_w = allocation.get_width();
            m_VPaned.set_position((int)(allocation.get_height()/2.0));
            m_HPaned.set_position((int)(allocation.get_width()/2.0));
        }

        Gtk::Frame::on_size_allocate(allocation);
    }

private:
    bool m_bShouldRun;
    SafeManager manager;
    MainWindow* m_pParent;
    string m_strAppName;
    yarp::os::Property* m_pConfig;
    std::vector<int> m_ModuleIDs;
    std::vector<int> m_ConnectionIDs;
    std::vector<int> m_ResourceIDs;

    Glib::RefPtr<Gdk::Pixbuf> m_refPixSuspended;
    Glib::RefPtr<Gdk::Pixbuf> m_refPixRunning;
    Glib::RefPtr<Gdk::Pixbuf> m_refPixWaiting;
    Glib::RefPtr<Gdk::Pixbuf> m_refPixConnected;
    Glib::RefPtr<Gdk::Pixbuf> m_refPixDisconnected;
    Glib::RefPtr<Gdk::Pixbuf> m_refPixAvailable;
    Glib::RefPtr<Gdk::Pixbuf> m_refPixUnAvailable;
    Glib::RefPtr<Gdk::Pixbuf> m_refPixComputer;
    Glib::RefPtr<Gdk::Pixbuf> m_refPixUnknown;
    Glib::RefPtr<Gdk::Pixbuf> m_refPixPort;
    Glib::RefPtr<Gdk::Pixbuf> m_refPixPortAvaibable;
    Glib::RefPtr<Gdk::Pixbuf> m_refPixPortUnAvailable;
    Glib::RefPtr<Gdk::Pixbuf> m_refPixPortUnknown;

    std::map<int, StdoutWindow*> m_MapstdWnds;

    void createWidgets(void);
    void setupSignals(void);
        
    bool getModRowByID(int id, Gtk::TreeModel::Row* row );
    bool getConRowByID(int id, Gtk::TreeModel::Row* row );
    bool getResRowByID(int id, Gtk::TreeModel::Row* row );
    void setCellsEditable(void);
    bool areAllShutdown(void);

    void prepareManagerFrom(Manager* lazy, const char* szName);
    void updateApplicationWindow(void);

    void reportErrors(void);
    
    void doClose(void);
    void doRun(void);
    void doStop(void);
    void doKill(void);
    void doConnect(void);
    void doDisconnect(void);
    void doRefresh(void);

};

#endif //_APPLICATION_WINDOW_H_

