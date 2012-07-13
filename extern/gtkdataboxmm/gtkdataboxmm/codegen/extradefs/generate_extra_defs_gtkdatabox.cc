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

#include <glibmm_generate_extra_defs/generate_extra_defs.h>
#include <gtk/gtk.h>

#include <gtkdatabox.h>
#include <gtkdatabox_bars.h>
#include <gtkdatabox_cross_simple.h>
#include <gtkdatabox_graph.h>
#include <gtkdatabox_grid.h>
#include <gtkdatabox_lines.h>
#include <gtkdatabox_markers.h>
#include <gtkdatabox_points.h>
#include <gtkdatabox_ruler.h>
#include <gtkdatabox_xyc_graph.h>

#include <iostream>

int main(int argc, char** argv)
{
  gtk_init(&argc, &argv);

  std::cout << get_defs(GTK_TYPE_DATABOX);
  std::cout << get_defs(GTK_DATABOX_TYPE_BARS);
  std::cout << get_defs(GTK_DATABOX_TYPE_CROSS_SIMPLE);
  std::cout << get_defs(GTK_DATABOX_TYPE_GRAPH);
  std::cout << get_defs(GTK_DATABOX_TYPE_GRID);
  std::cout << get_defs(GTK_DATABOX_TYPE_LINES);
  std::cout << get_defs(GTK_DATABOX_TYPE_MARKERS);
  std::cout << get_defs(GTK_DATABOX_TYPE_POINTS);
  std::cout << get_defs(GTK_DATABOX_TYPE_RULER);
  std::cout << get_defs(GTK_DATABOX_TYPE_XYC_GRAPH);

  return 0;
}
