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

#include <string.h>

#define RULER_SIZE          14

#define ROUND(x) ((int) ((x) + 0.5))

#define FORMAT_LENGTH 20 /* the length of the label format string */

#define LINEAR_FORMAT_MARKUP "%%-+%dg"
#define LOG_FORMAT_MARKUP "%%-%dg"

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

enum {
    PROP_0,
    PROP_LOWER,
    PROP_UPPER,
    PROP_POSITION,
    PROP_DRAW_POSITION,
    PROP_MAX_LENGTH,
    PROP_ORIENTATION,
    PROP_TEXT_ORIENTATION,
    PROP_TEXT_ALIGNMENT,
    PROP_TEXT_HOFFSET,
    PROP_DRAW_TICKS,
    PROP_DRAW_SUBTICKS,
    PROP_MANUAL_TICKS,
    PROP_MANUAL_TICK_CNT,
    PROP_MANUAL_TICK_LABELS,
    PROP_INVERT_EDGE,
    PROP_LINEAR_LABEL_FORMAT,
    PROP_LOG_LABEL_FORMAT,
    PROP_BOX_SHADOW,
    PROP_END_OF_LIST
};

struct _GtkDataboxRulerPrivate {
    GdkPixmap *backing_pixmap;
    gint xsrc;
    gint ysrc;
    /* The lower limit of the ruler */
    gdouble lower;
    /* The upper limit of the ruler */
    gdouble upper;
    /* The position of the mark on the ruler */
    gdouble position;
    /* whether to draw the position arrows*/
    gboolean draw_position;

    /* The maximum length of the labels (in characters) */
    guint max_length;
    /* The scale type of the ruler */
    GtkDataboxScaleType scale_type;
    /* Orientation of the ruler */
    GtkOrientation orientation;
    /* Orientation of the tick marks on the vertical ruler */
    GtkOrientation text_orientation;

    /* Whether the horizontal text on the vertical ruler is aligned left or right or center */
    PangoAlignment text_alignment;
    /* horizontal tick offset (shift ticks left or right) */
    gint text_hoffset;

    /* The maximum height of text on the horizontal ruler */
    gint max_x_text_height;
    /* The maximum width of text on the horizontal ruler */
    gint max_y_text_width;

    /* When true draw the ticks */
    gboolean draw_ticks;

    /* When true draw the subticks */
    gboolean draw_subticks;

    /* Whether the ruler is inverted (i.e. drawn with the edge on the left or right, top or bottom) */
    gboolean invert_edge;

    /* Strings used to mark up the g_sprintf label format - one for log scaling and one for linear scaling */
    gchar linear_format[FORMAT_LENGTH], log_format[FORMAT_LENGTH];

    /* If we are manually setting ticks, this will be non-null */
    gfloat *manual_ticks;
    guint manual_tick_cnt;
    /* we have the option of manually setting the tick labels. */
    gchar **manual_tick_labels;

    GtkShadowType box_shadow; /* The type of shadow drawn on the ruler pixmap */
};

G_DEFINE_TYPE (GtkDataboxRuler, gtk_databox_ruler, GTK_TYPE_WIDGET)

