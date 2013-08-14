// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_BUFFEREDCONNECTIONWRITER_
#define _YARP2_BUFFEREDCONNECTIONWRITER_

#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/SizedWriter.h>
#include <yarp/os/ManagedBytes.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/NetType.h>
#include <yarp/os/StringOutputStream.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/Bottle.h>

#include <yarp/os/impl/PlatformVector.h>
#include <yarp/os/impl/PlatformStdlib.h>

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
class YARP_OS_impl_API yarp::os::impl::BufferedConnectionWriter : public ConnectionWriter, public SizedWriter {
public:

    BufferedConnectionWriter(bool textMode = false) : textMode(textMode) {
        reader = NULL;
        target = &lst;
        ref = NULL;
        stopPool();
        shouldDrop = false;
        convertTextModePending = false;
    }

    virtual ~BufferedConnectionWriter() {
        clear();
    }

    void reset(bool textMode) {
        this->textMode = textMode;
        clear();
        reader = NULL;
        ref = NULL;
        convertTextModePending = false;
    }

    void clear() {
        target = &lst;
        size_t i;
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

    bool addPool(const yarp::os::Bytes& data) {
        if (pool!=NULL) {
            if (data.length()+poolIndex>pool->length()) {
                pool = NULL;
            }
        }
        if (pool==NULL && data.length()<poolLength) {
            pool = new yarp::os::ManagedBytes(poolLength);
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

    virtual void appendBlock(const yarp::os::Bytes& data) {
        stopPool();
        target->push_back(new yarp::os::ManagedBytes(data,false));
    }

    virtual void appendBlockCopy(const Bytes& data) {
        if (addPool(data)) return;
        yarp::os::ManagedBytes *buf = new yarp::os::ManagedBytes(data,false);
        buf->copy();
        target->push_back(buf);
    }

    virtual void appendInt(int data) {
        NetInt32 i = data;
        yarp::os::Bytes b((char*)(&i),sizeof(i));
        if (addPool(b)) return;
        yarp::os::ManagedBytes *buf = new yarp::os::ManagedBytes(b,false);
        buf->copy();
        target->push_back(buf);
    }

    virtual void appendDouble(double data) {
        NetFloat64 i = data;
        yarp::os::Bytes b((char*)(&i),sizeof(i));
        if (addPool(b)) return;
        yarp::os::ManagedBytes *buf = new yarp::os::ManagedBytes(b,false);
        buf->copy();
        target->push_back(buf);
    }

    virtual void appendStringBase(const String& data) {
        yarp::os::Bytes b((char*)(data.c_str()),data.length()+1);
        if (addPool(b)) return;
        yarp::os::ManagedBytes *buf = new yarp::os::ManagedBytes(b,false);
        buf->copy();
        target->push_back(buf);
    }

    virtual void appendBlock(const String& data) {
        Bytes b((char*)(data.c_str()),data.length()+1);
        if (addPool(b)) return;
        yarp::os::ManagedBytes *buf = new yarp::os::ManagedBytes(b,false);
        target->push_back(buf);
    }

    virtual void appendLine(const String& data) {
        String copy = data;
        copy += "\r\n"; // for windows compatibility
        yarp::os::Bytes b((char*)(copy.c_str()),copy.length());
        if (addPool(b)) return;
        yarp::os::ManagedBytes *buf = new yarp::os::ManagedBytes(b,false);
        buf->copy();

        target->push_back(buf);
    }

    virtual bool isTextMode() {
        return textMode;
    }

    bool write(ConnectionWriter& connection) {
        stopWrite();
        size_t i;
        for (i=0; i<header.size(); i++) {
            yarp::os::ManagedBytes& b = *(header[i]);
            connection.appendBlock(b.get(),b.used());
        }
        for (i=0; i<lst.size(); i++) {
            yarp::os::ManagedBytes& b = *(lst[i]);
            connection.appendBlock(b.get(),b.used());
        }
        return !connection.isError();
    }

    void write(OutputStream& os) {
        stopWrite();
        size_t i;
        for (i=0; i<header.size(); i++) {
            yarp::os::ManagedBytes& b = *(header[i]);
            os.write(b.usedBytes());
        }
        for (i=0; i<lst.size(); i++) {
            yarp::os::ManagedBytes& b = *(lst[i]);
            os.write(b.usedBytes());
        }
    }

    virtual size_t length() {
        return header.size()+lst.size();
    }

    virtual size_t headerLength() {
        return header.size();
    }

    virtual size_t length(size_t index) {
        if (index<header.size()) {
            yarp::os::ManagedBytes& b = *(header[index]);
            return b.used();
        }
        yarp::os::ManagedBytes& b = *(lst[index-header.size()]);
        return b.used();
    }

    virtual const char *data(size_t index) {
        if (index<header.size()) {
            yarp::os::ManagedBytes& b = *(header[index]);
            return (const char *)b.get();
        }
        yarp::os::ManagedBytes& b = *(lst[index-header.size()]);
        return (const char *)b.get();
    }


    String toString() {
        stopWrite();
        StringOutputStream sos;
        write(sos);
        return sos.toString();
    }


    // new interface

    virtual void appendBlock(const char *data, size_t len) {
        appendBlockCopy(yarp::os::Bytes((char*)data,len));
    }

    virtual void appendString(const char *str, int terminate = '\n') {
        if (terminate=='\n') {
            appendLine(str);
        } else if (terminate==0) {
            appendStringBase(str);
        } else {
            String s = str;
            str += terminate;
            appendBlockCopy(yarp::os::Bytes((char*)(s.c_str()),s.length()));
        }
    }

    virtual void appendExternalBlock(const char *data, size_t len) {
        appendBlock(yarp::os::Bytes((char*)data,len));
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

   virtual void requestDrop() {
        shouldDrop = true;
    }

    bool dropRequested() {
        return shouldDrop;
    }

    virtual void startWrite() {}

    virtual void stopWrite() {
        // convert, last thing, if requested
        applyConvertTextMode();
    }

private:
    bool applyConvertTextMode();


    PlatformVector<yarp::os::ManagedBytes *> lst;
    PlatformVector<yarp::os::ManagedBytes *> header;
    PlatformVector<yarp::os::ManagedBytes *> *target;
    yarp::os::ManagedBytes *pool;
    size_t poolIndex;
    size_t poolCount;
    size_t poolLength;
    yarp::os::PortReader *reader;
    bool textMode;
    bool convertTextModePending;
    yarp::os::Portable *ref;
    bool shouldDrop;
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
    bool skipNextInt;
    yarp::os::Bottle blank;
public:
    ConnectionRecorder() {
        reader = NULL;
        writing = false;
        wrote = false;
        skipNextInt = false;
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

    virtual bool expectBlock(const char *data, size_t len) {
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
        if (!skipNextInt) {
            readerStore.appendInt(x);
        } else {
            skipNextInt = false;
        }
        return x;
    }

    virtual bool pushInt(int x) {
        bool ok = reader->pushInt(x);
        skipNextInt = skipNextInt || ok;
        return ok;
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

    virtual size_t getSize() {
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


    virtual void appendBlock(const char *data, size_t len) {
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

    virtual void appendExternalBlock(const char *data, size_t len) {
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

    virtual void requestDrop() {
    }

    virtual yarp::os::Searchable& getConnectionModifiers() {
        return blank;
    }

    BufferedConnectionWriter& getMessage() { return readerStore; }
    BufferedConnectionWriter& getReply() { return writerStore; }
    bool hasReply() { return wrote; }
};



#endif
