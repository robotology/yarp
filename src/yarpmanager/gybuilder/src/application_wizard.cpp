/*
 *  Yarp Modules Manager
 *  Copyright: 2011 (C) Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
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
#include "application_wizard.h"
#include "main_window.h"

#include "icon_res.h"
#include <sstream>
#include <iostream>
#include <fstream>

using namespace std;

ApplicationWizard::ApplicationWizard(Gtk::Widget* parent, const char* title) 
: m_Table(6,3)
{
    m_pParent = parent;
    if(title)
        set_title(title);
    else
        set_title("Application Wizard");

    //set_default_size(600, 400);
    set_resizable(false);

    add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL); 
    m_pCreateButton = add_button("Create", Gtk::RESPONSE_OK); 
    m_pCreateButton->set_sensitive(false);
   
    m_Image.set(Gdk::Pixbuf::create_from_data(ymanager_ico.pixel_data, 
                        Gdk::COLORSPACE_RGB,
                        true,
                        8,
                        ymanager_ico.width,
                        ymanager_ico.height,
                        ymanager_ico.bytes_per_pixel*ymanager_ico.width));


    m_FrameLeft.add(m_Image);
    m_HBox.set_spacing(5);    
    m_HBox.pack_start(m_FrameLeft);
    m_HBox.pack_start(m_FrameRight);
    m_FrameLeft.set_size_request(100, -1);
    m_FrameRight.set_size_request(400, -1);

    m_ButtonFile.set_label("...");
    m_ButtonFile.signal_pressed().connect(sigc::mem_fun(*this, &ApplicationWizard::onButtonFilePressed));
    m_Table.set_border_width(5);
    m_Table.set_row_spacings(5);
    m_Table.set_col_spacings(10);
    m_FrameRight.add(m_Table);

    m_refSizeGroup = Gtk::SizeGroup::create(Gtk::SIZE_GROUP_HORIZONTAL);

    m_EntryName.signal_changed().connect(sigc::mem_fun(*this, &ApplicationWizard::onEntryNameInsert));

    add_row(m_Table, 0, m_refSizeGroup, "Application Name:", m_EntryName, NULL);    
    add_row(m_Table, 1, m_refSizeGroup, "File Name:", m_EntryFileName, &m_ButtonFile);
    add_row(m_Table, 2, m_refSizeGroup, "Description:", m_EntryDesc, NULL);    
    add_row(m_Table, 3, m_refSizeGroup, "Version:", m_EntryVersion, NULL);    
    //add_row(m_Table, 4, m_refSizeGroup, "Prefix:", m_EntryPrefix, NULL);    
    add_row(m_Table, 4, m_refSizeGroup, "Author's Name:", m_EntryAuthor, NULL);    
    add_row(m_Table, 5, m_refSizeGroup, "Author's email:", m_EntryEmail, NULL);    

    get_vbox()->pack_start(m_HBox);
    get_vbox()->set_spacing(5);


    show_all_children();
}


ApplicationWizard::~ApplicationWizard()
{
}

void ApplicationWizard::add_row(Gtk::Table& table, int row,
                                const Glib::RefPtr<Gtk::SizeGroup>& size_group,
                                const Glib::ustring& label_text,
                                Gtk::Entry& entry, Gtk::Button* btn)
{
    Gtk::Label* pLabel = Gtk::manage(new Gtk::Label(label_text, true));
    pLabel->set_alignment(Gtk::ALIGN_LEFT, Gtk::ALIGN_BOTTOM);

    table.attach(*pLabel, 0, 1, row, row + 1, Gtk::FILL, Gtk::AttachOptions(0));
    table.attach(entry, 1, 2, row, row + 1, Gtk::EXPAND|Gtk::FILL, Gtk::AttachOptions(0));
    if(btn)
        table.attach(*btn, 2, 3, row, row + 1, Gtk::SHRINK, Gtk::AttachOptions(0));
}


void ApplicationWizard::onEntryNameInsert()
{
    printf("%d\n", __LINE__);

    if(!m_EntryName.get_text().size())
    {
        printf("%d\n", __LINE__);
        m_pCreateButton->set_sensitive(false);
        return;
    }

    MainWindow* wnd = dynamic_cast<MainWindow*>(m_pParent);
    if(wnd)
    {
        printf("%d\n", __LINE__);

        KnowledgeBase* kb = wnd->getManager()->getKnowledgeBase();
        if(kb->getApplication(m_EntryName.get_text().c_str()))
        {
            printf("%d\n", __LINE__);
            Gdk::Color color("red");
            m_EntryName.modify_text(m_EntryName.get_state(), color);
            m_pCreateButton->set_sensitive(true);
        }
        else
        {
            printf("%d\n", __LINE__);
            Gdk::Color color("red");
            m_EntryName.modify_text(m_EntryName.get_state(), color);
            m_pCreateButton->set_sensitive(false);
        }
    }

}

bool ApplicationWizard::on_delete_event(GdkEventAny* event)
{   
    /*
    ApplicationWindow* wnd = dynamic_cast<ApplicationWindow*>(m_pParent);
    if(wnd)
        wnd->onDetachStdout();
    */    
    return Window::on_delete_event(event);
}

void ApplicationWizard::onButtonFilePressed()
{
    Gtk::FileChooserDialog dialog("Create new Application description file");
    dialog.set_transient_for(*this);
    dialog.set_action(Gtk::FILE_CHOOSER_ACTION_SAVE);
    dialog.set_do_overwrite_confirmation(true);

    //Add response buttons the the dialog:
    dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    dialog.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);

    //Add filters, so that only certain file types can be selected:
    Gtk::FileFilter filter_app;
    filter_app.set_name("Application description files (xml)");
    filter_app.add_mime_type("text/xml");
    dialog.add_filter(filter_app);
    
    if(dialog.run() == Gtk::RESPONSE_OK)
    {
        m_EntryFileName.set_text(dialog.get_filename());
    }
}

/*
void ApplicationWizard::onPMenuSave()
{    
    Gtk::FileChooserDialog dialog("Save log to a file");
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
*/

