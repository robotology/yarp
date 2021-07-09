/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/idl/WireReader.h>

using namespace yarp::os::idl;
using namespace yarp::os;

namespace {
constexpr yarp::conf::vocab32_t VOCAB_FAIL = yarp::os::createVocab32('f', 'a', 'i', 'l');
constexpr yarp::conf::vocab32_t VOCAB_IS = yarp::os::createVocab32('i', 's');
} // namespace

WireReader::WireReader(ConnectionReader& reader) :
        reader(reader)
{
    reader.convertTextMode();
    state = &baseState;
    flush_if_needed = false;
    get_mode = false;
    support_get_mode = false;
    expecting = false;
    get_is_vocab = false;
}

WireReader::~WireReader()
{
    if (state->need_ok) {
        std::int32_t dummy;
        readVocab32(dummy);
        state->need_ok = false;
    }
    if (flush_if_needed) {
        clear();
    }
}

void WireReader::expectAccept()
{
    expecting = true;
    flush_if_needed = true;
}

void WireReader::accept()
{
    expecting = false;
}

void WireReader::allowGetMode()
{
    support_get_mode = true;
}

bool WireReader::clear()
{
    size_t pending = reader.getSize();
    if (pending > 0) {
        while (pending > 0) {
            char buf[1000];
            size_t next = (pending < sizeof(buf)) ? pending : sizeof(buf);
            reader.expectBlock(&buf[0], next);
            pending -= next;
        }
        return true;
    }
    return false;
}

void WireReader::fail()
{
    clear();
    Bottle b("[fail]");
    b.write(getWriter());
}

bool WireReader::read(WirePortable& obj)
{
    return obj.read(*this);
}

bool WireReader::read(yarp::os::PortReader& obj)
{
    return obj.read(reader);
}

bool WireReader::readNested(WirePortable& obj)
{
    return obj.read(reader);
}

bool WireReader::readNested(yarp::os::PortReader& obj)
{
    return obj.read(reader);
}

bool WireReader::readBool(bool& x)
{
    if (state->code < 0) {
        if (noMore()) {
            return false;
        }
        std::int32_t tag = reader.expectInt32();
        if (tag != BOTTLE_TAG_INT32 && tag != BOTTLE_TAG_VOCAB32) {
            return false;
        }
    }
    if (noMore()) {
        return false;
    }
    std::int32_t v = reader.expectInt32();
    x = (v != 0) && (v != VOCAB_FAIL);
    state->len--;
    return !reader.isError();
}

bool WireReader::readI8(std::int8_t& x)
{
    std::int32_t tag = state->code;
    if (tag < 0) {
        if (noMore()) {
            return false;
        }
        tag = reader.expectInt32();
    }
    if (noMore()) {
        return false;
    }
    switch (tag) {
    case BOTTLE_TAG_INT8:
        x = reader.expectInt8();
        break;
    case BOTTLE_TAG_INT32:
        x = static_cast<std::int8_t>(reader.expectInt32());
        break;
    default:
        return false;
    }

    state->len--;
    return !reader.isError();
}

bool WireReader::readI16(std::int16_t& x)
{
    std::int32_t tag = state->code;
    if (tag < 0) {
        if (noMore()) {
            return false;
        }
        tag = reader.expectInt32();
    }
    if (noMore()) {
        return false;
    }
    switch (tag) {
    case BOTTLE_TAG_INT8:
        x = static_cast<std::int16_t>(reader.expectInt8());
        break;
    case BOTTLE_TAG_INT16:
        x = reader.expectInt16();
        break;
    case BOTTLE_TAG_INT32:
        x = static_cast<std::int16_t>(reader.expectInt32());
        break;
    default:
        return false;
    }

    state->len--;
    return !reader.isError();
}

