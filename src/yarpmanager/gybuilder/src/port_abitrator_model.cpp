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

#include "port_abitrator_model.h"
#include "ext_port_model.h"
#include "application_window.h"
#include "main_window.h"
#include <goocanvas.h>

#ifndef MAX
    #define MAX(X,Y) (((X) > (Y))?(X):(Y))
#endif

#define MIN_WIDTH           PORTARB_DEF_WIDTH
#define V_MARGINE           10
#define H_MARGINE           3
#define PORT_GAP            10
#define MIN_HEIGHT          PORTARB_DEF_HEIGH
#define FONT_DESC           "Monospace 10"

using namespace yarp::manager;


PortArbitratorModel::PortArbitratorModel(ApplicationWindow* parentWnd, 
                                         Glib::RefPtr<PortModel> dest, Arbitrator* arb, bool nested) : PortModel(INOUTD)
{
    portColor = ARBITRATOR_COLOR;
    parentWindow = parentWnd;
    bNested = nested;
    destination = dest;
    if(arb)
        arbitrator = *arb;
    arbitrator.setModel(this); 
    Application* application = parentWindow->manager.getKnowledgeBase()->getApplication();
    arbitrator = parentWindow->manager.getKnowledgeBase()->addArbitratorToApplication(application, arbitrator);

    poly = Goocanvas::PolylineModel::create(0,0,0,0);
    poly->property_close_path().set_value(true);
    poly->property_line_width().set_value(1.0);
    poly->property_fill_color().set_value(ARBITRATOR_COLOR);
    poly->property_stroke_color().set_value("black");    

    shadow = Goocanvas::PolylineModel::create(0,0,0,0);
    shadow->property_close_path().set_value(true);
    shadow->property_fill_color().set_value("gray");
    shadow->property_stroke_color().set_value("gray");    

    PangoLayout *layout = gtk_widget_create_pango_layout((GtkWidget*)parentWindow->gobj(), "O1");
    int text_w, text_h;
    PangoFontDescription *fontdesc = pango_font_description_from_string(FONT_DESC);
    pango_layout_set_font_description (layout, fontdesc);
    pango_layout_get_pixel_size (layout, &text_w, &text_h);

    Gdk::Point defSize = getDefaultSize();

    width =  defSize.get_x();
    height = defSize.get_y();
 
    Goocanvas::Points points(7);
    points.set_coordinate(0, 0, 0);
    points.set_coordinate(1, width, 0);
    points.set_coordinate(2, width, height/2.0-5);
    points.set_coordinate(3, width+5, height/2.0);
    points.set_coordinate(4, width, height/2.0+5);
    points.set_coordinate(5, width, height);
    points.set_coordinate(6, 0, height);
    poly->property_points().set_value(points);
    shadow->property_points().set_value(points);
    shadow->translate(3,3);

    this->add_child(shadow);
    this->add_child(poly);

}

PortArbitratorModel::~PortArbitratorModel(void) 
{
    //Application* app = arbitrator->owner(); 
    parentWindow->manager.getKnowledgeBase()->removeArbitratorFromApplication((Application*)arbitrator.owner(), arbitrator);
}


Gdk::Point PortArbitratorModel::getContactPoint(ArrowModel* arrow)
{
    if(!strlen(arrow->getId()))
        return Gdk::Point(-1, -1);

    GooCanvasItemModel* model = (GooCanvasItemModel*) poly->gobj();    
    GooCanvas* canvas = (GooCanvas*) parentWindow->m_Canvas->gobj();
    GooCanvasItem* item = goo_canvas_get_item(canvas, model); 
    GooCanvasBounds bi;
    goo_canvas_item_get_bounds(item, &bi);

    if(strcmp(arrow->getId(), "O1") == 0)
        return Gdk::Point((int)bi.x2, (int)((bi.y2+bi.y1)/2.0));

    Gdk::Point pt = contacts[arrow->getId()];
    return Gdk::Point((int)bi.x1, (int)(bi.y1+pt.get_y()));
}

bool PortArbitratorModel::addSourceArrow(ArrowModel* arrow) 
{
    arrow->setId("O1");
    contacts["O1"] = Gdk::Point(0, 0); 
    return PortModel::addSourceArrow(arrow);
}

