/*
 *  Yarp Modules Manager
 *  Copyright: 2011 (C) Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#if defined(WIN32)
    #pragma warning (disable : 4250)
    #pragma warning (disable : 4520)
#endif

#include "application_window.h"
#include "main_window.h"
#include "icon_res.h"
#include "module_model.h"
#include "application_model.h"
#include "port_model.h"
#include "midpoint_model.h"
#include "label_model.h"
#include "tooltip_model.h"
#include "ext_port_model.h"
#include "int_port_model.h"

#include <sstream>
#include <gdk-pixbuf/gdk-pixbuf.h>

#define GRID_PATTERN_SIZE       15

using namespace std;

static void put_pixel (GdkPixbuf *pixbuf, int x, int y, 
            guchar red, guchar green, guchar blue, guchar alpha)
{    
      int width, height, rowstride, n_channels;
      guchar *pixels, *p;
      n_channels = gdk_pixbuf_get_n_channels (pixbuf);
      g_assert (gdk_pixbuf_get_colorspace (pixbuf) == GDK_COLORSPACE_RGB);
      g_assert (gdk_pixbuf_get_bits_per_sample (pixbuf) == 8);
      g_assert (gdk_pixbuf_get_has_alpha (pixbuf));
      g_assert (n_channels == 4);
      width = gdk_pixbuf_get_width (pixbuf);
      height = gdk_pixbuf_get_height (pixbuf);
      g_assert (x >= 0 && x < width);
      g_assert (y >= 0 && y < height);
      rowstride = gdk_pixbuf_get_rowstride (pixbuf);
      pixels = gdk_pixbuf_get_pixels (pixbuf);
      p = pixels + y * rowstride + x * n_channels;
      p[0] = red;
      p[1] = green;
      p[2] = blue;
      p[3] = alpha;
}


ApplicationWindow::ApplicationWindow(const char* szAppName, Manager* lazy, 
                                    yarp::os::Property* config, MainWindow* parent,
                                     bool grid, bool snap)
{
    dummy_h = dummy_w = 0;
    m_Canvas = NULL;
    m_Grid = NULL;
    m_snapToGrid = snap;
    m_showGrid = grid;
    m_pConfig = config;
    m_pParent = parent;
    m_strAppName = szAppName;
    m_connector.clear();    
    m_selector.clear();    
    m_bModified = false;  //TODO: set it in the right places
    createWidgets();
    setupSignals();
    show_all_children();
    set_can_focus(true);
    grab_focus();
    //add_events(Gdk::KEY_PRESS_MASK | Gdk::KEY_RELEASE_MASK);

    prepareManagerFrom(lazy, szAppName);
}


ApplicationWindow::~ApplicationWindow()
{
    delete modPropertyWindow;
    delete appPropertyWindow;
    delete conPropertyWindow;
}

void ApplicationWindow::createWidgets(void)
{

    m_Canvas = Gtk::manage(new Goocanvas::Canvas()) ;
    m_Canvas->signal_item_created().connect(sigc::mem_fun(*this, &ApplicationWindow::on_item_created)) ;
    root = Goocanvas::GroupModel::create();
    m_Canvas->set_root_item_model(root) ;
    m_Canvas->property_background_color().set_value("GhostWhite");
    m_Canvas->set_bounds(0,0,600,400);
    m_Canvas->drag_dest_set(m_pParent->dragTargets, 
                            Gtk::DEST_DEFAULT_ALL, Gdk::ACTION_COPY);

    Goocanvas::Bounds b;
    m_Canvas->get_bounds(b);     
    m_Grid = goo_canvas_grid_model_new(root->gobj(), 0, 0, b.get_x2(), b.get_y2(),
                                                       GRID_PATTERN_SIZE, GRID_PATTERN_SIZE, 0.0, 0.0,
                                                       "horz-grid-line-width", 1.0,
                                                       "horz-grid-line-color", "lightgray",
                                                       "vert-grid-line-width", 1.0,
                                                       "vert-grid-line-color", "lightgray",
                                                       "border-width", 0.0,
                                                       "border-color", "white",
                                                       /*"antialias", CAIRO_ANTIALIAS_NONE,*/
                                                       NULL);
    goo_canvas_item_model_add_child(root->gobj(), m_Grid, -1);
    if(!m_showGrid)        
        g_object_set(m_Grid, "visibility", GOO_CANVAS_ITEM_HIDDEN, NULL);

    // setting grid pattern
    // TODO: check how to free the pixbuf
    pixV = gdk_pixbuf_new(GDK_COLORSPACE_RGB, true, 8, 1, GRID_PATTERN_SIZE);
    pixH = gdk_pixbuf_new(GDK_COLORSPACE_RGB, true, 8, GRID_PATTERN_SIZE, 1);
    for(int i=0; i<GRID_PATTERN_SIZE; i++)
    {
        if((i<1) || (i>GRID_PATTERN_SIZE-2))
        {
            put_pixel(pixV, 0, i, 200, 200, 200, 255);
            put_pixel(pixH, i, 0, 200, 200, 200, 255);
        }
        else
        {
            put_pixel(pixV, 0, i, 0, 0, 0, 0); 
            put_pixel(pixH, i, 0, 0, 0, 0, 0);
        }
    }
    g_object_set(m_Grid, "vert-grid-line-pixbuf", pixV, NULL);
    g_object_set(m_Grid, "horz-grid-line-pixbuf", pixH, NULL);



    modPropertyWindow = new ModulePropertyWindow(m_pParent, &manager, this);
    appPropertyWindow = new ApplicationPropertyWindow(m_pParent, &manager, this);
    conPropertyWindow = new ConnectionPropertyWindow(m_pParent, &manager, this);

    m_ScrollView.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    m_ScrollView.add(*m_Canvas);
    
    m_HPaned.add1(m_ScrollView);

    //m_VPaned.set_position(WND_DEF_HEIGHT - WND_DEF_HEIGHT/3);
    m_HPaned.set_size_request(-1, 300);
    add(m_HPaned);
    //m_VBox.pack_start(m_VPaned);
    //m_VBox.pack_start(m_Statusbar, Gtk::PACK_SHRINK);


}

void ApplicationWindow::setupSignals(void)
{
    m_Canvas->signal_drag_data_received().connect(sigc::mem_fun(*this,
            &ApplicationWindow::onDragDataReceived) );      
    m_Canvas->signal_drag_motion().connect(sigc::mem_fun(*this,
            &ApplicationWindow::onDragMotion) );
    m_Canvas->signal_drag_drop().connect(sigc::mem_fun(*this,
            &ApplicationWindow::onDragDrop) );

//  m_TreeModView.signal_button_press_event().connect_notify(sigc::mem_fun(*this,
//            &ApplicationWindow::onModuleTreeButtonPressed) );

//  m_TreeConView.signal_button_press_event().connect_notify(sigc::mem_fun(*this,
//            &ApplicationWindow::onConnectionTreeButtonPressed) );

//  m_TreeResView.signal_button_press_event().connect_notify(sigc::mem_fun(*this,
//            &ApplicationWindow::onResourceTreeButtonPressed) );

}


bool ApplicationWindow::onRefresh(void)
{
   return true;    
}


void ApplicationWindow::onTabCloseRequest() 
{ 
    m_pParent->onTabCloseRequest(this); 
}


bool ApplicationWindow::onClose(void) 
{
    if(m_bModified)
    {
        ostringstream msg;
        msg<<getApplicationName()<<" has been modified.";
        Gtk::MessageDialog dialog(msg.str(), false, Gtk::MESSAGE_QUESTION,  Gtk::BUTTONS_NONE);
        dialog.set_secondary_text("Do you want to save it before closing?");
        dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
        dialog.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_YES);
        dialog.add_button(Gtk::Stock::NO, Gtk::RESPONSE_NO);

        int response = dialog.run();
        if(response == Gtk::RESPONSE_CANCEL)
            return false;

        if(response == Gtk::RESPONSE_YES)
            m_pParent->onMenuFileSave();
    }
    releaseApplication();
    return true;
} 


bool ApplicationWindow::onSave(const char* szFileName)
{
    Application* application = manager.getKnowledgeBase()->getApplication();
    if(!application)
        return false;

    m_bModified = !manager.saveApplication(application->getName(), szFileName);
    return !m_bModified;
    //return manager.saveApplication(application->getName(), szFileName);

}

