/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/impl/BufferedConnectionWriter.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/DummyConnector.h>
#include <yarp/os/ManagedBytes.h>
#include <yarp/os/NetFloat32.h>
#include <yarp/os/NetFloat64.h>
#include <yarp/os/NetInt16.h>
#include <yarp/os/NetInt32.h>
#include <yarp/os/NetInt64.h>
#include <yarp/os/NetInt8.h>
#include <yarp/os/NetType.h>
#include <yarp/os/SizedWriter.h>
#include <yarp/os/StringOutputStream.h>
#include <yarp/os/Vocab.h>

#include <cstdlib>
#include <cstring>


using namespace yarp::os::impl;
using namespace yarp::os;

BufferedConnectionWriter::BufferedConnectionWriter(bool textMode,
                                                   bool bareMode) :
        target(&lst),
        reader(nullptr),
        textMode(textMode),
        bareMode(bareMode),
        convertTextModePending(false),
        ref(nullptr),
        shouldDrop(false),
        lst_used(0),
        header_used(0),
        target_used(&lst_used),
        initialPoolSize(BUFFERED_CONNECTION_INITIAL_POOL_SIZE)
{
    stopPool();
}


BufferedConnectionWriter::~BufferedConnectionWriter()
{
    clear();
}


void BufferedConnectionWriter::reset(bool textMode)
{
    this->textMode = textMode;
    clear();
    reader = nullptr;
    ref = nullptr;
    convertTextModePending = false;
}

void BufferedConnectionWriter::restart()
{
    lst_used = 0;
    header_used = 0;
    reader = nullptr;
    ref = nullptr;
    convertTextModePending = false;
    target = &lst;
    target_used = &lst_used;
    stopPool();
}

void BufferedConnectionWriter::clear()
{
    target = &lst;
    target_used = &lst_used;

    size_t i;
    for (i = 0; i < lst.size(); i++) {
        delete lst[i];
    }
    lst.clear();
    for (i = 0; i < header.size(); i++) {
        delete header[i];
    }
    header.clear();
    stopPool();
    lst_used = 0;
    header_used = 0;
}

bool BufferedConnectionWriter::addPool(const yarp::os::Bytes& data)
{
    if (pool != nullptr) {
        if (data.length() + poolIndex > pool->length()) {
            pool = nullptr;
        }
    }
    if (pool == nullptr && data.length() < poolLength) {
        bool add = false;
        if (*target_used < target->size()) {
            yarp::os::ManagedBytes*& bytes = (*target)[*target_used];
            if (bytes->length() < poolLength) {
                delete bytes;
                bytes = new yarp::os::ManagedBytes(poolLength);
            }
            pool = bytes;
            if (pool == nullptr) {
                return false;
            }
        } else {
            pool = new yarp::os::ManagedBytes(poolLength);
            if (pool == nullptr) {
                return false;
            }
            add = true;
        }
        (*target_used)++;
        poolCount++;
        poolIndex = 0;
        if (poolLength < 65536) {
            poolLength *= 2;
        }
        pool->setUsed(0);
        if (add) {
            target->push_back(pool);
        }
    }
    if (pool != nullptr) {
        memcpy(pool->get() + poolIndex, data.get(), data.length());
        poolIndex += data.length();
        pool->setUsed(poolIndex);
        return true;
    }
    return false;
}


void BufferedConnectionWriter::stopPool()
{
    pool = nullptr;
    poolIndex = 0;
    poolLength = initialPoolSize;
    poolCount = 0;
}


void BufferedConnectionWriter::push(const Bytes& data, bool copy)
{
    if (copy) {
        if (addPool(data)) {
            return;
        }
    }
    yarp::os::ManagedBytes* buf = nullptr;
    if (*target_used < target->size()) {
        yarp::os::ManagedBytes*& bytes = (*target)[*target_used];
        if (bytes->isOwner() != copy || bytes->length() < data.length()) {
            delete bytes;
            bytes = new yarp::os::ManagedBytes(data, false);
            if (copy) {
                bytes->copy();
            }
            (*target_used)++;
            return;
        }
        buf = bytes;
        bytes->setUsed(data.length());
    }
    if (buf == nullptr) {
        buf = new yarp::os::ManagedBytes(data, false);
        if (copy) {
            buf->copy();
        }
        target->push_back(buf);
    } else {
        if (copy) {
            buf->copy();
            memmove(buf->get(), data.get(), data.length());
        } else {
            *buf = ManagedBytes(data, false);
        }
    }
    (*target_used)++;
}


