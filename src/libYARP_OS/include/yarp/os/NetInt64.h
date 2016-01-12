/*
 * Copyright (C) 2006, 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_NETINT64_H
#define YARP_OS_NETINT64_H

#include <yarp/conf/numeric.h>
#include <yarp/os/api.h>

////////////////////////////////////////////////////////////////////////
//
// The goal of this file is just to define a 64 bit signed little-endian
// integer type
//
// If you are having trouble with it, and your system has a 64 bit
// little-endian type called e.g. ___my_system_int64, you can replace
// this whole file with:
//    typedef ___my_system_int64 NetInt64;
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

        typedef YARP_INT64 NetInt64;

#else

        typedef YARP_INT64 RawNetInt64;
        union UnionNetInt64 {
            YARP_INT64 d;
            unsigned char c[8];
        };
        class YARP_OS_API NetInt64 {
        private:
            YARP_INT64 raw_value;
            YARP_INT64 swap(YARP_INT64 x) const;
            RawNetInt64 get() const;
            void set(RawNetInt64 v);
        public:
            NetInt64();
            NetInt64(RawNetInt64 val);
            operator RawNetInt64() const;
            RawNetInt64 operator+(RawNetInt64 v) const;
            RawNetInt64 operator-(RawNetInt64 v) const;
            RawNetInt64 operator*(RawNetInt64 v) const;
            RawNetInt64 operator/(RawNetInt64 v) const;
            void operator+=(RawNetInt64 v);
            void operator-=(RawNetInt64 v);
            void operator*=(RawNetInt64 v);
            void operator/=(RawNetInt64 v);
        };

#endif // YARP_LITTLE_ENDIAN

    }
}

#else

#error "NetInt64 not defined"

#endif // YARP_INT32

#endif // YARP_OS_NETINT64_H
