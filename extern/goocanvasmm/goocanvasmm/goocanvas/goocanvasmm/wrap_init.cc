
#include <glib.h>

// Disable the 'const' function attribute of the get_type() functions.
// GCC would optimize them out because we don't use the return value.
#undef  G_GNUC_CONST
#define G_GNUC_CONST /* empty */

#include <goocanvasmm/wrap_init.h>
#include <glibmm/error.h>
#include <glibmm/object.h>

// #include the widget headers so that we can call the get_type() static methods:

#include "canvas.h"
#include "bounds.h"
#include "ellipse.h"
#include "ellipsemodel.h"
#include "grid.h"
#include "gridmodel.h"
#include "group.h"
#include "groupmodel.h"
#include "image.h"
#include "imagemodel.h"
#include "item.h"
#include "itemmodel.h"
#include "itemmodelsimple.h"
#include "itemsimple.h"
#include "linedash.h"
#include "points.h"
#include "polyline.h"
#include "polylinemodel.h"
#include "path.h"
#include "pathmodel.h"
#include "rect.h"
#include "rectmodel.h"
#include "style.h"
#include "table.h"
#include "tablemodel.h"
#include "text.h"
#include "textmodel.h"
#include "widget.h"
#include "enums.h"

extern "C"
{

//Declarations of the *_get_type() functions:

GType goo_canvas_get_type(void);
GType goo_canvas_ellipse_get_type(void);
GType goo_canvas_ellipse_model_get_type(void);
GType goo_canvas_grid_get_type(void);
GType goo_canvas_grid_model_get_type(void);
GType goo_canvas_group_get_type(void);
GType goo_canvas_group_model_get_type(void);
GType goo_canvas_image_get_type(void);
GType goo_canvas_image_model_get_type(void);
GType goo_canvas_item_model_simple_get_type(void);
GType goo_canvas_item_simple_get_type(void);
GType goo_canvas_path_get_type(void);
GType goo_canvas_path_model_get_type(void);
GType goo_canvas_polyline_get_type(void);
GType goo_canvas_polyline_model_get_type(void);
GType goo_canvas_rect_get_type(void);
GType goo_canvas_rect_model_get_type(void);
GType goo_canvas_style_get_type(void);
GType goo_canvas_table_get_type(void);
GType goo_canvas_table_model_get_type(void);
GType goo_canvas_text_get_type(void);
GType goo_canvas_text_model_get_type(void);
GType goo_canvas_widget_get_type(void);

//Declarations of the *_error_quark() functions:

} // extern "C"


//Declarations of the *_Class::wrap_new() methods, instead of including all the private headers:

namespace Goocanvas {  class Canvas_Class { public: static Glib::ObjectBase* wrap_new(GObject*); };  }
namespace Goocanvas {  class Ellipse_Class { public: static Glib::ObjectBase* wrap_new(GObject*); };  }
namespace Goocanvas {  class EllipseModel_Class { public: static Glib::ObjectBase* wrap_new(GObject*); };  }
namespace Goocanvas {  class Grid_Class { public: static Glib::ObjectBase* wrap_new(GObject*); };  }
namespace Goocanvas {  class GridModel_Class { public: static Glib::ObjectBase* wrap_new(GObject*); };  }
namespace Goocanvas {  class Group_Class { public: static Glib::ObjectBase* wrap_new(GObject*); };  }
namespace Goocanvas {  class GroupModel_Class { public: static Glib::ObjectBase* wrap_new(GObject*); };  }
namespace Goocanvas {  class Image_Class { public: static Glib::ObjectBase* wrap_new(GObject*); };  }
namespace Goocanvas {  class ImageModel_Class { public: static Glib::ObjectBase* wrap_new(GObject*); };  }
namespace Goocanvas {  class ItemModelSimple_Class { public: static Glib::ObjectBase* wrap_new(GObject*); };  }
namespace Goocanvas {  class ItemSimple_Class { public: static Glib::ObjectBase* wrap_new(GObject*); };  }
namespace Goocanvas {  class Path_Class { public: static Glib::ObjectBase* wrap_new(GObject*); };  }
namespace Goocanvas {  class PathModel_Class { public: static Glib::ObjectBase* wrap_new(GObject*); };  }
namespace Goocanvas {  class Polyline_Class { public: static Glib::ObjectBase* wrap_new(GObject*); };  }
namespace Goocanvas {  class PolylineModel_Class { public: static Glib::ObjectBase* wrap_new(GObject*); };  }
namespace Goocanvas {  class Rect_Class { public: static Glib::ObjectBase* wrap_new(GObject*); };  }
namespace Goocanvas {  class RectModel_Class { public: static Glib::ObjectBase* wrap_new(GObject*); };  }
namespace Goocanvas {  class Style_Class { public: static Glib::ObjectBase* wrap_new(GObject*); };  }
namespace Goocanvas {  class Table_Class { public: static Glib::ObjectBase* wrap_new(GObject*); };  }
namespace Goocanvas {  class TableModel_Class { public: static Glib::ObjectBase* wrap_new(GObject*); };  }
namespace Goocanvas {  class Text_Class { public: static Glib::ObjectBase* wrap_new(GObject*); };  }
namespace Goocanvas {  class TextModel_Class { public: static Glib::ObjectBase* wrap_new(GObject*); };  }
namespace Goocanvas {  class Widget_Class { public: static Glib::ObjectBase* wrap_new(GObject*); };  }

