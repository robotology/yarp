// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_WIREREADER_
#define _YARP2_WIREREADER_

#include <yarp/os/NullConnectionReader.h>
#include <yarp/os/NullConnectionWriter.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/idl/WireState.h>
#include <yarp/os/idl/WirePortable.h>

namespace yarp {
    namespace os {
        namespace idl {
            class WireReader;
        }
    }
}

/**
 *
 * IDL-friendly connection reader.
 *
 */
class YARP_OS_API yarp::os::idl::WireReader {
public:
    NullConnectionWriter null_writer;
    ConnectionReader& reader;
    WireState baseState;
    WireState *state;
    bool flush_if_needed;
    bool get_mode;
    std::string get_string;
    bool get_is_vocab;
    bool support_get_mode;

    WireReader(ConnectionReader& reader) : reader(reader) {
        reader.convertTextMode();
        state = &baseState;
        size_t pending = reader.getSize();
        flush_if_needed = false;
        get_mode = false;
        support_get_mode = false;
    }

    ~WireReader() {
        if (state->need_ok) {
            int32_t dummy;
            readVocab(dummy);
            state->need_ok = false;
        }
        if (flush_if_needed) {
            clear();
        }
    }

    void allowGetMode() {
        support_get_mode = true;
    }

    bool clear() {
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

    void fail() {
        clear();
        Bottle b("[fail]");
        b.write(getWriter());
    }


    bool read(WirePortable& obj) {
        return obj.read(*this);
    }

    bool readI32(int32_t& x) {
        size_t pending = reader.getSize();
        int tag = state->code;
        if (tag<0) {
            tag = reader.expectInt();
        }
        if (tag!=BOTTLE_TAG_INT) return false;
        int v = reader.expectInt();
        x = (int32_t) v;
        state->len--;
        return !reader.isError();
    }

    bool readBool(bool& x) {
        if (state->code<0) {
            int tag = reader.expectInt();
            if (tag!=BOTTLE_TAG_INT&&tag!=BOTTLE_TAG_VOCAB) return false;
        }
        int v = reader.expectInt();
        x = (v!=0) && (v!=VOCAB4('f','a','i','l'));
        state->len--;
        return !reader.isError();
    }

    bool readVocab(int32_t& x) {
        int tag = state->code;
        if (tag<0) {
            tag = reader.expectInt();
        }
        if (tag!=BOTTLE_TAG_VOCAB) return false;
        int v = reader.expectInt();
        x = (int32_t) v;
        state->len--;
        return !reader.isError();
    }

    bool readDouble(double& x) {
        int tag = state->code;
        if (tag<0) {
            tag = reader.expectInt();
        }
        if (tag==BOTTLE_TAG_INT) {
            int v = reader.expectInt();
            x = v;
            state->len--;
            return !reader.isError();
        }
        if (tag!=BOTTLE_TAG_DOUBLE) return false;
        x = reader.expectDouble();
        state->len--;
        return !reader.isError();
    }

    bool readString(std::string& str, bool *is_vocab = 0 /*NULL*/) {
        if (state->len<=0) return false;
        int tag = state->code;
        if (state->code<0) {
            tag = reader.expectInt();
            if (tag!=BOTTLE_TAG_STRING&&tag!=BOTTLE_TAG_VOCAB) return false;
        }
        state->len--;
        if (tag==BOTTLE_TAG_VOCAB) {
            if (is_vocab) *is_vocab = true;
            NetInt32 v = reader.expectInt();
            if (reader.isError()) return false;
            str = Vocab::decode(v);
            return true;
        }
        if (is_vocab) *is_vocab = false;
        int len = reader.expectInt();
        if (reader.isError()) return false;
        if (len<1) return false;
        str.resize(len);
        reader.expectBlock((const char *)str.c_str(),len);
        str.resize(len-1);
        return !reader.isError();
    }

    bool readListHeader() {
        int x1 = 0, x2 = 0;
        x1 = reader.expectInt();
        if (!(x1&BOTTLE_TAG_LIST)) return false;
        x2 = reader.expectInt();
        int code = x1&(~BOTTLE_TAG_LIST);
        state->len = x2;
        if (code!=0) state->code = code;
        return !reader.isError();
    }

    bool readListHeader(int len) {
        if (!readListHeader()) return false;
        return len == state->len;
    }

    bool readListReturn() {
        if (!readListHeader()) return false;
        if (!support_get_mode) return true;
        if (state->len == 1) return true;
        if (state->len != 4) return false;
        // possibly old-style return: [is] foo val [ok]
        int32_t v = 0;
        if (!readVocab(v)) return false;
        if (v!=VOCAB2('i','s')) return false;
        std::string dummy;
        if (!readString(dummy)) return false; // string OR vocab
        // now we are ready to consume real result
        state->need_ok = true;
        return true;
    }

    ConnectionWriter& getWriter() {
        flush_if_needed = false;
        ConnectionWriter *writer = reader.getWriter();
        if (writer) return *writer;
        return null_writer;
    }

    bool isValid() {
        return reader.isValid();
    }

    bool isError() {
        return reader.isError();
    }

    yarp::os::ConstString readTag() {
        flush_if_needed = true;
        std::string str;
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
                int x = reader.expectInt();
                reader.pushInt(x);
                is_vocab = (x==BOTTLE_TAG_VOCAB);
            }
            if (is_vocab) {
                std::string str2;
                if (!readString(str2,&is_vocab)) return "";
                scanString(str2,is_vocab);
                str += "_";
                str += str2;
            }
        }
        return str.c_str();
    }

    void readListBegin(WireState& nstate, uint32_t& len) {
        nstate.parent = state;
        state = &nstate;
        len = 0;
        readListHeader();
        len = (uint32_t)state->len;
    }

    void readListEnd() {
        state = state->parent;
    }
private:
    void scanString(std::string& str, bool is_vocab) {
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
};


#endif