static void gtk_databox_ruler_class_init (GtkDataboxRulerClass * class) {
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
                                     PROP_DRAW_POSITION,
                                     g_param_spec_uint ("draw-position",
                                             "Draw Position Arrows",
                                             "Draw the position arrows: true or false",
                                             FALSE,
                                             TRUE,
                                             TRUE,
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
    g_object_class_install_property (gobject_class,
                                     PROP_TEXT_ORIENTATION,
                                     g_param_spec_uint ("text-orientation",
                                             "Text Orientation",
                                             "Orientation of the tick mark text (on the vertical ruler): horizontal or vertical",
                                             GTK_ORIENTATION_HORIZONTAL,
                                             GTK_ORIENTATION_VERTICAL,
                                             GTK_ORIENTATION_VERTICAL,
                                             G_PARAM_READWRITE));
    g_object_class_install_property (gobject_class,
                                     PROP_TEXT_ALIGNMENT,
                                     g_param_spec_uint ("text-alignment",
                                             "Text Alignment",
                                             "Alignment of the tick mark text (on the vertical ruler when using horizonal text): { PANGO_ALIGN_LEFT, PANGO_ALIGN_CENTER, PANGO_ALIGN_RIGHT}",
                                             PANGO_ALIGN_LEFT,
                                             PANGO_ALIGN_RIGHT,
                                             PANGO_ALIGN_LEFT,
                                             G_PARAM_READWRITE));
    g_object_class_install_property (gobject_class,
                                     PROP_TEXT_HOFFSET,
                                     g_param_spec_uint ("text-hoffset",
                                             "Text Horizonal offset",
                                             "Move the tick mark text left or right : pixels",
                                             0,
                                             20,
                                             0,
                                             G_PARAM_READWRITE));
    g_object_class_install_property (gobject_class,
                                     PROP_DRAW_TICKS,
                                     g_param_spec_uint ("draw-ticks",
                                             "Draw Ticks",
                                             "Draw the Ticks: true or false",
                                             FALSE,
                                             TRUE,
                                             TRUE,
                                             G_PARAM_READWRITE));
    g_object_class_install_property (gobject_class,
                                     PROP_DRAW_SUBTICKS,
                                     g_param_spec_uint ("draw-subticks",
                                             "Draw Subticks",
                                             "Draw the subticks: true or false",
                                             FALSE,
                                             TRUE,
                                             TRUE,
                                             G_PARAM_READWRITE));
    g_object_class_install_property (gobject_class,
                                     PROP_MANUAL_TICKS,
                                     g_param_spec_pointer ("manual-ticks",
                                             "Manual Ticks",
                                             "Manually specify the tick locations",
                                             G_PARAM_READWRITE));
    g_object_class_install_property (gobject_class,
                                     PROP_MANUAL_TICK_CNT,
                                     g_param_spec_uint ("manual-tick-cnt",
                                             "Manual Tick Count",
                                             "The number of manual ticks in the manual_tick array: horizontal or vertical",
                                             0,
                                             G_MAXUINT,
                                             0,
                                             G_PARAM_READWRITE));
    g_object_class_install_property (gobject_class,
                                     PROP_MANUAL_TICK_LABELS,
                                     g_param_spec_pointer ("manual-tick-labels",
                                             "Manual Tick Labels",
                                             "Manually specify the tick labels",
                                             G_PARAM_READWRITE));
    g_object_class_install_property (gobject_class,
                                     PROP_INVERT_EDGE,
                                     g_param_spec_uint ("invert-edge",
                                             "Invert Edge",
                                             "Invert the Edge - the edge is drawn inverted: true or false",
                                             FALSE,
                                             TRUE,
                                             FALSE,
                                             G_PARAM_READWRITE));
    g_object_class_install_property (gobject_class,
                                     PROP_LINEAR_LABEL_FORMAT,
                                     g_param_spec_string ("linear-label-format",
                                             "Linear Label Format",
                                             "Linear Label format mark up strings: marked up formatting strings for linear labels (i.e. \"%%-+%dg\")",
                                             LINEAR_FORMAT_MARKUP,
                                             G_PARAM_READWRITE));
    g_object_class_install_property (gobject_class,
                                     PROP_LOG_LABEL_FORMAT,
                                     g_param_spec_string ("log-label-format",
                                             "Log Label Format",
                                             "Log Label format mark up strings: marked up formatting strings for log labels (i.e. \"%%-%dg\")",
                                             LOG_FORMAT_MARKUP,
                                             G_PARAM_READWRITE));
    g_object_class_install_property (gobject_class,
                                     PROP_BOX_SHADOW,
                                     g_param_spec_uint ("box-shadow",
                                             "Box Shadow",
                                             "Style of the box shadow: GTK_SHADOW_NONE, GTK_SHADOW_IN, GTK_SHADOW_OUT, GTK_SHADOW_ETCHED_IN, GTK_SHADOW_ETCHED_OUT",
                                             GTK_SHADOW_NONE,
                                             GTK_SHADOW_ETCHED_OUT,
                                             GTK_SHADOW_OUT,
                                             G_PARAM_READWRITE));
}

static void
gtk_databox_ruler_init (GtkDataboxRuler * ruler) {
    ruler->priv = g_new0 (GtkDataboxRulerPrivate, 1);
    ruler->priv->backing_pixmap = NULL;
    ruler->priv->xsrc = 0;
    ruler->priv->ysrc = 0;
    ruler->priv->lower = 0;
    ruler->priv->upper = 0;
    ruler->priv->position = 0;
    ruler->priv->draw_position = TRUE;
    ruler->priv->max_length = 6;
    ruler->priv->scale_type = GTK_DATABOX_SCALE_LINEAR;
    ruler->priv->orientation = GTK_ORIENTATION_HORIZONTAL;
    ruler->priv->text_orientation = GTK_ORIENTATION_VERTICAL;
    ruler->priv->text_hoffset=0;
    ruler->priv->max_x_text_height = 0;
    ruler->priv->max_y_text_width = 0;
    ruler->priv->draw_ticks = TRUE;
    ruler->priv->draw_subticks = TRUE;
    ruler->priv->invert_edge = FALSE;
    g_stpcpy(ruler->priv->linear_format, LINEAR_FORMAT_MARKUP);
    g_stpcpy(ruler->priv->log_format, LOG_FORMAT_MARKUP);
    ruler->priv->manual_ticks=NULL;
    ruler->priv->manual_tick_cnt=0;
    ruler->priv->manual_tick_labels=NULL;
    ruler->priv->box_shadow=GTK_SHADOW_OUT;
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
gtk_databox_ruler_new (GtkOrientation orientation) {
    return g_object_new (GTK_DATABOX_TYPE_RULER, "orientation", orientation,
                         NULL);
}

static gint
gtk_databox_ruler_motion_notify (GtkWidget * widget, GdkEventMotion * event) {
    GtkDataboxRuler *ruler;
    gint x;
    gint y;


    ruler = GTK_DATABOX_RULER (widget);

    if (gtk_databox_ruler_get_draw_position (ruler)) {

        if (event->is_hint) {
            gdk_window_get_pointer (widget->window, &x, &y, NULL);
        } else {
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
            if (ruler->priv->draw_position)
                gtk_databox_ruler_draw_pos (ruler);
    }
    return FALSE;
}

static void
gtk_databox_ruler_set_property (GObject * object,
                                guint prop_id,
                                const GValue * value, GParamSpec * pspec) {
    GtkDataboxRuler *ruler = GTK_DATABOX_RULER (object);

    switch (prop_id) {
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
    case PROP_DRAW_POSITION:
        gtk_databox_ruler_set_draw_position (ruler, (gboolean) g_value_get_boolean (value));
        break;
    case PROP_MAX_LENGTH:
        gtk_databox_ruler_set_max_length (ruler, g_value_get_uint (value));
        break;
    case PROP_ORIENTATION:
        gtk_databox_ruler_set_orientation (ruler, (GtkOrientation) g_value_get_uint (value));
        break;
    case PROP_TEXT_ORIENTATION:
        gtk_databox_ruler_set_text_orientation (ruler, (GtkOrientation) g_value_get_uint (value));
        break;
    case PROP_TEXT_ALIGNMENT:
        gtk_databox_ruler_set_text_alignment (ruler, (GtkOrientation) g_value_get_uint (value));
        break;
    case PROP_TEXT_HOFFSET:
        gtk_databox_ruler_set_text_hoffset (ruler, (GtkOrientation) g_value_get_uint (value));
        break;
    case PROP_DRAW_TICKS:
        gtk_databox_ruler_set_draw_ticks (ruler, (gboolean) g_value_get_boolean (value));
        break;
    case PROP_DRAW_SUBTICKS:
        gtk_databox_ruler_set_draw_subticks (ruler, (gboolean) g_value_get_boolean (value));
        break;
    case PROP_MANUAL_TICKS:
        gtk_databox_ruler_set_manual_ticks (ruler, (gfloat *) g_value_get_pointer (value));
        break;
    case PROP_MANUAL_TICK_CNT:
        gtk_databox_ruler_set_manual_tick_cnt (ruler, g_value_get_uint (value));
        break;
    case PROP_MANUAL_TICK_LABELS:
        gtk_databox_ruler_set_manual_tick_labels (ruler, (gchar **) g_value_get_pointer (value));
        break;
    case PROP_INVERT_EDGE:
        gtk_databox_ruler_set_invert_edge (ruler, (gboolean) g_value_get_boolean (value));
        break;
    case PROP_LINEAR_LABEL_FORMAT:
        gtk_databox_ruler_set_linear_label_format (ruler, (gchar *) g_value_get_string (value));
        break;
    case PROP_LOG_LABEL_FORMAT:
        gtk_databox_ruler_set_log_label_format (ruler, (gchar *) g_value_get_string (value));
        break;
    case PROP_BOX_SHADOW:
        gtk_databox_ruler_set_box_shadow (ruler, (GtkShadowType) g_value_get_uint (value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}

static void
gtk_databox_ruler_get_property (GObject * object,
                                guint prop_id,
                                GValue * value, GParamSpec * pspec) {
    GtkDataboxRuler *ruler = GTK_DATABOX_RULER (object);

    switch (prop_id) {
    case PROP_LOWER:
        g_value_set_double (value, ruler->priv->lower);
        break;
    case PROP_UPPER:
        g_value_set_double (value, ruler->priv->upper);
        break;
    case PROP_POSITION:
        g_value_set_double (value, ruler->priv->position);
        break;
    case PROP_DRAW_POSITION:
        g_value_set_boolean (value, ruler->priv->draw_position);
        break;
    case PROP_MAX_LENGTH:
        g_value_set_uint (value, ruler->priv->max_length);
        break;
    case PROP_ORIENTATION:
        g_value_set_uint (value, ruler->priv->orientation);
        break;
    case PROP_TEXT_ORIENTATION:
        g_value_set_uint (value, ruler->priv->text_orientation);
        break;
    case PROP_TEXT_ALIGNMENT:
        g_value_set_uint (value, ruler->priv->text_alignment);
        break;
    case PROP_TEXT_HOFFSET:
        g_value_set_uint (value, ruler->priv->text_hoffset);
        break;
    case PROP_DRAW_TICKS:
        g_value_set_boolean (value, ruler->priv->draw_ticks);
        break;
    case PROP_DRAW_SUBTICKS:
        g_value_set_boolean (value, ruler->priv->draw_subticks);
        break;
    case PROP_MANUAL_TICKS:
        g_value_set_pointer (value, ruler->priv->manual_ticks);
        break;
    case PROP_MANUAL_TICK_CNT:
        g_value_set_uint (value, ruler->priv->manual_tick_cnt);
        break;
    case PROP_MANUAL_TICK_LABELS:
        g_value_set_pointer (value, ruler->priv->manual_tick_labels);
        break;
    case PROP_INVERT_EDGE:
        g_value_set_boolean (value, ruler->priv->invert_edge);
        break;
    case PROP_LINEAR_LABEL_FORMAT:
        g_value_set_string (value, ruler->priv->linear_format);
        break;
    case PROP_LOG_LABEL_FORMAT:
        g_value_set_string (value, ruler->priv->log_format);
        break;
    case PROP_BOX_SHADOW:
        g_value_set_uint (value, ruler->priv->box_shadow);
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
                             gdouble lower, gdouble upper, gdouble position) {
    g_return_if_fail (GTK_DATABOX_IS_RULER (ruler));

    g_object_freeze_notify (G_OBJECT (ruler));
    if (ruler->priv->lower != lower) {
        ruler->priv->lower = lower;
        g_object_notify (G_OBJECT (ruler), "lower");
    }
    if (ruler->priv->upper != upper) {
        ruler->priv->upper = upper;
        g_object_notify (G_OBJECT (ruler), "upper");
    }
    if (ruler->priv->position != position) {
        ruler->priv->position = position;
        g_object_notify (G_OBJECT (ruler), "position");
    }
    g_object_thaw_notify (G_OBJECT (ruler));

    if (gtk_widget_is_drawable (GTK_WIDGET (ruler)))
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
gtk_databox_ruler_set_max_length (GtkDataboxRuler * ruler, guint max_length) {
    g_return_if_fail (GTK_DATABOX_IS_RULER (ruler));
    g_return_if_fail (max_length < GTK_DATABOX_RULER_MAX_MAX_LENGTH + 1);

    g_object_freeze_notify (G_OBJECT (ruler));
    if (ruler->priv->max_length != max_length) {
        ruler->priv->max_length = max_length;
        g_object_notify (G_OBJECT (ruler), "max-length");
    }
    g_object_thaw_notify (G_OBJECT (ruler));

    if (gtk_widget_is_drawable (GTK_WIDGET (ruler)))
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
                                  GtkDataboxScaleType scale_type) {
    g_return_if_fail (GTK_DATABOX_IS_RULER (ruler));

    if (ruler->priv->scale_type != scale_type) {
        ruler->priv->scale_type = scale_type;
        /* g_object_notify (G_OBJECT (ruler), "scale-type"); */
    }

    if (gtk_widget_is_drawable (GTK_WIDGET (ruler)))
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
                                   GtkOrientation orientation) {
    GtkWidget *widget;
    g_return_if_fail (GTK_DATABOX_IS_RULER (ruler));

    if (ruler->priv->orientation != orientation) {
        ruler->priv->orientation = orientation;
        g_object_notify (G_OBJECT (ruler), "orientation");
    }

    widget = GTK_WIDGET (ruler);
    if (orientation == GTK_ORIENTATION_HORIZONTAL) {
        widget->requisition.width = widget->style->xthickness * 2 + 1;
        widget->requisition.height = widget->style->ythickness * 2 + RULER_SIZE;
    } else {
        widget->requisition.height = widget->style->ythickness * 2 + 1;
        if (ruler->priv->max_y_text_width==0)
            widget->requisition.width = widget->style->xthickness * 2 + RULER_SIZE;
        else
            widget->requisition.width = ruler->priv->max_y_text_width;
    }

    if (gtk_widget_is_drawable (GTK_WIDGET (ruler))) {
        gtk_widget_queue_resize (GTK_WIDGET (ruler));
        gtk_widget_queue_draw (GTK_WIDGET (ruler));
    }
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
gtk_databox_ruler_get_orientation (GtkDataboxRuler * ruler) {

    g_return_val_if_fail (GTK_DATABOX_IS_RULER (ruler), -1);

    return ruler->priv->orientation;
}

/**
 * gtk_databox_ruler_set_text_orientation:
 * @ruler: a #GtkDataboxRuler
 * @orientation: new orientation of the tick marks in the vertical ruler
 *
 * Sets the text orientation of the @ruler (vertical).
 **/
void
gtk_databox_ruler_set_text_orientation (GtkDataboxRuler * ruler,
                                        GtkOrientation orientation) {
    GtkWidget *widget;
    g_return_if_fail (GTK_DATABOX_IS_RULER (ruler));

    /* check this is a vertical ruler */
    if (ruler->priv->orientation != GTK_ORIENTATION_VERTICAL)
        return;

    if (ruler->priv->text_orientation != orientation) {
        ruler->priv->text_orientation = orientation;
        g_object_notify (G_OBJECT (ruler), "text-orientation");
    }

    widget = GTK_WIDGET (ruler);
    widget->requisition.height = widget->style->ythickness * 2 + 1;
    if (ruler->priv->max_y_text_width==0)
        widget->requisition.width = widget->style->xthickness * 2 + RULER_SIZE;
    else
        widget->requisition.width = ruler->priv->max_y_text_width;

    if (gtk_widget_is_drawable (GTK_WIDGET (ruler)))
        gtk_widget_queue_draw (GTK_WIDGET (ruler));

}

/**
 * gtk_databox_ruler_get_text_orientation:
 * @ruler: a #GtkDataboxRuler
 *
 * Gets the text orientation of the @ruler (horizontal or vertical).
 * Horizontal rulers always have horizontal text
 *
 * Return value: Text orientation of the @ruler.
 **/
GtkOrientation
gtk_databox_ruler_get_text_orientation (GtkDataboxRuler * ruler) {

    g_return_val_if_fail (GTK_DATABOX_IS_RULER (ruler), -1);

    return ruler->priv->text_orientation;
}

/**
 * gtk_databox_ruler_set_text_alignment:
 * @ruler: a #GtkDataboxRuler
 * @alignment: new alignment of the tick label in the vertical ruler when horizontal text is set
 *
 * Sets the text alignment of the @ruler (vertical with horizontal text).
 **/
void
gtk_databox_ruler_set_text_alignment (GtkDataboxRuler * ruler,
                                        PangoAlignment alignment) {
    GtkWidget *widget;
    g_return_if_fail (GTK_DATABOX_IS_RULER (ruler));

    /* check this is a vertical ruler */
    if (ruler->priv->orientation != GTK_ORIENTATION_VERTICAL)
        return;

    if (ruler->priv->text_alignment != alignment) {
        ruler->priv->text_alignment = alignment;
        g_object_notify (G_OBJECT (ruler), "text-alignment");
    }

    if (gtk_widget_is_drawable (GTK_WIDGET (ruler)))
        gtk_widget_queue_draw (GTK_WIDGET (ruler));
}

/**
 * gtk_databox_ruler_get_text_alignment:
 * @ruler: a #GtkDataboxRuler
 *
 * Gets the text alignment of the @ruler (vertical).
 * Vertical rulers with vertical text do not use this flag
 *
 * Return value: Text alignment of the @ruler.
 **/
PangoAlignment
gtk_databox_ruler_get_text_alignment (GtkDataboxRuler * ruler) {

    g_return_val_if_fail (GTK_DATABOX_IS_RULER (ruler), -1);

    return ruler->priv->text_alignment;
}

/**
 * gtk_databox_ruler_set_text_hoffset:
 * @ruler: a #GtkDataboxRuler
 * @offset: new x offset of the tick label in the ruler
 *
 * Sets the text x (horizontal) offset of the @ruler.
 **/
void
gtk_databox_ruler_set_text_hoffset (GtkDataboxRuler * ruler,
                                        gint offset) {
    GtkWidget *widget;
    g_return_if_fail (GTK_DATABOX_IS_RULER (ruler));

    if (ruler->priv->text_hoffset != offset) {
        ruler->priv->text_hoffset = offset;
        g_object_notify (G_OBJECT (ruler), "text-hoffset");
    }

    if (gtk_widget_is_drawable (GTK_WIDGET (ruler)))
        gtk_widget_queue_draw (GTK_WIDGET (ruler));
}

/**
 * gtk_databox_ruler_get_text_hoffset:
 * @ruler: a #GtkDataboxRuler
 *
 * Gets the text x (horizontal) offset of the @ruler.
 *
 * Return value: Text horizontal (x) offset of the @ruler.
 **/
gint
gtk_databox_ruler_get_text_hoffset (GtkDataboxRuler * ruler) {

    g_return_val_if_fail (GTK_DATABOX_IS_RULER (ruler), -1);

    return ruler->priv->text_hoffset;
}

/**
 * gtk_databox_ruler_set_draw_position:
 * @ruler: a #GtkDataboxRuler
 * @draw: whether to draw the position arrows on the ruler at all
 *
 * Sets the option for drawing the position arrows. If false, don't draw any arrows,
 * If true draw arrows.
 **/
void
gtk_databox_ruler_set_draw_position(GtkDataboxRuler * ruler, gboolean draw) {
    g_return_if_fail (GTK_DATABOX_IS_RULER (ruler));

    if (ruler->priv->draw_position!= draw) {
        ruler->priv->draw_position = draw;
        g_object_notify (G_OBJECT (ruler), "draw-position");

        if (gtk_widget_is_drawable (GTK_WIDGET (ruler)))
            gtk_widget_queue_draw (GTK_WIDGET (ruler));
    }
}

/**
 * gtk_databox_ruler_get_draw_position:
 * @ruler: a #GtkDataboxRuler
 *
 * Gets the draw position arrows option from the @ruler (horizontal or vertical).
 *
 * Return value: Position drawing option of the @ruler.
 **/
gboolean
gtk_databox_ruler_get_draw_position(GtkDataboxRuler * ruler) {

    g_return_val_if_fail (GTK_DATABOX_IS_RULER (ruler), -1);

    return ruler->priv->draw_position;
}

/**
 * gtk_databox_ruler_set_draw_ticks:
 * @ruler: a #GtkDataboxRuler
 * @draw: whether to draw the ticks on the ruler at all
 *
 * Sets the option for drawing the ticks. If false, don't draw any ticks,
 * If true draw major ticks and subticks if the draw_subticks boolean is set.
 **/
void
gtk_databox_ruler_set_draw_ticks(GtkDataboxRuler * ruler, gboolean draw) {
    g_return_if_fail (GTK_DATABOX_IS_RULER (ruler));

    if (ruler->priv->draw_ticks!= draw) {
        ruler->priv->draw_ticks = draw;
        g_object_notify (G_OBJECT (ruler), "draw-ticks");

        if (gtk_widget_is_drawable (GTK_WIDGET (ruler)))
            gtk_widget_queue_draw (GTK_WIDGET (ruler));
    }
}

/**
 * gtk_databox_ruler_get_draw_ticks:
 * @ruler: a #GtkDataboxRuler
 *
 * Gets the draw ticks option from the @ruler (horizontal or vertical).
 *
 * Return value: Tick drawing option of the @ruler.
 **/
gboolean
gtk_databox_ruler_get_draw_ticks(GtkDataboxRuler * ruler) {

    g_return_val_if_fail (GTK_DATABOX_IS_RULER (ruler), -1);

    return ruler->priv->draw_ticks;
}

/**
 * gtk_databox_ruler_set_draw_subticks:
 * @ruler: a #GtkDataboxRuler
 * @draw: whether to draw the subticks on the ruler
 *
 * Sets the option for drawing the subticks
 **/
void
gtk_databox_ruler_set_draw_subticks(GtkDataboxRuler * ruler, gboolean draw) {
    g_return_if_fail (GTK_DATABOX_IS_RULER (ruler));

    if (ruler->priv->draw_subticks!= draw) {
        ruler->priv->draw_subticks = draw;
        g_object_notify (G_OBJECT (ruler), "draw-subticks");

        if (gtk_widget_is_drawable (GTK_WIDGET (ruler)))
            gtk_widget_queue_draw (GTK_WIDGET (ruler));
    }
}

/**
 * gtk_databox_ruler_get_draw_subticks:
 * @ruler: a #GtkDataboxRuler
 *
 * Gets the draw subticks option from the @ruler (horizontal or vertical).
 *
 * Return value: Subtick drawing option of the @ruler.
 **/
gboolean
gtk_databox_ruler_get_draw_subticks(GtkDataboxRuler * ruler) {

    g_return_val_if_fail (GTK_DATABOX_IS_RULER (ruler), -1);

    return ruler->priv->draw_subticks;
}

/**
 * gtk_databox_grid_set_manual_ticks:
 * @ruler: a #GtkDataboxRuler
 * @manual_ticks: sets the pointer to the hline values for the @ruler
 *
 * Sets the ticks for the @ruler (horizontal or vertical).
 **/
void
gtk_databox_ruler_set_manual_ticks (GtkDataboxRuler * ruler, gfloat *manual_ticks) {
    g_return_if_fail (GTK_DATABOX_IS_RULER (ruler));

    ruler->priv->manual_ticks = manual_ticks;

    g_object_notify (G_OBJECT(ruler), "manual-ticks");
}

/**
 * gtk_databox_grid_get_manual_ticks:
 * @ruler: a #GtkDataboxRuler
 *
 * Gets the pointer to the manual tick values for the @ruler.
 *
 * Return value: Pointer to the manual tick values for the @ruler.
 **/
gfloat*
gtk_databox_ruler_get_manual_ticks (GtkDataboxRuler * ruler) {
    g_return_val_if_fail (GTK_DATABOX_IS_RULER (ruler), NULL);

    return ruler->priv->manual_ticks;
}

/**
 * gtk_databox_grid_set_manual_tick_cnt:
 * @ruler: a #GtkDataboxRuler
 * @manual_tick_cnt: sets the number of manual ticks for the @ruler
 *
 * Sets the number of manual ticks for the @ruler (horizontal or vertical).
 **/
void
gtk_databox_ruler_set_manual_tick_cnt (GtkDataboxRuler * ruler, guint manual_tick_cnt) {
    g_return_if_fail (GTK_DATABOX_IS_RULER (ruler));

    ruler->priv->manual_tick_cnt = manual_tick_cnt;

    g_object_notify (G_OBJECT(ruler), "manual-tick-cnt");
}

/**
 * gtk_databox_grid_get_manual_tick_cnt:
 * @ruler: a #GtkDataboxRuler
 *
 * Gets the number manual tick values for the @ruler.
 *
 * Return value: The number of manual tick values for the @ruler.
 **/
guint
gtk_databox_ruler_get_manual_tick_cnt (GtkDataboxRuler * ruler) {
    g_return_val_if_fail (GTK_DATABOX_IS_RULER (ruler), -1);

    return ruler->priv->manual_tick_cnt;
}

/**
 * gtk_databox_grid_set_manual_tick_labels:
 * @ruler: a #GtkDataboxRuler
 * @manual_tick_labels: sets the pointer to the labels for the ticks on the @ruler
 *
 * Note: This function should be preceeded by calls to gtk_databox_ruler_set_manual_ticks() and  gtk_databox_ruler_set_manual_tick_cnt().
 *       The number of tick labels should match gtk_databox_ruler_get_manual_tick_cnt().
 *
 * Sets the tick labels of the @ruler (horizontal or vertical).
 **/
void
gtk_databox_ruler_set_manual_tick_labels (GtkDataboxRuler * ruler, gchar **manual_tick_labels) {
    g_return_if_fail (GTK_DATABOX_IS_RULER (ruler));

    ruler->priv->manual_tick_labels = manual_tick_labels;

    g_object_notify (G_OBJECT(ruler), "manual-tick-labels");
}

/**
 * gtk_databox_grid_get_manual_tick_labels:
 * @ruler: a #GtkDataboxRuler
 *
 * Gets the pointer to the manual tick labels for the @ruler.
 *
 * Return value: Pointer to the manual tick labels for the @ruler.
 **/
gchar**
gtk_databox_ruler_get_manual_tick_labels (GtkDataboxRuler * ruler) {
    g_return_val_if_fail (GTK_DATABOX_IS_RULER (ruler), NULL);

    return ruler->priv->manual_tick_labels;
}

/**
 * gtk_databox_ruler_set_invert_edge:
 * @ruler: a #GtkDataboxRuler
 * @invert: whether to draw the ruler detail with the edge inverted
 *
 * Sets the option for drawing the ruler detail on the opposite edge
 **/
void
gtk_databox_ruler_set_invert_edge(GtkDataboxRuler * ruler, gboolean invert) {
    g_return_if_fail (GTK_DATABOX_IS_RULER (ruler));

    if (ruler->priv->invert_edge!= invert) {
        ruler->priv->invert_edge = invert;
        g_object_notify (G_OBJECT (ruler), "invert-edge");

        if (gtk_widget_is_drawable (GTK_WIDGET (ruler)))
            gtk_widget_queue_draw (GTK_WIDGET (ruler));
    }
}

/**
 * gtk_databox_ruler_get_invert_edge:
 * @ruler: a #GtkDataboxRuler
 *
 * Gets the invert edge option from the @ruler (horizontal or vertical).
 *
 * Return value: Edge inversion option of the @ruler.
 **/
gboolean
gtk_databox_ruler_get_invert_edge(GtkDataboxRuler * ruler) {

    g_return_val_if_fail (GTK_DATABOX_IS_RULER (ruler), -1);

    return ruler->priv->invert_edge;
}

/**
 * gtk_databox_ruler_set_linear_label_format:
 * @ruler: a #GtkDataboxRuler
 * @invert: How to format the labels for linear rulers
 *
 * Sets the meta format string for the labels of linear rulers, for example "%%+-%dg" will become "%+-#g" where # is the int variable
 **/
void
gtk_databox_ruler_set_linear_label_format(GtkDataboxRuler * ruler, gchar *format) {
    g_return_if_fail (GTK_DATABOX_IS_RULER (ruler));

    if (g_strcmp0(ruler->priv->linear_format, format)!=0) {
        if (strlen(format)>FORMAT_LENGTH) {
            g_warning("maximum format length = %d chars exceeded, truncating to the maximum from %d",FORMAT_LENGTH,(int)strlen(format));
            format[FORMAT_LENGTH]='\0';
        }

        g_stpcpy(ruler->priv->linear_format, format);
        g_object_notify (G_OBJECT (ruler), "linear-label-format");

        if (gtk_widget_is_drawable (GTK_WIDGET (ruler)))
            gtk_widget_queue_draw (GTK_WIDGET (ruler));
    }
}

/**
 * gtk_databox_ruler_get_linear_label_format:
 * @ruler: a #GtkDataboxRuler
 *
 * Gets the linear label meta format of the @ruler (horizontal or vertical).
 *
 * Return value: The string meta format the @ruler NULL on failure.
 **/
gchar*
gtk_databox_ruler_get_linear_label_format(GtkDataboxRuler * ruler) {

    g_return_val_if_fail (GTK_DATABOX_IS_RULER (ruler), NULL);

    return ruler->priv->linear_format;
}

/**
 * gtk_databox_ruler_set_log_label_format:
 * @ruler: a #GtkDataboxRuler
 * @invert: How to format the labels for log scaled rulers
 *
 * Sets the meta format string for the labels of log scaled rulers, for example "%%-%dg" will become "%-#g" where # is the int variable
 **/
void
gtk_databox_ruler_set_log_label_format(GtkDataboxRuler * ruler, gchar *format) {
    g_return_if_fail (GTK_DATABOX_IS_RULER (ruler));

    if (g_strcmp0(ruler->priv->log_format, format)!=0) {
        if (strlen(format)>FORMAT_LENGTH) {
            g_warning("maximum format length = %d chars exceeded, truncating to the maximum from %d",FORMAT_LENGTH,(int)strlen(format));
            format[FORMAT_LENGTH]='\0';
        }

        g_stpcpy(ruler->priv->log_format, format);
        g_object_notify (G_OBJECT (ruler), "log-label-format");

        if (gtk_widget_is_drawable (GTK_WIDGET (ruler)))
            gtk_widget_queue_draw (GTK_WIDGET (ruler));
    }
}

/**
 * gtk_databox_ruler_get_log_label_format:
 * @ruler: a #GtkDataboxRuler
 *
 * Gets the log label meta format of the @ruler (horizontal or vertical).
 *
 * Return value: The string meta format the @ruler, NULL on failure.
 **/
gchar*
gtk_databox_ruler_get_log_label_format(GtkDataboxRuler * ruler) {

    g_return_val_if_fail (GTK_DATABOX_IS_RULER (ruler), NULL);

    return ruler->priv->log_format;
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
                             gdouble * upper, gdouble * position) {
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
gtk_databox_ruler_get_max_length (GtkDataboxRuler * ruler) {
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
gtk_databox_ruler_get_scale_type (GtkDataboxRuler * ruler) {
    g_return_val_if_fail (GTK_DATABOX_IS_RULER (ruler), 0);

    return ruler->priv->scale_type;
}

/**
 * gtk_databox_ruler_set_box_shadow:
 * @ruler: a #GtkDataboxRuler
 * @which_shadow: How to render the box shadow on the ruler edges.
 *
 * Sets the shadow type when using gtk_paint_box. This will draw the desired edge shadow.
 **/
void
gtk_databox_ruler_set_box_shadow(GtkDataboxRuler * ruler, GtkShadowType which_shadow) {
    g_return_if_fail (GTK_DATABOX_IS_RULER (ruler));
    g_return_if_fail (which_shadow>=0);
    g_return_if_fail (which_shadow<=GTK_SHADOW_ETCHED_OUT);

    if (ruler->priv->box_shadow!=which_shadow) {
        ruler->priv->box_shadow=which_shadow;
        if (gtk_widget_is_drawable (GTK_WIDGET (ruler)))
            gtk_widget_queue_draw (GTK_WIDGET (ruler));
    }
}

/**
 * gtk_databox_ruler_get_box_shadow:
 * @ruler: a #GtkDataboxRuler
 *
 * Gets the type of shadow being rendered to the @ruler (GTK_SHADOW_NONE, GTK_SHADOW_IN, GTK_SHADOW_OUT, GTK_SHADOW_ETCHED_IN, GTK_SHADOW_ETCHED_OUT).
 *
 * Return value: The currently used shadow type of the @ruler, -1 on failure.
 **/
GtkShadowType
gtk_databox_ruler_get_box_shadow(GtkDataboxRuler * ruler) {

    g_return_val_if_fail (GTK_DATABOX_IS_RULER (ruler), -1);

    return ruler->priv->box_shadow;
}

static void
gtk_databox_ruler_draw_ticks (GtkDataboxRuler * ruler) {
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
    gdouble start, end, cur, cur_text;
    gchar unit_str[GTK_DATABOX_RULER_MAX_MAX_LENGTH + 1];	/* buffer for writing numbers */
    gint digit_width;
    gint text_width;
    gint pos;
    gint y_loc, x_loc;
    gint subtick_start;
    gchar format_string[FORMAT_LENGTH];
    PangoMatrix matrix = PANGO_MATRIX_INIT;
    PangoContext *context;
    PangoLayout *layout;
    PangoRectangle logical_rect, ink_rect;

    if (ruler->priv->scale_type == GTK_DATABOX_SCALE_LINEAR)
        if (ruler->priv->max_length==1)
            g_snprintf (format_string, FORMAT_LENGTH, ruler->priv->linear_format, ruler->priv->max_length);
        else
            g_snprintf (format_string, FORMAT_LENGTH, ruler->priv->linear_format, ruler->priv->max_length - 1);
    else if (ruler->priv->max_length==1)
        g_snprintf (format_string, FORMAT_LENGTH, ruler->priv->log_format, ruler->priv->max_length);
    else
        g_snprintf (format_string, FORMAT_LENGTH, ruler->priv->log_format, ruler->priv->max_length - 1);

    if (!gtk_widget_is_drawable (GTK_WIDGET (ruler)))
        return;

    widget = GTK_WIDGET (ruler);

    xthickness = widget->style->xthickness;
    ythickness = widget->style->ythickness;

    layout = gtk_widget_create_pango_layout (widget, "E+-012456789");

    if ((ruler->priv->orientation == GTK_ORIENTATION_VERTICAL) && (ruler->priv->text_orientation == GTK_ORIENTATION_VERTICAL)) {
        /* vertical ruler with vertical text */
        context = gtk_widget_get_pango_context (widget);
        pango_context_set_base_gravity (context, PANGO_GRAVITY_WEST);
        pango_matrix_rotate (&matrix, 90.);
        pango_context_set_matrix (context, &matrix);
        pango_layout_context_changed(layout);
    }

    pango_layout_get_pixel_extents (layout, &ink_rect, &logical_rect);

    digit_width = ceil ((logical_rect.width) / 12);

    width = widget->allocation.width;
    height = widget->allocation.height;
    gtk_paint_box (widget->style, ruler->priv->backing_pixmap,
                   GTK_STATE_NORMAL, ruler->priv->box_shadow,
                   NULL, widget, "ruler", 0, 0, width, height);

    cr = gdk_cairo_create (ruler->priv->backing_pixmap);
    gdk_cairo_set_source_color (cr, &widget->style->fg[widget->state]);

    if (ruler->priv->draw_ticks) /* only draw the bottom line IF we are drawing ticks */
        cairo_rectangle (cr, xthickness, height - ythickness, width - 2 * xthickness, 1);

    if (ruler->priv->scale_type == GTK_DATABOX_SCALE_LINEAR) {
        upper = ruler->priv->upper;
        lower = ruler->priv->lower;
    } else if (ruler->priv->scale_type == GTK_DATABOX_SCALE_LOG2) {
        if (ruler->priv->upper <= 0 || ruler->priv->lower <= 0) {
            g_warning
            ("For logarithmic scaling, the visible limits must by larger than 0!");
        }
        upper = log2 (ruler->priv->upper);
        lower = log2 (ruler->priv->lower);
    } else {
        if (ruler->priv->upper <= 0 || ruler->priv->lower <= 0) {
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
    if (ruler->priv->scale_type == GTK_DATABOX_SCALE_LINEAR) {
        text_width = (ruler->priv->max_length) * digit_width + 1;

        for (power = -20; power < 21; power++) {
            if ((digit = 1) * pow (10, power) * fabs (increment) > text_width)
                break;
            if ((digit = 2.5) * pow (10, power) * fabs (increment) > text_width)
                break;
            if ((digit = 5) * pow (10, power) * fabs (increment) > text_width)
                break;
        }


        if (power == 21) {
            power = 20;
            digit = 5;
        }
        subd_incr = digit * pow (10, power);
    } else if (ruler->priv->scale_type == GTK_DATABOX_SCALE_LOG2) {
        subd_incr = 1.;
    } else {
        subd_incr = 1.;
    }

    length = (ruler->priv->orientation == GTK_ORIENTATION_HORIZONTAL)
             ? height - 1 : width - 1;

    if (ruler->priv->manual_ticks==NULL)
        if (lower < upper) {
            start = floor (lower / subd_incr) * subd_incr;
            end = ceil (upper / subd_incr) * subd_incr;
        } else {
            start = floor (upper / subd_incr) * subd_incr;
            end = ceil (lower / subd_incr) * subd_incr;
        }
    else { /* we are manually setting the tick labels and marks. */
        start = 0.;
        end = (gfloat)ruler->priv->manual_tick_cnt-1;
        subd_incr=1.;
    }


    for (cur = start; cur <= end; cur += subd_incr) {
        if (ruler->priv->manual_ticks==NULL)
            pos = ROUND (((cur_text=cur) - lower) * increment);
        else {
            /* manual ticks must be positioned according to the scale */
            if (ruler->priv->scale_type == GTK_DATABOX_SCALE_LINEAR)
                cur_text=ruler->priv->manual_ticks[(int)cur];
            else if (ruler->priv->scale_type == GTK_DATABOX_SCALE_LOG2)
                cur_text=log2(ruler->priv->manual_ticks[(int)cur]);
            else
                cur_text=log10(ruler->priv->manual_ticks[(int)cur]);
            pos = ROUND ((cur_text - lower) * increment);
            cur_text=ruler->priv->manual_ticks[(int)cur];
        }
        if (ruler->priv->draw_ticks) {
            if (ruler->priv->orientation == GTK_ORIENTATION_HORIZONTAL)
                cairo_rectangle (cr, pos, height + ythickness - length, 1, length);
            else
                cairo_rectangle (cr, width + xthickness - length, pos, length, 1);
        }


        /* draw label */
        /* if manual tick labels are present, display them instead of calculated labels */
        if ((ruler->priv->manual_ticks!=NULL) && (ruler->priv->manual_tick_cnt!=0) && (ruler->priv->manual_tick_labels!=NULL))
            pango_layout_set_text (layout, ruler->priv->manual_tick_labels[(int)cur], -1);
        else {
            if ((ruler->priv->scale_type == GTK_DATABOX_SCALE_LINEAR) || (ruler->priv->manual_ticks!=NULL)) {
                if (ABS (cur_text) < 0.1 * subd_incr)	/* Rounding errors occur and might make "0" look funny without this check */
                    cur_text = 0;

                g_snprintf (unit_str, ruler->priv->max_length + 1, format_string, cur_text);
            } else if (ruler->priv->scale_type == GTK_DATABOX_SCALE_LOG2)
                g_snprintf (unit_str, ruler->priv->max_length + 1, format_string, pow (2, cur_text));
            else
                g_snprintf (unit_str, ruler->priv->max_length + 1, format_string, pow (10, cur_text));

            pango_layout_set_text (layout, unit_str, -1);
        }
        pango_layout_get_pixel_extents (layout, &ink_rect, &logical_rect);

        /* remember the pixel extents for sizing later. */
        if ((ruler->priv->orientation == GTK_ORIENTATION_VERTICAL) & (ruler->priv->max_y_text_width<logical_rect.width)) {
            if (ruler->priv->text_orientation ==GTK_ORIENTATION_VERTICAL)
                ruler->priv->max_y_text_width=logical_rect.height;
            else
                ruler->priv->max_y_text_width=logical_rect.width;
            gtk_widget_set_size_request(GTK_WIDGET(ruler), ruler->priv->max_y_text_width, ruler->priv->max_x_text_height);
            /*printf("set ruler->priv->max_x_text_height=%d\n",ruler->priv->max_x_text_height);
            printf("set ruler->priv->max_y_text_width=%d\n",ruler->priv->max_y_text_width);*/
        } else if (ruler->priv->max_x_text_height<logical_rect.height) {
            ruler->priv->max_x_text_height=logical_rect.height;
            gtk_widget_set_size_request(GTK_WIDGET(ruler), ruler->priv->max_y_text_width, ruler->priv->max_x_text_height);
            /*printf("set ruler->priv->max_x_text_height=%d\n",ruler->priv->max_x_text_height);
            printf("set ruler->priv->max_y_text_width=%d\n",ruler->priv->max_y_text_width);*/
        }



        if (ruler->priv->orientation == GTK_ORIENTATION_HORIZONTAL){
            if (!ruler->priv->draw_ticks) /* if ticks aren't present, draw a little lower */
                pos=pos - logical_rect.width+2+ruler->priv->text_hoffset;
            gtk_paint_layout (widget->style,
                              ruler->priv->backing_pixmap,
                              gtk_widget_get_state (widget),
                              FALSE,
                              NULL,
                              widget, "ruler", pos + 2, ythickness - 1, layout);
        } else {
            y_loc=pos - logical_rect.width - 2; /* standard vertical text y alignment */
            if (ruler->priv->text_orientation == GTK_ORIENTATION_HORIZONTAL) /* if ticks are present, then draw a little higher */
                y_loc=pos - logical_rect.width*2/3; /* horizontal text y alignment */
            if (ruler->priv->text_orientation == GTK_ORIENTATION_HORIZONTAL & !ruler->priv->draw_ticks) /* if ticks aren't present, draw a little lower */
                y_loc=pos - logical_rect.width/3;
            x_loc=xthickness-1+ruler->priv->text_hoffset;
            if (ruler->priv->text_orientation == GTK_ORIENTATION_HORIZONTAL & ruler->priv->text_alignment == PANGO_ALIGN_RIGHT) /* set right adjusted text */
                x_loc=width-ink_rect.width-2+ruler->priv->text_hoffset; /* shift 2 pixels left to give a better aesthetic */
            if (ruler->priv->text_orientation == GTK_ORIENTATION_HORIZONTAL & ruler->priv->text_alignment == PANGO_ALIGN_CENTER) /* set centrally adjusted text */
                x_loc=(width-ink_rect.width)/2-2+ruler->priv->text_hoffset;
            gtk_paint_layout (widget->style,
                              ruler->priv->backing_pixmap,
                              gtk_widget_get_state (widget),
                              FALSE,
                              NULL,
                              widget, "ruler", x_loc, y_loc, layout);
        }

        /* Draw sub-ticks */
        if (ruler->priv->draw_subticks & ruler->priv->draw_ticks) {
            if (!ruler->priv->invert_edge) /* sub-ticks on the bottom */
                subtick_start=length / 2;
            else
                subtick_start=length;

            if (ruler->priv->scale_type == GTK_DATABOX_SCALE_LINEAR)
                for (i = 1; i < 5; ++i) {
                    pos = ROUND ((cur - lower + subd_incr / 5 * i) * increment);

                    if (ruler->priv->orientation == GTK_ORIENTATION_HORIZONTAL)
                        cairo_rectangle (cr, pos, height + ythickness - subtick_start, 1, length / 2);
                    else
                        cairo_rectangle (cr, width + xthickness - subtick_start, pos, length / 2, 1);
                }
            else  if (ruler->priv->scale_type == GTK_DATABOX_SCALE_LOG2)
                for (i = 1; i < 8; ++i) {
                    pos = ROUND ((cur - lower + log2 (i)) * increment);

                    if (ruler->priv->orientation == GTK_ORIENTATION_HORIZONTAL)
                        cairo_rectangle (cr, pos, height + ythickness - subtick_start, 1, length / 2);
                    else
                        cairo_rectangle (cr, width + xthickness - subtick_start, pos, length / 2, 1);
                }
            else
                for (i = 2; i < 10; ++i) {
                    pos = ROUND ((cur - lower + log10 (i)) * increment);

                    if (ruler->priv->orientation == GTK_ORIENTATION_HORIZONTAL)
                        cairo_rectangle (cr, pos, height + ythickness - subtick_start, 1, length / 2);
                    else
                        cairo_rectangle (cr, width + xthickness - subtick_start, pos, length / 2, 1);
                }
        }
    }

    cairo_fill (cr);
out:
    cairo_destroy (cr);

    g_object_unref (layout);
}

static void
gtk_databox_ruler_draw_pos (GtkDataboxRuler * ruler) {
    GtkWidget *widget = GTK_WIDGET (ruler);
    gint x, y;
    gint width, height;
    gint bs_width, bs_height;
    gint xthickness;
    gint ythickness;
    gdouble increment;
    cairo_t *cr;

    if (gtk_widget_is_drawable (GTK_WIDGET (ruler))) {
        xthickness = widget->style->xthickness;
        ythickness = widget->style->ythickness;
        width = widget->allocation.width - xthickness * 2;
        height = widget->allocation.height - ythickness * 2;

        if (ruler->priv->orientation == GTK_ORIENTATION_HORIZONTAL) {
            bs_width = height / 2 + 2;
            bs_width |= 1;		/* make sure it's odd */
            bs_height = bs_width / 2 + 1;
            if (ruler->priv->invert_edge)
                bs_height=-bs_height;
        } else {
            bs_height = width / 2 + 2;
            bs_height |= 1;	/* make sure it's odd */
            bs_width = bs_height / 2 + 1;
            if (ruler->priv->invert_edge)
                bs_width=-bs_width;
        }

        if (!ruler->priv->invert_edge && (bs_width < 0) && (bs_height < 0))
            return; /* return if negative values and not inverted */

        if (ruler->priv->invert_edge && (bs_width > 0) && (bs_height > 0))
            return; /* return if positive values and inverted */

        cr = gdk_cairo_create (widget->window);

        /*  If a backing store exists, restore the ruler  */
        if (ruler->priv->backing_pixmap)
            gdk_draw_drawable (widget->window,
                               widget->style->black_gc,
                               ruler->priv->backing_pixmap,
                               ruler->priv->xsrc, ruler->priv->ysrc,
                               ruler->priv->xsrc, ruler->priv->ysrc, bs_width, bs_height);

        if (ruler->priv->orientation == GTK_ORIENTATION_HORIZONTAL) {
            increment = (gdouble) width / (ruler->priv->upper - ruler->priv->lower);

            x = ROUND ((ruler->priv->position - ruler->priv->lower) * increment) +
                (xthickness - bs_width) / 2 - 1;
            y = (height + bs_height) / 2 + ythickness;

            gdk_cairo_set_source_color (cr, &widget->style->fg[widget->state]);

            cairo_move_to (cr, x, y);
            cairo_line_to (cr, x + bs_width / 2., y + bs_height);
            cairo_line_to (cr, x + bs_width, y);
        } else {
            increment = (gdouble) height / (ruler->priv->upper - ruler->priv->lower);

            x = (width + bs_width) / 2 + xthickness;
            y = ROUND ((ruler->priv->position - ruler->priv->lower) * increment) +
                (ythickness - bs_height) / 2 - 1;

            gdk_cairo_set_source_color (cr, &widget->style->fg[widget->state]);

            cairo_move_to (cr, x, y);
            cairo_line_to (cr, x + bs_width, y + bs_height / 2.);
            cairo_line_to (cr, x, y + bs_height);
        }
        cairo_fill (cr);

        cairo_destroy (cr);

        /* remember the rectangle of the arrow - so that it may be cleared on re-run */
        ruler->priv->xsrc = x;
        ruler->priv->ysrc = y;
        if (ruler->priv->invert_edge) { /* inverted edges need clearing in the negative direction */
            if (ruler->priv->orientation == GTK_ORIENTATION_HORIZONTAL)
                ruler->priv->ysrc = y+bs_height; /* bs_height is negative */
            else
                ruler->priv->xsrc = x+bs_width; /* bs_width is negative */
        }
    }
}


static void
gtk_databox_ruler_realize (GtkWidget * widget) {
    GtkDataboxRuler *ruler;
    GdkWindowAttr attributes;
    gint attributes_mask;

    ruler = GTK_DATABOX_RULER (widget);

    gtk_widget_set_realized(GTK_WIDGET (ruler), GTK_REALIZED);
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
gtk_databox_ruler_unrealize (GtkWidget * widget) {
    GtkDataboxRuler *ruler = GTK_DATABOX_RULER (widget);
    gtk_widget_set_realized(widget, FALSE);

    if (ruler->priv->backing_pixmap)
        g_object_unref (ruler->priv->backing_pixmap);
    ruler->priv->backing_pixmap=NULL;


    if (GTK_WIDGET_CLASS (gtk_databox_ruler_parent_class)->unrealize)
        (*GTK_WIDGET_CLASS (gtk_databox_ruler_parent_class)->unrealize) (widget);
}

static void
gtk_databox_ruler_size_allocate (GtkWidget * widget,
                                 GtkAllocation * allocation) {
    GtkDataboxRuler *ruler = GTK_DATABOX_RULER (widget);

    widget->allocation = *allocation;

    if (gtk_widget_get_realized (widget))
        if (gtk_widget_is_drawable(widget)) {
            gdk_window_move_resize (widget->window,
                                    allocation->x, allocation->y,
                                    allocation->width, allocation->height);

            gtk_databox_ruler_make_pixmap (ruler);
        }
}

static gint
gtk_databox_ruler_expose (GtkWidget * widget, GdkEventExpose * event) {
    GtkDataboxRuler *ruler;

    if (gtk_widget_is_drawable (widget)) {
        ruler = GTK_DATABOX_RULER (widget);

        gtk_databox_ruler_draw_ticks (ruler);

        gdk_draw_drawable (widget->window,
                           widget->style->fg_gc[gtk_widget_get_state (GTK_WIDGET (ruler))],
                           ruler->priv->backing_pixmap,
                           0, 0, 0, 0,
                           widget->allocation.width, widget->allocation.height);

        if (ruler->priv->draw_position)
            gtk_databox_ruler_draw_pos (ruler);
    }

    return FALSE;
}

static void
gtk_databox_ruler_make_pixmap (GtkDataboxRuler * ruler) {
    GtkWidget *widget;
    gint width;
    gint height;

    widget = GTK_WIDGET (ruler);

    if (ruler->priv->backing_pixmap) {
        gdk_pixmap_get_size (ruler->priv->backing_pixmap, &width, &height);
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
