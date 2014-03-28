/* $Id: gtkdatabox.c 4 2008-06-22 09:19:11Z rbock $ */
/* GtkDatabox - An extension to the gtk+ library
 * Copyright (C) 1998 - 2008  Dr. Roland Bock
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <gtkdatabox.h>
#include <gtkdatabox_marshal.h>
#include <gtk/gtkhscrollbar.h>
#include <gtk/gtkvscrollbar.h>
#include <gtkdatabox_ruler.h>
#include <gtk/gtktable.h>
#include <gtk/gtkgc.h>
#include <math.h>


static gint gtk_databox_button_press (GtkWidget * widget,
				      GdkEventButton * event);
static gint gtk_databox_button_release (GtkWidget * widget,
					GdkEventButton * event);
static gint gtk_databox_motion_notify (GtkWidget * widget,
				       GdkEventMotion * event);
static void gtk_databox_realize (GtkWidget * widget);
static void gtk_databox_unrealize (GtkWidget * widget);
static void gtk_databox_size_allocate (GtkWidget * widget,
				       GtkAllocation * allocation);
static gint gtk_databox_expose (GtkWidget * widget,
		                GdkEventExpose * event);
static void gtk_databox_set_property (GObject * object,
					    guint prop_id,
					    const GValue * value,
					    GParamSpec * pspec);
static void gtk_databox_get_property (GObject * object,
					    guint prop_id,
					    GValue * value,
					    GParamSpec * pspec);

static gfloat gtk_databox_get_offset_x (GtkDatabox* box);
static gfloat gtk_databox_get_page_size_x (GtkDatabox* box);
static gfloat gtk_databox_get_offset_y (GtkDatabox* box);
static gfloat gtk_databox_get_page_size_y (GtkDatabox* box);
static void gtk_databox_calculate_visible_limits (GtkDatabox * box);
static void gtk_databox_create_backing_pixmap (GtkDatabox * box);
static void gtk_databox_calculate_selection_values (GtkDatabox * box);
static void gtk_databox_selection_cancel (GtkDatabox * box);
static void gtk_databox_draw_selection (GtkDatabox * box,
					GdkRectangle * pixmapCopyRect);
static void gtk_databox_adjustment_value_changed (GtkDatabox * box);
static void gtk_databox_ruler_update (GtkDatabox * box);

/* IDs of signals */
enum
{
   ZOOMED_SIGNAL,
   SELECTION_STARTED_SIGNAL,
   SELECTION_CHANGED_SIGNAL,
   SELECTION_FINALIZED_SIGNAL,
   SELECTION_CANCELED_SIGNAL,
   LAST_SIGNAL
};

/* signals will be configured during class_init */
static gint gtk_databox_signals[LAST_SIGNAL] = { 0 };


/* IDs of properties */
enum
{
   ENABLE_SELECTION = 1,
   ENABLE_ZOOM,
   ADJUSTMENT_X,
   ADJUSTMENT_Y,
   RULER_X,
   RULER_Y,
   SCALE_TYPE_X,
   SCALE_TYPE_Y,
   LAST_PROPERTY
};

struct _GtkDataboxPrivate
{
   GdkPixmap *backing_pixmap;

   /* Total and visible limits (values, not pixels) */
   gfloat total_left;
   gfloat total_right;
   gfloat total_top;
   gfloat total_bottom;
   gfloat visible_left;
   gfloat visible_right;
   gfloat visible_top;
   gfloat visible_bottom;

   /* Translation information between values and pixels */
   GtkDataboxScaleType scale_type_x;
   GtkDataboxScaleType scale_type_y;
   gfloat translation_factor_x;
   gfloat translation_factor_y;

   /* Properties */
   gboolean enable_selection;
   gboolean enable_zoom;
   GtkAdjustment *adj_x;
   GtkAdjustment *adj_y;
   GtkDataboxRuler *ruler_x;
   GtkDataboxRuler *ruler_y;

   /* Other private stuff */
   GList *graphs;
   GdkGC *select_gc;
   GdkPoint marked;
   GdkPoint select;
   GtkDataboxValueRectangle selectionValues;
   gfloat zoom_limit;

   /* flags */
   gboolean selection_active;
   gboolean selection_finalized;
};

/** 
 * gtk_databox_get_type
 *
 * Determines the #GType of the GtkDatabox widget type.
 *
 * Return value: The #GType of the GtkDatabox widget type.
 *
 */
G_DEFINE_TYPE (GtkDatabox, gtk_databox, GTK_TYPE_WIDGET)

static void
gtk_databox_class_init (GtkDataboxClass * class)
{
   GObjectClass *gobject_class;
   GtkWidgetClass *widget_class;

   gobject_class = G_OBJECT_CLASS (class);
   widget_class = (GtkWidgetClass *) class;

   gobject_class->set_property = gtk_databox_set_property;
   gobject_class->get_property = gtk_databox_get_property;

   widget_class->realize = gtk_databox_realize;
   widget_class->unrealize = gtk_databox_unrealize;
   widget_class->size_allocate = gtk_databox_size_allocate;
   widget_class->expose_event = gtk_databox_expose;
   widget_class->motion_notify_event = gtk_databox_motion_notify;
   widget_class->button_press_event = gtk_databox_button_press;
   widget_class->button_release_event = gtk_databox_button_release;

   /**
    * GtkDatabox:enable-selection:
    *
    * Defines whether the user can select 
    * rectangular areas with the mouse (#TRUE) or not (#FALSE). 
    *
    */
   g_object_class_install_property (gobject_class,
         ENABLE_SELECTION, 
         g_param_spec_boolean ("enable-selection",
            "Enable Selection",
            "Enable selection of areas via mouse (TRUE/FALSE)",
            TRUE,       /* default value */
            G_PARAM_CONSTRUCT | G_PARAM_READWRITE));

   /**
    * GtkDatabox:enable-zoom:
    *
    * Defines whether the user can use the mouse to zoom in or out (#TRUE) or not (#FALSE).
    *
    */
   g_object_class_install_property (gobject_class,
         ENABLE_ZOOM, 
         g_param_spec_boolean ("enable-zoom",
            "Enable Zoom",
            "Enable zooming in or out via mouse click (TRUE/FALSE)",
            TRUE,       /* default value */
            G_PARAM_CONSTRUCT | G_PARAM_READWRITE));

   /**
    * GtkDatabox:adjustment_x:
    *
    * The #GtkAdjustment associated with the horizontal scrollbar. The #GtkDatabox widget 
    * creates a GtkAdjustment itself. Normally there is no need for you to create another 
    * GtkAdjustment. You could simply use the one you get via gtk_databox_get_adjustment_x().
    *
    */
   g_object_class_install_property (gobject_class,
         ADJUSTMENT_X, 
         g_param_spec_object ("adjustment-x",
            "Horizontal Adjustment",
            "GtkAdjustment for horizontal scrolling",
            GTK_TYPE_ADJUSTMENT,
            G_PARAM_CONSTRUCT | G_PARAM_READWRITE));

   /**
    * GtkDatabox:adjustment_y:
    *
    * The #GtkAdjustment associated with the vertical scrollbar. The #GtkDatabox widget 
    * creates a GtkAdjustment itself. Normally there is no need for you to create another 
    * GtkAdjustment. You could simply use the one you get via gtk_databox_get_adjustment_y().
    *
    */
   g_object_class_install_property (gobject_class,
         ADJUSTMENT_Y,
         g_param_spec_object ("adjustment-y",
            "Vertical Adjustment",
            "GtkAdjustment for vertical scrolling",
            GTK_TYPE_ADJUSTMENT,
            G_PARAM_CONSTRUCT | G_PARAM_READWRITE));

   /**
    * GtkDatabox:ruler_x:
    *
    * The horizontal %GtkDataboxRuler (or NULL).
    *
    */
   g_object_class_install_property (gobject_class,
         RULER_X, 
         g_param_spec_object ("ruler-x",
            "Horizontal Ruler",
            "A horizontal GtkDataboxRuler or NULL",
            GTK_DATABOX_TYPE_RULER,
            G_PARAM_CONSTRUCT | G_PARAM_READWRITE));

   /**
    * GtkDatabox:ruler_y:
    *
    * The vertical %GtkDataboxRuler (or NULL).
    *
    */
   g_object_class_install_property (gobject_class,
         RULER_Y, 
         g_param_spec_object ("ruler-y",
            "Vertical Ruler",
            "A vertical GtkDataboxRuler or NULL",
            GTK_DATABOX_TYPE_RULER,
            G_PARAM_CONSTRUCT | G_PARAM_READWRITE));

   /**
    * GtkDatabox:scale_type_x:
    *
    * The horizontal scale type (linear or lograrithmic).
    */
   g_object_class_install_property (gobject_class,
         SCALE_TYPE_X,
         g_param_spec_enum ("scale-type-x",
            "Horizontal scale type",
            "Horizontal scale type (linear or logarithmic)",
            gtk_databox_scale_type_get_type (),
            GTK_DATABOX_SCALE_LINEAR,
            G_PARAM_CONSTRUCT | G_PARAM_READWRITE));

   /**
    * GtkDatabox:scale_type_y:
    *
    * The vertical scale type (linear or lograrithmic).
    */
   g_object_class_install_property (gobject_class,
         SCALE_TYPE_Y,
         g_param_spec_enum ("scale-type-y",
            "Vertical scale type",
            "Vertical scale type (linear or logarithmic)",
            gtk_databox_scale_type_get_type (),
            GTK_DATABOX_SCALE_LINEAR,
            G_PARAM_CONSTRUCT | G_PARAM_READWRITE));


   /**
    * GtkDatabox::zoomed:
    * @box: The #GtkDatabox widget which zoomed in or out.
    *
    * This signal is emitted each time the zoom of the widget is changed, see for example 
    * gtk_databox_zoom_to_selection(), gtk_databox_set_visible_limits().
    */
   gtk_databox_signals[ZOOMED_SIGNAL] = 
      g_signal_new ("zoomed", 
            G_TYPE_FROM_CLASS (gobject_class), 
            G_SIGNAL_RUN_FIRST, 
            G_STRUCT_OFFSET (GtkDataboxClass, zoomed), 
            NULL,	/* accumulator */
	    NULL,	/* accumulator_data */
	    gtk_databox_marshal_VOID__VOID,
	    G_TYPE_NONE, 0);

   /**
    * GtkDatabox::selection-started:
    * @box: The #GtkDatabox widget in which the selection has been started.
    * @selection_values: The corners of the selection rectangle.
    *
    * This signal is emitted when the mouse is firstmoved 
    * with the left button pressed after the mouse-down (and the #GtkDatabox:enable-selection property 
    * is set). The corners of the selection rectangle are stored in @selection_values.
    *
    * @see_also: #GtkDatabox::selection-changed
    */
   gtk_databox_signals[SELECTION_STARTED_SIGNAL] = 
      g_signal_new ("selection-started", 
            G_TYPE_FROM_CLASS (gobject_class), 
            G_SIGNAL_RUN_FIRST, 
            G_STRUCT_OFFSET (GtkDataboxClass, selection_started), 
            NULL,	/* accumulator */
	    NULL,	/* accumulator_data */
	    gtk_databox_marshal_VOID__POINTER,
	    G_TYPE_NONE,
	    1,
	    G_TYPE_POINTER);

   /**
    * GtkDatabox::selection-changed:
    * @box: The #GtkDatabox widget in which the selection was changed.
    * @selection_values: The corners of the selection rectangle.
    *
    * This signal is emitted when the mouse is moved 
    * with the left button pressed (and the #GtkDatabox:enable-selection property 
    * is set). The corners of the selection rectangle are stored in @selection_values.
    */
   gtk_databox_signals[SELECTION_CHANGED_SIGNAL] = 
      g_signal_new ("selection-changed", 
            G_TYPE_FROM_CLASS (gobject_class), 
            G_SIGNAL_RUN_FIRST, 
            G_STRUCT_OFFSET (GtkDataboxClass, selection_changed), 
            NULL,	/* accumulator */
	    NULL,	/* accumulator_data */
	    gtk_databox_marshal_VOID__POINTER,
	    G_TYPE_NONE,
            1,
	    G_TYPE_POINTER);

  /**
    * GtkDatabox::selection-finalized:
    * @box: The #GtkDatabox widget in which the selection has been stopped.
    * @selection_values: The corners of the selection rectangle.
    *
    * This signal is emitted when the left mouse button
    * is released after a selection was started before.
    *
    * @see_also: #GtkDatabox::selection-changed
    */
    gtk_databox_signals[SELECTION_FINALIZED_SIGNAL] = 
       g_signal_new ("selection-finalized", 
             G_TYPE_FROM_CLASS (gobject_class), 
             G_SIGNAL_RUN_FIRST, 
             G_STRUCT_OFFSET (GtkDataboxClass, selection_finalized), 
             NULL,	/* accumulator */
	     NULL,	/* accumulator_data */
             gtk_databox_marshal_VOID__POINTER,
	     G_TYPE_NONE,
	     1,
	     G_TYPE_POINTER);

  /**
    * GtkDatabox::selection-canceled:
    * @box: The #GtkDatabox widget which zoomed in or out.
    *
    * This signal is emitted after a right click outside 
    * a selection rectangle.
    */
    gtk_databox_signals[SELECTION_CANCELED_SIGNAL] = 
       g_signal_new ("selection-canceled", 
             G_TYPE_FROM_CLASS (gobject_class), 
             G_SIGNAL_RUN_FIRST, 
             G_STRUCT_OFFSET (GtkDataboxClass, selection_canceled), 
             NULL,	/* accumulator */
             NULL,	/* accumulator_data */
	     gtk_databox_marshal_VOID__VOID,
	     G_TYPE_NONE,
	     0);

   class->zoomed = NULL;
   class->selection_started = NULL;
   class->selection_changed = NULL;
   class->selection_finalized = NULL;
   class->selection_canceled = NULL;
}

