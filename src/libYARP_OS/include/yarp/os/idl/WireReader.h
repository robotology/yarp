/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IDL_WIREREADER_H
#define YARP_OS_IDL_WIREREADER_H

#include <yarp/conf/numeric.h>
#include <yarp/os/NullConnectionReader.h>
#include <yarp/os/NullConnectionWriter.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/idl/WireState.h>
#include <yarp/os/idl/WirePortable.h>
#include <yarp/os/idl/WireVocab.h>

namespace yarp {
    namespace os {
        namespace idl {
            class WireReader;
        }
    }
}

/**
 *
 * IDL-friendly connection reader.  Used by YARP IDL tools, not intended
 * for end-user.
 *
 */
class YARP_OS_API yarp::os::idl::WireReader {
public:
    WireReader(ConnectionReader& reader);

    ~WireReader();

    void expectAccept();

    void accept();

    void allowGetMode();

    bool clear();

    void fail();

    bool read(WirePortable& obj);

    bool read(yarp::os::PortReader& obj);

    bool readNested(WirePortable& obj);

    bool readNested(yarp::os::PortReader& obj);

    bool readI16(YARP_INT16& x);

    bool readI32(YARP_INT32& x);

    bool readI64(YARP_INT64& x);

    bool readBool(bool& x);

    bool readByte(YARP_INT8& x);

    bool readVocab(YARP_INT32& x);

    bool readDouble(double& x);

    int expectInt() {
        YARP_INT32 x;
        readI32(x);
        return x;
    }

    double expectDouble() {
        double x;
        readDouble(x);
        return x;
    }

    bool readString(yarp::os::ConstString& str, bool *is_vocab = nullptr);

    bool readBinary(yarp::os::ConstString& str);

#ifndef YARP_CONSTSTRING_IS_STD_STRING
    // we need to do the WIN32 dance to read an std::string without
    // running into DLL linkage trouble
    inline bool readString(std::string& str, bool *is_vocab = nullptr) {
        yarp::os::ConstString tmp;
        bool ok = readString(tmp, is_vocab);
        str = tmp;
        return ok;
    }

    inline bool readBinary(std::string& str) {
        yarp::os::ConstString tmp;
        bool ok = readBinary(tmp);
        str = tmp;
        return ok;
    }
#endif

    bool readEnum(YARP_INT32& x, WireVocab& converter);

    bool readListHeader();

    bool readListHeader(int len);

    bool readListReturn();

    int getLength() const {
        return state->len;
    }

    ConnectionWriter& getWriter();

    bool isValid();

    bool isError();

    yarp::os::ConstString readTag();

    void readListBegin(WireState& nstate, unsigned YARP_INT32& len);

    void readSetBegin(WireState& nstate, unsigned YARP_INT32& len);

    void readMapBegin(WireState& nstate, WireState& nstate2, unsigned YARP_INT32& len);

    void readListEnd();

    void readSetEnd();

    void readMapEnd();

    bool noMore();

    bool getMode() const;

    bool getIsVocab() const;

    const yarp::os::ConstString& getString() const;

private:
    NullConnectionWriter null_writer;
    ConnectionReader& reader;
    WireState baseState;
    WireState *state;
    bool flush_if_needed;
    bool support_get_mode;
    bool expecting;
    bool get_is_vocab;
    yarp::os::ConstString get_string;
    bool get_mode;


    void scanString(yarp::os::ConstString& str, bool is_vocab);
};


#endif // YARP_OS_IDL_WIREREADER_H
