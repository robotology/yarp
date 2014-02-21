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
#include "message_list.h"
#include <iostream>
#include <fstream>

using namespace std;
using namespace yarp::manager;


MessagesList::MessagesList(Gtk::Window* pParent) : bTimeStamp(false)
{
    m_pParent = pParent;

    /* Create a new scrolled window, with scrollbars only if needed */
    set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    add(m_TreeView);

    /* create list store */
    m_refListStore = Gtk::ListStore::create(m_Columns);
    m_TreeView.set_model(m_refListStore);

    //Add the Model’s column to the View’s columns:
    Gtk::CellRendererText* textRenderer = Gtk::manage(new Gtk::CellRendererText());
    Gtk::TreeViewColumn* col = Gtk::manage(new Gtk::TreeViewColumn("Messages")); 
    textRenderer->property_editable() = false;
    col->pack_start(*textRenderer, true);
    col->add_attribute(*textRenderer, "text", 0);
    col->add_attribute(*textRenderer, "background-gdk", 1);
    m_TreeView.append_column(*col);
    m_TreeView.set_headers_visible(false);


    // adding popup menubar
    m_refActionGroup = Gtk::ActionGroup::create();
    m_refActionGroup->add( Gtk::Action::create("PMenuClear", Gtk::Stock::CLEAR, "_Clear log", "Clear log"),
                            sigc::mem_fun(*this, &MessagesList::onPMenuClear) );
    m_refActionGroup->add( Gtk::Action::create("PMenueSave", Gtk::Stock::SAVE, "_Save log...", "Save log..."),
                            sigc::mem_fun(*this, &MessagesList::onPMenuSave) );

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


    m_TreeView.signal_button_press_event().connect_notify(sigc::mem_fun(*this,
            &MessagesList::onTreeButtonPressed) );

    show_all_children();
}


MessagesList::~MessagesList()
{
}


void MessagesList::addWarning( const char* warning)
{
    OSTRINGSTREAM msg;
    msg<<"[WAR]:   "<<warning;
    Gtk::TreeModel::Row row = *(m_refListStore->append());
    row.set_value(0, Glib::ustring(msg.str()));
    row[m_Columns.m_col_color] = Gdk::Color("#FFF6C8");
    m_TreeView.scroll_to_row( m_refListStore->get_path(row));
}

void MessagesList::addError(const char* error)
{
    OSTRINGSTREAM msg;
    msg<<"[ERR]:   "<<error;
    Gtk::TreeModel::Row row = *(m_refListStore->append());
    row.set_value(0, Glib::ustring(msg.str()));
    row[m_Columns.m_col_color] = Gdk::Color("#F9CCCA");
    m_TreeView.scroll_to_row( m_refListStore->get_path(row));
}

void MessagesList::addMessage(const char* text)
{
    OSTRINGSTREAM msg;
    msg<<text;
    Gtk::TreeModel::Row row = *(m_refListStore->append());
    row.set_value(0, Glib::ustring(msg.str()));
    row[m_Columns.m_col_color] = Gdk::Color("#FFFFFF");
    m_TreeView.scroll_to_row( m_refListStore->get_path(row));
}


void MessagesList::onTreeButtonPressed(GdkEventButton* event)
{
    if((event->type == GDK_BUTTON_PRESS) && (event->button == 3))
    {
        Gtk::Menu* pMenu = dynamic_cast<Gtk::Menu*>(
                    m_refUIManager->get_widget("/PopupMenu"));
        if(pMenu)
        pMenu->popup(event->button, event->time);
    }
}



void MessagesList::onPMenuClear()
{
    m_refListStore->clear();
}


void MessagesList::onPMenuSave()
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
            typedef Gtk::TreeModel::Children type_children;
            type_children children = m_refListStore->children();
            for(type_children::iterator iter = children.begin(); iter != children.end(); ++iter)
            {
              Gtk::TreeModel::Row row = *iter;
              ser<<row[m_Columns.m_col_text]<<endl;
            }   
            ser.close();
        }
        else
        {
            addError("Cannot open log file to write.");
        }
    }
}


