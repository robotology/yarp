// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006, 2008 Paul Fitzpatrick, Arjan Gijsberts
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_BOTTLEIMPL_
#define _YARP2_BOTTLEIMPL_

#include <yarp/String.h>
#include <yarp/ManagedBytes.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/Portable.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/Semaphore.h>

#include <ace/Vector_T.h>

#include <yarp/os/Bottle.h>
#include <yarp/Logger.h>

namespace yarp {
    namespace os {
        namespace impl {
            class BottleImpl;
            class Storable;
            class StoreNull;
            class StoreInt;
            class StoreVocab;
            class StoreDouble;
            class StoreString;
            class StoreBlob;
            class StoreList;
        }
    }
}


class yarp::os::impl::Storable : public yarp::os::Value {
public:
    virtual bool isInt() const     { return false; }
    virtual bool isString() const { return false; }
    virtual bool isDouble() const  { return false; }
    virtual bool isList() const    { return false; }
    virtual bool isVocab() const   { return false; }
    virtual bool isBlob() const    { return false; }

    virtual int asInt() const            { return 0; }
    virtual int asVocab() const          { return 0; }
    virtual double asDouble() const      { return 0; }
    virtual yarp::os::ConstString asString() const { 
        return yarp::os::ConstString(asStringFlex().c_str()); 
    }
    virtual yarp::os::Bottle *asList() const { return NULL; }
    virtual const char *asBlob() const   { return (const char*)0; }
    virtual int asBlobLength() const     { return 0; }

    virtual bool read(ConnectionReader& connection) = 0;
    virtual bool write(ConnectionWriter& connection) = 0;

    virtual bool check(const char *key);

    virtual yarp::os::Value& find(const char *key);
    virtual yarp::os::Bottle& findGroup(const char *key);

    virtual bool operator == (const yarp::os::Value& alt) const;

    virtual yarp::os::Value *create() const { return createStorable(); }

    virtual yarp::os::Value *clone() const {
        return cloneStorable();
    }


    virtual ~Storable() {}

    virtual void fromString(const String& src) = 0;
    virtual void fromStringNested(const String& src) {
        fromString(src);
    }

    virtual yarp::os::ConstString toString() const {
        return yarp::os::ConstString(toStringFlex().c_str());
    }

    virtual String toStringFlex() const = 0;
    virtual String toStringNested() const { return toStringFlex(); }

    //virtual int getCode() = 0;
    virtual Storable *createStorable() const = 0;

    virtual String asStringFlex() const { return ""; }

    virtual Storable *cloneStorable() const {
        Storable *item = createStorable();
        YARP_ASSERT(item!=NULL);
        item->copy(*this);
        return item;
    }

    virtual void copy(const Storable& alt) = 0;

    virtual int subCode() const {
        return 0;
    }


};


class yarp::os::impl::StoreNull : public Storable {
public:
    StoreNull() { }
    virtual yarp::os::ConstString toString() const { return ""; }
    virtual String toStringFlex() const { return ""; }
    virtual void fromString(const String& src) {}
    virtual int getCode() const { return -1; }
    virtual bool read(ConnectionReader& connection) { return false; }
    virtual bool write(ConnectionWriter& connection) { return false; }
    virtual Storable *createStorable() const { return new StoreNull(); }
    virtual bool isNull() const { return true; }
    virtual void copy(const Storable& alt) {}
};


class yarp::os::impl::StoreInt : public Storable {
private:
    int x;
public:
    StoreInt() { x = 0; }
    StoreInt(int x) { this->x = x; }
    virtual String toStringFlex() const;
    virtual void fromString(const String& src);
    virtual int getCode() const { return code; }
    virtual bool read(ConnectionReader& connection);
    virtual bool write(ConnectionWriter& connection);
    virtual Storable *createStorable() const { return new StoreInt(0); }
    virtual int asInt() const { return x; }
    virtual int asVocab() const { return x; }
    virtual double asDouble() const { return x; }
    virtual bool isInt() const { return true; }
    static const int code;
    virtual void copy(const Storable& alt) { x = alt.asInt(); }
};

