/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include "label_model.h"
#include "application_window.h"
#include "main_window.h"
#include "midpoint_model.h"
#include <goocanvas.h>

using namespace yarp::manager;


LabelModel::LabelModel(ApplicationWindow* parentWnd,
                             ArrowModel* arw, const char* text) 
                             : TextModel(text)

{
    parentWindow = parentWnd;
    arrow = arw;
    selected = false;
    if(text) label = text;
    this->property_font() = "Monospace 9";
    this->property_alignment().set_value(Pango::ALIGN_LEFT);
}


LabelModel::~LabelModel(void) 
{
}

void LabelModel::updateCoordiantes(void)
{
    if(arrow)
        arrow->onPointUpdated();
}


Gdk::Point LabelModel::getContactPoint(void)
{
    GooCanvasItemModel* model = (GooCanvasItemModel*) this->gobj();    
    GooCanvas* canvas = (GooCanvas*) parentWindow->m_Canvas->gobj();
    if(model && canvas)
    {
        GooCanvasItem* item = goo_canvas_get_item(canvas, model); 
        GooCanvasBounds bi;
        goo_canvas_item_get_bounds(item, &bi);
        goo_canvas_item_get_bounds(item, &bi);
        return Gdk::Point((int)(bi.x2), (int)(bi.y2));
    }    
    return Gdk::Point(-1, -1);
}


Glib::RefPtr<LabelModel> LabelModel::create(ApplicationWindow* parentWnd,
                                            ArrowModel* arw, const char* text) 
{
    return Glib::RefPtr<LabelModel>(new LabelModel(parentWnd, arw, text));
}


bool LabelModel::onItemButtonPressEvent(const Glib::RefPtr<Goocanvas::Item>& item, 
                    GdkEventButton* event)
{
    if((event->button == 1) && selected)
    {
        _dragging = item ;
        _drag_x = (int) event->x ;
        _drag_y = (int) event->y ;
    }
    return true;
}


bool LabelModel::onItemButtonReleaseEvent(const Glib::RefPtr<Goocanvas::Item>& item, 
                    GdkEventButton* event)
{
  if(event->button == 1)
  {    
        snapToGrid();
       _dragging.clear() ;
  }
  return true;
}

bool LabelModel::onItemMotionNotifyEvent(const Glib::RefPtr<Goocanvas::Item>& item, 
                    GdkEventMotion* event)
{
    if(item && _dragging && item == _dragging)
    {
        parentWindow->setModified();
        double new_x = event->x ;
        double new_y = event->y ;   
        item->translate(new_x - _drag_x, new_y - _drag_y);
        
        Goocanvas::Bounds bi = item->get_bounds();
        bi = item->get_bounds();

        if(bi.get_x1() < 0)
            item->translate(-bi.get_x1(), 0);
        if(bi.get_y1() < 0)
            item->translate(0, -bi.get_y1());
       
        snapToGrid();

        // updating arrows coordination        
        updateCoordiantes();
    }
    return true;
}

bool LabelModel::onItemEnterNotify(const Glib::RefPtr<Goocanvas::Item>& item, 
                    GdkEventCrossing* event)
{
   // printf("entered\n");
    return true;
}

bool LabelModel::onItemLeaveNotify(const Glib::RefPtr<Goocanvas::Item>& item, 
                    GdkEventCrossing* event)
{
    //printf("left\n");
    return true;
}

void LabelModel::setSelected(bool sel)
{

    selected = sel;
    if(selected)
    {
        this->property_fill_color().set_value("DodgerBlue3");
        this->raise();
    }
    else
    {
        this->property_fill_color().set_value("black");
    }
}

void LabelModel::snapToGrid(void)
{
    /*
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
        if(parentWindow->m_snapToGrid)
            goo_canvas_item_translate(item, -c_x, -c_y); 
        // force update
        goo_canvas_item_get_bounds(item, &bi);
        updateCoordiantes();
    }
    */
}

