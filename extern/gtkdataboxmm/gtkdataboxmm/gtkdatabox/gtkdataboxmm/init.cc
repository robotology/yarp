/* Copyright (c) 2012  iCub Facility, Istituto Italiano di Tecnologia
 *   @author Daniele E. Domenichelli <daniele.domenichelli@iit.it>
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
#include <gtkdataboxmmconfig.h>
#include <gtkdataboxmm/wrap_init.h>


namespace GDatabox
{

void init()
{
    Gtk::Main::init_gtkmm_internals(); //Sets up the g type system and the Glib::wrap() table.
    wrap_init(); //Tells the Glib::wrap() table about the libsomethingmm classes.
}

}
