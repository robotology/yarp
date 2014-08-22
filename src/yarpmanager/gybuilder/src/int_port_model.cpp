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

#include "int_port_model.h"
#include "application_window.h"
#include "main_window.h"
#include <goocanvas.h>

using namespace yarp::manager;


InternalPortModel::InternalPortModel(ApplicationWindow* parentWnd,
                   NodeType t, void* data) : PortModel(t)
{
    parentWindow = parentWnd;
    input  = NULL;
    output = NULL;
    bServicePort = false;

    strColor = COLOR_NOMAL;
    Goocanvas::Points points(8);
    if(type == INPUTD)
    {
        input = (InputData*) data;
        input->setModel(this);
        tool = TooltipModel::create(parentWindow, input->getPort());
        if(input->isRequired())
            strColor = COLOR_WARNING;

        if(input->getPortType() == STREAM_PORT)
        {
            points.set_coordinate(0, 0, 0);
            points.set_coordinate(1, 10, PORT_SIZE/2.0);
            points.set_coordinate(2, 15, PORT_SIZE/2.0);
            points.set_coordinate(3, 10, PORT_SIZE/2.0);
            points.set_coordinate(4, 0, PORT_SIZE);
            points.set_coordinate(5, 0, 0);
            points.set_coordinate(6, 0, 0);     // redundant
            points.set_coordinate(7, 0, 0);     // redundant
        }
        else if(input->getPortType() == EVENT_PORT)
        {
            points.set_coordinate(0, 0, 0);
            points.set_coordinate(1, 10, 0);
            points.set_coordinate(2, 10, PORT_SIZE/2.0);
            points.set_coordinate(3, 15, PORT_SIZE/2.0);
            points.set_coordinate(4, 10, PORT_SIZE/2.0);
            points.set_coordinate(5, 10, PORT_SIZE);
            points.set_coordinate(6, 0, PORT_SIZE);
            points.set_coordinate(7, 5, PORT_SIZE/2.0);
        }
        else if(input->getPortType() == SERVICE_PORT)
        {
            bServicePort = true;
            poly = Goocanvas::PolylineModel::create(0, PORT_SIZE/2.0, 15, PORT_SIZE/2.0);
            poly->property_close_path().set_value(false);
            poly->property_line_width().set_value(1.0);
            poly->property_stroke_color().set_value("black");
            this->add_child(poly);
            ellipse = Goocanvas::EllipseModel::create(5, PORT_SIZE/2.0, 5, 5);
            ellipse->property_line_width().set_value(1.0);
            ellipse->property_fill_color().set_value(strColor.c_str());
            ellipse->property_stroke_color().set_value("black");
            this->add_child(ellipse);
        }
    }
    else
    {
        output = (OutputData*) data;
        output->setModel(this);
        tool = TooltipModel::create(parentWindow, output->getPort());

        if(output->getPortType() == STREAM_PORT)
        {
            points.set_coordinate(0, 0, PORT_SIZE/2.0);
            points.set_coordinate(1, 5, PORT_SIZE/2.0);
            points.set_coordinate(2, 5, 0);
            points.set_coordinate(3, 15, PORT_SIZE/2.0);
            points.set_coordinate(4, 5, PORT_SIZE);
            points.set_coordinate(5, 5, PORT_SIZE/2.0);
            points.set_coordinate(6, 5, PORT_SIZE/2.0);     // redundant
            points.set_coordinate(7, 5, PORT_SIZE/2.0);     // redundant
        }
        else if(output->getPortType() == EVENT_PORT)
        {
            points.set_coordinate(0, 0, PORT_SIZE/2.0);
            points.set_coordinate(1, 5, PORT_SIZE/2.0);
            points.set_coordinate(2, 5, 0);
            points.set_coordinate(3, 10, 0);
            points.set_coordinate(4, 15, PORT_SIZE/2.0);
            points.set_coordinate(5, 10, PORT_SIZE);
            points.set_coordinate(6, 5, PORT_SIZE);
            points.set_coordinate(7, 5, PORT_SIZE/2.0);
        }
        else if(output->getPortType() == SERVICE_PORT)
        {
            bServicePort = true;
            poly = Goocanvas::PolylineModel::create(0, PORT_SIZE/2.0, 5, PORT_SIZE/2.0);
            poly->property_close_path().set_value(false);
            poly->property_line_width().set_value(1.0);
            poly->property_stroke_color().set_value("black");
            this->add_child(poly);
            char str[128];
            sprintf(str, "M10,2 a5,5 0 0,0 0,10");
            path = Goocanvas::PathModel::create(Glib::ustring(str));
            path->property_line_width().set_value(1.0);
            //path->property_fill_color().set_value(strColor.c_str());
            path->property_stroke_color().set_value("black");
            this->add_child(path);
        }
    }


    if(!bServicePort)
    {
        poly = Goocanvas::PolylineModel::create(0,0,0,0);
        poly->property_points().set_value(points);
        poly->property_close_path().set_value(true);
        poly->property_fill_color().set_value(strColor.c_str());
        poly->property_line_width().set_value(1.0);
        poly->property_stroke_color().set_value("black");
        this->add_child(poly);
    }
}


