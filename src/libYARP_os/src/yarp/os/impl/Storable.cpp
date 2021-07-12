/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-FileCopyrightText: 2006, 2008 Arjan Gijsberts
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/impl/Storable.h>

#include <yarp/conf/numeric.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/NetType.h>
#include <yarp/os/Value.h>
#include <yarp/os/impl/BottleImpl.h>
#include <yarp/os/impl/LogComponent.h>

#include <cstdio>
#include <cstdlib>

using yarp::os::Bottle;
using yarp::os::ConnectionReader;
using yarp::os::ConnectionWriter;
using yarp::os::Value;
using yarp::os::impl::BottleImpl;
using yarp::os::impl::Storable;
using yarp::os::impl::StoreBlob;
using yarp::os::impl::StoreDict;
using yarp::os::impl::StoreFloat32;
using yarp::os::impl::StoreFloat64;
using yarp::os::impl::StoreInt16;
using yarp::os::impl::StoreInt32;
using yarp::os::impl::StoreInt64;
using yarp::os::impl::StoreInt8;
using yarp::os::impl::StoreList;
using yarp::os::impl::StoreString;
using yarp::os::impl::StoreVocab32;


YARP_OS_LOG_COMPONENT(STORABLE, "yarp.os.impl.Storable")


const int StoreInt8::code = BOTTLE_TAG_INT8;
const int StoreInt16::code = BOTTLE_TAG_INT16;
const int StoreInt32::code = BOTTLE_TAG_INT32;
const int StoreInt64::code = BOTTLE_TAG_INT64;
const int StoreFloat32::code = BOTTLE_TAG_FLOAT32;
const int StoreFloat64::code = BOTTLE_TAG_FLOAT64;
const int StoreVocab32::code = BOTTLE_TAG_VOCAB32;
const int StoreString::code = BOTTLE_TAG_STRING;
const int StoreBlob::code = BOTTLE_TAG_BLOB;
const int StoreList::code = BOTTLE_TAG_LIST;
const int StoreDict::code = BOTTLE_TAG_LIST | BOTTLE_TAG_DICT;



////////////////////////////////////////////////////////////////////////////
// Storable

Storable::~Storable() = default;

Storable* Storable::createByCode(std::int32_t id)
{
    Storable* storable = nullptr;
    std::int32_t subCode = 0;
    switch (id) {
    case StoreInt8::code:
        storable = new StoreInt8();
        break;
    case StoreInt16::code:
        storable = new StoreInt16();
        break;
    case StoreInt32::code:
        storable = new StoreInt32();
        break;
    case StoreInt64::code:
        storable = new StoreInt64();
        break;
    case StoreVocab32::code:
        storable = new StoreVocab32();
        break;
    case StoreFloat32::code:
        storable = new StoreFloat32();
        break;
    case StoreFloat64::code:
        storable = new StoreFloat64();
        break;
    case StoreString::code:
        storable = new StoreString();
        break;
    case StoreBlob::code:
        storable = new StoreBlob();
        break;
    case StoreList::code:
        storable = new StoreList();
        yCAssert(STORABLE, storable != nullptr);
        storable->asList()->implementation->setNested(true);
        break;
    default:
        if ((id & GROUP_MASK) != 0) {
            // typed list
            subCode = (id & UNIT_MASK);
            if ((id & BOTTLE_TAG_DICT) != 0) {
                storable = new StoreDict();
                yCAssert(STORABLE, storable != nullptr);
            } else {
                storable = new StoreList();
                yCAssert(STORABLE, storable != nullptr);
                storable->asList()->implementation->specialize(subCode);
                storable->asList()->implementation->setNested(true);
            }
        }
        break;
    }
    return storable;
}

Value& Storable::find(const std::string& key) const
{
    YARP_UNUSED(key);
    return BottleImpl::getNull();
}

Bottle& Storable::findGroup(const std::string& key) const
{
    YARP_UNUSED(key);
    return Bottle::getNullBottle();
}

bool Storable::check(const std::string& key) const
{
    Bottle& val = findGroup(key);
    if (!val.isNull()) {
        return true;
    }
    Value& val2 = find(key);
    return !val2.isNull();
}

bool Storable::operator==(const Value& alt) const
{
    return toString() == alt.toString();
}


bool Storable::read(ConnectionReader& connection)
{
    std::int32_t x = connection.expectInt32();
    if (x != getCode()) {
        return false;
    }
    return readRaw(connection);
}

bool Storable::write(ConnectionWriter& connection) const
{
    connection.appendInt32(getCode());
    return writeRaw(connection);
}


////////////////////////////////////////////////////////////////////////////
// StoreInt8

std::string StoreInt8::toString() const
{
    return std::to_string(x);
}

void StoreInt8::fromString(const std::string& src)
{
    x = static_cast<std::int8_t>(strtol(src.c_str(), static_cast<char**>(nullptr), 0));
}

bool StoreInt8::readRaw(ConnectionReader& reader)
{
    x = reader.expectInt8();
    return true;
}

bool StoreInt8::writeRaw(ConnectionWriter& writer) const
{
    writer.appendInt8(x);
    return true;
}