static void
gtk_databox_init (GtkDatabox * box)
{
   box->priv = g_new0 (GtkDataboxPrivate, 1);
   box->priv->backing_pixmap = NULL;
   box->priv->scale_type_x = GTK_DATABOX_SCALE_LINEAR;
   box->priv->scale_type_y = GTK_DATABOX_SCALE_LINEAR;
   box->priv->translation_factor_x = 0;
   box->priv->translation_factor_y = 0;
   box->priv->enable_selection = TRUE;
   box->priv->enable_zoom = TRUE;
   box->priv->ruler_x = NULL;
   box->priv->ruler_y = NULL;
   box->priv->graphs = NULL;
   box->priv->select_gc = NULL;
   box->priv->zoom_limit = 0.01;
   box->priv->selection_active = FALSE;
   box->priv->selection_finalized = FALSE;

   gtk_databox_set_adjustment_x (box, NULL);
   gtk_databox_set_adjustment_y (box, NULL);
   gtk_databox_set_total_limits(box, -1., 1., 1., -1.);
}

/** 
 * gtk_databox_new
 *
 * Creates a new #GtkDatabox widget.
 *
 * Return value: The new #GtkDatabox widget.
 *
 */
GtkWidget *
gtk_databox_new (void)
{
   return g_object_new (GTK_TYPE_DATABOX, NULL);
}

static gint
gtk_databox_motion_notify (GtkWidget * widget, GdkEventMotion * event)
{
   GtkDatabox *box;
   GdkModifierType state;
   gint x;
   gint y;

   box = GTK_DATABOX (widget);

   if (event->is_hint)
   {
      gdk_window_get_pointer (widget->window, &x, &y, &state);
   }
   else
   {
      state = event->state;
      x = event->x;
      y = event->y;
   }

   if (state & GDK_BUTTON1_MASK
       && box->priv->enable_selection && !box->priv->selection_finalized)
   {
      GdkRectangle rect;
      gint width;
      gint height;

      gdk_drawable_get_size (widget->window, &width, &height);
      x = MAX (0, MIN (width - 1, x));
      y = MAX (0, MIN (height - 1, y));

      if (box->priv->selection_active)
      {
	 /* Clear current selection from backing_pixmap */
	 gtk_databox_draw_selection (box, NULL);
      }
      else
      {
	 box->priv->selection_active = TRUE;
	 box->priv->marked.x = x;
	 box->priv->marked.y = y;
	 box->priv->select.x = x;
	 box->priv->select.y = y;
	 gtk_databox_calculate_selection_values (box);
	 g_signal_emit (G_OBJECT (box),
			gtk_databox_signals[SELECTION_STARTED_SIGNAL], 0,
			&box->priv->selectionValues);
      }

      /* Determine the exposure rectangle (covering old selection and new) */
      rect.x = MIN (MIN (box->priv->marked.x, box->priv->select.x), x);
      rect.y = MIN (MIN (box->priv->marked.y, box->priv->select.y), y);
      rect.width = MAX (MAX (box->priv->marked.x, box->priv->select.x), x)
	 - rect.x + 1;
      rect.height = MAX (MAX (box->priv->marked.y, box->priv->select.y), y)
	 - rect.y + 1;

      box->priv->select.x = x;
      box->priv->select.y = y;

      /* Draw new selection */
      gtk_databox_draw_selection (box, &rect);

      gtk_databox_calculate_selection_values (box);
      g_signal_emit (G_OBJECT (box),
		     gtk_databox_signals[SELECTION_CHANGED_SIGNAL],
		     0, &box->priv->selectionValues);
   }

   return FALSE;
}

static void
gtk_databox_set_property (GObject * object,
			  guint property_id,
			  const GValue * value, GParamSpec * pspec)
{
   GtkDatabox *box = GTK_DATABOX (object);

   switch (property_id)
   {
   case ENABLE_SELECTION:
      gtk_databox_set_enable_selection (box, g_value_get_boolean (value));
      break;
   case ENABLE_ZOOM:
      gtk_databox_set_enable_zoom (box, g_value_get_boolean (value));
      break;
   case ADJUSTMENT_X:
      gtk_databox_set_adjustment_x (box, g_value_get_object (value));
      break;
   case ADJUSTMENT_Y:
      gtk_databox_set_adjustment_y (box, g_value_get_object (value));
      break;
   case RULER_X:
      gtk_databox_set_ruler_x (box, g_value_get_object (value));
      break;
   case RULER_Y:
      gtk_databox_set_ruler_y (box, g_value_get_object (value));
      break;
   case SCALE_TYPE_X:
      gtk_databox_set_scale_type_x (box, g_value_get_enum (value));
      break;
   case SCALE_TYPE_Y:
      gtk_databox_set_scale_type_y (box, g_value_get_enum (value));
      break;
   default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
   }
}

