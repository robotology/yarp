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
#include "application_wizard.h"

#include "icon_res.h"
#include <yarp/manager/ymm-dir.h>
#include <iostream>
#include <fstream>

using namespace std;
using namespace yarp::manager;

inline bool isAbsolute(const char *path) {  //copied from yarp_OS ResourceFinder.cpp
        if (path[0]=='/'||path[0]=='\\') {
            return true;
        }
        std::string str(path);
        if (str.length()>1) {
            if (str[1]==':') {
                return true;
            }
        }
        return false;
    };

ApplicationWizard::ApplicationWizard(Gtk::Widget* parent, const char* title, Application* app) 
: m_Table(7,3)
    #if (GTKMM_MAJOR_VERSION == 2 && GTKMM_MINOR_VERSION >= 24)
, m_EntryFolderName(true)
#endif
{
    m_Application = app;
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
    m_HBox.set_spacing(3);
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

    MainWindow* wnd = dynamic_cast<MainWindow*>(m_pParent);

//     if(m_Application)
//     {
//         string filename = m_Application->getXmlFile();
//         const size_t last_slash_idx = filename.rfind(PATH_SEPERATOR);
//         if (std::string::npos != last_slash_idx)
//         {
//             string directory = filename.substr(0, last_slash_idx);
// #if (GTKMM_MAJOR_VERSION == 2 && GTKMM_MINOR_VERSION < 24)
//             m_EntryFolderName.append_text(Glib::ustring(directory.c_str()));
// #else
//             m_EntryFolderName.append(Glib::ustring(directory.c_str()));
// #endif
//         }
//     }


    if(wnd->m_config.check("apppath"))
    {
        std::string basepath=wnd->m_config.check("ymanagerini_dir", yarp::os::Value("")).asString().c_str();
        string appPaths(wnd->m_config.find("apppath").asString().c_str());
        string strPath;
		
	do
        {
	    string::size_type pos=appPaths.find(";");
	    strPath=appPaths.substr(0, pos);
            trimString(strPath);
            if (!isAbsolute(strPath.c_str()))
                strPath=basepath+strPath;
            
            if((strPath.rfind(PATH_SEPERATOR)==string::npos) || 
                    (strPath.rfind(PATH_SEPERATOR)!=strPath.size()-1))
                strPath = strPath + string(PATH_SEPERATOR);
#if (GTKMM_MAJOR_VERSION == 2 && GTKMM_MINOR_VERSION < 24)
            m_EntryFolderName.append_text(Glib::ustring(strPath));
#else
            m_EntryFolderName.append(Glib::ustring(strPath));
#endif          
            if (pos==string::npos)
                break;
            appPaths=appPaths.substr(pos+1);
        }
	while (appPaths!="");

        
    }
    
    if (wnd->m_config.check("yarpdatahome"))
    {
        string appPaths(wnd->m_config.find("apppath").asString().c_str());
        Glib::ustring homePath=wnd->m_config.find("yarpdatahome").asString().c_str();
        homePath +=  string(PATH_SEPERATOR) + string("applications");
        
        if (appPaths.find(homePath)==string::npos)
#if (GTKMM_MAJOR_VERSION == 2 && GTKMM_MINOR_VERSION < 24)
            m_EntryFolderName.prepend_text(homePath);
#else
            m_EntryFolderName.prepend(homePath);
#endif
    }

    m_EntryFolderName.set_active(0);
    m_EntryVersion.set_text("1.0");

    if(m_Application)
        m_EntryName.set_text(m_Application->getName());
    add_row(m_Table, 0, m_refSizeGroup, "Application Name:", m_EntryName, NULL);    
    add_row(m_Table, 1, m_refSizeGroup, "File Name:", m_EntryFileName, NULL);
    add_row(m_Table, 2, m_refSizeGroup, "Folder Name:", m_EntryFolderName, &m_ButtonFile);
    if(!m_Application)
    {
        add_row(m_Table, 3, m_refSizeGroup, "Description:", m_EntryDesc, NULL);    
        add_row(m_Table, 4, m_refSizeGroup, "Version:", m_EntryVersion, NULL);    
        //add_row(m_Table, 4, m_refSizeGroup, "Prefix:", m_EntryPrefix, NULL);    
        add_row(m_Table, 5, m_refSizeGroup, "Author's Name:", m_EntryAuthor, NULL);    
        add_row(m_Table, 6, m_refSizeGroup, "Author's email:", m_EntryEmail, NULL);    
    }
    get_vbox()->pack_start(m_HBox);
    get_vbox()->set_spacing(3);


    show_all_children();
}


ApplicationWizard::~ApplicationWizard()
{
}

void ApplicationWizard::add_row(Gtk::Table& table, int row,
                                const Glib::RefPtr<Gtk::SizeGroup>& size_group,
                                const Glib::ustring& label_text,
                                Gtk::Widget& entry, Gtk::Button* btn)
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
    if(!m_EntryName.get_text().size())
    {
        m_pCreateButton->set_sensitive(false);
        m_EntryFileName.set_text("");
        return;
    }

    MainWindow* wnd = dynamic_cast<MainWindow*>(m_pParent);
    if(wnd)
    {
        KnowledgeBase* kb = wnd->getManager()->getKnowledgeBase();
        if(!kb->getApplication(m_EntryName.get_text().c_str()))
        {
            Gdk::Color color("black");
            m_EntryName.modify_text(m_EntryName.get_state(), color);
            m_pCreateButton->set_sensitive(true);
            m_EntryFileName.set_text(m_EntryName.get_text() + Glib::ustring(".xml"));
        }
        else
        {
            Gdk::Color color("red");
            m_EntryName.modify_text(m_EntryName.get_state(), color);
            m_pCreateButton->set_sensitive(false);
            m_EntryFileName.set_text(m_EntryName.get_text() + Glib::ustring(".xml"));
        }
    }
}

bool ApplicationWizard::on_delete_event(GdkEventAny* event)
{   
    return Window::on_delete_event(event);
}

void ApplicationWizard::onButtonFilePressed()
{
    Gtk::FileChooserDialog dialog("Create new Application description file");
    dialog.set_transient_for(*this);
    dialog.set_action(Gtk::FILE_CHOOSER_ACTION_SELECT_FOLDER);
    dialog.set_do_overwrite_confirmation(true);
    dialog.set_current_folder(m_EntryFolderName.get_entry()->get_text());
    

    //Add response buttons the the dialog:
    dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    dialog.add_button("Select", Gtk::RESPONSE_OK);

    if(dialog.run() == Gtk::RESPONSE_OK)
    {
        m_EntryFolderName.get_entry()->set_text(dialog.get_filename());
    }
}


