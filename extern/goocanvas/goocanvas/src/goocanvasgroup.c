/*
 * GooCanvas. Copyright (C) 2005 Damon Chaplin.
 * Released under the GNU LGPL license. See COPYING for details.
 *
 * goocanvasgroup.c - group item.
 */

/**
 * SECTION:goocanvasgroup
 * @Title: GooCanvasGroup
 * @Short_Description: a group of items.
 *
 * #GooCanvasGroup represents a group of items. Groups can be nested to
 * any depth, to create a hierarchy of items. Items are ordered within each
 * group, with later items being displayed above earlier items.
 *
 * #GooCanvasGroup is a subclass of #GooCanvasItemSimple and so
 * inherits all of the style properties such as "stroke-color", "fill-color"
 * and "line-width". Setting a style property on a #GooCanvasGroup will affect
 * all children of the #GooCanvasGroup (unless the children override the
 * property setting).
 *
 * #GooCanvasGroup implements the #GooCanvasItem interface, so you can use
 * the #GooCanvasItem functions such as goo_canvas_item_raise() and
 * goo_canvas_item_rotate(), and the properties such as "visibility" and
 * "pointer-events".
 *
 * If the #GooCanvasGroup:width and #GooCanvasGroup:height properties are
 * set to positive values then the group is clipped to the given size.
 *
 * To create a #GooCanvasGroup use goo_canvas_group_new().
 *
 * To get or set the properties of an existing #GooCanvasGroup, use
 * g_object_get() and g_object_set().
 */
#include <config.h>
#include <glib/gi18n-lib.h>
#include <gtk/gtk.h>
#include "goocanvasprivate.h"
#include "goocanvasgroup.h"
#include "goocanvasitemmodel.h"
#include "goocanvas.h"
#include "goocanvasmarshal.h"
#include "goocanvasatk.h"

typedef struct _GooCanvasGroupPrivate GooCanvasGroupPrivate;
struct _GooCanvasGroupPrivate {
  gdouble x;
  gdouble y;
  gdouble width;
  gdouble height;
};

#define GOO_CANVAS_GROUP_GET_PRIVATE(group)  \
   (G_TYPE_INSTANCE_GET_PRIVATE ((group), GOO_TYPE_CANVAS_GROUP, GooCanvasGroupPrivate))
#define GOO_CANVAS_GROUP_MODEL_GET_PRIVATE(group)  \
   (G_TYPE_INSTANCE_GET_PRIVATE ((group), GOO_TYPE_CANVAS_GROUP_MODEL, GooCanvasGroupPrivate))

enum {
  PROP_0,

  PROP_X,
  PROP_Y,
  PROP_WIDTH,
  PROP_HEIGHT
};

static void goo_canvas_group_dispose	  (GObject            *object);
static void goo_canvas_group_finalize     (GObject            *object);
static void goo_canvas_group_get_property (GObject            *object,
                                           guint               prop_id,
                                           GValue             *value,
                                           GParamSpec         *pspec);
static void goo_canvas_group_set_property (GObject            *object,
                                           guint               prop_id,
                                           const GValue       *value,
                                           GParamSpec         *pspec);
static void canvas_item_interface_init    (GooCanvasItemIface *iface);

G_DEFINE_TYPE_WITH_CODE (GooCanvasGroup, goo_canvas_group,
			 GOO_TYPE_CANVAS_ITEM_SIMPLE,
			 G_IMPLEMENT_INTERFACE (GOO_TYPE_CANVAS_ITEM,
						canvas_item_interface_init))

