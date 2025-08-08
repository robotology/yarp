/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/idl/WireWriter.h>

using namespace yarp::os;
using namespace yarp::os::idl;

namespace {
constexpr yarp::conf::vocab32_t VOCAB_OK = yarp::os::createVocab32('o', 'k');
constexpr yarp::conf::vocab32_t VOCAB_FAIL = yarp::os::createVocab32('f', 'a', 'i', 'l');
constexpr yarp::conf::vocab32_t VOCAB_IS = yarp::os::createVocab32('i', 's');
constexpr yarp::conf::vocab32_t VOCAB_DONE = yarp::os::createVocab32('d', 'o', 'n', 'e');
} // namespace


WireWriter::WireWriter(ConnectionWriter& writer) :
        writer(writer)
{
    writer.convertTextMode();
}

WireWriter::WireWriter(WireReader& reader) :
        get_mode(reader.getMode()),
        get_string(get_mode ? reader.getString() : ""),
        get_is_vocab32(get_mode ? reader.getIsVocab32() : false),
        reader(&reader.getReader()),
        writer(reader.getWriter())
{
    writer.convertTextMode();
}

WireWriter::~WireWriter()
{
    if (need_ok) {
        writeBool(true);
    }
    if(reader) {
        reader->flushWriter();
    }
}

bool WireWriter::isNull() const
{
    return writer.isNull();
}

bool WireWriter::write(const WirePortable& obj) const
{
    return obj.write(*this);
}

bool WireWriter::write(const yarp::os::PortWriter& obj) const
{
    return obj.write(writer);
}

bool WireWriter::writeNested(const WirePortable& obj) const
{
    return obj.write(writer);
}

bool WireWriter::writeNested(const yarp::os::PortWriter& obj) const
{
    return obj.write(writer);
}

bool WireWriter::writeBool(bool x, bool skip_tag) const
{
    if (!skip_tag) {
        writer.appendInt32(BOTTLE_TAG_VOCAB32);
    }
    writer.appendInt32(x ? VOCAB_OK : VOCAB_FAIL);
    return !writer.isError();
}

bool WireWriter::writeI8(std::int8_t x, bool skip_tag) const
{
    if (!skip_tag) {
        writer.appendInt32(BOTTLE_TAG_INT8);
    }
    writer.appendInt8(x);
    return !writer.isError();
}

bool WireWriter::writeI16(std::int16_t x, bool skip_tag) const
{
    if (!skip_tag) {
        writer.appendInt32(BOTTLE_TAG_INT16);
    }
    writer.appendInt16(x);
    return !writer.isError();
}

bool WireWriter::writeI32(std::int32_t x, bool skip_tag) const
{
    if (!skip_tag) {
        writer.appendInt32(BOTTLE_TAG_INT32);
    }
    writer.appendInt32(x);
    return !writer.isError();
}

bool WireWriter::writeI64(std::int64_t x, bool skip_tag) const
{
    if (!skip_tag) {
        writer.appendInt32(BOTTLE_TAG_INT64);
    }
    writer.appendInt64(x);
    return !writer.isError();
}

bool WireWriter::writeFloat32(yarp::conf::float32_t x, bool skip_tag) const
{
    if (!skip_tag) {
        writer.appendInt32(BOTTLE_TAG_FLOAT32);
    }
    writer.appendFloat32(x);
    return !writer.isError();
}

bool WireWriter::writeFloat64(yarp::conf::float64_t x, bool skip_tag) const
{
    if (!skip_tag) {
        writer.appendInt32(BOTTLE_TAG_FLOAT64);
    }
    writer.appendFloat64(x);
    return !writer.isError();
}

bool WireWriter::writeUI8(std::uint8_t x, bool skip_tag) const
{
    return writeI8(reinterpret_cast<std::int8_t&>(x), skip_tag);
}

bool WireWriter::writeUI16(std::uint16_t x, bool skip_tag) const
{
    return writeI16(reinterpret_cast<std::int16_t&>(x), skip_tag);
}

bool WireWriter::writeUI32(std::uint32_t x, bool skip_tag) const
{
    return writeI32(reinterpret_cast<std::int32_t&>(x), skip_tag);
}

