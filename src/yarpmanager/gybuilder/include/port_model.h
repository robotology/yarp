/*
 *  Yarp Modules Manager
 *  Copyright: 2011 (C) Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
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
#include "ymm-types.h"
#include "arrow_model.h"
#include "data.h"


class ApplicationWindow;

class PortModel : public Goocanvas::GroupModel, public GraphicModel
{
public:
    virtual ~PortModel();

    static Glib::RefPtr<PortModel> create(NodeType t=INPUTD); 

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

    virtual Gdk::Point getContactPoint(void);
    virtual void updateArrowCoordination(void) { };

    NodeType getType(void) { return type;}

    bool addSourceArrow(ArrowModel* arrow);
    bool addDestinationArrow(ArrowModel* arrow);
    bool removeSourceArrow(ArrowModel* arrow);
    bool removeDestinationArrow(ArrowModel* arrow);
   
    std::vector<ArrowModel*>& getSourceArrows(void) { return sourceArrows; }
    std::vector<ArrowModel*>& getDestinationArrows(void) { return destinationArrows; }

protected: 
    PortModel(NodeType t=INPUTD);
    virtual void onSourceAdded(void) {}
    virtual void onSourceRemoved(void) {}
    virtual void onDestinationAdded(void) {}
    virtual void onDestinationRemoved(void) {}

protected:
    NodeType type;
    std::vector<ArrowModel*> sourceArrows;
    std::vector<ArrowModel*> destinationArrows;
};

#endif //_PORT_MODEL_

