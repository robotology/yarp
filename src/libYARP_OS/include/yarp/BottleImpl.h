// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef _YARP2_BOTTLEIMPL_
#define _YARP2_BOTTLEIMPL_

#include <yarp/String.h>
#include <yarp/ManagedBytes.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/Portable.h>

#include <ace/Vector_T.h>

#include <yarp/os/Bottle.h>

namespace yarp {
    class BottleImpl;
    class Storable;
    class StoreNull;
    class StoreInt;
    class StoreDouble;
    class StoreString;
    class StoreList;
}


class yarp::Storable : public yarp::os::BottleBit {
public:
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

    virtual int getCode() = 0;
    virtual bool read(ConnectionReader& reader) = 0;
    virtual bool write(ConnectionWriter& writer) = 0;
    virtual Storable *create() = 0;

    virtual int asInt() { return 0; }
    virtual double asDouble() { return 0; }
    virtual String asStringFlex() { return ""; }
    virtual yarp::os::ConstString asString() { 
        return yarp::os::ConstString(asStringFlex().c_str()); 
    }
    virtual yarp::os::Bottle *asList() { return NULL; }

    virtual Storable *clone() {
        Storable *item = create();
        YARP_ASSERT(item!=NULL);
        item->copy(*this);
        return item;
    }    

    virtual void copy(const Storable& alt) {
        fromString(alt.toStringFlex());  
        // general, if inefficient, copy method
        // ideally would have special cases in subclasses
    }
};


class yarp::StoreNull : public Storable {
public:
    StoreNull() { }
    virtual yarp::os::ConstString toString() const { return ""; }
    virtual String toStringFlex() const { return ""; }
    virtual void fromString(const String& src) {}
    virtual int getCode() { return -1; }
    virtual bool read(ConnectionReader& reader) { return false; }
    virtual bool write(ConnectionWriter& writer) { return false; }
    virtual Storable *create() { return new StoreNull(); }
    virtual bool isNull() { return true; }
};


class yarp::StoreInt : public Storable {
private:
    int x;
public:
    StoreInt() { x = 0; }
    StoreInt(int x) { this->x = x; }
    virtual String toStringFlex() const;
    virtual void fromString(const String& src);
    virtual int getCode() { return code; }
    virtual bool read(ConnectionReader& reader);
    virtual bool write(ConnectionWriter& writer);
    virtual Storable *create() { return new StoreInt(0); }
    virtual int asInt() { return x; }
    virtual double asDouble() { return x; }
    virtual bool isInt() { return true; }
    static const int code;
};

class yarp::StoreString : public Storable {
private:
    String x;
public:
    StoreString() { x = ""; }
    StoreString(const String& x) { this->x = x; }
    virtual String toStringFlex() const;
    virtual void fromString(const String& src);
    virtual String toStringNested() const;
    virtual void fromStringNested(const String& src);
    virtual int getCode() { return code; }
    virtual bool read(ConnectionReader& reader);
    virtual bool write(ConnectionWriter& writer);
    virtual Storable *create() { return new StoreString(String("")); }
    virtual String asStringFlex() { return x; }
    virtual bool isString() { return true; }
    static const int code;
};

class yarp::StoreDouble : public Storable {
private:
    double x;
public:
    StoreDouble() { x = 0; }
    StoreDouble(double x) { this->x = x; }
    virtual String toStringFlex() const;
    virtual void fromString(const String& src);
    virtual int getCode() { return code; }
    virtual bool read(ConnectionReader& reader);
    virtual bool write(ConnectionWriter& writer);
    virtual Storable *create() { return new StoreDouble(0); }
    virtual int asInt() { return (int)x; }
    virtual double asDouble() { return x; }
    virtual bool isDouble() { return true; }
    static const int code;
};


class yarp::StoreList : public Storable {
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
    virtual int getCode() { return code; }
    virtual bool read(ConnectionReader& reader);
    virtual bool write(ConnectionWriter& writer);
    virtual Storable *create() { return new StoreList(); }
    virtual bool isList() { return true; }
    virtual yarp::os::Bottle *asList() { return &content; }
    static const int code;
};



/**
 * A flexible data format for holding a bunch of numbers and strings.
 * Handy to use until you work out how to make your own more 
 * efficient formats for transmission.
 */
class yarp::BottleImpl : public yarp::os::Portable {
public:

    BottleImpl();
    virtual ~BottleImpl();


    bool isInt(int index);
    bool isString(int index);
    bool isDouble(int index);
    bool isList(int index);

    int getInt(int index);
    yarp::os::ConstString getString(int index);
    double getDouble(int index);

    yarp::os::BottleBit& get(int x);

    yarp::os::Bottle *getList(int index);

    void addInt(int x) {
        add(new StoreInt(x));
    }

    void addDouble(double x) {
        add(new StoreDouble(x));
    }

    void addInts(int *x, int ct) {
    }

    void addFloats(double *x, int ct) {
    }

    void addString(const char *text) {
        add(new StoreString(String(text)));
    }

    yarp::os::Bottle& addList();

    void clear();

    void fromString(const String& line);
    String toString();
    int size();

    bool fromBytes(const Bytes& data);
    void toBytes(const Bytes& data);

    virtual bool write(ConnectionWriter& writer);

    virtual bool read(ConnectionReader& reader);

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



private:
    static StoreNull storeNull;

    ACE_Vector<Storable*> content;
    ACE_Vector<char> data;
    bool dirty;

    void add(Storable *s);
    void smartAdd(const String& str);

    void synch();
};


#endif

