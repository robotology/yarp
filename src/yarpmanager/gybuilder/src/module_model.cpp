/*
 *  Yarp Modules Manager
 *  Copyright: 2011 (C) Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <string.h>
#include "module_model.h"
#include "application_window.h"
#include "int_port_model.h"
#include <goocanvas.h>
#include "main_window.h"

#ifndef MAX
    #define MAX(X,Y) (((X) > (Y))?(X):(Y))
#endif

#define MIN_WIDTH           80
#define TEXT_MARGINE        30
#define PORT_GAP            10
#define MIN_HEIGHT          2*PORT_GAP + PORT_SIZE
#define FONT_DESC           "Monospace 10"

#define MODULE_COLOR        "White" //"WhiteSmoke"

ModuleModel::ModuleModel(ApplicationWindow* parentWnd, Module* mod, bool nested) : GroupModel()
{
    parentWindow = parentWnd;
    module = mod;
    bNested = nested;
    mod->setModel(this);

    // adding module name
    Glib::RefPtr<Goocanvas::TextModel> text = Goocanvas::TextModel::create(mod->getName());
#ifdef GLIBMM_PROPERTIES_ENABLED
    text->property_font() = FONT_DESC;
    text->property_alignment().set_value(Pango::ALIGN_CENTER);
#endif

    PangoLayout *layout = gtk_widget_create_pango_layout((GtkWidget*)parentWindow->gobj(), 
                            mod->getName());
    int text_w, text_h;
    PangoFontDescription *fontdesc = pango_font_description_from_string(FONT_DESC);
    pango_layout_set_font_description (layout, fontdesc);
    pango_layout_get_pixel_size (layout, &text_w, &text_h);

    //double w = strlen(mod->getName()) * FONT_WIDTH;
    double w = text_w + TEXT_MARGINE*2;

    int nInputs = module->inputCount();
    int nPorts = MAX(nInputs, module->outputCount());
    double h = nPorts*PORT_SIZE + 
              (nPorts-1)*PORT_GAP + 2*PORT_GAP;
    h = (h<MIN_HEIGHT) ? MIN_HEIGHT : h;

    
    shadowRect = Goocanvas::RectModel::create(3, 3, w, h);
    shadowRect->property_line_width().set_value(1.5) ;
    shadowRect->property_radius_x().set_value(3.0) ;
    shadowRect->property_radius_y().set_value(3.0) ;
    shadowRect->property_stroke_color().set_value("gray");
    shadowRect->property_fill_color().set_value("gray");    
    this->add_child(shadowRect);    

    mainRect = Goocanvas::RectModel::create(0,0, w, h);
    mainRect->property_line_width().set_value(1.2); 
    mainRect->property_radius_x().set_value(3.0) ;
    mainRect->property_radius_y().set_value(3.0) ;
    mainRect->property_stroke_color().set_value("DodgerBlue3");
    mainRect->property_fill_color().set_value(MODULE_COLOR);
    
    //if(module->owner() != parentWindow->manager.getKnowledgeBase()->getApplication())
    if(bNested)
    {
        GooCanvasLineDash *dash = goo_canvas_line_dash_new (2, 3.0, 3.0);
        g_object_set(mainRect->gobj(), "line-dash", dash, NULL);
    }
    this->add_child(mainRect);

    text->property_x().set_value(TEXT_MARGINE);
    text->property_y().set_value(h/2.0 - text_h/2.0);
    this->add_child(text);

    double top = (h - (nInputs*PORT_SIZE+(nInputs-1)*PORT_GAP))/2.0;
    for(int i=0; i<module->inputCount(); i++)
    {
        Glib::RefPtr<InternalPortModel> port = InternalPortModel::create(parentWindow, INPUTD, 
                                                        &module->getInputAt(i));
        port->set_property("x", -5);
        port->set_property("y", top +i*PORT_SIZE + i*PORT_GAP);
        this->add_child(port);
        // adding module name
        OSTRINGSTREAM str;
        str<<"I"<<i+1;
        Glib::RefPtr<Goocanvas::TextModel> text = Goocanvas::TextModel::create(str.str().c_str());
#ifdef GLIBMM_PROPERTIES_ENABLED
        text->property_font() = "Monospace 9";
        text->property_alignment().set_value(Pango::ALIGN_LEFT);
        text->property_x().set_value(8);
        text->property_y().set_value(top +i*PORT_SIZE + i*PORT_GAP);
#endif
        this->add_child(text);
    }

    top = (h - (module->outputCount()*PORT_SIZE+(module->outputCount()-1)*PORT_GAP))/2.0;  
    for(int i=0; i<module->outputCount(); i++)
    {
        Glib::RefPtr<InternalPortModel> port = InternalPortModel::create(parentWindow, OUTPUTD, 
                                                         &module->getOutputAt(i));
        port->set_property("x", w-3);
        port->set_property("y", top +i*PORT_SIZE + i*PORT_GAP);
        this->add_child(port);
        OSTRINGSTREAM str;
        str<<"O"<<i+1;
        Glib::RefPtr<Goocanvas::TextModel> text = Goocanvas::TextModel::create(str.str().c_str());
#ifdef GLIBMM_PROPERTIES_ENABLED
        text->property_font() = "Monospace 9";
        text->property_alignment().set_value(Pango::ALIGN_RIGHT);
        text->property_x().set_value(w-strlen(str.str().c_str())*10);
        text->property_y().set_value(top +i*PORT_SIZE + i*PORT_GAP);
#endif
        this->add_child(text);
    }
    
    width  = w;
    height = h;
}


ModuleModel::~ModuleModel(void) 
{
    if(!module)
        return; 

    Application* application = parentWindow->manager.getKnowledgeBase()->getApplication();            
    if(application)
       parentWindow->manager.getKnowledgeBase()->removeIModuleFromApplication(application,
                                    module->getLabel());
    module = NULL;                                    
}


Glib::RefPtr<ModuleModel> ModuleModel::create(ApplicationWindow* parentWnd, Module* mod, bool nested)
{    
    return Glib::RefPtr<ModuleModel>(new ModuleModel(parentWnd, mod, nested));
}


bool ModuleModel::onItemButtonPressEvent(const Glib::RefPtr<Goocanvas::Item>& item, 
                    GdkEventButton* event)
{
    if(bNested)
        return true;

    if(item && Glib::RefPtr<InternalPortModel>::cast_dynamic(item->get_model()))
        return Glib::RefPtr<InternalPortModel>::cast_dynamic(item->get_model())->onItemButtonPressEvent(item, event);
    
    if(event->type == GDK_2BUTTON_PRESS)
    {
       parentWindow->onUpdateModuleProperty(module);
       return true;
    }
    
    if(event->button == 1)
    {
        _dragging = item ;
        _drag_x = (int) event->x ;
        _drag_y = (int) event->y ;
        return false;
    }
    else
    if(event->button == 3)
    {
        MainWindow* wnd = parentWindow->getMainWindow();
        wnd->m_refActionGroup->get_action("EditDelete")->set_sensitive(selected);
        wnd->m_refActionGroup->get_action("EditCopy")->set_sensitive(selected);
        Gtk::Menu* pMenu = dynamic_cast<Gtk::Menu*>(
        wnd->m_refUIManager->get_widget("/PopupModuleModel"));
        if(pMenu)
            pMenu->popup(event->button, event->time);
    }

    return true;
}

bool ModuleModel::onItemButtonReleaseEvent(const Glib::RefPtr<Goocanvas::Item>& item, 
                    GdkEventButton* event)
{
  if(bNested)
        return true;

  if(event->button == 1)
  {    
        snapToGrid();
       _dragging.clear() ;
  }
  return true;
}

bool ModuleModel::onItemMotionNotifyEvent(const Glib::RefPtr<Goocanvas::Item>& item, 
                    GdkEventMotion* event)
{
    if(bNested)
    {
        updateArrowCoordination();
        return true;
    }

    if(item && _dragging && (item == _dragging))
    {
        parentWindow->setModified();
        
        double new_x = event->x ;
        double new_y = event->y ;   
        item->get_parent()->translate(new_x - _drag_x, new_y - _drag_y);
        
        Goocanvas::Bounds bi = item->get_parent()->get_bounds();
        bi = item->get_parent()->get_bounds();

        if(bi.get_x1() < 0)
            item->get_parent()->translate(-bi.get_x1(), 0);
        if(bi.get_y1() < 0)
            item->get_parent()->translate(0, -bi.get_y1());
       
        snapToGrid();

        bi = item->get_parent()->get_bounds();
        this->points.clear();
        GyPoint pt;
        pt.x = bi.get_x1();
        pt.y = bi.get_y1();
        this->points.push_back(pt);

        // updating arrows coordination
        updateArrowCoordination();

        // updating canvas boundries 
        bool needUpdate = false;
        if(parentWindow->m_Canvas)
        {
            Goocanvas::Bounds bc;
            parentWindow->m_Canvas->get_bounds(bc);
            needUpdate = (bi.get_x2() > bc.get_x2()) || (bi.get_y2() > bc.get_y2());
            if(needUpdate)
            {
                new_x = (bi.get_x2() > bc.get_x2()) ? bi.get_x2() : bc.get_x2(); 
                new_y = (bi.get_y2() > bc.get_y2()) ? bi.get_y2() : bc.get_y2(); 
                parentWindow->m_Canvas->set_bounds(0,0, new_x, new_y);            
                if(parentWindow->m_Grid)
                {
                    g_object_set(parentWindow->m_Grid, "width", new_x, NULL);
                    g_object_set(parentWindow->m_Grid, "height", new_y, NULL);
                }
            }
        }

    }
    return true;
}

bool ModuleModel::onItemEnterNotify(const Glib::RefPtr<Goocanvas::Item>& item, 
                    GdkEventCrossing* event)
{
    /*
    if(item && Glib::RefPtr<InternalPortModel>::cast_dynamic(item->get_model()))
        Glib::RefPtr<InternalPortModel>::cast_dynamic(item->get_model())->onItemEnterNotify(item, event);
    */    
    return true;
}

