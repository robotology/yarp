// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_BUFFEREDCONNECTIONWRITER_
#define _YARP2_BUFFEREDCONNECTIONWRITER_

#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/impl/SizedWriter.h>
#include <yarp/ManagedBytes.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/NetType.h>
#include <yarp/os/impl/StringOutputStream.h>

#include <ace/Vector_T.h>
#include <ace/OS_NS_stdlib.h>

namespace yarp {
    namespace os {
        namespace impl {
            class BufferedConnectionWriter;
            using yarp::os::ConnectionWriter;
            using yarp::os::ConnectionReader;
        }
    }
    using os::ConnectionWriter;
    using os::ConnectionReader;
}

#define BUFFERED_CONNECTION_INITIAL_POOL_SIZE (1024)

/**
 * A helper for creating cached object descriptions.
 */
class yarp::os::impl::BufferedConnectionWriter : public ConnectionWriter, public SizedWriter {
public:

    BufferedConnectionWriter(bool textMode = false) : textMode(textMode) {
        reader = NULL;
        target = &lst;
        ref = NULL;
        stopPool();
    }

    virtual ~BufferedConnectionWriter() {
        clear();
    }

    void reset(bool textMode) {
        this->textMode = textMode;
        clear();
        reader = NULL;
        ref = NULL;
    }

    void clear() {
        target = &lst;
		unsigned int i;
        for (i=0; i<lst.size(); i++) {
            delete lst[i];
        }
        lst.clear();
        for (i=0; i<header.size(); i++) {
            delete header[i];
        }
        header.clear();
        stopPool();
    }

    bool addPool(const Bytes& data) {
        if (pool!=NULL) {
            if (data.length()+poolIndex>pool->length()) {
                pool = NULL;
            }
        }
        if (pool==NULL && data.length()<poolLength) {
            pool = new ManagedBytes(poolLength);
            if (pool==NULL) { return false; }
            poolCount++;
            poolIndex = 0;
            if (poolLength<65536) {
                poolLength *= 2;
            }
            pool->setUsed(0);
            target->push_back(pool);
        }
        if (pool!=NULL) {
            ACE_OS::memcpy(pool->get()+poolIndex,data.get(),data.length());
            poolIndex += data.length();
            pool->setUsed(poolIndex);
            return true;
        }
        return false;
    }
    
    void stopPool() {
        pool = NULL;
        poolIndex = 0;
        poolLength = BUFFERED_CONNECTION_INITIAL_POOL_SIZE;
        poolCount = 0;
    }

    virtual void appendBlock(const Bytes& data) {
        stopPool();
        target->push_back(new ManagedBytes(data,false));
    }

    virtual void appendBlockCopy(const Bytes& data) {
        if (addPool(data)) return;
        ManagedBytes *buf = new ManagedBytes(data,false);
        buf->copy();
        target->push_back(buf);
    }

    virtual void appendInt(int data) {
        NetType::NetInt32 i = data;
        Bytes b((char*)(&i),sizeof(i));
        if (addPool(b)) return;
        ManagedBytes *buf = new ManagedBytes(b,false);
        buf->copy();
        target->push_back(buf);
    }

    virtual void appendDouble(double data) {
        NetType::NetFloat64 i = data;
        Bytes b((char*)(&i),sizeof(i));
        if (addPool(b)) return;
        ManagedBytes *buf = new ManagedBytes(b,false);
        buf->copy();
        target->push_back(buf);
    }

    virtual void appendStringBase(const String& data) {
        Bytes b((char*)(data.c_str()),data.length()+1);
        if (addPool(b)) return;
        ManagedBytes *buf = new ManagedBytes(b,false);
        buf->copy();
        target->push_back(buf);
    }

    virtual void appendBlock(const String& data) {
        Bytes b((char*)(data.c_str()),data.length()+1);
        if (addPool(b)) return;
        ManagedBytes *buf = new ManagedBytes(b,false);
        target->push_back(buf);
    }

    virtual void appendLine(const String& data) {
        String copy = data;
        copy += "\r\n"; // for windows compatibility
        Bytes b((char*)(copy.c_str()),copy.length());
        if (addPool(b)) return;
        ManagedBytes *buf = new ManagedBytes(b,false);
        buf->copy();

        target->push_back(buf);
    }

    virtual bool isTextMode() {
        return textMode;
    }

    void write(OutputStream& os) {
		unsigned int i;
        for (i=0; i<header.size(); i++) {
            ManagedBytes& b = *(header[i]);
            os.write(b.usedBytes());
        }
        for (i=0; i<lst.size(); i++) {
            ManagedBytes& b = *(lst[i]);
            os.write(b.usedBytes());
        }    
    }

    virtual int length() {
        return header.size()+lst.size();
    }

    virtual int length(int index) {
        if (index<(int)header.size()) {
            ManagedBytes& b = *(header[index]);
            return b.used();
        }
        ManagedBytes& b = *(lst[index-header.size()]);
        return b.used();
    }

    virtual const char *data(int index) {
        if (index<(int)header.size()) {
            ManagedBytes& b = *(header[index]);
            return (const char *)b.get();
        }
        ManagedBytes& b = *(lst[index]);
        return (const char *)b.get();
    }


    String toString() {
        StringOutputStream sos;
        write(sos);
        return sos.toString();
    }


    // new interface

    virtual void appendBlock(const char *data, int len) {
        appendBlockCopy(Bytes((char*)data,len));
    }

    virtual void appendString(const char *str, int terminate = '\n') {
        if (terminate=='\n') {
            appendLine(str);
        } else if (terminate==0) {
            appendStringBase(str);
        } else {
            String s = str;
            str += terminate;
            appendBlockCopy(Bytes((char*)(s.c_str()),s.length()));
        }
    }

    virtual void appendExternalBlock(const char *data, int len) {
        appendBlock(Bytes((char*)data,len));
    }

    virtual void declareSizes(int argc, int *argv) {
        // cannot do anything with this yet
    }

    virtual void setReplyHandler(PortReader& reader) {
        this->reader = &reader;
    }

    virtual PortReader *getReplyHandler() {
        return reader;
    }

    virtual bool convertTextMode();

    void addToHeader() {
        stopPool();
        target = &header;
    }

    virtual yarp::os::Portable *getReference() {
        return ref;
    }

    virtual void setReference(yarp::os::Portable *obj) {
        ref = obj;
    }

    virtual bool isValid() {
        return true;
    }

    virtual bool isActive() {
        return true;
    }

    virtual bool isError() {
        return false;  // output errors are of no significance at user level
    }

private:
    ACE_Vector<ManagedBytes *> lst;
    ACE_Vector<ManagedBytes *> header;
    ACE_Vector<ManagedBytes *> *target;
    ManagedBytes *pool;
    int poolIndex;
    int poolCount;
    int poolLength;
    PortReader *reader;
    bool textMode;
    yarp::os::Portable *ref;
};

#endif

