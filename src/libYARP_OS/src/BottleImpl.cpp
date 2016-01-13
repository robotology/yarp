/*
 * Copyright (C) 2006, 2008 RobotCub Consortium, Arjan Gijsberts
 * Authors: Paul Fitzpatrick, Arjan Gijsberts
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include <yarp/os/impl/BottleImpl.h>

#include <yarp/os/NetFloat64.h>
#include <yarp/os/StringInputStream.h>
#include <yarp/os/StringOutputStream.h>
#include <yarp/os/Vocab.h>

#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/os/impl/MemoryOutputStream.h>
#include <yarp/os/impl/PlatformStdio.h>
#include <yarp/os/impl/PlatformStdlib.h>
#include <yarp/os/impl/StreamConnectionReader.h>

#include <clocale>

using yarp::os::impl::String;
using yarp::os::impl::StoreInt;
using yarp::os::impl::StoreVocab;
using yarp::os::impl::StoreDouble;
using yarp::os::impl::StoreString;
using yarp::os::impl::StoreList;
using yarp::os::impl::StoreBlob;
using yarp::os::impl::StoreDict;
using yarp::os::impl::StoreInt64;
using yarp::os::impl::BottleImpl;
using yarp::os::impl::Storable;
using yarp::os::Bytes;
using yarp::os::ConnectionReader;
using yarp::os::ConnectionWriter;
using yarp::os::Bottle;
using yarp::os::ConstString;
using yarp::os::Searchable;
using yarp::os::Value;

#define YARP_STRINIT(len) ((size_t)(len)), 0

//#define YMSG(x) ACE_OS::printf x;
//#define YTRACE(x) YMSG(("at %s\n",x))

#define YMSG(x)
#define YTRACE(x)

// YARP1 compatible codes
// const int StoreInt::code = 1;
// const int StoreString::code = 5;
// const int StoreDouble::code = 2;
// const int StoreList::code = 16;
// const int StoreVocab::code = 32;
// const int StoreBlob::code = 33;
//#define USE_YARP1_PREFIX

// new YARP2 codes
const int StoreInt::code = BOTTLE_TAG_INT;
const int StoreVocab::code = BOTTLE_TAG_VOCAB;
const int StoreDouble::code = BOTTLE_TAG_DOUBLE;
const int StoreString::code = BOTTLE_TAG_STRING;
const int StoreBlob::code = BOTTLE_TAG_BLOB;
const int StoreList::code = BOTTLE_TAG_LIST;
const int StoreDict::code = BOTTLE_TAG_LIST | BOTTLE_TAG_DICT;
const int StoreInt64::code = BOTTLE_TAG_INT64;

#define UNIT_MASK                                            \
    (BOTTLE_TAG_INT | BOTTLE_TAG_VOCAB | BOTTLE_TAG_DOUBLE | \
     BOTTLE_TAG_STRING | BOTTLE_TAG_BLOB | BOTTLE_TAG_INT64)
#define GROUP_MASK (BOTTLE_TAG_LIST | BOTTLE_TAG_DICT)


yarp::os::impl::StoreNull* BottleImpl::storeNull = NULL;

BottleImpl::BottleImpl() : parent(NULL)
{
    dirty = true;
    nested = false;
    speciality = 0;
}

BottleImpl::BottleImpl(Searchable* parent) : parent(parent)
{
    dirty = true;
    nested = false;
    speciality = 0;
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

void BottleImpl::smartAdd(const String& str)
{
    if (str.length() > 0) {
        char ch = str[0];
        Storable* s = NULL;
        StoreString* ss = NULL;
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
                s = new StoreInt(0);
            } else {
                s = new StoreDouble(0);
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
        if (s != NULL) {
            s->fromStringNested(str);
            if (ss != NULL) {
                if (str.length() == 0 || str[0] != '\"') {
                    String val = ss->asStringFlex();
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
        ss = NULL;
    }
}

void BottleImpl::fromString(const String& line)
{
    clear();
    dirty = true;
    String arg = "";
    bool quoted = false;
    bool back = false;
    bool begun = false;
    int nested = 0;
    int nestedAlt = 0;
    String nline = line + " ";

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
    String nline = txt;
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


ConstString BottleImpl::toString()
{
    ConstString result = "";
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

Storable* Storable::createByCode(int id)
{
    Storable* storable = NULL;
    int subCode = 0;
    switch (id) {
    case StoreInt::code:
        storable = new StoreInt();
        break;
    case StoreVocab::code:
        storable = new StoreVocab();
        break;
    case StoreDouble::code:
        storable = new StoreDouble();
        break;
    case StoreString::code:
        storable = new StoreString();
        break;
    case StoreBlob::code:
        storable = new StoreBlob();
        break;
    case StoreList::code:
        storable = new StoreList();
        yAssert(storable != NULL);
        storable->asList()->implementation->setNested(true);
        break;
    case StoreInt64::code:
        storable = new StoreInt64();
        break;
    default:
        if ((id & GROUP_MASK) != 0) {
            // typed list
            subCode = (id & UNIT_MASK);
            if (id & BOTTLE_TAG_DICT) {
                storable = new StoreDict();
                yAssert(storable != NULL);
            } else {
                storable = new StoreList();
                yAssert(storable != NULL);
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
    int id = speciality;
    YMSG(("READING, nest flag is %d\n", nested));
    if (id == 0) {
        id = reader.expectInt();
        YMSG(("READ subcode %d\n", id));
    } else {
        YMSG(("READ skipped subcode %d\n", speciality));
    }
    Storable* storable = Storable::createByCode(id);
    if (storable == NULL) {
        YARP_SPRINTF1(Logger::get(), error,
                      "BottleImpl reader failed, unrecognized object code %d",
                      id);
        return false;
    }
    storable->readRaw(reader);
    add(storable);
    return true;
}


void BottleImpl::fromBinary(const char* text, int len)
{
    ConstString wrapper(text, len);
    StringInputStream sis;
    sis.add(wrapper);
    StreamConnectionReader reader;
    Route route;
    reader.reset(sis, NULL, route, len, false);
    read(reader);
}


bool BottleImpl::fromBytes(const Bytes& data)
{
    ConstString wrapper(data.get(), data.length());
    StringInputStream sis;
    sis.add(wrapper);
    StreamConnectionReader reader;
    Route route;
    reader.reset(sis, NULL, route, data.length(), false);

    clear();
    dirty = true; // for clarity

    if (!nested) {
        clear();
        specialize(0);

        int code = reader.expectInt();
        if (reader.isError()) {
            return false;
        }
        YMSG(("READ got top level code %d\n", code));
        code = code & UNIT_MASK;
        if (code != 0) {
            specialize(code);
        }
    }
    int len = reader.expectInt();
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
    ACE_OS::memcpy(data.get(), getBytes(), byteCount());
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
#ifdef USE_YARP1_PREFIX
        if (!nested) {
            ConstString name = "YARP2";
            writer.appendInt(name.length() + 1);
            writer.appendString(name.c_str(), '\0');
        }
#endif
        synch();
        /*
          if (!nested) {
          // No byte count any more, to facilitate nesting
          //YMSG(("bottle byte count %d\n",byteCount()));
          //writer.appendInt(byteCount()+sizeof(NetInt32));

          writer.appendInt(StoreList::code + speciality);
          }
        */
        // writer.appendBlockCopy(Bytes((char*)getBytes(),byteCount()));
        writer.appendBlock(getBytes(), byteCount());
    }
    return !writer.isError();
}


