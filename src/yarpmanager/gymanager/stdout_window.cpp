/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */



#if defined(WIN32)
    #pragma warning (disable : 4250)
    #pragma warning (disable : 4520)
#endif

#include "main_window.h"
#include "application_window.h"
#include "stdout_window.h"
#include <iostream>
#include <fstream>

using namespace std;

StdoutWindow::StdoutWindow(Gtk::Widget* parent, int ID, const char* title) : bTimeStamp(false)
{
    m_pParent = parent;
    //set_parent(parent); 

    m_ID = ID;
    if(title)
        set_title(title);
    else
        set_title("STDOUT");

    set_default_size(300, 300);
   
    m_ScrollModView.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    m_ScrollModView.add(m_textView);
    m_VBox.pack_start(m_ScrollModView);
    add(m_VBox);

    m_textView.set_property("editable", false);

    m_refTextBuff = Gtk::TextBuffer::create();
    m_textView.set_buffer(m_refTextBuff);

    show_all_children();
}


StdoutWindow::~StdoutWindow()
{
}


void StdoutWindow::addMessage(const char* text)
{
    m_refTextBuff->insert(m_refTextBuff->end(), Glib::ustring(text));
}

bool StdoutWindow::on_delete_event(GdkEventAny* event)
{
    ApplicationWindow* wnd = dynamic_cast<ApplicationWindow*>(m_pParent);
    if(wnd)
        wnd->onDetachStdout();
    return Window::on_delete_event(event);
}


void StdoutWindow::onPMenuSave()
{
    Gtk::FileChooserDialog dialog("Save log to a file");
    /*
    if(m_pParent)
        dialog.set_transient_for(*m_pParent);
    */
    dialog.set_action(Gtk::FILE_CHOOSER_ACTION_SAVE);
    dialog.set_do_overwrite_confirmation(true);

    //Add response buttons the the dialog:
    dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    dialog.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);


    //Add filters, so that only certain file types can be selected:
    Gtk::FileFilter filter_any;
    filter_any.set_name("Any files");
    filter_any.add_pattern("*");
    dialog.add_filter(filter_any);

    if(dialog.run() == Gtk::RESPONSE_OK)
    {
        string fname = dialog.get_filename();
        ofstream ser(fname.c_str());
        if (ser.is_open())
        {
            ser<<m_refTextBuff->get_text();
            ser.close();
        }
    }
}


