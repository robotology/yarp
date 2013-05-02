/*
 *  Yarp Modules Manager
 *  Copyright: 2011 (C) Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include "port_model.h"
#include "application_window.h"

PortModel::PortModel(NodeType t) : GroupModel()
{
    type = t;
}


PortModel::~PortModel(void) 
{
}


Glib::RefPtr<PortModel> PortModel::create(NodeType t)
{
    return Glib::RefPtr<PortModel>(new PortModel(t));
}

Gdk::Point PortModel::getContactPoint(ArrowModel* arrow)
{
    return Gdk::Point(-1, -1);
}


bool PortModel::onItemButtonPressEvent(const Glib::RefPtr<Goocanvas::Item>& item, 
                    GdkEventButton* event)
{
    return true;
}

bool PortModel::onItemButtonReleaseEvent(const Glib::RefPtr<Goocanvas::Item>& item, 
                    GdkEventButton* event)
{
   return true;
}

bool PortModel::onItemMotionNotifyEvent(const Glib::RefPtr<Goocanvas::Item>& item, 
                    GdkEventMotion* event)
{
   return true;
}

bool PortModel::onItemEnterNotify(const Glib::RefPtr<Goocanvas::Item>& item, 
                    GdkEventCrossing* event)
{
    return true;
}

bool PortModel::onItemLeaveNotify(const Glib::RefPtr<Goocanvas::Item>& item, 
                    GdkEventCrossing* event)
{
    return true;
}

void PortModel::setSelected(bool sel) 
{ 
}

bool PortModel::getSelected(void) 
{
    return false;
}

bool PortModel::addSourceArrow(ArrowModel* arrow)
{
    if(type == INPUTD)
        return false;
    sourceArrows.push_back(arrow); 
    onSourceAdded();
    return true;
}

bool PortModel::addDestinationArrow(ArrowModel* arrow)
{
    if(type == OUTPUTD)
        return false;
    destinationArrows.push_back(arrow); 
    onDestinationAdded();
    return true;
}

bool PortModel::removeSourceArrow(ArrowModel* arrow)
{
    std::vector<ArrowModel*>::iterator itr;
    for(itr=sourceArrows.begin(); itr!=sourceArrows.end(); itr++)
        if((*itr) == arrow)
        {
            sourceArrows.erase(itr);
            onSourceRemoved();
            return true;
        }
    return false;
}

bool PortModel::removeDestinationArrow(ArrowModel* arrow)
{
    std::vector<ArrowModel*>::iterator itr;
    for(itr=destinationArrows.begin(); itr!=destinationArrows.end(); itr++)
        if((*itr) == arrow)
        {
            destinationArrows.erase(itr);
            onDestinationRemoved();
            return true;
        }
    return false;
}


