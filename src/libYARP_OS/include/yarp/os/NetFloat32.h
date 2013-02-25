// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006, 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_NETFLOAT32_
#define _YARP2_NETFLOAT32_

#include <yarp/os/NetInt32.h>

////////////////////////////////////////////////////////////////////////
//
// The goal of this file is just to define a 32 bit signed little-endian
// floating point type
//
// If you are having trouble with it, and your system has a 32 bit
// little-endian type called e.g. ___my_system_float, you can replace
// this whole file with:
//    typedef ___my_system_float NetFloat32;
//
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
//
// If we are compiling with CMake, we should have all the information
// we need.
//   YARP_FLOAT32 should be a 32-bit float
//   YARP_BIG_ENDIAN should be defined if we are big endian
//   YARP_LITTLE_ENDIAN should be defined if we are little endian


#ifdef YARP_FLOAT32

namespace yarp {
    namespace os {
        /**
         * Definition of the NetFloat32 type
         */

#ifdef YARP_LITTLE_ENDIAN

        typedef YARP_FLOAT32 NetFloat32;

#else // YARP_LITTLE_ENDIAN

        typedef YARP_FLOAT32 RawNetFloat32;
        union UnionNetFloat32 {
            YARP_FLOAT32 d;
            unsigned char c[4];
        };
        class YARP_OS_API NetFloat32 {
        private:
            double raw_value;
            double swap(double x) const {
                UnionNetFloat32 in, out;
                in.d = x;
                for (int i=0; i<4; i++) {
                    out.c[i] = in.c[3-i];
                }
                return out.d;
            }
            RawNetFloat32 get() const {
                return (double)swap((double)raw_value);
            }
            void set(RawNetFloat32 v) {
                raw_value = (double)swap((double)v);
            }
        public:
            NetFloat32() {
            }
            NetFloat32(RawNetFloat32 val) {
                set(val);
            }
            operator RawNetFloat32() const {
                return get();
            }
            RawNetFloat32 operator+(RawNetFloat32 v) const {
                return get()+v;
            }
            RawNetFloat32 operator-(RawNetFloat32 v) const {
                return get()-v;
            }
            RawNetFloat32 operator*(RawNetFloat32 v) const {
                return get()*v;
            }
            RawNetFloat32 operator/(RawNetFloat32 v) const {
                return get()/v;
            }
            void operator+=(RawNetFloat32 v) {
                set(get()+v);
            }
            void operator-=(RawNetFloat32 v) {
                set(get()-v);
            }
            void operator*=(RawNetFloat32 v) {
                set(get()*v);
            }
            void operator/=(RawNetFloat32 v) {
                set(get()/v);
            }
        };

#endif // YARP_LITTLE_ENDIAN

    }
}


#endif  // YARP_FLOAT32


#endif /* _YARP2_NETFLOAT32_ */



