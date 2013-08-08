// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006, 2008 RobotCub Consortium, Arjan Gijsberts
 * Authors: Paul Fitzpatrick, Arjan Gijsberts
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_BOTTLEIMPL_
#define _YARP2_BOTTLEIMPL_

#include <yarp/os/impl/String.h>
#include <yarp/os/ManagedBytes.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/Portable.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/Semaphore.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/Property.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/PlatformVector.h>

namespace yarp {
    namespace os {
        namespace impl {
            class BottleImpl;
            class Storable;
            class StoreNull;
            class StoreInt;
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
 *
 * A single item in a Bottle.  This extends the public yarp::os::Value
 * interface with some implementation-specific details.
 *
 */
class YARP_OS_impl_API yarp::os::impl::Storable : public yarp::os::Value {
public:
    virtual bool isBool() const    { return false; }
    virtual bool isInt() const     { return false; }
    virtual bool isString() const  { return false; }
    virtual bool isDouble() const  { return false; }
    virtual bool isList() const    { return false; }
    virtual bool isDict() const    { return false; }
    virtual bool isVocab() const   { return false; }
    virtual bool isBlob() const    { return false; }
    virtual bool isNull() const    { return false; }

    virtual bool asBool() const          { return false; }
    virtual int asInt() const            { return 0; }
    virtual int asVocab() const          { return 0; }
    virtual double asDouble() const      { return 0; }
    virtual yarp::os::ConstString asString() const {
        String str = asStringFlex();
        return yarp::os::ConstString(str.c_str(),str.length());
    }
    virtual Searchable *asSearchable() const {
        if (isDict()) return asDict();
        return asList();
    }
    virtual yarp::os::Bottle *asList() const { return NULL; }
    virtual yarp::os::Property *asDict() const { return NULL; }
    virtual const char *asBlob() const   { return (const char*)0; }
    virtual size_t asBlobLength() const     { return 0; }

    virtual bool read(ConnectionReader& connection);
    virtual bool write(ConnectionWriter& connection);

    virtual bool readRaw(ConnectionReader& connection) = 0;
    virtual bool writeRaw(ConnectionWriter& connection) = 0;

    virtual bool check(const yarp::os::ConstString& key);

    virtual yarp::os::Value& find(const yarp::os::ConstString& key);
    virtual yarp::os::Bottle& findGroup(const yarp::os::ConstString& key);

    virtual bool operator == (const yarp::os::Value& alt) const;

    virtual yarp::os::Value *create() const { return createStorable(); }

    virtual yarp::os::Value *clone() const {
        return cloneStorable();
    }

    /**
     *
     * Destructor.
     *
     */
    virtual ~Storable() {}

    /**
     *
     * Initialize from a string representation, assuming that any
     * syntax around this representation such as braces or
     * parentheses has already been consumed.
     *
     */
    virtual void fromString(const String& src) = 0;

    /**
     *
     * Initialize from a string representation.  This should consume
     * any syntax around that representation such as braces or
     * parentheses.
     *
     */
    virtual void fromStringNested(const String& src) {
        fromString(src);
    }

    virtual yarp::os::ConstString toString() const {
        String str = toStringFlex();
        return yarp::os::ConstString(str.c_str(),str.length());
    }

    /**
     *
     * Synonym for toString(), but with the internal String class
     * rather than ConstString.
     *
     */
    virtual String toStringFlex() const = 0;

    /**
     *
     * Create string representation, including any syntax that should
     * wrap it such as braces or parentheses.
     *
     */
    virtual String toStringNested() const { return toStringFlex(); }

    /**
     *
     * Factory method.
     *
     */
    virtual Storable *createStorable() const = 0;

    /**
     *
     * Synonym for asString(), but with the internal String class
     * rather than ConstString.
     *
     */
    virtual String asStringFlex() const { return ""; }

    /**
     *
     * Typed synonym for clone()
     *
     */
    virtual Storable *cloneStorable() const {
        Storable *item = createStorable();
        YARP_ASSERT(item!=NULL);
        item->copy(*this);
        return item;
    }

    /**
     *
     * Become a copy of the passed item.
     *
     */
    virtual void copy(const Storable& alt) = 0;

    /**
     *
     * Return a code describing this item, used in serializing bottles.
     *
     */
    virtual int subCode() const {
        return 0;
    }


    virtual bool isLeaf() const { return true; }

