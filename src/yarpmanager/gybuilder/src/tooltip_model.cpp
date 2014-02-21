/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include "tooltip_model.h"
#include "application_window.h"
#include "main_window.h"
#include "midpoint_model.h"
#include <goocanvas.h>

#ifndef MAX
    #define MAX(X,Y) (((X) > (Y))?(X):(Y))
#endif

#define MIN_WIDTH           10
#define TEXT_MARGINE        5
#define MIN_HEIGHT          10
#define FONT_DESC           "Monospace 9"

using namespace yarp::manager;


TooltipModel::TooltipModel(ApplicationWindow* parentWnd,
                           const char* txt) 
                             : GroupModel()
{
    parentWindow = parentWnd;
    if(txt) tooltip = txt;
    // adding module name
    Glib::RefPtr<Goocanvas::TextModel> text = Goocanvas::TextModel::create(txt);
#ifdef GLIBMM_PROPERTIES_ENABLED
    text->property_font() = FONT_DESC;
    text->property_alignment().set_value(Pango::ALIGN_CENTER);
#endif

    PangoLayout *layout = gtk_widget_create_pango_layout((GtkWidget*)parentWindow->gobj(), 
                            txt);
    int text_w, text_h;
    PangoFontDescription *fontdesc = pango_font_description_from_string(FONT_DESC);
    pango_layout_set_font_description (layout, fontdesc);
    pango_layout_get_pixel_size (layout, &text_w, &text_h);

    width = text_w + TEXT_MARGINE*2;
    height = text_h + TEXT_MARGINE*2;
   
    /*
    shadowRect = Goocanvas::RectModel::create(3, 3, w, h);
    shadowRect->property_line_width().set_value(1.5) ;
    shadowRect->property_radius_x().set_value(3.0) ;
    shadowRect->property_radius_y().set_value(3.0) ;
    shadowRect->property_stroke_color().set_value("gray") ;
    shadowRect->property_fill_color().set_value("gray") ;    
    this->add_child(shadowRect);
    */

    Glib::RefPtr<Goocanvas::RectModel> mainRect = Goocanvas::RectModel::create(0,0, width, height);
    mainRect->property_line_width().set_value(1.0) ;
    mainRect->property_radius_x().set_value(3.0) ;
    mainRect->property_radius_y().set_value(3.0) ;
    mainRect->property_stroke_color().set_value("Gold1");
    mainRect->property_fill_color_rgba().set_value(0xfff8dcff); //0xffd70030
    mainRect->set_property("antialias", Cairo::ANTIALIAS_NONE);
    this->add_child(mainRect);

    text->property_x().set_value(TEXT_MARGINE);
    text->property_y().set_value(height/2.0 - text_h/2.0);
    this->add_child(text);
}


TooltipModel::~TooltipModel(void) 
{

}


Glib::RefPtr<TooltipModel> TooltipModel::create(ApplicationWindow* parentWnd,
                                                const char* txt) 
{
    return Glib::RefPtr<TooltipModel>(new TooltipModel(parentWnd, txt));
}


bool TooltipModel::onItemButtonPressEvent(const Glib::RefPtr<Goocanvas::Item>& item, 
                    GdkEventButton* event)
{
    if(event->button == 1)
    {
    }
    return true;
}


bool TooltipModel::onItemButtonReleaseEvent(const Glib::RefPtr<Goocanvas::Item>& item, 
                    GdkEventButton* event)
{
  if(event->button == 1)
  {    
  }
  return true;
}

bool TooltipModel::onItemMotionNotifyEvent(const Glib::RefPtr<Goocanvas::Item>& item, 
                    GdkEventMotion* event)
{
    if(item)
    {
    }
    return true;
}

bool TooltipModel::onItemEnterNotify(const Glib::RefPtr<Goocanvas::Item>& item, 
                    GdkEventCrossing* event)
{
   // printf("entered\n");
    return true;
}

bool TooltipModel::onItemLeaveNotify(const Glib::RefPtr<Goocanvas::Item>& item, 
                    GdkEventCrossing* event)
{
    //printf("left\n");
    return true;
}


