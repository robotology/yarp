/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IDL_WIREWRITER_H
#define YARP_OS_IDL_WIREWRITER_H

#include <yarp/conf/numeric.h>
#include <yarp/os/api.h>
#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/idl/WireReader.h>
#include <yarp/os/idl/WirePortable.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/Bottle.h>

#include <string>

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

    bool writeBool(bool x);

    bool writeI8(std::int8_t x);

    bool writeI16(std::int16_t x);

    bool writeI32(std::int32_t x);

    bool writeI64(std::int64_t x);

    bool writeFloat32(yarp::conf::float32_t x);

    bool writeFloat64(yarp::conf::float64_t x);

#ifndef YARP_NO_DEPRECATED // Since YARP 3.0.0
    YARP_DEPRECATED_MSG("Use writeI8 instead")
    bool writeByte(std::int8_t x) { return writeI8(x); }

    YARP_DEPRECATED_MSG("Use writeFloat64 instead")
    bool writeDouble(double x) { return writeFloat64(static_cast<yarp::conf::float64_t>(x)); }
#endif // YARP_NO_DEPRECATED

    bool writeVocab(std::int32_t x);

    bool isValid();

    bool isError();

    bool writeTag(const char *tag, int split, int len);

    bool writeString(const std::string& tag);

    bool writeBinary(const std::string& tag);

    bool writeListHeader(int len);

    bool writeListBegin(int tag, std::uint32_t len);

    bool writeSetBegin(int tag, std::uint32_t len);

    bool writeMapBegin(int tag, int tag2, std::uint32_t len);

    bool writeListEnd();

    bool writeSetEnd();

    bool writeMapEnd();

    bool writeOnewayResponse();

private:
    bool get_mode;
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::string) get_string;
    bool get_is_vocab;
    bool need_ok;
    ConnectionWriter& writer;
};

#endif // YARP_OS_IDL_WIREWRITER_H