static void
goo_canvas_group_install_common_properties (GObjectClass *gobject_class)
{
  g_object_class_install_property (gobject_class, PROP_X,
                                  g_param_spec_double ("x",
                                                       "X",
                                                       _("The x coordinate of the group"),
                                                       -G_MAXDOUBLE,
                                                       G_MAXDOUBLE, 0.0,
                                                       G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_Y,
                                  g_param_spec_double ("y",
                                                       "Y",
                                                       _("The y coordinate of the group"),
                                                       -G_MAXDOUBLE,
                                                       G_MAXDOUBLE, 0.0,
                                                       G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_WIDTH,
                                  g_param_spec_double ("width",
                                                       _("Width"),
                                                       _("The width of the group, or -1 to use the default width"),
                                                       -G_MAXDOUBLE,
                                                       G_MAXDOUBLE, -1.0,
                                                       G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_HEIGHT,
                                  g_param_spec_double ("height",
                                                       _("Height"),
                                                       _("The height of the group, or -1 to use the default height"),
                                                       -G_MAXDOUBLE,
                                                       G_MAXDOUBLE, -1.0,
                                                       G_PARAM_READWRITE));
}

static void
goo_canvas_group_class_init (GooCanvasGroupClass *klass)
{
  GObjectClass *gobject_class = (GObjectClass*) klass;

  g_type_class_add_private (gobject_class, sizeof (GooCanvasGroupPrivate));

  gobject_class->dispose  = goo_canvas_group_dispose;
  gobject_class->finalize = goo_canvas_group_finalize;
  gobject_class->get_property = goo_canvas_group_get_property;
  gobject_class->set_property = goo_canvas_group_set_property;

  /* Register our accessible factory, but only if accessibility is enabled. */
  if (!ATK_IS_NO_OP_OBJECT_FACTORY (atk_registry_get_factory (atk_get_default_registry (), GTK_TYPE_WIDGET)))
    {
      atk_registry_set_factory_type (atk_get_default_registry (),
				     GOO_TYPE_CANVAS_GROUP,
				     goo_canvas_item_accessible_factory_get_type ());
    }

  goo_canvas_group_install_common_properties (gobject_class);
}


static void
goo_canvas_group_init (GooCanvasGroup *group)
{
  GooCanvasGroupPrivate* priv = GOO_CANVAS_GROUP_GET_PRIVATE (group);

  group->items = g_ptr_array_sized_new (8);

  priv->x = 0.0;
  priv->y = 0.0;
  priv->width = -1.0;
  priv->height = -1.0;
}


/**
 * goo_canvas_group_new:
 * @parent: the parent item, or %NULL. If a parent is specified, it will assume
 *  ownership of the item, and the item will automatically be freed when it is
 *  removed from the parent. Otherwise call g_object_unref() to free it.
 * @...: optional pairs of property names and values, and a terminating %NULL.
 * 
 * Creates a new group item.
 * 
 * Return value: a new group item.
 **/
GooCanvasItem*
goo_canvas_group_new (GooCanvasItem *parent,
		      ...)
{
  GooCanvasItem *item;
  GooCanvasGroup *group;
  va_list var_args;
  const char *first_property;

  item = g_object_new (GOO_TYPE_CANVAS_GROUP, NULL);
  group = (GooCanvasGroup*) item;

  va_start (var_args, parent);
  first_property = va_arg (var_args, char*);
  if (first_property)
    g_object_set_valist (G_OBJECT (item), first_property, var_args);
  va_end (var_args);

  if (parent)
    {
      goo_canvas_item_add_child (parent, item, -1);
      g_object_unref (item);
    }

  return item;
}


static void
goo_canvas_group_dispose (GObject *object)
{
  GooCanvasGroup *group = (GooCanvasGroup*) object;
  gint i;

  /* Unref all the items in the group. */
  for (i = 0; i < group->items->len; i++)
    {
      GooCanvasItem *item = group->items->pdata[i];
      goo_canvas_item_set_parent (item, NULL);
      g_object_unref (item);
    }

  g_ptr_array_set_size (group->items, 0);

  G_OBJECT_CLASS (goo_canvas_group_parent_class)->dispose (object);
}


static void
goo_canvas_group_finalize (GObject *object)
{
  GooCanvasGroup *group = (GooCanvasGroup*) object;

  g_ptr_array_free (group->items, TRUE);

  G_OBJECT_CLASS (goo_canvas_group_parent_class)->finalize (object);
}


/* Gets the private data to use, from the model or from the item itself. */
static GooCanvasGroupPrivate*
goo_canvas_group_get_private (GooCanvasGroup *group)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) group;

  if (simple->model)
    return GOO_CANVAS_GROUP_MODEL_GET_PRIVATE (simple->model);
  else
    return GOO_CANVAS_GROUP_GET_PRIVATE (group);
}


static void
goo_canvas_group_get_common_property (GObject               *object,
                                      GooCanvasGroupPrivate *priv,
                                      guint                  prop_id,
                                      GValue                *value,
                                      GParamSpec            *pspec)
{
  switch (prop_id)
    {
    case PROP_X:
      g_value_set_double (value, priv->x);
      break;
    case PROP_Y:
      g_value_set_double (value, priv->y);
      break;
    case PROP_WIDTH:
      g_value_set_double (value, priv->width);
      break;
    case PROP_HEIGHT:
      g_value_set_double (value, priv->height);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
goo_canvas_group_get_property (GObject               *object,
                               guint                  prop_id,
                               GValue                *value,
                               GParamSpec            *pspec)
{
  GooCanvasGroup *group = (GooCanvasGroup*) object;
  GooCanvasGroupPrivate *priv = goo_canvas_group_get_private (group);

  goo_canvas_group_get_common_property (object, priv, prop_id, value, pspec);
}

static void
goo_canvas_group_set_common_property (GObject               *object,
                                      GooCanvasGroupPrivate *priv,
                                      guint                  prop_id,
                                      const GValue          *value,
                                      GParamSpec            *pspec)
{
  switch (prop_id)
    {
    case PROP_X:
      priv->x = g_value_get_double (value);
      break;
    case PROP_Y:
      priv->y = g_value_get_double (value);
      break;
    case PROP_WIDTH:
      priv->width = g_value_get_double (value);
      break;
    case PROP_HEIGHT:
      priv->height = g_value_get_double (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
goo_canvas_group_set_property (GObject                  *object,
                               guint                     prop_id,
                               const GValue             *value,
                               GParamSpec               *pspec)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) object;
  GooCanvasGroup *group = (GooCanvasGroup*) object;
  GooCanvasGroupPrivate *priv = goo_canvas_group_get_private (group);

  if (simple->model)
    {
      g_warning ("Can't set property of a canvas item with a model - set the model property instead");
      return;
    }

  goo_canvas_group_set_common_property (object, priv, prop_id, value, pspec);
  goo_canvas_item_simple_changed (simple, TRUE);
}

static void
goo_canvas_group_add_child     (GooCanvasItem  *item,
				GooCanvasItem  *child,
				gint            position)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) item;
  GooCanvasGroup *group = (GooCanvasGroup*) item;
  AtkObject *atk_obj, *child_atk_obj;

  g_object_ref (child);

  if (position >= 0)
    {
      goo_canvas_util_ptr_array_insert (group->items, child, position);
    }
  else
    {
      position = group->items->len;
      g_ptr_array_add (group->items, child);
    }

  goo_canvas_item_set_parent (child, item);
  goo_canvas_item_set_is_static (child, simple->simple_data->is_static);

  /* Emit the "children_changed" ATK signal, if ATK is enabled. */
  atk_obj = atk_gobject_accessible_for_object (G_OBJECT (item));
  if (!ATK_IS_NO_OP_OBJECT (atk_obj))
    {
      child_atk_obj = atk_gobject_accessible_for_object (G_OBJECT (child));
      g_signal_emit_by_name (atk_obj, "children_changed::add",
			     position, child_atk_obj);
    }

  goo_canvas_item_request_update (item);
}


static void
goo_canvas_group_move_child    (GooCanvasItem  *item,
				gint	        old_position,
				gint            new_position)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) item;
  GooCanvasGroup *group = (GooCanvasGroup*) item;
  GooCanvasItem *child;
  GooCanvasBounds bounds;

  /* Request a redraw of the item's bounds. */
  child = group->items->pdata[old_position];
  if (simple->canvas)
    {
      goo_canvas_item_get_bounds (child, &bounds);
      goo_canvas_request_item_redraw (simple->canvas, &bounds,
				      simple->simple_data->is_static);
    }

  goo_canvas_util_ptr_array_move (group->items, old_position, new_position);

  goo_canvas_item_request_update (item);
}


