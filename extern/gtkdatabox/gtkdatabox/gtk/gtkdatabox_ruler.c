/* $Id: gtkdatabox_ruler.c 4 2008-06-22 09:19:11Z rbock $ */
/* GTK - The GIMP Toolkit
 * Copyright (C) 1995-1997 Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/*
 * Modified by the GTK+ Team and others 1997-2000.  See the AUTHORS
 * file for a list of people on the GTK+ Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GTK+ at ftp://ftp.gtk.org/pub/gtk/. 
 */

/* Modified by Roland Bock 2007, see ChangeLog */

#include <gtkdatabox_ruler.h>

#include <math.h>
#include <glib/gprintf.h>

#define RULER_SIZE          14

#define ROUND(x) ((int) ((x) + 0.5))

static void gtk_databox_ruler_draw_ticks (GtkDataboxRuler * ruler);
static void gtk_databox_ruler_draw_pos (GtkDataboxRuler * ruler);
static gint gtk_databox_ruler_motion_notify (GtkWidget * widget,
					     GdkEventMotion * event);
static void gtk_databox_ruler_realize (GtkWidget * widget);
static void gtk_databox_ruler_unrealize (GtkWidget * widget);
static void gtk_databox_ruler_size_allocate (GtkWidget * widget,
					     GtkAllocation * allocation);
static gint gtk_databox_ruler_expose (GtkWidget * widget,
				      GdkEventExpose * event);
static void gtk_databox_ruler_make_pixmap (GtkDataboxRuler * ruler);
static void gtk_databox_ruler_set_property (GObject * object,
					    guint prop_id,
					    const GValue * value,
					    GParamSpec * pspec);
static void gtk_databox_ruler_get_property (GObject * object,
					    guint prop_id,
					    GValue * value,
					    GParamSpec * pspec);

enum
{
   PROP_0,
   PROP_LOWER,
   PROP_UPPER,
   PROP_POSITION,
   PROP_MAX_LENGTH,
   PROP_ORIENTATION,
   PROP_END_OF_LIST
};

struct _GtkDataboxRulerPrivate
{
   GdkPixmap *backing_pixmap;
   gint xsrc;
   gint ysrc;
   /* The lower limit of the ruler */
   gdouble lower;
   /* The upper limit of the ruler */
   gdouble upper;
   /* The position of the mark on the ruler */
   gdouble position;
   /* The maximum length of the labels (in characters) */
   guint max_length;
   /* The scale type of the ruler */
   GtkDataboxScaleType scale_type;
   /* Orientation of the ruler */
   GtkOrientation orientation;
};

G_DEFINE_TYPE (GtkDataboxRuler, gtk_databox_ruler, GTK_TYPE_WIDGET)

static void gtk_databox_ruler_class_init (GtkDataboxRulerClass * class)
{
   GObjectClass *gobject_class;
   GtkWidgetClass *widget_class;

   gobject_class = G_OBJECT_CLASS (class);
   widget_class = (GtkWidgetClass *) class;

   gobject_class->set_property = gtk_databox_ruler_set_property;
   gobject_class->get_property = gtk_databox_ruler_get_property;

   widget_class->realize = gtk_databox_ruler_realize;
   widget_class->unrealize = gtk_databox_ruler_unrealize;
   widget_class->size_allocate = gtk_databox_ruler_size_allocate;
   widget_class->expose_event = gtk_databox_ruler_expose;
   widget_class->motion_notify_event = gtk_databox_ruler_motion_notify;

   g_object_class_install_property (gobject_class,
				    PROP_LOWER,
				    g_param_spec_double ("lower",
							 "Lower",
							 "Lower limit of ruler",
							 -G_MAXDOUBLE,
							 G_MAXDOUBLE,
							 0.0,
							 G_PARAM_READWRITE));

   g_object_class_install_property (gobject_class,
				    PROP_UPPER,
				    g_param_spec_double ("upper",
							 "Upper",
							 "Upper limit of ruler",
							 -G_MAXDOUBLE,
							 G_MAXDOUBLE,
							 0.0,
							 G_PARAM_READWRITE));

   g_object_class_install_property (gobject_class,
				    PROP_POSITION,
				    g_param_spec_double ("position",
							 "Position",
							 "Position of mark on the ruler",
							 -G_MAXDOUBLE,
							 G_MAXDOUBLE,
							 0.0,
							 G_PARAM_READWRITE));

   g_object_class_install_property (gobject_class,
				    PROP_MAX_LENGTH,
				    g_param_spec_uint ("max-length",
						       "Max Length",
						       "Maximum length of the labels (in digits)",
						       2,
						       GTK_DATABOX_RULER_MAX_MAX_LENGTH,
						       6, G_PARAM_READWRITE));
   g_object_class_install_property (gobject_class,
				    PROP_ORIENTATION,
				    g_param_spec_uint ("orientation",
						       "Orientation",
						       "Orientation of the ruler: horizontal or vertical",
						       GTK_ORIENTATION_HORIZONTAL,
						       GTK_ORIENTATION_VERTICAL,
						       GTK_ORIENTATION_HORIZONTAL,
						       G_PARAM_READWRITE |
						       G_PARAM_CONSTRUCT_ONLY));
}

