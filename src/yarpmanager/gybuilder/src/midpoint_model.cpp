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
    #pragma warning (disable : 4099)
#endif

#include "arrow_model.h"
#include "application_window.h"
#include "main_window.h"
#include "midpoint_model.h"
#include <goocanvas.h>

#define AUTOSNIPE_MARGINE           5

using namespace yarp::manager;

MidpointModel::MidpointModel(ApplicationWindow* parentWnd,
                             ArrowModel* arw, double x, double y) 
                             : RectModel(0, 0, POINT_SIZE, POINT_SIZE)

{
    parentWindow = parentWnd;
    arrow = arw;
    point_x = x;
    point_y = y;
    selected = false;

    this->property_line_width().set_value(1.0) ;
    this->property_stroke_color().set_value("black");
    this->property_fill_color().set_value("DodgerBlue3");
}


MidpointModel::~MidpointModel(void) 
{

}

void MidpointModel::updateCoordiantes(void)
{
    if(!arrow)
        return;

    Gdk::Point pt = getContactPoint();
    arrow->setPoint(point_x, point_y, 
                    pt.get_x(), pt.get_y());
    point_x = pt.get_x();
    point_y = pt.get_y();
    arrow->updatLabelCoordiante();
}


void MidpointModel::remove(void)
{
    if(!arrow)
        return;
    //arrow->deletePoint(point_x, point_y);
}


Gdk::Point MidpointModel::getContactPoint(void)
{
    GooCanvasItemModel* model = (GooCanvasItemModel*) this->gobj();    
    GooCanvas* canvas = (GooCanvas*) parentWindow->m_Canvas->gobj();
    if(model && canvas)
    {
        GooCanvasItem* item = goo_canvas_get_item(canvas, model); 
        GooCanvasBounds bi;
        goo_canvas_item_get_bounds(item, &bi);
        goo_canvas_item_get_bounds(item, &bi);
        return Gdk::Point((int)(bi.x2-POINT_SIZE/2.0), (int)(bi.y2-POINT_SIZE/2.0));
    }    
    return Gdk::Point(-1, -1);
}


Glib::RefPtr<MidpointModel> MidpointModel::create(ApplicationWindow* parentWnd,
                                            ArrowModel* arw, double x, double y) 
{
    return Glib::RefPtr<MidpointModel>(new MidpointModel(parentWnd, arw, x, y));
}


bool MidpointModel::onItemButtonPressEvent(const Glib::RefPtr<Goocanvas::Item>& item, 
                    GdkEventButton* event)
{   
    if(event->button == 1)
    {
        _dragging = item ;
        _drag_x = (int) event->x ;
        _drag_y = (int) event->y ;
    }
    return true;
}

bool MidpointModel::onItemButtonReleaseEvent(const Glib::RefPtr<Goocanvas::Item>& item, 
                    GdkEventButton* event)
{
  if(event->button == 1)
  {    
        snapToGrid();
       _dragging.clear() ;
  }
  return true;
}

bool MidpointModel::onItemMotionNotifyEvent(const Glib::RefPtr<Goocanvas::Item>& item, 
                    GdkEventMotion* event)
{
    if(item && _dragging && item == _dragging)
    {
        parentWindow->setModified();
        double new_x = event->x ;
        double new_y = event->y ;   
        item->translate(new_x - _drag_x, new_y - _drag_y);
  
        if(event->state  & GDK_CONTROL_MASK)
        {
           int index = arrow->getIndex(point_x, point_y);
           if(index)
           {
                Gdk::Point current_point, base_point;
                int dist;

                current_point = getContactPoint();

                // adjusting to the previous point
                base_point = arrow->getPoint(index-1);                               
                dist =  current_point.get_y() -  base_point.get_y();
                if(abs(dist) <= AUTOSNIPE_MARGINE)
                    item->translate(0, -dist);
                dist =  current_point.get_x() -  base_point.get_x();
                if(abs(dist) <= AUTOSNIPE_MARGINE)
                    item->translate(-dist, 0);

                // adjusting to the next point
                base_point = arrow->getPoint(index+1);                               
                dist =  current_point.get_y() -  base_point.get_y();
                if(abs(dist) <= AUTOSNIPE_MARGINE)
                    item->translate(0, -dist);
                dist =  current_point.get_x() -  base_point.get_x();
                if(abs(dist) <= AUTOSNIPE_MARGINE)
                    item->translate(-dist, 0);
           }
        }

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

bool MidpointModel::onItemEnterNotify(const Glib::RefPtr<Goocanvas::Item>& item, 
                    GdkEventCrossing* event)
{
   // printf("entered\n");
    return true;
}

bool MidpointModel::onItemLeaveNotify(const Glib::RefPtr<Goocanvas::Item>& item, 
                    GdkEventCrossing* event)
{
    //printf("left\n");
    return true;
}

void MidpointModel::setSelected(bool sel)
{
    selected = sel;
    if(selected)
    {
        this->property_stroke_color().set_value("Red");
        this->raise();
        this->property_visibility().set_value(Goocanvas::ITEM_VISIBLE);
    }
    else
    {
        this->property_stroke_color().set_value("black");
        this->property_visibility().set_value(Goocanvas::ITEM_HIDDEN);
    }
}

void MidpointModel::snapToGrid(void)
{
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
        double c_x = (int) (((int)bi.x1+(int)(POINT_SIZE/2.0)) % (int)xs);
        double c_y = (int) (((int)bi.y1+(int)(POINT_SIZE/2.0)) % (int)ys);
        if(parentWindow->m_snapToGrid)
            goo_canvas_item_translate(item, -c_x, -c_y); 
        // force update
        goo_canvas_item_get_bounds(item, &bi);
        updateCoordiantes();
    }
}

