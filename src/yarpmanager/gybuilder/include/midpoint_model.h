/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _MIDPOINT_MODEL__
#define _MIDPOINT_MODEL__


#include <goocanvasmm.h>
#include <goocanvasrect.h>
#include "ymm-types.h"
#include "node.h"

#define POINT_SIZE      6


class ApplicationWindow;
class ArrowModel;

class MidpointModel : public Goocanvas::RectModel, public GraphicModel
{
public:
    virtual ~MidpointModel();

    static Glib::RefPtr<MidpointModel> create(ApplicationWindow* parentWnd,
                                           ArrowModel* arw, double x, double y);

    bool onItemButtonPressEvent(const Glib::RefPtr<Goocanvas::Item>& item, 
                        GdkEventButton* event);
    bool onItemButtonReleaseEvent(const Glib::RefPtr<Goocanvas::Item>& item, 
                        GdkEventButton* event);
    bool onItemMotionNotifyEvent(const Glib::RefPtr<Goocanvas::Item>& item, 
                        GdkEventMotion* event);
    bool onItemEnterNotify(const Glib::RefPtr<Goocanvas::Item>& item, 
                        GdkEventCrossing* event);
    bool onItemLeaveNotify(const Glib::RefPtr<Goocanvas::Item>& item, 
                        GdkEventCrossing* event);
    
    void snapToGrid(void);
    void updateCoordiantes(void);
    void setSelected(bool sel);
    bool getSelected(void) { return selected; }
    void remove();
    Gdk::Point getContactPoint(void);

    double get_x(void) { return point_x; } 
    double get_y(void) { return point_y; } 

protected: 
    MidpointModel(ApplicationWindow* parentWnd,
                  ArrowModel* arw, double x, double y);

private:
    ApplicationWindow* parentWindow;
    ArrowModel* arrow;
    double point_x;
    double point_y;
    bool selected;

    Glib::RefPtr< Goocanvas::Item > _dragging ;
    int _drag_x ;
    int _drag_y ;

};

#endif //_MIDPOINT_MODEL_

