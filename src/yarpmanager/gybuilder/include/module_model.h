/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 *
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _MODULE_MODEL__
#define _MODULE_MODEL__

#include <goocanvasmm.h>
#include <goocanvasrect.h>
#include <yarp/manager/module.h>

class ApplicationWindow;

class ModuleModel : public Goocanvas::GroupModel, public yarp::manager::GraphicModel
{
public:
    virtual ~ModuleModel();

    static Glib::RefPtr<ModuleModel> create(ApplicationWindow* parentWnd, yarp::manager::Module* mod, bool nested=false);

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
    void updateArrowCoordination(void);
    void setSelected(bool sel);
    bool getSelected(void) { return selected; }
    void setArrowsSelected(bool sel);
    yarp::manager::Module* getModule(void) { return module; }

    double getWidth(void) { return width; }
    double getHeight(void) { return height; }


protected:
    ModuleModel(ApplicationWindow* parentWnd, yarp::manager::Module* mod, bool nested);
    //void onItemCreated(const Glib::RefPtr<Goocanvas::Item>& item,
    //                    const Glib::RefPtr<Goocanvas::ItemModel>& model) ;
private:
    yarp::manager::Module* module;
    ApplicationWindow* parentWindow;
    Glib::RefPtr<Goocanvas::RectModel> mainRect;
    Glib::RefPtr<Goocanvas::RectModel> shadowRect;

    Glib::RefPtr<GroupModel> group;
    Glib::RefPtr< Goocanvas::Item > _dragging ;
    int _drag_x ;
    int _drag_y ;
    bool selected;
    double width;
    double height;
    bool bNested;
};

#endif //_MODULE_MODEL_

