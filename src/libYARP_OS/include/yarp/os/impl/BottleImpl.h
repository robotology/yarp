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

#include <yarp/os/Bottle.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/ManagedBytes.h>
#include <yarp/os/Portable.h>
#include <yarp/os/Property.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Vocab.h>

#include <yarp/os/impl/Logger.h>

#include <vector>

namespace yarp {
namespace os {
namespace impl {


/**
 * A single item in a Bottle.  This extends the public yarp::os::Value
 * interface with some implementation-specific details.
 */
class YARP_OS_impl_API Storable : public yarp::os::Value
{
public:
    /**
     * Destructor.
     */
    virtual ~Storable();

    /**
     * Factory method.
     */
    virtual Storable* createStorable() const = 0;

    /**
     * Typed synonym for clone()
     */
    virtual Storable* cloneStorable() const
    {
        Storable* item = createStorable();
        yAssert(item != nullptr);
        item->copy(*this);
        return item;
    }

    /**
     * Become a copy of the passed item.
     */
    virtual void copy(const Storable& alt) = 0;

    bool operator==(const yarp::os::Value& alt) const;

    virtual yarp::os::Value* create() const override { return createStorable(); }
    virtual yarp::os::Value* clone() const override { return cloneStorable(); }

    static Storable* createByCode(std::int32_t id);


    virtual bool read(ConnectionReader& connection) override;
    virtual bool write(ConnectionWriter& connection) override;

    virtual bool readRaw(ConnectionReader& connection) = 0;
    virtual bool writeRaw(ConnectionWriter& connection) = 0;

    virtual bool isBool() const override { return false; }
    virtual bool asBool() const override { return false; }

    virtual bool isInt8() const override { return false; }
    virtual std::int8_t asInt8() const override { return 0; }

    virtual bool isInt16() const override { return false; }
    virtual std::int16_t asInt16() const override { return 0; }

    virtual bool isInt32() const override { return false; }
    virtual std::int32_t asInt32() const override { return 0; }

    virtual bool isInt64() const override { return false; }
    virtual std::int64_t asInt64() const override { return 0; }

    virtual bool isFloat32() const override { return false; }
    virtual yarp::conf::float32_t asFloat32() const override { return 0.0f; }

    virtual bool isFloat64() const override { return false; }
    virtual yarp::conf::float64_t asFloat64() const override { return 0.0; }

    virtual bool isString() const override { return false; }
    virtual std::string asString() const override { return std::string(""); }

    virtual bool isList() const override { return false; }
    virtual yarp::os::Bottle* asList() const override { return nullptr; }

    virtual bool isDict() const override { return false; }
    virtual yarp::os::Property* asDict() const override { return nullptr; }

    virtual bool isVocab() const override { return false; }
    virtual std::int32_t asVocab() const override { return 0; }

    virtual bool isBlob() const override { return false; }
    virtual const char* asBlob() const override { return static_cast<const char*>(nullptr); }
    virtual size_t asBlobLength() const override { return 0; }

    virtual bool isNull() const override { return false; }


    virtual Searchable* asSearchable() const override
    {
        if (isDict()) {
            return asDict();
        }
        return asList();
    }
    using yarp::os::Searchable::check;
    virtual bool check(const std::string& key) const override;

    virtual yarp::os::Value& find(const std::string& key) const override;
    virtual yarp::os::Bottle& findGroup(const std::string& key) const override;


    /**
     * Initialize from a string representation, assuming that any
     * syntax around this representation such as braces or
     * parentheses has already been consumed.
     */
    virtual void fromString(const std::string& src) = 0;

    /**
     * Initialize from a string representation.  This should consume
     * any syntax around that representation such as braces or
     * parentheses.
     */
    virtual void fromStringNested(const std::string& src) { fromString(src); }
    virtual std::string toString() const override = 0;
    /**
     * Create string representation, including any syntax that should
     * wrap it such as braces or parentheses.
     */
    virtual std::string toStringNested() const { return toString(); }

    /**
     * Return a code describing this item, used in serializing bottles.
     */
    virtual std::int32_t subCode() const { return 0; }

    virtual bool isLeaf() const override { return true; }
};


/**
 * An empty item.
 */
class YARP_OS_impl_API StoreNull : public Storable
{
public:
    StoreNull() {}
    virtual Storable* createStorable() const override { return new StoreNull(); }
    virtual void copy(const Storable& alt) override { YARP_UNUSED(alt); }

    virtual std::string toString() const override { return ""; }
    virtual void fromString(const std::string& src) override { YARP_UNUSED(src); }

    virtual std::int32_t getCode() const override { return -1; }

    virtual bool readRaw(ConnectionReader& connection) override { YARP_UNUSED(connection); return false; }
    virtual bool writeRaw(ConnectionWriter& connection) override { YARP_UNUSED(connection); return false; }