static void
gtk_databox_ruler_init (GtkDataboxRuler * ruler)
{
   ruler->priv = g_new0 (GtkDataboxRulerPrivate, 1);
   ruler->priv->backing_pixmap = NULL;
   ruler->priv->xsrc = 0;
   ruler->priv->ysrc = 0;
   ruler->priv->lower = 0;
   ruler->priv->upper = 0;
   ruler->priv->position = 0;
   ruler->priv->max_length = 6;
   ruler->priv->scale_type = GTK_DATABOX_SCALE_LINEAR;
   ruler->priv->orientation = GTK_ORIENTATION_HORIZONTAL;
}

/**
 * gtk_databox_ruler_new:
 * @orientation: orientation of the ruler
 *
 * Creates a new #GtkDataboxRuler widget with the given @orientation (horizontal or vertical).
 *
 * Return value: A new #GtkDataboxRuler
 **/
GtkWidget *
gtk_databox_ruler_new (GtkOrientation orientation)
{
   return g_object_new (GTK_DATABOX_TYPE_RULER, "orientation", orientation,
			NULL);
}

static gint
gtk_databox_ruler_motion_notify (GtkWidget * widget, GdkEventMotion * event)
{
   GtkDataboxRuler *ruler;
   gint x;
   gint y;

   ruler = GTK_DATABOX_RULER (widget);

   if (event->is_hint)
   {
      gdk_window_get_pointer (widget->window, &x, &y, NULL);
   }
   else
   {
      x = event->x;
      y = event->y;
   }

   if (ruler->priv->orientation == GTK_ORIENTATION_HORIZONTAL)
      ruler->priv->position =
	 ruler->priv->lower +
	 ((ruler->priv->upper - ruler->priv->lower) * x) / widget->allocation.width;
   else
      ruler->priv->position =
	 ruler->priv->lower +
	 ((ruler->priv->upper - ruler->priv->lower) * y) / widget->allocation.height;

   g_object_notify (G_OBJECT (ruler), "position");

   /*  Make sure the ruler has been allocated already  */
   if (ruler->priv->backing_pixmap != NULL)
      gtk_databox_ruler_draw_pos (ruler);

   return FALSE;
}

static void
gtk_databox_ruler_set_property (GObject * object,
				guint prop_id,
				const GValue * value, GParamSpec * pspec)
{
   GtkDataboxRuler *ruler = GTK_DATABOX_RULER (object);

   switch (prop_id)
   {
   case PROP_LOWER:
      gtk_databox_ruler_set_range (ruler, g_value_get_double (value),
				   ruler->priv->upper, ruler->priv->position);
      break;
   case PROP_UPPER:
      gtk_databox_ruler_set_range (ruler, ruler->priv->lower,
				   g_value_get_double (value),
				   ruler->priv->position);
      break;
   case PROP_POSITION:
      gtk_databox_ruler_set_range (ruler, ruler->priv->lower, ruler->priv->upper,
				   g_value_get_double (value));
      break;
   case PROP_MAX_LENGTH:
      gtk_databox_ruler_set_max_length (ruler, g_value_get_uint (value));
      break;
   case PROP_ORIENTATION:
      gtk_databox_ruler_set_orientation (ruler,
					 (GtkOrientation)
					 g_value_get_uint (value));
      break;
   default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
   }
}

