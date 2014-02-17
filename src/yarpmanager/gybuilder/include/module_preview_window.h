/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _MODULE_PREVIEW_WINDOW_H_
#define _MODULE_PREVIEW_WINDOW_H_

#include <gtkmm.h>
#include <goocanvasmm.h>
#include "module_model.h"

class MainWindow;
class ApplicationWindow;

class ModulePreviewWindow: public Gtk::Frame
{
public:
    ModulePreviewWindow(MainWindow* parent);
    virtual ~ModulePreviewWindow();

    void onTabCloseRequest();
    void onRefresh();
    void update(yarp::manager::Module* module);
    void release(void) { m_pModule = NULL; } 
    yarp::manager::Module* getModule(void) { return m_pModule; }

private:
    MainWindow* m_pParent;
    yarp::manager::Module* m_pModule;

    Goocanvas::Canvas* m_Canvas;
    Glib::RefPtr<Goocanvas::ItemModel> root;
    Gtk::ScrolledWindow m_ScrollView; 

};


#endif //_MODULE_PREVIEW_WINDOW_H_
