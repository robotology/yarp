/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * Copyright (C) 2006, 2008 Arjan Gijsberts
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/impl/BottleImpl.h>
#include <yarp/conf/numeric.h>

#include <string>
#include <yarp/os/NetFloat64.h>
#include <yarp/os/StringInputStream.h>
#include <yarp/os/StringOutputStream.h>
#include <yarp/os/Vocab.h>

#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/os/impl/MemoryOutputStream.h>
#include <yarp/os/impl/StreamConnectionReader.h>

#include <clocale>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cfloat>
#include <cinttypes>

using yarp::os::impl::StoreInt8;
using yarp::os::impl::StoreInt16;
using yarp::os::impl::StoreInt32;
using yarp::os::impl::StoreInt64;
using yarp::os::impl::StoreFloat32;
using yarp::os::impl::StoreFloat64;
using yarp::os::impl::StoreVocab;
using yarp::os::impl::StoreString;
using yarp::os::impl::StoreList;
using yarp::os::impl::StoreBlob;
using yarp::os::impl::StoreDict;
using yarp::os::impl::BottleImpl;
using yarp::os::impl::Storable;
using yarp::os::Bytes;
using yarp::os::ConnectionReader;
using yarp::os::ConnectionWriter;
using yarp::os::Bottle;
using yarp::os::Searchable;
using yarp::os::Value;

#define YARP_STRINIT(len) ((size_t)(len)), 0

/*
 * The maximum string length for a 'double' printed as a string using ("%.*g", DBL_DIG) will be:
 *  Initial +/- sign                        1 char
 *  First digit for exponential notation    1 char
 * '.' decimal separator char               1 char
 *  DBL_DIG digits for the mantissa         DBL_DIG chars
 * 'e+/-'                                   2 chars
 * YARP_DBL_EXP_DIG  for the exponential    YARP_DBL_EXP_DIG chars
 * string terminator                        1 char
 * FILLER                                   10 chars  (you know, for safety)
 * -----------------------------------------------------
 * TOTAL is                                 16 + DBL_DIG + YARP_DBL_EXP_DIG
 */
#define YARP_DOUBLE_TO_STRING_MAX_LENGTH    16 + DBL_DIG + YARP_DBL_EXP_DIG

namespace {
/*
 * Converts a floating point number to a string, dealing with locale issues
 */
template <typename T>
inline std::string fp_to_string(T x)
{
    char buf[YARP_DOUBLE_TO_STRING_MAX_LENGTH];    // -> see comment at the top of the file
    std::snprintf(buf, YARP_DOUBLE_TO_STRING_MAX_LENGTH, "%.*g", DBL_DIG, x);
    std::string str(buf);

    // If locale is set, the locale version of the decimal point is used.
    // In this case we change it to the standard "."
    // If there is no decimal point, and it is not being used the exponential
    // notation (i.e. the number is in integer form, for example 100000 and not
    // 1e5) we add ".0" to ensure that it will be interpreted as a double.
    struct lconv* lc = localeconv();
    size_t offset = str.find(lc->decimal_point);
    if (offset != std::string::npos) {
        str[offset] = '.';
    } else {
        if (str.find('e') == std::string::npos) {
            str += ".0";
        }
    }
    return str;
}

/*
 * Converts a string to a floating point number, dealing with locale issues
 */
template <typename T>
inline T fp_from_string(std::string src)
{
    // YARP Bug 2526259: Locale settings influence YARP behavior
    // Need to deal with alternate versions of the decimal point.
    size_t offset = src.find('.');
    if (offset != std::string::npos) {
        struct lconv* lc = localeconv();
        src[offset] = lc->decimal_point[0];
    }
    return static_cast<T>(strtod(src.c_str(), nullptr));
}
}

//#define YMSG(x) printf x;
//#define YTRACE(x) YMSG(("at %s\n", x))

#define YMSG(x)
#define YTRACE(x)

