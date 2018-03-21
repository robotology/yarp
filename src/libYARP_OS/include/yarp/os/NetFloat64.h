/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_NETFLOAT64_H
#define YARP_OS_NETFLOAT64_H

#include <yarp/conf/numeric.h>
#include <yarp/os/api.h>

////////////////////////////////////////////////////////////////////////
//
// The goal of this file is just to define a 64 bit signed little-endian
// floating point type
//
// If you are having trouble with it, and your system has a 32 bit
// little-endian type called e.g. ___my_system_double, you can replace
// this whole file with:
//    typedef ___my_system_double NetFloat64;
//
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
//   YARP_FLOAT64 should be a 64-bit float
//   YARP_BIG_ENDIAN should be defined if we are big endian
//   YARP_LITTLE_ENDIAN should be defined if we are little endian


#ifdef YARP_FLOAT64

namespace yarp {
    namespace os {
        /**
         * Definition of the NetFloat64 type
         */

#ifdef YARP_LITTLE_ENDIAN

        typedef YARP_FLOAT64 NetFloat64;

#else // YARP_LITTLE_ENDIAN

        typedef YARP_FLOAT64 RawNetFloat64;
        union UnionNetFloat64 {
            YARP_FLOAT64 d;
            unsigned char c[8];
        };
        class YARP_OS_API NetFloat64 {
        private:
            double raw_value;
            double swap(double x) const;
            RawNetFloat64 get() const;
            void set(RawNetFloat64 v);
        public:
            NetFloat64();
            NetFloat64(RawNetFloat64 val);
            operator RawNetFloat64() const;
            RawNetFloat64 operator+(RawNetFloat64 v) const;
            RawNetFloat64 operator-(RawNetFloat64 v) const;
            RawNetFloat64 operator*(RawNetFloat64 v) const;
            RawNetFloat64 operator/(RawNetFloat64 v) const;
            void operator+=(RawNetFloat64 v);
            void operator-=(RawNetFloat64 v);
            void operator*=(RawNetFloat64 v);
            void operator/=(RawNetFloat64 v);
        };

#endif // YARP_LITTLE_ENDIAN

    }
}

#endif // YARP_FLOAT64

#endif // YARP_OS_NETFLOAT64_H
