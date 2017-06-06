/*
 * Copyright (C) 2006, 2008 RobotCub Consortium, Arjan Gijsberts
 * Authors: Paul Fitzpatrick, Arjan Gijsberts
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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
#include <yarp/os/impl/PlatformVector.h>

namespace yarp {
    namespace os {
        namespace impl {
            class BottleImpl;
            class Storable;
            class StoreNull;
            class StoreInt;
            class StoreInt64;
            class StoreBool;
            class StoreVocab;
            class StoreDouble;
            class StoreString;
            class StoreBlob;
            class StoreList;
            class StoreDict;
        }
    }
}


/**
 * A single item in a Bottle.  This extends the public yarp::os::Value
 * interface with some implementation-specific details.
 */
class YARP_OS_impl_API yarp::os::impl::Storable : public yarp::os::Value
{
public:
    virtual bool isBool() const override { return false; }
    virtual bool isInt() const override { return false; }
    virtual bool isInt64() const override { return false; }
    virtual bool isString() const override { return false; }
    virtual bool isDouble() const override { return false; }
    virtual bool isList() const override { return false; }
    virtual bool isDict() const override { return false; }
    virtual bool isVocab() const override { return false; }
    virtual bool isBlob() const override { return false; }
    virtual bool isNull() const override { return false; }
    virtual bool asBool() const override { return false; }
    virtual int asInt() const override { return 0; }
    virtual YARP_INT64 asInt64() const override { return 0; }
    virtual int asVocab() const override { return 0; }
    virtual double asDouble() const override { return 0; }
    virtual yarp::os::ConstString asString() const override { return ""; }
    virtual Searchable* asSearchable() const override
    {
        if (isDict()) {
            return asDict();
        }
        return asList();
    }
    virtual yarp::os::Bottle* asList() const override { return YARP_NULLPTR; }
    virtual yarp::os::Property* asDict() const override { return YARP_NULLPTR; }
    virtual const char* asBlob() const override
    {
        return static_cast<const char*>(YARP_NULLPTR);
    }
    virtual size_t asBlobLength() const override { return 0; }
    virtual bool read(ConnectionReader& connection) override;
    virtual bool write(ConnectionWriter& connection) override;

    virtual bool readRaw(ConnectionReader& connection) = 0;
    virtual bool writeRaw(ConnectionWriter& connection) = 0;

    using yarp::os::Searchable::check;
    virtual bool check(const yarp::os::ConstString& key) const override;

    virtual yarp::os::Value& find(const yarp::os::ConstString& key) const override;
    virtual yarp::os::Bottle& findGroup(const yarp::os::ConstString& key) const override;

    bool operator==(const yarp::os::Value& alt) const;

    virtual yarp::os::Value* create() const override { return createStorable(); }
    virtual yarp::os::Value* clone() const override { return cloneStorable(); }
    /**
     * Destructor.
     */
    virtual ~Storable();

    /**
     * Initialize from a string representation, assuming that any
     * syntax around this representation such as braces or
     * parentheses has already been consumed.
     */
    virtual void fromString(const ConstString& src) = 0;

    /**
     * Initialize from a string representation.  This should consume
     * any syntax around that representation such as braces or
     * parentheses.
     */
    virtual void fromStringNested(const ConstString& src) { fromString(src); }
    virtual yarp::os::ConstString toString() const override = 0;
    /**
     * Create string representation, including any syntax that should
     * wrap it such as braces or parentheses.
     */
    virtual ConstString toStringNested() const { return toString(); }
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
        yAssert(item != YARP_NULLPTR);
        item->copy(*this);
        return item;
    }

    /**
     * Become a copy of the passed item.
     */
    virtual void copy(const Storable& alt) = 0;

    /**
     * Return a code describing this item, used in serializing bottles.
     */
    virtual int subCode() const { return 0; }
    virtual bool isLeaf() const override { return true; }
    static Storable* createByCode(int id);
};


/**
 * An empty item.
 */