bool PortArbitratorModel::addDestinationArrow(ArrowModel* arrow) 
{
    int id = 1;
    string strId;
    std::map<std::string, Gdk::Point>::iterator itr;        
    for(itr=contacts.begin(); itr!=contacts.end(); itr++)
    {
        char szId[128];
        sprintf(szId, "C%d", id);
        if(contacts.find(szId) == contacts.end())
        {
            strId = szId;
            break;
        }       
        id++;
    }

    if(!strId.size())
    {
        char szId[128];
        sprintf(szId, "C%d", id);
        strId = szId;
    }

    if(strlen(arrow->getId())==0)
        arrow->setId(strId.c_str());

    // first get the rule if exist
    string rule = arbitrator.getRule(arrow->getId());
    arbitrator.addRule(arrow->getId(), rule.c_str());
    Application* application = parentWindow->manager.getKnowledgeBase()->getApplication();
    arbitrator = parentWindow->manager.getKnowledgeBase()->addArbitratorToApplication(application, arbitrator);

//#ifdef GLIBMM_PROPERTIES_ENABLED
    PangoLayout *layout = gtk_widget_create_pango_layout((GtkWidget*)parentWindow->gobj(), arrow->getId());
    int text_w, text_h;
    PangoFontDescription *fontdesc = pango_font_description_from_string(FONT_DESC);
    pango_layout_set_font_description (layout, fontdesc);
    pango_layout_get_pixel_size (layout, &text_w, &text_h);

    // finding maximum text width of connection ids
    int max_w = text_w;
    int max_h = text_h;
    //std::map<std::string, Gdk::Point>::iterator itr;        
    for(itr=contacts.begin(); itr!=contacts.end(); itr++)
    {
        int text_w, text_h;
        PangoLayout *layout = gtk_widget_create_pango_layout((GtkWidget*)parentWindow->gobj(), (itr->first).c_str());
        PangoFontDescription *fontdesc = pango_font_description_from_string(FONT_DESC);
        pango_layout_set_font_description (layout, fontdesc);
        pango_layout_get_pixel_size (layout, &text_w, &text_h);
        max_w = (max_w<text_w) ? text_w : max_w; 
        max_h = (max_h<text_h) ? text_h : max_h; 
    }

    width = max_w + H_MARGINE*2;
    height = max_h*contacts.size() + V_MARGINE*(contacts.size()+1);
  
    Goocanvas::Points points(7);
    points.set_coordinate(0, 0, 0);
    points.set_coordinate(1, width, 0);
    points.set_coordinate(2, width, height/2.0-5);
    points.set_coordinate(3, width+5, height/2.0);
    points.set_coordinate(4, width, height/2.0+5);
    points.set_coordinate(5, width, height);
    points.set_coordinate(6, 0, height);
    poly->property_points().set_value(points);
    shadow->property_points().set_value(points);

    contacts[arrow->getId()] = Gdk::Point(0, id*V_MARGINE + (id-1)*max_h + max_h/2);

    labels[arrow->getId()] = Goocanvas::TextModel::create(arrow->getId());
    labels[arrow->getId()]->property_font() = FONT_DESC;
    labels[arrow->getId()]->property_alignment().set_value(Pango::ALIGN_CENTER);
    labels[arrow->getId()]->property_x().set_value(H_MARGINE);
    labels[arrow->getId()]->property_y().set_value(id*V_MARGINE+(id-1)*max_h);
    this->add_child(labels[arrow->getId()]);
//#endif

    // shifting down other contacts and labels
    Gdk::Point curPt = contacts[arrow->getId()];
    //std::map<std::string, Gdk::Point>::iterator itr;        
    for(itr=contacts.begin(); itr!=contacts.end(); itr++)
    {
        if(itr->first != arrow->getId())
        {
            Gdk::Point pt = itr->second;
            if(pt.get_y() >= curPt.get_y())
                itr->second = Gdk::Point(pt.get_x(), pt.get_y()+(max_h + V_MARGINE));         
        }
    }

    // shifting down text position
    std::map<std::string, Glib::RefPtr<Goocanvas::TextModel> >::iterator itr2;        
    int curY = (int)labels[arrow->getId()]->property_y().get_value();
    for(itr2=labels.begin(); itr2!=labels.end(); itr2++)
    {
        if(itr2->first != arrow->getId())
        {
            Glib::RefPtr<Goocanvas::TextModel> text = itr2->second;
            if(text->property_y() >= curY)
                text->property_y().set_value(text->property_y().get_value() + (max_h + V_MARGINE));
        } 
    }

    bool ret = PortModel::addDestinationArrow(arrow);
    
    getContactPoint(arrow);

    return ret;
}


Arbitrator& PortArbitratorModel::setArbitrator(Arbitrator& arb)
{
    Application* application = parentWindow->manager.getKnowledgeBase()->getApplication();
    arbitrator = parentWindow->manager.getKnowledgeBase()->addArbitratorToApplication(application, arb);
    return arbitrator;
}

