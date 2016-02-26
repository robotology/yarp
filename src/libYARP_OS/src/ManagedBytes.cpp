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

ManagedBytes::ManagedBytes() :
        Portable(),
        b(Bytes(NULL, 0)),
        owned(false),
        use(0),
        use_set(false) {
}

ManagedBytes::ManagedBytes(size_t len) :
        Portable(),
        b(Bytes(new char[len],len)),
        owned(true),
        use(0),
        use_set(false) {
}

ManagedBytes::ManagedBytes(const Bytes& ext, bool owned) :
        Portable(),
        b(ext),
        owned(owned),
        use(0),
        use_set(false) {
}

ManagedBytes::ManagedBytes(const ManagedBytes& alt) :
        Portable(),
        b(alt.b),
        owned(false),
        use(0),
        use_set(false) {
    if (alt.owned) {
        copy();
    }
}

const ManagedBytes &ManagedBytes::operator=(const ManagedBytes& alt) {
    if (&alt != this) {
        clear();
        b = alt.b;
        use = alt.use;
        use_set = alt.use_set;
        owned = false;
        if (alt.owned) {
            copy();
        }
    }
    return *this;
}

ManagedBytes::~ManagedBytes() {
    clear();
}


void ManagedBytes::allocate(size_t len) {
    clear();
    char *buf = new char[len];
    b = Bytes(buf,len);
    owned = true;
    use = 0;
    use_set = false;
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

void ManagedBytes::copy() {
    if (!owned) {
        YARP_SSIZE_T len = length();
        char *buf = new char[len];
        yarp::os::NetworkBase::assertion(buf!=NULL);
        ACE_OS::memcpy(buf,get(),len);
        b = Bytes(buf,len);
        owned = true;
    }
}

size_t ManagedBytes::length() const {
    return b.length();
}

size_t ManagedBytes::used() const {
    return use_set ? use : length();
}

char *ManagedBytes::get() const {
    return b.get();
}

void ManagedBytes::clear() {
    if (owned) {
        if (get()!=0) {
            delete[] get();
        }
        owned = 0;
    }
    b = Bytes(NULL, 0);
    use = 0;
    use_set = false;
}

const Bytes& ManagedBytes::bytes() {
    return b;
}

Bytes ManagedBytes::usedBytes() {
    return Bytes(get(),used());
}

size_t ManagedBytes::setUsed(size_t used) {
    use_set = true;
    use = used;
    return this->used();
}

size_t ManagedBytes::resetUsed() {
    use = 0;
    use_set = false;
    return this->used();
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