class YARP_OS_impl_API yarp::os::impl::StoreNull : public Storable
{
public:
    StoreNull() {}
    virtual ConstString toString() const override { return ""; }
    virtual void fromString(const ConstString& src) override {}
    virtual int getCode() const override { return -1; }
    virtual bool readRaw(ConnectionReader& connection) override { return false; }
    virtual bool writeRaw(ConnectionWriter& connection) override { return false; }
    virtual Storable* createStorable() const override { return new StoreNull(); }
    virtual bool isNull() const override { return true; }
    virtual void copy(const Storable& alt) override {}
};


/**
 * An integer item.
 */
class YARP_OS_impl_API yarp::os::impl::StoreInt : public Storable
{
private:
    int x;

public:
    StoreInt() { x = 0; }
    StoreInt(int x) { this->x = x; }
    virtual ConstString toString() const override;
    virtual void fromString(const ConstString& src) override;
    virtual int getCode() const override { return code; }
    virtual bool readRaw(ConnectionReader& reader) override;
    virtual bool writeRaw(ConnectionWriter& writer) override;
    virtual Storable* createStorable() const override { return new StoreInt(0); }
    virtual bool asBool() const override { return x; }
    virtual int asInt() const override { return x; }
    virtual YARP_INT64 asInt64() const override { return x; }
    virtual int asVocab() const override { return x; }
    virtual double asDouble() const override { return x; }
    virtual bool isInt() const override { return true; }
    virtual bool isInt64() const override { return true; }
    static const int code;
    virtual void copy(const Storable& alt) override { x = alt.asInt(); }
};

/**
 * A 64-bit integer item.
 */
class YARP_OS_impl_API yarp::os::impl::StoreInt64 : public Storable
{
private:
    YARP_INT64 x;

public:
    StoreInt64() { x = 0; }
    StoreInt64(const YARP_INT64& x) { this->x = x; }
    virtual ConstString toString() const override;
    virtual void fromString(const ConstString& src) override;
    virtual int getCode() const override { return code; }
    virtual bool readRaw(ConnectionReader& reader) override;
    virtual bool writeRaw(ConnectionWriter& writer) override;
    virtual Storable* createStorable() const override { return new StoreInt64(0); }
    virtual int asInt() const override { return (int)x; }
    virtual YARP_INT64 asInt64() const override { return x; }
    virtual int asVocab() const override { return (int)x; }
    virtual double asDouble() const override { return (double)x; }
    virtual bool isInt() const override { return false; }
    virtual bool isInt64() const override { return true; }
    static const int code;
    virtual void copy(const Storable& alt) override { x = alt.asInt64(); }
};

/**
 * A vocabulary item.
 */
class YARP_OS_impl_API yarp::os::impl::StoreVocab : public Storable
{
private:
    int x;

public:
    StoreVocab() { x = 0; }
    StoreVocab(int x) { this->x = x; }
    virtual ConstString toString() const override;
    virtual void fromString(const ConstString& src) override;
    virtual ConstString toStringNested() const override;
    virtual void fromStringNested(const ConstString& src) override;
    virtual int getCode() const override { return code; }
    virtual bool readRaw(ConnectionReader& reader) override;
    virtual bool writeRaw(ConnectionWriter& writer) override;
    virtual Storable* createStorable() const override { return new StoreVocab(0); }
    virtual bool asBool() const override { return x != 0; }
    virtual int asInt() const override { return x; }
    virtual YARP_INT64 asInt64() const override { return x; }
    virtual int asVocab() const override { return x; }
    virtual double asDouble() const override { return x; }
    virtual bool isVocab() const override { return true; }
    virtual bool isBool() const override { return (x == 0 || x == '1'); }
    static const int code;
    virtual void copy(const Storable& alt) override { x = alt.asVocab(); }
    virtual ConstString asString() const override { return toString(); }
};

/**
 * A string item.
 */
