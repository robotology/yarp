/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/ManagedBytes.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>

#include <cstdlib>
#include <cstring>

using namespace yarp::os;

ManagedBytes::ManagedBytes() :
        Portable(),
        b(Bytes(nullptr, 0)),
        owned(false),
        use(0),
        use_set(false)
{
}

ManagedBytes::ManagedBytes(size_t len) :
        Portable(),
        b(Bytes(new char[len], len)),
        owned(true),
        use(0),
        use_set(false)
{
}

ManagedBytes::ManagedBytes(const Bytes& ext, bool owned) :
        Portable(),
        b(ext),
        owned(owned),
        use(0),
        use_set(false)
{
}

ManagedBytes::ManagedBytes(const ManagedBytes& alt) :
        Portable(),
        b(alt.b),
        owned(false),
        use(0),
        use_set(false)
{
    if (alt.owned) {
        copy();
    }
}

void ManagedBytes::moveOwnership(ManagedBytes &other)
{
    b = other.b;
    owned = other.owned;
    use = other.use;
    use_set = other.use_set;
    other.owned = false;
    other.clear();
}

ManagedBytes::ManagedBytes(ManagedBytes&& other) noexcept
{
    moveOwnership(other);
}

ManagedBytes& ManagedBytes::operator=(ManagedBytes&& other) noexcept
{
    if (&other != this) {
        clear();
        moveOwnership(other);
    }
    return *this;
}

const ManagedBytes& ManagedBytes::operator=(const ManagedBytes& alt)
{
    if (&alt != this) {
        clear();
        b = alt.b;
        use = alt.use;
        use_set = alt.use_set;
        owned = false;
        if (alt.owned) {
            copy();
        }
    }
    return *this;
}

ManagedBytes::~ManagedBytes()
{
    clear();
}


void ManagedBytes::allocate(size_t len)
{
    clear();
    char* buf = new char[len];
    b = Bytes(buf, len);
    owned = true;
    use = 0;
    use_set = false;
}

bool ManagedBytes::allocateOnNeed(size_t neededLen, size_t allocateLen)
{
    if (length() < neededLen && allocateLen >= neededLen) {
        char* buf = new char[allocateLen];
        yarp::os::NetworkBase::assertion(buf != nullptr);
        memcpy(buf, get(), length());
        if (owned) {
            delete[] get();
            owned = false;
        }
        b = Bytes(buf, allocateLen);
        owned = true;
        return true;
    }
    return false;
}

void ManagedBytes::copy()
{
    if (!owned) {
        yarp::conf::ssize_t len = length();
        char* buf = new char[len];
        yarp::os::NetworkBase::assertion(buf != nullptr);
        memcpy(buf, get(), len);
        b = Bytes(buf, len);
        owned = true;
    }
}

size_t ManagedBytes::length() const
{
    return b.length();
}

size_t ManagedBytes::used() const
{
    return use_set ? use : length();
}

const char* ManagedBytes::get() const
{
    return b.get();
}

char* ManagedBytes::get()
{
    return b.get();
}

void ManagedBytes::clear()
{
    if (owned) {
        if (get() != nullptr) {
            delete[] get();
        }
        owned = false;
    }
    b = Bytes(nullptr, 0);
    use = 0;
    use_set = false;
}

const Bytes& ManagedBytes::bytes() const
{
    return b;
}

Bytes& ManagedBytes::bytes()
{
    return b;
}

Bytes ManagedBytes::usedBytes()
{
    return {get(), used()};
}

size_t ManagedBytes::setUsed(size_t used)
{
    use_set = true;
    use = used;
    return this->used();
}

size_t ManagedBytes::resetUsed()
{
    use = 0;
    use_set = false;
    return this->used();
}


bool ManagedBytes::read(ConnectionReader& reader)
{
    reader.convertTextMode();
    std::int32_t listTag;
    std::int32_t listLen;
    std::int32_t blobLen;
    listTag = reader.expectInt32();
    listLen = reader.expectInt32();
    blobLen = reader.expectInt32();
    if (listTag != BOTTLE_TAG_LIST + BOTTLE_TAG_BLOB) {
        return false;
    }
    if (listLen != 1) {
        return false;
    }
    allocate(blobLen);
    if (get() == nullptr) {
        return false;
    }
    return reader.expectBlock(get(), length());
}

bool ManagedBytes::write(ConnectionWriter& writer) const
{
    writer.appendInt32(BOTTLE_TAG_LIST + BOTTLE_TAG_BLOB);
    writer.appendInt32(1);
    writer.appendInt32(static_cast<std::int32_t>(length()));
    writer.appendExternalBlock(get(), length());
    writer.convertTextMode();
    return !writer.isError();
}