bool ApplicationWindow::onSelectAll(void)
{
    bool selected = false;
    for(int j=0; j<root->get_n_children(); j++)
        for(int i=0; i<root->get_n_children(); i++)
        {
            selected = true;
            Glib::RefPtr<ApplicationModel> app = Glib::RefPtr<ApplicationModel>::cast_dynamic(root->get_child(i));
            if(app) app->setSelected(true);
            Glib::RefPtr<ModuleModel> mod = Glib::RefPtr<ModuleModel>::cast_dynamic(root->get_child(i));
            if(mod) mod->setSelected(true);
            Glib::RefPtr<ArrowModel> arw = Glib::RefPtr<ArrowModel>::cast_dynamic(root->get_child(i));
            if(arw) arw->setSelected(true);
            Glib::RefPtr<ExternalPortModel> extPort = Glib::RefPtr<ExternalPortModel>::cast_dynamic(root->get_child(i));
            if(extPort) extPort->setSelected(true);
        }
    m_pParent->m_refActionGroup->get_action("EditDelete")->set_sensitive(selected);
    m_pParent->m_refActionGroup->get_action("EditCopy")->set_sensitive(selected);
    m_pParent->m_refActionGroup->get_action("EditPaste")->set_sensitive(selected);
   
    return true;
}

bool ApplicationWindow::onExportGraph(void)
{
    Gtk::FileChooserDialog dialog("Export graph");
    dialog.set_transient_for(*m_pParent);
    dialog.set_action(Gtk::FILE_CHOOSER_ACTION_SAVE);
    dialog.set_do_overwrite_confirmation(true);

    //Add response buttons the the dialog:
    dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    dialog.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);


    //Add filters, so that only certain file types can be selected:
    Gtk::FileFilter filter_any;
    filter_any.set_name("SVG format");
    filter_any.add_pattern("*.svg");
    dialog.add_filter(filter_any);
    
    Gtk::FileFilter filter_pdf;
    filter_pdf.set_name("PDF format");
    filter_pdf.add_pattern("*.pdf");
    dialog.add_filter(filter_pdf);

    if(dialog.run() == Gtk::RESPONSE_OK)
    {
        string fname = dialog.get_filename();
        // ignoring grid
        g_object_set(m_Grid, "visibility", GOO_CANVAS_ITEM_HIDDEN, NULL);
        
        // finding boundries 
        Goocanvas::Bounds bc(-1,-1,-1,-1);
        /*
        for(int i=0; i<root->get_n_children(); i++)
        {
            Glib::RefPtr<Goocanvas::Item> item = m_Canvas->get_item(root->get_child(i));
            if(item && (item->property_visibility().get_value() != Goocanvas::ITEM_HIDDEN))
            {
                Goocanvas::Bounds bi = item->get_bounds();
                if((bc.get_x1()<0) || (bc.get_x1()>bi.get_x1()))
                    bc.set_x1(bi.get_x1());
       
                if((bc.get_x2()<0) || (bc.get_x2()<bi.get_x2()))
                    bc.set_x2(bi.get_x2());

                if((bc.get_y1()<0) || (bc.get_y1()>bi.get_y1()))
                    bc.set_y1(bi.get_y1());

                if((bc.get_y2()<0) || (bc.get_y2()<bi.get_y2()))
                    bc.set_y2(bi.get_y2());
            }
        }        
        //printf("(%f, %f) (%f, %f)\n", bc.get_x1(), bc.get_y1(),bc.get_x2(),bc.get_y2());
        */

        // Rendering entire the graph
        m_Canvas->get_bounds(bc);
        int w = (int) ceil(bc.get_x2() - bc.get_x1());
        int h = (int) ceil(bc.get_y2() - bc.get_y1());
        Cairo::RefPtr<Cairo::Context> ctx;
        Cairo::RefPtr<Cairo::SvgSurface> svg = Cairo::SvgSurface::create(fname.c_str(), w, h);
        Cairo::RefPtr<Cairo::PdfSurface> pdf = Cairo::PdfSurface::create(fname.c_str(), w, h);
        if(dialog.get_filter()->get_name() == "SVG format")
            ctx = Cairo::Context::create(svg);
        else
            ctx = Cairo::Context::create(pdf);
        ctx->set_source_rgb(1, 1, 1);
        ctx->fill();
        ctx->set_source_rgb(0, 0, 0);
        //m_Canvas->render(ctx, bc);
        m_Canvas->render(ctx);
        ctx->show_page();

        // putting back grid 
         if(m_showGrid)
            g_object_set(m_Grid, "visibility", GOO_CANVAS_ITEM_VISIBLE, NULL);
    }

    return true;
}

void ApplicationWindow::onDelete(void)
{
    ostringstream msg;
    msg<<getApplicationName()<<" has been modified.";
    Gtk::MessageDialog dialog("What's done cannot be undone!", false, Gtk::MESSAGE_QUESTION,  Gtk::BUTTONS_NONE);
    dialog.set_secondary_text("Do you still want to delete them?");
    dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_NO);
    dialog.add_button("Delete", Gtk::RESPONSE_YES);
 
    int response = dialog.run();
    if(response != Gtk::RESPONSE_YES)
        return;

    // switch to application propoerty window 
    Glib::RefPtr<Gtk::ToggleAction> act;
    act = Glib::RefPtr<Gtk::ToggleAction>::cast_dynamic(
            m_pParent->m_refActionGroup->get_action("WindowProperty"));            
    if(act && act->get_active())
    {
        if(m_HPaned.get_child2())
            m_HPaned.remove(*m_HPaned.get_child2());  
        appPropertyWindow->update(manager.getKnowledgeBase()->getApplication());
        m_HPaned.add2(*appPropertyWindow);
        m_HPaned.show_all();
    }
       
    deleteSelectedArrows();
    
    while(countSelected())
        for(int i=0; i<root->get_n_children(); i++)
        {
            Glib::RefPtr<ExternalPortModel> extPort = Glib::RefPtr<ExternalPortModel>::cast_dynamic(root->get_child(i));
            if(extPort && extPort->getSelected()) 
            {
                int id = root->find_child(extPort);
                if(id != -1)
                    root->remove_child(id);
                extPort->setArrowsSelected(true);
                // removing all connected arrows
                deleteSelectedArrows();
                extPort.clear();
            }        

            Glib::RefPtr<ModuleModel> child = Glib::RefPtr<ModuleModel>::cast_dynamic(root->get_child(i));
            if(child && child->getSelected()) 
            {
                int id = root->find_child(child);
                if(id != -1)               
                    root->remove_child(id); 
                if(modPropertyWindow->getModule() == child->getModule())
                    modPropertyWindow->release();
                child->setArrowsSelected(true);
                // removing all connected arrows
                deleteSelectedArrows();
                child.clear();
            } 

            Glib::RefPtr<ApplicationModel> childApp = Glib::RefPtr<ApplicationModel>::cast_dynamic(root->get_child(i));
            if(childApp && childApp->getSelected()) 
            {
                int id = root->find_child(childApp);
                if(id != -1)               
                    root->remove_child(id); 
                if(appPropertyWindow->getApplication() == childApp->getApplication())
                    appPropertyWindow->release();
                childApp->setArrowsSelected(true);
                // removing all connected arrows
                deleteSelectedArrows(); 
                childApp->releaseApplication();
                childApp.clear();
            }        
        }
    m_bModified = true;
}

void ApplicationWindow::onCopy(void)
{
    copiedItems.clear();   
    for(int i=0; i<root->get_n_children(); i++)
    {
      Glib::RefPtr<ModuleModel> model = Glib::RefPtr<ModuleModel>::cast_dynamic(root->get_child(i));
      if(model && model->getSelected())
        copiedItems.push_back(model.operator->());
    }
    
    m_pParent->m_refActionGroup->get_action("EditPaste")->set_sensitive(copiedItems.size()>0);
}

void ApplicationWindow::onRotateRight()
{
    for(int i=0; i<root->get_n_children(); i++)
    {
        Glib::RefPtr<ModuleModel> model = Glib::RefPtr<ModuleModel>::cast_dynamic(root->get_child(i));
        if(model && model->getSelected())
        {
            Goocanvas::Bounds bd = m_Canvas->get_item(model)->get_bounds();
            double xorg = (bd.get_x2()-bd.get_x1())/2.0;
            double yorg = (bd.get_y2()-bd.get_y1())/2.0;
            m_Canvas->get_item(model)->rotate(90, xorg, yorg);
            model->updateArrowCoordination();
        }
    }
}