static void
gtk_databox_get_property (GObject * object,
			  guint property_id,
			  GValue * value, GParamSpec * pspec)
{
   GtkDatabox *box = GTK_DATABOX (object);

   switch (property_id)
   {
   case ENABLE_SELECTION:
      g_value_set_boolean (value, gtk_databox_get_enable_selection (box));
      break;
   case ENABLE_ZOOM:
      g_value_set_boolean (value, gtk_databox_get_enable_zoom (box));
      break;
   case ADJUSTMENT_X:
      g_value_set_object (value, G_OBJECT (gtk_databox_get_adjustment_x (box)));
      break;
   case ADJUSTMENT_Y:
      g_value_set_object (value, G_OBJECT (gtk_databox_get_adjustment_y (box)));
      break;
   case RULER_X:
      g_value_set_object (value, G_OBJECT (gtk_databox_get_ruler_x (box)));
      break;
   case RULER_Y:
      g_value_set_object (value, G_OBJECT (gtk_databox_get_ruler_y (box)));
      break;
   case SCALE_TYPE_X:
      g_value_set_enum (value, gtk_databox_get_scale_type_x (box));
      break;
   case SCALE_TYPE_Y:
      g_value_set_enum (value, gtk_databox_get_scale_type_y (box));
      break;
   default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
   }
}


static void
gtk_databox_realize (GtkWidget * widget)
{
   GtkDatabox *box;
   GdkWindowAttr attributes;
   gint attributes_mask;

   box = GTK_DATABOX (widget);
   GTK_WIDGET_SET_FLAGS (box, GTK_REALIZED);

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
                             GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK |
			     GDK_POINTER_MOTION_MASK |
			     GDK_POINTER_MOTION_HINT_MASK);

   attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;

   widget->window =
      gdk_window_new (gtk_widget_get_parent_window (widget), &attributes,
		      attributes_mask);
   gdk_window_set_user_data (widget->window, box);

   widget->style = gtk_style_attach (widget->style, widget->window);
   gtk_style_set_background (widget->style, widget->window, GTK_STATE_NORMAL);

   gtk_databox_create_backing_pixmap (box);
}

static void
gtk_databox_unrealize (GtkWidget * widget)
{
   GtkDatabox *box = GTK_DATABOX (widget);

   if (box->priv->backing_pixmap)
      g_object_unref (box->priv->backing_pixmap);
   if (box->priv->select_gc)
      gtk_gc_release (box->priv->select_gc);
   if (box->priv->adj_x)
      g_object_unref (box->priv->adj_x);
   if (box->priv->adj_y)
      g_object_unref (box->priv->adj_y);
   if (box->priv->graphs)
      g_list_free (box->priv->graphs);

   g_free (box->priv);

   if (GTK_WIDGET_CLASS (gtk_databox_parent_class)->unrealize)
      (*GTK_WIDGET_CLASS (gtk_databox_parent_class)->unrealize) (widget);
}


/** 
 * gtk_databox_set_enable_selection
 * @box: A #GtkDatabox widget
 * @enable: Whether selection via mouse is enabled or not.
 *
 * Setter function for the #GtkDatabox:enable-selection property.
 *
 */
void
gtk_databox_set_enable_selection (GtkDatabox * box, gboolean enable)
{
   g_return_if_fail (GTK_IS_DATABOX (box));

   box->priv->enable_selection = enable;
   if (box->priv->selection_active)
   {
      gtk_databox_selection_cancel (box);
   }

   g_object_notify (G_OBJECT (box), "enable-selection");
}

/** 
 * gtk_databox_set_enable_zoom
 * @box: A #GtkDatabox widget
 * @enable: Whether zoom via mouse is enabled or not.
 *
 * Setter function for the #GtkDatabox:enable-zoom property.
 *
 */
void
gtk_databox_set_enable_zoom (GtkDatabox * box, gboolean enable)
{
   g_return_if_fail (GTK_IS_DATABOX (box));

   box->priv->enable_zoom = enable;

   g_object_notify (G_OBJECT (box), "enable-zoom");
}

/** 
 * gtk_databox_set_adjustment_x
 * @box: A #GtkDatabox widget
 * @adj: A #GtkAdjustment object
 *
 * Setter function for the #GtkDatabox:adjustment-x property. Normally, it should not be 
 * required to use this function, see property documentation.
 *
 */
void
gtk_databox_set_adjustment_x (GtkDatabox * box, GtkAdjustment * adj)
{
   if (!adj)
      adj = GTK_ADJUSTMENT(gtk_adjustment_new (0, 0, 0, 0, 0, 0));

   g_return_if_fail (GTK_IS_DATABOX (box));
   g_return_if_fail (GTK_IS_ADJUSTMENT (adj));

   if (box->priv->adj_x)
   {
      /* @@@ Do we need to disconnect from the signals here? */
      g_object_unref (box->priv->adj_x);
      if (g_object_is_floating(G_OBJECT(box->priv->adj_x)))
         g_object_ref_sink (box->priv->adj_x);
   }

   box->priv->adj_x = adj;
   g_object_ref (box->priv->adj_x);

   /* We always scroll from 0 to 1.0 */
   box->priv->adj_x->lower = 0;
   box->priv->adj_x->value = gtk_databox_get_offset_x (box);
   box->priv->adj_x->upper = 1.0;
   box->priv->adj_x->page_size = gtk_databox_get_page_size_x (box);
   box->priv->adj_x->step_increment = box->priv->adj_x->page_size / 20;
   box->priv->adj_x->page_increment = box->priv->adj_x->page_size * 0.9;

   gtk_adjustment_changed (box->priv->adj_x);

   g_signal_connect_swapped (G_OBJECT (box->priv->adj_x), "value_changed",
			     G_CALLBACK
			     (gtk_databox_adjustment_value_changed), box);

   g_object_notify (G_OBJECT (box), "adjustment-x");
}

/** 
 * gtk_databox_set_adjustment_y
 * @box: A #GtkDatabox widget
 * @adj: A #GtkAdjustment object
 *
 * Setter function for the #GtkDatabox:adjustment-y property. Normally, it should not be 
 * required to use this function, see property documentation.
 *
 */
void
gtk_databox_set_adjustment_y (GtkDatabox * box, GtkAdjustment * adj)
{
   if (!adj)
      adj = GTK_ADJUSTMENT(gtk_adjustment_new (0, 0, 0, 0, 0, 0));

   g_return_if_fail (GTK_IS_DATABOX (box));
   g_return_if_fail (GTK_IS_ADJUSTMENT (adj));

   if (box->priv->adj_y)
   {
      /* @@@ Do we need to disconnect from the signals here? */
      g_object_unref (box->priv->adj_y);
      if (g_object_is_floating(G_OBJECT(box->priv->adj_y)))
         g_object_ref_sink (box->priv->adj_y);
   }

   box->priv->adj_y = adj;
   g_object_ref (box->priv->adj_y);

   /* We always scroll from 0 to 1.0 */
   box->priv->adj_y->lower = 0;
   box->priv->adj_y->value = gtk_databox_get_offset_y (box);
   box->priv->adj_y->upper = 1.0;
   box->priv->adj_y->page_size = gtk_databox_get_page_size_y (box);
   box->priv->adj_y->step_increment = box->priv->adj_y->page_size / 20;
   box->priv->adj_y->page_increment = box->priv->adj_y->page_size * 0.9;

   gtk_adjustment_changed (box->priv->adj_y);

   g_signal_connect_swapped (G_OBJECT (box->priv->adj_y), "value_changed",
			     G_CALLBACK
			     (gtk_databox_adjustment_value_changed), box);

   g_object_notify (G_OBJECT (box), "adjustment-y");
}

/** 
 * gtk_databox_set_ruler_x
 * @box: A #GtkDatabox widget
 * @ruler: A #GtkDataboxRuler object
 *
 * Setter function for the #GtkDatabox:ruler-x property.
 *
 */
void
gtk_databox_set_ruler_x (GtkDatabox * box, GtkDataboxRuler * ruler)
{
   g_return_if_fail (GTK_IS_DATABOX (box));
   g_return_if_fail (ruler == NULL || GTK_DATABOX_IS_RULER (ruler));
   g_return_if_fail (ruler == NULL || gtk_databox_ruler_get_orientation(ruler) == GTK_ORIENTATION_HORIZONTAL);

   if (box->priv->ruler_x)
   {
      /* @@@ Do we need to disconnect the signals here? */
      /* @@@ Do we need to call object_ref and object_unref here and for adjustments? */
   }

   box->priv->ruler_x = ruler;

   if (GTK_DATABOX_IS_RULER (ruler))
   {
      gtk_databox_ruler_set_scale_type (ruler,
					box->priv->scale_type_x);

      gtk_databox_ruler_update (box);
      g_signal_connect_swapped (box, "motion_notify_event",
				G_CALLBACK (GTK_WIDGET_GET_CLASS
					    (box->priv->ruler_x)->
					    motion_notify_event),
				G_OBJECT (box->priv->ruler_x));
   }

   g_object_notify (G_OBJECT (box), "ruler-x");
}