static void
goo_canvas_group_remove_child  (GooCanvasItem  *item,
				gint            child_num)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) item;
  GooCanvasGroup *group = (GooCanvasGroup*) item;
  GooCanvasItem *child;
  GooCanvasBounds bounds;
  AtkObject *atk_obj, *child_atk_obj;

  g_return_if_fail (child_num < group->items->len);

  /* Request a redraw of the item's bounds. */
  child = group->items->pdata[child_num];
  if (simple->canvas)
    {
      goo_canvas_item_get_bounds (child, &bounds);
      goo_canvas_request_item_redraw (simple->canvas, &bounds,
				      simple->simple_data->is_static);
    }

  /* Emit the "children_changed" ATK signal, if ATK is enabled. */
  atk_obj = atk_gobject_accessible_for_object (G_OBJECT (item));
  if (!ATK_IS_NO_OP_OBJECT (atk_obj))
    {
      child_atk_obj = atk_gobject_accessible_for_object (G_OBJECT (child));
      g_signal_emit_by_name (atk_obj, "children_changed::remove",
			     child_num, child_atk_obj);
    }

  g_ptr_array_remove_index (group->items, child_num);

  goo_canvas_item_set_parent (child, NULL);
  g_object_unref (child);

  goo_canvas_item_request_update (item);
}