void ApplicationWindow::onRotateLeft()
{
    for(int i=0; i<root->get_n_children(); i++)
    {
        Glib::RefPtr<ModuleModel> model = Glib::RefPtr<ModuleModel>::cast_dynamic(root->get_child(i));
        if(model && model->getSelected())
        {
            Goocanvas::Bounds bd = m_Canvas->get_item(model)->get_bounds();
            double xorg = (bd.get_x2()-bd.get_x1())/2.0;
            double yorg = (bd.get_y2()-bd.get_y1())/2.0;
            m_Canvas->get_item(model)->rotate(-90, xorg, yorg);
            model->updateArrowCoordination();
        }
    }
}

void ApplicationWindow::onPaste(void)
{
    Application* application = manager.getKnowledgeBase()->getApplication();            
    if(!application)
        return;

    int offset = 0;
    std::vector<ModuleModel*>::iterator itr;
    for(itr=copiedItems.begin(); itr<copiedItems.end(); itr++)
    {
        ModuleInterface imod((*itr)->getModule());
        ModuleInterface* pIMod = NULL;
        for(int i=0; i<application->imoduleCount(); i++)
            if(strcmp(application->getImoduleAt(i).getTag(), (*itr)->getModule()->getLabel()) == 0)
            {
                pIMod = &(application->getImoduleAt(i));
                break;
            }
        if(pIMod)
            imod.setParam(pIMod->getParam());

        Module* module = manager.getKnowledgeBase()->addIModuleToApplication(application, imod, true);
        if(module)
        {
            ModuleInterface* pIModule = NULL;
            // updating prefix of imoduld and module
            for(int i=0; i<application->imoduleCount(); i++)
                if(strcmp(application->getImoduleAt(i).getTag(), module->getLabel()) == 0)
                {
                    pIModule = &(application->getImoduleAt(i));
                    pIModule->setPrefix(string(string("/")+module->getLabel()).c_str());
                    break;
                }                    
            module->setPrefix(string(string("/")+module->getLabel()).c_str());

            // copying arguments
            for(int i=0; i<(*itr)->getModule()->argumentCount(); i++)
            {
                for(int j=0; j<module->argumentCount(); j++)
                    if(string(module->getArgumentAt(j).getParam()) ==
                        string((*itr)->getModule()->getArgumentAt(i).getParam()))                           
                            module->getArgumentAt(j).setValue((*itr)->getModule()->getArgumentAt(i).getValue());
            }
            Glib::RefPtr<ModuleModel> mod = 
                                     ModuleModel::create(this, module);
            root->add_child(mod);
            Goocanvas::Bounds bd = m_Canvas->get_item(mod)->get_bounds();
            m_Canvas->get_item(mod)->translate(_x+offset - bd.get_x1(), _y+offset - bd.get_y1());
            mod->snapToGrid();
            offset += 20;
        }
    }
    
    m_bModified = true;
}

void ApplicationWindow::onMenuInsertSrcPort()
{
    Glib::RefPtr<ExternalPortModel> port = 
        ExternalPortModel::create(this, OUTPUTD, "Source");
    root->add_child(port);
    port->set_property("x", _x);
    port->set_property("y", _y);
    port->snapToGrid();
    m_bModified = true;
}

void ApplicationWindow::onMenuInsertDestPort()
{
    Glib::RefPtr<ExternalPortModel> port = 
        ExternalPortModel::create(this, INPUTD, "Destination");
    root->add_child(port);
    port->set_property("x", _x);
    port->set_property("y", _y);
    port->snapToGrid();
    m_bModified = true;
}

void ApplicationWindow::onMenuWindowProperty(bool active)
{
   if(!active)
   {
        if(m_HPaned.get_child2())
            m_HPaned.remove(*m_HPaned.get_child2());
   }     
   else   
        m_HPaned.add2(*appPropertyWindow);

    m_HPaned.show_all();
}

void ApplicationWindow::onViewLabel(bool label)
{
    m_showLabel = label;
    for(int j=0; j<root->get_n_children(); j++)
    {
        Glib::RefPtr<ArrowModel> arw = Glib::RefPtr<ArrowModel>::cast_dynamic(root->get_child(j));
        if(arw)
            arw->showLabel(m_showLabel);
    }
}


void ApplicationWindow::deleteSelectedArrows(void)
{
    for(int k=0; k<root->get_n_children(); k++)
        for(int j=0; j<root->get_n_children(); j++)
        {
            Glib::RefPtr<ArrowModel> arw = Glib::RefPtr<ArrowModel>::cast_dynamic(root->get_child(j));
            if(arw && arw->getSelected()) 
            {
                int id = root->find_child(arw);
                if(id != -1)               
                    root->remove_child(id);
                if(conPropertyWindow->getArrow() == arw)    
                    conPropertyWindow->release();
                arw.clear();
            }        
        } 
    m_bModified = true;        
}

void ApplicationWindow::releaseApplication(void)
{
    //First, delteing all arrows  
    for(int k=0; k<root->get_n_children(); k++)
        for(int j=0; j<root->get_n_children(); j++)
        {
            Glib::RefPtr<ArrowModel> arw = Glib::RefPtr<ArrowModel>::cast_dynamic(root->get_child(j));
            if(arw) 
            {
                int id = root->find_child(arw);
                if(id != -1)
                    root->remove_child(id);
                 arw.clear();
            }        
            
            Glib::RefPtr<ApplicationModel> childApp = Glib::RefPtr<ApplicationModel>::cast_dynamic(root->get_child(j));
            if(childApp)
                childApp->releaseApplication();
        }
}

/*
void ApplicationWindow::onError(void) 
{
    GDK_THREADS_ENTER();
    reportErrors();
    GDK_THREADS_LEAVE();
}
*/

void ApplicationWindow::reportErrors(void)
{
/*
    ErrorLogger* logger  = ErrorLogger::Instance(); 
    if(logger->errorCount() || logger->warningCount())
    {
        const char* err;
        while((err=logger->getLastError()))
        {
            ostringstream msg;
            msg<<"("<<getApplicationName()<<") "<<err; 
            m_pParent->m_refMessageList->addError(msg.str().c_str());
        }

        while((err=logger->getLastWarning()))
        {
            ostringstream msg;
            msg<<"("<<getApplicationName()<<") "<<err; 
            m_pParent->m_refMessageList->addWarning(msg.str().c_str());
        }
    }
 */
}

void ApplicationWindow::on_item_created(const Glib::RefPtr<Goocanvas::Item>& item, 
                                        const Glib::RefPtr<Goocanvas::ItemModel>& model)
{
 
    Glib::RefPtr<Goocanvas::Group> group = Glib::RefPtr<Goocanvas::Group>::cast_dynamic(item) ;
    if(group) return;

    item->signal_button_press_event().connect(sigc::mem_fun(*this, &ApplicationWindow::on_item_button_press_event)) ;
    item->signal_button_release_event().connect(sigc::mem_fun(*this, &ApplicationWindow::on_item_button_release_event)) ;
    item->signal_motion_notify_event().connect(sigc::mem_fun(*this, &ApplicationWindow::on_item_motion_notify_event));

    item->signal_enter_notify_event().connect(sigc::mem_fun(*this, &ApplicationWindow::on_item_enter_notify_event));
    item->signal_leave_notify_event().connect(sigc::mem_fun(*this, &ApplicationWindow::on_item_leave_notify_event));

    /*
    // We need only mouse over event on PortModels or ArrowModels 
    if((Glib::RefPtr<ArrowModel>::cast_dynamic(item->get_model())) ||
      (Glib::RefPtr<MidpointModel>::cast_dynamic(item->get_model())) || 
      (Glib::RefPtr<LabelModel>::cast_dynamic(item->get_model())) )
    {  
        item->signal_enter_notify_event().connect(sigc::mem_fun(*this, &ApplicationWindow::on_item_enter_notify_event));
        item->signal_leave_notify_event().connect(sigc::mem_fun(*this, &ApplicationWindow::on_item_leave_notify_event));
    }

    if(Glib::RefPtr<Goocanvas::PolylineModel>::cast_dynamic(item->get_model()) )
    {
        printf("%d\n", __LINE__);
    }
    */
}


