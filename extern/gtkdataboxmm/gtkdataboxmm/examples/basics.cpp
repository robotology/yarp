/* Copyright (c) 2012  iCub Facility, Istituto Italiano di Tecnologia
 *   @author Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 * Copyright (C) 1998 - 2008  Dr. Roland Bock
 *
 * This file is part of gtkdataboxmm.
 *
 * gtkdataboxmm is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 2.1 of the License,
 * or (at your option) any later version.
 *
 * gtkdataboxmm is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <gtkmm.h>
#include <gtkdataboxmm.h>


const unsigned int points = 2000;

// ----------------------------------------------------------------
// databox basics
// ----------------------------------------------------------------

static void create_basics (void)
{
    // We define some data */
    float *X = new float[points];
    float *Y = new float[points];

    for (unsigned int i = 0; i < points; i++)
    {
        X[i] = i;
        Y[i] = sin (i * 4 * G_PI / points);
    }

    Gtk::Window window;

    window.resize(500, 500);
    window.set_title("GtkDataboxMM: Basics");
    window.set_border_width(0);

    Gtk::VBox vbox(false, 0);
    window.add(vbox);

    Gtk::Label label("\n"
                     "The code for this example demonstrates the simplest way to use a\n"
                     "GtkDataboxMM widget.\n"
                     "\n"
                     "Usage:\n"
                     "  Draw a selection with the left button pressed,\n"
                     "  Than click into the selection.\n"
                     "  Use the right mouse button to zoom out.\n"
                     "  Shift + right mouse button zooms to default.");

    vbox.pack_start(label, false, false, 0);

    Gtk::HSeparator separator1;
    vbox.pack_start(separator1, false, false, 0);

    // -----------------------------------------------------------------
    // This is all you need:
    // -----------------------------------------------------------------

    // Create the GDatabox::Databox widget
    Gtk::Table *table;
    GDatabox::Databox box(&table, true, true, true, true);
    vbox.pack_start(*table, true, true, 0);

    Glib::RefPtr<GDatabox::Graph> graph = GDatabox::Points::create(points, X, Y, Gdk::Color("Black"), 1);
    box.graph_add(graph);

    box.set_total_limits(-1000., 5000., -10000., 23000.);
    box.auto_rescale(0.05);

    // -----------------------------------------------------------------
    // Done :-)
    // -----------------------------------------------------------------

    Gtk::HSeparator separator2;
    vbox.pack_start(separator2, false, true, 0);

    Gtk::Button close_button("close");

    vbox.pack_start(close_button, false, false, 0);
    close_button.set_can_default(GTK_CAN_DEFAULT);
    close_button.grab_default();
    close_button.grab_focus();

    close_button.signal_clicked().connect(sigc::ptr_fun(&Gtk::Main::quit));

    window.show_all();

    Gtk::Main::run(window);

}

int main (int argc, char *argv[])
{
    // Init gtkmm and gtkdataboxmm
    Gtk::Main kit(argc, argv);
    GDatabox::init();

    create_basics();

    return 0;
}