const int StoreInt8::code = BOTTLE_TAG_INT8;
const int StoreInt16::code = BOTTLE_TAG_INT16;
const int StoreInt32::code = BOTTLE_TAG_INT32;
const int StoreInt64::code = BOTTLE_TAG_INT64;
const int StoreFloat32::code = BOTTLE_TAG_FLOAT32;
const int StoreFloat64::code = BOTTLE_TAG_FLOAT64;
const int StoreVocab::code = BOTTLE_TAG_VOCAB;
const int StoreString::code = BOTTLE_TAG_STRING;
const int StoreBlob::code = BOTTLE_TAG_BLOB;
const int StoreList::code = BOTTLE_TAG_LIST;
const int StoreDict::code = BOTTLE_TAG_LIST | BOTTLE_TAG_DICT;

#define UNIT_MASK        \
    (BOTTLE_TAG_INT8  | \
     BOTTLE_TAG_INT16  | \
     BOTTLE_TAG_INT32  | \
     BOTTLE_TAG_INT64  | \
     BOTTLE_TAG_FLOAT32 | \
     BOTTLE_TAG_FLOAT64 | \
     BOTTLE_TAG_VOCAB  | \
     BOTTLE_TAG_STRING | \
     BOTTLE_TAG_BLOB)

#define GROUP_MASK \
    (BOTTLE_TAG_LIST | \
     BOTTLE_TAG_DICT)


yarp::os::impl::StoreNull* BottleImpl::storeNull = nullptr;

BottleImpl::BottleImpl() :
        parent(nullptr),
        invalid(false),
        ro(false),
        speciality(0),
        nested(false),
        dirty(true)
{
}

BottleImpl::BottleImpl(Searchable* parent) :
        parent(parent),
        invalid(false),
        ro(false),
        speciality(0),
        nested(false),
        dirty(true)
{
}


BottleImpl::~BottleImpl()
{
    clear();
}


void BottleImpl::add(Storable* s)
{
    content.push_back(s);
    dirty = true;
}


void BottleImpl::clear()
{
    for (unsigned int i = 0; i < content.size(); i++) {
        delete content[i];
    }
    content.clear();
    dirty = true;
}

void BottleImpl::smartAdd(const std::string& str)
{
    if (str.length() > 0) {
        char ch = str[0];
        Storable* s = nullptr;
        StoreString* ss = nullptr;
        bool numberLike = true;
        bool preamble = true;
        bool hexActive = false;
        int hexStart = 0;
        int periodCount = 0;
        int signCount = 0;
        bool hasPeriodOrE = false;
        for (unsigned int i = 0; i < str.length(); i++) {
            char ch2 = str[i];
            if (ch2 == '.') {
                hasPeriodOrE = true;
                periodCount++;
                if (periodCount > 1) {
                    numberLike = false;
                }
            }
            if (!hexActive && (ch2 == 'e' || ch2 == 'E')) {
                hasPeriodOrE = true;
            }
            if (preamble) {
                if (ch2 == 'x' || ch2 == 'X') {
                    hexActive = true;
                    hexStart = i;
                    continue;
                }
            }
            if (preamble) {
                if (ch2 == '0' || ch2 == '+' || ch2 == '-') {
                    if (ch2 == '+' || ch2 == '-') {
                        signCount++;
                        if (signCount > 1) {
                            numberLike = false;
                        }
                    }
                    continue;
                }
            }
            preamble = false;
            if (!((ch2 >= '0' && ch2 <= '9') || ch2 == '.' || ch2 == 'e' ||
                  ch2 == 'E' || ch2 == '+' || ch2 == '-' ||
                  (hexActive && ((ch2 >= 'a' && ch2 <= 'f') ||
                                 (ch2 >= 'A' && ch2 <= 'F'))))) {
                numberLike = false;
                break;
            }
        }
        if (hexActive) {
            if (static_cast<int>(str.length()) - (hexStart + 1) > 8) {
                // we can only deal with 32bit hexadecimal
                numberLike = false;
            }
        }

        if (numberLike &&
            ((ch >= '0' && ch <= '9') || ch == '+' || ch == '-' || ch == '.') &&
            (ch != '.' || str.length() > 1)) {
            if (!hasPeriodOrE) {
                s = new StoreInt32(0);
            } else {
                s = new StoreFloat64(0);
            }
        } else if (ch == '(') {
            s = new StoreList();
        } else if (ch == '[') {
            s = new StoreVocab();
        } else if (ch == '{') {
            s = new StoreBlob();
        } else {
            s = ss = new StoreString("");
        }
        if (s != nullptr) {
            s->fromStringNested(str);
            if (ss != nullptr) {
                if (str.length() == 0 || str[0] != '\"') {
                    std::string val = ss->asString();
                    if (val == "true") {
                        delete s;
                        s = new StoreVocab(static_cast<int>('1'));
                    } else if (val == "false") {
                        delete s;
                        s = new StoreVocab(0);
                    }
                }
            }
            add(s);
        }
        ss = nullptr;
    }
}

