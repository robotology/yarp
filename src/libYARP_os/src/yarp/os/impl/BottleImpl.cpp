/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-FileCopyrightText: 2006, 2008 Arjan Gijsberts
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/impl/BottleImpl.h>

#include <yarp/conf/numeric.h>

#include <yarp/os/StringInputStream.h>
#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/os/impl/LogComponent.h>
#include <yarp/os/impl/MemoryOutputStream.h>
#include <yarp/os/impl/StreamConnectionReader.h>

#include <limits>

using yarp::os::Bottle;
using yarp::os::Bytes;
using yarp::os::ConnectionReader;
using yarp::os::ConnectionWriter;
using yarp::os::Searchable;
using yarp::os::Value;
using yarp::os::impl::BottleImpl;
using yarp::os::impl::Storable;

namespace {
YARP_OS_LOG_COMPONENT(BOTTLEIMPL, "yarp.os.impl.BottleImpl")
} // namespace

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
    for (auto& i : content) {
        delete i;
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
        size_t hexStart = 0;
        int periodCount = 0;
        int signCount = 0;
        bool hasPeriodOrE = false;
        for (size_t i = 0; i < str.length(); i++) {
            if (str == "inf" || str == "-inf" || str == "nan") {
                hasPeriodOrE = true;
                break;
            }
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
            ((ch >= '0' && ch <= '9') || ch == '+' || ch == '-' || ch == '.' || ch == 'i' /* inf */ || ch == 'n' /* nan */) &&
            (ch != '.' || str.length() > 1)) {
            if (!hasPeriodOrE) {
                s = new StoreInt64(0);
            } else {
                s = new StoreFloat64(0);
            }
        } else if (ch == '(') {
            s = new StoreList();
        } else if (ch == '[') {
            s = new StoreVocab32();
        } else if (ch == '{') {
            s = new StoreBlob();
        } else {
            s = ss = new StoreString("");
        }
        if (s != nullptr) {
            s->fromStringNested(str);

            // Traditionally all int are read as 32 bit integers, but 64 bit
            // integers will not fit.
            // Therefore the value is read as 64 bit, but if the value would fit
            // a 32 bit integer, it is cast to a 32 bit integer.
            if (s->isInt64()
                && s->asInt64() >= std::numeric_limits<int32_t>::min()
                && s->asInt64() <= std::numeric_limits<int32_t>::max()) {
                Storable* s_i32 = new StoreInt32(s->asInt32());
                delete s;
                s = s_i32;
                s_i32 = nullptr;
            }

            if (ss != nullptr) {
                if (str.length() == 0 || str[0] != '\"') {
                    std::string val = ss->asString();
                    if (val == "true") {
                        delete s;
                        s = new StoreVocab32(static_cast<int>('1'));
                    } else if (val == "false") {
                        delete s;
                        s = new StoreVocab32(0);
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
    std::string arg;
    bool quoted = false;
    bool back = false;
    bool begun = false;
    int nested = 0;
    int nestedAlt = 0;
    std::string nline = line + " ";

    for (char ch : nline) {
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
                        if (!arg.empty()) {
                            if (arg == "null") {
                                add(new StoreVocab32(yarp::os::createVocab32('n', 'u', 'l', 'l')));
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

    for (char ch : nline) {
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
    return nested == 0 && nestedAlt == 0 && !quoted;
}


std::string BottleImpl::toString() const
{
    std::string result;
    for (unsigned int i = 0; i < content.size(); i++) {
        if (i > 0) {
            result += " ";
        }
        Storable& s = *content[i];
        result += s.toStringNested();
    }
    return result;
}

BottleImpl::size_type BottleImpl::size() const
{
    return content.size();
}


bool BottleImpl::fromBytes(ConnectionReader& reader)
{
    if (reader.isError()) {
        return false;
    }
    std::int32_t id = speciality;
    yCTrace(BOTTLEIMPL, "READING, nest flag is %d", nested);
    if (id == 0) {
        id = reader.expectInt32();
        yCTrace(BOTTLEIMPL, "READ subcode %" PRId32, id);
    } else {
        yCTrace(BOTTLEIMPL, "READ skipped subcode %" PRId32, speciality);
    }
    Storable* storable = Storable::createByCode(id);
    if (storable == nullptr) {
        yCError(BOTTLEIMPL, "Reader failed, unrecognized object code %" PRId32, id);
        return false;
    }
    storable->readRaw(reader);
    add(storable);
    return true;
}


void BottleImpl::fromBinary(const char* text, size_t len)
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
        yCTrace(BOTTLEIMPL, "READ got top level code %" PRId32, code);
        code = code & UNIT_MASK;
        if (code != 0) {
            specialize(code);
        }
    }
    std::int32_t len = reader.expectInt32();
    if (reader.isError()) {
        return false;
    }
    yCTrace(BOTTLEIMPL, "READ bottle length %d", len);
    for (int i = 0; i < len; i++) {
        bool ok = fromBytes(reader);
        if (!ok) {
            return false;
        }
    }

    return true;
}

void BottleImpl::toBytes(Bytes& data)
{
    synch();
    yCAssert(BOTTLEIMPL, data.length() == byteCount());
    memcpy(data.get(), getBytes(), byteCount());
}


const char* BottleImpl::getBytes() const
{
    yCTrace(BOTTLEIMPL, "am I nested? %d", nested);
    synch();
    return &data[0];
}

size_t BottleImpl::byteCount() const
{
    synch();
    return data.size();
}

void BottleImpl::onCommencement()
{
    synch();
}

bool BottleImpl::write(ConnectionWriter& writer) const
{
    // could simplify this if knew lengths of blocks up front
    if (writer.isTextMode()) {
        writer.appendText(toString());
    } else {
        synch();
        writer.appendBlock(getBytes(), byteCount());
    }
    return !writer.isError();
}


bool BottleImpl::read(ConnectionReader& reader)
{
    bool result = false;

    if (reader.isTextMode()) {
        std::string str = reader.expectText();
        if (reader.isError()) {
            return false;
        }
        bool done = (str.length() <= 0);
        while (!done) {
            if (str[str.length() - 1] == '\\') {
                str = str.substr(0, str.length() - 1);
                str += reader.expectText();
                if (reader.isError()) {
                    return false;
                }
            } else {
                if (isComplete(str.c_str())) {
                    done = true;
                } else {
                    str += "\n";
                    str += reader.expectText();
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
            yCTrace(BOTTLEIMPL, "READ got top level code %" PRId32, code);
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
        yCTrace(BOTTLEIMPL, "READ got length %d", len);
        for (int i = 0; i < len; i++) {
            bool ok = fromBytes(reader);
            if (!ok) {
                return false;
            }
        }
    }
    return result;
}

void BottleImpl::synch() const
{
    const_cast<BottleImpl*>(this)->synch();
}

void BottleImpl::synch()
{
    if (dirty) {
        if (!nested) {
            subCode();
            yCTrace(BOTTLEIMPL, "bottle code %" PRId32, StoreList::code + subCode());
        }
        data.clear();
        BufferedConnectionWriter writer;
        if (!nested) {
            writer.appendInt32(StoreList::code + speciality);
            yCTrace(BOTTLEIMPL, "wrote bottle code %" PRId32, StoreList::code + speciality);
        }
        yCTrace(BOTTLEIMPL, "bottle length %zd", size());
        writer.appendInt32(static_cast<std::int32_t>(size()));
        for (auto s : content) {
            if (speciality == 0) {
                yCTrace(BOTTLEIMPL, "subcode %" PRId32, s->getCode());
                writer.appendInt32(s->getCode());
            } else {
                yCTrace(BOTTLEIMPL, "skipped subcode %" PRId32, s->getCode());
                yCAssert(BOTTLEIMPL, speciality == s->getCode());
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


std::int32_t BottleImpl::subCode()
{
    return subCoder(*this);
}

bool BottleImpl::checkIndex(size_type index) const
{
    return index < size();
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
    yCAssert(BOTTLEIMPL, stb != nullptr);
    return stb;
}

Storable& BottleImpl::get(size_type index) const
{
    return (checkIndex(index) ? *(content[index]) : getNull());
}

yarp::os::Bottle& BottleImpl::addList()
{
    auto* lst = new StoreList();
    add(lst);
    return lst->internal();
}

yarp::os::Property& BottleImpl::addDict()
{
    auto* lst = new StoreDict();
    add(lst);
    return lst->internal();
}

void BottleImpl::copyRange(const BottleImpl* alt, size_type first, size_type len)
{

    if (len == 0 || alt->size() == 0) {
        clear();
        return;
    }

    // Handle copying to the same object just a subset of the bottle
    const BottleImpl* src = alt;
    BottleImpl tmp(nullptr);
    if (alt == this) {
        tmp.fromString(toString());
        src = &tmp;
    }

    clear();

    const size_t last = src->size() - 1;
    for (size_t i = 0; (i < len) && (first + i <= last); ++i) {
        add(src->get(first + i).cloneStorable());
    }
}

void BottleImpl::edit()
{
    if (ro) {
        yCFatal(BOTTLEIMPL, "Attempted to modify the null bottle");
    }
    if (invalid) {
        invalid = false;
    }
}

Value& BottleImpl::findGroupBit(const std::string& key) const
{
    for (size_t i = 0; i < size(); i++) {
        Value* org = &(get(static_cast<int>(i)));
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
        Value* org = &(get(static_cast<int>(i)));
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
            if ((parent != nullptr) && (parent->getMonitor() != nullptr)) {
                SearchReport report;
                report.key = key;
                report.isFound = true;
                if (size() == 2) {
                    report.value = get(static_cast<int>(i + 1)).toString();
                }
                if (parent != nullptr) {
                    parent->reportToMonitor(report);
                }
            }
            return get(static_cast<int>(i + 1));
        }
    }
    // return invalid object
    if ((parent != nullptr) && (parent->getMonitor() != nullptr)) {
        SearchReport report;
        report.key = key;
        parent->reportToMonitor(report);
    }
    return get(-1);
}
