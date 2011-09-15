/*
 *  Yarp Modules Manager
 *  Copyright: 2011 (C) Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _APPLICATION_LIST_
#define _APPLICATION_LIST_


#include <gtkmm.h>
#include "application.h"

//Tree model columns:
class AppModelColumns : public Gtk::TreeModel::ColumnRecord
{
public:

	AppModelColumns() { 
		add(m_col_refPix); 
		add(m_col_name); 
		add(m_col_type);
		}
/*
	~AppModelColumns(){

	}
*/
	//Gtk::TreeModelColumn<int> m_col_id;
	Gtk::TreeModelColumn< Glib::RefPtr<Gdk::Pixbuf> > m_col_refPix;
	Gtk::TreeModelColumn<Glib::ustring> m_col_name;
	Gtk::TreeModelColumn<NodeType> m_col_type;

};


class ApplicationList: public Gtk::ScrolledWindow
{

public:
	ApplicationList();
	virtual ~ApplicationList();
	bool addApplication(Application* app);

	Glib::RefPtr<Gtk::TreeStore> m_refTreeModel;
	AppModelColumns m_appColumns;
	Gtk::TreeView& getTreeView(void) { return m_TreeView;}

protected:
	virtual void onAppListRowActivated(const Gtk::TreeModel::Path& path, 
				Gtk::TreeViewColumn* column);
	Gtk::TreeView m_TreeView; //The Tree View.
	Gtk::TreeModel::Row m_appRow;
	Gtk::TreeModel::Row m_modRow;
};


#endif //_APPLICATION_LIST_