bool ApplicationWindow::on_item_button_press_event(const Glib::RefPtr<Goocanvas::Item>& item, 
                                                   GdkEventButton* event)
{

    // on double click, first deselect all items 
    // if it is not an "CTRL + Duble click"
    if(event->type == GDK_2BUTTON_PRESS)
    {
        // switch to application propoerty window 
        Glib::RefPtr<Gtk::ToggleAction> act;
        act = Glib::RefPtr<Gtk::ToggleAction>::cast_dynamic(
                m_pParent->m_refActionGroup->get_action("WindowProperty"));            
        if(act && act->get_active())
        {
            if(m_HPaned.get_child2())
                m_HPaned.remove(*m_HPaned.get_child2());  
            appPropertyWindow->update(manager.getKnowledgeBase()->getApplication());
            m_HPaned.add2(*appPropertyWindow);
            m_HPaned.show_all();
        }

        if(!(event->state  & GDK_CONTROL_MASK))
        {
           for(int i=0; i<root->get_n_children(); i++)
           {
                Glib::RefPtr<ApplicationModel> app = Glib::RefPtr<ApplicationModel>::cast_dynamic(root->get_child(i));
                if(app) app->setSelected(false);                    
                Glib::RefPtr<ModuleModel> child = Glib::RefPtr<ModuleModel>::cast_dynamic(root->get_child(i));
                if(child) child->setSelected(false);                    
                Glib::RefPtr<ArrowModel> arrow = Glib::RefPtr<ArrowModel>::cast_dynamic(root->get_child(i));
                if(arrow) arrow->setSelected(false);
                Glib::RefPtr<ExternalPortModel> extport = Glib::RefPtr<ExternalPortModel>::cast_dynamic(root->get_child(i));
                if(extport) extport->setSelected(false);
           }
           m_pParent->m_refActionGroup->get_action("EditDelete")->set_sensitive(countSelected()>0);
           m_pParent->m_refActionGroup->get_action("EditCopy")->set_sensitive(countSelected()>0);
           m_pParent->m_refActionGroup->get_action("EditPaste")->set_sensitive(copiedItems.size()>0);
        }
    }

    if(!item)
    {
        if(event->button == 3)
        {
            _x = event->x;
            _y = event->y;
            m_pParent->m_refActionGroup->get_action("EditDelete")->set_sensitive(countSelected()>0);
            m_pParent->m_refActionGroup->get_action("EditCopy")->set_sensitive(countSelected()>0);
            m_pParent->m_refActionGroup->get_action("EditPaste")->set_sensitive(copiedItems.size()>0);

            Gtk::Menu* pMenu = dynamic_cast<Gtk::Menu*>(
            m_pParent->m_refUIManager->get_widget("/PopupCanvas"));
            if(pMenu)
                pMenu->popup(event->button, event->time);
        }
        
        if(m_connector)
        {
            int id = root->find_child(m_connector);
            if(id != -1)
                root->remove_child(id);
            m_connector.clear();
        }
        else if(!m_selector && (event->button == 1))
        {
            m_selector = Goocanvas::RectModel::create(event->x, event->y, 1, 1);            
            m_selector->property_line_width().set_value(1.0);
            m_selector->set_property("antialias", Cairo::ANTIALIAS_NONE);
            m_selector->property_stroke_color().set_value("DodgerBlue3");
            m_selector->property_fill_color_rgba().set_value(0x6495ed30);
            GooCanvasLineDash *dash = goo_canvas_line_dash_new (2, 3.0, 3.0);
            g_object_set(m_selector->gobj(), "line-dash", dash, NULL);
            root->add_child(m_selector); 
        }
        return true;
    }       

    // let external port set its position on double clicked + dragging
    Glib::RefPtr<Goocanvas::Item> exParent = item->get_parent();
    if(exParent)
    {       
        // external ports doubleclick event
        Glib::RefPtr<ExternalPortModel> extPort = Glib::RefPtr<ExternalPortModel>::cast_dynamic(exParent->get_model());
        if(extPort)
        {
            // we propaget only double click event 
            if((event->type == GDK_2BUTTON_PRESS) && (extPort->isNested()==false))
            {
                extPort->setSelected(true);
                m_pParent->m_refActionGroup->get_action("EditDelete")->set_sensitive(true);
                m_pParent->m_refActionGroup->get_action("EditCopy")->set_sensitive(true);
                return extPort->onItemButtonPressEvent(item, event);
            }
        }
    }

    // we handle connecting ports
    Glib::RefPtr<PortModel> port = Glib::RefPtr<PortModel>::cast_dynamic(item->get_parent()->get_model());
    if(port)
    {
        // make a new connection 
        if(event->button == 1)
        {
            if(!m_connector && (port->getType() == OUTPUTD))
            {
                sourcePort = port;
                Gdk::Point pt = port->getContactPoint();
                m_connector = Goocanvas::PolylineModel::create(pt.get_x(), pt.get_y(), 
                                                               pt.get_x(), pt.get_y());
                m_connector->property_line_width().set_value(1.0);
                m_connector->property_stroke_color().set_value("black");    
                root->add_child(m_connector); 
            }
            else
            {
                if(m_connector && (port->getType() == INPUTD))
                {
                    Glib::RefPtr<ArrowModel> arrow = ArrowModel::create(this, sourcePort, port);
                    root->add_child(arrow);
                    m_bModified = true;
                }

                int id = root->find_child(m_connector);
                if(id != -1)
                    root->remove_child(id);
                m_connector.clear();

            }
         }
    }

    // let module set its position on dragging
    Glib::RefPtr<Goocanvas::Item> parent = item->get_parent();
    if(parent)
    {   
         Glib::RefPtr<ModuleModel> mod = Glib::RefPtr<ModuleModel>::cast_dynamic(parent->get_model());
         if(mod)
         {
            if(event->type == GDK_2BUTTON_PRESS)
            {
                mod->setSelected(true);
                m_pParent->m_refActionGroup->get_action("EditDelete")->set_sensitive(true);
                m_pParent->m_refActionGroup->get_action("EditCopy")->set_sensitive(true);
            }
            return mod->onItemButtonPressEvent(item, event);
         }

         Glib::RefPtr<ApplicationModel> app = Glib::RefPtr<ApplicationModel>::cast_dynamic(parent->get_model());
         if(app)
         {
            if(event->type == GDK_2BUTTON_PRESS)
            {
                app->setSelected(true);
                m_pParent->m_refActionGroup->get_action("EditDelete")->set_sensitive(true);
                m_pParent->m_refActionGroup->get_action("EditCopy")->set_sensitive(true);
                // switch to application property
                Glib::RefPtr<Gtk::ToggleAction> act;
                act = Glib::RefPtr<Gtk::ToggleAction>::cast_dynamic(
                            m_pParent->m_refActionGroup->get_action("WindowProperty"));            
                if(act && act->get_active())
                {
                    if(m_HPaned.get_child2())
                        m_HPaned.remove(*m_HPaned.get_child2());  
                    appPropertyWindow->update(app->getApplication());
                    m_HPaned.add2(*appPropertyWindow);
                    m_HPaned.show_all();
                }    
            }
            return app->onItemButtonPressEvent(item, event);
         }


        Glib::RefPtr<ExternalPortModel> extport = Glib::RefPtr<ExternalPortModel>::cast_dynamic(parent->get_model());
        if(extport) 
        {
            if((event->type == GDK_2BUTTON_PRESS) && (extport->isNested()==false))
            {
                m_pParent->m_refActionGroup->get_action("EditDelete")->set_sensitive(true);
                //m_pParent->m_refActionGroup->get_action("EditCopy")->set_sensitive(true);
                extport->setSelected(true);
            }

            else
                return extport->onItemButtonPressEvent(item, event);
        }
    }
    
    Glib::RefPtr<ArrowModel> arrow = Glib::RefPtr<ArrowModel>::cast_dynamic(item->get_model());    
    if(arrow)
    {
        if(event->type == GDK_2BUTTON_PRESS)
        {
            arrow->setSelected(true);
            m_pParent->m_refActionGroup->get_action("EditDelete")->set_sensitive(true);
            //m_pParent->m_refActionGroup->get_action("EditCopy")->set_sensitive(true);

            // switch to connection propoerty window 
            Glib::RefPtr<Gtk::ToggleAction> act;
            act = Glib::RefPtr<Gtk::ToggleAction>::cast_dynamic(
                    m_pParent->m_refActionGroup->get_action("WindowProperty"));            
            if(act && act->get_active())
            {
                if(m_HPaned.get_child2())
                    m_HPaned.remove(*m_HPaned.get_child2());  
                conPropertyWindow->update(arrow);
                m_HPaned.add2(*conPropertyWindow);
                m_HPaned.show_all();
            }
        }
        else
            return arrow->onItemButtonPressEvent(item, event);

        return true;
    }

    Glib::RefPtr<MidpointModel> mid = Glib::RefPtr<MidpointModel>::cast_dynamic(item->get_model());    
    if(mid)
    {
        //if(event->type == GDK_2BUTTON_PRESS)
        //    mid->setSelected(true);
        //else
        return mid->onItemButtonPressEvent(item, event);
    }

    Glib::RefPtr<LabelModel> lab = Glib::RefPtr<LabelModel>::cast_dynamic(item->get_model());    
    if(lab)
    {
        //if(event->type == GDK_2BUTTON_PRESS)
        //    lab->setSelected(true);
        //else
        return lab->onItemButtonPressEvent(item, event);
    }

    return true;
}