bool WireReader::readI32(std::int32_t& x)
{
    std::int32_t tag = state->code;
    if (tag < 0) {
        if (noMore()) {
            return false;
        }
        tag = reader.expectInt32();
    }
    if (noMore()) {
        return false;
    }
    switch (tag) {
    case BOTTLE_TAG_INT8:
        x = static_cast<std::int32_t>(reader.expectInt8());
        break;
    case BOTTLE_TAG_INT16:
        x = static_cast<std::int32_t>(reader.expectInt16());
        break;
    case BOTTLE_TAG_INT32:
    case BOTTLE_TAG_VOCAB32:
        x = reader.expectInt32();
        break;
    default:
        return false;
    }
    state->len--;
    return !reader.isError();
}

bool WireReader::readI64(std::int64_t& x)
{
    std::int32_t tag = state->code;
    if (tag < 0) {
        if (noMore()) {
            return false;
        }
        tag = reader.expectInt32();
    }
    if (noMore()) {
        return false;
    }
    switch (tag) {
    case BOTTLE_TAG_INT8:
        x = static_cast<std::int64_t>(reader.expectInt8());
        break;
    case BOTTLE_TAG_INT16:
        x = static_cast<std::int64_t>(reader.expectInt16());
        break;
    case BOTTLE_TAG_INT32:
        x = static_cast<std::int64_t>(reader.expectInt32());
        break;
    case BOTTLE_TAG_INT64:
        x = reader.expectInt64();
        break;
    default:
        return false;
    }

    state->len--;
    return !reader.isError();
}

bool WireReader::readFloat32(yarp::conf::float32_t& x)
{
    std::int32_t tag = state->code;
    if (tag < 0) {
        if (noMore()) {
            return false;
        }
        tag = reader.expectInt32();
    }
    if (noMore()) {
        return false;
    }
    switch (tag) {
    case BOTTLE_TAG_INT8:
        x = static_cast<yarp::conf::float32_t>(reader.expectInt8());
        break;
    case BOTTLE_TAG_INT16:
        x = static_cast<yarp::conf::float32_t>(reader.expectInt16());
        break;
    case BOTTLE_TAG_INT32:
        x = static_cast<yarp::conf::float32_t>(reader.expectInt32());
        break;
    case BOTTLE_TAG_INT64:
        x = static_cast<yarp::conf::float32_t>(reader.expectInt64());
        break;
    case BOTTLE_TAG_FLOAT32:
        x = reader.expectFloat32();
        break;
    case BOTTLE_TAG_FLOAT64:
        x = static_cast<yarp::conf::float32_t>(reader.expectFloat64());
        break;
    default:
        return false;
    }

    state->len--;
    return !reader.isError();
}

bool WireReader::readFloat64(yarp::conf::float64_t& x)
{
    std::int32_t tag = state->code;
    if (tag < 0) {
        if (noMore()) {
            return false;
        }
        tag = reader.expectInt32();
    }
    if (noMore()) {
        return false;
    }
    switch (tag) {
    case BOTTLE_TAG_INT8:
        x = static_cast<yarp::conf::float64_t>(reader.expectInt8());
        break;
    case BOTTLE_TAG_INT16:
        x = static_cast<yarp::conf::float64_t>(reader.expectInt16());
        break;
    case BOTTLE_TAG_INT32:
        x = static_cast<yarp::conf::float64_t>(reader.expectInt32());
        break;
    case BOTTLE_TAG_INT64:
        x = static_cast<yarp::conf::float64_t>(reader.expectInt64());
        break;
    case BOTTLE_TAG_FLOAT32:
        x = static_cast<yarp::conf::float64_t>(reader.expectFloat32());
        break;
    case BOTTLE_TAG_FLOAT64:
        x = reader.expectFloat64();
        break;
    default:
        return false;
    }

    state->len--;
    return !reader.isError();
}

bool WireReader::readUI8(std::uint8_t& x)
{
    return readI8(reinterpret_cast<std::int8_t&>(x));
}

bool WireReader::readUI16(std::uint16_t& x)
{
    return readI16(reinterpret_cast<std::int16_t&>(x));
}

bool WireReader::readUI32(std::uint32_t& x)
{
    return readI32(reinterpret_cast<std::int32_t&>(x));
}