bool BottleImpl::read(ConnectionReader& reader)
{
    bool result = false;

    if (reader.isTextMode()) {
        ConstString str = reader.expectText().c_str();
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
#ifdef USE_YARP1_PREFIX
        if (!nested) {
            int len = reader.expectInt();
            if (reader.isError()) {
                return false;
            }
            // ConstString name = reader.expectString(len);
            ConstString buf(YARP_STRINIT(len));
            reader.expectBlock((const char*)buf.c_str(), len);
            if (reader.isError()) {
                return false;
            }
            ConstString name = buf.c_str();
        }
#endif
        if (!nested) {
            // no byte length any more to facilitate nesting
            // reader.expectInt(); // the bottle byte ct; ignored

            clear();
            specialize(0);

            int code = reader.expectInt();
            if (reader.isError()) {
                return false;
            }
            YMSG(("READ got top level code %d\n", code));
            code = code & UNIT_MASK;
            if (code != 0) {
                specialize(code);
            }
        }

        result = true;
        clear();
        dirty = true; // for clarity

        int len = 0;
        int i = 0;
        len = reader.expectInt();
        if (reader.isError()) {
            return false;
        }
        YMSG(("READ got length %d\n", len));
        for (i = 0; i < len; i++) {
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
            YMSG(("bottle code %d\n", StoreList::code + subCode()));
        }
        data.clear();
        BufferedConnectionWriter writer;
        if (!nested) {
            writer.appendInt(StoreList::code + speciality);
            YMSG(("wrote bottle code %d\n", StoreList::code + speciality));
        }
        YMSG(("bottle length %d\n", size()));
        writer.appendInt(static_cast<int>(size()));
        for (unsigned int i = 0; i < content.size(); i++) {
            Storable* s = content[i];
            if (speciality == 0) {
                YMSG(("subcode %d\n", s->getCode()));
                writer.appendInt(s->getCode());
            } else {
                YMSG(("skipped subcode %d\n", s->getCode()));
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


void BottleImpl::specialize(int subCode)
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
// StoreInt

ConstString StoreInt::toStringFlex() const
{
    char buf[256];
    ACE_OS::sprintf(buf, "%d", x);
    return ConstString(buf);
}

void StoreInt::fromString(const ConstString& src)
{
    // x = ACE_OS::atoi(src.c_str());
    x = ACE_OS::strtol(src.c_str(), static_cast<char**>(NULL), 0);
}

bool StoreInt::readRaw(ConnectionReader& reader)
{
    x = reader.expectInt();
    return true;
}

bool StoreInt::writeRaw(ConnectionWriter& writer)
{
    writer.appendInt(x);
    return true;
}


////////////////////////////////////////////////////////////////////////////
// StoreInt64

ConstString StoreInt64::toStringFlex() const
{
    char buf[256];
    ACE_OS::sprintf(buf, "%" YARP_INT64_FMT, x);
    return ConstString(buf);
}

void StoreInt64::fromString(const ConstString& src)
{
    x = ACE_OS::strtoll(src.c_str(), static_cast<char**>(NULL), 0);
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

ConstString StoreVocab::toStringFlex() const
{
    if (x == 0) {
        return "false";
    }
    if (x == '1') {
        return "true";
    }
    return ConstString(Vocab::decode(x).c_str());
}

void StoreVocab::fromString(const ConstString& src)
{
    x = Vocab::encode(src.c_str());
}

ConstString StoreVocab::toStringNested() const
{
    if (x == 0) {
        return "false";
    }
    if (x == '1') {
        return "true";
    }
    return ConstString("[") + toStringFlex() + "]";
}

void StoreVocab::fromStringNested(const ConstString& src)
{
    x = 0;
    if (src.length() > 0) {
        if (src[0] == '[') {
            // ignore first [ and last ]
            ConstString buf = src.substr(1, src.length() - 2);
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
    x = reader.expectInt();
    return true;
}

bool StoreVocab::writeRaw(ConnectionWriter& writer)
{
    writer.appendInt(x);
    return true;
}


////////////////////////////////////////////////////////////////////////////
// StoreDouble

ConstString StoreDouble::toStringFlex() const
{
    char buf[512];
    ACE_OS::sprintf(buf, "%f", x);
    ConstString str(buf);

    // YARP Bug 2526259: Locale settings influence YARP behavior
    // Need to deal with alternate versions of the decimal point.
    struct lconv* lc = localeconv();
    size_t offset = str.find(lc->decimal_point);
    if (offset != ConstString::npos) {
        str[offset] = '.';
    } else {
        str += ".0";
    }

    int ct = 0;
    for (size_t i = str.length(); i >= 1; i--) {
        if (str[i - 1] != '0') {
            if (str[i - 1] == '.') {
                ct--;
                i++;
            }
            if (ct >= 1) {
                str = str.substr(0, i);
            }
            break;
        }
        ct++;
    }
    return str;
}

void StoreDouble::fromString(const ConstString& src)
{
    // YARP Bug 2526259: Locale settings influence YARP behavior
    // Need to deal with alternate versions of the decimal point.
    ConstString tmp = src;
    size_t offset = tmp.find(".");
    if (offset != ConstString::npos) {
        struct lconv* lc = localeconv();
        tmp[offset] = lc->decimal_point[0];
    }
    x = ACE_OS::strtod(tmp.c_str(), NULL);
}

bool StoreDouble::readRaw(ConnectionReader& reader)
{
    NetFloat64 flt = 0;
    reader.expectBlock(reinterpret_cast<const char*>(&flt), sizeof(flt));
    x = flt;
    return true;
}

bool StoreDouble::writeRaw(ConnectionWriter& writer)
{
    // writer.appendBlockCopy(Bytes((char*)&x,sizeof(x)));
    NetFloat64 flt = x;
    writer.appendBlock(reinterpret_cast<char*>(&flt), sizeof(flt));
    return true;
}


////////////////////////////////////////////////////////////////////////////
// StoreString

ConstString StoreString::toStringFlex() const
{
    return x;
}

ConstString StoreString::toStringNested() const
{
    // quoting code: very inefficient, but portable
    ConstString result;

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

void StoreString::fromString(const ConstString& src)
{
    x = src;
}

void StoreString::fromStringNested(const ConstString& src)
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
    int len = reader.expectInt();
    ConstString buf(YARP_STRINIT(len));
    reader.expectBlock(buf.c_str(), len);
    // This is needed for compatiblity with versions of yarp before March 2015
    if (len > 0) {
        if (buf[len - 1] == '\0') {
            len--;
        }
    }
    x = buf.substr(0, len);
    return true;
}

bool StoreString::writeRaw(ConnectionWriter& writer)
{
    writer.appendInt(static_cast<int>(x.length()));
    writer.appendBlock(x.c_str(), x.length());
    return true;
}


////////////////////////////////////////////////////////////////////////////
// StoreBlob

ConstString StoreBlob::toStringFlex() const
{
    ConstString result = "";
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

ConstString StoreBlob::toStringNested() const
{
    return ConstString("{") + toStringFlex() + "}";
}

void StoreBlob::fromString(const ConstString& src)
{
    Bottle bot(src.c_str());
    ConstString buf(YARP_STRINIT(bot.size()));
    for (int i = 0; i < bot.size(); i++) {
        buf[i] =
            static_cast<char>(static_cast<unsigned char>(bot.get(i).asInt()));
    }
    x = buf;
}

void StoreBlob::fromStringNested(const ConstString& src)
{
    if (src.length() > 0) {
        if (src[0] == '{') {
            // ignore first { and last }
            ConstString buf = src.substr(1, src.length() - 2);
            fromString(buf.c_str());
        }
    }
}

bool StoreBlob::readRaw(ConnectionReader& reader)
{
    int len = reader.expectInt();
    ConstString buf(YARP_STRINIT(len));
    reader.expectBlock(static_cast<const char*>(buf.c_str()), len);
    x = buf;
    return true;
}

bool StoreBlob::writeRaw(ConnectionWriter& writer)
{
    writer.appendInt(static_cast<int>(x.length()));
    writer.appendBlock(x.c_str(), x.length());
    return true;
}


////////////////////////////////////////////////////////////////////////////
// StoreList

ConstString StoreList::toStringFlex() const
{
    return ConstString(content.toString().c_str());
}

ConstString StoreList::toStringNested() const
{
    return ConstString("(") + content.toString().c_str() + ")";
}

void StoreList::fromString(const ConstString& src)
{
    content.fromString(src.c_str());
}

void StoreList::fromStringNested(const ConstString& src)
{
    if (src.length() > 0) {
        if (src[0] == '(') {
            // ignore first ( and last )
            ConstString buf = src.substr(1, src.length() - 2);
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

template <class T>
int subCoder(T& content)
{
    int c = -1;
    bool ok = false;
    for (unsigned int i = 0; i < content.size(); ++i) {
        int sc = content.get(i).getCode();
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

int StoreList::subCode() const
{
    return subCoder(*(content.implementation));
}


////////////////////////////////////////////////////////////////////////////
// StoreDict

ConstString StoreDict::toStringFlex() const
{
    return ConstString(content.toString().c_str());
}

ConstString StoreDict::toStringNested() const
{
    return ConstString("(") + content.toString().c_str() + ")";
}

void StoreDict::fromString(const ConstString& src)
{
    content.fromString(src.c_str());
}

void StoreDict::fromStringNested(const ConstString& src)
{
    if (src.length() > 0) {
        if (src[0] == '(') {
            // ignore first ( and last )
            ConstString buf = src.substr(1, src.length() - 2);
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


int BottleImpl::subCode()
{
    return subCoder(*this);
}

bool BottleImpl::isInt(int index)
{
    if (index >= 0 && index < static_cast<int>(size())) {
        return content[index]->getCode() == StoreInt::code;
    }
    return false;
}


bool BottleImpl::isString(int index)
{
    if (index >= 0 && index < static_cast<int>(size())) {
        return content[index]->getCode() == StoreString::code;
    }
    return false;
}

bool BottleImpl::isDouble(int index)
{
    if (index >= 0 && index < static_cast<int>(size())) {
        return content[index]->getCode() == StoreDouble::code;
    }
    return false;
}


bool BottleImpl::isList(int index)
{
    if (index >= 0 && index < static_cast<int>(size())) {
        return content[index]->isList();
    }
    return false;
}

Storable* BottleImpl::pop()
{
    Storable* stb = NULL;
    if (size() == 0) {
        stb = new StoreNull();
    } else {
        stb = content[size() - 1];
        content.pop_back();
        dirty = true;
    }
    yAssert(stb != NULL);
    return stb;
}

Storable& BottleImpl::get(int index) const
{
    if (index >= 0 && index < static_cast<int>(size())) {
        return *(content[index]);
    }
    return getNull();
}

int BottleImpl::getInt(int index)
{
    if (!isInt(index)) {
        return 0;
    }
    return content[index]->asInt();
}

yarp::os::ConstString BottleImpl::getString(int index)
{
    if (!isString(index)) {
        return "";
    }
    return content[index]->asString();
}

double BottleImpl::getDouble(int index)
{
    if (!isDouble(index)) {
        return 0;
    }
    return content[index]->asDouble();
}

yarp::os::Bottle* BottleImpl::getList(int index)
{
    if (!isList(index)) {
        return NULL;
    }
    return &((dynamic_cast<StoreList*>(content[index]))->internal());
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
    BottleImpl tmp(NULL);
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

Value& Storable::find(const ConstString& key) const
{
    YARP_UNUSED(key);
    return BottleImpl::getNull();
}

Bottle& Storable::findGroup(const ConstString& key) const
{
    YARP_UNUSED(key);
    return Bottle::getNullBottle();
}

bool Storable::check(const ConstString& key) const
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
    return String(toString().c_str()) == alt.toString().c_str();
}


bool Storable::read(ConnectionReader& connection)
{
    int x = connection.expectInt();
    if (x != getCode()) {
        return false;
    }
    return readRaw(connection);
}

bool Storable::write(ConnectionWriter& connection)
{
    connection.appendInt(getCode());
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

Value& BottleImpl::findGroupBit(const ConstString& key) const
{
    for (size_t i = 0; i < size(); i++) {
        Value* org = &(get(i));
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

Value& BottleImpl::findBit(const ConstString& key) const
{
    for (size_t i = 0; i < size(); i++) {
        Value* org = &(get(i));
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
            if (parent && parent->getMonitor() != NULL) {
                SearchReport report;
                report.key = key;
                report.isFound = true;
                if (size() == 2) {
                    report.value = get(i + 1).toString();
                }
                if (parent) {
                    parent->reportToMonitor(report);
                }
            }
            return get(i + 1);
        }
    }
    // return invalid object
    if (parent && parent->getMonitor() != NULL) {
        SearchReport report;
        report.key = key;
        parent->reportToMonitor(report);
    }
    return get(-1);
}