////////////////////////////////////////////////////////////////////////////
// StoreInt16

std::string StoreInt16::toString() const
{
    return std::to_string(x);
}

void StoreInt16::fromString(const std::string& src)
{
    x = static_cast<std::int16_t>(strtol(src.c_str(), static_cast<char**>(nullptr), 0));
}

bool StoreInt16::readRaw(ConnectionReader& reader)
{
    x = reader.expectInt16();
    return true;
}

bool StoreInt16::writeRaw(ConnectionWriter& writer) const
{
    writer.appendInt16(x);
    return true;
}


////////////////////////////////////////////////////////////////////////////
// StoreInt32

std::string StoreInt32::toString() const
{
    return std::to_string(x);
}

void StoreInt32::fromString(const std::string& src)
{
    x = strtol(src.c_str(), static_cast<char**>(nullptr), 0);
}

bool StoreInt32::readRaw(ConnectionReader& reader)
{
    x = reader.expectInt32();
    return true;
}

bool StoreInt32::writeRaw(ConnectionWriter& writer) const
{
    writer.appendInt32(x);
    return true;
}


////////////////////////////////////////////////////////////////////////////
// StoreInt64

std::string StoreInt64::toString() const
{
    return std::to_string(x);
}

void StoreInt64::fromString(const std::string& src)
{
    x = strtoll(src.c_str(), static_cast<char**>(nullptr), 0);
}

bool StoreInt64::readRaw(ConnectionReader& reader)
{
    x = reader.expectInt64();
    return true;
}

bool StoreInt64::writeRaw(ConnectionWriter& writer) const
{
    writer.appendInt64(x);
    return true;
}


////////////////////////////////////////////////////////////////////////////
// StoreVocab32

std::string StoreVocab32::toString() const
{
    if (x == 0) {
        return "false";
    }
    if (x == '1') {
        return "true";
    }
    return Vocab32::decode(x);
}

void StoreVocab32::fromString(const std::string& src)
{
    x = Vocab32::encode(src);
}

std::string StoreVocab32::toStringNested() const
{
    if (x == 0) {
        return "false";
    }
    if (x == '1') {
        return "true";
    }
    return std::string("[") + toString() + "]";
}

void StoreVocab32::fromStringNested(const std::string& src)
{
    x = 0;
    if (src.length() > 0) {
        if (src[0] == '[') {
            // ignore first [ and last ]
            fromString(src.substr(1, src.length() - 2));
        } else if (src == "true") {
            x = static_cast<int>('1');
        } else if (src == "false") {
            x = 0;
        }
    }
}

bool StoreVocab32::readRaw(ConnectionReader& reader)
{
    x = reader.expectInt32();
    return true;
}

bool StoreVocab32::writeRaw(ConnectionWriter& writer) const
{
    writer.appendInt32(x);
    return true;
}


////////////////////////////////////////////////////////////////////////////
// StoreFloat32


std::string StoreFloat32::toString() const
{
    return yarp::conf::numeric::to_string(x);
}

void StoreFloat32::fromString(const std::string& src)
{
    x = yarp::conf::numeric::from_string<yarp::conf::float32_t>(src);
}

bool StoreFloat32::readRaw(ConnectionReader& reader)
{
    x = reader.expectFloat32();
    return true;
}

bool StoreFloat32::writeRaw(ConnectionWriter& writer) const
{
    writer.appendFloat32(x);
    return true;
}


////////////////////////////////////////////////////////////////////////////
// StoreFloat64

std::string StoreFloat64::toString() const
{
    return yarp::conf::numeric::to_string(x);
}

void StoreFloat64::fromString(const std::string& src)
{
    x = yarp::conf::numeric::from_string<yarp::conf::float64_t>(src);
}

bool StoreFloat64::readRaw(ConnectionReader& reader)
{
    x = reader.expectFloat64();
    return true;
}

bool StoreFloat64::writeRaw(ConnectionWriter& writer) const
{
    writer.appendFloat64(x);
    return true;
}


////////////////////////////////////////////////////////////////////////////
// StoreString

std::string StoreString::toString() const
{
    return x;
}

std::string StoreString::quotedString(const std::string& x)
{
    // quoting code: very inefficient, but portable
    std::string result;

    bool needQuote = false;
    for (unsigned int i = 0; i < x.length(); i++) {
        char ch = x[i];
        if ((ch < 'a' || ch > 'z') && (ch < 'A' || ch > 'Z') && ch != '_') {
            if ((ch >= '0' && ch <= '9') || ch == '.' || ch == '-') {
                if (i == 0) {
                    needQuote = true;
                    break;
                }
            } else {
                needQuote = true;
                break;
            }
        }
    }
    if (x.length() == 0) {
        needQuote = true;
    }
    if (x == "true" || x == "false") {
        needQuote = true;
    }

    if (!needQuote) {
        return x;
    }

    result += "\"";
    for (char ch : x) {
        if (ch == '\n') {
            result += '\\';
            result += 'n';
        } else if (ch == '\r') {
            result += '\\';
            result += 'r';
        } else if (ch == '\0') {
            result += '\\';
            result += '0';
        } else {
            if (ch == '\\' || ch == '\"') {
                result += '\\';
            }
            result += ch;
        }
    }
    result += "\"";

    return result;
}

