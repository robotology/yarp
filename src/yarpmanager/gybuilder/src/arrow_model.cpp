/*
 *  Yarp Modules Manager
 *  Copyright: 2011 (C) Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include "arrow_model.h"
#include "application_window.h"
#include "main_window.h"
#include "port_model.h"
#include "midpoint_model.h"
#include "label_model.h"
#include "int_port_model.h"
#include "ext_port_model.h"
#include "application_model.h"

#include <math.h>
#if defined(_WIN32) || defined(_WIN64)
# define fmax max
# define fmin min
#endif

#define ARROW_LINEWIDTH     2.0


ArrowModel::ArrowModel(ApplicationWindow* parentWnd,
                       Glib::RefPtr<PortModel> src, Glib::RefPtr<PortModel> dest,
                       const char* szLabel, ApplicationModel* appModel): PolylineModel(0,0,0,0)
{
    parentWindow = parentWnd;
    source = src;
    destination = dest;
    selected = false;
    bExist = false;
    applicationModel = appModel; 
    bNested = (applicationModel != NULL);
    if(appModel)
        application = appModel->getApplication();
    else        
        application = parentWindow->manager.getKnowledgeBase()->getApplication();
    if(szLabel)
        strLabel = szLabel;
    this->property_close_path().set_value(false);
    this->property_line_width().set_value(ARROW_LINEWIDTH);
    this->property_arrow_width().set_value(5.0);
    this->property_end_arrow().set_value(true);
    if(bNested)
    {
        defaultColor = "#555555";
        GooCanvasLineDash *dash = goo_canvas_line_dash_new (ARROW_LINEWIDTH, 3.0, 3.0);
        g_object_set(this->gobj(), "line-dash", dash, NULL);
    }
    else
        defaultColor = "black";

    source->addSourceArrow(this);
    destination->addDestinationArrow(this);
  
    string strCarrier = strLabel;
    //Adding connection 
    if(application)
    {
        string strFrom, strTo;
        Glib::RefPtr<InternalPortModel> intPort;
        Glib::RefPtr<ExternalPortModel> extPort;
        Module* module;
        InputData* input = NULL;
        OutputData* output = NULL;
        // source 
        intPort = Glib::RefPtr<InternalPortModel>::cast_dynamic(source);
        if(intPort)
        {
            output = intPort->getOutput();
            module = (Module*) output->owner();
            strFrom = string(module->getPrefix()) + string(intPort->getOutput()->getPort()); 
            if(!strCarrier.size())            
                strCarrier = intPort->getOutput()->getCarrier();
        }
        else
        {
            extPort = Glib::RefPtr<ExternalPortModel>::cast_dynamic(source);
            strFrom = extPort->getPort();
        }
      
        // destination
        intPort = Glib::RefPtr<InternalPortModel>::cast_dynamic(destination);
        if(intPort)
        {
            input = intPort->getInput();
            module = (Module*) input->owner();
            strTo = string(module->getPrefix()) + string(intPort->getInput()->getPort()); 
            if(!strCarrier.size())            
                strCarrier = intPort->getInput()->getCarrier();
        }
        else
        {
            extPort = Glib::RefPtr<ExternalPortModel>::cast_dynamic(destination);
            strTo = extPort->getPort();
        }

        Connection cnn(strFrom.c_str(), strTo.c_str(), strCarrier.c_str());
        cnn.setCorOutputData(output);
        cnn.setCorInputData(input);
        cnn.setModel(this);
        connection = parentWindow->manager.getKnowledgeBase()->addConnectionToApplication(application, cnn);
    }

    this->property_stroke_color().set_value(defaultColor.c_str());
    if(strLabel.size())
        label = LabelModel::create(parentWindow, this, strLabel.c_str());
    else
        label = LabelModel::create(parentWindow, this, strCarrier.c_str());

    if(bNested)
    {
        applicationModel->add_child(label);
        label->property_visibility().set_value(Goocanvas::ITEM_HIDDEN);
    }
    else
        parentWindow->getRootModel()->add_child(label);
    label->raise();

    showLabel(parentWindow->m_showLabel);

    Gdk::Point pt1 = source->getContactPoint();
    Gdk::Point pt2 = destination->getContactPoint();
    setPoint(0, pt1.get_x(), pt1.get_y()-ARROW_LINEWIDTH/2.0);
    Goocanvas::Points points = this->property_points().get_value();
    setPoint(points.get_num_points()-1, pt2.get_x(), pt2.get_y()-ARROW_LINEWIDTH/2.0);
    updatLabelCoordiante();
}
    
void ArrowModel::setLabel(const char* szLabel)
{
    if(!szLabel)
        return;
    label->property_text() = szLabel;
}

ArrowModel::~ArrowModel(void) 
{
    //printf("%s\n",__PRETTY_FUNCTION__ );
    
    // removing label 
    int id = parentWindow->getRootModel()->find_child(label);
    if(id != -1)
        parentWindow->getRootModel()->remove_child(id);

    // removing midpoints
    vector<Glib::RefPtr<MidpointModel> >::iterator itr;
    for(itr=midpoints.begin(); itr!=midpoints.end(); itr++)
    {
        id = parentWindow->getRootModel()->find_child((*itr));
        if(id != -1)
            parentWindow->getRootModel()->remove_child(id);
    }
    midpoints.clear();

    // unregistering from source and destination 
    if(source)
        source->removeSourceArrow(this);
    if(destination)    
        destination->removeDestinationArrow(this);   
 
      if(application)
        parentWindow->manager.getKnowledgeBase()->removeConnectionFromApplication(application,
                                connection);
}

void ArrowModel::updatCoordiantes(void)
{
    if(!bNested)
    {
        Gdk::Point pt1 = source->getContactPoint();
        Gdk::Point pt2 = destination->getContactPoint();
        setPoint(0, pt1.get_x(), pt1.get_y()-ARROW_LINEWIDTH/2.0);
        Goocanvas::Points points = this->property_points().get_value();
        setPoint(points.get_num_points()-1, pt2.get_x(), pt2.get_y()-ARROW_LINEWIDTH/2.0);
        updatLabelCoordiante();
    }        
}


void ArrowModel::updatLabelCoordiante(void)
{
    double x1, y1, x2, y2;
    Goocanvas::Points points = this->property_points().get_value();
    int index = points.get_num_points() / 2;
    points.get_coordinate(index-1, x1, y1);
    points.get_coordinate(index, x2, y2);
    setLabelPosition((x1+x2)/2.0, (y1+y2)/2.0);
}

void ArrowModel::setLabelPosition(double x, double y)
{
    Glib::RefPtr<Goocanvas::Item> item = parentWindow->m_Canvas->get_item(label); 
    if(item)
    {
        Goocanvas::Bounds bi = item->get_bounds();
        bi = item->get_bounds();
        item->translate(x-bi.get_x1(), y-bi.get_y1());
    }
    onPointUpdated();
}

Gdk::Point ArrowModel::getPoint(int index)
{
    Goocanvas::Points points = this->property_points().get_value();
    if(index >= points.get_num_points())
        return Gdk::Point(-1,-1);
    double x, y;
    points.get_coordinate(index, x, y);
    return Gdk::Point(x,y);
}

int ArrowModel::getIndex(double x, double y)
{
    double x1, y1;
    Goocanvas::Points points = this->property_points().get_value();
    for(int i=0; i<points.get_num_points(); i++)
    {
        points.get_coordinate(i, x1, y1);
        if((x1 == x) && (y1 == y))
            return i;
    }
    
    return -1;
}


bool ArrowModel::setPoint(int index, double x, double y)
{
    Goocanvas::Points points = this->property_points().get_value();
    if(index >= points.get_num_points())
        return false;
    points.set_coordinate(index, x, y);
    this->property_points().set_value(points);
    onPointUpdated();
    return true;
}


bool ArrowModel::setPoint(double x, double y,
                          double x_new, double y_new)
{
    double x1, y1;
    Goocanvas::Points points = this->property_points().get_value();
    for(int i=0; i<points.get_num_points(); i++)
    {
        points.get_coordinate(i, x1, y1);
        if((x1 == x) && (y1 == y))
        {
            points.set_coordinate(i, x_new, y_new);
            break;
        }
    }
    this->property_points().set_value(points);
    onPointUpdated();
    return true;
}


int ArrowModel::addPoint(double x, double y)
{
    int index = 0;
    double x1, y1;
    double x2, y2;

    Goocanvas::Points points = this->property_points().get_value();
    for(int i=0; i<points.get_num_points()-1; i++)
    {
        points.get_coordinate(i, x1, y1);
        points.get_coordinate(i+1, x2, y2);
        if((x > fmin(x1, x2)) && (x < fmax(x1, x2)) &&
           (y > fmin(y1, y2)) && (y < fmax(y1, y2)))
        {   
            double d = ((x-x1)*(x2-x1) + (y-y1)*(y2-y1)) / ((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));
            if((d > 0.0) && (d < 1.0))
            {
                index = i;
                break;
            }
        }
    }

    addPoint(index, x, y);
    return index+1;
}

// add a new point after index
bool ArrowModel::addPoint(int index, double x, double y)
{
    Goocanvas::Points points = this->property_points().get_value();
    if(index >= points.get_num_points())
        return false;

    Goocanvas::Points new_points(points.get_num_points()+1);
    for(int i=0; i<=index; i++)
    {
        double _x, _y;
        points.get_coordinate(i, _x, _y);
        new_points.set_coordinate(i, _x, _y);
    }
    new_points.set_coordinate(index+1, x, y);
    for(int i=index+1; i<points.get_num_points(); i++)
    {
        double _x, _y;
        points.get_coordinate(i, _x, _y);
        new_points.set_coordinate(i+1, _x, _y);
    }
    this->property_points().set_value(new_points);
    onPointUpdated();
    return true;
}

void ArrowModel::onPointUpdated(void)
{
    GraphicModel::points.clear();
    double x1, y1;
    GyPoint pt;

    // Adding label position 
    Glib::RefPtr<Goocanvas::Item> item = parentWindow->m_Canvas->get_item(label); 
    if(item)
    {
        Goocanvas::Bounds bi = item->get_bounds();
        bi = item->get_bounds();
        pt.x = bi.get_x1();
        pt.y = bi.get_y1();
    }
    else
        pt.x = pt.y = -1;

    GraphicModel::points.push_back(pt);

    Goocanvas::Points pts = this->property_points().get_value();
    for(int i=0; i<pts.get_num_points(); i++)
    {
        pts.get_coordinate(i, x1, y1);
        pt.x = x1;
        pt.y = y1;
        GraphicModel::points.push_back(pt);
    }
}

Glib::RefPtr<ArrowModel> ArrowModel::create(ApplicationWindow* parentWnd,
                                    Glib::RefPtr<PortModel> src, Glib::RefPtr<PortModel> dest,
                                    const char* szLabel, ApplicationModel* appModel)
{
    return Glib::RefPtr<ArrowModel>(new ArrowModel(parentWnd, src, dest, szLabel, appModel));
}


bool ArrowModel::onItemButtonPressEvent(const Glib::RefPtr<Goocanvas::Item>& item, 
                    GdkEventButton* event)
{
    if(!item)
        return false;

    // right click on selected line 
    if((event->button == 3) && selected)
        addMidPoint(event->x, event->y);
    
    parentWindow->setModified();

    return true;
}

void ArrowModel::addMidPoint(double x, double y, int index)
{
    if(index >= 0)
        addPoint(index, x, y);
    else
        addPoint(x, y);

    if(!bNested)
    {
        Glib::RefPtr<MidpointModel> mid = MidpointModel::create(parentWindow, 
                                          this, x, y);
        parentWindow->getRootModel()->add_child(mid);        
        mid->set_property("x", x-POINT_SIZE/2.0);
        mid->set_property("y", y-POINT_SIZE/2.0);
        mid->snapToGrid();
        midpoints.push_back(mid);
    }        
}

bool ArrowModel::onItemButtonReleaseEvent(const Glib::RefPtr<Goocanvas::Item>& item, 
                    GdkEventButton* event)
{
  if(event->button == 1)
  {    
       // printf("released\n");
  }
  return true;
}

bool ArrowModel::onItemMotionNotifyEvent(const Glib::RefPtr<Goocanvas::Item>& item, 
                    GdkEventMotion* event)
{
    if(item)
    {
       // printf("motion\n");
    }
    return true;
}

bool ArrowModel::onItemEnterNotify(const Glib::RefPtr<Goocanvas::Item>& item, 
                    GdkEventCrossing* event)
{
    //parentWindow->get_window()->set_cursor(Gdk::Cursor(Gdk::HAND1));
    //this->property_stroke_color().set_value("red");
   // printf("entered\n");
    return true;
}

bool ArrowModel::onItemLeaveNotify(const Glib::RefPtr<Goocanvas::Item>& item, 
                    GdkEventCrossing* event)
{
    //parentWindow->get_window()->set_cursor();
    //this->property_stroke_color().set_value("black");
   // printf("left\n");
    return true;
}

bool ArrowModel::inside(double p1, double q1, double p2, double q2)
{
    double x1, y1;
    double x2, y2;
    Goocanvas::Points points = this->property_points().get_value();
    points.get_coordinate(0, x1, y1);
    points.get_coordinate(points.get_num_points()-1, x2, y2);
    x1 = fmin(x1, x2);
    x2 = fmax(x1, x2);
    y1 = fmin(y1, y2);
    y2 = fmax(y1, y2);

    if((x1>=p1) && (x1<=p2) && (y1>=q1) && (y2<=q2))
        return true;
 
    return false;
}

bool ArrowModel::intersect(double p1, double q1, double p2, double q2)
{
    double x1, y1;
    double x2, y2;
    Goocanvas::Points points = this->property_points().get_value();
    for(int i=0; i<points.get_num_points()-1; i++)
    {
        points.get_coordinate(i, x1, y1);
        points.get_coordinate(i+1, x2, y2);

        // calculte intersection 
        float s1_x, s1_y, s2_x, s2_y;
        s1_x = x2 - x1;     
        s1_y = y2 - y1;
        s2_x = p2 - p1;     
        s2_y = q2 - q1;

        float s, t;
        s = (-s1_y * (x1 - p1) + s1_x * (y1 - q1)) / (-s2_x * s1_y + s1_x * s2_y);
        t = ( s2_x * (y1 - q1) - s2_y * (x1 - p1)) / (-s2_x * s1_y + s1_x * s2_y);

        if ((s >= 0) && (s <= 1) && (t >= 0) && (t <= 1))
            return true;
    }

    return false;
}

void ArrowModel::showLabel(bool bShow)
{
    if(bShow && !bNested)
        label->property_visibility().set_value(Goocanvas::ITEM_VISIBLE);
    else
       label->property_visibility().set_value(Goocanvas::ITEM_HIDDEN);
}

void ArrowModel::setSelected(bool sel)
{
    if(bNested)
        return;

    selected = sel;
    if(selected)
    {
        this->property_stroke_color().set_value("DodgerBlue3");
        this->raise();
        label->setSelected(true);        
        vector<Glib::RefPtr<MidpointModel> >::iterator itr;
        for(itr=midpoints.begin(); itr!=midpoints.end(); itr++)
        {
            (*itr)->setSelected(false);
            (*itr)->property_visibility().set_value(Goocanvas::ITEM_VISIBLE);
            (*itr)->raise();
        }
    }
    else
    {
        this->property_stroke_color().set_value(defaultColor.c_str());
        label->setSelected(false); 
        vector<Glib::RefPtr<MidpointModel> >::iterator itr;
        for(itr=midpoints.begin(); itr!=midpoints.end(); itr++)
            (*itr)->property_visibility().set_value(Goocanvas::ITEM_HIDDEN);
    }
}

