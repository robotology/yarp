// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef _YARP2_NETINT16_
#define _YARP2_NETINT16_

#include <yarp/conf/numeric.h>
#include <yarp/os/api.h>

////////////////////////////////////////////////////////////////////////
//
// The goal of this file is just to define a 16 bit signed little-endian
// integer type.
//
// If you are having trouble with it, and your system has a 16 bit
// little-endian type called e.g. ___my_system_int16, you can replace
// this whole file with:
//    typedef ___my_system_int16 NetInt16;
//
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
//   YARP_INT16 should be a 16-bit integer
//   YARP_BIG_ENDIAN should be defined if we are big endian
//   YARP_LITTLE_ENDIAN should be defined if we are little endian


#ifdef YARP_INT16

namespace yarp {
    namespace os {
        /**
         * Definition of the NetInt16 type
         */

#ifdef YARP_LITTLE_ENDIAN

        typedef YARP_INT16 NetInt16;

#else // YARP_LITTLE_ENDIAN

        class YARP_OS_API NetInt16 {
        private:
            unsigned YARP_INT16 raw_value;
            unsigned YARP_INT16 swap(unsigned YARP_INT16 x) const;
            YARP_INT16 get() const;
            void set(YARP_INT16 v);
        public:
            NetInt16();
            NetInt16(YARP_INT16 val);
            operator YARP_INT16() const;
            YARP_INT16 operator+(YARP_INT16 v) const;
            YARP_INT16 operator-(YARP_INT16 v) const;
            YARP_INT16 operator*(YARP_INT16 v) const;
            YARP_INT16 operator/(YARP_INT16 v) const;
            void operator+=(YARP_INT16 v);
            void operator-=(YARP_INT16 v);
            void operator*=(YARP_INT16 v);
            void operator/=(YARP_INT16 v);
            void operator++(int);
            void operator--(int);
        };

#endif // YARP_LITTLE_ENDIAN

    }
}

#else // YARP_INT16

#error "NetInt16 not defined"

#endif  // YARP_INT16

#endif // _YARP2_NETINT16_
