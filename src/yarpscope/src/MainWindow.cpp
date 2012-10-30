/*
 * Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
 * Authors: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include "MainWindow.h"
#include "DataPlot.h"
#include "Debug.h"
#include "PlotManager.h"
#include "PortReader.h"

#include <glibmm/ustring.h>
//#include <glibmm/i18n.h>
#define _(String) (String)
#define N_(String) String
#define textdomain(Domain)
#define bindtextdomain(Package, Directory)

#include <gtkmm/aboutdialog.h>
#include <gtkmm/actiongroup.h>
#include <gtkmm/box.h>
#include <gtkmm/stock.h>
#include <gtkmm/toolbar.h>
#include <gtkmm/uimanager.h>
#include <gtkmm/menubar.h>
#include <gtkmm/table.h>
#include <gtkmm/toggleaction.h>
#include <gtkmm/spinbutton.h>

#include <yarp/os/Value.h>
#include <yarp/os/Network.h>
#include <yarp/os/Bottle.h>


namespace YarpScope {

class MainWindow::Private
{
public:
    Private(MainWindow *parent) :
        parent(parent),
        refActionGroup(Gtk::ActionGroup::create()),
        refUIManager(Gtk::UIManager::create()),
        intervalAdjustment(1, -1, 1000),
        intervalSpinButton(intervalAdjustment, 0.0, 0),
        intervalLabel(_("Interval")),
        running(true)
    {
    }

    ~Private()
    {
    }

    // Signal handlers:
    void on_action_file_quit();
    void on_action_help_about();
    void on_action_actions_stop_start();
    void on_action_actions_clear();
    void on_action_actions_autorescale();
    void on_action_interval_value_changed();

    // Parse inputs
    void parseInputs();

    void setInputPort(DataPlot *plot, const Glib::ustring &local);
    void connectRemotesToLocal(std::list<Glib::ustring> remotes, const Glib::ustring &local);
    void setIndexMask(DataPlot *plot);

    // parent window
    MainWindow * const parent;

    // Child widgets:
    Glib::RefPtr<Gtk::ActionGroup> refActionGroup;
    Glib::RefPtr<Gtk::UIManager> refUIManager;

    Gtk::VBox windowBox;

    // Interval Button
    Gtk::Adjustment intervalAdjustment;
    Gtk::SpinButton intervalSpinButton;
    Gtk::Label intervalLabel;
    Gtk::VBox intervalVBox;
    Gtk::ToolItem intervalToolItem;

    // Other private members
    bool running;
};

} // YarpScope



void YarpScope::MainWindow::Private::on_action_file_quit()
{
    debug() << "Quit clicked";
    parent->hide();
}

void YarpScope::MainWindow::Private::on_action_help_about()
{
    debug() << "About clicked";

    Gtk::AboutDialog dialog;
    dialog.set_transient_for(*parent);

    dialog.set_comments("A simple graphical user interface for visualizing the numerical content of a yarp port.");

    std::list<Glib::ustring> authors;
    authors.push_back("Daniele E. Domenichelli <daniele.domenichelli@iit.it>");
    dialog.set_authors(authors);

    dialog.set_license("Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia\n"
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

void YarpScope::MainWindow::Private::on_action_actions_stop_start()
{
    Glib::RefPtr<Gtk::Action> stopStartAction = refActionGroup->get_action("StopStart");
    if(!stopStartAction) {
        fatal() << "Action \"StopStart\" is missing.";
    }

    if (running) {
        debug() << "Stop clicked";
        stopStartAction->set_icon_name("media-playback-start");
        stopStartAction->set_label(_("Start"));
        stopStartAction->set_tooltip(_("Start plotting"));
    } else {
        debug() << "Start clicked";
        stopStartAction->set_icon_name("media-playback-pause");
        stopStartAction->set_label(_("Stop"));
        stopStartAction->set_tooltip(_("Stop plotting"));
    }

    running = !running;

    PortReader::instance().toggleAcquire(running);
}

void YarpScope::MainWindow::Private::on_action_interval_value_changed()
{
    int interval = intervalSpinButton.get_value_as_int();
    PortReader::instance().setInterval(interval);
}

void YarpScope::MainWindow::Private::on_action_actions_clear()
{
    PortReader::instance().clearData();
}

void YarpScope::MainWindow::Private::on_action_actions_autorescale()
{
    PlotManager::instance().autoRescale();;
}

YarpScope::MainWindow::MainWindow() :
        mPriv(new Private(this))
{
    set_border_width(3);
    set_default_size(640, 480);
    set_icon_name("yarpscope"); // FIXME

    add(mPriv->windowBox);

    // Setup actions
    mPriv->refActionGroup = Gtk::ActionGroup::create();

    mPriv->refActionGroup->add(Gtk::Action::create("MenuFile", _("_File")));
    mPriv->refActionGroup->add(Gtk::Action::create("Quit", Gtk::Stock::QUIT),
                sigc::mem_fun(*mPriv, &MainWindow::Private::on_action_file_quit));
    mPriv->refActionGroup->add(Gtk::Action::create("MenuActions", _("_Actions")));
    mPriv->refActionGroup->add(Gtk::ToggleAction::create_with_icon_name("StopStart", "media-playback-pause", _("Stop"), _("Stop plotting")),
                sigc::mem_fun(*mPriv, &MainWindow::Private::on_action_actions_stop_start));
    mPriv->refActionGroup->add(Gtk::Action::create_with_icon_name("Clear", "edit-clear", _("Clear"), _("Clear plots")),
                sigc::mem_fun(*mPriv, &MainWindow::Private::on_action_actions_clear));
    mPriv->refActionGroup->add(Gtk::Action::create_with_icon_name("AutoRescale", "transform-scale", _("Auto Rescale"), _("Automatically rescale plots")),
                sigc::mem_fun(*mPriv, &MainWindow::Private::on_action_actions_autorescale));
    mPriv->refActionGroup->add(Gtk::Action::create("MenuHelp", _("Help")));
    mPriv->refActionGroup->add(Gtk::Action::create("About", Gtk::Stock::ABOUT),
                sigc::mem_fun(*mPriv, &MainWindow::Private::on_action_help_about));

    mPriv->refUIManager->insert_action_group(mPriv->refActionGroup);
    add_accel_group(mPriv->refUIManager->get_accel_group());

    Glib::ustring ui_info =
    "<ui>"
    "  <menubar name='MenuBar'>"
    "    <menu action='MenuFile'>"
    "      <separator />"
    "      <menuitem action='Quit'/>"
    "    </menu>"
    "    <menu action='MenuActions'>"
    "      <menuitem action='StopStart'/>"
    "      <menuitem action='Clear'/>"
    "      <menuitem action='AutoRescale'/>"
    "    </menu>"
    "    <menu action='MenuHelp'>"
    "      <menuitem action='About'/>"
    "    </menu>"
    "  </menubar>"
    "  <toolbar  name='ToolBar'>"
    "    <toolitem action='StopStart'/>"
    "    <toolitem action='Clear'/>"
    "    <toolitem action='AutoRescale'/>"
    "  </toolbar>"
    "</ui>";

    try {
        mPriv->refUIManager->add_ui_from_string(ui_info);
    }
    catch(const Glib::Error& ex) {
        fatal() << "building menus failed: " <<  ex.what();
    }

    // Setup menu bar
    Gtk::MenuBar* menubar = dynamic_cast<Gtk::MenuBar*>(mPriv->refUIManager->get_widget("/MenuBar"));
    if(menubar) {
        mPriv->windowBox.pack_start(*menubar, Gtk::PACK_SHRINK);
    } else {
        fatal() << "building menus failed: \"/MenuBar\" is missing";
    }

    // Setup toolbar
    Gtk::Toolbar* toolbar = dynamic_cast<Gtk::Toolbar*>(mPriv->refUIManager->get_widget("/ToolBar"));
    if(toolbar) {
        mPriv->windowBox.pack_start(*toolbar, Gtk::PACK_SHRINK);
        toolbar->set_toolbar_style(Gtk::TOOLBAR_BOTH);
    } else {
        fatal() << "building menus failed: \"/ToolBar\" is missing";
    }

    mPriv->intervalSpinButton.set_value(PortReader::instance().interval());
    mPriv->intervalSpinButton.signal_value_changed().connect(sigc::mem_fun(*mPriv, &YarpScope::MainWindow::Private::on_action_interval_value_changed));
    mPriv->intervalVBox.pack_start(mPriv->intervalSpinButton);
    mPriv->intervalVBox.pack_start(mPriv->intervalLabel);
    mPriv->intervalToolItem.add(mPriv->intervalVBox);
    toolbar->prepend(mPriv->intervalToolItem);

    mPriv->windowBox.pack_start(*PlotManager::instance().getPlotWidget());

    show_all_children();
}

YarpScope::MainWindow::~MainWindow()
{
    delete mPriv;
}

void YarpScope::MainWindow::setInterval(int interval)
{
    mPriv->intervalSpinButton.set_value(interval);
}