InternalPortModel::~InternalPortModel(void)
{
    if(tool)
        tool.clear();
}

Gdk::Point InternalPortModel::getContactPoint(ArrowModel* arrow)
{
    GooCanvas* canvas = (GooCanvas*) parentWindow->m_Canvas->gobj();
    if(!bServicePort)
    {
        GooCanvasItemModel* model = (GooCanvasItemModel*) poly->gobj();
        if(model && canvas)
        {
            GooCanvasItem* item = goo_canvas_get_item(canvas, model);
            if(item)
            {
                GooCanvasBounds bi;
                goo_canvas_item_get_bounds(item, &bi);
                if(type == INPUTD)
                    return Gdk::Point((int)(bi.x1+1), (int)(bi.y2-PORT_SIZE/2.0));
                return Gdk::Point((int)(bi.x2-1), (int)(bi.y2-PORT_SIZE/2.0));
            }
        }
    }
    else
    {
        if(type == INPUTD)
        {
            GooCanvasItemModel* model = (GooCanvasItemModel*) ellipse->gobj();
            if(model && canvas)
            {
                GooCanvasItem* item = goo_canvas_get_item(canvas, model);
                if(item)
                {
                    GooCanvasBounds bi;
                    goo_canvas_item_get_bounds(item, &bi);
                    return Gdk::Point((int)(bi.x1), (int)(bi.y2-5));
                }
            }
        }
        else
        {
            GooCanvasItemModel* model = (GooCanvasItemModel*) path->gobj();
            if(model && canvas)
            {
                GooCanvasItem* item = goo_canvas_get_item(canvas, model);
                if(item)
                {
                    GooCanvasBounds bi;
                    goo_canvas_item_get_bounds(item, &bi);
                    return Gdk::Point((int)(bi.x2), (int)(bi.y2-5));
                }
            }
        }
    }
    return Gdk::Point(-1, -1);
}



Glib::RefPtr<InternalPortModel> InternalPortModel::create(ApplicationWindow* parentWnd, NodeType t, void* data)
{
    return Glib::RefPtr<InternalPortModel>(new InternalPortModel(parentWnd, t, data));
}


bool InternalPortModel::onItemButtonPressEvent(const Glib::RefPtr<Goocanvas::Item>& item,
                    GdkEventButton* event)
{
    if((event->button == 3) && (getType() == INPUTD))
    {
        MainWindow* wnd = parentWindow->getMainWindow();
        parentWindow->m_currentPortModel = Glib::RefPtr<PortModel>::cast_dynamic(item->get_parent()->get_model());
        wnd->m_refActionGroup->get_action("InsertPortArbitrator")->set_sensitive(true);
        Gtk::Menu* pMenu = dynamic_cast<Gtk::Menu*>(
        wnd->m_refUIManager->get_widget("/PopupIntPortModel"));
        if(pMenu)
            pMenu->popup(event->button, event->time);
    }
    return true;
}

