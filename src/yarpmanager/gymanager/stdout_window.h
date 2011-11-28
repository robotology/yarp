/*
 *  Yarp Modules Manager
 *  Copyright: 2011 (C) Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _STDOUTWINDOW_H_
#define _STDOUTWINDOW_H__H_

#include <gtkmm.h>

//class StdoutWindow;
class StdoutWindow: public Gtk::Window

{
public:
    StdoutWindow(Gtk::Window* parent, int ID, const char* title=NULL);

    virtual ~StdoutWindow();
    void addMessage(const char* msg);
    void enableTimeStamp(void) { bTimeStamp = true; } 
    void disableTimeStamp(void) { bTimeStamp = false; } 

protected:
    void onPMenuClear();
    void onPMenuSave();

private:
    int m_ID;
    bool bTimeStamp; 
    Gtk::Window* m_pParent;
    Glib::RefPtr<Gtk::TextBuffer> m_refTextBuff;
    Glib::RefPtr<Gtk::UIManager> m_refUIManager;
    Glib::RefPtr<Gtk::ActionGroup> m_refActionGroup;
    Gtk::VBox m_VBox;
    Gtk::ScrolledWindow m_ScrollModView;
    Gtk::TextView m_textView;

};


#endif //_STDOUTWINDOW_H_