static gint
goo_canvas_group_get_n_children (GooCanvasItem  *item)
{
  GooCanvasGroup *group = (GooCanvasGroup*) item;

  return group->items->len;
}


static GooCanvasItem*
goo_canvas_group_get_child   (GooCanvasItem       *item,
			      gint                 child_num)
{
  GooCanvasGroup *group = (GooCanvasGroup*) item;

  if (child_num < group->items->len)
    return group->items->pdata[child_num];
  return NULL;
}


/* This is only used to set the canvas of the root group. It isn't normally
   needed by apps. */
static void
goo_canvas_group_set_canvas  (GooCanvasItem *item,
			      GooCanvas     *canvas)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) item;
  GooCanvasGroup *group = (GooCanvasGroup*) item;
  gint i;

  if (simple->canvas == canvas)
    return;

  simple->canvas = canvas;

  /* Recursively set the canvas of all child items. */
  for (i = 0; i < group->items->len; i++)
    {
      GooCanvasItem *item = group->items->pdata[i];
      goo_canvas_item_set_canvas (item, canvas);
    }
}


static void
goo_canvas_group_set_is_static  (GooCanvasItem *item,
				 gboolean       is_static)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) item;
  GooCanvasItemSimpleData *simple_data = simple->simple_data;
  GooCanvasGroup *group = (GooCanvasGroup*) item;
  gint i;

  if (simple_data->is_static == is_static)
    return;

  simple_data->is_static = is_static;

  /* Recursively set the canvas of all child items. */
  for (i = 0; i < group->items->len; i++)
    {
      GooCanvasItem *item = group->items->pdata[i];
      goo_canvas_item_set_is_static (item, is_static);
    }
}


static void
on_model_child_added (GooCanvasGroupModel *model,
		      gint                 position,
		      GooCanvasGroup      *group)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) group;
  GooCanvasItem *item = (GooCanvasItem*) group;
  GooCanvasItemModel *child_model;
  GooCanvasItem *child;

  /* Create a canvas item for the model. */
  child_model = goo_canvas_item_model_get_child ((GooCanvasItemModel*) model,
						 position);
  child = goo_canvas_create_item (simple->canvas, child_model);
  goo_canvas_item_add_child (item, child, position);
  g_object_unref (child);
}


static void
on_model_child_moved (GooCanvasGroupModel *model,
		      gint                 old_position,
		      gint                 new_position,
		      GooCanvasGroup      *group)
{
  goo_canvas_item_move_child ((GooCanvasItem*) group, old_position,
			       new_position);
}


static void
on_model_child_removed (GooCanvasGroupModel *model,
			gint                 child_num,
			GooCanvasGroup      *group)
{
  goo_canvas_item_remove_child ((GooCanvasItem*) group, child_num);
}


static void
goo_canvas_group_set_model (GooCanvasItem       *item,
			    GooCanvasItemModel  *model)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) item;
  GooCanvasGroup *group = (GooCanvasGroup*) item;
  gint n_children, i;

  /* Do the default GooCanvasItemSimple code first. */
  goo_canvas_item_simple_set_model (simple, model);

  /* Now add our own handlers. */
  g_signal_connect (model, "child-added",
		    G_CALLBACK (on_model_child_added), group);
  g_signal_connect (model, "child-moved",
		    G_CALLBACK (on_model_child_moved), group);
  g_signal_connect (model, "child-removed",
		    G_CALLBACK (on_model_child_removed), group);

  /* Recursively create child items for any children. */
  n_children = goo_canvas_item_model_get_n_children (model);
  for (i = 0; i < n_children; i++)
    on_model_child_added ((GooCanvasGroupModel*) simple->model, i, group);
}


static void
goo_canvas_group_request_update  (GooCanvasItem *item)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) item;

  if (!simple->need_update)
    {
      simple->need_update = TRUE;

      if (simple->parent)
	goo_canvas_item_request_update (simple->parent);
      else if (simple->canvas)
	goo_canvas_request_update (simple->canvas);
    }
}


