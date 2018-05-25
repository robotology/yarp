/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * Copyright (C) 2006, 2008 Arjan Gijsberts
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_BOTTLEIMPL_H
#define YARP_OS_IMPL_BOTTLEIMPL_H

#include <yarp/os/impl/Storable.h>
#include <vector>

namespace yarp {
namespace os {

class Bottle;
class Property;
class ConnectionReader;
class ConnectionWriter;

namespace impl {


/**
 * A flexible data format for holding a bunch of numbers and strings.
 * Handy to use until you work out how to make your own more
 * efficient formats for transmission.
 */
class YARP_OS_impl_API BottleImpl
{
public:
    BottleImpl();
    BottleImpl(Searchable* parent);
    virtual ~BottleImpl();

    Searchable* const parent;

    bool isInt8(int index);
    bool isInt16(int index);
    bool isInt32(int index);
    bool isInt64(int index);
    bool isFloat32(int index);
    bool isFloat64(int index);
    bool isString(int index);
    bool isList(int index);

    Storable* pop();

    Storable& get(int index) const;

    void addInt8(std::int8_t x) { add(new StoreInt8(x)); }
    void addInt16(std::int16_t x) { add(new StoreInt16(x)); }
    void addInt32(std::int32_t x) { add(new StoreInt32(x)); }
    void addInt64(std::int64_t x) { add(new StoreInt64(x)); }
    void addFloat32(yarp::conf::float32_t x) { add(new StoreFloat32(x)); }
    void addFloat64(yarp::conf::float64_t x) { add(new StoreFloat64(x)); }
    void addVocab(std::int32_t x) { add(new StoreVocab(x)); }
    void addString(const std::string& text) { add(new StoreString(text)); }

    yarp::os::Bottle& addList();

    yarp::os::Property& addDict();

    void clear();

    void fromString(const std::string& line);
    std::string toString();
    size_t size() const;

    bool read(ConnectionReader& reader);
    bool write(ConnectionWriter& writer);

    void onCommencement();

    const char* getBytes();
    size_t byteCount();

    void copyRange(const BottleImpl* alt, int first = 0, int len = -1);

    bool fromBytes(const yarp::os::Bytes& data);
    void toBytes(const yarp::os::Bytes& data);

    bool fromBytes(yarp::os::ConnectionReader& reader);

    void fromBinary(const char* text, int len);

    void specialize(std::int32_t subCode);
    int getSpecialization();
    void setNested(bool nested);

    std::int32_t subCode();

    void addBit(yarp::os::Value* bit)
    {
        // all Values are Storables -- important invariant!
        add((Storable*)(bit));
    }

    void addBit(const yarp::os::Value& bit)
    {
        // all Values are Storables -- important invariant!
        if (!bit.isNull()) {
            add((Storable*)(bit.clone()));
        }
    }

    yarp::os::Value& addBit(const char* str)
    {
        size_t len = size();
        std::string x(str);
        smartAdd(x);
        if (size() > len) {
            return get((int)size() - 1);
        }
        return get(-1);
    }

    static StoreNull& getNull()
    {
        static StoreNull storeNull;
        return storeNull;
    }

    // check if a piece of text is a completed bottle
    static bool isComplete(const char* txt);

    void hasChanged() { dirty = true; }

    bool checkIndex(int index) const;

    bool invalid;
    bool ro;

    void edit();

    Value& findGroupBit(const std::string& key) const;
    Value& findBit(const std::string& key) const;

private:
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::vector<Storable*>) content;
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::vector<char>) data;
    int speciality;
    bool nested;
    bool dirty;

    void add(Storable* s);
    void smartAdd(const std::string& str);

    void synch();
};

} // namespace impl
} // namespace os
} // namespace yarp


#endif // YARP_OS_IMPL_BOTTLEIMPL_H
