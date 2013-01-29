/* Copyright (C) 1998-2006 The gtkmm Development Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <iostream>

#include <glibmm_generate_extra_defs/generate_extra_defs.h>
#include <gdk/gdk.h>
#include <gtk/gtk.h>
#include <goocanvas.h>


int main (int argc, char *argv[])
{
  gtk_init(&argc, &argv);

  std::cout << get_defs(GOO_TYPE_CANVAS);
  std::cout << get_defs(GOO_TYPE_CANVAS_ELLIPSE);
  std::cout << get_defs(GOO_TYPE_CANVAS_ELLIPSE_MODEL);
  std::cout << get_defs(GOO_TYPE_CANVAS_GRID);
  std::cout << get_defs(GOO_TYPE_CANVAS_GRID_MODEL);
  std::cout << get_defs(GOO_TYPE_CANVAS_GROUP);
  std::cout << get_defs(GOO_TYPE_CANVAS_GROUP_MODEL);
  std::cout << get_defs(GOO_TYPE_CANVAS_IMAGE);
  std::cout << get_defs(GOO_TYPE_CANVAS_IMAGE_MODEL);
  std::cout << get_defs(GOO_TYPE_CANVAS_ITEM);
  std::cout << get_defs(GOO_TYPE_CANVAS_ITEM_MODEL);
  std::cout << get_defs(GOO_TYPE_CANVAS_ITEM_SIMPLE);
  std::cout << get_defs(GOO_TYPE_CANVAS_ITEM_MODEL_SIMPLE);
  std::cout << get_defs(GOO_TYPE_CANVAS_LINE_DASH);      //Causes an error.
  std::cout << get_defs(GOO_TYPE_CANVAS_PATH);
  std::cout << get_defs(GOO_TYPE_CANVAS_PATH_MODEL);
  std::cout << get_defs(GOO_TYPE_CANVAS_POINTS);        //Causes an error.
  std::cout << get_defs(GOO_TYPE_CANVAS_POLYLINE);
  std::cout << get_defs(GOO_TYPE_CANVAS_POLYLINE_MODEL);
  std::cout << get_defs(GOO_TYPE_CANVAS_RECT);
  std::cout << get_defs(GOO_TYPE_CANVAS_RECT_MODEL);
  std::cout << get_defs(GOO_TYPE_CANVAS_STYLE);
  std::cout << get_defs(GOO_TYPE_CANVAS_TABLE);
  std::cout << get_defs(GOO_TYPE_CANVAS_TABLE_MODEL);
  std::cout << get_defs(GOO_TYPE_CANVAS_TEXT);
  std::cout << get_defs(GOO_TYPE_CANVAS_TEXT_MODEL);
  std::cout << get_defs(GOO_TYPE_CANVAS_WIDGET);

    return 0;
}