void BottleImpl::fromString(const std::string& line)
{
    clear();
    dirty = true;
    std::string arg = "";
    bool quoted = false;
    bool back = false;
    bool begun = false;
    int nested = 0;
    int nestedAlt = 0;
    std::string nline = line + " ";

    for (unsigned int i = 0; i < nline.length(); i++) {
        char ch = nline[i];
        if (back) {
            arg += ch;
            back = false;
        } else {
            if (!begun) {
                if (ch != ' ' && ch != '\t' && ch != '\n' && ch != '\r') {
                    begun = true;
                }
            }
            if (begun) {
                if (ch == '\"') {
                    quoted = !quoted;
                }
                if (!quoted) {
                    if (ch == '(') {
                        nested++;
                    }
                    if (ch == ')') {
                        nested--;
                    }
                    if (ch == '{') {
                        nestedAlt++;
                    }
                    if (ch == '}') {
                        nestedAlt--;
                    }
                }
                if (ch == '\\') {
                    back = true;
                    arg += ch;
                } else {
                    if ((!quoted) && (ch == ',' || ch == ' ' || ch == '\t' ||
                                      ch == '\n' || ch == '\r') &&
                        (nestedAlt == 0) && (nested == 0)) {
                        if (arg != "") {
                            if (arg == "null") {
                                add(new StoreVocab(VOCAB4('n', 'u', 'l', 'l')));
                            } else {
                                smartAdd(arg);
                            }
                        }
                        arg = "";
                        begun = false;
                    } else {
                        arg += ch;
                    }
                }
            }
        }
    }
}

bool BottleImpl::isComplete(const char* txt)
{
    bool quoted = false;
    bool back = false;
    bool begun = false;
    int nested = 0;
    int nestedAlt = 0;
    std::string nline = txt;
    nline += " ";

    for (unsigned int i = 0; i < nline.length(); i++) {
        char ch = nline[i];
        if (back) {
            back = false;
        } else {
            if (!begun) {
                if (ch != ' ' && ch != '\t' && ch != '\n' && ch != '\r') {
                    begun = true;
                }
            }
            if (begun) {
                if (ch == '\"') {
                    quoted = !quoted;
                }
                if (!quoted) {
                    if (ch == '(') {
                        nested++;
                    }
                    if (ch == ')') {
                        nested--;
                    }
                    if (ch == '{') {
                        nestedAlt++;
                    }
                    if (ch == '}') {
                        nestedAlt--;
                    }
                }
                if (ch == '\\') {
                    back = true;
                    // arg += ch;
                } else {
                    if ((!quoted) &&
                        (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r') &&
                        (nestedAlt == 0) && (nested == 0)) {
                        // smartAdd(arg);
                        begun = false;
                    } else {
                        // arg += ch;
                    }
                }
            }
        }
    }
    return nested == 0 && nestedAlt == 0 && quoted == false;
}


std::string BottleImpl::toString()
{
    std::string result = "";
    for (unsigned int i = 0; i < content.size(); i++) {
        if (i > 0) {
            result += " ";
        }
        Storable& s = *content[i];
        result += s.toStringNested();
    }
    return result;
}

size_t BottleImpl::size() const
{
    return content.size();
}

Storable::~Storable()
{
}

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
    case StoreVocab::code:
        storable = new StoreVocab();
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
        yAssert(storable != nullptr);
        storable->asList()->implementation->setNested(true);
        break;
    default:
        if ((id & GROUP_MASK) != 0) {
            // typed list
            subCode = (id & UNIT_MASK);
            if (id & BOTTLE_TAG_DICT) {
                storable = new StoreDict();
                yAssert(storable != nullptr);
            } else {
                storable = new StoreList();
                yAssert(storable != nullptr);
                storable->asList()->implementation->specialize(subCode);
                storable->asList()->implementation->setNested(true);
            }
        }
        break;
    }
    return storable;
}