int ApplicationWindow::countSelected(void)
{
    int num = 0;
    for(int i=0; i<root->get_n_children(); i++)
    {
        Glib::RefPtr<ApplicationModel> childApp = Glib::RefPtr<ApplicationModel>::cast_dynamic(root->get_child(i));
        if(childApp && childApp->getSelected())
            num++;
        Glib::RefPtr<ExternalPortModel> extPort = Glib::RefPtr<ExternalPortModel>::cast_dynamic(root->get_child(i));
        if(extPort && extPort->getSelected())
            num++;
        Glib::RefPtr<ModuleModel> child = Glib::RefPtr<ModuleModel>::cast_dynamic(root->get_child(i));
        if(child && child->getSelected())
            num++;
        Glib::RefPtr<ArrowModel> arrow = Glib::RefPtr<ArrowModel>::cast_dynamic(root->get_child(i));
        if(arrow && arrow->getSelected())
            num++;
    }
    return num;
}


void ApplicationWindow::setSelected(void)
{
    if(!m_selector)
        return;

    Glib::RefPtr<Gtk::ToggleAction> act;
    act = Glib::RefPtr<Gtk::ToggleAction>::cast_dynamic(
            m_pParent->m_refActionGroup->get_action("WindowProperty"));            

    //deselect all items
    for(int i=0; i<root->get_n_children(); i++)
    {
        Glib::RefPtr<ApplicationModel> app = Glib::RefPtr<ApplicationModel>::cast_dynamic(root->get_child(i));
        if(app) app->setSelected(false);                    
        Glib::RefPtr<ModuleModel> child = Glib::RefPtr<ModuleModel>::cast_dynamic(root->get_child(i));
        if(child) child->setSelected(false);                    
        Glib::RefPtr<ArrowModel> arrow = Glib::RefPtr<ArrowModel>::cast_dynamic(root->get_child(i));
        if(arrow) arrow->setSelected(false);
        Glib::RefPtr<ExternalPortModel> extPort = Glib::RefPtr<ExternalPortModel>::cast_dynamic(root->get_child(i));
        if(extPort) extPort->setSelected(false);

    }

    // select items within area 
    Glib::RefPtr<Goocanvas::Item> selector = m_Canvas->get_item(m_selector);
    std::vector<Glib::RefPtr<Goocanvas::Item> > items = 
        m_Canvas->get_items_in_area(selector->get_bounds(), true, true, true);
    std::vector<Glib::RefPtr<Goocanvas::Item> >::iterator itr;
    int itemCount = 0;
    for(itr=items.begin(); itr != items.end(); itr++)
    {
        Glib::RefPtr<Goocanvas::ItemModel> model = (*itr)->get_model();

        Glib::RefPtr<ApplicationModel> app = Glib::RefPtr<ApplicationModel>::cast_dynamic(model);
        if(app) 
        {
            itemCount++;
            app->setSelected(true);  
            if(itemCount == 1)
            {
                // switch to application property
                if(act && act->get_active())
                {
                    if(m_HPaned.get_child2())
                        m_HPaned.remove(*m_HPaned.get_child2());  
                    appPropertyWindow->update(app->getApplication());
                    m_HPaned.add2(*appPropertyWindow);
                    m_HPaned.show_all();
                }    
            }
        }
 
        Glib::RefPtr<ModuleModel> child = Glib::RefPtr<ModuleModel>::cast_dynamic(model);
        if(child) 
        {
            itemCount++;
            child->setSelected(true);  
            if(itemCount == 1)
            {
                // switch to module property
                if(act && act->get_active())
                {
                    if(m_HPaned.get_child2())
                        m_HPaned.remove(*m_HPaned.get_child2());  
                    modPropertyWindow->update(child->getModule());
                    m_HPaned.add2(*modPropertyWindow);
                    m_HPaned.show_all();
                }
            }
        }
        
        Glib::RefPtr<ArrowModel> arrow = Glib::RefPtr<ArrowModel>::cast_dynamic(model);
        if(arrow) 
        {
            double x1 = selector->get_bounds().get_x1();
            double y1 = selector->get_bounds().get_y1();
            double x2 = selector->get_bounds().get_x2();
            double y2 = selector->get_bounds().get_y2();

            if(arrow->intersect(x1, y1, x2, y1) || 
               arrow->intersect(x1, y1, x1, y2) || 
               arrow->intersect(x2, y1, x2, y2) || 
               arrow->intersect(x1, y2, x2, y2) ||
               arrow->inside(x1, y1, x2, y2) )
            {

                itemCount++;
                arrow->setSelected(true);
                if(itemCount == 1)
                {
                    // switch to connection property
                    if(act && act->get_active())
                    {
                        if(m_HPaned.get_child2())
                            m_HPaned.remove(*m_HPaned.get_child2());  
                        conPropertyWindow->update(arrow);
                        m_HPaned.add2(*conPropertyWindow);
                        m_HPaned.show_all();
                    }
                }
            }
        }
        
        Glib::RefPtr<ExternalPortModel> extPort = Glib::RefPtr<ExternalPortModel>::cast_dynamic(model);
        if(extPort) 
        {
            itemCount+=2;
            extPort->setSelected(true);
        }
    }
    
    // more than one or no item is selected
    if(itemCount != 1)
    {
        // switch to application property
        if(act && act->get_active())
        {
            if(m_HPaned.get_child2())
                m_HPaned.remove(*m_HPaned.get_child2());  
            appPropertyWindow->update(manager.getKnowledgeBase()->getApplication());
            m_HPaned.add2(*appPropertyWindow);
            m_HPaned.show_all();
        }    
    }

    m_pParent->m_refActionGroup->get_action("EditDelete")->set_sensitive(countSelected()>0);
    m_pParent->m_refActionGroup->get_action("EditCopy")->set_sensitive(countSelected()>0);
    m_pParent->m_refActionGroup->get_action("EditPaste")->set_sensitive(copiedItems.size()>0);
}


bool ApplicationWindow::on_item_button_release_event(const Glib::RefPtr<Goocanvas::Item>& item, 
                                                     GdkEventButton* event)
{
    if(m_selector)
    {
        setSelected();
        int id = root->find_child(m_selector);
        if(id != -1)
            root->remove_child(id);
        m_selector.clear();
        return true;
    }

    if(!item)
        return false;


    Glib::RefPtr<Goocanvas::Item> parent = item->get_parent();
    if(parent)
    {   
        Glib::RefPtr<ModuleModel> mod = Glib::RefPtr<ModuleModel>::cast_dynamic(parent->get_model());
        if(mod)
            return mod->onItemButtonReleaseEvent(item, event);

        Glib::RefPtr<ApplicationModel> app = Glib::RefPtr<ApplicationModel>::cast_dynamic(parent->get_model());
        if(app)
            return app->onItemButtonReleaseEvent(item, event);


        Glib::RefPtr<ExternalPortModel> extport = Glib::RefPtr<ExternalPortModel>::cast_dynamic(parent->get_model());
        if(extport) 
            return extport->onItemButtonReleaseEvent(item, event);
    }

    Glib::RefPtr<ArrowModel> arrow = Glib::RefPtr<ArrowModel>::cast_dynamic(item->get_model());    
    if(arrow)
        return arrow->onItemButtonReleaseEvent(item, event);

    Glib::RefPtr<MidpointModel> mid = Glib::RefPtr<MidpointModel>::cast_dynamic(item->get_model());    
    if(mid)
        return mid->onItemButtonReleaseEvent(item, event);

    Glib::RefPtr<LabelModel> lab = Glib::RefPtr<LabelModel>::cast_dynamic(item->get_model());    
    if(lab)
        return lab->onItemButtonReleaseEvent(item, event);

    return true;
}

