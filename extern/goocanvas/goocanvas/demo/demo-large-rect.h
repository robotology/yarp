/*
 * GooCanvas Demo. Copyright (C) 2007 Damon Chaplin.
 * Released under the GNU LGPL license. See COPYING for details.
 *
 * demo-large-rect.h - a demo item that exceeds the cairo 16-bit size limit.
 */
#ifndef __GOO_DEMO_LARGE_RECT_H__
#define __GOO_DEMO_LARGE_RECT_H__

#include <gtk/gtk.h>
#include "goocanvasitemsimple.h"

G_BEGIN_DECLS


#define GOO_TYPE_DEMO_LARGE_RECT            (goo_demo_large_rect_get_type ())
#define GOO_DEMO_LARGE_RECT(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOO_TYPE_DEMO_LARGE_RECT, GooDemoLargeRect))
#define GOO_DEMO_LARGE_RECT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GOO_TYPE_DEMO_LARGE_RECT, GooDemoLargeRectClass))
#define GOO_IS_DEMO_LARGE_RECT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOO_TYPE_DEMO_LARGE_RECT))
#define GOO_IS_DEMO_LARGE_RECT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GOO_TYPE_DEMO_LARGE_RECT))
#define GOO_DEMO_LARGE_RECT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GOO_TYPE_DEMO_LARGE_RECT, GooDemoLargeRectClass))


typedef struct _GooDemoLargeRect       GooDemoLargeRect;
typedef struct _GooDemoLargeRectClass  GooDemoLargeRectClass;

struct _GooDemoLargeRect
{
  GooCanvasItemSimple parent_object;

  gdouble x, y, width, height;
};

struct _GooDemoLargeRectClass
{
  GooCanvasItemSimpleClass parent_class;
};


GType               goo_demo_large_rect_get_type  (void) G_GNUC_CONST;

GooCanvasItem*      goo_demo_large_rect_new       (GooCanvasItem      *parent,
						   gdouble             x,
						   gdouble             y,
						   gdouble             width,
						   gdouble             height,
						   ...);


G_END_DECLS

#endif /* __GOO_DEMO_LARGE_RECT_H__ */