namespace Goocanvas { 

void wrap_init()
{
  // Register Error domains:

// Map gtypes to gtkmm wrapper-creation functions:
  Glib::wrap_register(goo_canvas_get_type(), &Goocanvas::Canvas_Class::wrap_new);
  Glib::wrap_register(goo_canvas_ellipse_get_type(), &Goocanvas::Ellipse_Class::wrap_new);
  Glib::wrap_register(goo_canvas_ellipse_model_get_type(), &Goocanvas::EllipseModel_Class::wrap_new);
  Glib::wrap_register(goo_canvas_grid_get_type(), &Goocanvas::Grid_Class::wrap_new);
  Glib::wrap_register(goo_canvas_grid_model_get_type(), &Goocanvas::GridModel_Class::wrap_new);
  Glib::wrap_register(goo_canvas_group_get_type(), &Goocanvas::Group_Class::wrap_new);
  Glib::wrap_register(goo_canvas_group_model_get_type(), &Goocanvas::GroupModel_Class::wrap_new);
  Glib::wrap_register(goo_canvas_image_get_type(), &Goocanvas::Image_Class::wrap_new);
  Glib::wrap_register(goo_canvas_image_model_get_type(), &Goocanvas::ImageModel_Class::wrap_new);
  Glib::wrap_register(goo_canvas_item_model_simple_get_type(), &Goocanvas::ItemModelSimple_Class::wrap_new);
  Glib::wrap_register(goo_canvas_item_simple_get_type(), &Goocanvas::ItemSimple_Class::wrap_new);
  Glib::wrap_register(goo_canvas_path_get_type(), &Goocanvas::Path_Class::wrap_new);
  Glib::wrap_register(goo_canvas_path_model_get_type(), &Goocanvas::PathModel_Class::wrap_new);
  Glib::wrap_register(goo_canvas_polyline_get_type(), &Goocanvas::Polyline_Class::wrap_new);
  Glib::wrap_register(goo_canvas_polyline_model_get_type(), &Goocanvas::PolylineModel_Class::wrap_new);
  Glib::wrap_register(goo_canvas_rect_get_type(), &Goocanvas::Rect_Class::wrap_new);
  Glib::wrap_register(goo_canvas_rect_model_get_type(), &Goocanvas::RectModel_Class::wrap_new);
  Glib::wrap_register(goo_canvas_style_get_type(), &Goocanvas::Style_Class::wrap_new);
  Glib::wrap_register(goo_canvas_table_get_type(), &Goocanvas::Table_Class::wrap_new);
  Glib::wrap_register(goo_canvas_table_model_get_type(), &Goocanvas::TableModel_Class::wrap_new);
  Glib::wrap_register(goo_canvas_text_get_type(), &Goocanvas::Text_Class::wrap_new);
  Glib::wrap_register(goo_canvas_text_model_get_type(), &Goocanvas::TextModel_Class::wrap_new);
  Glib::wrap_register(goo_canvas_widget_get_type(), &Goocanvas::Widget_Class::wrap_new);

  // Register the gtkmm gtypes:
  Goocanvas::Canvas::get_type();
  Goocanvas::Ellipse::get_type();
  Goocanvas::EllipseModel::get_type();
  Goocanvas::Grid::get_type();
  Goocanvas::GridModel::get_type();
  Goocanvas::Group::get_type();
  Goocanvas::GroupModel::get_type();
  Goocanvas::Image::get_type();
  Goocanvas::ImageModel::get_type();
  Goocanvas::ItemModelSimple::get_type();
  Goocanvas::ItemSimple::get_type();
  Goocanvas::Path::get_type();
  Goocanvas::PathModel::get_type();
  Goocanvas::Polyline::get_type();
  Goocanvas::PolylineModel::get_type();
  Goocanvas::Rect::get_type();
  Goocanvas::RectModel::get_type();
  Goocanvas::Style::get_type();
  Goocanvas::Table::get_type();
  Goocanvas::TableModel::get_type();
  Goocanvas::Text::get_type();
  Goocanvas::TextModel::get_type();
  Goocanvas::Widget::get_type();

} // wrap_init()

} //Goocanvas


