/*
 *  Yarp Modules Manager
 *  Copyright: 2011 (C) Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include "application_list.h"
#include <sstream>
#include <string>
#include <iostream>
using namespace std;

ApplicationList::ApplicationList()
{
	/* Create a new scrolled window, with scrollbars only if needed */
	set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	add(m_TreeView);

	/* create list store */
	m_refTreeModel = Gtk::TreeStore::create(m_appColumns);
	m_TreeView.set_model(m_refTreeModel);

	m_appRow = *(m_refTreeModel->append());
	m_appRow[m_appColumns.m_col_name] = "Applications";
	m_appRow[m_appColumns.m_col_refPix] = Gtk::IconTheme::get_default()->load_icon("folder", 16);
	//Gtk::IconTheme::get_default()->load_icon("folder", Gtk::ICON_SIZE_MENU)->save("test.jpg", "jpeg");

//	m_modRow = *(m_refTreeModel->append());
//	m_modRow[m_appColumns.m_col_name] = "Module";
//	m_modRow[m_appColumns.m_col_refPix] = Gtk::IconTheme::get_default()->load_icon("folder", 16);

	//Add the Model’s column to the View’s columns:	
	Gtk::TreeViewColumn col("Entities");
	Gtk::CellRendererText cellText;
	Gtk::CellRendererPixbuf cellPix;
	col.pack_start(cellPix, false);
	col.pack_start(cellText, true);
	col.add_attribute(cellText, "text", 1);
	col.add_attribute(cellPix, "pixbuf", 0);
	m_TreeView.append_column(col);
	m_TreeView.expand_all();

  	//m_TreeView.signal_row_activated().connect(sigc::mem_fun(*this,
    //          			&ApplicationList::onAppListRowActivated) );

	show_all_children();
}

ApplicationList::~ApplicationList()
{
}


bool ApplicationList::addApplication(const char* szName)
{
	typedef Gtk::TreeModel::Children type_children;

	type_children children = m_refTreeModel->children();
	for(type_children::iterator iter = children.begin(); iter != children.end(); ++iter)
	{
	  Gtk::TreeModel::Row row = *iter;
	  if(row[m_appColumns.m_col_name] == Glib::ustring("Applications"))
	  {
	  	type_children appchild = row->children();
		for(type_children::iterator itrapp = appchild.begin(); itrapp!= appchild.end(); ++itrapp)
		{
			Gtk::TreeModel::Row childrow = *itrapp;
		 	if(childrow[m_appColumns.m_col_name] == Glib::ustring(szName))
	  			return false;
		}
	  }
	}

	Gtk::TreeModel::Row childrow = *(m_refTreeModel->append(m_appRow.children()));
	childrow[m_appColumns.m_col_name] = szName;
	childrow[m_appColumns.m_col_refPix] = Gtk::IconTheme::get_default()->load_icon("document", 16);
	m_TreeView.expand_all();
	return true;
}

void ApplicationList::onAppListRowActivated(const Gtk::TreeModel::Path& path, 
			Gtk::TreeViewColumn* column)
{
	/*
	Gtk::TreeModel::iterator iter = m_refTreeModel->get_iter(path);
	if(iter)
  	{
    	Gtk::TreeModel::Row row = *iter;
    	cout<<"Row activated: name="<< row[m_appColumns.m_col_name]<<endl;
  	}
	*/
}


