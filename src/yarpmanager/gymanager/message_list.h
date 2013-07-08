/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _MESSAGESLIST_H_
#define _MESSAGESLIST_H_

#include <gtkmm.h>

//class MainWindow;

class MsgModelColumns : public Gtk::TreeModel::ColumnRecord
{
public:
    MsgModelColumns() { add(m_col_text);  add(m_col_color); }

    Gtk::TreeModelColumn<Glib::ustring> m_col_text;
    Gtk::TreeModelColumn<Gdk::Color> m_col_color;
};


class MessagesList: public Gtk::ScrolledWindow
{
public:
    MessagesList(Gtk::Window* pParent=NULL);
    virtual ~MessagesList();
    void addWarning(const char* warning);
    void addError(const char* error);
    void addMessage(const char* msg);
    void enableTimeStamp(void) { bTimeStamp = true; } 
    void disableTimeStamp(void) { bTimeStamp = false; } 

    MsgModelColumns m_Columns;  
    Glib::RefPtr<Gtk::ListStore> m_refListStore; //The Tree Model.

protected:
    void onTreeButtonPressed(GdkEventButton* event);
    void onPMenuClear();
    void onPMenuSave();

    Gtk::TreeView m_TreeView; //The Tree View.
    Glib::RefPtr<Gtk::UIManager> m_refUIManager;
    Glib::RefPtr<Gtk::ActionGroup> m_refActionGroup;

private:
    bool bTimeStamp; 
    Gtk::Window* m_pParent;

};


#endif //_MESSAGESLIST_H_

