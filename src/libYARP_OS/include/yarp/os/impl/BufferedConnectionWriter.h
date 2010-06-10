// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_BUFFEREDCONNECTIONWRITER_
#define _YARP2_BUFFEREDCONNECTIONWRITER_

#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/impl/SizedWriter.h>
#include <yarp/ManagedBytes.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/NetType.h>
#include <yarp/os/impl/StringOutputStream.h>
#include <yarp/os/Vocab.h>

#include <ace/Vector_T.h>
#include <ace/OS_NS_stdlib.h>

namespace yarp {
    namespace os {
        namespace impl {
            class BufferedConnectionWriter;
            class ConnectionRecorder;
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

    bool write(ConnectionWriter& connection) {
		unsigned int i;
        for (i=0; i<header.size(); i++) {
            ManagedBytes& b = *(header[i]);
            connection.appendBlock(b.get(),b.used());
        }
        for (i=0; i<lst.size(); i++) {
            ManagedBytes& b = *(lst[i]);
            connection.appendBlock(b.get(),b.used());
        }
        return !connection.isError();
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
        ManagedBytes& b = *(lst[index-header.size()]);
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

    bool forceConvertTextMode();

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


/**
 * A helper for recording entire message/reply transactions
 */
class yarp::os::impl::ConnectionRecorder : public ConnectionReader, 
            public ConnectionWriter, public yarp::os::PortWriter {
private:
    ConnectionReader *reader;
    ConnectionWriter *writer;
    BufferedConnectionWriter readerStore;
    BufferedConnectionWriter writerStore;
    bool writing;
    bool wrote;
public:
    ConnectionRecorder() {
        reader = NULL;
        writing = false;
        wrote = false;
    }

    /**
     *
     * Call this to wrap a specific ConnectionReader.
     *
     */
    void init(ConnectionReader *wrappedReader) {
        reader = wrappedReader;
        if (reader->isTextMode()) {
            reader->convertTextMode();
        }
        writing = false;
    }

    /**
     *
     * Call this when all reading/writing has been done.
     *
     */
    void fini() {
        if (writing) {
            if (writer->isTextMode()) {
                writer->convertTextMode();
            }
            writing = false;
        }
    }

    virtual bool expectBlock(const char *data, int len) {
        bool ok = reader->expectBlock(data,len);
        if (ok) {
            readerStore.appendBlock(data,len);
        }
        return ok;
    }

    virtual ConstString expectText(int terminatingChar) {
        ConstString str = reader->expectText(terminatingChar);
        readerStore.appendString(str.c_str(),terminatingChar);
        return str;
    }

    virtual int expectInt() {
        int x = reader->expectInt();
        readerStore.appendInt(x);
        return x;
    }

    virtual double expectDouble() {
        double x = reader->expectDouble();
        readerStore.appendDouble(x);
        return x;
    }

    virtual bool isTextMode() {
        return false;
    }

    virtual bool convertTextMode() {
        return false;
    }

    virtual int getSize() {
        return reader->getSize();
    }


    virtual ConnectionWriter *getWriter() {
        writer = reader->getWriter();
        writing = true;
        wrote = true;
        return this;
    }

    virtual Portable *getReference() {
        return reader->getReference();
    }

    virtual Contact getRemoteContact() {
        return reader->getRemoteContact();
    }

    virtual Contact getLocalContact() {
        return reader->getLocalContact();
    }

    virtual bool isValid() {
        // shared
        if (writing) {
            return writer->isValid();
        }
        return reader->isValid();
    }

    virtual bool isActive() {
        // shared
        if (writing) {
            return writer->isActive();
        }
        return reader->isActive();
    }

    virtual bool isError() {
        // shared
        if (writing) {
            return writer->isError();
        }
        return reader->isError();
    }


    virtual void appendBlock(const char *data, int len) {
        writer->appendBlock(data,len);
        writerStore.appendBlock(data,len);
    }

    virtual void appendInt(int data) {
        writer->appendInt(data);
        writerStore.appendInt(data);
    }

    virtual void appendDouble(double data) {
        writer->appendDouble(data);
        writerStore.appendDouble(data);
    }

    virtual void appendString(const char *str, int terminate) {
        writer->appendString(str,terminate);
        writerStore.appendString(str,terminate);
    }

    virtual void appendExternalBlock(const char *data, int len) {
        writer->appendExternalBlock(data,len);
        writerStore.appendExternalBlock(data,len);
    }

    virtual void declareSizes(int argc, int *argv) {
        writer->declareSizes(argc,argv);
    }

    virtual void setReplyHandler(PortReader& reader) {
        writer->setReplyHandler(reader);
    }

    virtual void setReference(Portable *obj) {
        writer->setReference(obj);
    }

    virtual bool write(ConnectionWriter& connection) {
        if (hasReply()) {
            connection.appendInt(BOTTLE_TAG_LIST); // nested structure
            connection.appendInt(3);               // with three elements
            connection.appendInt(BOTTLE_TAG_VOCAB);
            connection.appendInt(VOCAB3('r','p','c'));
            bool ok = readerStore.write(connection);
            if (ok) {
                writerStore.write(connection);
            }
            return ok;
        } else {
            return readerStore.write(connection);
        }
    }

    BufferedConnectionWriter& getMessage() { return readerStore; }
    BufferedConnectionWriter& getReply() { return writerStore; }
    bool hasReply() { return wrote; }
};



#endif

