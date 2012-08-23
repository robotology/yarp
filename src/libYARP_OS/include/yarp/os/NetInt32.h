// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_NETINT32_
#define _YARP2_NETINT32_

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
//    typedef ___my_system_int16 NetInt16;
//
////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////
//
// If we are compiling with CMake, we should have all the information
// we need in yarp/conf/system.h
//   YARP_INT32 should be a 32-bit integer
//   YARP_BIG_ENDIAN should be defined if we are big endian
//   YARP_LITTLE_ENDIAN should be defined if we are little endian

#include <yarp/conf/system.h>

namespace yarp {
    namespace os {

#ifdef YARP_LITTLE_ENDIAN

        typedef YARP_INT32 NetInt32;
        typedef YARP_INT16 NetInt16;
        typedef unsigned YARP_INT32 NetUint32;
        typedef unsigned YARP_INT16 NetUint16;

#else

        class YARP_OS_API NetInt32 {
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


        class YARP_OS_API NetUint32 {
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


        class YARP_OS_API NetInt16 {
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


        class YARP_OS_API NetUint16 {
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


#endif /* _YARP2_NETINT32_ */