void PortArbitratorModel::setPort(const char* szPort)
{
    Glib::RefPtr<ExternalPortModel> port = Glib::RefPtr<ExternalPortModel>::cast_dynamic(destination);
    if(port)
    {
        port->setPort(szPort);
        std::vector<ArrowModel*>::iterator itr;   
        for(itr = destinationArrows.begin(); itr!= destinationArrows.end(); itr++)
        {
            Connection* pConnection = (*itr)->getConnection();
            Connection con = *pConnection;
            con.setTo(szPort);
            Application* application = parentWindow->manager.getKnowledgeBase()->getApplication();
            parentWindow->manager.getKnowledgeBase()->updateConnectionOfApplication(application, *pConnection, con);
            pConnection->setTo(szPort);
        }
    }        
}

void PortArbitratorModel::updateExcitation(ArrowModel* arrow, const char* szOld, const char* szNew)
{
    // rnaming exictatory link to this connection from the other
    std::vector<ArrowModel*>::iterator jtr;
    for(jtr=destinationArrows.begin(); jtr!=destinationArrows.end(); jtr++)
        if((*jtr) != arrow)
            (*jtr)->renameExcitation(szOld, szNew); 
}


bool PortArbitratorModel::removeSourceArrow(ArrowModel* arrow)
{
    contacts.erase(contacts.find(arrow->getId()));
    return PortModel::removeSourceArrow(arrow);
}

bool PortArbitratorModel::removeDestinationArrow(ArrowModel* arrow)
{
    Gdk::Point ptRemoved = contacts[arrow->getId()];

    arbitrator.removeRule(arrow->getId());
    Application* application = parentWindow->manager.getKnowledgeBase()->getApplication();
    arbitrator = parentWindow->manager.getKnowledgeBase()->addArbitratorToApplication(application, arbitrator);

    contacts.erase(contacts.find(arrow->getId()));
    //removing the label 
    int id = this->find_child(labels[arrow->getId()]);
    if(id != -1) this->remove_child(id);
    labels.erase(labels.find(arrow->getId()));

    // updating width and height
    int max_w = 0;
    int max_h = 0;
    std::map<std::string, Gdk::Point>::iterator itr;        
    for(itr=contacts.begin(); itr!=contacts.end(); itr++)
    {
        int text_w, text_h;
        PangoLayout *layout = gtk_widget_create_pango_layout((GtkWidget*)parentWindow->gobj(), (itr->first).c_str());
        PangoFontDescription *fontdesc = pango_font_description_from_string(FONT_DESC);
        pango_layout_set_font_description (layout, fontdesc);
        pango_layout_get_pixel_size (layout, &text_w, &text_h);
        max_w = (max_w<text_w) ? text_w : max_w; 
        max_h = (max_h<text_h) ? text_h : max_h; 
        //updating contact points
        Gdk::Point pt = itr->second;
        if(pt.get_y() > ptRemoved.get_y())
            itr->second = Gdk::Point(pt.get_x(),pt.get_y()-(text_h + V_MARGINE));         
    }

    width = max_w + H_MARGINE*2;
    height = max_h*(contacts.size()-1) + V_MARGINE*(contacts.size());

    Gdk::Point defSize = getDefaultSize();
    width = (width<defSize.get_x()) ? defSize.get_x() : width;
    height = (height<defSize.get_y()) ? defSize.get_y() : height;

    Goocanvas::Points points(7);
    points.set_coordinate(0, 0, 0);
    points.set_coordinate(1, width, 0);
    points.set_coordinate(2, width, height/2.0-5);
    points.set_coordinate(3, width+5, height/2.0);
    points.set_coordinate(4, width, height/2.0+5);
    points.set_coordinate(5, width, height);
    points.set_coordinate(6, 0, height);
    poly->property_points().set_value(points);
    shadow->property_points().set_value(points);

        // updating text position
    std::map<std::string, Glib::RefPtr<Goocanvas::TextModel> >::iterator itr2;        
    for(itr2=labels.begin(); itr2!=labels.end(); itr2++)
    {
        Glib::RefPtr<Goocanvas::TextModel> text = itr2->second;
        if(text->property_y().get_value() > ptRemoved.get_y())
            text->property_y().set_value(text->property_y().get_value() - (max_h + V_MARGINE));
        //itr->second = text;
    }

    // removing exictatory link to this connection from the other
    std::vector<ArrowModel*>::iterator jtr;
    for(jtr=destinationArrows.begin(); jtr!=destinationArrows.end(); jtr++)
        if((*jtr) != arrow)
            (*jtr)->removeExcitation(arrow->getConnection()->from());
    
    bool ret = PortModel::removeDestinationArrow(arrow);
    updateArrowCoordination();
    return ret;
}

