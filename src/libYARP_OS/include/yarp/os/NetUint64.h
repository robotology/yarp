// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2013 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef _YARP2_NETUINT64_
#define _YARP2_NETUINT64_

#include <yarp/conf/system.h>
#include <yarp/os/api.h>

////////////////////////////////////////////////////////////////////////
//
// The goal of this file is just to define a 64 bit unsigned little-endian
// integer type
//
// If you are having trouble with it, and your system has a 64 bit
// little-endian type called e.g. ___my_system_uint64, you can replace
// this whole file with:
//    typedef ___my_system_uint64 NetUint64;
//
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
//   YARP_INT64 should be a 64-bit integer
//   YARP_BIG_ENDIAN should be defined if we are big endian
//   YARP_LITTLE_ENDIAN should be defined if we are little endian


#ifdef YARP_INT64

namespace yarp {
    namespace os {
        /**
         * Definition of the NetInt64 type
         */

#ifdef YARP_LITTLE_ENDIAN

        typedef unsigned YARP_INT64 NetUint64;

#else

        typedef unsigned YARP_INT64 RawNetUint64;
        union UnionNetUint64 {
            unsigned YARP_INT64 d;
            unsigned char c[8];
        };
        class YARP_OS_API NetUint64 {
        private:
            unsigned YARP_INT64 raw_value;
            unsigned YARP_INT64 swap(unsigned YARP_INT64 x) const;
            RawNetUint64 get() const;
            void set(RawNetUint64 v);
        public:
            NetUint64();
            NetUint64(RawNetUint64 val);
            operator RawNetUint64() const;
            RawNetUint64 operator+(RawNetUint64 v) const;
            RawNetUint64 operator-(RawNetUint64 v) const;
            RawNetUint64 operator*(RawNetUint64 v) const;
            RawNetUint64 operator/(RawNetUint64 v) const;
            void operator+=(RawNetUint64 v);
            void operator-=(RawNetUint64 v);
            void operator*=(RawNetUint64 v);
            void operator/=(RawNetUint64 v);
        };

#endif // YARP_LITTLE_ENDIAN

    }
}

#else

#error "NetInt64 not defined"

#endif  // YARP_INT32

#endif /* _YARP2_NETINT64_ */