    virtual bool isNull() const override { return true; }
};


/**
 * A 8-bit integer item.
 */
class YARP_OS_impl_API StoreInt8 : public Storable
{
private:
    std::int8_t x;

public:
    StoreInt8() : x(0) {}
    StoreInt8(std::int8_t x) : x(x) {}
    virtual Storable* createStorable() const override { return new StoreInt8(0); }
    virtual void copy(const Storable& alt) override { x = alt.asInt8(); }

    virtual std::string toString() const override;
    virtual void fromString(const std::string& src) override;

    static const std::int32_t code;
    virtual std::int32_t getCode() const override { return code; }

    virtual bool readRaw(ConnectionReader& reader) override;
    virtual bool writeRaw(ConnectionWriter& writer) override;

    virtual bool isInt8() const override { return true; }
    virtual bool asBool() const override { return x != 0; }
    virtual std::int8_t asInt8() const override { return x; }
    virtual std::int16_t asInt16() const override { return x; }
    virtual std::int32_t asInt32() const override { return x; }
    virtual std::int64_t asInt64() const override { return x; }
    virtual yarp::conf::float32_t asFloat32() const override { return x; }
    virtual yarp::conf::float64_t asFloat64() const override { return x; }
    virtual std::int32_t asVocab() const override { return x; }
};



/**
 * A 16-bit integer item.
 */
class YARP_OS_impl_API StoreInt16 : public Storable
{
private:
    std::int16_t x;

public:
    StoreInt16() : x(0) {}
    StoreInt16(std::int16_t x) : x(x) {}
    virtual Storable* createStorable() const override { return new StoreInt16(0); }
    virtual void copy(const Storable& alt) override { x = alt.asInt16(); }

    virtual std::string toString() const override;
    virtual void fromString(const std::string& src) override;

    static const std::int32_t code;
    virtual std::int32_t getCode() const override { return code; }

    virtual bool readRaw(ConnectionReader& reader) override;
    virtual bool writeRaw(ConnectionWriter& writer) override;

    virtual bool isInt16() const override { return true; }
    virtual bool asBool() const override { return x != 0; }
    virtual std::int8_t asInt8() const override { return static_cast<std::int8_t>(x); }
    virtual std::int16_t asInt16() const override { return x; }
    virtual std::int32_t asInt32() const override { return x; }
    virtual std::int64_t asInt64() const override { return x; }
    virtual yarp::conf::float32_t asFloat32() const override { return x; }
    virtual yarp::conf::float64_t asFloat64() const override { return x; }
    virtual std::int32_t asVocab() const override { return x; }
};


/**
 * A 32-bit integer item.
 */
class YARP_OS_impl_API StoreInt32 : public Storable
{
private:
    std::int32_t x;

public:
    StoreInt32() : x(0) {}
    StoreInt32(std::int32_t x) : x(x) {}
    virtual Storable* createStorable() const override { return new StoreInt32(0); }
    virtual void copy(const Storable& alt) override { x = alt.asInt32(); }

    virtual std::string toString() const override;
    virtual void fromString(const std::string& src) override;

    static const std::int32_t code;
    virtual std::int32_t getCode() const override { return code; }

    virtual bool readRaw(ConnectionReader& reader) override;
    virtual bool writeRaw(ConnectionWriter& writer) override;

    virtual std::int8_t asInt8() const override { return static_cast<std::int8_t>(x); }
    virtual bool asBool() const override { return x != 0; }
    virtual std::int16_t asInt16() const override { return static_cast<std::int16_t>(x); }
    virtual bool isInt32() const override { return true; }
    virtual std::int32_t asInt32() const override { return x; }
    virtual std::int64_t asInt64() const override { return x; }
    virtual yarp::conf::float32_t asFloat32() const override { return x; }
    virtual yarp::conf::float64_t asFloat64() const override { return x; }
    virtual std::int32_t asVocab() const override { return x; }
};

/**
 * A 64-bit integer item.
 */
class YARP_OS_impl_API StoreInt64 : public Storable
{
private:
    std::int64_t x;

public:
    StoreInt64() : x(0) {}
    StoreInt64(std::int64_t x) : x(x) {}
    virtual Storable* createStorable() const override { return new StoreInt64(0); }
    virtual void copy(const Storable& alt) override { x = alt.asInt64(); }

    virtual std::string toString() const override;
    virtual void fromString(const std::string& src) override;

    static const std::int32_t code;
    virtual std::int32_t getCode() const override { return code; }

    virtual bool readRaw(ConnectionReader& reader) override;
    virtual bool writeRaw(ConnectionWriter& writer) override;

