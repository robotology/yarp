// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_STRING_
#define _YARP2_STRING_

#if YARP_USE_STL_STRING

#include <string>
namespace yarp {
    namespace os {
        namespace impl {
            typedef std::string String;
        }
    }
}

#define YARP_STRSTR(haystack,needle) (haystack).find(needle)
#define YARP_STRSET(str,cstr,len,owned) str = std::string(cstr,len)
typedef size_t YARP_STRING_INDEX;

class __KeyedString : public yarp::os::impl::String {
public:
    unsigned long int hash() const {
        unsigned long int h = 5381;
        for (unsigned int i=0; i<length(); i++) {
            h = ((h << 5) + h) + (*this)[i]; 
        }
        return h;
    }

    __KeyedString() {}

    __KeyedString(const char *str) : yarp::os::impl::String(str) {
    }

    __KeyedString(const yarp::os::impl::String& str) : yarp::os::impl::String(str) {
    }
};
typedef __KeyedString YARP_KEYED_STRING;

#else

#include <ace/String_Base.h>
namespace yarp {
    namespace os {
        namespace impl {
            typedef ACE_String_Base<char> String;
        }
    }
}

#define YARP_STRSTR(haystack,needle) (haystack).strstr(needle)
#define YARP_STRSET(str,cstr,len,owned) str.set(cstr,len,owned)
typedef yarp::os::impl::String YARP_KEYED_STRING;
typedef ssize_t YARP_STRING_INDEX;

#endif

//#undef main //undef ace floating defines

#endif