class YARP_OS_impl_API yarp::os::impl::StoreString : public Storable
{
private:
    ConstString x;

public:
    StoreString() { x = ""; }
    StoreString(const ConstString& x) { this->x = x; }
    virtual ConstString toString() const override;
    virtual void fromString(const ConstString& src) override;
    virtual ConstString toStringNested() const override;
    virtual void fromStringNested(const ConstString& src) override;
    virtual int getCode() const override { return code; }
    virtual bool readRaw(ConnectionReader& reader) override;
    virtual bool writeRaw(ConnectionWriter& writer) override;
    virtual Storable* createStorable() const override
    {
        return new StoreString(ConstString(""));
    }
    virtual ConstString asString() const override { return x; }
    virtual int asVocab() const override { return yarp::os::Vocab::encode(x.c_str()); }
    virtual bool isString() const override { return true; }
    static const int code;
    virtual void copy(const Storable& alt) override
    {
        // yarp::os::ConstString y =
        x = alt.asString();
        // ConstString tmp(y.c_str(), y.length());
        // x = tmp;
    }
};

/**
 * A binary blob item.
 */
class YARP_OS_impl_API yarp::os::impl::StoreBlob : public Storable
{
private:
    ConstString x;

public:
    StoreBlob() { x = ""; }
    StoreBlob(const ConstString& x) { this->x = x; }
    virtual ConstString toString() const override;
    virtual void fromString(const ConstString& src) override;
    virtual ConstString toStringNested() const override;
    virtual void fromStringNested(const ConstString& src) override;
    virtual int getCode() const override { return code; }
    virtual bool readRaw(ConnectionReader& reader) override;
    virtual bool writeRaw(ConnectionWriter& writer) override;
    virtual Storable* createStorable() const override
    {
        return new StoreBlob(ConstString(""));
    }
    virtual bool isBlob() const override { return true; }
    virtual const char* asBlob() const override { return x.c_str(); }
    virtual size_t asBlobLength() const override { return x.length(); }
    static const int code;
    virtual void copy(const Storable& alt) override
    {
        if (alt.isBlob()) {
            ConstString tmp((char*)alt.asBlob(), alt.asBlobLength());
            x = tmp;
        } else {
            x = ConstString();
        }
    }
};

/**
 * A floating point number item.
 */
class YARP_OS_impl_API yarp::os::impl::StoreDouble : public Storable
{
private:
    double x;

public:
    StoreDouble() { x = 0; }
    StoreDouble(double x) { this->x = x; }
    virtual ConstString toString() const override;
    virtual void fromString(const ConstString& src) override;
    virtual int getCode() const override { return code; }
    virtual bool readRaw(ConnectionReader& reader) override;
    virtual bool writeRaw(ConnectionWriter& writer) override;
    virtual Storable* createStorable() const override { return new StoreDouble(0); }
    virtual int asInt() const override { return (int)x; }
    virtual YARP_INT64 asInt64() const override { return (YARP_INT64)x; }
    virtual double asDouble() const override { return x; }
    virtual bool isDouble() const override { return true; }
    static const int code;
    virtual void copy(const Storable& alt) override { x = alt.asDouble(); }
};


/**
 * A nested list of items.
 */
class YARP_OS_impl_API yarp::os::impl::StoreList : public Storable
{
private:
    yarp::os::Bottle content;

public:
    StoreList() {}
    yarp::os::Bottle& internal() { return content; }
    virtual ConstString toString() const override;
    virtual void fromString(const ConstString& src) override;
    virtual ConstString toStringNested() const override;
    virtual void fromStringNested(const ConstString& src) override;
    virtual int getCode() const override { return code + subCode(); }
    virtual bool readRaw(ConnectionReader& reader) override;
    virtual bool writeRaw(ConnectionWriter& writer) override;
    virtual Storable* createStorable() const override { return new StoreList(); }
    virtual bool isList() const override { return true; }
    virtual yarp::os::Bottle* asList() const override
    {
        return (yarp::os::Bottle*)(&content);
    }
    static const int code;
    virtual int subCode() const override;

