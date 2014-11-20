// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2013 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/idl/WireReader.h>

using namespace yarp::os::idl;
using namespace yarp::os;

WireReader::WireReader(ConnectionReader& reader) : reader(reader) {
    reader.convertTextMode();
    state = &baseState;
    flush_if_needed = false;
    get_mode = false;
    support_get_mode = false;
    expecting = false;
}

WireReader::~WireReader() {
    if (state->need_ok) {
        YARP_INT32 dummy;
        readVocab(dummy);
        state->need_ok = false;
    }
    if (flush_if_needed) {
        clear();
    }
}

void WireReader::expectAccept() {
    expecting = true;
    flush_if_needed = true;
}

void WireReader::accept() {
    expecting = false;
}

void WireReader::allowGetMode() {
    support_get_mode = true;
}

bool WireReader::clear() {
    size_t pending = reader.getSize();
    if (pending>0) {
        while (pending>0) {
            char buf[1000];
            size_t next = (pending<sizeof(buf))?pending:sizeof(buf);
            reader.expectBlock(&buf[0],next);
            pending -= next;
        }
        return true;
    }
    return false;
}

void WireReader::fail() {
    clear();
    Bottle b("[fail]");
    b.write(getWriter());
}

bool WireReader::read(WirePortable& obj) {
    return obj.read(*this);
}

bool WireReader::read(yarp::os::PortReader& obj) {
    return obj.read(reader);
}

bool WireReader::readNested(WirePortable& obj) {
    return obj.read(reader);
}

bool WireReader::readNested(yarp::os::PortReader& obj) {
    return obj.read(reader);
}

bool WireReader::readI16(YARP_INT16& x) {
    int tag = state->code;
    if (tag<0) {
        if (noMore()) return false;
        tag = reader.expectInt();
    }
    if (tag!=BOTTLE_TAG_INT) return false;
    if (noMore()) return false;
    int v = reader.expectInt();
    x = (YARP_INT16) v;
    state->len--;
    return !reader.isError();
}

bool WireReader::readI32(YARP_INT32& x) {
    int tag = state->code;
    if (tag<0) {
        if (noMore()) return false;
        tag = reader.expectInt();
    }
    if (tag!=BOTTLE_TAG_INT) return false;
    if (noMore()) return false;
    int v = reader.expectInt();
    x = (YARP_INT32) v;
    state->len--;
    return !reader.isError();
}

bool WireReader::readI64(YARP_INT64& x) {
    int tag = state->code;
    if (tag<0) {
        if (noMore()) return false;
        tag = reader.expectInt();
    }
    if (tag!=BOTTLE_TAG_INT && tag!=BOTTLE_TAG_INT64) return false;
    if (noMore()) return false;
    if (tag==BOTTLE_TAG_INT) {
        int v = reader.expectInt();
        x = (YARP_INT32) v;
    } else {
        x = reader.expectInt64();
    }
    state->len--;
    return !reader.isError();
}

bool WireReader::readBool(bool& x) {
    if (state->code<0) {
        if (noMore()) return false;
        int tag = reader.expectInt();
        if (tag!=BOTTLE_TAG_INT&&tag!=BOTTLE_TAG_VOCAB) return false;
    }
    if (noMore()) return false;
    int v = reader.expectInt();
    x = (v!=0) && (v!=VOCAB4('f','a','i','l'));
    state->len--;
    return !reader.isError();
}

bool WireReader::readByte(YARP_INT8& x) {
    int tag = state->code;
    if (tag<0) {
        if (noMore()) return false;
        tag = reader.expectInt();
    }
    if (tag!=BOTTLE_TAG_INT) return false;
    if (noMore()) return false;
    int v = reader.expectInt();
    x = (YARP_INT8) v;
    state->len--;
    return !reader.isError();
}

bool WireReader::readVocab(YARP_INT32& x) {
    int tag = state->code;
    if (tag<0) {
        if (noMore()) return false;
        tag = reader.expectInt();
    }
    if (tag!=BOTTLE_TAG_VOCAB) return false;
    if (noMore()) return false;
    int v = reader.expectInt();
    x = (YARP_INT32) v;
    state->len--;
    return !reader.isError();
}

bool WireReader::readDouble(double& x) {
    int tag = state->code;
    if (tag<0) {
        if (noMore()) return false;
        tag = reader.expectInt();
    }
    if (tag==BOTTLE_TAG_INT) {
        if (noMore()) return false;
        int v = reader.expectInt();
        x = v;
        state->len--;
        return !reader.isError();
    }
    if (tag!=BOTTLE_TAG_DOUBLE) return false;
    if (noMore()) return false;
    x = reader.expectDouble();
    state->len--;
    return !reader.isError();
}

bool WireReader::readString(ConstString& str, bool *is_vocab) {
    if (state->len<=0) return false;
    int tag = state->code;
    if (state->code<0) {
        if (noMore()) return false;
        tag = reader.expectInt();
        if (tag!=BOTTLE_TAG_STRING&&tag!=BOTTLE_TAG_VOCAB) return false;
    }
    state->len--;
    if (tag==BOTTLE_TAG_VOCAB) {
        if (is_vocab) *is_vocab = true;
        if (noMore()) return false;
        NetInt32 v = reader.expectInt();
        if (reader.isError()) return false;
        str = Vocab::decode(v);
        return true;
    }
    if (is_vocab) *is_vocab = false;
    if (noMore()) return false;
    int len = reader.expectInt();
    if (reader.isError()) return false;
    if (len<1) return false;
    if (noMore()) return false;
    str.resize(len);
    reader.expectBlock((const char *)str.c_str(),len);
    str.resize(len-1);
    return !reader.isError();
}