static void
goo_canvas_group_update  (GooCanvasItem   *item,
			  gboolean         entire_tree,
			  cairo_t         *cr,
			  GooCanvasBounds *bounds)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) item;
  GooCanvasGroup *group = (GooCanvasGroup*) item;
  GooCanvasGroupPrivate *priv = goo_canvas_group_get_private (group);
  GooCanvasBounds child_bounds;
  gboolean initial_bounds = TRUE;
  gint i;

  if (entire_tree || simple->need_update)
    {
      if (simple->need_entire_subtree_update)
	entire_tree = TRUE;

      simple->need_update = FALSE;
      simple->need_entire_subtree_update = FALSE;

      goo_canvas_item_simple_check_style (simple);

      simple->bounds.x1 = simple->bounds.y1 = 0.0;
      simple->bounds.x2 = simple->bounds.y2 = 0.0;

      cairo_save (cr);
      if (simple->simple_data->transform)
        cairo_transform (cr, simple->simple_data->transform);

      cairo_translate (cr, priv->x, priv->y);

      for (i = 0; i < group->items->len; i++)
        {
          GooCanvasItem *child = group->items->pdata[i];

          goo_canvas_item_update (child, entire_tree, cr, &child_bounds);
          
          /* If the child has non-empty bounds, compute the union. */
          if (child_bounds.x1 < child_bounds.x2
              && child_bounds.y1 < child_bounds.y2)
            {
              if (initial_bounds)
                {
                  simple->bounds.x1 = child_bounds.x1;
                  simple->bounds.y1 = child_bounds.y1;
                  simple->bounds.x2 = child_bounds.x2;
                  simple->bounds.y2 = child_bounds.y2;
                  initial_bounds = FALSE;
                }
              else
                {
                  simple->bounds.x1 = MIN (simple->bounds.x1, child_bounds.x1);
                  simple->bounds.y1 = MIN (simple->bounds.y1, child_bounds.y1);
                  simple->bounds.x2 = MAX (simple->bounds.x2, child_bounds.x2);
                  simple->bounds.y2 = MAX (simple->bounds.y2, child_bounds.y2);
                }
            }
        }

      cairo_restore (cr);
    }

  *bounds = simple->bounds;
}


static GList*
goo_canvas_group_get_items_at (GooCanvasItem  *item,
			       gdouble         x,
			       gdouble         y,
			       cairo_t        *cr,
			       gboolean        is_pointer_event,
			       gboolean        parent_visible,
			       GList          *found_items)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) item;
  GooCanvasItemSimpleData *simple_data = simple->simple_data;
  GooCanvasGroup *group = (GooCanvasGroup*) item;
  GooCanvasGroupPrivate *priv = goo_canvas_group_get_private (group);
  gboolean visible = parent_visible;
  int i;

  if (simple->need_update)
    goo_canvas_item_ensure_updated (item);

  /* Skip the item if the point isn't in the item's bounds. */
  if (simple->bounds.x1 > x || simple->bounds.x2 < x
      || simple->bounds.y1 > y || simple->bounds.y2 < y)
    return found_items;

  if (simple_data->visibility <= GOO_CANVAS_ITEM_INVISIBLE
      || (simple_data->visibility == GOO_CANVAS_ITEM_VISIBLE_ABOVE_THRESHOLD
	  && simple->canvas->scale < simple_data->visibility_threshold))
    visible = FALSE;

  /* Check if the group should receive events. */
  if (is_pointer_event
      && (simple_data->pointer_events == GOO_CANVAS_EVENTS_NONE
	  || ((simple_data->pointer_events & GOO_CANVAS_EVENTS_VISIBLE_MASK)
	      && !visible)))
    return found_items;

  cairo_save (cr);
  if (simple_data->transform)
    cairo_transform (cr, simple_data->transform);

  cairo_translate (cr, priv->x, priv->y);

  /* If the group has a clip path, check if the point is inside it. */
  if (simple_data->clip_path_commands)
    {
      double user_x = x, user_y = y;

      cairo_device_to_user (cr, &user_x, &user_y);
      goo_canvas_create_path (simple_data->clip_path_commands, cr);
      cairo_set_fill_rule (cr, simple_data->clip_fill_rule);
      if (!cairo_in_fill (cr, user_x, user_y))
	{
	  cairo_restore (cr);
	  return found_items;
	}
    }

  if (priv->width > 0.0 && priv->height > 0.0)
    {
      double user_x = x, user_y = y;

      cairo_device_to_user (cr, &user_x, &user_y);
      if (user_x < 0.0 || user_x >= priv->width
	  || user_y < 0.0 || user_y >= priv->height)
	{
	  cairo_restore (cr);
	  return found_items;
	}
    }

  /* Step up from the bottom of the children to the top, adding any items
     found to the start of the list. */
  for (i = 0; i < group->items->len; i++)
    {
      GooCanvasItem *child = group->items->pdata[i];

      found_items = goo_canvas_item_get_items_at (child, x, y, cr,
						  is_pointer_event, visible,
						  found_items);
    }
  cairo_restore (cr);

  return found_items;
}


