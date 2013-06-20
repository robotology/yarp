// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/impl/NetType.h>
#include <yarp/os/ManagedBytes.h>

using namespace yarp::os::impl;
using namespace yarp::os;

// slow implementation - only relevant for textmode operation

String NetType::readLine(InputStream& is, int terminal, bool *success) {
    String buf("");
    bool done = false;
    int esc = 0;
    if (success!=NULL) *success = true;
    while (!done) {
        //ACE_OS::printf("preget\n");
        int v = is.read();
        if (v<0) {
            if (success!=NULL) *success = false;
            return "";
        }
        //ACE_OS::printf("got [%d]\n",v);
        char ch = (char)v;
        if (v=='\\') {
            esc++;
        }
        if (v!=0&&v!='\r'&&v!='\n') {
            if (v!='\\'||esc>=2) {
                while (esc) {
                    buf += '\\';
                    esc--;
                }
            }
            if (v!='\\') {
                buf += ch;
            }
        }
        if (ch==terminal) {
            if (!esc) {
                done = true;
            } else {
                esc = 0;
            }
        }
        if (v<0) { 
            if (success!=NULL) *success = false;
            return "";
        }
    }
    return buf;
}    

ssize_t NetType::readFull(InputStream& is, const Bytes& b) {
    ssize_t off = 0;
    ssize_t fullLen = b.length();
    ssize_t remLen = fullLen;
    ssize_t result = 1;
    while (result>0&&remLen>0) {
        result = is.read(b,off,remLen);
        //printf("read result is %d\n",result);
        if (result>0) {
            remLen -= result;
            off += result;
        }
    }
    return (result<=0)?-1:fullLen;
}

ssize_t NetType::readDiscard(InputStream& is, size_t len) {
    if (len<100) {
        char buf[100];
        Bytes b(buf,len);
        return readFull(is,b);
    } else {
        ManagedBytes b(len);
        return readFull(is,b.bytes());
    }
}


String NetType::toHexString(int x) {
    char buf[256];
    ACE_OS::sprintf(buf,"%x",x);
    return buf;
}

String NetType::toString(int x) {
    char buf[256];
    ACE_OS::sprintf(buf,"%d",x);
    return buf;
}

String NetType::toString(long x) {
    char buf[256];
    ACE_OS::sprintf(buf,"%ld",x);
    return buf;
}

String NetType::toString(unsigned int x) {
    char buf[256];
    ACE_OS::sprintf(buf,"%u",x);
    return buf;
}


int NetType::toInt(const char *x) {
    return ACE_OS::atoi(x);
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
