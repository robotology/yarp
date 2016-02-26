/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/DummyConnector.h>

using namespace yarp::os::impl;
using namespace yarp::os;

bool BufferedConnectionWriter::applyConvertTextMode() {
    if (convertTextModePending) {
        convertTextModePending = false;

        Bottle b;
        StringOutputStream sos;
        for (size_t i=0; i<lst_used; i++) {
            yarp::os::ManagedBytes& m = *(lst[i]);
            sos.write(m.usedBytes());
        }
        const String& str = sos.str();
        b.fromBinary(str.c_str(),str.length());
        ConstString replacement = b.toString() + "\n";
        for (size_t i=0; i<lst.size(); i++) {
            delete lst[i];
        }
        lst_used = 0;
        target = &lst;
        lst.clear();
        stopPool();
        Bytes data((char*)replacement.c_str(),replacement.length());
        appendBlockCopy(data);
    }
    return true;
}

bool BufferedConnectionWriter::convertTextMode() {
    if (isTextMode()) {
        convertTextModePending = true;
    }
    return true;
}

String BufferedConnectionWriter::toString() {
    stopWrite();
    size_t total_size = dataSize();
    String output(total_size,0);
    char *dest = (char *)output.c_str();
    for (size_t i=0; i<header_used; i++) {
        const char *data = header[i]->get();
        size_t len = header[i]->used();
        memmove(dest,data,len);
        dest += len;
    }
    for (size_t i=0; i<lst_used; i++) {
        const char *data = lst[i]->get();
        size_t len = lst[i]->used();
        memmove(dest,data,len);
        dest += len;
    }
    return output;
}


bool BufferedConnectionWriter::addPool(const yarp::os::Bytes& data) {
    if (pool!=NULL) {
        if (data.length()+poolIndex>pool->length()) {
            pool = NULL;
        }
    }
    if (pool==NULL && data.length()<poolLength) {
        bool add = false;
        if (*target_used < target->size()) {
            yarp::os::ManagedBytes*&bytes = (*target)[*target_used];
            if (bytes->length()<poolLength) {
                delete bytes;
                bytes = new yarp::os::ManagedBytes(poolLength);
            }
            pool = bytes;
            if (pool==NULL) { return false; }
        } else {
            pool = new yarp::os::ManagedBytes(poolLength);
            if (pool==NULL) { return false; }
            add = true;
        }
        (*target_used)++;
        poolCount++;
        poolIndex = 0;
        if (poolLength<65536) {
            poolLength *= 2;
        }
        pool->setUsed(0);
        if (add) target->push_back(pool);
    }
    if (pool!=NULL) {
        ACE_OS::memcpy(pool->get()+poolIndex,data.get(),data.length());
        poolIndex += data.length();
        pool->setUsed(poolIndex);
        return true;
    }
    return false;
}


void BufferedConnectionWriter::push(const Bytes& data, bool copy) {
    if (copy) {
        if (addPool(data)) return;
    }
    yarp::os::ManagedBytes *buf = NULL;
    if (*target_used < target->size()) {
        yarp::os::ManagedBytes*&bytes = (*target)[*target_used];
        if (bytes->isOwner()!=copy||bytes->length()<data.length()) {
            delete bytes;
            bytes = new yarp::os::ManagedBytes(data,false);
            if (copy) bytes->copy();
            (*target_used)++;
            return;
        }
        buf = bytes;
        bytes->setUsed(data.length());
    } 
    if (buf == NULL) {
        buf = new yarp::os::ManagedBytes(data,false);
        if (copy) buf->copy();
        target->push_back(buf);
    } else {
        if (copy) {
            buf->copy();
            memmove(buf->get(),data.get(),data.length());
        } else {
            *buf = ManagedBytes(data,false);
        }
    }
    (*target_used)++;
}

void BufferedConnectionWriter::restart() {
    lst_used = 0;
    header_used = 0;
    reader = NULL;
    ref = NULL;
    convertTextModePending = false;
    target = &lst;
    target_used = &lst_used;
    stopPool();
}


void BufferedConnectionWriter::write(OutputStream& os) {
    stopWrite();
    for (size_t i=0; i<header_used; i++) {
        yarp::os::ManagedBytes& b = *(header[i]);
        os.write(b.usedBytes());
    }
    for (size_t i=0; i<lst_used; i++) {
        yarp::os::ManagedBytes& b = *(lst[i]);
        os.write(b.usedBytes());
    }
}


bool BufferedConnectionWriter::write(PortReader& obj) {
    DummyConnector con;
    con.setTextMode(isTextMode());
    if (!write(con.getWriter())) return false;
    return obj.read(con.getReader());
}