static void
goo_canvas_group_paint (GooCanvasItem         *item,
			cairo_t               *cr,
			const GooCanvasBounds *bounds,
			gdouble                scale)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) item;
  GooCanvasItemSimpleData *simple_data = simple->simple_data;
  GooCanvasGroup *group = (GooCanvasGroup*) item;
  GooCanvasGroupPrivate *priv = goo_canvas_group_get_private (group);
  gint i;

  /* Skip the item if the bounds don't intersect the expose rectangle. */
  if (simple->bounds.x1 > bounds->x2 || simple->bounds.x2 < bounds->x1
      || simple->bounds.y1 > bounds->y2 || simple->bounds.y2 < bounds->y1)
    return;

  /* Check if the item should be visible. */
  if (simple_data->visibility <= GOO_CANVAS_ITEM_INVISIBLE
      || (simple_data->visibility == GOO_CANVAS_ITEM_VISIBLE_ABOVE_THRESHOLD
	  && simple->canvas->scale < simple_data->visibility_threshold))
    return;

  /* Paint all the items in the group. */
  cairo_save (cr);
  if (simple_data->transform)
    cairo_transform (cr, simple_data->transform);

  cairo_translate (cr, priv->x, priv->y);

  /* Clip with the group's clip path, if it is set. */
  if (simple_data->clip_path_commands)
    {
      goo_canvas_create_path (simple_data->clip_path_commands, cr);
      cairo_set_fill_rule (cr, simple_data->clip_fill_rule);
      cairo_clip (cr);
    }

  if (priv->width > 0.0 && priv->height > 0.0)
    {
      cairo_rectangle (cr, 0.0, 0.0, priv->width, priv->height);
      cairo_clip (cr);
    }

  for (i = 0; i < group->items->len; i++)
    {
      GooCanvasItem *child = group->items->pdata[i];
      goo_canvas_item_paint (child, cr, bounds, scale);
    }
  cairo_restore (cr);
}


static void
canvas_item_interface_init (GooCanvasItemIface *iface)
{
  iface->set_canvas     = goo_canvas_group_set_canvas;
  iface->get_n_children = goo_canvas_group_get_n_children;
  iface->get_child      = goo_canvas_group_get_child;
  iface->request_update = goo_canvas_group_request_update;

  iface->add_child      = goo_canvas_group_add_child;
  iface->move_child     = goo_canvas_group_move_child;
  iface->remove_child   = goo_canvas_group_remove_child;

  iface->get_items_at	= goo_canvas_group_get_items_at;
  iface->update         = goo_canvas_group_update;
  iface->paint          = goo_canvas_group_paint;

  iface->set_model      = goo_canvas_group_set_model;
  iface->set_is_static  = goo_canvas_group_set_is_static;
}


/**
 * SECTION:goocanvasgroupmodel
 * @Title: GooCanvasGroupModel
 * @Short_Description: a model for a group of items.
 *
 * #GooCanvasGroupModel represents a group of items. Groups can be nested to
 * any depth, to create a hierarchy of items. Items are ordered within each
 * group, with later items being displayed above earlier items.
 *
 * #GooCanvasGroupModel is a subclass of #GooCanvasItemModelSimple and so
 * inherits all of the style properties such as "stroke-color", "fill-color"
 * and "line-width". Setting a style property on a #GooCanvasGroupModel will
 * affect all children of the #GooCanvasGroupModel (unless the children
 * override the property setting).
 *
 * #GooCanvasGroupModel implements the #GooCanvasItemModel interface, so you
 * can use the #GooCanvasItemModel functions such as
 * goo_canvas_item_model_raise() and goo_canvas_item_model_rotate(), and the
 * properties such as "visibility" and "pointer-events".
 *
 * To create a #GooCanvasGroupModel use goo_canvas_group_model_new().
 *
 * To get or set the properties of an existing #GooCanvasGroupModel, use
 * g_object_get() and g_object_set().
 *
 * To respond to events such as mouse clicks on the group you must connect
 * to the signal handlers of the corresponding #GooCanvasGroup objects.
 * (See goo_canvas_get_item() and #GooCanvas::item-created.)
 */
