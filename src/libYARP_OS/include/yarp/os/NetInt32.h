// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_NETINT32_
#define _YARP2_NETINT32_


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
//
// If we are compiling with CMake, we should have all the information
// we need.
//   YARP_INT32 should be a 32-bit integer
//   YARP_BIG_ENDIAN should be defined if we are big endian
//   YARP_LITTLE_ENDIAN should be defined if we are little endian


#ifdef YARP_INT32

// to smooth upgrades, when YARP_INT16 hasn't made it into
// cmake cache for YARP - remove after a month or so from June 2008
#ifndef YARP_INT16
#define YARP_INT16 short
#endif

namespace yarp {
    namespace os {

#ifdef YARP_LITTLE_ENDIAN

        typedef YARP_INT32 NetInt32;
        typedef YARP_INT16 NetInt16;
        typedef unsigned YARP_INT32 NetUint32;
        typedef unsigned YARP_INT16 NetUint16;

#else

        class NetInt32 {
        private:
            unsigned YARP_INT32 raw_value;
            unsigned YARP_INT32 swap(unsigned YARP_INT32 x) const {
                return (x>>24) | ((x>>8) & 0xff00) | ((x<<8) & 0xff0000) | (x<<24);
            }
            YARP_INT32 get() const {
                return (YARP_INT32)swap(raw_value);
            }
            void set(YARP_INT32 v) {
                raw_value = (YARP_INT32)swap((unsigned YARP_INT32)v);
            }
        public:
            NetInt32() {
            }
            NetInt32(YARP_INT32 val) {
                set(val);
            }
            operator YARP_INT32() const {
                return get();
            }
            YARP_INT32 operator+(YARP_INT32 v) const {
                return get()+v;
            }
            YARP_INT32 operator-(YARP_INT32 v) const {
                return get()-v;
            }
            YARP_INT32 operator*(YARP_INT32 v) const {
                return get()*v;
            }
            YARP_INT32 operator/(YARP_INT32 v) const {
                return get()/v;
            }
            void operator+=(YARP_INT32 v) {
                set(get()+v);
            }
            void operator-=(YARP_INT32 v) {
                set(get()-v);
            }
            void operator*=(YARP_INT32 v) {
                set(get()*v);
            }
            void operator/=(YARP_INT32 v) {
                set(get()/v);
            }
            void operator++(int) { 
                set(get()+1);
            };
            void operator--(int) { 
                set(get()-1);
            };
        };


        class NetUint32 {
        private:
            unsigned YARP_INT32 raw_value;
            unsigned YARP_INT32 swap(unsigned YARP_INT32 x) const {
                return (x>>24) | ((x>>8) & 0xff00) | ((x<<8) & 0xff0000) | (x<<24);
            }
            unsigned YARP_INT32 get() const {
                return (unsigned YARP_INT32)swap(raw_value);
            }
            void set(unsigned YARP_INT32 v) {
                raw_value = (unsigned YARP_INT32)swap((unsigned YARP_INT32)v);
            }
        public:
            NetUint32() {
            }
            NetUint32(unsigned YARP_INT32 val) {
                set(val);
            }
            operator unsigned YARP_INT32() const {
                return get();
            }
            unsigned YARP_INT32 operator+(unsigned YARP_INT32 v) const {
                return get()+v;
            }
            unsigned YARP_INT32 operator-(unsigned YARP_INT32 v) const {
                return get()-v;
            }
            unsigned YARP_INT32 operator*(unsigned YARP_INT32 v) const {
                return get()*v;
            }
            unsigned YARP_INT32 operator/(unsigned YARP_INT32 v) const {
                return get()/v;
            }
            void operator+=(unsigned YARP_INT32 v) {
                set(get()+v);
            }
            void operator-=(unsigned YARP_INT32 v) {
                set(get()-v);
            }
            void operator*=(unsigned YARP_INT32 v) {
                set(get()*v);
            }
            void operator/=(unsigned YARP_INT32 v) {
                set(get()/v);
            }
            void operator++(int) { 
                set(get()+1);
            };
            void operator--(int) { 
                set(get()-1);
            };
        };