bool WireReader::readBinary(ConstString& str) {
    if (state->len<=0) return false;
    int tag = state->code;
    if (state->code<0) {
        if (noMore()) return false;
        tag = reader.expectInt();
        if (tag!=BOTTLE_TAG_BLOB) return false;
    }
    state->len--;
    if (noMore()) return false;
    int len = reader.expectInt();
    if (reader.isError()) return false;
    if (len == 0) {
        str = ConstString();
        return true;
    }
    if (len<0) return false;
    if (noMore()) return false;
    str.resize(len);
    reader.expectBlock((const char *)str.c_str(),len);
    return !reader.isError();
}

bool WireReader::readEnum(YARP_INT32& x, WireVocab& converter) {
    int tag = state->code;
    if (tag<0) {
        if (noMore()) return false;
        tag = reader.expectInt();
    }
    if (tag==BOTTLE_TAG_INT) {
        if (noMore()) return false;
        int v = reader.expectInt();
        x = (YARP_INT32) v;
        state->len--;
        return !reader.isError();
    }
    if (tag==BOTTLE_TAG_STRING) {
        if (noMore()) return false;
        int len = reader.expectInt();
        if (reader.isError()) return false;
        if (len<1) return false;
        if (noMore()) return false;
        std::string str;
        str.resize(len);
        reader.expectBlock((const char *)str.c_str(),len);
        str.resize(len-1);
        state->len--;
        if (reader.isError()) return false;
        x = (YARP_INT32)converter.fromString(str);
        return (x>=0);
    }
    return false;
}

bool WireReader::readListHeader() {
    int x1 = 0, x2 = 0;
    if (noMore()) return false;
    x1 = reader.expectInt();
    if (!(x1&BOTTLE_TAG_LIST)) return false;
    if (noMore()) return false;
    x2 = reader.expectInt();
    int code = x1&(~BOTTLE_TAG_LIST);
    state->len = x2;
    if (code!=0) state->code = code;
    return !reader.isError();
}

bool WireReader::readListHeader(int len) {
    if (!readListHeader()) return false;
    return len == state->len;
}

bool WireReader::readListReturn() {
    if (!readListHeader()) return false;
    if (!support_get_mode) return true;
    if (state->len == 1) return true;
    if (state->len != 4) return false;
    // possibly old-style return: [is] foo val [ok]
    YARP_INT32 v = 0;
    if (!readVocab(v)) return false;
    if (v!=VOCAB2('i','s')) return false;
    ConstString dummy;
    if (!readString(dummy)) return false; // string OR vocab
    // now we are ready to consume real result
    state->need_ok = true;
    return true;
}

ConnectionWriter& WireReader::getWriter() {
    flush_if_needed = false;
    ConnectionWriter *writer = reader.getWriter();
    if (writer) return *writer;
    return null_writer;
}

bool WireReader::isValid() {
    return reader.isValid();
}

bool WireReader::isError() {
    return reader.isError();
}

yarp::os::ConstString WireReader::readTag() {
    flush_if_needed = true;
    ConstString str;
    bool is_vocab;
    if (!readString(str,&is_vocab)) {
        fail();
        return "";
    }
    scanString(str,is_vocab);
    if (!is_vocab) return str.c_str();
    while (is_vocab&&state->len>0) {
        if (state->code>=0) {
            is_vocab = (state->code==BOTTLE_TAG_VOCAB);
        } else {
            if (noMore()) return "";
            int x = reader.expectInt();
            reader.pushInt(x);
            is_vocab = (x==BOTTLE_TAG_VOCAB);
        }
        if (is_vocab) {
            ConstString str2;
            if (!readString(str2,&is_vocab)) return "";
            scanString(str2,is_vocab);
            str += "_";
            str += str2;
        }
    }
    return str.c_str();
}

void WireReader::readListBegin(WireState& nstate, unsigned YARP_INT32& len) {
    nstate.parent = state;
    state = &nstate;
    len = 0;
    readListHeader();
    len = (unsigned YARP_INT32)state->len;
}

void WireReader::readSetBegin(WireState& nstate, unsigned YARP_INT32& len) {
    readListBegin(nstate,len);
}

void WireReader::readMapBegin(WireState& nstate, WireState& nstate2, 
                              unsigned YARP_INT32& len) {
    readListBegin(nstate,len);
}

void WireReader::readListEnd() {
    state = state->parent;
}

void WireReader::readSetEnd() {
    state = state->parent;
}

void WireReader::readMapEnd() {
    state = state->parent;
}

bool WireReader::noMore() {
    if (!flush_if_needed) return false;
    size_t pending = reader.getSize();
    return pending==0;
}

void WireReader::scanString(ConstString& str, bool is_vocab) {
    if (!support_get_mode) return;
    if (get_string=="") {
        if (get_mode && get_string=="") {
            get_string = str;
            get_is_vocab = is_vocab;
        } else if (str=="get") {
            get_mode = true;
        } else {
            get_string = "alt";
        }
    }
}


bool WireReader::getMode() const {
    return get_mode;
}

bool WireReader::getIsVocab() const {
    return get_is_vocab;
}

const ConstString& WireReader::getString() const {
    return get_string;
}