static void item_model_interface_init (GooCanvasItemModelIface *iface);
static void goo_canvas_group_model_dispose      (GObject            *object);
static void goo_canvas_group_model_finalize     (GObject            *object);
static void goo_canvas_group_model_get_property (GObject            *object,
                                                 guint               prop_id,
                                                 GValue             *value,
                                                 GParamSpec         *pspec);
static void goo_canvas_group_model_set_property (GObject            *object,
                                                 guint               prop_id,
                                                 const GValue       *value,
                                                 GParamSpec         *pspec);

G_DEFINE_TYPE_WITH_CODE (GooCanvasGroupModel, goo_canvas_group_model,
			 GOO_TYPE_CANVAS_ITEM_MODEL_SIMPLE,
			 G_IMPLEMENT_INTERFACE (GOO_TYPE_CANVAS_ITEM_MODEL,
						item_model_interface_init))


static void
goo_canvas_group_model_class_init (GooCanvasGroupModelClass *klass)
{
  GObjectClass *gobject_class = (GObjectClass*) klass;
  g_type_class_add_private (gobject_class, sizeof (GooCanvasGroupPrivate));

  gobject_class->dispose  = goo_canvas_group_model_dispose;
  gobject_class->finalize = goo_canvas_group_model_finalize;
  gobject_class->get_property  = goo_canvas_group_model_get_property;
  gobject_class->set_property = goo_canvas_group_model_set_property;

  goo_canvas_group_install_common_properties (gobject_class);
}


static void
goo_canvas_group_model_init (GooCanvasGroupModel *gmodel)
{
  GooCanvasGroupPrivate *priv = GOO_CANVAS_GROUP_MODEL_GET_PRIVATE (gmodel);
  gmodel->children = g_ptr_array_sized_new (8);

  priv->x = 0.0;
  priv->y = 0.0;
  priv->width = -1.0;
  priv->height = -1.0;
}


/**
 * goo_canvas_group_model_new:
 * @parent: the parent model, or %NULL. If a parent is specified, it will
 *  assume ownership of the item, and the item will automatically be freed when
 *  it is removed from the parent. Otherwise call g_object_unref() to free it.
 * @...: optional pairs of property names and values, and a terminating %NULL.
 * 
 * Creates a new group item.
 * 
 * Return value: a new group model.
 **/
GooCanvasItemModel*
goo_canvas_group_model_new (GooCanvasItemModel *parent,
			    ...)
{
  GooCanvasItemModel *model;
  GooCanvasGroupModel *gmodel;
  va_list var_args;
  const char *first_property;

  model = g_object_new (GOO_TYPE_CANVAS_GROUP_MODEL, NULL);
  gmodel = (GooCanvasGroupModel*) model;

  va_start (var_args, parent);
  first_property = va_arg (var_args, char*);
  if (first_property)
    g_object_set_valist (G_OBJECT (model), first_property, var_args);
  va_end (var_args);

  if (parent)
    {
      goo_canvas_item_model_add_child (parent, model, -1);
      g_object_unref (model);
    }

  return model;
}


static void
goo_canvas_group_model_dispose (GObject *object)
{
  GooCanvasGroupModel *gmodel = (GooCanvasGroupModel*) object;
  gint i;

  /* Unref all the items in the group. */
  for (i = 0; i < gmodel->children->len; i++)
    {
      GooCanvasItemModel *child = gmodel->children->pdata[i];
      goo_canvas_item_model_set_parent (child, NULL);
      g_object_unref (child);
    }

  g_ptr_array_set_size (gmodel->children, 0);

  G_OBJECT_CLASS (goo_canvas_group_model_parent_class)->dispose (object);
}