bool BottleImpl::fromBytes(ConnectionReader& reader)
{
    if (reader.isError()) {
        return false;
    }
    std::int32_t id = speciality;
    YMSG(("READING, nest flag is %d\n", nested));
    if (id == 0) {
        id = reader.expectInt32();
        YMSG(("READ subcode %" PRId32 "\n", id));
    } else {
        YMSG(("READ skipped subcode %" PRId32 "\n", speciality));
    }
    Storable* storable = Storable::createByCode(id);
    if (storable == nullptr) {
        YARP_SPRINTF1(Logger::get(), error,
                      "BottleImpl reader failed, unrecognized object code %" PRId32,
                      id);
        return false;
    }
    storable->readRaw(reader);
    add(storable);
    return true;
}


void BottleImpl::fromBinary(const char* text, int len)
{
    std::string wrapper(text, len);
    StringInputStream sis;
    sis.add(wrapper);
    StreamConnectionReader reader;
    Route route;
    reader.reset(sis, nullptr, route, len, false);
    read(reader);
}


bool BottleImpl::fromBytes(const Bytes& data)
{
    std::string wrapper(data.get(), data.length());
    StringInputStream sis;
    sis.add(wrapper);
    StreamConnectionReader reader;
    Route route;
    reader.reset(sis, nullptr, route, data.length(), false);

    clear();
    dirty = true; // for clarity

    if (!nested) {
        clear();
        specialize(0);

        std::int32_t code = reader.expectInt32();
        if (reader.isError()) {
            return false;
        }
        YMSG(("READ got top level code %" PRId32 "\n", code));
        code = code & UNIT_MASK;
        if (code != 0) {
            specialize(code);
        }
    }
    std::int32_t len = reader.expectInt32();
    if (reader.isError()) {
        return false;
    }
    YMSG(("READ bottle length %d\n", len));
    for (int i = 0; i < len; i++) {
        bool ok = fromBytes(reader);
        if (!ok) {
            return false;
        }
    }

    return true;
}

void BottleImpl::toBytes(const Bytes& data)
{
    synch();
    yAssert(data.length() == byteCount());
    memcpy(data.get(), getBytes(), byteCount());
}


const char* BottleImpl::getBytes()
{
    YMSG(("am I nested? %d\n", nested));
    synch();
    return &data[0];
}

size_t BottleImpl::byteCount()
{
    synch();
    return data.size();
}

void BottleImpl::onCommencement()
{
    synch();
}

bool BottleImpl::write(ConnectionWriter& writer)
{
    // could simplify this if knew lengths of blocks up front
    if (writer.isTextMode()) {
        // writer.appendLine(toString());
        writer.appendString(toString().c_str(), '\n');
    } else {
        synch();
        /*
          if (!nested) {
          // No byte count any more, to facilitate nesting
          //YMSG(("bottle byte count %d\n", byteCount()));
          //writer.appendInt32(byteCount()+sizeof(NetInt32));

          writer.appendInt32(StoreList::code + speciality);
          }
        */
        // writer.appendBlockCopy(Bytes((char*)getBytes(), byteCount()));
        writer.appendBlock(getBytes(), byteCount());
    }
    return !writer.isError();
}