    virtual bool isInt64() const override { return true; }
    virtual bool asBool() const override { return x != 0; }
    virtual std::int8_t asInt8() const override { return static_cast<std::int8_t>(x); }
    virtual std::int16_t asInt16() const override { return static_cast<std::int16_t>(x); }
    virtual std::int32_t asInt32() const override { return static_cast<std::int32_t>(x); }
    virtual std::int64_t asInt64() const override { return x; }
    virtual yarp::conf::float32_t asFloat32() const override { return static_cast<yarp::conf::float32_t>(x); }
    virtual yarp::conf::float64_t asFloat64() const override { return static_cast<yarp::conf::float64_t>(x); }
    virtual std::int32_t asVocab() const override { return (std::int32_t)x; }
};

/**
 * A 32-bit floating point number item.
 */
class YARP_OS_impl_API StoreFloat32 : public Storable
{
private:
    yarp::conf::float32_t x;

public:
    StoreFloat32() : x(0) {}
    StoreFloat32(yarp::conf::float32_t x) : x(x) {}
    virtual Storable* createStorable() const override { return new StoreFloat32(0); }
    virtual void copy(const Storable& alt) override { x = alt.asFloat32(); }

    virtual std::string toString() const override;
    virtual void fromString(const std::string& src) override;

    static const std::int32_t code;
    virtual std::int32_t getCode() const override { return code; }

    virtual bool readRaw(ConnectionReader& reader) override;
    virtual bool writeRaw(ConnectionWriter& writer) override;

    virtual bool isFloat32() const override { return true; }
    virtual std::int8_t asInt8() const override { return static_cast<std::int8_t>(x); }
    virtual std::int16_t asInt16() const override { return static_cast<std::int16_t>(x); }
    virtual std::int32_t asInt32() const override { return static_cast<std::int32_t>(x); }
    virtual std::int64_t asInt64() const override { return static_cast<std::int64_t>(x); }
    virtual yarp::conf::float32_t asFloat32() const override { return x; }
    virtual yarp::conf::float64_t asFloat64() const override { return x; }
};

/**
 * A 64-bit floating point number item.
 */
class YARP_OS_impl_API StoreFloat64 : public Storable
{
private:
    yarp::conf::float64_t x;

public:
    StoreFloat64() : x(0) {}
    StoreFloat64(yarp::conf::float64_t x) : x(x) {}
    virtual Storable* createStorable() const override { return new StoreFloat64(0); }
    virtual void copy(const Storable& alt) override { x = alt.asFloat64(); }

    virtual std::string toString() const override;
    virtual void fromString(const std::string& src) override;

    static const std::int32_t code;
    virtual std::int32_t getCode() const override { return code; }

    virtual bool readRaw(ConnectionReader& reader) override;
    virtual bool writeRaw(ConnectionWriter& writer) override;

    virtual bool isFloat64() const override { return true; }
    virtual std::int8_t asInt8() const override { return static_cast<std::int8_t>(x); }
    virtual std::int16_t asInt16() const override { return static_cast<std::int16_t>(x); }
    virtual std::int32_t asInt32() const override { return static_cast<std::int32_t>(x); }
    virtual std::int64_t asInt64() const override { return static_cast<std::int64_t>(x); }
    virtual yarp::conf::float32_t asFloat32() const override { return static_cast<yarp::conf::float32_t>(x); }
    virtual yarp::conf::float64_t asFloat64() const override { return x; }
};

/**
 * A vocabulary item.
 */
class YARP_OS_impl_API StoreVocab : public Storable
{
    std::int32_t x;

public:
    StoreVocab() : x(0) {}
    StoreVocab(std::int32_t x) : x(x) {}
    virtual Storable* createStorable() const override { return new StoreVocab(0); }
    virtual void copy(const Storable& alt) override { x = alt.asVocab(); }

    virtual std::string toString() const override;
    virtual void fromString(const std::string& src) override;
    virtual std::string toStringNested() const override;
    virtual void fromStringNested(const std::string& src) override;

    static const std::int32_t code;
    virtual std::int32_t getCode() const override { return code; }

    virtual bool readRaw(ConnectionReader& reader) override;
    virtual bool writeRaw(ConnectionWriter& writer) override;

    virtual bool isBool() const override { return (x == 0 || x == '1'); }
    virtual bool asBool() const override { return x != 0; }

    virtual std::int32_t asInt32() const override { return x; }
    virtual std::int64_t asInt64() const override { return x; }
    virtual yarp::conf::float32_t asFloat32() const override { return x; }
    virtual yarp::conf::float64_t asFloat64() const override { return x; }

    virtual bool isVocab() const override { return true; }
    virtual std::int32_t asVocab() const override { return x; }

    virtual std::string asString() const override { return toString(); }
};

/**
 * A string item.
 */
class YARP_OS_impl_API StoreString : public Storable
{
private:
    std::string x;

public:
    StoreString() { x = ""; }
    StoreString(const std::string& x) { this->x = x; }
    virtual Storable* createStorable() const override { return new StoreString(std::string("")); }
    virtual void copy(const Storable& alt) override { x = alt.asString(); }

