/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _PORT_MODEL__
#define _PORT_MODEL__

#include <vector>
#include <goocanvasmm.h>
#include <goocanvasrect.h>
#include <yarp/manager/ymm-types.h>
#include "arrow_model.h"
#include <yarp/manager/data.h>


class ApplicationWindow;

class PortModel : public Goocanvas::GroupModel, public yarp::manager::GraphicModel
{
public:
    virtual ~PortModel();

    static Glib::RefPtr<PortModel> create(yarp::manager::NodeType t=yarp::manager::INPUTD);

    virtual bool onItemButtonPressEvent(const Glib::RefPtr<Goocanvas::Item>& item, 
                        GdkEventButton* event);
    virtual bool onItemButtonReleaseEvent(const Glib::RefPtr<Goocanvas::Item>& item, 
                        GdkEventButton* event);
    virtual bool onItemMotionNotifyEvent(const Glib::RefPtr<Goocanvas::Item>& item, 
                        GdkEventMotion* event);
    virtual bool onItemEnterNotify(const Glib::RefPtr<Goocanvas::Item>& item, 
                        GdkEventCrossing* event);
    virtual bool onItemLeaveNotify(const Glib::RefPtr<Goocanvas::Item>& item, 
                        GdkEventCrossing* event);

    virtual Gdk::Point getContactPoint(ArrowModel* arrow=NULL);
    virtual void updateArrowCoordination(void) { };

    yarp::manager::NodeType getType(void) { return type;}

    virtual void setSelected(bool sel);
    virtual bool getSelected(void);
    virtual bool addSourceArrow(ArrowModel* arrow);
    virtual bool addDestinationArrow(ArrowModel* arrow);
    virtual bool removeSourceArrow(ArrowModel* arrow);
    virtual bool removeDestinationArrow(ArrowModel* arrow);
   
    std::vector<ArrowModel*>& getSourceArrows(void) { return sourceArrows; }
    std::vector<ArrowModel*>& getDestinationArrows(void) { return destinationArrows; }

    bool hasArbitrator(void) {
        std::vector<ArrowModel*>::iterator itr;
        for(itr=destinationArrows.begin(); itr!=destinationArrows.end(); itr++)
            if((*itr)->isNullArrow())
                return true;
        return false;
    }
    
    Glib::RefPtr<PortModel> getArbitrator(void) {
        std::vector<ArrowModel*>::iterator itr;
        for(itr=destinationArrows.begin(); itr!=destinationArrows.end(); itr++)
        {
            if((*itr)->isNullArrow())
                return (*itr)->getSource();
        }
        return Glib::RefPtr<PortModel>(NULL);
    }

protected: 
    PortModel(yarp::manager::NodeType t=yarp::manager::INPUTD);
    virtual void onSourceAdded(void) {}
    virtual void onSourceRemoved(void) {}
    virtual void onDestinationAdded(void) {}
    virtual void onDestinationRemoved(void) {}

protected:
    yarp::manager::NodeType type;
    std::vector<ArrowModel*> sourceArrows;
    std::vector<ArrowModel*> destinationArrows;
};

#endif //_PORT_MODEL_