static void
goo_canvas_group_model_finalize (GObject *object)
{
  GooCanvasGroupModel *gmodel = (GooCanvasGroupModel*) object;

  g_ptr_array_free (gmodel->children, TRUE);

  G_OBJECT_CLASS (goo_canvas_group_model_parent_class)->finalize (object);
}

static void goo_canvas_group_model_get_property (GObject            *object,
                                                 guint               prop_id,
                                                 GValue             *value,
                                                 GParamSpec         *pspec)
{
  GooCanvasGroupModel *model = (GooCanvasGroupModel*) object;
  GooCanvasGroupPrivate *priv = GOO_CANVAS_GROUP_MODEL_GET_PRIVATE (model);

  goo_canvas_group_get_common_property (object, priv, prop_id, value, pspec);
}

static void goo_canvas_group_model_set_property (GObject            *object,
                                                 guint               prop_id,
                                                 const GValue       *value,
                                                 GParamSpec         *pspec)
{
  GooCanvasGroupModel *model = (GooCanvasGroupModel*) object;
  GooCanvasGroupPrivate *priv = GOO_CANVAS_GROUP_MODEL_GET_PRIVATE (model);

  goo_canvas_group_set_common_property (object, priv, prop_id, value, pspec);
  g_signal_emit_by_name (model, "changed", TRUE);
}

extern void _goo_canvas_item_model_emit_child_added (GooCanvasItemModel *model,
						     gint                position);

static void
goo_canvas_group_model_add_child     (GooCanvasItemModel *model,
				      GooCanvasItemModel *child,
				      gint                position)
{
  GooCanvasGroupModel *gmodel = (GooCanvasGroupModel*) model;

  g_object_ref (child);

  if (position >= 0)
    {
      goo_canvas_util_ptr_array_insert (gmodel->children, child, position);
    }
  else
    {
      position = gmodel->children->len;
      g_ptr_array_add (gmodel->children, child);
    }

  goo_canvas_item_model_set_parent (child, model);

  _goo_canvas_item_model_emit_child_added (model, position);
}


static void
goo_canvas_group_model_move_child    (GooCanvasItemModel *model,
				      gint	          old_position,
				      gint                new_position)
{
  GooCanvasGroupModel *gmodel = (GooCanvasGroupModel*) model;

  goo_canvas_util_ptr_array_move (gmodel->children, old_position,
				  new_position);

  g_signal_emit_by_name (gmodel, "child-moved", old_position, new_position);
}


static void
goo_canvas_group_model_remove_child  (GooCanvasItemModel *model,
				      gint                child_num)
{
  GooCanvasGroupModel *gmodel = (GooCanvasGroupModel*) model;
  GooCanvasItemModel *child;

  child = gmodel->children->pdata[child_num];
  goo_canvas_item_model_set_parent (child, NULL);

  g_ptr_array_remove_index (gmodel->children, child_num);

  g_signal_emit_by_name (gmodel, "child-removed", child_num);

  g_object_unref (child);
}


static gint
goo_canvas_group_model_get_n_children (GooCanvasItemModel  *model)
{
  GooCanvasGroupModel *gmodel = (GooCanvasGroupModel*) model;

  return gmodel->children->len;
}


static GooCanvasItemModel*
goo_canvas_group_model_get_child   (GooCanvasItemModel  *model,
				    gint                 child_num)
{
  GooCanvasGroupModel *gmodel = (GooCanvasGroupModel*) model;

  if (child_num < gmodel->children->len)
    return gmodel->children->pdata[child_num];
  return NULL;
}


static GooCanvasItem*
goo_canvas_group_model_create_item (GooCanvasItemModel *model,
				    GooCanvas          *canvas)
{
  GooCanvasItem *item;

  item = goo_canvas_group_new (NULL, NULL);
  /* Note that we set the canvas before the model, since we may need the
     canvas to create any child items. */
  goo_canvas_item_set_canvas (item, canvas);
  goo_canvas_item_set_model (item, model);

  return item;
}


static void
item_model_interface_init (GooCanvasItemModelIface *iface)
{
  iface->add_child      = goo_canvas_group_model_add_child;
  iface->move_child     = goo_canvas_group_model_move_child;
  iface->remove_child   = goo_canvas_group_model_remove_child;
  iface->get_n_children = goo_canvas_group_model_get_n_children;
  iface->get_child      = goo_canvas_group_model_get_child;

  iface->create_item    = goo_canvas_group_model_create_item;
}


