// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2013 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/idl/WireWriter.h>

using namespace yarp::os;
using namespace yarp::os::idl;


WireWriter::WireWriter(ConnectionWriter& writer) : writer(writer) {
    get_mode = get_is_vocab = false;
    need_ok = false;
    writer.convertTextMode();
}

WireWriter::WireWriter(WireReader& reader) : writer(reader.getWriter()) {
    get_is_vocab = false;
    need_ok = false;
    writer.convertTextMode();
    get_mode = reader.getMode();
    if (get_mode) {
        get_string = reader.getString();
        get_is_vocab = reader.getIsVocab();
    }
}

WireWriter::~WireWriter() {
    if (need_ok) {
        writeBool(true);
    }
}

bool WireWriter::isNull() const {
    return writer.isNull();
}

bool WireWriter::write(WirePortable& obj) {
    return obj.write(*this);
}

bool WireWriter::write(yarp::os::PortWriter& obj) {
    return obj.write(writer);
}

bool WireWriter::writeNested(WirePortable& obj) {
    return obj.write(writer);
}

bool WireWriter::writeNested(yarp::os::PortWriter& obj) {
    return obj.write(writer);
}

bool WireWriter::writeI16(const YARP_INT16& x) {
    writer.appendInt(BOTTLE_TAG_INT);
    writer.appendInt((int)x);
    return !writer.isError();
}

bool WireWriter::writeI32(const YARP_INT32& x) {
    writer.appendInt(BOTTLE_TAG_INT);
    writer.appendInt((int)x);
    return !writer.isError();
}

bool WireWriter::writeI64(const YARP_INT64& x) {
    writer.appendInt(BOTTLE_TAG_INT64);
    writer.appendInt64(x);
    return !writer.isError();
}

bool WireWriter::writeBool(bool x) {
    writer.appendInt(BOTTLE_TAG_VOCAB);
    writer.appendInt(x?VOCAB2('o','k'):VOCAB4('f','a','i','l'));
    return !writer.isError();
}

bool WireWriter::writeByte(const YARP_INT8& x) {
    writer.appendInt(BOTTLE_TAG_INT);
    writer.appendInt((int)x);
    return !writer.isError();
}

bool WireWriter::writeDouble(double x) {
    writer.appendInt(BOTTLE_TAG_DOUBLE);
    writer.appendDouble(x);
    return !writer.isError();
}

bool WireWriter::writeVocab(int x) {
    writer.appendInt(BOTTLE_TAG_VOCAB);
    writer.appendInt((int)x);
    return !writer.isError();
}

bool WireWriter::isValid() {
    return writer.isValid();
}

bool WireWriter::isError() {
    return writer.isError();
}

bool WireWriter::writeTag(const char *tag, int split, int len) {
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

bool WireWriter::writeString(const yarp::os::ConstString& tag) {
    writer.appendInt(BOTTLE_TAG_STRING);
    writer.appendInt((int)tag.length());
    writer.appendBlock(tag.c_str(),tag.length());
    return !writer.isError();
}

bool WireWriter::writeBinary(const yarp::os::ConstString& tag) {
    writer.appendInt(BOTTLE_TAG_BLOB);
    writer.appendInt((int)tag.length());
    writer.appendBlock(tag.c_str(),tag.length());
    return !writer.isError();
}

bool WireWriter::writeListHeader(int len) {
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


bool WireWriter::writeListBegin(int tag, unsigned YARP_INT32 len) {
    // this should be optimized for double/int/etc
    writer.appendInt(BOTTLE_TAG_LIST);
    writer.appendInt((int)len);
    return !writer.isError();
}

bool WireWriter::writeSetBegin(int tag, unsigned YARP_INT32 len) {
    return writeListBegin(tag,len);
}

bool WireWriter::writeMapBegin(int tag, int tag2, unsigned YARP_INT32 len) {
    writer.appendInt(BOTTLE_TAG_LIST);
    writer.appendInt((int)len);
    return !writer.isError();
}

bool WireWriter::writeListEnd() {
    return true;
}

bool WireWriter::writeSetEnd() {
    return true;
}

bool WireWriter::writeMapEnd() {
    return true;
}

bool WireWriter::writeOnewayResponse() {
    if (!writeListHeader(1)) return false;
    writer.appendInt(BOTTLE_TAG_VOCAB);
    writer.appendInt(VOCAB4('d','o','n','e'));
    return true;
}