bool BufferedConnectionWriter::isTextMode() const
{
    return textMode;
}

bool BufferedConnectionWriter::isBareMode() const
{
    return bareMode;
}


bool BufferedConnectionWriter::convertTextMode()
{
    if (isTextMode()) {
        convertTextModePending = true;
    }
    return true;
}

void BufferedConnectionWriter::declareSizes(int argc, int* argv)
{
    YARP_UNUSED(argc);
    YARP_UNUSED(argv);
    // this method is never called yet, so no point using it yet.
}

void BufferedConnectionWriter::setReplyHandler(PortReader& reader)
{
    this->reader = &reader;
}

namespace {
template <typename T, typename NetT>
inline void appendType(BufferedConnectionWriter* buf, T data)
{
    if (std::is_same<T, NetT>::value) {
        yarp::os::Bytes b(reinterpret_cast<char*>(&data), sizeof(T));
        buf->push(b, true);
    } else {
        NetT i = data;
        yarp::os::Bytes b(reinterpret_cast<char*>(&i), sizeof(T));
        buf->push(b, true);
    }
}
} // namespace

void BufferedConnectionWriter::appendInt8(std::int8_t data)
{
    appendType<std::int8_t, NetInt8>(this, data);
}

void BufferedConnectionWriter::appendInt16(std::int16_t data)
{
    appendType<std::int16_t, NetInt16>(this, data);
}

void BufferedConnectionWriter::appendInt32(std::int32_t data)
{
    appendType<std::int32_t, NetInt32>(this, data);
}

void BufferedConnectionWriter::appendInt64(std::int64_t data)
{
    appendType<std::int64_t, NetInt64>(this, data);
}

void BufferedConnectionWriter::appendFloat32(yarp::conf::float32_t data)
{
    appendType<yarp::conf::float32_t, NetFloat32>(this, data);
}

void BufferedConnectionWriter::appendFloat64(yarp::conf::float64_t data)
{
    appendType<yarp::conf::float64_t, NetFloat64>(this, data);
}

void BufferedConnectionWriter::appendBlock(const char* data, size_t len)
{
    appendBlockCopy(yarp::os::Bytes(const_cast<char*>(data), len));
}

void BufferedConnectionWriter::appendText(const std::string& str, const char terminate)
{
    if (terminate == '\n') {
        appendLine(str);
    } else if (terminate == 0) {
        yarp::os::Bytes b(const_cast<char*>(str.data()), str.length() + 1);
        push(b, true);
    } else {
        std::string s = str;
        s += terminate;
        appendBlockCopy(yarp::os::Bytes(const_cast<char*>(s.c_str()), s.length()));
    }
}

void BufferedConnectionWriter::appendExternalBlock(const char* data, size_t len)
{
    appendBlock(yarp::os::Bytes(const_cast<char*>(data), len));
}

void BufferedConnectionWriter::appendBlock(const yarp::os::Bytes& data)
{
    stopPool();
    push(data, false);
}

void BufferedConnectionWriter::appendBlockCopy(const yarp::os::Bytes& data)
{
    push(data, true);
}

void BufferedConnectionWriter::appendLine(const std::string& data)
{
    yarp::os::Bytes b(const_cast<char*>(data.c_str()), data.length());
    push(b, true);
    const char* eol = "\r\n"; // for windows compatibility
    yarp::os::Bytes beol(const_cast<char*>(eol), 2);
    push(beol, true);
}


size_t BufferedConnectionWriter::length() const
{
    return header_used + lst_used;
}

size_t BufferedConnectionWriter::headerLength() const
{
    return header_used;
}

size_t BufferedConnectionWriter::length(size_t index) const
{
    if (index < header_used) {
        yarp::os::ManagedBytes& b = *(header[index]);
        return b.used();
    }
    yarp::os::ManagedBytes& b = *(lst[index - header.size()]);
    return b.used();
}

const char* BufferedConnectionWriter::data(size_t index) const
{
    if (index < header_used) {
        yarp::os::ManagedBytes& b = *(header[index]);
        return b.get();
    }
    yarp::os::ManagedBytes& b = *(lst[index - header.size()]);
    return b.get();
}