        class NetInt16 {
        private:
            unsigned YARP_INT16 raw_value;
            unsigned YARP_INT16 swap(unsigned YARP_INT16 x) const {
                return (x>>8) | ((x<<8) & 0xff00);
            }
            YARP_INT16 get() const {
                return (YARP_INT16)swap(raw_value);
            }
            void set(YARP_INT16 v) {
                raw_value = (YARP_INT16)swap((unsigned YARP_INT16)v);
            }
        public:
            NetInt16() {
            }
            NetInt16(YARP_INT16 val) {
                set(val);
            }
            operator YARP_INT16() const {
                return get();
            }
            YARP_INT16 operator+(YARP_INT16 v) const {
                return get()+v;
            }
            YARP_INT16 operator-(YARP_INT16 v) const {
                return get()-v;
            }
            YARP_INT16 operator*(YARP_INT16 v) const {
                return get()*v;
            }
            YARP_INT16 operator/(YARP_INT16 v) const {
                return get()/v;
            }
            void operator+=(YARP_INT16 v) {
                set(get()+v);
            }
            void operator-=(YARP_INT16 v) {
                set(get()-v);
            }
            void operator*=(YARP_INT16 v) {
                set(get()*v);
            }
            void operator/=(YARP_INT16 v) {
                set(get()/v);
            }
            void operator++(int) { 
                set(get()+1);
            };
            void operator--(int) { 
                set(get()-1);
            };
        };


        class NetUint16 {
        private:
            unsigned YARP_INT16 raw_value;
            unsigned YARP_INT16 swap(unsigned YARP_INT16 x) const {
                return (x>>8) | ((x<<8) & 0xff00);
            }
            unsigned YARP_INT16 get() const {
                return (unsigned YARP_INT16)swap(raw_value);
            }
            void set(unsigned YARP_INT16 v) {
                raw_value = (unsigned YARP_INT16)swap((unsigned YARP_INT16)v);
            }
        public:
            NetUint16() {
            }
            NetUint16(unsigned YARP_INT16 val) {
                set(val);
            }
            operator unsigned YARP_INT16() const {
                return get();
            }
            unsigned YARP_INT16 operator+(unsigned YARP_INT16 v) const {
                return get()+v;
            }
            unsigned YARP_INT16 operator-(unsigned YARP_INT16 v) const {
                return get()-v;
            }
            unsigned YARP_INT16 operator*(unsigned YARP_INT16 v) const {
                return get()*v;
            }
            unsigned YARP_INT16 operator/(unsigned YARP_INT16 v) const {
                return get()/v;
            }
            void operator+=(unsigned YARP_INT16 v) {
                set(get()+v);
            }
            void operator-=(unsigned YARP_INT16 v) {
                set(get()-v);
            }
            void operator*=(unsigned YARP_INT16 v) {
                set(get()*v);
            }
            void operator/=(unsigned YARP_INT16 v) {
                set(get()/v);
            }
            void operator++(int) { 
                set(get()+1);
            };
            void operator--(int) { 
                set(get()-1);
            };
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

#ifndef YARP_INT32



// Sleazy trick for big endian systems we haven't tested on yet.
// The two files NetInt32.h and NetFloat64.h need to be cleaned up.
// They can be much simplified by deferring to cmake/autoconf.
#ifdef YARP_BIG_ENDIAN
#define YARP2_OSX
#include <yarp/String.h>
#endif

namespace yarp {
    namespace os {

