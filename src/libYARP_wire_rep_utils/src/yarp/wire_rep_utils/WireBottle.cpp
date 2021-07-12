/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <cstdio>
#include "WireBottle.h"

#include <yarp/os/NetInt32.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/NetType.h>

using namespace yarp::os;
using namespace yarp::wire_rep_utils;

static NetInt32 getInt(char *cursor) {
    auto* icursor = reinterpret_cast<NetInt32 *> (cursor);
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
                return nullptr;
            }
            tag = getInt(cursor);
            cursor += 4;
            remaining -= 4;
        }
        //printf("tag is %d\n", tag);
        switch (tag) {
        case BOTTLE_TAG_INT32:
        case BOTTLE_TAG_VOCAB32:
            if (remaining<4) { return nullptr; }
            cursor += 4;
            remaining -= 4;
            break;
        case BOTTLE_TAG_FLOAT64:
            if (remaining<8) { return nullptr; }
            cursor += 8;
            remaining -= 8;
            break;
        case BOTTLE_TAG_STRING:
        case BOTTLE_TAG_BLOB:
            {
                if (remaining<4) {
                    return nullptr;
                }
                NetInt32 len = getInt(cursor);
                cursor += 4;
                remaining -= 4;
                if (len<0||len>remaining) {
                    return nullptr;
                }
                cursor += len;
                remaining -= len;
            }
            break;
        default:
            if (tag&BOTTLE_TAG_LIST) {
                if (remaining<4) {
                    return nullptr;
                }
                NetInt32 len = getInt(cursor);
                cursor += 4;
                remaining -= 4;
                cursor = checkBottle(cursor,remaining,len,tag&0xff);
                if (cursor == nullptr) {
                    return nullptr;
                }
            } else {
                return nullptr;
            }
            break;
        }
    }
    if (remaining!=0) { return nullptr; }
    if (ct!=0) { return nullptr; }
    return cursor;
}

bool WireBottle::checkBottle(void *cursor, int len) {
    int rem = len;
    return ::checkBottle((char *)cursor,rem,1,0) != nullptr;
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
