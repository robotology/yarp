// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/impl/PlatformStdlib.h>

#include <yarp/os/ManagedBytes.h>
#include <yarp/os/Bottle.h>

using namespace yarp::os;

void ManagedBytes::copy() {
    if (!owned) {
        ssize_t len = length();
        char *buf = new char[len];
        yarp::os::NetworkBase::assertion(buf!=NULL);
        ACE_OS::memcpy(buf,get(),len);
        b = Bytes(buf,len);
        owned = true;
    }
}

bool ManagedBytes::allocateOnNeed(size_t neededLen, size_t allocateLen) {
    if (length()<neededLen && allocateLen>=neededLen) {
        char *buf = new char[allocateLen];
        yarp::os::NetworkBase::assertion(buf!=NULL);
        ACE_OS::memcpy(buf,get(),length());
        if (owned) {
            delete[] get();
            owned = false;
        }
        b = Bytes(buf,allocateLen);
        owned = true;
        return true;
    }
    return false;
}


bool ManagedBytes::read(ConnectionReader& reader) {
    reader.convertTextMode();
    int listTag;
    int listLen;
    int blobLen;
    listTag = reader.expectInt();
    listLen = reader.expectInt();
    blobLen = reader.expectInt();
    if (listTag!=BOTTLE_TAG_LIST+BOTTLE_TAG_BLOB) {
        return false;
    }
    if (listLen!=1) {
        return false;
    }
    allocate(blobLen);
    if (get()==NULL) {
        return false;
    }
    return reader.expectBlock(get(),length());
}

bool ManagedBytes::write(ConnectionWriter& writer) {
    writer.appendInt(BOTTLE_TAG_LIST+BOTTLE_TAG_BLOB);
    writer.appendInt(1);
    writer.appendInt((int)length());
    writer.appendExternalBlock(get(),length());
    writer.convertTextMode();
    return !writer.isError();
}
