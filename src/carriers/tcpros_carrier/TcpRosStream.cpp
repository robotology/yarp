// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include "TcpRosStream.h"

#include <yarp/os/impl/NetType.h>
#include <yarp/os/NetInt32.h>
#include <yarp/os/Bottle.h>

using namespace yarp::os;
using namespace yarp::os::impl;
using namespace std;

static NetInt32 getInt(char *cursor) {
    NetInt32 *icursor = (NetInt32 *)cursor;
    return *icursor;
}

static char *checkBottle(char *cursor, int& remaining, int ct, int list_tag) {
    while (remaining>0 && ct>0) {
        ct--;
        //printf("at %ld : %d : %d\n", (long int) cursor, remaining, list_tag);
        int tag = 0;
        if (list_tag!=0) {
            tag = list_tag;
        } else {
            if (remaining<4) {
                return NULL;
            }
            tag = getInt(cursor);
            cursor += 4;
            remaining -= 4;
        }
        //printf("tag is %d\n", tag);
        switch (tag) {
        case BOTTLE_TAG_INT:
        case BOTTLE_TAG_VOCAB:
            if (remaining<4) { return NULL; }
            cursor += 4;
            remaining -= 4;
            break;
        case BOTTLE_TAG_DOUBLE:
            if (remaining<8) { return NULL; }
            cursor += 8;
            remaining -= 8;
            break;
        case BOTTLE_TAG_STRING:
        case BOTTLE_TAG_BLOB:
            {
                if (remaining<4) {
                    return NULL;
                }
                NetInt32 len = getInt(cursor);
                cursor += 4;
                remaining -= 4;
                if (len<0||len>remaining) {
                    return NULL;
                }
                cursor += len;
                remaining -= len;
            }
            break;
        default:
            if (tag&BOTTLE_TAG_LIST) {
                if (remaining<4) {
                    return NULL;
                }
                NetInt32 len = getInt(cursor);
                cursor += 4;
                remaining -= 4;
                cursor = checkBottle(cursor,remaining,len,tag&0xff);
                if (cursor==NULL) return NULL;
            } else {
                return NULL;
            }
            break;
        }
    }
    if (remaining!=0) { return NULL; }
    if (ct!=0) { return NULL; }
    return cursor;
}

int TcpRosStream::read(const Bytes& b) {
    if (phase==-1) return -1;
    if (remaining==0) {
        if (phase==1) {
            phase = 2;
            if (scan.length()>0) {
                cursor = scan.get();
                remaining = scan.length();
            } else {
                cursor = NULL;
                remaining = header.blobLen;
            }
        } else {
            scan.clear();
            phase = 0;
        }
    }
    if (phase==0) {
        if (expectTwiddle) {
            // I have no idea what this is yet, but let's consume it for now.
            // It is probably frightfully important.
            char twiddle[1];
            Bytes twiddle_buf(twiddle,1);
            NetType::readFull(delegate->getInputStream(),twiddle_buf);
        }

        char mlen[4];
        Bytes mlen_buf(mlen,4);
        int res = NetType::readFull(delegate->getInputStream(),mlen_buf);
        if (res<4) {
            //printf("tcpros_carrier failed, %s %d\n", __FILE__, __LINE__);
            phase = -1;
            return -1;
        }
        int len = NetType::netInt(mlen_buf);
        //printf("Unit length %d\n", len);

        if (raw==-1) {
            scan.allocate(len);
            int res = NetType::readFull(delegate->getInputStream(),
                                        scan.bytes());
            if (res<0) {
                //printf("tcpros_carrier failed, %s %d\n", __FILE__, __LINE__);
                phase = -1;
                return -1;
            }
            int len_scan = scan.length();
            if (checkBottle(scan.get(),len_scan,1,0)!=NULL) {
                //printf("Looks YARP-compatible\n");
                raw = 1;
            } else {
                //printf("Looks strange, blobbing...\n");
                raw = 0;
            }
        }

        header.init(len);
        if (raw==1) {
            phase = 2;
            if (scan.length()>0) {
                cursor = scan.get();
                remaining = scan.length();
            } else {
                cursor = NULL;
                remaining = header.blobLen;
            }
        } else {
            phase = 1;
            cursor = (char*) &header;
            remaining = sizeof(header);
        }
    }
    if (remaining>0) {
        if (cursor!=NULL) {
            int allow = remaining;
            if (b.length()<allow) {
                allow = b.length();
            }
            memcpy(b.get(),cursor,allow);
            cursor+=allow;
            remaining-=allow;
            //printf("%d bytes of header\n", allow);
            return allow;
        } else {
            int result = delegate->getInputStream().read(b);
            if (result>0) {
                remaining-=result;
                //printf("%d bytes of meat\n", result);
                return result;
            }
        }
    }
    phase = -1;
    return -1;
}


void TcpRosStream::write(const Bytes& b) {
    delegate->getOutputStream().write(b);
}