static void
gtk_databox_ruler_get_property (GObject * object,
				guint prop_id,
				GValue * value, GParamSpec * pspec)
{
   GtkDataboxRuler *ruler = GTK_DATABOX_RULER (object);

   switch (prop_id)
   {
   case PROP_LOWER:
      g_value_set_double (value, ruler->priv->lower);
      break;
   case PROP_UPPER:
      g_value_set_double (value, ruler->priv->upper);
      break;
   case PROP_POSITION:
      g_value_set_double (value, ruler->priv->position);
      break;
   case PROP_MAX_LENGTH:
      g_value_set_uint (value, ruler->priv->max_length);
      break;
   case PROP_ORIENTATION:
      g_value_set_uint (value, ruler->priv->orientation);
      break;
   default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
   }
}

/**
 * gtk_databox_ruler_set_range:
 * @ruler: a #GtkDataboxRuler
 * @lower: lower limit of the ruler
 * @upper: upper limit of the ruler
 * @position: current position of the mark on the ruler
 *
 * Sets values indicating the range and current position of a #GtkDataboxRuler.
 *
 * See gtk_databox_ruler_get_range().
 **/
void
gtk_databox_ruler_set_range (GtkDataboxRuler * ruler,
			     gdouble lower, gdouble upper, gdouble position)
{
   g_return_if_fail (GTK_DATABOX_IS_RULER (ruler));

   g_object_freeze_notify (G_OBJECT (ruler));
   if (ruler->priv->lower != lower)
   {
      ruler->priv->lower = lower;
      g_object_notify (G_OBJECT (ruler), "lower");
   }
   if (ruler->priv->upper != upper)
   {
      ruler->priv->upper = upper;
      g_object_notify (G_OBJECT (ruler), "upper");
   }
   if (ruler->priv->position != position)
   {
      ruler->priv->position = position;
      g_object_notify (G_OBJECT (ruler), "position");
   }
   g_object_thaw_notify (G_OBJECT (ruler));

   if (GTK_WIDGET_DRAWABLE (ruler))
      gtk_widget_queue_draw (GTK_WIDGET (ruler));
}

/** 
 * gtk_databox_ruler_set_max_length:
 * @ruler: A #GtkDataboxRuler widget
 * @max_length: Maximum length (digits) of tick labels
 *
 * This function sets the maximum number of digits to be used for each tick
 * label of the @ruler.
 *
 * The @max_length cannot be smaller than 2 and not bigger than
 * #GTK_DATABOX_RULER_MAX_MAX_LENGTH.
 *
 */
void
gtk_databox_ruler_set_max_length (GtkDataboxRuler * ruler, guint max_length)
{
   g_return_if_fail (GTK_DATABOX_IS_RULER (ruler));
   g_return_if_fail (max_length > 1);
   g_return_if_fail (max_length < GTK_DATABOX_RULER_MAX_MAX_LENGTH + 1);

   g_object_freeze_notify (G_OBJECT (ruler));
   if (ruler->priv->max_length != max_length)
   {
      ruler->priv->max_length = max_length;
      g_object_notify (G_OBJECT (ruler), "max-length");
   }
   g_object_thaw_notify (G_OBJECT (ruler));

   if (GTK_WIDGET_DRAWABLE (ruler))
      gtk_widget_queue_draw (GTK_WIDGET (ruler));
}

/** 
 * gtk_databox_ruler_set_scale_type:
 * @ruler: A #GtkDataboxRuler widget
 * @scale_type: The new scale type for @ruler (linear or logarithmic)
 *
 * This function sets the scale type of the @ruler.
 *
 */
