// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_NETFLOAT64_
#define _YARP2_NETFLOAT64_

#include <yarp/os/NetInt32.h>

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
//
// If we are compiling with CMake, we should have all the information
// we need.
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

#else

        typedef YARP_FLOAT64 RawNetFloat64;
        union UnionNetFloat64 {
            YARP_FLOAT64 d;
            unsigned char c[8];
        };
        class YARP_OS_API NetFloat64 {
        private:
            double raw_value;
            double swap(double x) const {
                UnionNetFloat64 in, out;
                in.d = x;
                for (int i=0; i<8; i++) {
                    out.c[i] = in.c[7-i];
                }
                return out.d;
            }
            RawNetFloat64 get() const {
                return (double)swap((double)raw_value);
            }
            void set(RawNetFloat64 v) {
                raw_value = (double)swap((double)v);
            }
        public:
            NetFloat64() {
            }
            NetFloat64(RawNetFloat64 val) {
                set(val);
            }
            operator RawNetFloat64() const {
                return get();
            }
            RawNetFloat64 operator+(RawNetFloat64 v) const {
                return get()+v;
            }
            RawNetFloat64 operator-(RawNetFloat64 v) const {
                return get()-v;
            }
            RawNetFloat64 operator*(RawNetFloat64 v) const {
                return get()*v;
            }
            RawNetFloat64 operator/(RawNetFloat64 v) const {
                return get()/v;
            }
            void operator+=(RawNetFloat64 v) {
                set(get()+v);
            }
            void operator-=(RawNetFloat64 v) {
                set(get()-v);
            }
            void operator*=(RawNetFloat64 v) {
                set(get()*v);
            }
            void operator/=(RawNetFloat64 v) {
                set(get()/v);
            }
        };

#endif

    }
}


#endif  // YARP_INT32









////////////////////////////////////////////////////////////////////////
//
// If we are not compiling with CMake, we fall back on older code
// for trying to guess the right type.  This code was starting to
// get rather clunky.
//

#ifndef YARP_FLOAT64


namespace yarp {
    namespace os {


#ifdef YARP2_LINUX
        typedef double NetFloat64;
#else
#  ifdef YARP2_WINDOWS
        typedef double NetFloat64;
#else
#  ifdef YARP2_CYGWIN
        typedef double NetFloat64;
#  else
#    ifdef YARP2_OSX


        /* OSX begins*/
#ifndef YARP_LITTLE_ENDIAN
#define YARP_ACTIVE_DOUBLE
        typedef double RawNetFloat64;
        union UnionNetFloat64 {
            double d;
            unsigned char c[8];
        };
        class YARP_OS_API NetFloat64 {
        private:
            double raw_value;
            double swap(double x) const {
                UnionNetFloat64 in, out;
                in.d = x;
                for (int i=0; i<8; i++) {
                    out.c[i] = in.c[7-i];
                }
                return out.d;
            }
            RawNetFloat64 get() const {
                return (double)swap((double)raw_value);
            }
            void set(RawNetFloat64 v) {
                raw_value = (double)swap((double)v);
            }
        public:
            NetFloat64() {
            }
            NetFloat64(RawNetFloat64 val) {
                set(val);
            }
            operator RawNetFloat64() const {
                return get();
            }
            RawNetFloat64 operator+(RawNetFloat64 v) const {
                return get()+v;
            }
            RawNetFloat64 operator-(RawNetFloat64 v) const {
                return get()-v;
            }
            RawNetFloat64 operator*(RawNetFloat64 v) const {
                return get()*v;
            }
            RawNetFloat64 operator/(RawNetFloat64 v) const {
                return get()/v;
            }
            void operator+=(RawNetFloat64 v) {
                set(get()+v);
            }
            void operator-=(RawNetFloat64 v) {
                set(get()-v);
            }
            void operator*=(RawNetFloat64 v) {
                set(get()*v);
            }
            void operator/=(RawNetFloat64 v) {
                set(get()/v);
            }
        };
#else
        typedef double NetFloat64; // random assumption
#endif
        /* OSX ends*/


#    else
        typedef double NetFloat64; // random assumption
#    endif
#  endif
#endif
#endif

    }
}

#endif

#endif /* _YARP2_NETFLOAT64_ */



