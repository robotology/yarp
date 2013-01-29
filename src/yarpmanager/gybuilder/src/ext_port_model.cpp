/*
 *  Yarp Modules Manager
 *  Copyright: 2011 (C) Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include "ext_port_model.h"
#include "application_window.h"
#include "main_window.h"
#include <goocanvas.h>

#ifndef MAX
    #define MAX(X,Y) (((X) > (Y))?(X):(Y))
#endif

#define MIN_WIDTH           10
#define H_MARGINE           10
#define V_MARGINE           5
#define MIN_HEIGHT          10
#define FONT_DESC           "Monospace 9"
#define PORT_DEPTH          10


ExternalPortModel::ExternalPortModel(ApplicationWindow* parentWnd, 
                        NodeType t, const char* port, bool nested) : PortModel(t)
{
    parentWindow = parentWnd;
    bNested = nested;
    if(port) strPort = port;

    text = Goocanvas::TextModel::create(port);
#ifdef GLIBMM_PROPERTIES_ENABLED
    text->property_font() = FONT_DESC;
    text->property_alignment().set_value(Pango::ALIGN_CENTER);
#endif

    /*
    PangoLayout *layout = gtk_widget_create_pango_layout((GtkWidget*)parentWindow->gobj(), 
                            port);
    int text_w, text_h;
    PangoFontDescription *fontdesc = pango_font_description_from_string(FONT_DESC);
    pango_layout_set_font_description (layout, fontdesc);
    pango_layout_get_pixel_size (layout, &text_w, &text_h);

    double width = text_w + H_MARGINE*2 + PORT_DEPTH;
    double height = text_h + V_MARGINE*2;
    */

    poly = Goocanvas::PolylineModel::create(0,0,0,0);
    poly->property_close_path().set_value(true);
    poly->property_line_width().set_value(1.0);
    poly->property_fill_color().set_value("WhiteSmoke");
    poly->property_stroke_color().set_value("black");    

    shadow = Goocanvas::PolylineModel::create(0,0,0,0);
    shadow->property_close_path().set_value(true);
    shadow->property_fill_color().set_value("gray");
    shadow->property_stroke_color().set_value("gray");    
    shadow->translate(3,3);

    /*
    if(type == INPUTD)
    {
        Goocanvas::Points points(5);
        points.set_coordinate(0, 0, 0);
        points.set_coordinate(1, width, 0);
        points.set_coordinate(2, width, height);
        points.set_coordinate(3, 0, height);
        points.set_coordinate(4, PORT_DEPTH, height/2.0);
        poly->property_points().set_value(points);
        shadow->property_points().set_value(points);
        shadow->translate(3,3);
    }
    else
    {
        Goocanvas::Points points(5);
        points.set_coordinate(0, 0, 0);
        points.set_coordinate(1, width-PORT_DEPTH, 0);
        points.set_coordinate(2, width, height/2.0);
        points.set_coordinate(3, width-PORT_DEPTH, height);
        points.set_coordinate(4, 0, height);
        poly->property_points().set_value(points);
        shadow->property_points().set_value(points);
        shadow->translate(3,3);
    }
    */
    this->add_child(shadow);
    this->add_child(poly);
    /*
    text->property_x().set_value(PORT_DEPTH+H_MARGINE/2.0);
    text->property_y().set_value(height/2.0 - text_h/2.0);
    */
    this->add_child(text);   
    setPort(port);
}

ExternalPortModel::~ExternalPortModel(void) 
{
    if(poly)
        poly.clear();
}


