// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include "WireTwiddler.h"

#include <vector>

#include <stdio.h>

using namespace yarp::os;
using namespace std;

int WireTwiddler::configure(Bottle& desc, int offset) {
    // example: list 4 int32 * float64 * vector int32 * vector int32 3 *
    bool is_vector = false;
    bool is_list = false;
    ConstString kind = desc.get(offset).asString();
    offset++;
    is_vector = (kind=="vector");
    is_list = (kind=="list");
    if (is_vector) {
        kind = desc.get(offset).asString();
        offset++;
    }
    int len = 1;
    bool data = false;
    if (is_vector||is_list) {
        Value v = desc.get(offset);
        offset++;
        if (v.isInt()) {
            len = v.asInt();
        } else {
            if (v.asString()!="*") {
                fprintf(stderr,"Does not look like length: %s\n", 
                        v.toString().c_str());
            }
            len = -1;
            if (is_list) {
                fprintf(stderr,"List should have fixed length\n");
                len = 0;
            }
            data = true;
        }
    }
    if (!is_list) {
        if (!data) {
            Value v = desc.get(offset);
            offset++;
            if (v.asString()!="*") {
                fprintf(stderr,"Does not look like data: %s\n", 
                        v.toString().c_str());
            }
            data = true;
        }
    }

    int tag = 0;
    int unit_length = 0;
    if (kind=="int32") {
        tag = BOTTLE_TAG_INT;
        unit_length = 4;
    } else if (kind=="float64") {
        tag = BOTTLE_TAG_DOUBLE;
        unit_length = 8;
    } else if (kind=="string") {
        tag = BOTTLE_TAG_STRING;
        unit_length = -1;
    }

    printf("Type %s (%s) len %d\n", 
           kind.c_str(),
           is_list?"LIST":(is_vector?"VECTOR":"PRIMITIVE"), len);

    if (is_list) {
        buffer.push_back(BOTTLE_TAG_LIST);
        buffer.push_back(len);
    } else if (is_vector) {
        buffer.push_back(BOTTLE_TAG_LIST+tag);
        if (len!=-1) {
            buffer.push_back(len);
        }
    } else {
        buffer.push_back(tag);
    }

    if (data) {
        WireTwiddlerGap gap;
        gap.buffer_start = buffer_start;
        gap.buffer_length = (int)buffer.size()-buffer_start;
        buffer_start = (int)buffer.size();
        gap.unit_length = unit_length;
        gap.length = len;
        gaps.push_back(gap);
    }

    if (is_list) {
        for (int i=0; i<len; i++) {
            offset = configure(desc,offset);
        }
    }
    return offset;
}

bool WireTwiddler::configure(const char *txt) {
    Bottle desc(txt);

    buffer_start = 0;
    buffer.clear();
    int at = configure(desc,0);
    if (buffer_start!=(int)buffer.size()) {
        WireTwiddlerGap gap;
        gap.buffer_start = buffer_start;
        gap.buffer_length = (int)buffer.size()-buffer_start;
        buffer_start = (int)buffer.size();
        gaps.push_back(gap);
    }
    printf("buffer has %d items\n", (int)buffer.size());
    printf("gaps has %d items\n", (int)gaps.size());
    show();
    return at == desc.size();
}


void WireTwiddler::show() {
    for (int i=0; i<(int)gaps.size(); i++) {
        WireTwiddlerGap& gap = gaps[i];
        printf("Unit %d\n", i);
        if (gap.buffer_length!=0) {
            printf("  Buffer from %d to %d\n", gap.buffer_start, 
                   gap.buffer_start+gap.buffer_length-1);
        }
        if (gap.unit_length!=0) {
            printf("  Expect %d x %d\n", gap.length, gap.unit_length);
        }
    }
}

