// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_WIREWRITER_
#define _YARP2_WIREWRITER_

#include <yarp/conf/numeric.h>
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
 * IDL-friendly connection writer. Used by YARP IDL tools, not intended
 * for end-user.
 *
 */
class YARP_OS_API yarp::os::idl::WireWriter {
public:

    WireWriter(ConnectionWriter& writer);

    WireWriter(WireReader& reader);

    ~WireWriter();

    bool isNull() const;

    bool write(WirePortable& obj);

    bool write(yarp::os::PortWriter& obj);

    bool writeNested(WirePortable& obj);

    bool writeNested(yarp::os::PortWriter& obj);
    
    bool writeI16(const YARP_INT16& x);

    bool writeI32(const YARP_INT32& x);

    bool writeI64(const YARP_INT64& x);

    bool writeBool(bool x);

    bool writeByte(const YARP_INT8& x);

    bool writeDouble(double x);

    bool writeVocab(int x);

    bool isValid();

    bool isError();

    bool writeTag(const char *tag, int split, int len);

    bool writeString(const yarp::os::ConstString& tag);

    bool writeBinary(const yarp::os::ConstString& tag);

    bool writeListHeader(int len);

    bool writeListBegin(int tag, unsigned YARP_INT32 len);

    bool writeSetBegin(int tag, unsigned YARP_INT32 len);

    bool writeMapBegin(int tag, int tag2, unsigned YARP_INT32 len);

    bool writeListEnd();

    bool writeSetEnd();

    bool writeMapEnd();

    bool writeOnewayResponse();

private:
    bool get_mode;
    yarp::os::ConstString get_string;
    bool get_is_vocab;
    bool need_ok;
    ConnectionWriter& writer;
};

#endif
