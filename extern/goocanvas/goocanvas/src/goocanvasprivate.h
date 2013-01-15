/*
 * GooCanvas. Copyright (C) 2005-6 Damon Chaplin.
 * Released under the GNU LGPL license. See COPYING for details.
 *
 * goocanvasprivate.h - private types & utility functions.
 */
#ifndef __GOO_CANVAS_PRIVATE_H__
#define __GOO_CANVAS_PRIVATE_H__

#include <gtk/gtk.h>
#include "goocanvasstyle.h"

G_BEGIN_DECLS


/*
 * GPtrArray extensions.
 */
void goo_canvas_util_ptr_array_insert     (GPtrArray *ptr_array,
					   gpointer   data,
					   gint       index);

void goo_canvas_util_ptr_array_move       (GPtrArray *ptr_array,
					   gint       old_index,
					   gint       new_index);

gint goo_canvas_util_ptr_array_find_index (GPtrArray *ptr_array,
					   gpointer   data);


cairo_pattern_t* goo_canvas_cairo_pattern_from_pixbuf (GdkPixbuf *pixbuf);
cairo_surface_t* goo_canvas_cairo_surface_from_pixbuf (GdkPixbuf *pixbuf);

guint goo_canvas_convert_colors_to_rgba (double red,
					 double green,
					 double blue,
					 double alpha);

void goo_canvas_get_rgba_value_from_pattern (cairo_pattern_t *pattern,
					     GValue          *value);

void goo_canvas_set_style_property_from_pattern (GooCanvasStyle  *style,
						 GQuark           property_id,
						 cairo_pattern_t *pattern);

cairo_pattern_t* goo_canvas_create_pattern_from_color_value  (const GValue *value);
cairo_pattern_t* goo_canvas_create_pattern_from_rgba_value   (const GValue *value);
cairo_pattern_t* goo_canvas_create_pattern_from_pixbuf_value (const GValue *value);


gboolean goo_canvas_boolean_handled_accumulator (GSignalInvocationHint *ihint,
						 GValue                *return_accu,
						 const GValue          *handler_return,
						 gpointer               dummy);


G_END_DECLS

#endif /* __GOO_CANVAS_PRIVATE_H__ */
