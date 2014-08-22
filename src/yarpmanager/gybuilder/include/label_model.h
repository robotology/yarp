/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 *
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _LABEL_MODEL__
#define _LABEL_MODEL__


#include <string>
#include <goocanvasmm.h>
#include <goocanvasrect.h>
#include <yarp/manager/ymm-types.h>
#include <yarp/manager/node.h>


class ApplicationWindow;
class ArrowModel;

class LabelModel : public Goocanvas::TextModel, public yarp::manager::GraphicModel
{
public:
    virtual ~LabelModel();

    static Glib::RefPtr<LabelModel> create(ApplicationWindow* parentWnd,
                                           ArrowModel* arw, const char* text);

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

    Gdk::Point getContactPoint(void);

protected:
    LabelModel(ApplicationWindow* parentWnd,
                  ArrowModel* arw, const char* text);

private:
    ApplicationWindow* parentWindow;
    ArrowModel* arrow;
    bool selected;
    std::string label;

    Glib::RefPtr< Goocanvas::Item > _dragging ;
    int _drag_x ;
    int _drag_y ;

};

#endif //_LABEL_MODEL_

