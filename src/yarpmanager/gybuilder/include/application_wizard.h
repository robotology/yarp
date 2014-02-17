/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _APPLICATION_WIZARD_H_
#define _APPLICATION_WIZARD_H_

#include <gtkmm.h>
#include <yarp/manager/application.h>

//class ApplicationWizard;
class ApplicationWizard: public Gtk::Dialog
{
public:
    ApplicationWizard(Gtk::Widget* parent, const char* title=NULL, 
                      Application* application=NULL);
    virtual ~ApplicationWizard();

protected:
   // void onPMenuSave();
    bool on_delete_event(GdkEventAny* event);
    void onButtonFilePressed();
    void onEntryNameInsert();

private:
    Application* m_Application;
    Gtk::Widget* m_pParent;
    Gtk::VBox m_VBox;
    Gtk::HBox m_HBox;

    Gtk::Button m_ButtonFile;
    Gtk::Button* m_pCreateButton;
    Gtk::Image m_Image;
    Gtk::Frame m_FrameLeft;
    Gtk::Frame m_FrameRight;

    Gtk::Table m_Table;
    Glib::RefPtr<Gtk::SizeGroup> m_refSizeGroup;

public:
    Gtk::Entry m_EntryName;
    Gtk::Entry m_EntryFileName;
    #if (GTKMM_MAJOR_VERSION == 2 && GTKMM_MINOR_VERSION < 24)
    Gtk::ComboBoxEntryText m_EntryFolderName;
#else
    Gtk::ComboBoxText m_EntryFolderName;
#endif
    Gtk::Entry m_EntryDesc;
    Gtk::Entry m_EntryVersion;
//    Gtk::Entry m_EntryPrefix;
    Gtk::Entry m_EntryAuthor;
    Gtk::Entry m_EntryEmail;

private:
    void add_row(Gtk::Table& table, int row,
                 const Glib::RefPtr<Gtk::SizeGroup>& size_group,
                 const Glib::ustring& label_text, Gtk::Widget& entry, Gtk::Button* btn);

};


#endif //_APPLICATION_WIZARD_H_