bool ApplicationWindow::on_item_motion_notify_event(const Glib::RefPtr<Goocanvas::Item>& item, 
                                                    GdkEventMotion* event)
{
    // in connecting mode
    if(m_connector)
    {
        if(!(event->state & GDK_BUTTON1_MASK))
        {
            Goocanvas::Points points = m_connector->property_points().get_value();
            points.set_coordinate(1, (int)event->x_root-2, (int)event->y_root-2);
            m_connector->property_points().set_value(points);
        }
        else
        {
            int id = root->find_child(m_connector);
            if(id != -1)
                root->remove_child(id);
            m_connector.clear();
           
        }
    }
    
    if(m_connector)
        return true;

    if(m_selector)
    {
        double x = event->x;
        double y = event->y;
        if(item)
            m_Canvas->convert_from_item_space(item, x, y);
        double w = x - m_selector->property_x().get_value();
        double h = y - m_selector->property_y().get_value();
        if((w>0) && (h>0))
        {
            m_selector->property_width().set_value(w);
            m_selector->property_height().set_value(h);
        }
       return true;
    }

    if(!item)
        return false;

    Glib::RefPtr<Goocanvas::Item> parent = item->get_parent();    
    if(parent)
    {   
         Glib::RefPtr<ModuleModel> mod = Glib::RefPtr<ModuleModel>::cast_dynamic(parent->get_model());
         if(mod)
            return mod->onItemMotionNotifyEvent(item, event);
         Glib::RefPtr<ApplicationModel> app = Glib::RefPtr<ApplicationModel>::cast_dynamic(parent->get_model());
         if(app)
            return app->onItemMotionNotifyEvent(item, event);
        Glib::RefPtr<ExternalPortModel> extport = Glib::RefPtr<ExternalPortModel>::cast_dynamic(parent->get_model());
        if(extport) 
            return extport->onItemMotionNotifyEvent(item, event);
    }

    Glib::RefPtr<ArrowModel> arrow = Glib::RefPtr<ArrowModel>::cast_dynamic(item->get_model());    
    if(arrow)
    {
        arrow->onItemMotionNotifyEvent(item, event);
    }

    Glib::RefPtr<MidpointModel> mid = Glib::RefPtr<MidpointModel>::cast_dynamic(item->get_model());    
    if(mid)
        return mid->onItemMotionNotifyEvent(item, event);

    Glib::RefPtr<LabelModel> lab = Glib::RefPtr<LabelModel>::cast_dynamic(item->get_model());    
    if(lab)
        return lab->onItemMotionNotifyEvent(item, event);

    return true;
}

bool ApplicationWindow::on_item_enter_notify_event(const Glib::RefPtr<Goocanvas::Item>& item, 
                                GdkEventCrossing* event)
{
    if(!item)
        return false;

    Glib::RefPtr<Goocanvas::Item> parent = item->get_parent();    
    if(parent)
    {   
         Glib::RefPtr<ModuleModel> mod = Glib::RefPtr<ModuleModel>::cast_dynamic(parent->get_model());
         if(mod)
            return mod->onItemEnterNotify(item, event);
         Glib::RefPtr<ApplicationModel> app = Glib::RefPtr<ApplicationModel>::cast_dynamic(parent->get_model());
         if(app)
            return app->onItemEnterNotify(item, event);
         Glib::RefPtr<PortModel> port = Glib::RefPtr<PortModel>::cast_dynamic(parent->get_model());
         if(port)
            return port->onItemEnterNotify(item, event);
    }

    Glib::RefPtr<ArrowModel> arrow = Glib::RefPtr<ArrowModel>::cast_dynamic(item->get_model());    
    if(arrow)
        return arrow->onItemEnterNotify(item, event);

    Glib::RefPtr<MidpointModel> mid = Glib::RefPtr<MidpointModel>::cast_dynamic(item->get_model());    
    if(mid)
        return mid->onItemEnterNotify(item, event);

    Glib::RefPtr<LabelModel> lab = Glib::RefPtr<LabelModel>::cast_dynamic(item->get_model());    
    if(lab)
        return lab->onItemEnterNotify(item, event);

    return true;
}

bool ApplicationWindow::on_item_leave_notify_event(const Glib::RefPtr<Goocanvas::Item>& item, 
                                GdkEventCrossing* event)
{
    if(!item)
        return false;

    Glib::RefPtr<Goocanvas::Item> parent = item->get_parent();    
    if(parent)
    {   
         Glib::RefPtr<ModuleModel> mod = Glib::RefPtr<ModuleModel>::cast_dynamic(parent->get_model());
         if(mod)
            return mod->onItemLeaveNotify(item, event);
         Glib::RefPtr<ApplicationModel> app = Glib::RefPtr<ApplicationModel>::cast_dynamic(parent->get_model());
         if(app)
            return app->onItemLeaveNotify(item, event);
         Glib::RefPtr<PortModel> port = Glib::RefPtr<PortModel>::cast_dynamic(parent->get_model());
         if(port)
            return port->onItemLeaveNotify(item, event);
    }

    Glib::RefPtr<ArrowModel> arrow = Glib::RefPtr<ArrowModel>::cast_dynamic(item->get_model());    
    if(arrow)
        return arrow->onItemLeaveNotify(item, event);

    Glib::RefPtr<MidpointModel> mid = Glib::RefPtr<MidpointModel>::cast_dynamic(item->get_model());    
    if(mid)
        return mid->onItemLeaveNotify(item, event);

    Glib::RefPtr<LabelModel> lab = Glib::RefPtr<LabelModel>::cast_dynamic(item->get_model());    
    if(lab)
        return lab->onItemLeaveNotify(item, event);

    return true;
}

void ApplicationWindow::on_size_allocate(Gtk::Allocation& allocation)
{
    if(allocation.get_width() != dummy_w)
    {
        dummy_w = allocation.get_width();
        m_HPaned.set_position((int)(allocation.get_width()-200));
    }

    /*
    if(m_HPaned.get_position()<50)
        m_HPaned.set_position(50);

    if(m_HPaned.get_position()>(allocation.get_height()-120))
        m_HPaned.set_position(allocation.get_height()-120);    
    */

    double h = allocation.get_height();
    double w = allocation.get_width();
    if(m_Canvas)
    {
        Goocanvas::Bounds b;
        m_Canvas->get_bounds(b);
        w = (w < b.get_x2()) ? b.get_x2() : w; 
        h = (h < b.get_y2()) ? b.get_y2() : h;
        m_Canvas->set_bounds(0,0, w, h); 
    }

    if(m_Grid)
    {
        g_object_set(m_Grid, "width", w, NULL);
        g_object_set(m_Grid, "height", h, NULL);
    }
    
    Gtk::Frame::on_size_allocate(allocation);
}

bool ApplicationWindow::on_key_press_event(GdkEventKey* event)
{
    if(!event)
        return false;

    switch (event->keyval) {
    case GDK_Delete:
            onDelete();
            break;
    default:
        return true;
        //printf("unknown key pressed!\n");
    }
    return true;
}

void ApplicationWindow::prepareManagerFrom(Manager* lazy, 
                                           const char* szAppName)
{
    
    // making manager from lazy manager
    KnowledgeBase* lazy_kb = lazy->getKnowledgeBase();

    ModulePContainer mods =  lazy_kb->getModules();
    for(ModulePIterator itr=mods.begin(); itr!=mods.end(); itr++)
        manager.getKnowledgeBase()->addModule((*itr));

    ResourcePContainer res =  lazy_kb->getResources();
    for(ResourcePIterator itr=res.begin(); itr!=res.end(); itr++)
        manager.getKnowledgeBase()->addResource((*itr));

    ApplicaitonPContainer apps =  lazy_kb->getApplications();
    for(ApplicationPIterator itr=apps.begin(); itr!=apps.end(); itr++)
        manager.getKnowledgeBase()->addApplication((*itr));

    // loading application
    if(manager.loadApplication(szAppName))
       updateApplicationWindow();
    
    reportErrors();
}