bool WireWriter::writeUI64(std::uint64_t x, bool skip_tag) const
{
    return writeI64(reinterpret_cast<std::int64_t&>(x), skip_tag);
}

bool WireWriter::writeVocab32(yarp::conf::vocab32_t x, bool skip_tag) const
{
    if (!skip_tag) {
        writer.appendInt32(BOTTLE_TAG_VOCAB32);
    }
    writer.appendInt32(x);
    return !writer.isError();
}

bool WireWriter::writeVocab64(yarp::conf::vocab64_t x, bool skip_tag) const
{
    if (!skip_tag) {
        writer.appendInt32(BOTTLE_TAG_VOCAB64);
    }
    writer.appendInt64(x);
    return !writer.isError();
}

bool WireWriter::writeSizeT(std::size_t x, bool skip_tag) const
{
    int tmp = x;
    return writeI32(tmp, skip_tag);
}

bool WireWriter::isValid() const
{
    return writer.isValid();
}

bool WireWriter::isError() const
{
    return writer.isError();
}

bool WireWriter::writeTag(const char* tag, int split, int len) const
{
    YARP_UNUSED(len);
    if (split == 0) {
        return writeString(tag);
    }
    std::string bit;
    char ch = 'x';
    while (ch != '\0') {
        ch = *tag;
        tag++;
        if (ch == '\0' || ch == '_') {
            if (bit.length() <= 4) {
                writeVocab32(bit);
            } else {
                writeString(bit);
            }
            bit.clear();
        } else {
            bit += ch;
        }
    }
    return true;
}

bool WireWriter::writeString(const std::string& str, bool skip_tag) const
{
    if (!skip_tag) {
        writer.appendInt32(BOTTLE_TAG_STRING);
    }
    // WARNING str.length() value is not checked here
    writer.appendString(str);
    return !writer.isError();
}

bool WireWriter::writeBlock(const char* data, size_t len) const
{
    // FIXME Check if data is nullptr or len is 0?
    writer.appendBlock(data, len);
    return !writer.isError();
}

bool WireWriter::writeBinary(const std::string& blob, bool skip_tag) const
{
    if (!skip_tag) {
        writer.appendInt32(BOTTLE_TAG_BLOB);
    }
    // WARNING blob.length() value is not checked here
    writer.appendInt32(static_cast<int>(blob.length()));
    writer.appendBlock(blob.c_str(), blob.length());
    return !writer.isError();
}

bool WireWriter::writeListHeader(int len) const
{
    writer.appendInt32(BOTTLE_TAG_LIST);
    if (get_mode) {
        writer.appendInt32(len + 3);
        writer.appendInt32(BOTTLE_TAG_VOCAB32);
        writer.appendInt32(VOCAB_IS);
        if (get_is_vocab32) {
            writer.appendInt32(BOTTLE_TAG_VOCAB32);
            writer.appendInt32(Vocab32::encode(get_string));
        } else {
            writeString(get_string);
        }
        need_ok = true;
    } else {
        writer.appendInt32(len);
    }
    return !writer.isError();
}

bool WireWriter::writeListBegin(int tag, size_t len) const
{
    writer.appendInt32(BOTTLE_TAG_LIST | tag);
    // FIXME check len
    writer.appendInt32(static_cast<int>(len));
    return !writer.isError();
}

bool WireWriter::writeSetBegin(int tag, size_t len) const
{
    return writeListBegin(tag, len);
}

bool WireWriter::writeMapBegin(int tag, int tag2, size_t len) const
{
    YARP_UNUSED(tag);
    YARP_UNUSED(tag2);
    writer.appendInt32(BOTTLE_TAG_LIST);
    // FIXME check len
    writer.appendInt32(static_cast<int>(len));
    return !writer.isError();
}

bool WireWriter::writeListEnd() const
{
    return true;
}

bool WireWriter::writeSetEnd() const
{
    return true;
}

bool WireWriter::writeMapEnd() const
{
    return true;
}

bool WireWriter::writeOnewayResponse() const
{
    if (!writeListHeader(1)) {
        return false;
    }
    writer.appendInt32(BOTTLE_TAG_VOCAB32);
    writer.appendInt32(VOCAB_DONE);
    return true;
}

void WireWriter::flush()
{
    if (reader) {
        reader->flushWriter();
    }
}
