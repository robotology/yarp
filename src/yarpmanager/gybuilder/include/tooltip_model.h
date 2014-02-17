/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _TOOLTIP_MODEL__
#define _TOOLTIP_MODEL__


#include <string>
#include <goocanvasmm.h>
#include <goocanvasrect.h>
#include <yarp/manager/ymm-types.h>
#include <yarp/manager/node.h>

class ApplicationWindow;
class ArrowModel;

class TooltipModel : public Goocanvas::GroupModel, public GraphicModel
{
public:
    virtual ~TooltipModel();

    static Glib::RefPtr<TooltipModel> create(ApplicationWindow* parentWnd,
                                             const char* text);

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
    
    double getWidth(void) { return width; }
    double getHeight(void) { return height; }

    //void snapToGrid(void);
    //void updateCoordiantes(void);
    //void setSelected(bool sel);
    //bool getSelected(void) { return selected; }
    //Gdk::Point getContactPoint(void);

protected: 
    TooltipModel(ApplicationWindow* parentWnd,
                 const char* text);

private:
    ApplicationWindow* parentWindow;
    std::string tooltip; 
    double width;
    double height;
};

#endif //_TOOLTIP_MODEL_