void
gtk_databox_ruler_set_scale_type (GtkDataboxRuler * ruler,
				  GtkDataboxScaleType scale_type)
{
   g_return_if_fail (GTK_DATABOX_IS_RULER (ruler));

   if (ruler->priv->scale_type != scale_type)
   {
      ruler->priv->scale_type = scale_type;
      /* g_object_notify (G_OBJECT (ruler), "scale-type"); */
   }

   if (GTK_WIDGET_DRAWABLE (ruler))
      gtk_widget_queue_draw (GTK_WIDGET (ruler));
}

/**
 * gtk_databox_ruler_set_orientation:
 * @ruler: a #GtkDataboxRuler
 * @orientation: new orientation of the ruler
 *
 * Sets the orientation of the @ruler (horizontal or vertical).
 **/
void
gtk_databox_ruler_set_orientation (GtkDataboxRuler * ruler,
				   GtkOrientation orientation)
{
   GtkWidget *widget;
   g_return_if_fail (GTK_DATABOX_IS_RULER (ruler));

   if (ruler->priv->orientation != orientation)
   {
      ruler->priv->orientation = orientation;
      g_object_notify (G_OBJECT (ruler), "orientation");
   }

   widget = GTK_WIDGET (ruler);
   if (orientation == GTK_ORIENTATION_HORIZONTAL)
   {
      widget->requisition.width = widget->style->xthickness * 2 + 1;
      widget->requisition.height = widget->style->ythickness * 2 + RULER_SIZE;
   }
   else
   {
      widget->requisition.height = widget->style->ythickness * 2 + 1;
      widget->requisition.width = widget->style->xthickness * 2 + RULER_SIZE;
   }

   if (GTK_WIDGET_DRAWABLE (ruler))
      gtk_widget_queue_draw (GTK_WIDGET (ruler));
}

/**
 * gtk_databox_ruler_get_orientation:
 * @ruler: a #GtkDataboxRuler
 *
 * Gets the orientation of the @ruler (horizontal or vertical).
 *
 * Return value: Orientation of the @ruler.
 **/
GtkOrientation
gtk_databox_ruler_get_orientation (GtkDataboxRuler * ruler)
{

   g_return_val_if_fail (GTK_DATABOX_IS_RULER (ruler), -1);

   return ruler->priv->orientation;
}

/**
 * gtk_databox_ruler_get_range:
 * @ruler: a #GtkDataboxRuler
 * @lower: location to store lower limit of the ruler, or %NULL
 * @upper: location to store upper limit of the ruler, or %NULL
 * @position: location to store the current position of the mark on the ruler, or %NULL
 *
 * Retrieves values indicating the range and current position of a #GtkDataboxRuler.
 * See gtk_databox_ruler_set_range().
 **/
void
gtk_databox_ruler_get_range (GtkDataboxRuler * ruler,
			     gdouble * lower,
			     gdouble * upper, gdouble * position)
{
   g_return_if_fail (GTK_DATABOX_IS_RULER (ruler));

   if (lower)
      *lower = ruler->priv->lower;
   if (upper)
      *upper = ruler->priv->upper;
   if (position)
      *position = ruler->priv->position;
}

/** 
 * gtk_databox_ruler_get_max_length:
 * @ruler: A #GtkDataboxRuler widget
 *
 * This function returns the maximum number of digits to be used for each tick
 * label of the @ruler.
 *
 * Return value: The maximum length of the tick labels.
 *
 */
guint
gtk_databox_ruler_get_max_length (GtkDataboxRuler * ruler)
{
   g_return_val_if_fail (GTK_DATABOX_IS_RULER (ruler), 0);

   return ruler->priv->max_length;
}

/** 
 * gtk_databox_ruler_get_scale_type:
 * @ruler: A #GtkDataboxRuler widget
 *
 * This function returns the scale type of the @ruler (linear or logarithmic).
 *
 * Return value: The scale type (linear or logarithmic)
 *
 */