Gdk::Point PortArbitratorModel::getDefaultSize(void)
{
    PangoLayout *layout = gtk_widget_create_pango_layout((GtkWidget*)parentWindow->gobj(), "O1");
    int text_w, text_h;
    PangoFontDescription *fontdesc = pango_font_description_from_string(FONT_DESC);
    pango_layout_set_font_description (layout, fontdesc);
    pango_layout_get_pixel_size (layout, &text_w, &text_h);
    int width =  text_w + H_MARGINE*2;
    int height = text_h+ V_MARGINE*2;
    return Gdk::Point(width, height);
}

Glib::RefPtr<PortArbitratorModel> PortArbitratorModel::create(ApplicationWindow* parentWnd, 
                                                              Glib::RefPtr<PortModel> dest, Arbitrator* arb, bool nested)
{
    return Glib::RefPtr<PortArbitratorModel>(new PortArbitratorModel(parentWnd, dest, arb, nested));
}


bool PortArbitratorModel::onItemButtonPressEvent(const Glib::RefPtr<Goocanvas::Item>& item, 
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
        /*
        MainWindow* wnd = parentWindow->getMainWindow();
        wnd->m_refActionGroup->get_action("EditDelete")->set_sensitive(selected);
        Gtk::Menu* pMenu = dynamic_cast<Gtk::Menu*>(
        wnd->m_refUIManager->get_widget("/PopupExtPortModel"));
        if(pMenu)
            pMenu->popup(event->button, event->time);
        */            
    }


    return true;
}

bool PortArbitratorModel::onItemButtonReleaseEvent(const Glib::RefPtr<Goocanvas::Item>& item, 
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

bool PortArbitratorModel::onItemMotionNotifyEvent(const Glib::RefPtr<Goocanvas::Item>& item, 
                    GdkEventMotion* event)
{
    if(bNested)
        return true;

    if(item && _dragging && item == _dragging)
    {
        parentWindow->setModified();
        
        //Goocanvas::Bounds biBase = item->get_bounds();

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

        bi = item->get_parent()->get_bounds();
        this->points.clear();
        GyPoint pt;
        pt.x = bi.get_x1();
        pt.y = bi.get_y1();
        this->points.push_back(pt);

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

bool PortArbitratorModel::onItemEnterNotify(const Glib::RefPtr<Goocanvas::Item>& item, 
                    GdkEventCrossing* event)
{
    parentWindow->get_window()->set_cursor(Gdk::Cursor(Gdk::HAND1));

    // do not change the color in error mode
    if(portColor != ARBITRATOR_COLOR)
        return true;
    poly->property_fill_color().set_value("LightSteelBlue");
    return true;
}

bool PortArbitratorModel::onItemLeaveNotify(const Glib::RefPtr<Goocanvas::Item>& item, 
                    GdkEventCrossing* event)
{
    parentWindow->get_window()->set_cursor();
    if(bNested)
    {
        poly->property_stroke_color().set_value("black");
        poly->property_fill_color().set_value(portColor.c_str());
        return true;
    }

    if(!selected)
    {
        poly->property_stroke_color().set_value("black");
        poly->property_fill_color().set_value(portColor.c_str());
    }
    return true;
}

void PortArbitratorModel::updateArrowCoordination(void)
{      
   std::vector<ArrowModel*>::iterator itr;
   for(itr = sourceArrows.begin(); itr!= sourceArrows.end(); itr++)
        (*itr)->updatCoordiantes();                           
   for(itr = destinationArrows.begin(); itr!= destinationArrows.end(); itr++)
    (*itr)->updatCoordiantes();
}

void PortArbitratorModel::setArrowsSelected(bool sel)
{

    std::vector<ArrowModel*>::iterator itr;
    for(itr = sourceArrows.begin(); itr!= sourceArrows.end(); itr++)
        (*itr)->setSelected(true);
    for(itr = destinationArrows.begin(); itr!= destinationArrows.end(); itr++)
        (*itr)->setSelected(true);
}

void PortArbitratorModel::setSelected(bool sel)
{
    selected = sel;

    // do not change the color in error mode
    if(portColor != ARBITRATOR_COLOR)
        return;

    if(selected)
    {
        poly->property_fill_color().set_value("LightSteelBlue");
        this->raise();
        std::vector<ArrowModel*>::iterator itr;
    }
    else
        poly->property_fill_color().set_value(ARBITRATOR_COLOR);
}


void PortArbitratorModel::snapToGrid(void)
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
        this->points.clear();
        GyPoint pt;
        pt.x = bi.x1;
        pt.y = bi.y1;
        this->points.push_back(pt);
        updateArrowCoordination();
    }
}


