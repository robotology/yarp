/*
 *  Yarp Modules Manager
 *  Copyright: 2011 (C) Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


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
	//m_TreeView.append_column("Messages", m_Columns.m_col_text);
	
	Gtk::CellRendererText* textRenderer = manage(new Gtk::CellRendererText());
	textRenderer->property_editable() = false;
	Gtk::TreeViewColumn *col = manage(new Gtk::TreeViewColumn("Messages", *textRenderer));

	col->add_attribute(*textRenderer, "background-gdk", m_Columns.m_col_color);
	col->add_attribute(*textRenderer, "text", m_Columns.m_col_text);
	m_TreeView.append_column(*col);
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
	row[m_Columns.m_col_text] = msg.str();
	row[m_Columns.m_col_color] = Gdk::Color("#FFF6C8");
	m_TreeView.scroll_to_row( m_refListStore->get_path(row));
}

void MessagesList::addError( const char* error)
{
	ostringstream msg;
	msg<<"[ERR]:   "<<error;
	Gtk::TreeModel::Row row = *(m_refListStore->append());
	row[m_Columns.m_col_text] = msg.str();
	row[m_Columns.m_col_color] = Gdk::Color("#F9CCCA");
	m_TreeView.scroll_to_row( m_refListStore->get_path(row));
}


