/*
 * GooCanvas. Copyright (C) 2005-8 Damon Chaplin.
 * Released under the GNU LGPL license. See COPYING for details.
 *
 * goocanvasgrid.h - a grid item.
 */
#ifndef __GOO_CANVAS_GRID_H__
#define __GOO_CANVAS_GRID_H__

#include <gtk/gtk.h>
#include "goocanvasitemsimple.h"

G_BEGIN_DECLS


/* This is the data used by both model and view classes. */
typedef struct _GooCanvasGridData   GooCanvasGridData;
struct _GooCanvasGridData
{
  /* The area that the grid covers. */
  gdouble x, y, width, height;

  /* The distance between grid lines. */
  gdouble x_step, y_step;

  /* The offset before the first grid line. */
  gdouble x_offset, y_offset;

  /* The widths of the grid lines, or -ve to use item's stroke width. */
  gdouble horz_grid_line_width, vert_grid_line_width;

  /* The color/pattern for the grid lines, or NULL to use the stroke color. */
  cairo_pattern_t *horz_grid_line_pattern, *vert_grid_line_pattern;

  /* The width of the border around the grid, or -1 for no border. */
  gdouble border_width;

  /* The color/pattern for the border, or NULL to use the stroke color. */
  cairo_pattern_t *border_pattern;

  /* If the horizontal and vertical grid lines should be shown. */
  guint show_horz_grid_lines : 1;
  guint show_vert_grid_lines : 1;

  /* If vertical grid lines are drawn on top. */
  guint vert_grid_lines_on_top : 1;
};


#define GOO_TYPE_CANVAS_GRID            (goo_canvas_grid_get_type ())
#define GOO_CANVAS_GRID(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOO_TYPE_CANVAS_GRID, GooCanvasGrid))
#define GOO_CANVAS_GRID_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GOO_TYPE_CANVAS_GRID, GooCanvasGridClass))
#define GOO_IS_CANVAS_GRID(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOO_TYPE_CANVAS_GRID))
#define GOO_IS_CANVAS_GRID_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GOO_TYPE_CANVAS_GRID))
#define GOO_CANVAS_GRID_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GOO_TYPE_CANVAS_GRID, GooCanvasGridClass))


typedef struct _GooCanvasGrid       GooCanvasGrid;
typedef struct _GooCanvasGridClass  GooCanvasGridClass;

/**
 * GooCanvasGrid
 *
 * The #GooCanvasGrid-struct struct contains private data only.
 */
struct _GooCanvasGrid
{
  GooCanvasItemSimple parent_object;

  GooCanvasGridData *grid_data;
};

struct _GooCanvasGridClass
{
  GooCanvasItemSimpleClass parent_class;

  /*< private >*/

  /* Padding for future expansion */
  void (*_goo_canvas_reserved1) (void);
  void (*_goo_canvas_reserved2) (void);
  void (*_goo_canvas_reserved3) (void);
  void (*_goo_canvas_reserved4) (void);
};


GType          goo_canvas_grid_get_type      (void) G_GNUC_CONST;
GooCanvasItem* goo_canvas_grid_new           (GooCanvasItem      *parent,
					      gdouble             x,
					      gdouble             y,
					      gdouble             width,
					      gdouble             height,
					      gdouble             x_step,
					      gdouble             y_step,
					      gdouble             x_offset,
					      gdouble             y_offset,
					      ...);



#define GOO_TYPE_CANVAS_GRID_MODEL            (goo_canvas_grid_model_get_type ())
#define GOO_CANVAS_GRID_MODEL(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOO_TYPE_CANVAS_GRID_MODEL, GooCanvasGridModel))
#define GOO_CANVAS_GRID_MODEL_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GOO_TYPE_CANVAS_GRID_MODEL, GooCanvasGridModelClass))
#define GOO_IS_CANVAS_GRID_MODEL(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOO_TYPE_CANVAS_GRID_MODEL))
#define GOO_IS_CANVAS_GRID_MODEL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GOO_TYPE_CANVAS_GRID_MODEL))
#define GOO_CANVAS_GRID_MODEL_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GOO_TYPE_CANVAS_GRID_MODEL, GooCanvasGridModelClass))


typedef struct _GooCanvasGridModel       GooCanvasGridModel;
typedef struct _GooCanvasGridModelClass  GooCanvasGridModelClass;

/**
 * GooCanvasGridModel
 *
 * The #GooCanvasGridModel-struct struct contains private data only.
 */
struct _GooCanvasGridModel
{
  GooCanvasItemModelSimple parent_object;

  GooCanvasGridData grid_data;
};

struct _GooCanvasGridModelClass
{
  GooCanvasItemModelSimpleClass parent_class;

  /*< private >*/

  /* Padding for future expansion */
  void (*_goo_canvas_reserved1) (void);
  void (*_goo_canvas_reserved2) (void);
  void (*_goo_canvas_reserved3) (void);
  void (*_goo_canvas_reserved4) (void);
};


GType               goo_canvas_grid_model_get_type (void) G_GNUC_CONST;
GooCanvasItemModel* goo_canvas_grid_model_new      (GooCanvasItemModel *parent,
						    gdouble             x,
						    gdouble             y,
						    gdouble             width,
						    gdouble             height,
						    gdouble             x_step,
						    gdouble             y_step,
						    gdouble             x_offset,
						    gdouble             y_offset,
						    ...);


G_END_DECLS

#endif /* __GOO_CANVAS_GRID_H__ */
