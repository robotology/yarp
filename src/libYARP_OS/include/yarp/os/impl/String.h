// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_STRING_
#define _YARP2_STRING_

#include <yarp/os/api.h>
#include <yarp/conf/system.h>

#include <string>
namespace yarp {
    namespace os {
        namespace impl {
            typedef std::string String;
        }
    }
}

#define YARP_STRSTR(haystack,needle) (haystack).find(needle)
#define YARP_STRSTR_OFFSET(haystack,needle,offset) (haystack).find_first_of(needle,offset)
#define YARP_STRSET(str,cstr,len,owned) str = std::string(cstr,len)
typedef size_t YARP_STRING_INDEX;

typedef std::string YARP_KEYED_STRING;

#endif