    virtual std::string toString() const override;
    virtual void fromString(const std::string& src) override;
    virtual std::string toStringNested() const override;
    virtual void fromStringNested(const std::string& src) override;

    static const std::int32_t code;
    virtual std::int32_t getCode() const override { return code; }

    virtual bool readRaw(ConnectionReader& reader) override;
    virtual bool writeRaw(ConnectionWriter& writer) override;

    virtual bool isString() const override { return true; }
    virtual std::string asString() const override { return x; }

    virtual std::int32_t asVocab() const override { return yarp::os::Vocab::encode(x.c_str()); }
};

/**
 * A binary blob item.
 */
class YARP_OS_impl_API StoreBlob : public Storable
{
private:
    std::string x;

public:
    StoreBlob() { x = ""; }
    StoreBlob(const std::string& x) { this->x = x; }
    virtual Storable* createStorable() const override { return new StoreBlob(std::string("")); }
    virtual void copy(const Storable& alt) override
    {
        if (alt.isBlob()) {
            std::string tmp((char*)alt.asBlob(), alt.asBlobLength());
            x = tmp;
        } else {
            x = std::string();
        }
    }

    virtual std::string toString() const override;
    virtual void fromString(const std::string& src) override;
    virtual std::string toStringNested() const override;
    virtual void fromStringNested(const std::string& src) override;

    static const std::int32_t code;
    virtual std::int32_t getCode() const override { return code; }

    virtual bool readRaw(ConnectionReader& reader) override;
    virtual bool writeRaw(ConnectionWriter& writer) override;

    virtual bool isBlob() const override { return true; }
    virtual const char* asBlob() const override { return x.c_str(); }
    virtual size_t asBlobLength() const override { return x.length(); }
};


/**
 * A nested list of items.
 */
class YARP_OS_impl_API StoreList : public Storable
{
private:
    yarp::os::Bottle content;

public:
    StoreList() {}
    virtual Storable* createStorable() const override { return new StoreList(); }
    virtual void copy(const Storable& alt) override { content = *(alt.asList()); }

    yarp::os::Bottle& internal() { return content; }

    virtual std::string toString() const override;
    virtual void fromString(const std::string& src) override;
    virtual std::string toStringNested() const override;
    virtual void fromStringNested(const std::string& src) override;

    static const std::int32_t code;
    virtual std::int32_t getCode() const override { return code + subCode(); }

    virtual bool readRaw(ConnectionReader& reader) override;
    virtual bool writeRaw(ConnectionWriter& writer) override;

    virtual bool isList() const override { return true; }
    virtual yarp::os::Bottle* asList() const override
    {
        return (yarp::os::Bottle*)(&content);
    }

    virtual std::int32_t subCode() const override;

    virtual yarp::os::Value& find(const std::string& key) const override
    {
        return content.find(key);
    }

    virtual yarp::os::Bottle& findGroup(const std::string& key) const override
    {
        return content.findGroup(key);
    }
};


/**
 * Key/value pairs
 */
class YARP_OS_impl_API StoreDict : public Storable
{
private:
    yarp::os::Property content;

public:
    StoreDict() {}
    virtual Storable* createStorable() const override { return new StoreDict(); }
    virtual void copy(const Storable& alt) override { content = *(alt.asDict()); }

    yarp::os::Property& internal() { return content; }

    virtual std::string toString() const override;
    virtual void fromString(const std::string& src) override;
    virtual std::string toStringNested() const override;
    virtual void fromStringNested(const std::string& src) override;

    static const std::int32_t code;
    virtual std::int32_t getCode() const override { return code; }

    virtual bool readRaw(ConnectionReader& reader) override;
    virtual bool writeRaw(ConnectionWriter& writer) override;

    virtual bool isDict() const override { return true; }
    virtual yarp::os::Property* asDict() const override { return const_cast<yarp::os::Property*>(&content); }

    virtual yarp::os::Value& find(const std::string& key) const override
    {
        return content.find(key);
    }

    virtual yarp::os::Bottle& findGroup(const std::string& key) const override
    {
        return content.findGroup(key);
    }
};


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
        if (!storeNull) {
            storeNull = new StoreNull;
        }
        return *storeNull;
    }

    // check if a piece of text is a completed bottle
    static bool isComplete(const char* txt);

    void hasChanged() { dirty = true; }
    static void fini()
    {
        if (storeNull) {
            delete storeNull;
            storeNull = nullptr;
        }
    }

    bool checkIndex(int index) const;

    bool invalid;
    bool ro;

    void edit();

    Value& findGroupBit(const std::string& key) const;
    Value& findBit(const std::string& key) const;

private:
    static StoreNull* storeNull;

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
