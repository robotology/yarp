// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <stdio.h>
#include "WireBottle.h"

#include <yarp/os/NetInt32.h>
#include <yarp/os/Bottle.h>

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

bool WireBottle::checkBottle(void *cursor, int len) {
    int rem = len;
    return ::checkBottle((char *)cursor,rem,1,0)!=NULL;
}