void ApplicationWindow::updateApplicationWindow(void)
{
    Application* mainApplication = manager.getKnowledgeBase()->getApplication();
    ModulePContainer modules = manager.getKnowledgeBase()->getModules(mainApplication);    
    CnnContainer connections = manager.getKnowledgeBase()->getConnections(mainApplication);
    ApplicaitonPContainer applications = manager.getKnowledgeBase()->getApplications(mainApplication);

    int index = 0;
    ApplicationPIterator appItr;
    Glib::RefPtr<ApplicationModel> app;
    for(appItr=applications.begin(); appItr!=applications.end(); appItr++)    
    { 
        Application* application = (*appItr);
        app = ApplicationModel::create(this, application);
        root->add_child(app);
        Glib::RefPtr<Goocanvas::Item> item = m_Canvas->get_item(app);
        item->translate(0,0);
        app->snapToGrid();
        app->updateChildItems();
        //Goocanvas::Bounds bd = item->get_bounds();
        //printf("%s : x:%.2f, y:%.2f\n", module->getName(), bd.get_x1(), bd.get_y1());
        if(application->getModelBase().points.size()>0)
        { 
            item->translate(application->getModelBase().points[0].x, 
                            application->getModelBase().points[0].y);
        }    
        else
        {
            item->translate(index%900+10, 
                            (index/900)*100+10);
            index += 300;
        }
        app->snapToGrid();               
    }  
 
    ModulePIterator itr;
    Glib::RefPtr<ModuleModel> mod;
    for(itr=modules.begin(); itr!=modules.end(); itr++)    
    { 
        Module* module = (*itr);
        mod = ModuleModel::create(this, module);
        root->add_child(mod);
        Glib::RefPtr<Goocanvas::Item> item = m_Canvas->get_item(mod);
        Goocanvas::Bounds bd = item->get_bounds();
        //printf("%s : x:%.2f, y:%.2f\n", module->getName(), bd.get_x1(), bd.get_y1());
        if(module->getModelBase().points.size()>0)
        { 
            item->translate(module->getModelBase().points[0].x - bd.get_x1(), 
                            module->getModelBase().points[0].y - bd.get_y1());
        }    
        else
        {
            item->translate(index%900+10 - bd.get_x1(), 
                            (index/900)*100+10 - bd.get_y1());
            index += 300;
        }
        mod->snapToGrid();
    }  
    
    index = (index/900)*100+50;
    CnnIterator citr;
    ModulePContainer allModules = manager.getKnowledgeBase()->getSelModules();
    for(citr=connections.begin(); citr<connections.end(); citr++) 
    {
        Connection baseCon = *citr;
        GraphicModel model = baseCon.getModelBase();
        InputData* input = NULL;
        OutputData* output = NULL;
        Glib::RefPtr<PortModel> source, dest;
        findInputOutputData((*citr), allModules, input, output);    
        if(output)
            source = findModelFromOutput(output);
        else
        {
            Glib::RefPtr<ExternalPortModel> extPort;
            bool bExist = false;
            for(int i=0; i<root->get_n_children(); i++)
            {
                extPort = Glib::RefPtr<ExternalPortModel>::cast_dynamic(root->get_child(i));
                if(extPort && compareString(extPort->getPort(), baseCon.from()))
                {
                    source = extPort;
                    bExist = true;
                    break;
                }    
            }
            if(!bExist)
            {
                source = ExternalPortModel::create(this, OUTPUTD, (*citr).from());
                root->add_child(source);

                if(model.points.size() > 1)
                {
                    source->set_property("x", model.points[1].x - Glib::RefPtr<ExternalPortModel>::cast_dynamic(source)->getWidth());
                    source->set_property("y", model.points[1].y - Glib::RefPtr<ExternalPortModel>::cast_dynamic(source)->getHeight()/2.0); 
                }
                else
                {
                    source->set_property("x", 10);
                    source->set_property("y", index);
                }
            } 
            index+=40;
        }
        if(input)
           dest = findModelFromInput(input);       
        else
        {
            Glib::RefPtr<ExternalPortModel> extPort;
            bool bExist = false;
            for(int i=0; i<root->get_n_children(); i++)
            {
                extPort = Glib::RefPtr<ExternalPortModel>::cast_dynamic(root->get_child(i));
                if(extPort && compareString(extPort->getPort(), baseCon.to()))
                {
                    dest = extPort;
                    bExist = true;
                    break;
                }    
            }
            if(!bExist) 
            {
                dest = ExternalPortModel::create(this, INPUTD, (*citr).to());
                root->add_child(dest);
                size_t size = model.points.size();
                if(size > 2)
                {
                    dest->set_property("x", model.points[size-1].x);
                    dest->set_property("y", model.points[size-1].y - 
                                       Glib::RefPtr<ExternalPortModel>::cast_dynamic(dest)->getHeight()/2.0); 

                }
                else
                {
                    dest->set_property("x", 400);
                    dest->set_property("y", index);
                }
            }    
        }

        Glib::RefPtr<ArrowModel> arrow = ArrowModel::create(this, source, dest, (*citr).carrier());
        root->add_child(arrow);
        int size = model.points.size();
        for(int i=2; i<size-1; i++)
            arrow->addMidPoint(model.points[i].x, model.points[i].y, i-2);            
        arrow->setSelected(false);
        if(size)
            arrow->setLabelPosition(model.points[0].x, model.points[0].y);
    }
    
    // update canvas and grid size
    double max_w = 0;
    double max_h = 0;
    Goocanvas::Bounds bc;
    for(int i=0; i<root->get_n_children(); i++)
    {
        Glib::RefPtr<Goocanvas::Item> item = m_Canvas->get_item(root->get_child(i));
        bc = item->get_bounds();
        max_w = (max_w < bc.get_x2()) ? bc.get_x2() : max_w;
        max_h = (max_h < bc.get_y2()) ? bc.get_y2() : max_h;
    } 
    m_Canvas->get_bounds(bc);    
    max_w = (max_w < bc.get_x2()) ? bc.get_x2() : max_w;
    max_h = (max_h < bc.get_y2()) ? bc.get_y2() : max_h;
    m_Canvas->set_bounds(0,0, max_w, max_h);            
    g_object_set(m_Grid, "width", max_w, NULL);
    g_object_set(m_Grid, "height", max_h, NULL);

    // switch to application property window
    Glib::RefPtr<Gtk::ToggleAction> act;
    act = Glib::RefPtr<Gtk::ToggleAction>::cast_dynamic(
            m_pParent->m_refActionGroup->get_action("WindowProperty"));            
    if(act && act->get_active())
        m_HPaned.add2(*appPropertyWindow);
    appPropertyWindow->update(manager.getKnowledgeBase()->getApplication());


    /*
    // TESTING 
    Application* subApp =  manager.getKnowledgeBase()->getApplication("EyesViewer");
    if(subApp)
    {
        Glib::RefPtr<ApplicationModel> app = ApplicationModel::create(this, subApp);
        root->add_child(app);
        Glib::RefPtr<Goocanvas::Item> item = m_Canvas->get_item(app);
        Goocanvas::Bounds bd = item->get_bounds();
        item->translate(10 - bd.get_x1(), 10 - bd.get_y1());
    }
    */
} 

Glib::RefPtr<PortModel> ApplicationWindow::findModelFromOutput(OutputData* output)
{
    for(int i=0; i<root->get_n_children(); i++)
    {
        // if is an application model
        Glib::RefPtr<ApplicationModel> application = Glib::RefPtr<ApplicationModel>::cast_dynamic(root->get_child(i));
        if(application)
        {
            for(int k=0; k<application->get_n_children(); k++)
            {
                Glib::RefPtr<ModuleModel> module = Glib::RefPtr<ModuleModel>::cast_dynamic(application->get_child(k));
                if(module)
                {
                    for(int j=0; j<module->get_n_children(); j++)
                    {
                        Glib::RefPtr<InternalPortModel> intPort = Glib::RefPtr<InternalPortModel>::cast_dynamic(module->get_child(j));
                        if(intPort && (intPort->getOutput() == output))
                            return intPort;
                    }
                }
            }
        }

        // if is an module model
        Glib::RefPtr<ModuleModel> module = Glib::RefPtr<ModuleModel>::cast_dynamic(root->get_child(i));
        if(module)
        {
            for(int j=0; j<module->get_n_children(); j++)
            {
                Glib::RefPtr<InternalPortModel> intPort = Glib::RefPtr<InternalPortModel>::cast_dynamic(module->get_child(j));
                if(intPort && (intPort->getOutput() == output))
                    return intPort;
            }
        }
    }
    return Glib::RefPtr<PortModel>(NULL);
}


