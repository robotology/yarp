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

namespace yarp::os::idl {

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

    bool writeBool(bool x, bool skip_tag = false) const;

    bool writeI8(std::int8_t x, bool skip_tag = false) const;

    bool writeI16(std::int16_t x, bool skip_tag = false) const;

    bool writeI32(std::int32_t x, bool skip_tag = false) const;

    bool writeI64(std::int64_t x, bool skip_tag = false) const;

    bool writeFloat32(yarp::conf::float32_t x, bool skip_tag = false) const;

    bool writeFloat64(yarp::conf::float64_t x, bool skip_tag = false) const;

    bool writeUI8(std::uint8_t x, bool skip_tag = false) const;

    bool writeUI16(std::uint16_t x, bool skip_tag = false) const;

    bool writeUI32(std::uint32_t x, bool skip_tag = false) const;

    bool writeUI64(std::uint64_t x, bool skip_tag = false) const;

    bool writeVocab32(yarp::conf::vocab32_t x, bool skip_tag = false) const;

    bool writeVocab32(char a, char b = 0, char c = 0, char d = 0, bool skip_tag = false) const
    {
        return writeVocab32(yarp::os::createVocab32(a, b, c, d));
    }

    // If the string is longer than 4 characters, only the first 4 are used.
    bool writeVocab32(const std::string& str, bool skip_tag = false) const
    {
        return writeVocab32(yarp::os::Vocab32::encode(str));
    }

    bool writeVocab64(yarp::conf::vocab64_t x, bool skip_tag = false) const;

    bool writeVocab64(char a, char b = 0, char c = 0, char d = 0, char e = 0,char f = 0,char g = 0,char h = 0, bool skip_tag = false) const
    {
        return writeVocab64(yarp::os::createVocab64(a, b, c, d, e, f, g, h));
    }

    // If the string is longer than 8 characters, only the first 8 are used.
    bool writeVocab64(const std::string& str, bool skip_tag = false) const
    {
        return writeVocab64(yarp::os::Vocab64::encode64(str));
    }

    bool writeSizeT(std::size_t x, bool skip_tag = false) const;

    bool isValid() const;

    bool isError() const;

    bool writeTag(const char* tag, int split, int len) const;

    bool writeString(const std::string& str, bool skip_tag = false) const;

    bool writeBlock(const char* data, size_t len) const;

    bool writeBinary(const std::string& blob, bool skip_tag = false) const;

    bool writeListHeader(int len) const;

    bool writeListBegin(int tag, size_t len) const;

    bool writeSetBegin(int tag, size_t len) const;

    bool writeMapBegin(int tag, int tag2, size_t len) const;

    bool writeListEnd() const;

    bool writeSetEnd() const;

    bool writeMapEnd() const;

    bool writeOnewayResponse() const;

    void flush();

private:
    bool get_mode {false};
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::string) get_string;
    bool get_is_vocab32 {false};
    mutable bool need_ok {false};
    ConnectionReader* reader {nullptr};
    ConnectionWriter& writer;
};

} // namespace yarp::os::idl

#endif // YARP_OS_IDL_WIREWRITER_H