std::string StoreString::toStringNested() const
{
    return quotedString(x);
}

void StoreString::fromString(const std::string& src)
{
    x = src;
}

void StoreString::fromStringNested(const std::string& src)
{
    // unquoting code: very inefficient, but portable
    x = "";
    size_t len = src.length();
    if (len > 0) {
        bool skip = false;
        bool back = false;
        if (src[0] == '\"') {
            skip = true;
        }
        for (size_t i = 0; i < len; i++) {
            if (skip && (i == 0 || i == len - 1)) {
                // omit
            } else {
                char ch = src[i];
                if (ch == '\\') {
                    if (!back) {
                        back = true;
                    } else {
                        x += '\\';
                        back = false;
                    }
                } else {
                    if (back) {
                        if (ch == 'n') {
                            x += '\n';
                        } else if (ch == 'r') {
                            x += '\r';
                        } else if (ch == '0') {
                            x += '\0';
                        } else {
                            x += ch;
                        }
                    } else {
                        x += ch;
                    }
                    back = false;
                }
            }
        }
    }
}


bool StoreString::readRaw(ConnectionReader& reader)
{
    std::int32_t len = reader.expectInt32();
    x.resize(len);
    reader.expectBlock(const_cast<char*>(x.data()), len);
    return true;
}

bool StoreString::writeRaw(ConnectionWriter& writer) const
{
    writer.appendInt32(static_cast<std::int32_t>(x.length()));
    writer.appendBlock(x.c_str(), x.length());
    return true;
}


////////////////////////////////////////////////////////////////////////////
// StoreBlob

std::string StoreBlob::toString() const
{
    std::string result;
    for (unsigned int i = 0; i < x.length(); i++) {
        if (i > 0) {
            result += " ";
        }
        const auto* src = reinterpret_cast<const unsigned char*>(&x[i]);
        result += yarp::conf::numeric::to_string(*src);
    }
    return result;
}

std::string StoreBlob::toStringNested() const
{
    return std::string("{") + toString() + "}";
}

void StoreBlob::fromString(const std::string& src)
{
    Bottle bot(src);
    std::string buf(bot.size(), 0);
    for (size_t i = 0; i < bot.size(); i++) {
        buf[i] = static_cast<char>(static_cast<unsigned char>(bot.get(i).asInt32()));
    }
    x = buf;
}

void StoreBlob::fromStringNested(const std::string& src)
{
    if (src.length() > 0) {
        if (src[0] == '{') {
            // ignore first { and last }
            std::string buf = src.substr(1, src.length() - 2);
            fromString(buf);
        }
    }
}

bool StoreBlob::readRaw(ConnectionReader& reader)
{
    std::int32_t len = reader.expectInt32();
    x.resize(len);
    reader.expectBlock(const_cast<char*>(x.data()), len);
    return true;
}

bool StoreBlob::writeRaw(ConnectionWriter& writer) const
{
    writer.appendInt32(static_cast<std::int32_t>(x.length()));
    writer.appendBlock(x.c_str(), x.length());
    return true;
}


////////////////////////////////////////////////////////////////////////////
// StoreList

std::string StoreList::toString() const
{
    return content.toString();
}

std::string StoreList::toStringNested() const
{
    return std::string("(") + content.toString() + ")";
}

void StoreList::fromString(const std::string& src)
{
    content.fromString(src);
}

void StoreList::fromStringNested(const std::string& src)
{
    if (src.length() > 0) {
        if (src[0] == '(') {
            // ignore first ( and last )
            std::string buf = src.substr(1, src.length() - 2);
            content.fromString(buf);
        }
    }
}

bool StoreList::readRaw(ConnectionReader& reader)
{
    // not using the most efficient representation
    content.read(reader);
    return true;
}

bool StoreList::writeRaw(ConnectionWriter& writer) const
{
    // not using the most efficient representation
    content.write(writer);
    return true;
}

std::int32_t StoreList::subCode() const
{
    return subCoder(*(content.implementation));
}


////////////////////////////////////////////////////////////////////////////
// StoreDict

std::string StoreDict::toString() const
{
    return std::string(content.toString());
}

std::string StoreDict::toStringNested() const
{
    return std::string("(") + content.toString() + ")";
}

void StoreDict::fromString(const std::string& src)
{
    content.fromString(src);
}

void StoreDict::fromStringNested(const std::string& src)
{
    if (src.length() > 0) {
        if (src[0] == '(') {
            // ignore first ( and last )
            std::string buf = src.substr(1, src.length() - 2);
            content.fromString(buf);
        }
    }
}

bool StoreDict::readRaw(ConnectionReader& reader)
{
    // not using the most efficient representation
    content.read(reader);
    return true;
}

bool StoreDict::writeRaw(ConnectionWriter& writer) const
{
    // not using the most efficient representation
    content.write(writer);
    return true;
}
