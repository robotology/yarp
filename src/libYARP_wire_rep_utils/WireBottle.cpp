/*
 * Copyright (C) 2010 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <stdio.h>
#include "WireBottle.h"

#include <yarp/os/NetInt32.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/NetType.h>

using namespace yarp::os;

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
                return YARP_NULLPTR;
            }
            tag = getInt(cursor);
            cursor += 4;
            remaining -= 4;
        }
        //printf("tag is %d\n", tag);
        switch (tag) {
        case BOTTLE_TAG_INT:
        case BOTTLE_TAG_VOCAB:
            if (remaining<4) { return YARP_NULLPTR; }
            cursor += 4;
            remaining -= 4;
            break;
        case BOTTLE_TAG_DOUBLE:
            if (remaining<8) { return YARP_NULLPTR; }
            cursor += 8;
            remaining -= 8;
            break;
        case BOTTLE_TAG_STRING:
        case BOTTLE_TAG_BLOB:
            {
                if (remaining<4) {
                    return YARP_NULLPTR;
                }
                NetInt32 len = getInt(cursor);
                cursor += 4;
                remaining -= 4;
                if (len<0||len>remaining) {
                    return YARP_NULLPTR;
                }
                cursor += len;
                remaining -= len;
            }
            break;
        default:
            if (tag&BOTTLE_TAG_LIST) {
                if (remaining<4) {
                    return YARP_NULLPTR;
                }
                NetInt32 len = getInt(cursor);
                cursor += 4;
                remaining -= 4;
                cursor = checkBottle(cursor,remaining,len,tag&0xff);
                if (cursor == YARP_NULLPTR) {
                    return YARP_NULLPTR;
                }
            } else {
                return YARP_NULLPTR;
            }
            break;
        }
    }
    if (remaining!=0) { return YARP_NULLPTR; }
    if (ct!=0) { return YARP_NULLPTR; }
    return cursor;
}

bool WireBottle::checkBottle(void *cursor, int len) {
    int rem = len;
    return ::checkBottle((char *)cursor,rem,1,0) != YARP_NULLPTR;
}

bool WireBottle::extractBlobFromBottle(yarp::os::SizedWriter& src,
                                       SizedWriterTail& dest) {
    size_t total_len = 0;
    for (size_t i=0; i<src.length(); i++) {
        total_len += src.length(i);
    }
    bool has_header = false;
    int payload_index = 0;
    int payload_offset = 0;
    int remaining = total_len;
    if (src.length(0)>=12) {
        // could this be a Bottle compatible blob?
        char *base = (char*)src.data(0);
        Bytes b1(base,4);
        Bytes b2(base+4,4);
        Bytes b3(base+8,4);
        int i1 = NetType::netInt(b1);
        int i2 = NetType::netInt(b2);
        int i3 = NetType::netInt(b3);
        //dbg_printf(">>> %d %d %d\n", i1, i2, i3);
        if (i1==BOTTLE_TAG_LIST+BOTTLE_TAG_BLOB) {
            if (i2==1) {
                if ((size_t)i3==total_len - 12) {
                    // good enough
                    //dbg_printf("Header detected\n");
                    has_header = true;
                    payload_index = 0;
                    payload_offset = 12;
                    remaining -= payload_offset;
                }
            }
        }
    }
    if (has_header) {
        dest.setDelegate(&src,payload_index,payload_offset);
        return true;
    }
    return false;
}