GtkDataboxScaleType
gtk_databox_ruler_get_scale_type (GtkDataboxRuler * ruler)
{
   g_return_val_if_fail (GTK_DATABOX_IS_RULER (ruler), 0);

   return ruler->priv->scale_type;
}

static void
gtk_databox_ruler_draw_ticks (GtkDataboxRuler * ruler)
{
   GtkWidget *widget;
   cairo_t *cr;
   gint i;
   gint width, height;
   gint xthickness;
   gint ythickness;
   gint length;
   gdouble lower, upper;	/* Upper and lower limits */
   gdouble increment;		/* pixel per value unit */
   gint power;
   gint digit;
   gdouble subd_incr;
   gdouble start, end, cur;
   gchar unit_str[GTK_DATABOX_RULER_MAX_MAX_LENGTH + 1];	/* buffer for writing numbers */
   gint digit_width;
   gint digit_height;
   gint digit_offset;
   gint text_width;
   gint pos;
   gchar format_string[10];
   PangoMatrix matrix = PANGO_MATRIX_INIT;
   PangoContext *context;
   PangoLayout *layout;
   PangoRectangle logical_rect, ink_rect;

   if (ruler->priv->scale_type == GTK_DATABOX_SCALE_LINEAR)
      g_sprintf (format_string, "%%-+%dg", ruler->priv->max_length - 1);
   else
      g_sprintf (format_string, "%%-%dg", ruler->priv->max_length - 1);


   if (!GTK_WIDGET_DRAWABLE (ruler))
      return;

   widget = GTK_WIDGET (ruler);

   xthickness = widget->style->xthickness;
   ythickness = widget->style->ythickness;

   layout = gtk_widget_create_pango_layout (widget, "E+-012456789");
   pango_layout_get_pixel_extents (layout, &ink_rect, &logical_rect);

   digit_width = ceil ((logical_rect.width) / 12);
   digit_height = (logical_rect.height) + 2;
   digit_offset = ink_rect.y;

   if (ruler->priv->orientation == GTK_ORIENTATION_VERTICAL)
   {
      context = gtk_widget_get_pango_context (widget);
      pango_context_set_base_gravity (context, PANGO_GRAVITY_WEST);
      pango_matrix_rotate (&matrix, 90.);
      pango_context_set_matrix (context, &matrix);
   }

   width = widget->allocation.width;
   height = widget->allocation.height;

   gtk_paint_box (widget->style, ruler->priv->backing_pixmap,
		  GTK_STATE_NORMAL, GTK_SHADOW_OUT,
		  NULL, widget, "ruler", 0, 0, width, height);

   cr = gdk_cairo_create (ruler->priv->backing_pixmap);
   gdk_cairo_set_source_color (cr, &widget->style->fg[widget->state]);

   cairo_rectangle (cr,
		    xthickness,
		    height - ythickness, width - 2 * xthickness, 1);

   if (ruler->priv->scale_type == GTK_DATABOX_SCALE_LINEAR)
   {
      upper = ruler->priv->upper;
      lower = ruler->priv->lower;
   }
   else if (ruler->priv->scale_type == GTK_DATABOX_SCALE_LOG2)
   {
      if (ruler->priv->upper <= 0 || ruler->priv->lower <= 0)
      {
	 g_warning
	    ("For logarithmic scaling, the visible limits must by larger than 0!");
      }
      upper = log2 (ruler->priv->upper);
      lower = log2 (ruler->priv->lower);
   }
   else
   {
      if (ruler->priv->upper <= 0 || ruler->priv->lower <= 0)
      {
	 g_warning
	    ("For logarithmic scaling, the visible limits must by larger than 0!");
      }
      upper = log10 (ruler->priv->upper);
      lower = log10 (ruler->priv->lower);
   }

   if ((upper - lower) == 0)
      goto out;

   if (ruler->priv->orientation == GTK_ORIENTATION_HORIZONTAL)
      increment = (gdouble) width / (upper - lower);
   else
      increment = (gdouble) height / (upper - lower);


   /* determine the scale, i.e. the distance between the most significant ticks
    *
    * the ticks have to be farther apart than the length of the displayed numbers
    */
   if (ruler->priv->scale_type == GTK_DATABOX_SCALE_LINEAR)
   {
      text_width = (ruler->priv->max_length) * digit_width + 1;

      for (power = -20; power < 21; power++)
      {
	 if ((digit = 1) * pow (10, power) * fabs (increment) > text_width)
	    break;
	 if ((digit = 2.5) * pow (10, power) * fabs (increment) > text_width)
	    break;
	 if ((digit = 5) * pow (10, power) * fabs (increment) > text_width)
	    break;
      }


      if (power == 21)
      {
	 power = 20;
	 digit = 5;
      }
      subd_incr = digit * pow (10, power);
   }
   else if (ruler->priv->scale_type == GTK_DATABOX_SCALE_LOG2)
   {
      subd_incr = 1.;
   }
   else
   {
      subd_incr = 1.;
   }

   length = (ruler->priv->orientation == GTK_ORIENTATION_HORIZONTAL)
      ? height - 1 : width - 1;

   if (lower < upper)
   {
      start = floor (lower / subd_incr) * subd_incr;
      end = ceil (upper / subd_incr) * subd_incr;
   }
   else
   {
      start = floor (upper / subd_incr) * subd_incr;
      end = ceil (lower / subd_incr) * subd_incr;
   }


   for (cur = start; cur <= end; cur += subd_incr)
   {
      pos = ROUND ((cur - lower) * increment);

      if (ruler->priv->orientation == GTK_ORIENTATION_HORIZONTAL)
	 cairo_rectangle (cr, pos, height + ythickness - length, 1, length);
      else
	 cairo_rectangle (cr, width + xthickness - length, pos, length, 1);


      /* draw label */
      if (ruler->priv->scale_type == GTK_DATABOX_SCALE_LINEAR)
      {
	 if (ABS (cur) < 0.1 * subd_incr)	/* Rounding errors occur and might make "0" look funny without this check */
	    cur = 0;

	 g_snprintf (unit_str, ruler->priv->max_length + 1, format_string, cur);
      }
      else if (ruler->priv->scale_type == GTK_DATABOX_SCALE_LOG2)
	 g_snprintf (unit_str, ruler->priv->max_length + 1, format_string,
		     pow (2, cur));
      else
	 g_snprintf (unit_str, ruler->priv->max_length + 1, format_string,
		     pow (10, cur));

      pango_layout_set_text (layout, unit_str, -1);
      pango_layout_get_pixel_extents (layout, NULL, &logical_rect);

      if (ruler->priv->orientation == GTK_ORIENTATION_HORIZONTAL)
	 gtk_paint_layout (widget->style,
			   ruler->priv->backing_pixmap,
			   GTK_WIDGET_STATE (widget),
			   FALSE,
			   NULL,
			   widget, "ruler", pos + 2, ythickness - 1, layout);
      else
	 gtk_paint_layout (widget->style,
			   ruler->priv->backing_pixmap,
			   GTK_WIDGET_STATE (widget),
			   FALSE,
			   NULL,
			   widget,
			   "ruler",
			   xthickness - 1,
			   pos - logical_rect.width - 2, layout);

      /* Draw sub-ticks */
      if (ruler->priv->scale_type == GTK_DATABOX_SCALE_LINEAR)
	 for (i = 1; i < 5; ++i)
	 {
	    pos = ROUND ((cur - lower + subd_incr / 5 * i) * increment);

	    if (ruler->priv->orientation == GTK_ORIENTATION_HORIZONTAL)
	       cairo_rectangle (cr,
				pos, height + ythickness - length / 2,
				1, length / 2);
	    else
	       cairo_rectangle (cr,
				width + xthickness - length / 2, pos,
				length / 2, 1);
	 }
      else  if (ruler->priv->scale_type == GTK_DATABOX_SCALE_LOG2)
	 for (i = 1; i < 8; ++i)
	 {
	    pos = ROUND ((cur - lower + log2 (i)) * increment);

	    if (ruler->priv->orientation == GTK_ORIENTATION_HORIZONTAL)
	       cairo_rectangle (cr,
				pos, height + ythickness - length / 2,
				1, length / 2);
	    else
	       cairo_rectangle (cr,
				width + xthickness - length / 2, pos,
				length / 2, 1);
	 }
      else
	 for (i = 2; i < 10; ++i)
	 {
	    pos = ROUND ((cur - lower + log10 (i)) * increment);

	    if (ruler->priv->orientation == GTK_ORIENTATION_HORIZONTAL)
	       cairo_rectangle (cr,
				pos, height + ythickness - length / 2,
				1, length / 2);
	    else
	       cairo_rectangle (cr,
				width + xthickness - length / 2, pos,
				length / 2, 1);
	 }

   }

   cairo_fill (cr);
 out:
   cairo_destroy (cr);

   g_object_unref (layout);
}

