/*
 *  Yarp Modules Manager
 *  Copyright: 2011 (C) Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _MESSAGESLIST_H_
#define _MESSAGESLIST_H_


#include <gtkmm.h>

class MessagesList: public Gtk::ScrolledWindow
{
public:
	MessagesList();
	virtual ~MessagesList();
	class ModelColumns : public Gtk::TreeModel::ColumnRecord
	{
	public:
		ModelColumns() { add(m_col_text);  add(m_col_color); }

	 	Gtk::TreeModelColumn<Glib::ustring> m_col_text;
		Gtk::TreeModelColumn<Gdk::Color> m_col_color;
	};

	ModelColumns m_Columns;
	
	void addWarning(const char* warning);
	void addError(const char* error);

protected:
	Glib::RefPtr<Gtk::ListStore> m_refListStore; //The Tree Model.
	Gtk::TreeView m_TreeView; //The Tree View.
};


#endif //_MESSAGESLIST_H_

