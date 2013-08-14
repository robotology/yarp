// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/NetType.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/ManagedBytes.h>

using namespace yarp::os::impl;
using namespace yarp::os;

int NetType::netInt(const yarp::os::Bytes& code) {
    YARP_ASSERT(code.length()==sizeof(NetInt32));
    NetInt32 tmp;
    ACE_OS::memcpy((char*)(&tmp),code.get(),code.length());
    return tmp;
}

bool NetType::netInt(int data, const yarp::os::Bytes& code) {
    NetInt32 i = data;
    yarp::os::Bytes b((char*)(&i),sizeof(i));
    if (code.length()!=sizeof(i)) {
        YARP_ERROR(Logger::get(),"not enough room for integer");
        return false;
    }
    ACE_OS::memcpy(code.get(),b.get(),code.length());
    return true;
}

ConstString NetType::toHexString(int x) {
    char buf[256];
    ACE_OS::sprintf(buf,"%x",x);
    return buf;
}

ConstString NetType::toString(int x) {
    char buf[256];
    ACE_OS::sprintf(buf,"%d",x);
    return buf;
}

ConstString NetType::toString(long x) {
    char buf[256];
    ACE_OS::sprintf(buf,"%ld",x);
    return buf;
}

ConstString NetType::toString(unsigned int x) {
    char buf[256];
    ACE_OS::sprintf(buf,"%u",x);
    return buf;
}


int NetType::toInt(const ConstString& x) {
    return ACE_OS::atoi(x.c_str());
}


/*
  PNG's nice and simple CRC code 
  (from http://www.w3.org/TR/PNG-CRCAppendix.html)
*/

/* Table of CRCs of all 8-bit messages. */
static unsigned long crc_table[256];

/* Flag: has the table been computed? Initially false. */
static int crc_table_computed = 0;
   
/* Make the table for a fast CRC. */
static void make_crc_table(void) {
    unsigned long c;
    int n, k;
   
    for (n = 0; n < 256; n++) {
        c = (unsigned long) n;
        for (k = 0; k < 8; k++) {
            if (c & 1)
                c = 0xedb88320L ^ (c >> 1);
            else
                c = c >> 1;
        }
        crc_table[n] = c;
    }
    crc_table_computed = 1;
}

/* Update a running CRC with the bytes buf[0..len-1]--the CRC
   should be initialized to all 1's, and the transmitted value
   is the 1's complement of the final running CRC (see the
   crc() routine below)). */

static unsigned long update_crc(unsigned long crc, unsigned char *buf,
                                size_t len) {

    unsigned long c = crc;
    size_t n;
  
    if (!crc_table_computed)
        make_crc_table();
    for (n = 0; n < len; n++) {
        c = crc_table[(c ^ buf[n]) & 0xff] ^ (c >> 8);
    }
    return c;
}

/* Return the CRC of the bytes buf[0..len-1]. */
unsigned long NetType::getCrc(char *buf, size_t len) {
    return update_crc(0xffffffffL, (unsigned char *)buf, len) ^ 0xffffffffL;
}
