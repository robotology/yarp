// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_BUFFEREDBLOCKWRITER_
#define _YARP2_BUFFEREDBLOCKWRITER_

#include <yarp/os/ConnectionWriter.h>
#include <yarp/SizedWriter.h>
#include <yarp/ManagedBytes.h>
#include <yarp/Logger.h>
#include <yarp/NetType.h>
#include <yarp/StringOutputStream.h>

#include <ace/Vector_T.h>

namespace yarp {
    class BufferedConnectionWriter;
    using os::ConnectionWriter;
    using os::ConnectionReader;
}

/**
 * A helper for creating cached object descriptions.
 */
class yarp::BufferedConnectionWriter : public ConnectionWriter, public SizedWriter {
public:

    BufferedConnectionWriter(bool textMode = false) : textMode(textMode) {
        reader = NULL;
        target = &lst;
        ref = NULL;
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
    }

    virtual void appendBlock(const Bytes& data) {
        target->push_back(new ManagedBytes(data,false));
    }

    virtual void appendBlockCopy(const Bytes& data) {
        ManagedBytes *buf = new ManagedBytes(data,false);
        buf->copy();
        target->push_back(buf);
    }

    virtual void appendInt(int data) {
        NetType::NetInt32 i = data;
        Bytes b((char*)(&i),sizeof(i));
        ManagedBytes *buf = new ManagedBytes(b,false);
        buf->copy();
        target->push_back(buf);
    }

    virtual void appendDouble(double data) {
        NetType::NetFloat64 i = data;
        Bytes b((char*)(&i),sizeof(i));
        ManagedBytes *buf = new ManagedBytes(b,false);
        buf->copy();
        target->push_back(buf);
    }

    virtual void appendStringBase(const String& data) {
        Bytes b((char*)(data.c_str()),data.length()+1);
        ManagedBytes *buf = new ManagedBytes(b,false);
        buf->copy();
        target->push_back(buf);
    }

    virtual void appendBlock(const String& data) {
        Bytes b((char*)(data.c_str()),data.length()+1);
        ManagedBytes *buf = new ManagedBytes(b,false);
        target->push_back(buf);
    }

    virtual void appendLine(const String& data) {
        String copy = data;
        copy += "\r\n"; // better windows support
        //copy += '\n';
        Bytes b((char*)(copy.c_str()),copy.length());
        ManagedBytes *buf = new ManagedBytes(b,false);
        buf->copy();

        //ACE_DEBUG((LM_DEBUG,"adding a line - %d bytes", copy.length()));

        target->push_back(buf);
    }

    virtual bool isTextMode() {
        return textMode;
    }

    void write(OutputStream& os) {
		unsigned int i;
        for (i=0; i<header.size(); i++) {
            ManagedBytes& b = *(header[i]);
            os.write(b.bytes());
        }
        for (i=0; i<lst.size(); i++) {
            ManagedBytes& b = *(lst[i]);
            os.write(b.bytes());
        }    
    }

    virtual int length() {
        return header.size()+lst.size();
    }

    virtual int length(int index) {
        if (index<(int)header.size()) {
            ManagedBytes& b = *(header[index]);
            return b.length();
        }
        ManagedBytes& b = *(lst[index-header.size()]);
        return b.length();
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
    PortReader *reader;
    bool textMode;
    yarp::os::Portable *ref;
};

#endif