        /**
         * Definition of the NetInt32 type
         */

#ifdef CYGWIN
#ifndef YARP2_CYGWIN
#define YARP2_CYGWIN
#endif
#endif


#ifndef YARP2_CYGWIN

#ifdef WIN32
#ifndef YARP2_WINDOWS
#define YARP2_WINDOWS
#endif
#endif

#ifdef _WIN32
#ifndef YARP2_WINDOWS
#define YARP2_WINDOWS
#endif
#endif

#ifdef WINDOW2
#ifndef YARP2_WINDOWS
#define YARP2_WINDOWS
#endif
#endif

#ifdef __WIN__
#ifndef YARP2_WINDOWS
#define YARP2_WINDOWS
#endif
#endif

#ifdef __WINDOWS__
#ifndef YARP2_WINDOWS
#define YARP2_WINDOWS
#endif
#endif

#ifdef WINDOWS
#ifndef YARP2_WINDOWS
#define YARP2_WINDOWS
#endif
#endif

#endif /* ifndef CYGWIN */


#ifdef __LINUX__
#ifndef YARP2_LINUX
#define YARP2_LINUX
#endif
#endif

#ifdef __linux__
#ifndef YARP2_LINUX
#define YARP2_LINUX
#endif
#endif

#ifdef __darwin__
#ifndef YARP2_OSX
#define YARP2_OSX
#endif
#endif

#ifdef __DARWIN__
#ifndef YARP2_OSX
#define YARP2_OSX
#endif
#endif

#ifdef DARWIN
#ifndef YARP2_OSX
#define YARP2_OSX
#endif
#endif

#ifdef MACOSX
#ifndef YARP2_OSX
#define YARP2_OSX
#endif
#endif

#ifdef __APPLE__
#ifndef YARP2_OSX
#define YARP2_OSX
#endif
#endif

#ifdef YARP2_LINUX
        typedef int NetInt32;
        typedef short NetInt16;
        typedef unsigned int NetUint32;
        typedef unsigned short NetUint16;
        //typedef int32_t NetInt32;
#else
#  ifdef YARP2_WINDOWS
#    ifdef __MINGW_H
        typedef int NetInt32;
        typedef short NetInt16;
        typedef unsigned int NetUint32;
        typedef unsigned short NetUint16;
#    else 
        typedef __int32 NetInt32;
        typedef __int16 NetInt16;
        typedef unsigned __int32 NetUint32;
        typedef unsigned __int16 NetUint16;
#    endif
#else
#  ifdef YARP2_CYGWIN
        typedef int NetInt32;
        typedef short NetInt16;
        typedef unsigned int NetUint32;
        typedef unsigned short NetUint16;
#  else
#    ifdef YARP2_OSX

#ifndef YARP_LITTLE_ENDIAN
#error "OSX PPC compiles only supported with CMake"
#else
        // not really guaranteed to have this
#include <sys/types.h>
        typedef int32_t NetInt32;
        typedef uint32_t NetUint32;
        typedef int16_t NetInt16;
        typedef uint16_t NetUint16;
#endif
        /* OSX ends*/


#    else
        // now we have to assume we are using autoconf etc
#undef  PACKAGE_BUGREPORT
#undef  PACKAGE_NAME
#undef  PACKAGE_STRING
#undef  PACKAGE_TARNAME
#undef  PACKAGE_VERSION
#    include <yarp/conf/yarp-config.h>
#    if !(WORDS_BIGENDIAN)
#if SIZEOF_INT == 4
        typedef int NetInt32;
        typedef short NetInt16;
        typedef unsigned int NetUint32;
        typedef unsigned short NetUint16;
#else
#if SIZEOF_SHORT == 4
#      error "could not choose NetInt32 type; see yarp/os/NetInt32.h"
#else
#      error "could not choose NetInt32 type; see yarp/os/NetInt32.h"
#endif
#endif
#      else
#        error "need to define NetInt32 for this OS; see yarp/os/NetInt32.h"
#      endif
#    endif
#  endif
#endif
#endif

    }
}

#endif // !YARP_INT32

#endif /* _YARP2_NETINT32_ */