bool BottleImpl::read(ConnectionReader& reader)
{
    bool result = false;

    if (reader.isTextMode()) {
        std::string str = reader.expectText().c_str();
        if (reader.isError()) {
            return false;
        }
        bool done = (str.length() <= 0);
        while (!done) {
            if (str[str.length() - 1] == '\\') {
                str = str.substr(0, str.length() - 1);
                str += reader.expectText().c_str();
                if (reader.isError()) {
                    return false;
                }
            } else {
                if (isComplete(str.c_str())) {
                    done = true;
                } else {
                    str += "\n";
                    str += reader.expectText().c_str();
                    if (reader.isError()) {
                        return false;
                    }
                }
            }
        }
        fromString(str);
        result = true;
    } else {
        if (!nested) {
            // no byte length any more to facilitate nesting
            // reader.expectInt32(); // the bottle byte ct; ignored

            clear();
            specialize(0);

            std::int32_t code = reader.expectInt32();
            if (reader.isError()) {
                return false;
            }
            YMSG(("READ got top level code %" PRId32 "\n", code));
            code = code & UNIT_MASK;
            if (code != 0) {
                specialize(code);
            }
        }

        result = true;
        clear();
        dirty = true; // for clarity

        std::int32_t len = 0;
        len = reader.expectInt32();
        if (reader.isError()) {
            return false;
        }
        YMSG(("READ got length %d\n", len));
        for (int i = 0; i < len; i++) {
            bool ok = fromBytes(reader);
            if (!ok) {
                return false;
            }
        }
    }
    return result;
}


void BottleImpl::synch()
{
    if (dirty) {
        if (!nested) {
            subCode();
            YMSG(("bottle code %" PRId32 "\n", StoreList::code + subCode()));
        }
        data.clear();
        BufferedConnectionWriter writer;
        if (!nested) {
            writer.appendInt32(StoreList::code + speciality);
            YMSG(("wrote bottle code %" PRId32 "\n", StoreList::code + speciality));
        }
        YMSG(("bottle length %zd\n", size()));
        writer.appendInt32(static_cast<std::int32_t>(size()));
        for (unsigned int i = 0; i < content.size(); i++) {
            Storable* s = content[i];
            if (speciality == 0) {
                YMSG(("subcode %" PRId32 "\n", s->getCode()));
                writer.appendInt32(s->getCode());
            } else {
                YMSG(("skipped subcode %" PRId32 "\n", s->getCode()));
                yAssert(speciality == s->getCode());
            }
            if (s->isList()) {
                s->asList()->implementation->setNested(true);
            }
            s->writeRaw(writer);
        }
        data.resize(writer.dataSize(), ' ');
        MemoryOutputStream m(&data[0]);
        writer.write(m);
        dirty = false;
    }
}


void BottleImpl::specialize(std::int32_t subCode)
{
    speciality = subCode;
}


int BottleImpl::getSpecialization()
{
    return speciality;
}

void BottleImpl::setNested(bool nested)
{
    this->nested = nested;
}


////////////////////////////////////////////////////////////////////////////
// StoreInt8

std::string StoreInt8::toString() const
{
    char buf[256];
    sprintf(buf, "%" PRId8, x);
    return std::string(buf);
}

void StoreInt8::fromString(const std::string& src)
{
    x = strtol(src.c_str(), static_cast<char**>(nullptr), 0);
}

bool StoreInt8::readRaw(ConnectionReader& reader)
{
    x = reader.expectInt8();
    return true;
}

bool StoreInt8::writeRaw(ConnectionWriter& writer)
{
    writer.appendInt8(x);
    return true;
}


////////////////////////////////////////////////////////////////////////////
// StoreInt16

std::string StoreInt16::toString() const
{
    char buf[256];
    sprintf(buf, "%" PRId16, x);
    return std::string(buf);
}

void StoreInt16::fromString(const std::string& src)
{
    x = strtol(src.c_str(), static_cast<char**>(nullptr), 0);
}

bool StoreInt16::readRaw(ConnectionReader& reader)
{
    x = reader.expectInt16();
    return true;
}

bool StoreInt16::writeRaw(ConnectionWriter& writer)
{
    writer.appendInt16(x);
    return true;
}


////////////////////////////////////////////////////////////////////////////
// StoreInt32