    static Storable *createByCode(int code);
};


/**
 *
 * An empty item.
 *
 */
class YARP_OS_impl_API yarp::os::impl::StoreNull : public Storable {
public:
    StoreNull() { }
    virtual yarp::os::ConstString toString() const { return ""; }
    virtual String toStringFlex() const { return ""; }
    virtual void fromString(const String& src) {}
    virtual int getCode() const { return -1; }
    virtual bool readRaw(ConnectionReader& connection) { return false; }
    virtual bool writeRaw(ConnectionWriter& connection) { return false; }
    virtual Storable *createStorable() const { return new StoreNull(); }
    virtual bool isNull() const { return true; }
    virtual void copy(const Storable& alt) {}
};


/**
 *
 * An integer item.
 *
 */
class YARP_OS_impl_API yarp::os::impl::StoreInt : public Storable {
private:
    int x;
public:
    StoreInt() { x = 0; }
    StoreInt(int x) { this->x = x; }
    virtual String toStringFlex() const;
    virtual void fromString(const String& src);
    virtual int getCode() const { return code; }
    virtual bool readRaw(ConnectionReader& connection);
    virtual bool writeRaw(ConnectionWriter& connection);
    virtual Storable *createStorable() const { return new StoreInt(0); }
    virtual int asInt() const { return x; }
    virtual int asVocab() const { return x; }
    virtual double asDouble() const { return x; }
    virtual bool isInt() const { return true; }
    static const int code;
    virtual void copy(const Storable& alt) { x = alt.asInt(); }
};

/**
 *
 * A vocabulary item.
 *
 */
class YARP_OS_impl_API yarp::os::impl::StoreVocab : public Storable {
private:
    int x;
public:
    StoreVocab() { x = 0; }
    StoreVocab(int x) { this->x = x; }
    virtual String toStringFlex() const;
    virtual void fromString(const String& src);
    virtual String toStringNested() const;
    virtual void fromStringNested(const String& src);
    virtual int getCode() const { return code; }
    virtual bool readRaw(ConnectionReader& connection);
    virtual bool writeRaw(ConnectionWriter& connection);
    virtual Storable *createStorable() const { return new StoreVocab(0); }
    virtual bool asBool() const { return x!=0; }
    virtual int asInt() const { return x; }
    virtual int asVocab() const { return x; }
    virtual double asDouble() const { return x; }
    virtual bool isVocab() const { return true; }
    virtual bool isBool() const { return (x==0||x=='1'); }
    static const int code;
    virtual void copy(const Storable& alt) { x = alt.asVocab(); }
    virtual String asStringFlex() const { return toStringFlex(); }
};

/**
 *
 * A string item.
 *
 */
class YARP_OS_impl_API yarp::os::impl::StoreString : public Storable {
private:
    String x;
public:
    StoreString() { x = ""; }
    StoreString(const String& x) { this->x = x; }
    virtual String toStringFlex() const;
    virtual void fromString(const String& src);
    virtual String toStringNested() const;
    virtual void fromStringNested(const String& src);
    virtual int getCode() const { return code; }
    virtual bool readRaw(ConnectionReader& connection);
    virtual bool writeRaw(ConnectionWriter& connection);
    virtual Storable *createStorable() const {
        return new StoreString(String(""));
    }
    virtual String asStringFlex() const { return x; }
    virtual int asVocab() const { return yarp::os::Vocab::encode(x.c_str()); }
    virtual bool isString() const { return true; }
    static const int code;
    virtual void copy(const Storable& alt) {
        yarp::os::ConstString y = alt.asString();
        String tmp;
        YARP_STRSET(tmp,(char*)y.c_str(),y.length(),0);
        x = tmp;
    }
};

/**
 *
 * A binary blob item.
 *
 */
class YARP_OS_impl_API yarp::os::impl::StoreBlob : public Storable {
private:
    String x;
public:
    StoreBlob() { x = ""; }
    StoreBlob(const String& x) { this->x = x; }
    virtual String toStringFlex() const;
    virtual void fromString(const String& src);
    virtual String toStringNested() const;
    virtual void fromStringNested(const String& src);
    virtual int getCode() const { return code; }
    virtual bool readRaw(ConnectionReader& connection);
    virtual bool writeRaw(ConnectionWriter& connection);
    virtual Storable *createStorable() const {
        return new StoreBlob(String(""));
    }
    virtual bool isBlob() const { return true; }
    virtual const char *asBlob() const         { return x.c_str(); }
    virtual size_t asBlobLength() const     { return x.length(); }
    static const int code;
    virtual void copy(const Storable& alt) {
        if (alt.isBlob()) {
            String tmp;
            YARP_STRSET(tmp,(char*)alt.asBlob(),alt.asBlobLength(),0);
            x = tmp;
        } else {
            x = String();
        }
    }
};

/**
 *
 * A floating point number item.
 *
 */
class YARP_OS_impl_API yarp::os::impl::StoreDouble : public Storable {
private:
    double x;
public:
    StoreDouble() { x = 0; }
    StoreDouble(double x) { this->x = x; }
    virtual String toStringFlex() const;
    virtual void fromString(const String& src);
    virtual int getCode() const { return code; }
    virtual bool readRaw(ConnectionReader& connection);
    virtual bool writeRaw(ConnectionWriter& connection);
    virtual Storable *createStorable() const {
        return new StoreDouble(0);
    }
    virtual int asInt() const { return (int)x; }
    virtual double asDouble() const { return x; }
    virtual bool isDouble() const { return true; }
    static const int code;
    virtual void copy(const Storable& alt) { x = alt.asDouble(); }
};


/**
 *
 * A nested list of items.
 *
 */
class YARP_OS_impl_API yarp::os::impl::StoreList : public Storable {
private:
    yarp::os::Bottle content;
public:
    StoreList() {}
    yarp::os::Bottle& internal() {
        return content;
    }
    virtual String toStringFlex() const;
    virtual void fromString(const String& src);
    virtual String toStringNested() const;
    virtual void fromStringNested(const String& src);
    virtual int getCode() const { return code+subCode(); }
    virtual bool readRaw(ConnectionReader& connection);
    virtual bool writeRaw(ConnectionWriter& connection);
    virtual Storable *createStorable() const {
        return new StoreList();
    }
    virtual bool isList() const { return true; }
    virtual yarp::os::Bottle *asList() const {
        return (yarp::os::Bottle*)(&content);
    }
    static const int code;
    virtual int subCode() const;

