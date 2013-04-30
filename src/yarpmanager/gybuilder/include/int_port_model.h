/*
 *  Yarp Modules Manager
 *  Copyright: 2011 (C) Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _INPORT_MODEL__
#define _INPORT_MODEL__

#include <vector>
#include <goocanvasmm.h>
#include <goocanvasrect.h>
#include "ymm-types.h"
#include "port_model.h"
#include "arrow_model.h"
#include "data.h"
#include "tooltip_model.h"
#include "port_model.h"

#define PORT_SIZE           15
#define COLOR_NOMAL         "darkgray"   
#define COLOR_WARNING       "IndianRed1"
#define COLOR_MISMATCH      "#FFC125" //"Orange"



class ApplicationWindow;

class InternalPortModel : public PortModel
{
public:
    virtual ~InternalPortModel();

    static Glib::RefPtr<InternalPortModel> create(ApplicationWindow* parentWnd, NodeType t=INPUTD, 
                                          void* data=NULL);

    virtual  bool onItemButtonPressEvent(const Glib::RefPtr<Goocanvas::Item>& item, 
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

    InputData* getInput(void) { return input; }
    OutputData* getOutput(void) { return output; }
    
protected: 
    InternalPortModel(ApplicationWindow* parentWnd, NodeType t=INPUTD, void* data=NULL);

    virtual void onSourceAdded(void) {
        updateOutputPortColor();
    }
    virtual void onSourceRemoved(void) {
        updateOutputPortColor();
    }

    virtual void onDestinationAdded(void) {
        updateInputPortColor();
    }

    virtual void onDestinationRemoved(void) {
        updateInputPortColor();        
    }

private: 
    void updateInputPortColor(void);
    void updateOutputPortColor(void);

private:
    Glib::RefPtr<Goocanvas::PolylineModel> poly;
    ApplicationWindow* parentWindow;
    InputData* input;
    OutputData* output;
    Glib::RefPtr<TooltipModel> tool;
    string strColor;
};

#endif //_INPORT_MODEL_

