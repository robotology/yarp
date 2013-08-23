// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_CONSTSTRING_
#define _YARP2_CONSTSTRING_

#include <yarp/os/api.h>

#include <string>

#define YARP_CONSTSTRING_IS_STD_STRING 1

namespace yarp {
    namespace os {
        typedef std::string ConstString;
    }
}

#endif