class yarp::os::impl::StoreVocab : public Storable {
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
    virtual bool read(ConnectionReader& connection);
    virtual bool write(ConnectionWriter& connection);
    virtual Storable *createStorable() const { return new StoreVocab(0); }
    virtual int asInt() const { return x; }
    virtual int asVocab() const { return x; }
    virtual double asDouble() const { return x; }
    virtual bool isVocab() const { return true; }
    static const int code;
    virtual void copy(const Storable& alt) { x = alt.asVocab(); }
};

class yarp::os::impl::StoreString : public Storable {
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
    virtual bool read(ConnectionReader& connection);
    virtual bool write(ConnectionWriter& connection);
    virtual Storable *createStorable() const { 
        return new StoreString(String("")); 
    }
    virtual String asStringFlex() const { return x; }
    virtual int asVocab() const { return yarp::os::Vocab::encode(x.c_str()); }
    virtual bool isString() const { return true; }
    static const int code;
    virtual void copy(const Storable& alt) { x = alt.asString().c_str(); }
};

class yarp::os::impl::StoreBlob : public Storable {
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
    virtual bool read(ConnectionReader& connection);
    virtual bool write(ConnectionWriter& connection);
    virtual Storable *createStorable() const { 
        return new StoreBlob(String("")); 
    }
    virtual bool isBlob() const { return true; }
    virtual const char *asBlob() const         { return x.c_str(); }
    virtual int asBlobLength() const     { return x.length(); }
    static const int code;
    virtual void copy(const Storable& alt) { x = alt.asBlob(); }
};

class yarp::os::impl::StoreDouble : public Storable {
private:
    double x;
public:
    StoreDouble() { x = 0; }
    StoreDouble(double x) { this->x = x; }
    virtual String toStringFlex() const;
    virtual void fromString(const String& src);
    virtual int getCode() const { return code; }
    virtual bool read(ConnectionReader& connection);
    virtual bool write(ConnectionWriter& connection);
    virtual Storable *createStorable() const { 
        return new StoreDouble(0); 
    }
    virtual int asInt() const { return (int)x; }
    virtual double asDouble() const { return x; }
    virtual bool isDouble() const { return true; }
    static const int code;
    virtual void copy(const Storable& alt) { x = alt.asDouble(); }
};


class yarp::os::impl::StoreList : public Storable {
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
    virtual bool read(ConnectionReader& connection);
    virtual bool write(ConnectionWriter& connection);
    virtual Storable *createStorable() const { 
        return new StoreList(); 
    }
    virtual bool isList() const { return true; }
    virtual yarp::os::Bottle *asList() const { 
        return (yarp::os::Bottle*)(&content); 
    }
    static const int code;
    virtual int subCode() const;

    virtual yarp::os::Value& find(const char *key) {
        return content.find(key);
    }

    virtual yarp::os::Bottle& findGroup(const char *key) {
        return content.findGroup(key);
    }
    virtual void copy(const Storable& alt) { content = *(alt.asList()); }
};



/**
 * A flexible data format for holding a bunch of numbers and strings.
 * Handy to use until you work out how to make your own more 
 * efficient formats for transmission.
 */
class yarp::os::impl::BottleImpl : public yarp::os::Portable {
public:

    BottleImpl();
    virtual ~BottleImpl();


    bool isInt(int index);
    bool isString(int index);
    bool isDouble(int index);
    bool isList(int index);

    Storable& pop();

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

    void addString(const char *text) {
        add(new StoreString(String(text)));
    }

    yarp::os::Bottle& addList();

    void clear();

    void fromString(const String& line);
    String toString();
    int size() const;

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
    int byteCount();

    void copyRange(const BottleImpl& alt, int first = 0, int len = -1);

    bool fromBytes(const Bytes& data);
    void toBytes(const Bytes& data);

    bool fromBytes(ConnectionReader& reader);

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
        int len = size();
        String x(str);
        smartAdd(x);
        if (size()>len) {
            return get(size()-1);
        }
        return get(-1);
    }

    static StoreNull& getNull() {
        return storeNull;
    }

    // check if a piece of text is a completed bottle
    static bool isComplete(const char *txt);

private:
    static StoreNull storeNull;

    ACE_Vector<Storable*> content;
    ACE_Vector<char> data;
    int speciality;
    bool nested;
    bool dirty;

    void add(Storable *s);
    void smartAdd(const String& str);

    void synch();
};


#endif