static void
gtk_databox_ruler_draw_pos (GtkDataboxRuler * ruler)
{
   GtkWidget *widget = GTK_WIDGET (ruler);
   gint x, y;
   gint width, height;
   gint bs_width, bs_height;
   gint xthickness;
   gint ythickness;
   gdouble increment;

   if (GTK_WIDGET_DRAWABLE (ruler))
   {
      xthickness = widget->style->xthickness;
      ythickness = widget->style->ythickness;
      width = widget->allocation.width - xthickness * 2;
      height = widget->allocation.height - ythickness * 2;

      if (ruler->priv->orientation == GTK_ORIENTATION_HORIZONTAL)
      {
	 bs_width = height / 2 + 2;
	 bs_width |= 1;		/* make sure it's odd */
	 bs_height = bs_width / 2 + 1;
      }
      else
      {
	 bs_height = width / 2 + 2;
	 bs_height |= 1;	/* make sure it's odd */
	 bs_width = bs_height / 2 + 1;
      }

      if ((bs_width > 0) && (bs_height > 0))
      {
	 cairo_t *cr = gdk_cairo_create (widget->window);

	 /*  If a backing store exists, restore the ruler  */
	 if (ruler->priv->backing_pixmap)
	    gdk_draw_drawable (widget->window,
			       widget->style->black_gc,
			       ruler->priv->backing_pixmap,
			       ruler->priv->xsrc, ruler->priv->ysrc,
			       ruler->priv->xsrc, ruler->priv->ysrc, bs_width, bs_height);

	 if (ruler->priv->orientation == GTK_ORIENTATION_HORIZONTAL)
	 {
	    increment = (gdouble) width / (ruler->priv->upper - ruler->priv->lower);

	    x = ROUND ((ruler->priv->position - ruler->priv->lower) * increment) +
	       (xthickness - bs_width) / 2 - 1;
	    y = (height + bs_height) / 2 + ythickness;

	    gdk_cairo_set_source_color (cr,
					&widget->style->fg[widget->state]);

	    cairo_move_to (cr, x, y);
	    cairo_line_to (cr, x + bs_width / 2., y + bs_height);
	    cairo_line_to (cr, x + bs_width, y);
	 }
	 else
	 {
	    increment = (gdouble) height / (ruler->priv->upper - ruler->priv->lower);

	    x = (width + bs_width) / 2 + xthickness;
	    y = ROUND ((ruler->priv->position - ruler->priv->lower) * increment) +
	       (ythickness - bs_height) / 2 - 1;

	    gdk_cairo_set_source_color (cr,
					&widget->style->fg[widget->state]);

	    cairo_move_to (cr, x, y);
	    cairo_line_to (cr, x + bs_width, y + bs_height / 2.);
	    cairo_line_to (cr, x, y + bs_height);
	 }
	 cairo_fill (cr);

	 cairo_destroy (cr);

	 ruler->priv->xsrc = x;
	 ruler->priv->ysrc = y;
      }
   }
}