std::string StoreInt32::toString() const
{
    char buf[256];
    sprintf(buf, "%" PRId32, x);
    return std::string(buf);
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

bool StoreInt32::writeRaw(ConnectionWriter& writer)
{
    writer.appendInt32(x);
    return true;
}


////////////////////////////////////////////////////////////////////////////
// StoreInt64

std::string StoreInt64::toString() const
{
    char buf[256];
    sprintf(buf, "%" PRId64, x);
    return std::string(buf);
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

bool StoreInt64::writeRaw(ConnectionWriter& writer)
{
    writer.appendInt64(x);
    return true;
}


////////////////////////////////////////////////////////////////////////////
// StoreVocab

std::string StoreVocab::toString() const
{
    if (x == 0) {
        return "false";
    }
    if (x == '1') {
        return "true";
    }
    return std::string(Vocab::decode(x).c_str());
}

void StoreVocab::fromString(const std::string& src)
{
    x = Vocab::encode(src.c_str());
}

std::string StoreVocab::toStringNested() const
{
    if (x == 0) {
        return "false";
    }
    if (x == '1') {
        return "true";
    }
    return std::string("[") + toString() + "]";
}

void StoreVocab::fromStringNested(const std::string& src)
{
    x = 0;
    if (src.length() > 0) {
        if (src[0] == '[') {
            // ignore first [ and last ]
            std::string buf = src.substr(1, src.length() - 2);
            fromString(buf.c_str());
        } else if (src == "true") {
            x = static_cast<int>('1');
        } else if (src == "false") {
            x = 0;
        }
    }
}

bool StoreVocab::readRaw(ConnectionReader& reader)
{
    x = reader.expectInt32();
    return true;
}

bool StoreVocab::writeRaw(ConnectionWriter& writer)
{
    writer.appendInt32(x);
    return true;
}


////////////////////////////////////////////////////////////////////////////
// StoreFloat32


std::string StoreFloat32::toString() const
{
    return fp_to_string<yarp::conf::float32_t>(x);
}

void StoreFloat32::fromString(const std::string& src)
{
    x = fp_from_string<yarp::conf::float32_t>(src);
}

bool StoreFloat32::readRaw(ConnectionReader& reader)
{
    x = reader.expectFloat32();
    return true;
}

bool StoreFloat32::writeRaw(ConnectionWriter& writer)
{
    writer.appendFloat32(x);
    return true;
}



////////////////////////////////////////////////////////////////////////////
// StoreFloat64

std::string StoreFloat64::toString() const
{
    return fp_to_string<yarp::conf::float64_t>(x);
}

void StoreFloat64::fromString(const std::string& src)
{
    x = fp_from_string<yarp::conf::float64_t>(src);
}

bool StoreFloat64::readRaw(ConnectionReader& reader)
{
    x = reader.expectFloat64();
    return true;
}

bool StoreFloat64::writeRaw(ConnectionWriter& writer)
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

std::string StoreString::toStringNested() const
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
    for (unsigned int j = 0; j < x.length(); j++) {
        char ch = x[j];
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
    std::string buf(YARP_STRINIT(len));
    reader.expectBlock(buf.c_str(), len);
#ifndef YARP_NO_DEPRECATED // Since YARP 2.3.72
    // This is needed for compatibility with versions of yarp before March 2015
    if (len > 0) {
        if (buf[len - 1] == '\0') {
            len--;
        }
    }
#endif // YARP_NO_DEPRECATED
    x = buf.substr(0, len);
    return true;
}

bool StoreString::writeRaw(ConnectionWriter& writer)
{
    writer.appendInt32(static_cast<std::int32_t>(x.length()));
    writer.appendBlock(x.c_str(), x.length());
    return true;
}


////////////////////////////////////////////////////////////////////////////
// StoreBlob

std::string StoreBlob::toString() const
{
    std::string result = "";
    for (unsigned int i = 0; i < x.length(); i++) {
        if (i > 0) {
            result += " ";
        }
        const unsigned char* src =
            reinterpret_cast<const unsigned char*>(&x[i]);
        result += NetType::toString(*src);
    }
    return result;
}

std::string StoreBlob::toStringNested() const
{
    return std::string("{") + toString() + "}";
}

void StoreBlob::fromString(const std::string& src)
{
    Bottle bot(src.c_str());
    std::string buf(YARP_STRINIT(bot.size()));
    for (int i = 0; i < bot.size(); i++) {
        buf[i] =
            static_cast<char>(static_cast<unsigned char>(bot.get(i).asInt32()));
    }
    x = buf;
}

void StoreBlob::fromStringNested(const std::string& src)
{
    if (src.length() > 0) {
        if (src[0] == '{') {
            // ignore first { and last }
            std::string buf = src.substr(1, src.length() - 2);
            fromString(buf.c_str());
        }
    }
}

bool StoreBlob::readRaw(ConnectionReader& reader)
{
    std::int32_t len = reader.expectInt32();
    std::string buf(YARP_STRINIT(len));
    reader.expectBlock(static_cast<const char*>(buf.c_str()), len);
    x = buf;
    return true;
}

bool StoreBlob::writeRaw(ConnectionWriter& writer)
{
    writer.appendInt32(static_cast<std::int32_t>(x.length()));
    writer.appendBlock(x.c_str(), x.length());
    return true;
}


////////////////////////////////////////////////////////////////////////////
// StoreList

std::string StoreList::toString() const
{
    return std::string(content.toString().c_str());
}

std::string StoreList::toStringNested() const
{
    return std::string("(") + content.toString().c_str() + ")";
}

void StoreList::fromString(const std::string& src)
{
    content.fromString(src.c_str());
}

void StoreList::fromStringNested(const std::string& src)
{
    if (src.length() > 0) {
        if (src[0] == '(') {
            // ignore first ( and last )
            std::string buf = src.substr(1, src.length() - 2);
            content.fromString(buf.c_str());
        }
    }
}

bool StoreList::readRaw(ConnectionReader& reader)
{
    // not using the most efficient representation
    content.read(reader);
    return true;
}

bool StoreList::writeRaw(ConnectionWriter& writer)
{
    // not using the most efficient representation
    content.write(writer);
    return true;
}

template <typename T>
inline std::int32_t subCoder(T& content)
{
    std::int32_t c = -1;
    bool ok = false;
    for (unsigned int i = 0; i < content.size(); ++i) {
        std::int32_t sc = content.get(i).getCode();
        if (c == -1) {
            c = sc;
            ok = true;
        }
        if (sc != c) {
            ok = false;
        }
    }
    // just optimize primitive types
    if ((c & GROUP_MASK) != 0) {
        ok = false;
    }
    c = ok ? c : 0;
    content.specialize(c);
    return c;
}

std::int32_t StoreList::subCode() const
{
    return subCoder(*(content.implementation));
}


////////////////////////////////////////////////////////////////////////////
// StoreDict

std::string StoreDict::toString() const
{
    return std::string(content.toString().c_str());
}

std::string StoreDict::toStringNested() const
{
    return std::string("(") + content.toString().c_str() + ")";
}

void StoreDict::fromString(const std::string& src)
{
    content.fromString(src.c_str());
}

void StoreDict::fromStringNested(const std::string& src)
{
    if (src.length() > 0) {
        if (src[0] == '(') {
            // ignore first ( and last )
            std::string buf = src.substr(1, src.length() - 2);
            content.fromString(buf.c_str());
        }
    }
}

bool StoreDict::readRaw(ConnectionReader& reader)
{
    // not using the most efficient representation
    content.read(reader);
    return true;
}

bool StoreDict::writeRaw(ConnectionWriter& writer)
{
    // not using the most efficient representation
    content.write(writer);
    return true;
}

////////////////////////////////////////////////////////////////////////////
// BottleImpl


std::int32_t BottleImpl::subCode()
{
    return subCoder(*this);
}

bool BottleImpl::checkIndex(int index) const
{
    if (index >= 0 && index < static_cast<int>(size())) {
        return true;
    }
    return false;
}

bool BottleImpl::isInt8(int index)
{
    return (checkIndex(index) ? content[index]->isInt8() : false);
}

bool BottleImpl::isInt16(int index)
{
    return (checkIndex(index) ? content[index]->isInt16() : false);
}

bool BottleImpl::isInt32(int index)
{
    return (checkIndex(index) ? content[index]->isInt32() : false);
}

bool BottleImpl::isInt64(int index)
{
    return (checkIndex(index) ? content[index]->isInt64() : false);
}

bool BottleImpl::isFloat32(int index)
{
    return (checkIndex(index) ? content[index]->isFloat32() : false);
}

bool BottleImpl::isFloat64(int index)
{
    return (checkIndex(index) ? content[index]->isFloat64() : false);
}

bool BottleImpl::isString(int index)
{
    return (checkIndex(index) ? content[index]->isString() : false);
}

bool BottleImpl::isList(int index)
{
    return (checkIndex(index) ? content[index]->isList() : false);
}

Storable* BottleImpl::pop()
{
    Storable* stb = nullptr;
    if (size() == 0) {
        stb = new StoreNull();
    } else {
        stb = content[size() - 1];
        content.pop_back();
        dirty = true;
    }
    yAssert(stb != nullptr);
    return stb;
}

Storable& BottleImpl::get(int index) const
{
    return (checkIndex(index) ? *(content[index]) : getNull());
}

yarp::os::Bottle& BottleImpl::addList()
{
    StoreList* lst = new StoreList();
    add(lst);
    return lst->internal();
}

yarp::os::Property& BottleImpl::addDict()
{
    StoreDict* lst = new StoreDict();
    add(lst);
    return lst->internal();
}

void BottleImpl::copyRange(const BottleImpl* alt, int first, int len)
{
    if (len == 0) {
        clear();
        return;
    }

    const BottleImpl* src = alt;
    BottleImpl tmp(nullptr);
    if (alt == this) {
        tmp.fromString(toString());
        src = &tmp;
    }

    clear();
    if (len == -1) {
        len = static_cast<int>(src->size());
    }
    int last = first + len - 1;
    int top = static_cast<int>(src->size()) - 1;
    if (first < 0) {
        first = 0;
    }
    if (last < 0) {
        last = 0;
    }
    if (first > top) {
        first = top;
    }
    if (last > top) {
        last = top;
    }

    if (last >= 0) {
        for (int i = first; i <= last; i++) {
            add(src->get(i).cloneStorable());
        }
    }
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
    return std::string(toString().c_str()) == alt.toString().c_str();
}


bool Storable::read(ConnectionReader& connection)
{
    std::int32_t x = connection.expectInt32();
    if (x != getCode()) {
        return false;
    }
    return readRaw(connection);
}

bool Storable::write(ConnectionWriter& connection)
{
    connection.appendInt32(getCode());
    return writeRaw(connection);
}

void BottleImpl::edit()
{
    if (ro) {
        yFatal("Attempted to modify the null bottle");
    }
    if (invalid) {
        invalid = false;
    }
}

Value& BottleImpl::findGroupBit(const std::string& key) const
{
    for (size_t i = 0; i < size(); i++) {
        Value* org = &(get((int)i));
        Value* cursor = org;
        if (cursor->isList()) {
            cursor = &(cursor->asList()->get(0));
        }
        if (key == cursor->toString()) {
            return *org;
        }
    }
    // return invalid object
    return get(-1);
}

Value& BottleImpl::findBit(const std::string& key) const
{
    for (size_t i = 0; i < size(); i++) {
        Value* org = &(get((int)i));
        Value* cursor = org;
        bool nested = false;
        if (cursor->isList()) {
            Bottle* bot = cursor->asList();
            cursor = &(bot->get(0));
            nested = true;
        }
        if (key == cursor->toString()) {
            if (nested) {
                return org->asList()->get(1);
            }
            if (parent && parent->getMonitor() != nullptr) {
                SearchReport report;
                report.key = key;
                report.isFound = true;
                if (size() == 2) {
                    report.value = get((int)(i + 1)).toString();
                }
                if (parent) {
                    parent->reportToMonitor(report);
                }
            }
            return get((int)(i + 1));
        }
    }
    // return invalid object
    if (parent && parent->getMonitor() != nullptr) {
        SearchReport report;
        report.key = key;
        parent->reportToMonitor(report);
    }
    return get(-1);
}
