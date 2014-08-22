/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 *
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _APPLICATION_MODEL__
#define _APPLICATION_MODEL__

#include <goocanvasmm.h>
#include <goocanvasrect.h>
#include <yarp/manager/application.h>
#include "port_model.h"
#include "ext_port_model.h"

class ApplicationWindow;

class ApplicationModel : public Goocanvas::GroupModel, public yarp::manager::GraphicModel
{
public:
    virtual ~ApplicationModel();

    static Glib::RefPtr<ApplicationModel> create(ApplicationWindow* parentWnd, yarp::manager::Application* app);

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
    void updateCoordination(void);
    void updateChildItems(void);

    void setSelected(bool sel);
    bool getSelected(void) { return selected; }
    void setArrowsSelected(bool sel);
    yarp::manager::Application* getApplication(void) { return application; }

    double getWidth(void) { return width; }
    double getHeight(void) { return height; }

    void releaseApplication(void);

protected:
    ApplicationModel(ApplicationWindow* parentWnd, yarp::manager::Application* app);

private:
    yarp::manager::Application* application;
    ApplicationWindow* parentWindow;
    Glib::RefPtr<Goocanvas::PolylineModel> poly;
    Glib::RefPtr<Goocanvas::TextModel> text;
    Glib::RefPtr<Goocanvas::PolylineModel> shadow;

    Glib::RefPtr<GroupModel> group;
    Glib::RefPtr< Goocanvas::Item > _dragging ;
    int _drag_x ;
    int _drag_y ;
    bool selected;
    double width;
    double height;
    int text_w;
    int text_h;
    Goocanvas::Bounds bounds;

private:
    void updateBounds(void);
    void getApplicationBounds(Goocanvas::Bounds& bound);
    void findInputOutputData(yarp::manager::Connection& cnn,  yarp::manager::ModulePContainer &modules,
                                            yarp::manager::InputData* &input_, yarp::manager::OutputData* &output_);
    Glib::RefPtr<PortModel> findModelFromOutput(yarp::manager::OutputData* output);
    Glib::RefPtr<PortModel> findModelFromInput(yarp::manager::InputData* input);

};

#endif //_APPLICATION_MODEL_