static void
gtk_databox_ruler_realize (GtkWidget * widget)
{
   GtkDataboxRuler *ruler;
   GdkWindowAttr attributes;
   gint attributes_mask;

   ruler = GTK_DATABOX_RULER (widget);
   GTK_WIDGET_SET_FLAGS (ruler, GTK_REALIZED);

   attributes.window_type = GDK_WINDOW_CHILD;
   attributes.x = widget->allocation.x;
   attributes.y = widget->allocation.y;
   attributes.width = widget->allocation.width;
   attributes.height = widget->allocation.height;
   attributes.wclass = GDK_INPUT_OUTPUT;
   attributes.visual = gtk_widget_get_visual (widget);
   attributes.colormap = gtk_widget_get_colormap (widget);
   attributes.event_mask = gtk_widget_get_events (widget);
   attributes.event_mask |= (GDK_EXPOSURE_MASK |
			     GDK_POINTER_MOTION_MASK |
			     GDK_POINTER_MOTION_HINT_MASK);

   attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;

   widget->window =
      gdk_window_new (gtk_widget_get_parent_window (widget), &attributes,
		      attributes_mask);
   gdk_window_set_user_data (widget->window, ruler);

   widget->style = gtk_style_attach (widget->style, widget->window);
   gtk_style_set_background (widget->style, widget->window, GTK_STATE_ACTIVE);

   gtk_databox_ruler_make_pixmap (ruler);
}

