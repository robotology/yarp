/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/DummyConnector.h>

#include <cstring>

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
        const std::string& str = sos.str();
        b.fromBinary(str.c_str(), str.length());
        std::string replacement = b.toString() + "\n";
        for (size_t i=0; i<lst.size(); i++) {
            delete lst[i];
        }
        lst_used = 0;
        target = &lst;
        lst.clear();
        stopPool();
        Bytes data((char*)replacement.c_str(), replacement.length());
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

std::string BufferedConnectionWriter::toString() {
    stopWrite();
    size_t total_size = dataSize();
    std::string output(total_size, 0);
    char *dest = (char *)output.c_str();
    for (size_t i=0; i<header_used; i++) {
        const char *data = header[i]->get();
        size_t len = header[i]->used();
        memmove(dest, data, len);
        dest += len;
    }
    for (size_t i=0; i<lst_used; i++) {
        const char *data = lst[i]->get();
        size_t len = lst[i]->used();
        memmove(dest, data, len);
        dest += len;
    }
    return output;
}


bool BufferedConnectionWriter::addPool(const yarp::os::Bytes& data) {
    if (pool != nullptr) {
        if (data.length()+poolIndex>pool->length()) {
            pool = nullptr;
        }
    }
    if (pool == nullptr && data.length() < poolLength) {
        bool add = false;
        if (*target_used < target->size()) {
            yarp::os::ManagedBytes*&bytes = (*target)[*target_used];
            if (bytes->length()<poolLength) {
                delete bytes;
                bytes = new yarp::os::ManagedBytes(poolLength);
            }
            pool = bytes;
            if (pool == nullptr) {
                return false;
            }
        } else {
            pool = new yarp::os::ManagedBytes(poolLength);
            if (pool == nullptr) {
                return false;
            }
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
    if (pool != nullptr) {
        memcpy(pool->get()+poolIndex, data.get(), data.length());
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
    yarp::os::ManagedBytes *buf = nullptr;
    if (*target_used < target->size()) {
        yarp::os::ManagedBytes*&bytes = (*target)[*target_used];
        if (bytes->isOwner()!=copy||bytes->length()<data.length()) {
            delete bytes;
            bytes = new yarp::os::ManagedBytes(data, false);
            if (copy) {
                bytes->copy();
            }
            (*target_used)++;
            return;
        }
        buf = bytes;
        bytes->setUsed(data.length());
    }
    if (buf == nullptr) {
        buf = new yarp::os::ManagedBytes(data, false);
        if (copy) buf->copy();
        target->push_back(buf);
    } else {
        if (copy) {
            buf->copy();
            memmove(buf->get(), data.get(), data.length());
        } else {
            *buf = ManagedBytes(data, false);
        }
    }
    (*target_used)++;
}

void BufferedConnectionWriter::restart() {
    lst_used = 0;
    header_used = 0;
    reader = nullptr;
    ref = nullptr;
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
