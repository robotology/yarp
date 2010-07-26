// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_WIREIMAGE
#define YARP2_WIREIMAGE

#include <yarp/os/impl/SizedWriter.h>
#include <yarp/sig/Image.h>

class WireImage {
private:
  yarp::sig::FlexImage img;
public:
  yarp::sig::FlexImage *checkForImage(yarp::os::impl::SizedWriter& writer);
};

#endif
