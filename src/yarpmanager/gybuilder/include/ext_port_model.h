/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _EXTPORT_MODEL__
#define _EXTPORT_MODEL__

#include <vector>
#include <goocanvasmm.h>
#include <goocanvasrect.h>
#include <yarp/manager/ymm-types.h>
#include <yarp/manager/data.h>
#include "arrow_model.h"
#include "port_model.h"

class ApplicationWindow;

class ExternalPortModel : public PortModel

{
public:
    virtual ~ExternalPortModel();

    static Glib::RefPtr<ExternalPortModel> create(ApplicationWindow* parentWnd, NodeType t, const char* szExternalPort, bool nested=false);

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

    void snapToGrid(void);
    virtual void updateArrowCoordination(void);
    void setArrowsSelected(bool sel);

    virtual void setSelected(bool sel);
    virtual bool getSelected(void) { 
        return selected; 
    }  

    const char* getPort() { return strPort.c_str(); }
    void setPort(const char* szPort);
    

    double getWidth(void) { return width; }
    double getHeight(void) { return height; }
    bool isNested(void) { return bNested; }

protected: 
    ExternalPortModel(ApplicationWindow* parentWnd, NodeType t, const char* szExternalPort, bool nested=false);

private:
    ApplicationWindow* parentWindow;
    std::string strPort;
    bool selected;
    Glib::RefPtr<Goocanvas::PolylineModel> poly;
    Glib::RefPtr<Goocanvas::TextModel> text;
    Glib::RefPtr<Goocanvas::PolylineModel> shadow;
    Glib::RefPtr< Goocanvas::Item > _dragging;
    int _drag_x ;
    int _drag_y ;
    double width;
    double height;
    bool bNested;
};

#endif //_EXTPORT_MODEL_