/** 
 * gtk_databox_set_ruler_y
 * @box: A #GtkDatabox widget
 * @ruler: An #GtkDataboxRuler object
 *
 * Setter function for the #GtkDatabox:ruler-y property.
 *
 */
void
gtk_databox_set_ruler_y (GtkDatabox * box, GtkDataboxRuler * ruler)
{
   g_return_if_fail (GTK_IS_DATABOX (box));
   g_return_if_fail (ruler == NULL || GTK_DATABOX_IS_RULER (ruler));
   g_return_if_fail (ruler == NULL || gtk_databox_ruler_get_orientation(ruler) == GTK_ORIENTATION_VERTICAL);

   if (box->priv->ruler_y)
   {
      /* @@@ Do we need to disconnect the signals here? */
      /* @@@ Do we need to call object_ref and object_unref here and for adjustments? */
   }

   box->priv->ruler_y = ruler;

   if (GTK_DATABOX_IS_RULER (ruler))
   {
      gtk_databox_ruler_set_scale_type (ruler,
					box->priv->scale_type_y);

      gtk_databox_ruler_update (box);
      g_signal_connect_swapped (box, "motion_notify_event",
				G_CALLBACK (GTK_WIDGET_GET_CLASS
					    (box->priv->ruler_y)->
					    motion_notify_event),
				G_OBJECT (box->priv->ruler_y));
   }

   g_object_notify (G_OBJECT (box), "ruler-y");
}

/** 
 * gtk_databox_set_scale_type_x
 * @box: A #GtkDatabox widget
 * @scale_type: An #GtkDataboxScaleType (linear or logarithmic)
 *
 * Setter function for the #GtkDatabox:scale-type-x property.
 *
 */
void
gtk_databox_set_scale_type_x (GtkDatabox * box,
			      GtkDataboxScaleType scale_type)
{
   box->priv->scale_type_x = scale_type;

   if (box->priv->ruler_x)
      gtk_databox_ruler_set_scale_type (box->priv->ruler_x, scale_type);

   g_object_notify (G_OBJECT (box), "scale-type-x");
}

/** 
 * gtk_databox_set_scale_type_y
 * @box: A #GtkDatabox widget
 * @scale_type: An #GtkDataboxScaleType (linear or logarithmic)
 *
 * Setter function for the #GtkDatabox:scale-type-y property.
 *
 */
void
gtk_databox_set_scale_type_y (GtkDatabox * box,
			      GtkDataboxScaleType scale_type)
{
   box->priv->scale_type_y = scale_type;
   
   if (box->priv->ruler_y)
      gtk_databox_ruler_set_scale_type (box->priv->ruler_y, scale_type);

   g_object_notify (G_OBJECT (box), "scale-type-y");
}

/** 
 * gtk_databox_get_enable_selection
 * @box: A #GtkDatabox widget.
 *
 * Getter function for the #GtkDatabox:enable-selection property.
 *
 * Return value: The #GtkDatabox:enable-selection property value.
 *
 */
gboolean
gtk_databox_get_enable_selection (GtkDatabox * box)
{
   g_return_val_if_fail (GTK_IS_DATABOX (box), FALSE);

   return box->priv->enable_selection;
}

/** 
 * gtk_databox_get_enable_zoom
 * @box: A #GtkDatabox widget.
 *
 * Getter function for the #GtkDatabox:enable-zoom property.
 *
 * Return value: The #GtkDatabox:enable-zoom property value.
 *
 */
gboolean
gtk_databox_get_enable_zoom (GtkDatabox * box)
{
   g_return_val_if_fail (GTK_IS_DATABOX (box), FALSE);

   return box->priv->enable_zoom;
}

/** 
 * gtk_databox_get_adjustment_x
 * @box: A #GtkDatabox widget.
 *
 * Getter function for the #GtkDatabox:adjustment-x property.
 *
 * Return value: The #GtkDatabox:adjustment-x property value.
 *
 */
GtkAdjustment *
gtk_databox_get_adjustment_x (GtkDatabox * box)
{
   g_return_val_if_fail (GTK_IS_DATABOX (box), NULL);

   return box->priv->adj_x;
}

/** 
 * gtk_databox_get_adjustment_y
 * @box: A #GtkDatabox widget.
 *
 * Getter function for the #GtkDatabox:adjustment-y property.
 *
 * Return value: The #GtkDatabox:adjustment-y property value.
 *
 */
GtkAdjustment *
gtk_databox_get_adjustment_y (GtkDatabox * box)
{
   g_return_val_if_fail (GTK_IS_DATABOX (box), NULL);

   return box->priv->adj_y;
}

/** 
 * gtk_databox_get_ruler_x
 * @box: A #GtkDatabox widget.
 *
 * Getter function for the #GtkDatabox:ruler-x property.
 *
 * Return value: The #GtkDatabox:ruler-x property value.
 *
 */
GtkDataboxRuler *
gtk_databox_get_ruler_x (GtkDatabox * box)
{
   g_return_val_if_fail (GTK_IS_DATABOX (box), NULL);

   return box->priv->ruler_x;
}

/** 
 * gtk_databox_get_ruler_y
 * @box: A #GtkDatabox widget.
 *
 * Getter function for the #GtkDatabox:ruler-y property.
 *
 * Return value: The #GtkDatabox:ruler-y property value.
 *
 */
GtkDataboxRuler *
gtk_databox_get_ruler_y (GtkDatabox * box)
{
   g_return_val_if_fail (GTK_IS_DATABOX (box), NULL);

   return box->priv->ruler_y;
}

/** 
 * gtk_databox_get_scale_type_x
 * @box: A #GtkDatabox widget.
 *
 * Getter function for the #GtkDatabox:scale-type-x property.
 *
 * Return value: The #GtkDatabox:scale-type-x property value.
 *
 */
GtkDataboxScaleType
gtk_databox_get_scale_type_x (GtkDatabox * box)
{
   return box->priv->scale_type_x;
}

/** 
 * gtk_databox_get_scale_type_y
 * @box: A #GtkDatabox widget.
 *
 * Getter function for the #GtkDatabox:scale-type-y property.
 *
 * Return value: The #GtkDatabox:scale-type-y property value.
 *
 */
GtkDataboxScaleType
gtk_databox_get_scale_type_y (GtkDatabox * box)
{
   return box->priv->scale_type_y;
}

static void
gtk_databox_calculate_translation_factors (GtkDatabox * box)
{
   /* @@@ Check for all external functions, if type checks are implemented! */
   GtkWidget *widget = GTK_WIDGET(box);

   if (box->priv->scale_type_x == GTK_DATABOX_SCALE_LINEAR)
      box->priv->translation_factor_x =
	 widget->allocation.width / (box->priv->visible_right -
				box->priv->visible_left);
   else if (box->priv->scale_type_x == GTK_DATABOX_SCALE_LOG2)
      box->priv->translation_factor_x =
	 widget->allocation.width / log2 (box->priv->visible_right /
				    box->priv->visible_left);
   else
      box->priv->translation_factor_x =
	 widget->allocation.width / log10 (box->priv->visible_right /
				    box->priv->visible_left);

   if (box->priv->scale_type_y == GTK_DATABOX_SCALE_LINEAR)
      box->priv->translation_factor_y =
	 widget->allocation.height / (box->priv->visible_bottom -
				 box->priv->visible_top);
   else if (box->priv->scale_type_y == GTK_DATABOX_SCALE_LOG2)
      box->priv->translation_factor_y =
	 widget->allocation.height / log2 (box->priv->visible_bottom /
				     box->priv->visible_top);
   else
      box->priv->translation_factor_y =
	 widget->allocation.height / log10 (box->priv->visible_bottom /
				     box->priv->visible_top);
}

static void
gtk_databox_create_backing_pixmap(GtkDatabox * box)
{
   GtkWidget *widget;
   gint width;
   gint height;

   widget = GTK_WIDGET (box);

   if (box->priv->backing_pixmap)
   {
      gdk_drawable_get_size (box->priv->backing_pixmap, &width, &height);
      if ((width == widget->allocation.width) &&
	  (height == widget->allocation.height))
	 return;

      g_object_unref (box->priv->backing_pixmap);
   }

   box->priv->backing_pixmap = gdk_pixmap_new (widget->window,
					  widget->allocation.width,
					  widget->allocation.height, -1);
}

/** 
 * gtk_databox_get_backing_pixmap:
 * @box: A #GtkDatabox widget
 *
 * This function returns the pixmap which is used by @box and its #GtkDataboxGraph objects 
 * for drawing operations before copying the result to the screen.
 *
 * The function is typically called by the #GtkDataboxGraph objects.
 *
 * Return value: Backing pixmap
 */
GdkPixmap *
gtk_databox_get_backing_pixmap(GtkDatabox * box)
{
   g_return_val_if_fail (GTK_IS_DATABOX (box), NULL);

   return box->priv->backing_pixmap;
}

