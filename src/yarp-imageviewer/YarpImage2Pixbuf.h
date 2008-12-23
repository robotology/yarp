#ifndef __YARPIMAGE2PIXBUF__

#include <yarp/sig/Image.h>
#include <gtk/gtk.h>

bool yarpImage2Pixbuf(yarp::sig::ImageOf<yarp::sig::PixelRgb> *sourceImg, 
                      GdkPixbuf* destPixbuf);
#endif
