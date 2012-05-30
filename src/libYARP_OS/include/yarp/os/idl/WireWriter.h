// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_WIREWRITER_
#define _YARP2_WIREWRITER_

#include <yarp/os/api.h>
#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/idl/WireReader.h>
#include <yarp/os/idl/WirePortable.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/Bottle.h>

namespace yarp {
    namespace os {
        namespace idl {
            class WireWriter;
        }
    }
}

/**
 *
 * IDL-friendly connection writer.
 *
 */
class YARP_OS_API yarp::os::idl::WireWriter {
private:
    bool get_mode;
    std::string get_string;
    bool get_is_vocab;
    bool need_ok;
public:
    ConnectionWriter& writer;

    WireWriter(ConnectionWriter& writer) : writer(writer) {
        get_mode = get_is_vocab = false;
        need_ok = false;
        writer.convertTextMode();
    }

    WireWriter(WireReader& reader) : writer(reader.getWriter()) {
        get_is_vocab = false;
        need_ok = false;
        writer.convertTextMode();
        get_mode = reader.get_mode;
        if (get_mode) {
            get_string = reader.get_string;
            get_is_vocab = reader.get_is_vocab;
        }
    }

    ~WireWriter() {
        if (need_ok) {
            writeBool(true);
        }
    }

    bool write(WirePortable& obj) {
        return obj.write(*this);
    }

    bool write(yarp::os::PortWriter& obj) {
        return obj.write(writer);
    }

    bool writeNested(WirePortable& obj) {
        return obj.write(writer);
    }

    bool writeNested(yarp::os::PortWriter& obj) {
        return obj.write(writer);
    }

    bool writeI32(int32_t x) {
        writer.appendInt(BOTTLE_TAG_INT);
        writer.appendInt((int)x);
        return !writer.isError();
    }

    bool writeBool(bool x) {
        writer.appendInt(BOTTLE_TAG_VOCAB);
        writer.appendInt(x?VOCAB2('o','k'):VOCAB4('f','a','i','l'));
        return !writer.isError();
    }

    bool writeDouble(double x) {
        writer.appendInt(BOTTLE_TAG_DOUBLE);
        writer.appendDouble(x);
        return !writer.isError();
    }

    bool isValid() {
        return writer.isValid();
    }

    bool isError() {
        return writer.isError();
    }

    bool writeTag(const char *tag, int split, int len) {
        if (!split) {
            return writeString(tag);
        }
        ConstString bit = "";
        char ch = 'x';
        while (ch!='\0') {
            ch = *tag;
            tag++;
            if (ch=='\0'||ch=='_') {
                if (bit.length()<=4) {
                    writer.appendInt(BOTTLE_TAG_VOCAB);
                    writer.appendInt(Vocab::encode(bit));
                } else {
                    writeString(bit.c_str());
                }
                bit = "";
            } else {
                bit += ch;
            }
        }
        return true;
    }

    bool writeString(const std::string& tag) {
        writer.appendInt(BOTTLE_TAG_STRING);
        writer.appendInt((int)tag.length()+1);
        writer.appendString(tag.c_str(),'\0');
        return !writer.isError();
    }

    bool writeListHeader(int len) {
        writer.appendInt(BOTTLE_TAG_LIST);
        if (get_mode) {
            writer.appendInt(len+3);
            writer.appendInt(BOTTLE_TAG_VOCAB);
            writer.appendInt(VOCAB2('i','s'));
            if (get_is_vocab) {
                writer.appendInt(BOTTLE_TAG_VOCAB);
                writer.appendInt(Vocab::encode(get_string.c_str()));
            } else {
                writeString(get_string);
            }
            need_ok = true;
        } else {
            writer.appendInt(len);
        }
        return !writer.isError();
    }


    bool writeListBegin(int tag, uint32_t len) {
        // this should be optimized for double/int/etc
        writer.appendInt(BOTTLE_TAG_LIST);
        writer.appendInt((int)len);
        return !writer.isError();
    }

    bool writeSetBegin(int tag, uint32_t len) {
        return writeListBegin(tag,len);
    }

    bool writeMapBegin(int tag, int tag2, uint32_t len) {
        writer.appendInt(BOTTLE_TAG_LIST);
        writer.appendInt((int)len);
        return !writer.isError();
    }

    bool writeListEnd() {
        return true;
    }

    bool writeSetEnd() {
        return true;
    }

    bool writeMapEnd() {
        return true;
    }
};

#endif