void ExternalPortModel::setPort(const char* szPort)
{
    if(!szPort)
        return;
   
    strPort = szPort;

    PangoLayout *layout = gtk_widget_create_pango_layout((GtkWidget*)parentWindow->gobj(), 
                            szPort);
    int text_w, text_h;
    PangoFontDescription *fontdesc = pango_font_description_from_string(FONT_DESC);
    pango_layout_set_font_description (layout, fontdesc);
    pango_layout_get_pixel_size (layout, &text_w, &text_h);

    width = text_w + H_MARGINE*2 + PORT_DEPTH;
    height = text_h + V_MARGINE*2;
    
    if(type == INPUTD)
    {
        Goocanvas::Points points(5);
        points.set_coordinate(0, 0, 0);
        points.set_coordinate(1, width, 0);
        points.set_coordinate(2, width, height);
        points.set_coordinate(3, 0, height);
        points.set_coordinate(4, PORT_DEPTH, height/2.0);
        poly->property_points().set_value(points);
        shadow->property_points().set_value(points);
    }
    else
    {
        Goocanvas::Points points(5);
        points.set_coordinate(0, 0, 0);
        points.set_coordinate(1, width-PORT_DEPTH, 0);
        points.set_coordinate(2, width, height/2.0);
        points.set_coordinate(3, width-PORT_DEPTH, height);
        points.set_coordinate(4, 0, height);
        poly->property_points().set_value(points);
        shadow->property_points().set_value(points);
    }

    text->property_text() = szPort;
    text->property_x().set_value(PORT_DEPTH+H_MARGINE/2.0);
    text->property_y().set_value(height/2.0 - text_h/2.0);
    updateArrowCoordination();
    
    // updating all connections from/to this port 
    Application* application = parentWindow->manager.getKnowledgeBase()->getApplication();
    std::vector<ArrowModel*>::iterator itr;   
    if(type == OUTPUTD)
    {
        for(itr = sourceArrows.begin(); itr!= sourceArrows.end(); itr++)
        {
            Connection* pConnection = (*itr)->getConnection();
            Connection con = *pConnection;
            con.setFrom(szPort);
            parentWindow->manager.getKnowledgeBase()->updateConnectionOfApplication(application, *pConnection, con);
            pConnection->setFrom(szPort);
        }            
    }
    else
    {
        for(itr = destinationArrows.begin(); itr!= destinationArrows.end(); itr++)
        {
            Connection* pConnection = (*itr)->getConnection();
            Connection con = *pConnection;
            con.setTo(szPort);
            parentWindow->manager.getKnowledgeBase()->updateConnectionOfApplication(application, *pConnection, con);
            pConnection->setTo(szPort);
        }            
    }
}


Gdk::Point ExternalPortModel::getContactPoint(void)
{
    GooCanvasItemModel* model = (GooCanvasItemModel*) poly->gobj();    
    GooCanvas* canvas = (GooCanvas*) parentWindow->m_Canvas->gobj();
    if(model && canvas)
    {
        GooCanvasItem* item = goo_canvas_get_item(canvas, model); 
        if(item)
        {
            GooCanvasBounds bi;
            goo_canvas_item_get_bounds(item, &bi);
            if(type == INPUTD)
                return Gdk::Point(bi.x1+PORT_DEPTH, bi.y2-((bi.y2-bi.y1)/2.0));
            return Gdk::Point(bi.x2, bi.y2-((bi.y2-bi.y1)/2.0));
        }            
    }    
    return Gdk::Point(-1, -1);
}



Glib::RefPtr<ExternalPortModel> ExternalPortModel::create(ApplicationWindow* parentWnd, NodeType t, const char* port, bool nested)
{
    return Glib::RefPtr<ExternalPortModel>(new ExternalPortModel(parentWnd, t, port, nested));
}


bool ExternalPortModel::onItemButtonPressEvent(const Glib::RefPtr<Goocanvas::Item>& item, 
                    GdkEventButton* event)
{
    if(bNested)
        return true;

    if(event->button == 1)
    {
        _dragging = item ;
        _drag_x = (int) event->x ;
        _drag_y = (int) event->y ;
    }
    else
    if(event->button == 3)
    {
        MainWindow* wnd = parentWindow->getMainWindow();
        wnd->m_refActionGroup->get_action("EditDelete")->set_sensitive(selected);
        //wnd->m_refActionGroup->get_action("EditCut")->set_sensitive(selected);
        //wnd->m_refActionGroup->get_action("EditCopy")->set_sensitive(selected);
        Gtk::Menu* pMenu = dynamic_cast<Gtk::Menu*>(
        wnd->m_refUIManager->get_widget("/PopupExtPortModel"));
        if(pMenu)
            pMenu->popup(event->button, event->time);
    }


    return true;
}

