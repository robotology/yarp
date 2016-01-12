// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Author Lorenzo Natale
 * email:   lorenzo.natale@robotcub.org
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARPIMAGE2PIXBUF
#define YARPIMAGE2PIXBUF

#include <yarp/sig/Image.h>
#include <gtk/gtk.h>

bool yarpImage2Pixbuf(yarp::sig::ImageOf<yarp::sig::PixelRgb> *sourceImg,
                      GdkPixbuf* destPixbuf);
#endif
