#ifndef __VIEWERRESOURCES__
#define __VIEWERRESOURCES__

#include <gtk/gtk.h>
#include "YarpImage2Pixbuf.h"

#include <yarp/os/Semaphore.h>
#include <yarp/os/Time.h>

#include "FpsStats.h"

class ViewerResources
{
public:
    ViewerResources()
    {
        frame=0;
        drawArea=0;
        fpsData=0;
        freezed=false;
    }

    bool attach(FpsStats *d)
    {
        if (fpsData!=0)
            return false;
        fpsData=d;
        return true;
    }

    void freeze()
    {
        freezed=true;
    }

    void unfreeze()
    {
        freezed=false;
    }

    bool isFreezed()
    { return freezed; }

    void lock() { mutex.wait(); }
    void unlock() { mutex.post(); }

    void invalidateDrawArea()
    {
        if (drawArea)
            gtk_widget_queue_draw (drawArea);
    }

   void draw(bool force=false)
    {
        if ( (!force) && freezed)
            return;

       if (fpsData!=0)
            fpsData->update(yarp::os::Time::now());

       draw(drawArea, 0, 0, drawArea->allocation.width, drawArea->allocation.height);
    }

    void draw(GtkWidget *widget,
              unsigned int areaX, unsigned int areaY, unsigned int areaW, unsigned int areaH)
    {
        unsigned int pixbufWidth=gdk_pixbuf_get_width(frame);
        unsigned int pixbufHeight=gdk_pixbuf_get_height(frame);

        lock();

        unsigned int imageWidth = yimage.width();
        unsigned int imageHeight = yimage.height();
        
        if (imageWidth==0)
        {
            unlock();
            return; //nothing to draw
        }
        if (imageHeight==0)
        {
            unlock(); 
            return; //nothing to draw
        } 
        
        if ((imageWidth!=pixbufWidth) || (imageHeight!=pixbufHeight))
                {
                    g_object_unref(frame);
                    frame=gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, imageWidth, imageHeight);
                }

        yarpImage2Pixbuf(&yimage, frame);
        unlock();

        if ( (areaW==pixbufWidth) && (areaH==pixbufHeight))
          _draw(frame, widget, areaX, areaY, areaW, areaH);
        else
          _scaleAndDraw(frame, widget, areaX, areaY, areaW, areaH);
    }

    void _scaleAndDraw(GdkPixbuf *frame, GtkWidget *widget,
                      unsigned int areaX, unsigned int areaY, unsigned int areaW, unsigned int areaH)
    {
        guchar *pixels;
        unsigned int rowstride;
        GdkPixbuf *scaledFrame;
        scaledFrame = gdk_pixbuf_scale_simple(frame,
                                              areaW,
                                              areaH,
                                              GDK_INTERP_BILINEAR); // Best quality
                    //GDK_INTERP_NEAREST); // Best speed

                    pixels = gdk_pixbuf_get_pixels (scaledFrame);
                    rowstride = gdk_pixbuf_get_rowstride(scaledFrame);
                    gdk_draw_rgb_image (widget->window,
                                        widget->style->black_gc,
                                        areaX, areaY,
                                        areaW, areaH,
                                        GDK_RGB_DITHER_NORMAL,
                                        pixels,
                                        rowstride);
                    g_object_unref(scaledFrame);
    }
			
    void _draw(GdkPixbuf *frame,
              GtkWidget *widget,
              unsigned int areaX, unsigned int areaY, unsigned int areaW, unsigned int areaH)
    {
            guchar *pixels;
            unsigned int rowstride;

            pixels = gdk_pixbuf_get_pixels (frame);
            rowstride = gdk_pixbuf_get_rowstride(frame);
            gdk_draw_rgb_image (widget->window,
                                  widget->style->black_gc,
                                  areaX, areaY,
                                  areaW, areaH,
                                  GDK_RGB_DITHER_NORMAL,
                                  pixels,
                                  rowstride);
    }

    void pushImage(yarp::sig::FlexImage &n)
    {
        if (freezed)
            return;
        
        lock();
        yimage.copy(n);
        unlock();
    }

    unsigned int height()
    {
        lock();
        unsigned int ret=yimage.height();
        unlock();

        return ret;
    }

    unsigned int width()
    {
        lock();
        unsigned int ret=yimage.width();
        unlock();
        return ret;
    }


    GdkPixbuf *frame;
    GtkWidget *drawArea;
    FpsStats  *fpsData;
    yarp::sig::ImageOf<yarp::sig::PixelRgb> yimage;

    bool freezed;

    yarp::os::Semaphore mutex;
 };


#endif __VIEWERRESOURCES__