static void
gtk_databox_size_allocate (GtkWidget * widget, GtkAllocation * allocation)
{
   GtkDatabox *box = GTK_DATABOX (widget);

   widget->allocation = *allocation;

   if (GTK_WIDGET_REALIZED (widget))
   {
      gdk_window_move_resize (widget->window,
			      allocation->x, allocation->y,
			      allocation->width, allocation->height);

      gtk_databox_create_backing_pixmap (box);
   }

   if (box->priv->selection_active)
   {
      gtk_databox_selection_cancel (box);
   }

   gtk_databox_calculate_translation_factors (box);
}

static gint
gtk_databox_expose (GtkWidget * widget, GdkEventExpose * event)
{
   GtkDatabox *box = GTK_DATABOX (widget);
   GList *list;

   gdk_draw_rectangle (box->priv->backing_pixmap, 
                       widget->style->bg_gc[0],
		       TRUE, 0, 0,
		       widget->allocation.width,
		       widget->allocation.height);

   list = g_list_last (box->priv->graphs);

   while (list)
   {
      if (list->data)
      {
	 gtk_databox_graph_draw (GTK_DATABOX_GRAPH (list->data), box);
      }
      else
      {
	 /* Do nothing if data == NULL */
      }
      list = g_list_previous (list);
   }

   if (box->priv->selection_active)
   {
      gtk_databox_draw_selection (box, NULL);
   }

   gdk_draw_drawable (widget->window,
		      widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
		      box->priv->backing_pixmap, 
                      event->area.x, event->area.y,
		      event->area.x, event->area.y, 
                      event->area.width,
		      event->area.height);

   return FALSE;
}

static void
gtk_databox_calculate_selection_values (GtkDatabox * box)
{
   box->priv->selectionValues.x1 =
      gtk_databox_pixel_to_value_x (box, box->priv->marked.x);
   box->priv->selectionValues.x2 =
      gtk_databox_pixel_to_value_x (box, box->priv->select.x);
   box->priv->selectionValues.y1 =
      gtk_databox_pixel_to_value_y (box, box->priv->marked.y);
   box->priv->selectionValues.y2 =
      gtk_databox_pixel_to_value_y (box, box->priv->select.y);
}

static gint
gtk_databox_button_press (GtkWidget * widget, GdkEventButton * event)
{
   GtkDatabox *box = GTK_DATABOX (widget);

   if (event->type != GDK_BUTTON_PRESS)
      return FALSE;

   box->priv->selection_finalized = FALSE;

   if ((event->button == 1 || event->button == 2))
   {
      if (box->priv->selection_active)
      {
	 if (event->x > MIN (box->priv->marked.x, box->priv->select.x)
	     && event->x < MAX (box->priv->marked.x, box->priv->select.x)
	     && event->y > MIN (box->priv->marked.y, box->priv->select.y)
	     && event->y < MAX (box->priv->marked.y, box->priv->select.y))
	 {
	    gtk_databox_zoom_to_selection (box);
	 }
	 else
	 {
	    gtk_databox_selection_cancel (box);
	 }
	 box->priv->marked.x = box->priv->select.x = event->x;
	 box->priv->marked.y = box->priv->select.y = event->y;
	 gtk_databox_calculate_selection_values (box);
      }
   }

   if (event->button == 3)
   {
      if (event->state & GDK_SHIFT_MASK)
      {
	 gtk_databox_zoom_home (box);
      }
      else
      {
	 gtk_databox_zoom_out (box);
      }
   }

   return FALSE;
}

static gint
gtk_databox_button_release (GtkWidget * widget, GdkEventButton * event)
{
   GtkDatabox *box = GTK_DATABOX (widget);

   if (event->type != GDK_BUTTON_RELEASE)
      return FALSE;

   if (box->priv->selection_active)
   {
      box->priv->selection_finalized = TRUE;

      g_signal_emit (G_OBJECT (box),
		     gtk_databox_signals[SELECTION_FINALIZED_SIGNAL],
		     0, &box->priv->selectionValues);
   }

   return FALSE;
}

static void
gtk_databox_selection_cancel (GtkDatabox * box)
{
   GdkRectangle rect;

   /* There is no active selection after cancellation */
   box->priv->selection_active = FALSE;

   /* Only active selections can be stopped */
   box->priv->selection_finalized = FALSE;

   /* Remove selection box */
   rect.x = MIN (box->priv->marked.x, box->priv->select.x);
   rect.y = MIN (box->priv->marked.y, box->priv->select.y);
   rect.width = ABS (box->priv->marked.x - box->priv->select.x) + 1;
   rect.height = ABS (box->priv->marked.y - box->priv->select.y) + 1;

   gtk_databox_draw_selection (box, &rect);

   /* Let everyone know that the selection has been canceled */
   g_signal_emit (G_OBJECT (box),
		  gtk_databox_signals[SELECTION_CANCELED_SIGNAL], 0);
}


static void
gtk_databox_zoomed (GtkDatabox * box)
{
   box->priv->selection_active = FALSE;
   box->priv->selection_finalized = FALSE;

   gtk_adjustment_changed (box->priv->adj_x);
   gtk_adjustment_changed (box->priv->adj_y);

   gtk_widget_queue_draw (GTK_WIDGET(box));

   g_signal_emit (G_OBJECT (box),
		  gtk_databox_signals[ZOOMED_SIGNAL], 0, NULL);
}

/** 
 * gtk_databox_zoom_to_selection:
 * @box: A #GtkDatabox widget
 *
 * This is equivalent to left-clicking into the selected area.
 *
 * This function works, if the attribute #enable-zoom is set to #TRUE. Calling the function 
 * then zooms to the area selected with the mouse. 
 *
 * Side effect: The @box emits #GtkDatabox::zoomed.
 */
void
gtk_databox_zoom_to_selection (GtkDatabox * box)
{
   GtkWidget *widget;

   g_return_if_fail(GTK_IS_DATABOX(box));

   widget = GTK_WIDGET (box);

   if (!box->priv->enable_zoom)
   {
      gtk_databox_selection_cancel (box);
      return;
   }

   box->priv->adj_x->value += (gfloat) (MIN (box->priv->marked.x,
					    box->priv->select.x))
      * box->priv->adj_x->page_size
      / widget->allocation.width;
   box->priv->adj_y->value += (gfloat) (MIN (box->priv->marked.y,
					    box->priv->select.y))
      * box->priv->adj_y->page_size
      / widget->allocation.height;

   box->priv->adj_x->page_size *=
      (gfloat) (ABS (box->priv->marked.x - box->priv->select.x) + 1)
      / widget->allocation.width;

   box->priv->adj_y->page_size *=
      (gfloat) (ABS (box->priv->marked.y - box->priv->select.y) + 1)
      / widget->allocation.height;

   /* If we zoom too far into the data, we will get funny results, because
    * of overflow effects. Therefore zooming is limited to box->zoom_limit.
    */
   if (box->priv->adj_x->page_size < box->priv->zoom_limit)
   {
      box->priv->adj_x->value = (gfloat) MAX (0,
					     box->priv->adj_x->value
					     - (box->priv->zoom_limit -
						box->priv->adj_x->page_size) /
					     2.0);
      box->priv->adj_x->page_size = box->priv->zoom_limit;
   }

   if (box->priv->adj_y->page_size < box->priv->zoom_limit)
   {
      box->priv->adj_y->value = (gfloat) MAX (0,
					     box->priv->adj_y->value
					     - (box->priv->zoom_limit -
						box->priv->adj_y->page_size) /
					     2.0);
      box->priv->adj_y->page_size = box->priv->zoom_limit;
   }

   gtk_databox_calculate_visible_limits(box);
   gtk_databox_zoomed (box);
}

/** 
 * gtk_databox_zoom_out:
 * @box: A #GtkDatabox widget
 *
 * This is equivalent to right-clicking into the @box.
 *
 * This function works, if the attribute #enable-zoom is set to #TRUE. Calling the function 
 * then zooms out by a factor of 2 in both dimensions (the maximum is defined by the total 
 * limits, see gtk_databox_set_total_limits()).
 * 
 * Side effect: The @box emits #GtkDatabox::zoomed.
 */
void
gtk_databox_zoom_out (GtkDatabox * box)
{
   if (!box->priv->enable_zoom)
   {
      return;
   }

   box->priv->adj_x->page_size = MIN (1.0, box->priv->adj_x->page_size * 2);
   box->priv->adj_y->page_size = MIN (1.0, box->priv->adj_y->page_size * 2);
   box->priv->adj_x->value =
      (box->priv->adj_x->page_size == 1.0)
      ? 0
      : MAX (0, MIN (box->priv->adj_x->value - box->priv->adj_x->page_size / 4,
		     1.0 - box->priv->adj_x->page_size));
   box->priv->adj_y->value =
      (box->priv->adj_y->page_size == 1.0)
      ? 0
      : MAX (0, MIN (box->priv->adj_y->value - box->priv->adj_y->page_size / 4,
		     1.0 - box->priv->adj_y->page_size));

   gtk_databox_calculate_visible_limits(box);
   gtk_databox_zoomed (box);
}

/** 
 * gtk_databox_zoom_home:
 * @box: A #GtkDatabox widget
 *
 * This is equivalent to shift right-clicking into the @box.
 *
 * This function works, if the attribute #enable-zoom is set to #TRUE. It is equivalent to 
 * calling the gtk_databox_set_visible_limits() with the total limits.
 * 
 */