    virtual yarp::os::Value& find(const yarp::os::ConstString& key) {
        return content.find(key);
    }

    virtual yarp::os::Bottle& findGroup(const yarp::os::ConstString& key) {
        return content.findGroup(key);
    }
    virtual void copy(const Storable& alt) { content = *(alt.asList()); }
};


/**
 *
 * Key/value pairs
 *
 */
class YARP_OS_impl_API yarp::os::impl::StoreDict : public Storable {
private:
    yarp::os::Property content;
public:
    StoreDict() {}
    yarp::os::Property& internal() {
        return content;
    }
    virtual String toStringFlex() const;
    virtual void fromString(const String& src);
    virtual String toStringNested() const;
    virtual void fromStringNested(const String& src);
    virtual int getCode() const { return code; }
    virtual bool readRaw(ConnectionReader& connection);
    virtual bool writeRaw(ConnectionWriter& connection);
    virtual Storable *createStorable() const {
        return new StoreDict();
    }
    virtual bool isDict() const { return true; }
    virtual yarp::os::Property *asDict() const {
        return (yarp::os::Property*)(&content);
    }
    static const int code;
  
    virtual yarp::os::Value& find(const yarp::os::ConstString& key) {
        return content.find(key);
    }

    virtual yarp::os::Bottle& findGroup(const yarp::os::ConstString& key) {
        return content.findGroup(key);
    }
    virtual void copy(const Storable& alt) { content = *(alt.asDict()); }
};



/**
 * A flexible data format for holding a bunch of numbers and strings.
 * Handy to use until you work out how to make your own more
 * efficient formats for transmission.
 */
class YARP_OS_impl_API yarp::os::impl::BottleImpl : public yarp::os::Portable {
public:
    BottleImpl();
    virtual ~BottleImpl();


    bool isInt(int index);
    bool isString(int index);
    bool isDouble(int index);
    bool isList(int index);

    Storable *pop();

    int getInt(int index);
    yarp::os::ConstString getString(int index);
    double getDouble(int index);

    Storable& get(int x) const;

    yarp::os::Bottle *getList(int index);

    void addInt(int x) {
        add(new StoreInt(x));
    }

    void addVocab(int x) {
        add(new StoreVocab(x));
    }

    void addDouble(double x) {
        add(new StoreDouble(x));
    }

    void addString(const yarp::os::ConstString& text) {
        add(new StoreString(text));
    }

    yarp::os::Bottle& addList();

    yarp::os::Property& addDict();

    void clear();

    void fromString(const String& line);
    String toString();
    size_t size() const;

    virtual bool read(ConnectionReader& connection);
    virtual bool write(ConnectionWriter& connection);

    virtual void onCommencement();

    /*
      virtual bool write(ConnectionWriter& writer) {
      writeBlock(writer);
      return true;
      }

      virtual bool read(yarp::os::ConnectionReader& reader) {
      readBlock(reader);
      return true;
      }
    */

    const char *getBytes();
    size_t byteCount();

    void copyRange(const BottleImpl& alt, int first = 0, int len = -1);

    bool fromBytes(const yarp::os::Bytes& data);
    void toBytes(const yarp::os::Bytes& data);

    bool fromBytes(yarp::os::ConnectionReader& reader);

    void fromBinary(const char *text, int len);

    void specialize(int subCode);
    int getSpecialization();
    void setNested(bool nested);

    int subCode();

    void addBit(yarp::os::Value *bit) {
        // all Values are Storables -- important invariant!
        add((Storable*)(bit));
    }

    void addBit(const yarp::os::Value& bit) {
        // all Values are Storables -- important invariant!
        if (!bit.isNull()) {
            add((Storable*)(bit.clone()));
        }
    }

    yarp::os::Value& addBit(const char *str) {
        size_t len = size();
        String x(str);
        smartAdd(x);
        if (size()>len) {
            return get((int)size()-1);
        }
        return get(-1);
    }

    static StoreNull& getNull() {
        return storeNull;
    }

    // check if a piece of text is a completed bottle
    static bool isComplete(const char *txt);

    void hasChanged() {
        dirty = true;
    }

private:
    static StoreNull storeNull;

    PlatformVector<Storable*> content;
    PlatformVector<char> data;
    int speciality;
    bool nested;
    bool dirty;

    void add(Storable *s);
    void smartAdd(const String& str);

    void synch();
};


#endif

