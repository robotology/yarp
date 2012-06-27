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


#ifndef GPORTSCOPE_MAINWINDOW_H
#define GPORTSCOPE_MAINWINDOW_H

#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include <glibmm/refptr.h>

namespace Gtk {
class ActionGroup;
class UIManager;
} // Gtk

namespace GPortScope {

class MainWindow : public Gtk::Window
{

public:
    MainWindow();
    virtual ~MainWindow();

private:
    // Signal handlers:
    void on_action_file_quit();
    void on_action_help_about();
    void on_action_actions_stop_start();

    void on_button_clicked();

    // Child widgets:
    Gtk::VBox m_windowBox;

    Glib::RefPtr<Gtk::ActionGroup> m_refActionGroup;
    Glib::RefPtr<Gtk::UIManager> m_refUIManager;

    // Other private members
    bool m_running;
};

} // GPortScope

#endif // GPORTSCOPE_MAINWINDOW_H