bool ModuleModel::onItemLeaveNotify(const Glib::RefPtr<Goocanvas::Item>& item, 
                    GdkEventCrossing* event)
{
    /*
    if(item && Glib::RefPtr<InternalPortModel>::cast_dynamic(item->get_model()))
        Glib::RefPtr<InternalPortModel>::cast_dynamic(item->get_model())->onItemLeaveNotify(item, event);
    */    
    return true;
}

void ModuleModel::setSelected(bool sel)
{
    if(bNested)
        return;

    selected = sel;
    if(selected)
    {
       //rect->property_stroke_color_rgb().set_value(127); 
        //Cairo::RadialGradient radial(0.5, 0.5, 0.25, 0.5, 0.5, 0.75);
        //radial.add_color_stop_rgba(0, 0, 0, 0, 1)
        //radial.add_color_stop_rgba(0.5, 0, 0, 0, 0)//
        mainRect->property_fill_color().set_value("LightSteelBlue");
        this->raise();
    }
    else
    {
        mainRect->property_fill_color().set_value(MODULE_COLOR);
    }
}

void ModuleModel::updateArrowCoordination(void)
{
    for(int i=0; i<this->get_n_children(); i++)
    {
        Glib::RefPtr<InternalPortModel> port = 
            Glib::RefPtr<InternalPortModel>::cast_dynamic(this->get_child(i));
        if(port)
        {   
            std::vector<ArrowModel*>::iterator itr;
            for(itr = port->getSourceArrows().begin(); itr!= port->getSourceArrows().end(); itr++)
               (*itr)->updatCoordiantes();                
            for(itr = port->getDestinationArrows().begin(); itr!= port->getDestinationArrows().end(); itr++)
               (*itr)->updatCoordiantes();
        }
    }
}


