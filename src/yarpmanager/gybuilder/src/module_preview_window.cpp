/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#if defined(WIN32)
    #pragma warning (disable : 4250)
    #pragma warning (disable : 4520)
#endif

#include <yarp/manager/ymm-dir.h>
#include "module_preview_window.h"
#include "icon_res.h"
#include "main_window.h"

ModulePreviewWindow::ModulePreviewWindow(MainWindow* parent) : m_pModule(NULL)
{   
    m_pParent = parent;

    /* Create a new scrolled window, with scrollbars only if needed */
    //set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    //add(m_TreeView);

    m_Canvas = Gtk::manage(new Goocanvas::Canvas()) ;
    //m_Canvas->signal_item_created().connect(sigc::mem_fun(*this, &ApplicationWindow::on_item_created)) ;
    root = Goocanvas::GroupModel::create();
    m_Canvas->set_root_item_model(root) ;
    m_Canvas->property_background_color().set_value("GhostWhite");
    m_Canvas->set_bounds(0,0,600,400);
    //m_Canvas->drag_dest_set(m_pParent->dragTargets, 
    //                        Gtk::DEST_DEFAULT_ALL, Gdk::ACTION_COPY);


    m_ScrollView.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    m_ScrollView.add(*m_Canvas);
    add(m_ScrollView); 
    show_all_children();
}


ModulePreviewWindow::~ModulePreviewWindow()
{
}

void ModulePreviewWindow::onRefresh()
{
    /*
    if(!m_pModule)
        return;
    update(m_pModule);
    m_pParent->reportErrors();
    */
}

void ModulePreviewWindow::onTabCloseRequest() 
{ 
    m_pParent->onTabCloseRequest(this); 
}


void ModulePreviewWindow::update(Module* module)
{
    m_pModule = module;
    for(int i=0; i<root->get_n_children(); i++)
        root->remove_child(i);

    /*
    Glib::RefPtr<ModuleModel> mod = ModuleModel::create(NULL, module);
    root->add_child(mod);
    mod->set_property("x", 10);
    mod->set_property("y", 10);
    */
}