bool WireReader::readUI64(std::uint64_t& x)
{
    return readI64(reinterpret_cast<std::int64_t&>(x));
}

bool WireReader::readVocab32(yarp::conf::vocab32_t& x)
{
    std::int32_t tag = state->code;
    if (tag < 0) {
        if (noMore()) {
            return false;
        }
        tag = reader.expectInt32();
    }
    if (tag != BOTTLE_TAG_VOCAB32) {
        return false;
    }
    if (noMore()) {
        return false;
    }
    x = reader.expectInt32();
    state->len--;
    return !reader.isError();
}


bool WireReader::readSizeT(std::size_t& x)
{
    std::int32_t tag = state->code;
    if (tag < 0) {
        if (noMore()) {
            return false;
        }
        tag = reader.expectInt32();
    }
    if (tag != BOTTLE_TAG_INT32) {
        return false;
    }
    if (noMore()) {
        return false;
    }
    std::int32_t tmp = reader.expectInt32();
    if (tmp < 0) {
        return false;
    }

    x = tmp;
    state->len--;
    return !reader.isError();
}

bool WireReader::readString(std::string& str, bool* is_vocab)
{
    if (state->len <= 0) {
        return false;
    }
    std::int32_t tag = state->code;
    if (state->code < 0) {
        if (noMore()) {
            return false;
        }
        tag = reader.expectInt32();
        if (tag != BOTTLE_TAG_STRING && tag != BOTTLE_TAG_VOCAB32) {
            return false;
        }
    }
    state->len--;
    if (tag == BOTTLE_TAG_VOCAB32) {
        if (is_vocab != nullptr) {
            *is_vocab = true;
        }
        if (noMore()) {
            return false;
        }
        std::int32_t v = reader.expectInt32();
        if (reader.isError()) {
            return false;
        }
        str = Vocab32::decode(v);
        return true;
    }
    if (is_vocab != nullptr) {
        *is_vocab = false;
    }
    if (noMore()) {
        return false;
    }
    std::int32_t len = reader.expectInt32();
    if (reader.isError()) {
        return false;
    }
    if (noMore()) {
        return false;
    }
    str.resize(len);
    reader.expectBlock(const_cast<char*>(str.data()), len);
    return !reader.isError();
}

bool WireReader::readBinary(std::string& str)
{
    if (state->len <= 0) {
        return false;
    }
    if (state->code < 0) {
        if (noMore()) {
            return false;
        }
        std::int32_t tag = reader.expectInt32();
        if (tag != BOTTLE_TAG_BLOB) {
            return false;
        }
    }
    state->len--;
    if (noMore()) {
        return false;
    }
    std::int32_t len = reader.expectInt32();
    if (reader.isError()) {
        return false;
    }
    if (len == 0) {
        str = std::string();
        return true;
    }
    if (len < 0) {
        return false;
    }
    if (noMore()) {
        return false;
    }
    str.resize(len);
    reader.expectBlock(const_cast<char*>(str.data()), len);
    return !reader.isError();
}

bool WireReader::readEnum(std::int32_t& x, WireVocab& converter)
{
    std::int32_t tag = state->code;
    if (tag < 0) {
        if (noMore()) {
            return false;
        }
        tag = reader.expectInt32();
    }
    if (tag == BOTTLE_TAG_INT32) {
        if (noMore()) {
            return false;
        }
        std::int32_t v = reader.expectInt32();
        x = v;
        state->len--;
        return !reader.isError();
    }
    if (tag == BOTTLE_TAG_STRING) {
        if (noMore()) {
            return false;
        }
        std::int32_t len = reader.expectInt32();
        if (reader.isError()) {
            return false;
        }
        if (len < 1) {
            return false;
        }
        if (noMore()) {
            return false;
        }
        std::string str;
        str.resize(len);
        reader.expectBlock(const_cast<char*>(str.data()), len);
        str.resize(len - 1);
        state->len--;
        if (reader.isError()) {
            return false;
        }
        x = static_cast<std::int32_t>(converter.fromString(str));
        return (x >= 0);
    }
    return false;
}