bool BufferedConnectionWriter::write(ConnectionWriter& connection) const
{
    stopWrite();
    size_t i;
    for (i = 0; i < header_used; i++) {
        yarp::os::ManagedBytes& b = *(header[i]);
        connection.appendBlock(b.get(), b.used());
    }
    for (i = 0; i < lst_used; i++) {
        yarp::os::ManagedBytes& b = *(lst[i]);
        connection.appendBlock(b.get(), b.used());
    }
    return !connection.isError();
}

void BufferedConnectionWriter::write(OutputStream& os)
{
    stopWrite();
    for (size_t i = 0; i < header_used; i++) {
        yarp::os::ManagedBytes& b = *(header[i]);
        os.write(b.usedBytes());
    }
    for (size_t i = 0; i < lst_used; i++) {
        yarp::os::ManagedBytes& b = *(lst[i]);
        os.write(b.usedBytes());
    }
    os.flush();
}

bool BufferedConnectionWriter::write(PortReader& obj)
{
    DummyConnector con;
    con.setTextMode(isTextMode());
    if (!write(con.getWriter())) {
        return false;
    }
    return obj.read(con.getReader());
}


size_t BufferedConnectionWriter::dataSize() const
{
    size_t i;
    size_t len = 0;
    for (i = 0; i < header_used; i++) {
        yarp::os::ManagedBytes& b = *(header[i]);
        len += b.usedBytes().length();
    }
    for (i = 0; i < lst_used; i++) {
        yarp::os::ManagedBytes& b = *(lst[i]);
        len += b.usedBytes().length();
    }
    return len;
}

size_t BufferedConnectionWriter::bufferCount() const
{
    return header.size() + lst.size();
}


PortReader* BufferedConnectionWriter::getReplyHandler()
{
    return reader;
}


void BufferedConnectionWriter::addToHeader()
{
    stopPool();
    target = &header;
    target_used = &header_used;
}

yarp::os::Portable* BufferedConnectionWriter::getReference()
{
    return ref;
}

void BufferedConnectionWriter::setReference(yarp::os::Portable* obj)
{
    ref = obj;
}

bool BufferedConnectionWriter::isValid() const
{
    return true;
}

bool BufferedConnectionWriter::isActive() const
{
    return true;
}

bool BufferedConnectionWriter::isError() const
{
    return false; // output errors are of no significance at user level
}

void BufferedConnectionWriter::requestDrop()
{
    shouldDrop = true;
}

bool BufferedConnectionWriter::dropRequested()
{
    return shouldDrop;
}

void BufferedConnectionWriter::startWrite() const
{
}

void BufferedConnectionWriter::stopWrite() const
{
    // convert, last thing, if requested
    applyConvertTextMode();
}

SizedWriter* BufferedConnectionWriter::getBuffer() const
{
    return const_cast<BufferedConnectionWriter*>(this);
}


void BufferedConnectionWriter::setInitialPoolSize(size_t size)
{
    initialPoolSize = size;
}


std::string BufferedConnectionWriter::toString() const
{
    stopWrite();
    size_t total_size = dataSize();
    std::string output(total_size, 0);
    char* dest = const_cast<char*>(output.c_str());
    for (size_t i = 0; i < header_used; i++) {
        const char* data = header[i]->get();
        size_t len = header[i]->used();
        memmove(dest, data, len);
        dest += len;
    }
    for (size_t i = 0; i < lst_used; i++) {
        const char* data = lst[i]->get();
        size_t len = lst[i]->used();
        memmove(dest, data, len);
        dest += len;
    }
    return output;
}

bool BufferedConnectionWriter::applyConvertTextMode() const
{
    return const_cast<BufferedConnectionWriter*>(this)->applyConvertTextMode();
}

bool BufferedConnectionWriter::applyConvertTextMode()
{
    if (convertTextModePending) {
        convertTextModePending = false;

        Bottle b;
        StringOutputStream sos;
        for (size_t i = 0; i < lst_used; i++) {
            yarp::os::ManagedBytes& m = *(lst[i]);
            sos.write(m.usedBytes());
        }
        const std::string& str = sos.str();
        b.fromBinary(str.c_str(), static_cast<int>(str.length()));
        std::string replacement = b.toString() + "\n";
        for (auto& i : lst) {
            delete i;
        }
        lst_used = 0;
        target = &lst;
        lst.clear();
        stopPool();
        Bytes data(const_cast<char*>(replacement.c_str()), replacement.length());
        appendBlockCopy(data);
    }
    return true;
}
