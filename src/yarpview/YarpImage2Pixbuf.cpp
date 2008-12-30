#include "YarpImage2Pixbuf.h"
#include "ace/OS.h"

bool yarpImage2Pixbuf(yarp::sig::ImageOf<yarp::sig::PixelRgb> *sourceImg, 
                      GdkPixbuf* destPixbuf)
{
	// il pixbuf deve essere già allocato e di dimensioni opportune
	guchar *dst_data;
	char *src_data;
	unsigned int rowstride;
	guchar *p_dst;
	char *p_src;
	unsigned int width, height;
	unsigned int n_channels;
	yarp::sig::PixelRgb srcPixel;
	unsigned int dst_size_in_memory;
	unsigned int src_line_size;

	dst_data = gdk_pixbuf_get_pixels(destPixbuf);
	width = sourceImg->width();
	height = sourceImg->height();
	rowstride = gdk_pixbuf_get_rowstride (destPixbuf);
	n_channels = gdk_pixbuf_get_n_channels (destPixbuf);
	dst_size_in_memory = rowstride * height;
	src_line_size = sourceImg->getRowSize(); //GetAllocatedLineSize();
	src_data = (char *) sourceImg->getRawImage(); //GetRawBuffer();

	if ( src_line_size == rowstride)
        {
            ACE_OS::memcpy(dst_data, src_data, dst_size_in_memory);
        }
	else
        {
            for (int i=0; i < (int)height; i++)
                {
                    p_dst = dst_data + i * rowstride;
                    p_src = src_data + i * src_line_size;
                    ACE_OS::memcpy(p_dst, p_src, (n_channels*width));
                }
        }

	return true;
}