void
gtk_databox_zoom_home (GtkDatabox * box)
{
   if (!box->priv->enable_zoom)
   {
      return;
   }

   gtk_databox_set_visible_limits (box, 
         box->priv->total_left, box->priv->total_right, 
         box->priv->total_top, box->priv->total_bottom);
}

static void
gtk_databox_draw_selection (GtkDatabox * box, GdkRectangle * pixmapCopyRect)
{
   GtkWidget *widget = GTK_WIDGET (box);

   if (!box->priv->select_gc)
   {
      GdkGCValues values;

      values.foreground = widget->style->white;
      values.function = GDK_XOR;
      box->priv->select_gc = gtk_gc_get (widget->style->depth,
					 widget->style->colormap,
					 &values,
					 GDK_GC_FUNCTION | GDK_GC_FOREGROUND);
   }


   /* Draw a selection box in XOR mode onto the buffer backing_pixmap */
   gdk_draw_rectangle (box->priv->backing_pixmap,
                       box->priv->select_gc,
		       FALSE,
		       MIN (box->priv->marked.x, box->priv->select.x),
		       MIN (box->priv->marked.y, box->priv->select.y),
		       ABS (box->priv->marked.x - box->priv->select.x),
		       ABS (box->priv->marked.y - box->priv->select.y));

   /* Copy a part of the backing_pixmap to the screen */
   if (pixmapCopyRect)
      gdk_draw_drawable (widget->window,
			 widget->style->fg_gc[GTK_WIDGET_STATE (box)],
			 box->priv->backing_pixmap,
			 pixmapCopyRect->x,
			 pixmapCopyRect->y,
			 pixmapCopyRect->x,
			 pixmapCopyRect->y,
			 pixmapCopyRect->width, pixmapCopyRect->height);
}

static void
gtk_databox_adjustment_value_changed (GtkDatabox * box)
{
   gtk_databox_calculate_visible_limits (box);

   gtk_widget_queue_draw (GTK_WIDGET(box));
}

static void
gtk_databox_ruler_update (GtkDatabox * box)
{
   if (box->priv->ruler_x)
   {
      gtk_databox_ruler_set_range (
            GTK_DATABOX_RULER (box->priv->ruler_x),
               box->priv->visible_left,
               box->priv->visible_right,
               0.5 * (box->priv->visible_left + box->priv->visible_right));
   }

   if (box->priv->ruler_y)
   {
      gtk_databox_ruler_set_range (
            GTK_DATABOX_RULER (box->priv->ruler_y),
	      box->priv->visible_top,
	      box->priv->visible_bottom,
	      0.5 * (box->priv->visible_top + box->priv->visible_bottom));
   }
}

/** 
 * gtk_databox_auto_rescale:
 * @box: A #GtkDatabox widget
 * @border: Relative border width (e.g. 0.1 means that the border on each side is 10% of the data area).
 *
 * This function is similar to gtk_databox_set_total_limits(). It sets the total limits 
 * to match the data extrema (see gtk_databox_calculate_extrema()). If you do not like data pixels exactly at the 
 * widget's border, you can add modify the limits using the border parameter: The limits are extended by  
 * @border*(max-min) if max!=min. If max==min, they are extended by @border*max (otherwise the data could not be 
 * scaled to the pixel realm).
 *
 * After calling this function, x values grow from left to right, y values grow from bottom to top.
 *
 * Return value: 0 on success,
 *          -1 if @box is no GtkDatabox widget,
 *          -2 if no datasets are available
 */
gint
gtk_databox_auto_rescale (GtkDatabox * box, gfloat border)
{
   gfloat min_x;
   gfloat max_x;
   gfloat min_y;
   gfloat max_y;
   gint extrema_success = gtk_databox_calculate_extrema (box, &min_x, &max_x,
                                              &min_y, &max_y);
   if (extrema_success)
      return extrema_success;
   else
   {
      gfloat width = max_x - min_x;
      gfloat height = max_y - min_y;

      if (width == 0) width = max_x;
      if (height == 0) height = max_y;

      min_x -= border * width;
      max_x += border * width;
      min_y -= border * height;
      max_y += border * height;
   }

   gtk_databox_set_total_limits (GTK_DATABOX (box), min_x, max_x, max_y,
				 min_y);

   return 0;
}


/** 
 * gtk_databox_calculate_extrema:
 * @box: A #GtkDatabox widget
 * @min_x: Will be filled with the lowest x value of all datasets
 * @max_x: Will be filled with the highest x value of all datasets
 * @min_y: Will be filled with the lowest y value of all datasets
 * @max_y: Will be filled with the highest y value of all datasets
 *
 * Determines the minimum and maximum x and y values of all 
 * #GtkDataboxGraph objects which have been added to the #GtkDatabox widget via gtk_databox_graph_add().
 *
 * Return value: 0 on success,
 *          -1 if @box is no GtkDatabox widget,
 *          -2 if no datasets are available
 */
gint
gtk_databox_calculate_extrema (GtkDatabox * box,
			       gfloat * min_x, gfloat * max_x, gfloat * min_y,
			       gfloat * max_y)
{
   GList *list;
   gint return_val = -2;
   gboolean first = TRUE;

   g_return_val_if_fail (GTK_IS_DATABOX (box), -1);

   list = g_list_last (box->priv->graphs);

   while (list)
   {
      gfloat graph_min_x;
      gfloat graph_max_x;
      gfloat graph_min_y;
      gfloat graph_max_y;
      gint value = -1;

      if (list->data)
      {
	 value =
	    gtk_databox_graph_calculate_extrema (GTK_DATABOX_GRAPH
						 (list->data), &graph_min_x,
						 &graph_max_x, &graph_min_y,
						 &graph_max_y);
      }
      else
      {
	 /* Do nothing if data == NULL */
      }

      if (value >= 0)
      {
	 return_val = 0;

	 if (first)
	 {
	    /* The min and max values need to be initialized with the 
	     * first valid values from the graph
	     */
	    *min_x = graph_min_x;
	    *max_x = graph_max_x;
	    *min_y = graph_min_y;
	    *max_y = graph_max_y;

	    first = FALSE;
	 }
	 else
	 {
	    *min_x = MIN (*min_x, graph_min_x);
	    *min_y = MIN (*min_y, graph_min_y);
	    *max_x = MAX (*max_x, graph_max_x);
	    *max_y = MAX (*max_y, graph_max_y);
	 }
      }

      list = g_list_previous (list);
   }

   return return_val;
}

static gfloat
gtk_databox_get_offset_x (GtkDatabox* box)
{
   if (box->priv->scale_type_x == GTK_DATABOX_SCALE_LINEAR)
      return (box->priv->visible_left - box->priv->total_left)
	 / (box->priv->total_right - box->priv->total_left);
   else if (box->priv->scale_type_x == GTK_DATABOX_SCALE_LOG2)
      return log2 (box->priv->visible_left / box->priv->total_left)
	 / log2 (box->priv->total_right / box->priv->total_left);
   else
      return log10 (box->priv->visible_left / box->priv->total_left)
	 / log10 (box->priv->total_right / box->priv->total_left);
}

static gfloat
gtk_databox_get_page_size_x (GtkDatabox* box)
{
   if (box->priv->scale_type_x == GTK_DATABOX_SCALE_LINEAR)
      return (box->priv->visible_left - box->priv->visible_right)
	 / (box->priv->total_left - box->priv->total_right);
   else if (box->priv->scale_type_x == GTK_DATABOX_SCALE_LOG2)
      return log2 (box->priv->visible_left / box->priv->visible_right)
	 / log2 (box->priv->total_left / box->priv->total_right);
   else
      return log10 (box->priv->visible_left / box->priv->visible_right)
	 / log10 (box->priv->total_left / box->priv->total_right);
}

static gfloat
gtk_databox_get_offset_y (GtkDatabox* box)
{
   if (box->priv->scale_type_y == GTK_DATABOX_SCALE_LINEAR)
      return (box->priv->visible_top - box->priv->total_top)
	 / (box->priv->total_bottom - box->priv->total_top);
   else if (box->priv->scale_type_y == GTK_DATABOX_SCALE_LOG2)
      return log2 (box->priv->visible_top / box->priv->total_top)
	 / log2 (box->priv->total_bottom / box->priv->total_top);
   else
      return log10 (box->priv->visible_top / box->priv->total_top)
	 / log10 (box->priv->total_bottom / box->priv->total_top);
}

static gfloat
gtk_databox_get_page_size_y (GtkDatabox* box)
{
   if (box->priv->scale_type_y == GTK_DATABOX_SCALE_LINEAR)
      return (box->priv->visible_top - box->priv->visible_bottom)
	 / (box->priv->total_top - box->priv->total_bottom);
   else if (box->priv->scale_type_y == GTK_DATABOX_SCALE_LOG2)
      return log2 (box->priv->visible_top / box->priv->visible_bottom)
	 / log2 (box->priv->total_top / box->priv->total_bottom);
   else
      return log10 (box->priv->visible_top / box->priv->visible_bottom)
	 / log10 (box->priv->total_top / box->priv->total_bottom);
}

