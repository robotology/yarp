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

#include "message_list.h"
#include <sstream>

using namespace std;

MessagesList::MessagesList()
{
	/* Create a new scrolled window, with scrollbars only if needed */
	set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	add(m_TreeView);

	/* create list store */
	m_refListStore = Gtk::ListStore::create(m_Columns);
	m_TreeView.set_model(m_refListStore);

	//Add the Model’s column to the View’s columns:
	
	Gtk::TreeViewColumn col("Messages");
	Gtk::CellRendererText textRenderer;
	textRenderer.property_editable() = false;
	col.pack_start(textRenderer, true);
	col.add_attribute(textRenderer, "text", 0);
	col.add_attribute(textRenderer, "background-gdk", 1);
	m_TreeView.append_column(col);
	m_TreeView.set_headers_visible(false);
	show_all_children();
}


MessagesList::~MessagesList()
{
}


void MessagesList::addWarning( const char* warning)
{
	ostringstream msg;
	msg<<"[WAR]:   "<<warning;
	Gtk::TreeModel::Row row = *(m_refListStore->append());
	row.set_value(0, Glib::ustring(msg.str()));
	row[m_Columns.m_col_color] = Gdk::Color("#FFF6C8");
	m_TreeView.scroll_to_row( m_refListStore->get_path(row));
}

void MessagesList::addError( const char* error)
{
	ostringstream msg;
	msg<<"[ERR]:   "<<error;
	Gtk::TreeModel::Row row = *(m_refListStore->append());
	row.set_value(0, Glib::ustring(msg.str()));
	row[m_Columns.m_col_color] = Gdk::Color("#F9CCCA");
	m_TreeView.scroll_to_row( m_refListStore->get_path(row));
}


