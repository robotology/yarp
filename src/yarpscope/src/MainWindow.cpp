/*
 *  This file is part of gPortScope
 *
 *  Copyright (C) 2012 Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "MainWindow.h"

#include <iostream>

#include <gtkmm/aboutdialog.h>
#include <gtkmm/actiongroup.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/stock.h>
#include <gtkmm/toolbar.h>
#include <gtkmm/uimanager.h>
#include <gtkmm/menubar.h>

#include <glibmm/i18n.h>


GPortScope::MainWindow::MainWindow()
    : m_refActionGroup(Gtk::ActionGroup::create()),
      m_refUIManager(Gtk::UIManager::create()),
      m_running(true)
{
    set_border_width(3);
    set_default_size(640, 480);
    set_icon_name("gportscope"); // FIXME

    add(m_windowBox);

    // Setup actions
    m_refActionGroup = Gtk::ActionGroup::create();

    m_refActionGroup->add(Gtk::Action::create("MenuFile", _("_File")));
    m_refActionGroup->add(Gtk::Action::create("Quit", Gtk::Stock::QUIT),
                sigc::mem_fun(*this, &MainWindow::on_action_file_quit));
    m_refActionGroup->add(Gtk::Action::create("MenuActions", _("_Actions")));
    m_refActionGroup->add(Gtk::Action::create("StopStart", _("Stop"), _("Stop plotting")),
                sigc::mem_fun(*this, &MainWindow::on_action_actions_stop_start));
    m_refActionGroup->add(Gtk::Action::create("MenuHelp", _("Help")));
    m_refActionGroup->add(Gtk::Action::create("About", Gtk::Stock::ABOUT),
                sigc::mem_fun(*this, &MainWindow::on_action_help_about));

    Glib::RefPtr <Gtk::Action > stopStartAction = m_refActionGroup->get_action("StopStart");
    if(!stopStartAction) {
        std::cerr << "FATAL: Action \"StopStart\" is missing." << std::endl;
    }
    stopStartAction->set_icon_name("media-playback-pause");


    m_refUIManager->insert_action_group(m_refActionGroup);
    add_accel_group(m_refUIManager->get_accel_group());

    Glib::ustring ui_info =
    "<ui>"
    "  <menubar name='MenuBar'>"
    "    <menu action='MenuFile'>"
    "      <separator />"
    "      <menuitem action='Quit'/>"
    "    </menu>"
    "    <menu action='MenuActions'>"
    "      <menuitem action='StopStart'/>"
    "    </menu>"
    "    <menu action='MenuHelp'>"
    "      <menuitem action='About'/>"
    "    </menu>"
    "  </menubar>"
    "  <toolbar  name='ToolBar'>"
    "    <toolitem action='StopStart'/>"
    "  </toolbar>"
    "</ui>";

    try {
        m_refUIManager->add_ui_from_string(ui_info);
    }
    catch(const Glib::Error& ex) {
        std::cerr << "FATAL: building menus failed: " <<  ex.what();
    }

    // Setup menu bar
    Gtk::MenuBar* menubar = dynamic_cast<Gtk::MenuBar*>(m_refUIManager->get_widget("/MenuBar"));
    if(menubar) {
        m_windowBox.pack_start(*menubar, Gtk::PACK_SHRINK);
    } else {
        std::cerr << "FATAL: building menus failed: \"/MenuBar\" is missing" << std::endl;
    }

    // Setup toolbar
    Gtk::Toolbar* toolbar = dynamic_cast<Gtk::Toolbar*>(m_refUIManager->get_widget("/ToolBar"));
    if(toolbar) {
        m_windowBox.pack_start(*toolbar, Gtk::PACK_SHRINK);
        toolbar->set_toolbar_style(Gtk::TOOLBAR_BOTH);
    } else {
        std::cerr << "FATAL: building menus failed: \"/ToolBar\" is missing" << std::endl;
    }

    Gtk::Button button(_("Hello World"));
    button.signal_clicked().connect(sigc::mem_fun(*this,
                &MainWindow::on_button_clicked));

    m_windowBox.pack_start(button);

    show_all_children();
}

GPortScope::MainWindow::~MainWindow()
{
}

void GPortScope::MainWindow::on_action_file_quit()
{
    std::cout << "DEBUG: Quit clicked" << std::endl;
    hide();
}

void GPortScope::MainWindow::on_action_help_about()
{
    std::cout << "DEBUG: About clicked" << std::endl;

    Gtk::AboutDialog dialog;
    dialog.set_transient_for(*this);

    dialog.set_comments("A simple graphical user interface for visualizing the numerical content of a yarp port.");

    std::list<Glib::ustring> authors;
    authors.push_back("Daniele E. Domenichelli <daniele.domenichelli@iit.it>");
    dialog.set_authors(authors);

    dialog.set_license("Copyright (C) 2012 Daniele E. Domenichelli <daniele.domenichelli@iit.it>\n"
                       "Copyright (C) 2006 Francesco Nori <francesco.nori@iit.it>\n"
                       "\n"
                       "This program is free software: you can redistribute it and/or modify\n"
                       "it under the terms of the GNU General Public License as published by\n"
                       "the Free Software Foundation, either version 2 of the License, or\n"
                       "(at your option) any later version.\n"
                       "\n"
                       "This program is distributed in the hope that it will be useful,\n"
                       "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
                       "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
                       "GNU General Public License for more details.\n"
                       "\n"
                       "You should have received a copy of the GNU General Public License\n"
                       "along with this program.  If not, see <http://www.gnu.org/licenses/>.");

    dialog.run();
}

void GPortScope::MainWindow::on_button_clicked()
{
    std::cout << "Hello World" << std::endl;
}

void GPortScope::MainWindow::on_action_actions_stop_start()
{
    Glib::RefPtr<Gtk::Action> stopStartAction = m_refActionGroup->get_action("StopStart");
    if(!stopStartAction) {
        std::cerr << "FATAL: Action \"StopStart\" is missing." << std::endl;
    }

    if (m_running) {
        std::cout << "DEBUG: Stop clicked" << std::endl;
        stopStartAction->set_icon_name("media-playback-start");
        stopStartAction->set_label(_("Start"));
        stopStartAction->set_tooltip(_("Start plotting"));
    } else {
        std::cout << "DEBUG: Start clicked" << std::endl;
        stopStartAction->set_icon_name("media-playback-pause");
        stopStartAction->set_label(_("Stop"));
        stopStartAction->set_tooltip(_("Stop plotting"));
    }

    m_running = !m_running;
}