bool ExternalPortModel::onItemButtonReleaseEvent(const Glib::RefPtr<Goocanvas::Item>& item, 
                    GdkEventButton* event)
{
    if(bNested)
        return true;

    if(event->button == 1)
    {    
        snapToGrid();
       _dragging.clear();
    }
    return true;
}

bool ExternalPortModel::onItemMotionNotifyEvent(const Glib::RefPtr<Goocanvas::Item>& item, 
                    GdkEventMotion* event)
{
    if(bNested)
        return true;

    if(item && _dragging && item == _dragging)
    {
        parentWindow->setModified();
        double new_x = event->x ;
        double new_y = event->y ;   
        item->get_parent()->translate(new_x - _drag_x, new_y - _drag_y);
        
        Goocanvas::Bounds bi = item->get_bounds();
        bi = item->get_parent()->get_bounds();

        if(bi.get_x1() < 0)
            item->get_parent()->translate(-bi.get_x1(), 0);
        if(bi.get_y1() < 0)
            item->get_parent()->translate(0, -bi.get_y1());
       
        snapToGrid();

        // updating arrows coordination        
        updateArrowCoordination();

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

bool ExternalPortModel::onItemEnterNotify(const Glib::RefPtr<Goocanvas::Item>& item, 
                    GdkEventCrossing* event)
{
    parentWindow->get_window()->set_cursor(Gdk::Cursor(Gdk::HAND1));
    poly->property_fill_color().set_value("LightSteelBlue");
    return true;
}

bool ExternalPortModel::onItemLeaveNotify(const Glib::RefPtr<Goocanvas::Item>& item, 
                    GdkEventCrossing* event)
{
    parentWindow->get_window()->set_cursor();
    if(bNested)
    {
        poly->property_stroke_color().set_value("black");
        poly->property_fill_color().set_value("WhiteSmoke");
        return true;
    }

    if(!selected)
    {
        poly->property_stroke_color().set_value("black");
        poly->property_fill_color().set_value("WhiteSmoke");
    }
    return true;
}

void ExternalPortModel::updateArrowCoordination(void)
{
   std::vector<ArrowModel*>::iterator itr;
   if(type == OUTPUTD)
   {
        for(itr = sourceArrows.begin(); itr!= sourceArrows.end(); itr++)
            (*itr)->updatCoordiantes();                           
   }
   else
   {
        for(itr = destinationArrows.begin(); itr!= destinationArrows.end(); itr++)
            (*itr)->updatCoordiantes();
   }
}

void ExternalPortModel::setArrowsSelected(bool sel)
{
   std::vector<ArrowModel*>::iterator itr;
   if(type == OUTPUTD)
   {
        for(itr = sourceArrows.begin(); itr!= sourceArrows.end(); itr++)
            (*itr)->setSelected(true);
   }
   else
   {
        for(itr = destinationArrows.begin(); itr!= destinationArrows.end(); itr++)
            (*itr)->setSelected(true);
   }
}


void ExternalPortModel::setSelected(bool sel)
{
    selected = sel;
    if(selected)
    {
        poly->property_fill_color().set_value("LightSteelBlue");
        this->raise();
    }
    else
        poly->property_fill_color().set_value("WhiteSmoke");
}



void ExternalPortModel::snapToGrid(void)
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
        double c_x = (int) (((int)bi.x1) % (int)xs);
        double c_y = (int) (((int)bi.y1) % (int)ys);
        if(parentWindow->m_snapToGrid)
            goo_canvas_item_translate(item, -c_x, -c_y); 
        // force update
        goo_canvas_item_get_bounds(item, &bi);
        updateArrowCoordination();
    }
}


