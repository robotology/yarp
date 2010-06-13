// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_WRITABLE_
#define _YARP2_WRITABLE_

#include <yarp/os/ConnectionWriter.h>

#include <yarp/os/PortWriter.h>

// this typedef will be removed soon - it used be a separate class

namespace yarp {
    typedef yarp::os::PortWriter Writable;
}


#endif