    virtual yarp::os::Value& find(const yarp::os::ConstString& key) const override
    {
        return content.find(key);
    }

    virtual yarp::os::Bottle& findGroup(const yarp::os::ConstString& key) const override
    {
        return content.findGroup(key);
    }
    virtual void copy(const Storable& alt) override { content = *(alt.asList()); }
};


/**
 * Key/value pairs
 */
class YARP_OS_impl_API yarp::os::impl::StoreDict : public Storable
{
private:
    yarp::os::Property content;

public:
    StoreDict() {}
    yarp::os::Property& internal() { return content; }
    virtual ConstString toString() const override;
    virtual void fromString(const ConstString& src) override;
    virtual ConstString toStringNested() const override;
    virtual void fromStringNested(const ConstString& src) override;
    virtual int getCode() const override { return code; }
    virtual bool readRaw(ConnectionReader& reader) override;
    virtual bool writeRaw(ConnectionWriter& writer) override;
    virtual Storable* createStorable() const override { return new StoreDict(); }
    virtual bool isDict() const override { return true; }
    virtual yarp::os::Property* asDict() const override
    {
        return (yarp::os::Property*)(&content);
    }
    static const int code;

    virtual yarp::os::Value& find(const yarp::os::ConstString& key) const override
    {
        return content.find(key);
    }

    virtual yarp::os::Bottle& findGroup(const yarp::os::ConstString& key) const override
    {
        return content.findGroup(key);
    }
    virtual void copy(const Storable& alt) override { content = *(alt.asDict()); }
};


/**
 * A flexible data format for holding a bunch of numbers and strings.
 * Handy to use until you work out how to make your own more
 * efficient formats for transmission.
 */
class YARP_OS_impl_API yarp::os::impl::BottleImpl : public yarp::os::Portable
{
public:
    BottleImpl();
    BottleImpl(Searchable* parent);
    virtual ~BottleImpl();

    Searchable* const parent;

    bool isInt(int index);
    bool isString(int index);
    bool isDouble(int index);
    bool isList(int index);

    Storable* pop();

    int getInt(int index);
    yarp::os::ConstString getString(int index);
    double getDouble(int index);

    Storable& get(int index) const;

    yarp::os::Bottle* getList(int index);

    void addInt(int x) { add(new StoreInt(x)); }
    void addInt64(const YARP_INT64& x) { add(new StoreInt64(x)); }
    void addVocab(int x) { add(new StoreVocab(x)); }
    void addDouble(double x) { add(new StoreDouble(x)); }
    void addString(const yarp::os::ConstString& text)
    {
        add(new StoreString(text));
    }

    yarp::os::Bottle& addList();

    yarp::os::Property& addDict();

    void clear();

    void fromString(const ConstString& line);
    ConstString toString();
    size_t size() const;

    virtual bool read(ConnectionReader& reader) override;
    virtual bool write(ConnectionWriter& writer) override;

    virtual void onCommencement() override;

    const char* getBytes();
    size_t byteCount();

    void copyRange(const BottleImpl* alt, int first = 0, int len = -1);

    bool fromBytes(const yarp::os::Bytes& data);
    void toBytes(const yarp::os::Bytes& data);

    bool fromBytes(yarp::os::ConnectionReader& reader);

    void fromBinary(const char* text, int len);

    void specialize(int subCode);
    int getSpecialization();
    void setNested(bool nested);

    int subCode();

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
        ConstString x(str);
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
            storeNull = YARP_NULLPTR;
        }
    }

    bool invalid;
    bool ro;

    void edit();

    Value& findGroupBit(const ConstString& key) const;
    Value& findBit(const ConstString& key) const;

private:
    static StoreNull* storeNull;

    PlatformVector<Storable*> content;
    PlatformVector<char> data;
    int speciality;
    bool nested;
    bool dirty;

    void add(Storable* s);
    void smartAdd(const ConstString& str);

    void synch();
};


#endif // YARP_OS_IMPL_BOTTLEIMPL_H
