/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_NETINT32_H
#define YARP_OS_NETINT32_H

#include <yarp/conf/numeric.h>
#include <yarp/os/api.h>

////////////////////////////////////////////////////////////////////////
//
// The goal of this file is just to define a 32 bit signed little-endian
// integer type.
//
// If you are having trouble with it, and your system has a 32 bit
// little-endian type called e.g. ___my_system_int32, you can replace
// this whole file with:
//    typedef ___my_system_int32 NetInt32;
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
         * Definition of the NetInt32 type
         */

#ifdef YARP_LITTLE_ENDIAN

        typedef YARP_INT32 NetInt32;

#else

        class YARP_OS_API NetInt32 {
        private:
            unsigned YARP_INT32 raw_value;
            unsigned YARP_INT32 swap(unsigned YARP_INT32 x) const;
            YARP_INT32 get() const;
            void set(YARP_INT32 v);
        public:
            NetInt32();
            NetInt32(YARP_INT32 val);
            operator YARP_INT32() const;
            YARP_INT32 operator+(YARP_INT32 v) const;
            YARP_INT32 operator-(YARP_INT32 v) const;
            YARP_INT32 operator*(YARP_INT32 v) const;
            YARP_INT32 operator/(YARP_INT32 v) const;
            void operator+=(YARP_INT32 v);
            void operator-=(YARP_INT32 v);
            void operator*=(YARP_INT32 v);
            void operator/=(YARP_INT32 v);
            void operator++(int);
            void operator--(int);
        };

#endif // YARP_LITTLE_ENDIAN

    }
}

#else // YARP_INT32

#error "NetInt32 not defined"

#endif // YARP_INT32

#endif // YARP_OS_NETINT32_H
