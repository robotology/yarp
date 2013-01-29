_EQUAL(glong,long)

_CONV_ENUM(GooCanvas,PointerEvents)
_CONV_ENUM(GooCanvas,StyleValuesMask)
_CONV_ENUM(GooCanvas,AnimateType)

_CONVERSION(`Bounds&',`GooCanvasBounds*',__FR2P)
_CONVERSION(`const Bounds&',`GooCanvasBounds*',__FCR2P))
_CONVERSION(`Goocanvas::Bounds&',`GooCanvasBounds*',($3).gobj())
_CONVERSION(`GooCanvasBounds*',`Goocanvas::Bounds*',`&Glib::wrap($3)')
_CONVERSION(`GooCanvasBounds*',`Bounds&',`Glib::wrap($3)')
_CONVERSION(`const Bounds*',`const GooCanvasBounds*',($3).gobj())
_CONVERSION(`const Bounds&',`const GooCanvasBounds*',($3).gobj())

_CONVERSION(`GooCanvas::StyleValuesMask',`gint',`$3')
_CONVERSION(`gint',`GooCanvas::StyleValuesMask',`static_cast<GooCanvas::StyleValuesMask($3)')

_CONVERSION(`GtkWidget*',`Canvas*',`Glib::Wrap((GtkCanvas*)($3))')
_CONVERSION(`GooCanvas*',`Canvas*',`Glib::wrap($3)')
_CONVERSION(`Canvas*',`GooCanvas*',`($3)->gobj()')
#_CONVERSION(`GooCanvas*',`Canvas*',__RP2P)
#_CONVERSION(`GooCanvas*',`const Canvas*',__RP2P)
#_CONVERSION(`Canvas&',`GooCanvas*',__FR2PD)

_CONVERSION(`GooCanvasItem*',`Glib::RefPtr<Item>',`Glib::wrap($3)')
_CONVERSION(`GooCanvasItemModel*',`Glib::RefPtr<ItemModel>',`Glib::wrap($3)')
_CONVERSION(`GooCanvasItemSimple',`Glib::RefPtr<ItemSimple>',`Glib::wrap($3)')
_CONVERSION(`GooCanvasStyle*',`Glib::RefPtr<Style>',`Glib::wrap($3)')
_CONVERSION(`GooCanvasStyle*',`Glib::RefPtr<const Style>',`Glib::wrap($3)')

_CONVERSION(`const Glib::RefPtr<Item>&',`GooCanvasItem*',__CONVERT_REFPTR_TO_P)
_CONVERSION(`Glib::RefPtr<Item>',`GooCanvasItem*',__CONVERT_REFPTR_TO_P)
_CONVERSION(`const Glib::RefPtr<ItemModel>&',`GooCanvasItemModel*',__CONVERT_REFPTR_TO_P)
_CONVERSION(`const Glib::RefPtr<ItemSimple>&',`GooCanvasItemSimple*',__CONVERT_REFPTR_TO_P)
_CONVERSION(`const Glib::RefPtr<Style>&',`GooCanvasStyle*',__CONVERT_REFPTR_TO_P)
_CONVERSION(`const Glib::RefPtr<Goocanvas::Widget>&',`GooCanvasWidget*',__CONVERT_REFPTR_TO_P)

_CONVERSION(`const Cairo::RefPtr<Cairo::Context>&',`cairo_t*',`($3)->cobj()')
_CONVERSION(`cairo_t*',`Cairo::RefPtr<Cairo::Context>',`Cairo::RefPtr<Cairo::Context>(new Cairo::Context($3))')
_CONVERSION(`cairo_matrix_t*',`Cairo::Matrix*',`((Cairo::Matrix*)($3))')
_CONVERSION(`Cairo::Matrix*',`cairo_matrix_t*',`((cairo_matrix_t*)($3))')
_CONVERSION(`const cairo_matrix_t*',`const Cairo::Matrix*',`((const Cairo::Matrix*)($3))')
_CONVERSION(`const Cairo::Matrix*',`const cairo_matrix_t*',`((const cairo_matrix_t*)($3))')

# GooCairoPattern* is actually a cairo_pattern_t*:
_CONVERSION(`Cairo::RefPtr<Cairo::Pattern>', `GooCairoPattern*',__CONVERT_REFPTR_TO_P)

_CONVERSION(`Gtk::AnchorType',`AnchorType',`($2)($3)')