bool InternalPortModel::onItemButtonReleaseEvent(const Glib::RefPtr<Goocanvas::Item>& item,
                    GdkEventButton* event)
{
    if(event->button == 1)
    {
    }
    return true;
}

bool InternalPortModel::onItemMotionNotifyEvent(const Glib::RefPtr<Goocanvas::Item>& item,
                    GdkEventMotion* event)
{
    if(item)
    {

    }
    return true;
}

bool InternalPortModel::onItemEnterNotify(const Glib::RefPtr<Goocanvas::Item>& item,
                    GdkEventCrossing* event)
{
    parentWindow->get_window()->set_cursor(Gdk::Cursor(Gdk::HAND1));
    //this->property_stroke_color().set_value("DodgerBlue3");
    if(!bServicePort)
        poly->property_fill_color().set_value("LightSteelBlue");
    else if(type == INPUTD)
        ellipse->property_fill_color().set_value("LightSteelBlue");


    parentWindow->getRootModel()->add_child(tool);
    Gdk::Point pt = getContactPoint();
    if(type == OUTPUTD)
    {
        tool->set_property("x", pt.get_x()+5);
        tool->set_property("y", pt.get_y()-tool->getHeight()/2.0);
    }
    else
    {
        tool->set_property("x", pt.get_x()-tool->getWidth()-5);
        tool->set_property("y", pt.get_y()-tool->getHeight()/2.0);
    }
    tool->raise();

    return true;
}

bool InternalPortModel::onItemLeaveNotify(const Glib::RefPtr<Goocanvas::Item>& item,
                    GdkEventCrossing* event)
{
    parentWindow->get_window()->set_cursor();

    if(!bServicePort)
    {
        poly->property_stroke_color().set_value("black");
        poly->property_fill_color().set_value(strColor.c_str());
    }
    else if(type == INPUTD)
        ellipse->property_fill_color().set_value(strColor.c_str());

    int id = parentWindow->getRootModel()->find_child(tool);
    if(id != -1)
        parentWindow->getRootModel()->remove_child(id);
    return true;
}

void InternalPortModel::updateInputPortColor(void)
{
    if((type == INPUTD) && input && (string(input->getName()) != "*"))
    {
        strColor = COLOR_NOMAL;
        if(!destinationArrows.size())
            strColor = (input->isRequired()) ? COLOR_WARNING : COLOR_NOMAL;
        else
        {
            std::vector<ArrowModel*>::iterator itr;
            for(itr=destinationArrows.begin(); itr<destinationArrows.end(); itr++)
                if((*itr)->getSource() &&
                   Glib::RefPtr<InternalPortModel>::cast_dynamic((*itr)->getSource()))
                {
                    OutputData* outp = Glib::RefPtr<InternalPortModel>::cast_dynamic((*itr)->getSource())->getOutput();
                    if(outp && !compareString(outp->getName(), input->getName()))
                    {
                        strColor = COLOR_MISMATCH;
                        break;
                    }
                }
        }
        poly->property_fill_color().set_value(strColor.c_str());
    }
}

void InternalPortModel::updateOutputPortColor(void)
{
    if(type == OUTPUTD)
    {
        strColor = COLOR_NOMAL;
        std::vector<ArrowModel*>::iterator itr;
        for(itr=sourceArrows.begin(); itr<sourceArrows.end(); itr++)
            if((*itr)->getDestination() &&
                Glib::RefPtr<InternalPortModel>::cast_dynamic((*itr)->getDestination()))
            {
                InputData* inp = Glib::RefPtr<InternalPortModel>::cast_dynamic((*itr)->getDestination())->getInput();
                if(inp && (string(inp->getName()) != "*") &&
                   !compareString(output->getName(), inp->getName()))
                {
                    strColor = COLOR_MISMATCH;
                    break;
                }
            }
        poly->property_fill_color().set_value(strColor.c_str());
    }
}