Glib::RefPtr<PortModel> ApplicationWindow::findModelFromInput(InputData* input)
{
    for(int i=0; i<root->get_n_children(); i++)
    {
        // if is an application model
        Glib::RefPtr<ApplicationModel> application = Glib::RefPtr<ApplicationModel>::cast_dynamic(root->get_child(i));
        if(application)
        {
            for(int k=0; k<application->get_n_children(); k++)
            {
                Glib::RefPtr<ModuleModel> module = Glib::RefPtr<ModuleModel>::cast_dynamic(application->get_child(k));
                if(module)
                {
                    for(int j=0; j<module->get_n_children(); j++)
                    {
                        Glib::RefPtr<InternalPortModel> intPort = Glib::RefPtr<InternalPortModel>::cast_dynamic(module->get_child(j));
                        if(intPort && (intPort->getInput() == input))
                            return intPort;
                    }
                }
            }
        }

        Glib::RefPtr<ModuleModel> module = Glib::RefPtr<ModuleModel>::cast_dynamic(root->get_child(i));
        if(module)
        {
            for(int j=0; j<root->get_child(i)->get_n_children(); j++)
            {
                Glib::RefPtr<InternalPortModel> intPort = Glib::RefPtr<InternalPortModel>::cast_dynamic(root->get_child(i)->get_child(j));
                if(intPort && (intPort->getInput() == input))
                    return intPort;
            }
        }
    }
    return Glib::RefPtr<PortModel>(NULL);
}

void ApplicationWindow::findInputOutputData(Connection& cnn,  ModulePContainer &modules,
                                            InputData* &input_, OutputData* &output_)
{
    input_ = NULL;
    output_ = NULL;
    string strTo = cnn.to();
    string strFrom = cnn.from();

    ModulePIterator itr;
    for(itr=modules.begin(); itr!=modules.end(); itr++)    
    { 
        Module* module = (*itr);
        for(int i=0; i<module->inputCount(); i++)
        {
            InputData &input = module->getInputAt(i);
            string strInput = string(module->getPrefix()) + string(input.getPort());
            if(strTo == strInput)
            {
                input_ = &input;
                break;
            }
        }

        for(int i=0; i<module->outputCount(); i++)
        {
            OutputData &output = module->getOutputAt(i);
            string strOutput = string(module->getPrefix()) + string(output.getPort());
            if(strFrom == strOutput)
            {
                output_ = &output;
                break;
            }
        }
    }
}

/*
InputData* ApplicationWindow::findInputData(Connection* cnn,  ModulePContainer &modules)
{
    string strTo = cnn->to();
    ModulePIterator itr;
    for(itr=modules.begin(); itr!=modules.end(); itr++)    
    { 
        Module* module = (*itr);
        for(int i=0; i<module->inputCount(); i++)
        {
            InputData &input = module->getInputAt(i);
            string strFrom = string(module->getPrefix()) + string(input.getPort());
            if(strTo == strFrom)
                return &input;
        }
    }
    return NULL;
}
*/

Connection* ApplicationWindow::findConnection( CnnContainer& connections, const char* szPort, bool from)
{
    CnnIterator jtr;
    for(jtr=connections.begin(); jtr!=connections.end(); jtr++)    
    {
        if(from && (string((*jtr).from()) == string(szPort)))
            return &(*jtr);        
        if(!from && (string((*jtr).to()) == string(szPort)))
            return &(*jtr);
    }

    return NULL;
}


void ApplicationWindow::onDragDataReceived(const Glib::RefPtr<Gdk::DragContext>& context, 
                            int x, int y, const Gtk::SelectionData& data, 
                            guint info, guint time)
{
    x+=m_ScrollView.get_hadjustment()->get_value();
    y+=m_ScrollView.get_vadjustment()->get_value();
    if((data.get_length() >= 0) && (data.get_format() == MODULE))
    {
        const guchar* name = data.get_data();
        if(name)
        {
            //Adding new module to the currently loaded application
            ModuleInterface imod((const char*)name);
            Application* application = manager.getKnowledgeBase()->getApplication();            
            if(!application)
                return;

            Module* module = manager.getKnowledgeBase()->addIModuleToApplication(application, imod, true);
            if(module)
            {
                string strPrefix = string("/") + module->getLabel();
                module->setBasePrefix(strPrefix.c_str());
                string strAppPrefix = manager.getKnowledgeBase()->getApplication()->getBasePrefix();
                manager.getKnowledgeBase()->setModulePrefix(module, (strAppPrefix+module->getBasePrefix()).c_str(), false);
                Glib::RefPtr<ModuleModel> mod = 
                                         ModuleModel::create(this, module);
                root->add_child(mod);
                Goocanvas::Bounds bd = m_Canvas->get_item(mod)->get_bounds();
                m_Canvas->get_item(mod)->translate(x - bd.get_x1(), y - bd.get_y1());
                
                mod->snapToGrid();
            }
        }
        m_bModified = true;
    }
    else if((data.get_length() >= 0) && (data.get_format() == APPLICATION))
    {
        const guchar* name = data.get_data();
        if(name)
        {
            ApplicationInterface iapp((const char*)name);
            Application* mainApplication = manager.getKnowledgeBase()->getApplication();            
            if(!mainApplication)
                return;
            
            string strPrefix = "/";
            strPrefix += string(manager.getKnowledgeBase()->getUniqueAppID(mainApplication, (const char*)name));
            iapp.setPrefix(strPrefix.c_str());            
            Application* application  = manager.getKnowledgeBase()->addIApplicationToApplication(mainApplication, iapp);
            if(application)
            {
                Glib::RefPtr<ApplicationModel> app = ApplicationModel::create(this, application);
                root->add_child(app);
                Glib::RefPtr<Goocanvas::Item> item = m_Canvas->get_item(app);
                item->translate(0,0);
                app->snapToGrid();
                app->updateChildItems();
                Goocanvas::Bounds bd = m_Canvas->get_item(app)->get_bounds();
                m_Canvas->get_item(app)->translate(x - bd.get_x1(), y - bd.get_y1());
                app->snapToGrid();
           }
        } 
    }
    else if((data.get_length() >= 0) && (data.get_format() == RESOURCE))
    {
        const guchar* name = data.get_data();
        if(name)
        {
            Glib::RefPtr<Goocanvas::Item> item = m_Canvas->get_item_at(x, y, true);
            if(item)
            {
                Glib::RefPtr<ModuleModel> mod = Glib::RefPtr<ModuleModel>::cast_dynamic(item->get_parent()->get_model());
                if(mod)
                {
                   mod->getModule()->setHost((const char*)name);
                   //unselect all 
                   for(int i=0; i<root->get_n_children(); i++)
                   {
                        Glib::RefPtr<ModuleModel> child = Glib::RefPtr<ModuleModel>::cast_dynamic(root->get_child(i));
                        if(child) child->setSelected(false);                    
                        Glib::RefPtr<ArrowModel> arrow = Glib::RefPtr<ArrowModel>::cast_dynamic(root->get_child(i));
                        if(arrow) arrow->setSelected(false);
                        Glib::RefPtr<ExternalPortModel> extport = Glib::RefPtr<ExternalPortModel>::cast_dynamic(root->get_child(i));
                        if(extport) extport->setSelected(false);
                   }

                    //select current module
                    mod->setSelected(true);
                    m_pParent->m_refActionGroup->get_action("EditDelete")->set_sensitive(true);
                    m_pParent->m_refActionGroup->get_action("EditCopy")->set_sensitive(true);
                    onUpdateModuleProperty(mod->getModule());
                    m_bModified = true;
                }    
            }
        }
    }
}


bool ApplicationWindow::onDragDrop(const Glib::RefPtr<Gdk::DragContext>& drag_context, 
                int x, int y, guint timestamp)
{
    return true;
}

bool ApplicationWindow::onDragMotion(const Glib::RefPtr<Gdk::DragContext>& drag_context, 
                int x, int y, guint timestamp)
{
    return true;
}

void ApplicationWindow::onUpdateModuleProperty(Module* module)
{
   
    if(m_HPaned.get_child2())
        m_HPaned.remove(*m_HPaned.get_child2());  
    modPropertyWindow->update(module);  
    m_HPaned.add2(*modPropertyWindow);
    m_HPaned.show_all();
   
    Glib::RefPtr<Gtk::ToggleAction> act;
    act = Glib::RefPtr<Gtk::ToggleAction>::cast_dynamic(
            m_pParent->m_refActionGroup->get_action("WindowProperty"));
    if(act)
        act->set_active(true);

}

