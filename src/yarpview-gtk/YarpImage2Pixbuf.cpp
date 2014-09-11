// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Author Matteo Brunettini
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include "YarpImage2Pixbuf.h"

#include <string.h>

bool yarpImage2Pixbuf(yarp::sig::ImageOf<yarp::sig::PixelRgb> *sourceImg,
                      GdkPixbuf* destPixbuf)
{
	// il pixbuf deve essere gia`  allocato e di dimensioni opportune
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
            memcpy(dst_data, src_data, dst_size_in_memory);
        }
	else
        {
            for (int i=0; i < (int)height; i++)
                {
                    p_dst = dst_data + i * rowstride;
                    p_src = src_data + i * src_line_size;
                    memcpy(p_dst, p_src, (n_channels*width));
                }
        }

	return true;
}