bool WireReader::readListHeader()
{
    std::int32_t x1 = 0;
    std::int32_t x2 = 0;
    if (noMore()) {
        return false;
    }
    x1 = reader.expectInt32();
    if ((x1 & BOTTLE_TAG_LIST) == 0) {
        return false;
    }
    if (noMore()) {
        return false;
    }
    x2 = reader.expectInt32();
    int code = (x1 & (~BOTTLE_TAG_LIST));
    state->len = x2;
    if (code != 0) {
        state->code = code;
    }
    return !reader.isError();
}

bool WireReader::readListHeader(int len)
{
    if (!readListHeader()) {
        return false;
    }
    return len == state->len;
}

bool WireReader::readListReturn()
{
    if (!readListHeader()) {
        return false;
    }
    if (!support_get_mode) {
        return true;
    }
    if (state->len == 1) {
        return true;
    }
    if (state->len != 4) {
        return false;
    }
    // possibly old-style return: [is] foo val [ok]
    std::int32_t v = 0;
    if (!readVocab32(v)) {
        return false;
    }
    if (v != VOCAB_IS) {
        return false;
    }
    std::string dummy;
    if (!readString(dummy)) {
        return false; // string OR vocab
    }
    // now we are ready to consume real result
    state->need_ok = true;
    return true;
}

ConnectionWriter& WireReader::getWriter()
{
    flush_if_needed = false;
    ConnectionWriter* writer = reader.getWriter();
    if (writer != nullptr) {
        return *writer;
    }
    return null_writer;
}

bool WireReader::isValid()
{
    return reader.isValid();
}

bool WireReader::isError()
{
    return reader.isError();
}

std::string WireReader::readTag()
{
    flush_if_needed = true;
    std::string str;
    bool is_vocab;
    if (!readString(str, &is_vocab)) {
        fail();
        return {};
    }
    scanString(str, is_vocab);
    if (!is_vocab) {
        return str;
    }
    while (is_vocab && state->len > 0) {
        if (state->code >= 0) {
            is_vocab = (state->code == BOTTLE_TAG_VOCAB32);
        } else {
            if (noMore()) {
                return {};
            }
            std::int32_t x = reader.expectInt32();
            reader.pushInt(x);
            is_vocab = (x == BOTTLE_TAG_VOCAB32);
        }
        if (is_vocab) {
            std::string str2;
            if (!readString(str2, &is_vocab)) {
                return {};
            }
            scanString(str2, is_vocab);
            str += "_";
            str += str2;
        }
    }
    return str;
}

void WireReader::readListBegin(WireState& nstate, std::uint32_t& len)
{
    nstate.parent = state;
    state = &nstate;
    len = 0;
    if (!readListHeader()) {
        return;
    }
    len = static_cast<std::uint32_t>(state->len);
}

void WireReader::readSetBegin(WireState& nstate, std::uint32_t& len)
{
    readListBegin(nstate, len);
}

void WireReader::readMapBegin(WireState& nstate,
                              WireState& nstate2,
                              std::uint32_t& len)
{
    YARP_UNUSED(nstate2);
    readListBegin(nstate, len);
}

void WireReader::readListEnd()
{
    state = state->parent;
}

void WireReader::readSetEnd()
{
    state = state->parent;
}

void WireReader::readMapEnd()
{
    state = state->parent;
}

bool WireReader::noMore()
{
    if (!flush_if_needed) {
        return false;
    }
    size_t pending = reader.getSize();
    return pending == 0;
}

void WireReader::scanString(std::string& str, bool is_vocab)
{
    if (!support_get_mode) {
        return;
    }
    if (get_string.empty()) {
        if (get_mode && get_string.empty()) {
            get_string = str;
            get_is_vocab = is_vocab;
        } else if (str == "get") {
            get_mode = true;
        } else {
            get_string = "alt";
        }
    }
}


bool WireReader::getMode() const
{
    return get_mode;
}

bool WireReader::getIsVocab32() const
{
    return get_is_vocab;
}

const std::string& WireReader::getString() const
{
    return get_string;
}
