/*
 *  Yarp Modules Manager
 *  Copyright: 2011 (C) Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */



#if defined(WIN32) || defined(WIN64)
    #pragma warning (disable : 4250)
    #pragma warning (disable : 4520)
#endif

#include "main_window.h"
#include "stdout_window.h"
#include <sstream>
#include <iostream>
#include <fstream>

using namespace std;

StdoutWindow::StdoutWindow(Gtk::Window* parent, int ID, const char* title) : bTimeStamp(false)
{
    m_pParent = parent;
    set_parent_window(parent->get_parent_window()); 

    m_ID = ID;
    if(title)
        set_title(title);
    else
        set_title("STDOUT");

    set_default_size(300, 300);
   
    m_ScrollModView.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    m_ScrollModView.add(m_textView);
    add(m_ScrollModView);
    m_VBox.pack_start(m_textView);
    m_textView.set_property("editable", false);

    
    //set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    //add(m_textView);

    m_refTextBuff = Gtk::TextBuffer::create();
    m_textView.set_buffer(m_refTextBuff);

    
    // adding popup menubar
    m_refActionGroup = Gtk::ActionGroup::create();
    m_refActionGroup->add( Gtk::Action::create("PMenuClear", Gtk::Stock::CLEAR, "_Clear log", "Clear log"),
                            sigc::mem_fun(*this, &StdoutWindow::onPMenuClear) );
    m_refActionGroup->add( Gtk::Action::create("PMenueSave", Gtk::Stock::SAVE, "_Save log...", "Save log..."),
                            sigc::mem_fun(*this, &StdoutWindow::onPMenuSave) );

    m_refUIManager = Gtk::UIManager::create();
    m_refUIManager->insert_action_group(m_refActionGroup);
    if(m_pParent)
        m_pParent->add_accel_group(m_refUIManager->get_accel_group());
    Glib::ustring ui_info =
        "<ui>"
        " <popup name='PopupMenu'>"
        "      <menuitem action='PMenuClear'/>"
        "      <menuitem action='PMenueSave'/>"
        " </popup>"
        "</ui>";


#ifdef GLIBMM_EXCEPTIONS_ENABLED
    try
    {
        m_refUIManager->add_ui_from_string(ui_info);
    }
    catch(const Glib::Error& ex)
    {
        std::cerr << "building popup menus failed: " << ex.what();
    }
#else
    std::auto_ptr<Glib::Error> ex;
    m_refUIManager->add_ui_from_string(ui_info, ex);
    if(ex.get())
    {
        std::cerr << "building popu menus failed: " << ex->what();
    }   
#endif //GLIBMM_EXCEPTIONS_ENABLED


   // m_TreeView.signal_button_press_event().connect_notify(sigc::mem_fun(*this,
   //         &StdoutWindow::onTreeButtonPressed) );
   

    show_all_children();
}


StdoutWindow::~StdoutWindow()
{
}


void StdoutWindow::addMessage(const char* text)
{
    /*
    ostringstream msg;
    msg<<text<<endl;
    m_refTextBuff->insert_at_cursor(msg.str());
    */
    m_refTextBuff->insert(m_refTextBuff->end(), Glib::ustring(text));   
}

/*
void StdoutWindow::onTreeButtonPressed(GdkEventButton* event)
{
    if((event->type == GDK_BUTTON_PRESS) && (event->button == 3))
    {
        Gtk::Menu* pMenu = dynamic_cast<Gtk::Menu*>(
                    m_refUIManager->get_widget("/PopupMenu"));
        if(pMenu)
        pMenu->popup(event->button, event->time);
    }
}
*/


void StdoutWindow::onPMenuClear()
{

}


void StdoutWindow::onPMenuSave()
{
    Gtk::FileChooserDialog dialog("Save log to a file");
    if(m_pParent)
        dialog.set_transient_for(*m_pParent);
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


