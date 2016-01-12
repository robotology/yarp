// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP2_NETUINT32
#define YARP2_NETUINT32

#include <yarp/conf/numeric.h>
#include <yarp/os/api.h>

////////////////////////////////////////////////////////////////////////
//
// The goal of this file is just to define a 32 bit unsigned little-endian
// integer type.
//
// If you are having trouble with it, and your system has a 32 bit unsigned
// little-endian type called e.g. ___my_system_uint32, you can replace
// this whole file with:
//    typedef ___my_system_uint32 NetUint32;
//
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
//   YARP_INT32 should be a 32-bit integer
//   YARP_BIG_ENDIAN should be defined if we are big endian
//   YARP_LITTLE_ENDIAN should be defined if we are little endian


#ifdef YARP_INT32

namespace yarp {
    namespace os {
        /**
         * Definition of the NetUint32 type
         */

#ifdef YARP_LITTLE_ENDIAN

        typedef unsigned YARP_INT32 NetUint32;

#else // YARP_LITTLE_ENDIAN

        class YARP_OS_API NetUint32 {
        private:
            unsigned YARP_INT32 raw_value;
            unsigned YARP_INT32 swap(unsigned YARP_INT32 x) const;
            unsigned YARP_INT32 get() const;
            void set(unsigned YARP_INT32 v);
        public:
            NetUint32();
            NetUint32(unsigned YARP_INT32 val);
            operator unsigned YARP_INT32() const;
            unsigned YARP_INT32 operator+(unsigned YARP_INT32 v) const;
            unsigned YARP_INT32 operator-(unsigned YARP_INT32 v) const;
            unsigned YARP_INT32 operator*(unsigned YARP_INT32 v) const;
            unsigned YARP_INT32 operator/(unsigned YARP_INT32 v) const;
            void operator+=(unsigned YARP_INT32 v);
            void operator-=(unsigned YARP_INT32 v);
            void operator*=(unsigned YARP_INT32 v);
            void operator/=(unsigned YARP_INT32 v);
            void operator++(int);
            void operator--(int);
        };

#endif // YARP_LITTLE_ENDIAN

    }
}

#else // YARP_INT32

#error "NetUint32 not defined"

#endif  // YARP_INT32

#endif // _YARP2_NETUINT32_
