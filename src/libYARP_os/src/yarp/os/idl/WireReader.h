/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_IDL_WIREREADER_H
#define YARP_OS_IDL_WIREREADER_H

#include <yarp/conf/numeric.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/NullConnectionReader.h>
#include <yarp/os/NullConnectionWriter.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/idl/WirePortable.h>
#include <yarp/os/idl/WireState.h>
#include <yarp/os/idl/WireVocab.h>

#include <string>

namespace yarp {
namespace os {
namespace idl {

/**
 * IDL-friendly connection reader.  Used by YARP IDL tools, not intended
 * for end-user.
 */
class YARP_os_API WireReader
{
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

    bool readBool(bool& x);

    bool readI8(std::int8_t& x);

    bool readI16(std::int16_t& x);

    bool readI32(std::int32_t& x);

    bool readI64(std::int64_t& x);

    bool readFloat32(yarp::conf::float32_t& x);

    bool readFloat64(yarp::conf::float64_t& x);

    bool readUI8(std::uint8_t& x);

    bool readUI16(std::uint16_t& x);

    bool readUI32(std::uint32_t& x);

    bool readUI64(std::uint64_t& x);

    bool readVocab32(yarp::conf::vocab32_t& x);

    bool readSizeT(std::size_t& x);

    std::int8_t expectInt8()
    {
        std::int8_t x;
        readI8(x);
        return x;
    }
    std::int16_t expectInt16()
    {
        std::int16_t x;
        readI16(x);
        return x;
    }
    std::int32_t expectInt32()
    {
        std::int32_t x;
        readI32(x);
        return x;
    }

    std::int64_t expectInt64()
    {
        std::int64_t x;
        readI64(x);
        return x;
    }

    yarp::conf::float32_t expectFloat32()
    {
        yarp::conf::float32_t x;
        readFloat32(x);
        return x;
    }

    yarp::conf::float64_t expectFloat64()
    {
        yarp::conf::float64_t x;
        readFloat64(x);
        return x;
    }

    bool readString(std::string& str, bool* is_vocab = nullptr);

    bool readBinary(std::string& str);

    bool readEnum(std::int32_t& x, yarp::os::idl::WireVocab& converter);

    bool readListHeader();

    bool readListHeader(int len);

    bool readListReturn();

    int getLength() const
    {
        return state->len;
    }

    ConnectionWriter& getWriter();

    bool isValid();

    bool isError();

    std::string readTag();

    void readListBegin(yarp::os::idl::WireState& nstate, std::uint32_t& len);

    void readSetBegin(yarp::os::idl::WireState& nstate, std::uint32_t& len);

    void readMapBegin(yarp::os::idl::WireState& nstate, yarp::os::idl::WireState& nstate2, std::uint32_t& len);

    void readListEnd();

    void readSetEnd();

    void readMapEnd();

    bool noMore();

    bool getMode() const;

    bool getIsVocab32() const;
#ifndef YARP_NO_DEPRECATED // Since YARP 3.0.0
    YARP_DEPRECATED_MSG("Use getIsVocab instead")
    bool getIsVocab() const {
        return getIsVocab32();
    }
#endif // YARP_NO_DEPRECATED

    const std::string& getString() const;

#ifndef YARP_NO_DEPRECATED // Since YARP 3.0.0
    YARP_DEPRECATED_MSG("Use readI8 instead")
    bool readByte(std::int8_t& x)
    {
        return readI8(x);
    }

    YARP_DEPRECATED_MSG("Use readFloat64 instead")
    bool readDouble(double& x)
    {
        return readFloat64(x);
    }
#endif // YARP_NO_DEPRECATED

#ifndef YARP_NO_DEPRECATED // Since YARP 3.5.0
    YARP_DEPRECATED_MSG("Use readVocab32 instead")
    bool readVocab(std::int32_t& x) {
        return readVocab32(x);
    }
#endif // YARP_NO_DEPRECATED

#ifndef YARP_NO_DEPRECATED // Since YARP 3.0.0
    YARP_DEPRECATED_MSG("Use expectInt32 instead")
    int expectInt()
    {
        return static_cast<int>(expectInt32());
    }

    YARP_DEPRECATED_MSG("Use expectFloat64 instead")
    double expectDouble()
    {
        return static_cast<double>(expectFloat64());
    }
#endif // YARP_NO_DEPRECATED

private:
    NullConnectionWriter null_writer;
    ConnectionReader& reader;
    WireState baseState;
    WireState* state;
    bool flush_if_needed;
    bool support_get_mode;
    bool expecting;
    bool get_is_vocab;
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::string) get_string;
    bool get_mode;


    void scanString(std::string& str, bool is_vocab);
};

} // namespace idl
} // namespace os
} // namespace yarp


#endif // YARP_OS_IDL_WIREREADER_H