/** 
 * gtk_databox_set_total_limits:
 * @box: A #GtkDatabox widget
 * @left: Left total limit
 * @right: Right total limit
 * @top: Top total limit
 * @bottom: Bottom total limit
 *
 * This function is used to set the limits of the total 
 * display area of @box.
 * This function can be used to invert the orientation of the displayed graphs, 
 * e.g. @top=-1000 and  @bottom=0.
 *
 * Side effect: The @box also internally calls gtk_databox_set_visible_limits() with the same values.
 *
 */
void
gtk_databox_set_total_limits (GtkDatabox * box,
			      gfloat left, gfloat right,
			      gfloat top, gfloat bottom)
{
   g_return_if_fail (GTK_IS_DATABOX (box));
   g_return_if_fail (left != right);
   g_return_if_fail (top != bottom);

   box->priv->total_left = left;
   box->priv->total_right = right;
   box->priv->total_top = top;
   box->priv->total_bottom = bottom;

   gtk_databox_set_visible_limits(box, left, right, top, bottom);
}

/** 
 * gtk_databox_set_visible_limits:
 * @box: A #GtkDatabox widget
 * @left: Left visible limit
 * @right: Right visible limit
 * @top: Top visible limit
 * @bottom: Bottom visible limit
 *
 * This function is used to set the limits of the visible 
 * display area of @box. The visible display area can be section of the total 
 * area, i.e. the @box zooms in, showing only a part of the complete picture.
 *
 * The orientation of the values have to be the same as in gtk_databox_set_total_limits() and 
 * the visible limits have to be within the total limits. The 
 * values will not be used otherwise.
 *
 * Side effect: The @box emits #GtkDatabox::zoomed.
 *
 */
void
gtk_databox_set_visible_limits (GtkDatabox * box,
				gfloat left, gfloat right,
				gfloat top, gfloat bottom)
{
   gboolean visible_inside_total = FALSE;

   g_return_if_fail (GTK_IS_DATABOX (box));

   visible_inside_total =
      ((box->priv->total_left <= left && left < right
	&& right <= box->priv->total_right)
       || (box->priv->total_left >= left && left > right
	   && right >= box->priv->total_right))
      &&
      ((box->priv->total_bottom <= bottom && bottom < top
	&& top <= box->priv->total_top)
       || (box->priv->total_bottom >= bottom && bottom > top
	   && top >= box->priv->total_top));

   g_return_if_fail (visible_inside_total);

   box->priv->visible_left = left;
   box->priv->visible_right = right;
   box->priv->visible_top = top;
   box->priv->visible_bottom = bottom;

   gtk_databox_calculate_translation_factors (box);

   box->priv->adj_x->value = gtk_databox_get_offset_x (box);
   box->priv->adj_x->page_size = gtk_databox_get_page_size_x (box);
   box->priv->adj_y->value = gtk_databox_get_offset_y (box);
   box->priv->adj_y->page_size = gtk_databox_get_page_size_y (box);

   /* Update rulers */
   gtk_databox_ruler_update(box);

   gtk_databox_calculate_translation_factors (box);

   gtk_databox_zoomed (box);
}

/** 
 * gtk_databox_calculate_visible_limits:
 * @box: A #GtkDatabox widget
 *
 * Calculates the visible limits based on the adjustment values and page sizes
 * and calls gtk_databox_set_visible_limits();
 */
static void
gtk_databox_calculate_visible_limits (GtkDatabox * box)
{
   if (!GTK_WIDGET_VISIBLE (box))
      return;

   if (box->priv->scale_type_x == GTK_DATABOX_SCALE_LINEAR)
   {
      box->priv->visible_left =
           box->priv->total_left 
           + (box->priv->total_right - box->priv->total_left) 
           * box->priv->adj_x->value;
      box->priv->visible_right =
           box->priv->total_left 
           + (box->priv->total_right - box->priv->total_left) 
           * (box->priv->adj_x->value + box->priv->adj_x->page_size);
   }
   else
   {
      box->priv->visible_left =
            box->priv->total_left 
            * pow (box->priv->total_right / box->priv->total_left, 
                  box->priv->adj_x->value);
      box->priv->visible_right =
            box->priv->total_left 
            * pow (box->priv->total_right / box->priv->total_left, 
                   box->priv->adj_x->value + box->priv->adj_x->page_size);
   }

   if (box->priv->scale_type_y == GTK_DATABOX_SCALE_LINEAR)
   {
      box->priv->visible_top =
           box->priv->total_top 
           + (box->priv->total_bottom - box->priv->total_top) 
           * box->priv->adj_y->value;
      box->priv->visible_bottom =
           box->priv->total_top 
           + (box->priv->total_bottom - box->priv->total_top) 
           * (box->priv->adj_y->value + box->priv->adj_y->page_size);
   }
   else
   {
      box->priv->visible_top =
            box->priv->total_top 
            * pow (box->priv->total_bottom / box->priv->total_top, 
                   box->priv->adj_y->value),
      box->priv->visible_bottom =
            box->priv->total_top 
            * pow (box->priv->total_bottom / box->priv->total_top, 
                  box->priv->adj_y->value + box->priv->adj_y->page_size);
   }

   /* Adjustments are the basis for the calculations in this function
    * so they do not need to be updated 
    */

   /* Update rulers */
   gtk_databox_ruler_update(box);

   gtk_databox_calculate_translation_factors (box);
}

/** 
 * gtk_databox_get_total_limits:
 * @box: A #GtkDatabox widget
 * @left: Space for total left value or #NULL
 * @right: Space for total right value or #NULL
 * @top: Space for total top value or #NULL
 * @bottom: Space for total bottom value or #NULL
 *
 * Gives the total limits (as set by gtk_databox_auto_rescale() or gtk_databox_set_total_limits()).
 */
void
gtk_databox_get_total_limits (GtkDatabox * box,
			      gfloat * left, gfloat * right,
			      gfloat * top, gfloat * bottom)
{
   g_return_if_fail (GTK_IS_DATABOX (box));

   if (left)
      *left = box->priv->total_left;
   if (right)
      *right = box->priv->total_right;
   if (top)
      *top = box->priv->total_top;
   if (bottom)
      *bottom = box->priv->total_bottom;
}

/** 
 * gtk_databox_get_visible_limits:
 * @box: A #GtkDatabox widget
 * @left: Space for visible left value or #NULL
 * @right: Space for visible right value or #NULL
 * @top: Space for visible top value or #NULL
 * @bottom: Space for visible bottom value or #NULL
 *
 * Gives the current visible limits. These differ from those given by gtk_databox_get_total_limits() if 
 * you zoomed into the data for instance by gtk_databox_zoom_to_selection() or gtk_databox_set_visible_limits() (these values
 * can be changed by scrolling, of course).
 */
void
gtk_databox_get_visible_limits (GtkDatabox * box,
				gfloat * left, gfloat * right,
				gfloat * top, gfloat * bottom)
{
   g_return_if_fail (GTK_IS_DATABOX (box));

   if (left)
      *left = box->priv->visible_left;
   if (right)
      *right = box->priv->visible_right;
   if (top)
      *top = box->priv->visible_top;
   if (bottom)
      *bottom = box->priv->visible_bottom;
}


/** 
 * gtk_databox_graph_add:
 * @box: A #GtkDatabox widget
 * @graph: A graph, e.g. a #GtkDataboxPoints or a #GtkDataboxGrid object
 *
 * Adds the @graph to the @box. The next time the @box is re-drawn, the graph will be shown.
 *
 * It might be becessary to modify the total_limits in order for the graph to be displayed properly (see gtk_databox_set_total_limits()).
 *
 * Return value: 0 on success, -1 otherwise
 */
gint
gtk_databox_graph_add (GtkDatabox * box, GtkDataboxGraph * graph)
{
   g_return_val_if_fail (GTK_IS_DATABOX (box), -1);
   g_return_val_if_fail (GTK_DATABOX_IS_GRAPH (graph), -1);

   box->priv->graphs = g_list_append (box->priv->graphs, graph);

   return (box->priv->graphs == NULL) ? -1 : 0;
}

/** 
 * gtk_databox_graph_remove:
 * @box: A #GtkDatabox widget
 * @graph: A graph, e.g. a #GtkDataboxPoints or a #GtkDataboxGrid object
 *
 * Removes the @graph from the @box once. The next time the @box is re-drawn, the graph will not be shown (unless it was added more 
 * than once).
 *
 * Return value: 0 on success, -1 otherwise
 */
gint
gtk_databox_graph_remove (GtkDatabox * box, GtkDataboxGraph * graph)
{
   GList *list;

   g_return_val_if_fail (GTK_IS_DATABOX (box), -1);
   g_return_val_if_fail (GTK_DATABOX_IS_GRAPH (graph), -1);

   list = g_list_find (box->priv->graphs, graph);
   g_return_val_if_fail (list, -1);

   box->priv->graphs = g_list_delete_link (box->priv->graphs, list);

   return 0;
}

/** 
 * gtk_databox_graph_remove_all:
 * @box: A #GtkDatabox widget
 *
 * Removes all graphs from the @box. The next time the @box is re-drawn, no graphs will be shown.
 *
 * Return value: 0 on success, -1 otherwise
 */
gint
gtk_databox_graph_remove_all (GtkDatabox * box)
{
   g_return_val_if_fail (GTK_IS_DATABOX (box), -1);

   g_list_free (box->priv->graphs);

   box->priv->graphs = 0;

   return 0;
}