static void
gtk_databox_ruler_unrealize (GtkWidget * widget)
{
   GtkDataboxRuler *ruler = GTK_DATABOX_RULER (widget);

   if (ruler->priv->backing_pixmap)
      g_object_unref (ruler->priv->backing_pixmap);

   g_free (ruler->priv);

   if (GTK_WIDGET_CLASS (gtk_databox_ruler_parent_class)->unrealize)
      (*GTK_WIDGET_CLASS (gtk_databox_ruler_parent_class)->
       unrealize) (widget);
}

static void
gtk_databox_ruler_size_allocate (GtkWidget * widget,
				 GtkAllocation * allocation)
{
   GtkDataboxRuler *ruler = GTK_DATABOX_RULER (widget);

   widget->allocation = *allocation;

   if (GTK_WIDGET_REALIZED (widget))
   {
      gdk_window_move_resize (widget->window,
			      allocation->x, allocation->y,
			      allocation->width, allocation->height);

      gtk_databox_ruler_make_pixmap (ruler);
   }
}

static gint
gtk_databox_ruler_expose (GtkWidget * widget, GdkEventExpose * event)
{
   GtkDataboxRuler *ruler;

   if (GTK_WIDGET_DRAWABLE (widget))
   {
      ruler = GTK_DATABOX_RULER (widget);

      gtk_databox_ruler_draw_ticks (ruler);

      gdk_draw_drawable (widget->window,
			 widget->style->fg_gc[GTK_WIDGET_STATE (ruler)],
			 ruler->priv->backing_pixmap,
			 0, 0, 0, 0,
			 widget->allocation.width, widget->allocation.height);

      gtk_databox_ruler_draw_pos (ruler);
   }

   return FALSE;
}

static void
gtk_databox_ruler_make_pixmap (GtkDataboxRuler * ruler)
{
   GtkWidget *widget;
   gint width;
   gint height;

   widget = GTK_WIDGET (ruler);

   if (ruler->priv->backing_pixmap)
   {
      gdk_drawable_get_size (ruler->priv->backing_pixmap, &width, &height);
      if ((width == widget->allocation.width) &&
	  (height == widget->allocation.height))
	 return;

      g_object_unref (ruler->priv->backing_pixmap);
   }

   ruler->priv->backing_pixmap = gdk_pixmap_new (widget->window,
					  widget->allocation.width,
					  widget->allocation.height, -1);

   ruler->priv->xsrc = 0;
   ruler->priv->ysrc = 0;
}

#define __GTK_DATABOX_RULER_C__
