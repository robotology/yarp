/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 *
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _INSPECT_YSCOPE_WINDOW_H_
#define _INSPECT_YSCOPE_WINDOW_H_

#include <gtkmm.h>

//class InspectYScopeWindow;
class InspectYScopeWindow: public Gtk::Dialog
{
public:
    InspectYScopeWindow() :
        m_adjustment_index(0.0, 0.0, 100.0, 1.0, 5.0, 0.0)
    {
        set_title("Inspecting using yarpscope");
        m_Label.set_text("Data index:");
        m_EntryIndex.set_adjustment(m_adjustment_index);
        m_EntryIndex.set_value(0);
        m_EntryIndex.set_wrap(true);
        //m_EntryIndex.set_size_request(20, -1);
        set_resizable(false);
        add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
        add_button("Inspect", Gtk::RESPONSE_OK);
        m_HBox.set_spacing(3);
        m_HBox.pack_start(m_Label);
        m_HBox.pack_start(m_EntryIndex);
        get_vbox()->pack_start(m_HBox);
        show_all_children();
    }
    virtual ~InspectYScopeWindow() { }

private:
    Gtk::HBox m_HBox;
    Gtk::Label m_Label;
    Gtk::Adjustment m_adjustment_index;
public:
    Gtk::SpinButton m_EntryIndex;

};

#endif // _INSPECT_YSCOPE_WINDOW_H_