/** 
 * gtk_databox_values_to_pixels:
 * @box: A #GtkDatabox widget
 * @len: Number of values/pixels
 * @values_x: Array of X values (length >= len)
 * @values_y: Array of Y values (length >= len)
 * @pixels: Array of pixel coordinates (length >= len)
 *
 * Calculates the pixel equivalents of the given X/Y values and stores them in the @pixels array.
 */
void
gtk_databox_values_to_pixels (GtkDatabox * box, guint len,
			      const gfloat * values_x,
			      const gfloat * values_y,
			      GdkPoint * pixels)
{
   guint i;

   for (i = 0; i < len; ++i, ++values_x, ++values_y, ++pixels)
   {
      if (box->priv->scale_type_x == GTK_DATABOX_SCALE_LINEAR)
	 pixels->x =
	    (*values_x -
	     box->priv->visible_left) * box->priv->translation_factor_x;
      else if (box->priv->scale_type_x == GTK_DATABOX_SCALE_LOG2)
	 pixels->x =
	    log2 (*values_x / box->priv->visible_left) *
	    box->priv->translation_factor_x;
      else
	 pixels->x =
	    log10 (*values_x / box->priv->visible_left) *
	    box->priv->translation_factor_x;

      if (box->priv->scale_type_y == GTK_DATABOX_SCALE_LINEAR)
	 pixels->y =
	    (*values_y -
	     box->priv->visible_top) * box->priv->translation_factor_y;
      else if (box->priv->scale_type_y == GTK_DATABOX_SCALE_LOG2)
	 pixels->y =
	    log2 (*values_y / box->priv->visible_top) *
	    box->priv->translation_factor_y;
      else
	 pixels->y =
	    log10 (*values_y / box->priv->visible_top) *
	    box->priv->translation_factor_y;
   }
}

/** 
 * gtk_databox_value_to_pixel_x:
 * @box: A #GtkDatabox widget
 * @value: An x value
 *
 * Calculates the horizontal pixel coordinate which represents the x @value.
 * Pixel coordinates are relative to the top-left corner of the @box which is equivalent to (0,0).
 *
 * Return value: Pixel coordinate
 */
gint16
gtk_databox_value_to_pixel_x (GtkDatabox * box, gfloat value)
{
   if (box->priv->scale_type_x == GTK_DATABOX_SCALE_LINEAR)
      return (value -
	      box->priv->visible_left) *
	 box->priv->translation_factor_x;
   else if (box->priv->scale_type_x == GTK_DATABOX_SCALE_LOG2)
      return log2 (value / box->priv->visible_left) *
	 box->priv->translation_factor_x;
   else
      return log10 (value / box->priv->visible_left) *
	 box->priv->translation_factor_x;
}

/** 
 * gtk_databox_value_to_pixel_y:
 * @box: A #GtkDatabox widget
 * @value: A y value
 *
 * Calculates the vertical pixel coordinate which represents the y @value.
 * Pixel coordinates are relative to the top-left corner of the @box which is equivalent to (0,0).
 *
 * Return value: Pixel coordinate
 */
gint16
gtk_databox_value_to_pixel_y (GtkDatabox * box, gfloat value)
{
   if (box->priv->scale_type_y == GTK_DATABOX_SCALE_LINEAR)
      return (value -
	      box->priv->visible_top) * box->priv->translation_factor_y;
   else if (box->priv->scale_type_y == GTK_DATABOX_SCALE_LOG2)
      return log2 (value / box->priv->visible_top) *
	 box->priv->translation_factor_y;
   else
      return log10 (value / box->priv->visible_top) *
	 box->priv->translation_factor_y;
}

/** 
 * gtk_databox_pixel_to_value_x:
 * @box: A #GtkDatabox widget
 * @pixel: A horizontal pixel coordinate
 *
 * Calculates the x value which is represented by the horizontal @pixel coordinate.
 * Pixel coordinates are relative to the top-left corner of the @box which is equivalent to (0,0).
 *
 * Return value: x value
 */
gfloat
gtk_databox_pixel_to_value_x (GtkDatabox * box, gint16 pixel)
{
   if (box->priv->scale_type_x == GTK_DATABOX_SCALE_LINEAR)
      return box->priv->visible_left +
	 pixel / box->priv->translation_factor_x;
   else if (box->priv->scale_type_x == GTK_DATABOX_SCALE_LOG2)
      return box->priv->visible_left * pow (2,
					       pixel /
					       box->priv->
					       translation_factor_x);
   else
      return box->priv->visible_left * pow (10,
					       pixel /
					       box->priv->
					       translation_factor_x);
}

/** 
 * gtk_databox_pixel_to_value_y:
 * @box: A #GtkDatabox widget
 * @pixel: A vertical pixel coordinate
 *
 * Calculates the y value which is represented by the vertical @pixel coordinate.
 * Pixel coordinates are relative to the top-left corner of the @box which is equivalent to (0,0).
 *
 * Return value: y value
 */
gfloat
gtk_databox_pixel_to_value_y (GtkDatabox * box, gint16 pixel)
{
   if (box->priv->scale_type_y == GTK_DATABOX_SCALE_LINEAR)
      return box->priv->visible_top +
	 pixel / box->priv->translation_factor_y;
   else if (box->priv->scale_type_y == GTK_DATABOX_SCALE_LOG2)
      return box->priv->visible_top * pow (2,
					      pixel /
					      box->priv->
					      translation_factor_y);
   else
      return box->priv->visible_top * pow (10,
					      pixel /
					      box->priv->
					      translation_factor_y);
}

/** 
 * gtk_databox_create_box_with_scrollbars_and_rulers:
 * @p_box: Will contain a pointer to a #GtkDatabox widget
 * @p_table: Will contain a pointer to a #GtkTable widget
 * @scrollbar_x: Whether to attach a horizontal scrollbar
 * @scrollbar_y: Whether to attach a vertical scrollbar
 * @ruler_x: Whether to attach a horizontal ruler
 * @ruler_y: Whether to attach a vertical ruler
 *
 * This is a convenience function which creates a #GtkDatabox widget in a 
 * GtkTable widget optionally accompanied by scrollbars and rulers. You only 
 * have to fill in the data (gtk_databox_graph_add()) and adjust the limits
 * (gtk_databox_set_total_limits() or gtk_databox_auto_rescale()).
 *
 * @see_also: gtk_databox_new(), gtk_databox_set_adjustment_x(), gtk_databox_set_adjustment_y(), gtk_databox_set_ruler_x(), gtk_databox_set_ruler_y()
 */
void
gtk_databox_create_box_with_scrollbars_and_rulers (GtkWidget ** p_box,
						   GtkWidget ** p_table,
						   gboolean scrollbar_x,
						   gboolean scrollbar_y,
						   gboolean ruler_x,
						   gboolean ruler_y)
{
   GtkTable *table;
   GtkDatabox *box;
   GtkWidget *scrollbar;
   GtkWidget *ruler;

   *p_table = gtk_table_new (3, 3, FALSE);
   *p_box = gtk_databox_new ();
   box = GTK_DATABOX (*p_box);
   table = GTK_TABLE (*p_table);

   gtk_table_attach (table, GTK_WIDGET (box), 1, 2, 1, 2,
		     GTK_FILL | GTK_EXPAND | GTK_SHRINK,
		     GTK_FILL | GTK_EXPAND | GTK_SHRINK, 0, 0);

   if (scrollbar_x)
   {
      scrollbar = gtk_hscrollbar_new (NULL);
      gtk_databox_set_adjustment_x (box,
				   gtk_range_get_adjustment (GTK_RANGE
							     (scrollbar)));
      gtk_table_attach (table, scrollbar, 1, 2, 2, 3,
			GTK_FILL | GTK_EXPAND | GTK_SHRINK, GTK_FILL, 0, 0);
   }

   if (scrollbar_y)
   {
      scrollbar = gtk_vscrollbar_new (NULL);
      gtk_databox_set_adjustment_y (box,
				   gtk_range_get_adjustment (GTK_RANGE
							     (scrollbar)));
      gtk_table_attach (table, scrollbar, 2, 3, 1, 2, GTK_FILL,
			GTK_FILL | GTK_EXPAND | GTK_SHRINK, 0, 0);
   }

   if (ruler_x)
   {
      ruler = gtk_databox_ruler_new (GTK_ORIENTATION_HORIZONTAL);
      gtk_databox_ruler_set_scale_type (GTK_DATABOX_RULER (ruler),
					box->priv->scale_type_x);
      gtk_table_attach (table, ruler, 1, 2, 0, 1,
			GTK_FILL | GTK_EXPAND | GTK_SHRINK, GTK_FILL, 0, 0);
      gtk_databox_set_ruler_x (box, GTK_DATABOX_RULER (ruler));
   }

   if (ruler_y)
   {
      ruler = gtk_databox_ruler_new (GTK_ORIENTATION_VERTICAL);
      gtk_databox_ruler_set_scale_type (GTK_DATABOX_RULER (ruler),
					box->priv->scale_type_y);
      gtk_table_attach (table, ruler, 0, 1, 1, 2, GTK_FILL,
			GTK_FILL | GTK_EXPAND | GTK_SHRINK, 0, 0);
      gtk_databox_set_ruler_y (box, GTK_DATABOX_RULER (ruler));
   }
}


