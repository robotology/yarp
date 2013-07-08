/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _PORT_ARBITRATOR_MODEL__
#define _PORT_ARBITRATOR_MODEL__

#include <vector>
#include <goocanvasmm.h>
#include <goocanvasrect.h>
#include "ymm-types.h"
#include "arrow_model.h"
#include "data.h"
#include "port_model.h"

#define PORTARB_DEF_HEIGH   60
#define PORTARB_DEF_WIDTH   15
#define ARBITRATOR_COLOR    "WhiteSmoke"


class ApplicationWindow;

class PortArbitratorModel : public PortModel
{
public:
    virtual ~PortArbitratorModel();

    static Glib::RefPtr<PortArbitratorModel> create(ApplicationWindow* parentWnd,
                                                     Glib::RefPtr<PortModel> dest, Arbitrator* arb, bool nested=false);

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

    virtual Gdk::Point getContactPoint(ArrowModel* arrow);
    virtual bool addSourceArrow(ArrowModel* arrow);
    virtual bool addDestinationArrow(ArrowModel* arrow);
    virtual bool removeSourceArrow(ArrowModel* arrow);
    virtual bool removeDestinationArrow(ArrowModel* arrow);
    virtual void updateArrowCoordination(void);  
    void snapToGrid(void);
    void setArrowsSelected(bool sel);

    virtual void setSelected(bool sel);
    virtual bool getSelected(void) { 
        return selected; 
    }  


    Glib::RefPtr<PortModel> getPortModel() { return destination; }

    void setPort(const char* szPort);
    void updateExcitation(ArrowModel* arrow, const char* szOld, const char* szNew); 

    double getWidth(void) { return width; }
    double getHeight(void) { return height; }
    bool isNested(void) { return bNested; }
    Arbitrator& getArbitrator(void) { return arbitrator; }
    Arbitrator& setArbitrator(Arbitrator& arb);

    void setErrorMode(bool mode) {
        portColor = (mode) ? "#F9CCCA" : ARBITRATOR_COLOR;
        poly->property_fill_color().set_value(portColor.c_str());
    }

    ArrowModel* getArrowModel(const char* id) {
        std::vector<ArrowModel*>::iterator itr;
        for(itr = destinationArrows.begin(); itr!= destinationArrows.end(); itr++)
            if(compareString((*itr)->getId(), id))
                return (*itr);
        return NULL;
    }

protected: 
    PortArbitratorModel(ApplicationWindow* parentWnd, 
                        Glib::RefPtr<PortModel> dest, Arbitrator* arb, bool nested=false);
    Gdk::Point getDefaultSize(void);

private:
    bool selected;
    Arbitrator arbitrator;
    ApplicationWindow* parentWindow;
    Glib::RefPtr<PortModel> destination;
    Glib::RefPtr<Goocanvas::PolylineModel> poly;
    Glib::RefPtr<Goocanvas::PolylineModel> shadow;
    Glib::RefPtr< Goocanvas::Item > _dragging ;
    int _drag_x ;
    int _drag_y ;
    double width;
    double height;
    bool bNested;
    std::map<std::string, Gdk::Point> contacts;
    std::map<std::string, Glib::RefPtr<Goocanvas::TextModel> > labels;
    std::string portColor;
};

#endif //_PORT_ARBITRATOR_MODEL__

