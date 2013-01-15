/*
 * GooCanvas Demo. Copyright (C) 2007 Damon Chaplin.
 * Released under the GNU LGPL license. See COPYING for details.
 *
 * demo-large-line.h - a demo item that exceeds the cairo 16-bit size limit.
 *                     Note that it doesn't support miters.
 */
#ifndef __GOO_DEMO_LARGE_LINE_H__
#define __GOO_DEMO_LARGE_LINE_H__

#include <gtk/gtk.h>
#include "goocanvasitemsimple.h"

G_BEGIN_DECLS


#define GOO_TYPE_DEMO_LARGE_LINE            (goo_demo_large_line_get_type ())
#define GOO_DEMO_LARGE_LINE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOO_TYPE_DEMO_LARGE_LINE, GooDemoLargeLine))
#define GOO_DEMO_LARGE_LINE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GOO_TYPE_DEMO_LARGE_LINE, GooDemoLargeLineClass))
#define GOO_IS_DEMO_LARGE_LINE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOO_TYPE_DEMO_LARGE_LINE))
#define GOO_IS_DEMO_LARGE_LINE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GOO_TYPE_DEMO_LARGE_LINE))
#define GOO_DEMO_LARGE_LINE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GOO_TYPE_DEMO_LARGE_LINE, GooDemoLargeLineClass))


typedef struct _GooDemoLargeLine       GooDemoLargeLine;
typedef struct _GooDemoLargeLineClass  GooDemoLargeLineClass;

struct _GooDemoLargeLine
{
  GooCanvasItemSimple parent_object;

  gdouble x1, y1, x2, y2;
};

struct _GooDemoLargeLineClass
{
  GooCanvasItemSimpleClass parent_class;
};


GType               goo_demo_large_line_get_type  (void) G_GNUC_CONST;

GooCanvasItem*      goo_demo_large_line_new       (GooCanvasItem      *parent,
						   gdouble             x1,
						   gdouble             y1,
						   gdouble             x2,
						   gdouble             y2,
						   ...);


G_END_DECLS

#endif /* __GOO_DEMO_LARGE_LINE_H__ */
