/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _STDOUTWINDOW_H_
#define _STDOUTWINDOW_H_

#include <gtkmm.h>

//class StdoutWindow;
class StdoutWindow: public Gtk::Window 
{
public:
    StdoutWindow(Gtk::Widget* parent, int ID, const char* title=NULL);

    virtual ~StdoutWindow();
    void addMessage(const char* msg);
    void enableTimeStamp(void) { bTimeStamp = true; } 
    void disableTimeStamp(void) { bTimeStamp = false; } 

protected:
    void onPMenuSave();
    bool on_delete_event(GdkEventAny* event);

private:
    int m_ID;
    bool bTimeStamp; 
    Gtk::Widget* m_pParent;
    Glib::RefPtr<Gtk::TextBuffer> m_refTextBuff;
    Gtk::VBox m_VBox;
    Gtk::ScrolledWindow m_ScrollModView;
    Gtk::TextView m_textView;
};


#endif //_STDOUTWINDOW_H_

