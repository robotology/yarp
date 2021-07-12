/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_IDL_WIREWRITER_H
#define YARP_OS_IDL_WIREWRITER_H

#include <yarp/conf/numeric.h>

#include <yarp/os/api.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/idl/WirePortable.h>
#include <yarp/os/idl/WireReader.h>

#include <string>

namespace yarp {
namespace os {
namespace idl {

/**
 * IDL-friendly connection writer. Used by YARP IDL tools, not intended
 * for end-user.
 */
class YARP_os_API WireWriter
{
public:
    WireWriter(ConnectionWriter& writer);

    WireWriter(WireReader& reader);

    ~WireWriter();

    bool isNull() const;

    bool write(const WirePortable& obj) const;

    bool write(const yarp::os::PortWriter& obj) const;

    bool writeNested(const WirePortable& obj) const;

    bool writeNested(const yarp::os::PortWriter& obj) const;

    bool writeBool(bool x) const;

    bool writeI8(std::int8_t x) const;

    bool writeI16(std::int16_t x) const;

    bool writeI32(std::int32_t x) const;

    bool writeI64(std::int64_t x) const;

    bool writeFloat32(yarp::conf::float32_t x) const;

    bool writeFloat64(yarp::conf::float64_t x) const;

    bool writeUI8(std::uint8_t x) const;

    bool writeUI16(std::uint16_t x) const;

    bool writeUI32(std::uint32_t x) const;

    bool writeUI64(std::uint64_t x) const;

    bool writeVocab32(yarp::conf::vocab32_t x) const;

    bool writeVocab32(char a, char b = 0, char c = 0, char d = 0) const
    {
        return writeVocab32(yarp::os::createVocab32(a, b, c, d));
    }

    // If the string is longer than 4 characters, only the first 4 are used.
    bool writeVocab32(const std::string& str) const
    {
        return writeVocab32(yarp::os::Vocab32::encode(str));
    }

    bool writeSizeT(std::size_t x) const;

    bool isValid() const;

    bool isError() const;

    bool writeTag(const char* tag, int split, int len) const;

    bool writeString(const std::string& tag) const;

    bool writeBinary(const std::string& tag) const;

    bool writeListHeader(int len) const;

    bool writeListBegin(int tag, std::uint32_t len) const;

    bool writeSetBegin(int tag, std::uint32_t len) const;

    bool writeMapBegin(int tag, int tag2, std::uint32_t len) const;

    bool writeListEnd() const;

    bool writeSetEnd() const;

    bool writeMapEnd() const;

    bool writeOnewayResponse() const;

#ifndef YARP_NO_DEPRECATED // Since YARP 3.0.0
    YARP_DEPRECATED_MSG("Use writeI8 instead")
    bool writeByte(std::int8_t x) const
    {
        return writeI8(x);
    }

    YARP_DEPRECATED_MSG("Use writeFloat64 instead")
    bool writeDouble(double x) const
    {
        return writeFloat64(static_cast<yarp::conf::float64_t>(x));
    }
#endif // YARP_NO_DEPRECATED

#ifndef YARP_NO_DEPRECATED // Since YARP 3.5.0
    YARP_DEPRECATED_MSG("Use writeVocab32 instead")
    bool writeVocab(std::int32_t x) const
    {
        return writeVocab32(x);
    }
#endif // YARP_NO_DEPRECATED

private:
    bool get_mode;
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::string) get_string;
    bool get_is_vocab32;
    mutable bool need_ok;
    ConnectionWriter& writer;
};

} // namespace idl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IDL_WIREWRITER_H