void ModuleModel::setArrowsSelected(bool sel)
{
    for(int i=0; i<this->get_n_children(); i++)
    {
        Glib::RefPtr<InternalPortModel> port = 
            Glib::RefPtr<InternalPortModel>::cast_dynamic(this->get_child(i));
        if(port)
        {   
            std::vector<ArrowModel*>::iterator itr;
            for(itr = port->getSourceArrows().begin(); itr!= port->getSourceArrows().end(); itr++)
               (*itr)->setSelected(sel);
            for(itr = port->getDestinationArrows().begin(); itr!= port->getDestinationArrows().end(); itr++)
               (*itr)->setSelected(sel);
        }
    }
}


void ModuleModel::snapToGrid(void)
{
    if(bNested)
        return;

    GooCanvasItemModel* model = (GooCanvasItemModel*) this->gobj();    
    GooCanvas* canvas = (GooCanvas*) parentWindow->m_Canvas->gobj();
    if(model && canvas)
    {
        GooCanvasItem* item = goo_canvas_get_item(canvas, model); 
        GooCanvasBounds bi;
        goo_canvas_item_get_bounds(item, &bi);
        double xs, ys;
        g_object_get (parentWindow->m_Grid,
               "x_step", &xs,
               "y_step", &ys,
               NULL);
        double c_x = (int) (((int)bi.x1) % (int)xs);
        double c_y = (int) (((int)bi.y1) % (int)ys);
        //printf("cx:%f, cy:%f\n", c_x, c_y);
        //printf("x:%f, y:%f\n", bi.x1, bi.y1);
        if(parentWindow->m_snapToGrid)
            goo_canvas_item_translate(item, -c_x, -c_y); 
        // force update
        goo_canvas_item_get_bounds(item, &bi);
        this->points.clear();
        GyPoint pt;
        pt.x = bi.x1;
        pt.y = bi.y1;
        this->points.push_back(pt);
        updateArrowCoordination();
    }
}


