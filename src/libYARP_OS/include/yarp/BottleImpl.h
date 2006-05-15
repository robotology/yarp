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
    class StoreInt;
    class StoreDouble;
    class StoreString;
    class StoreList;
}


class yarp::Storable {
public:
    virtual ~Storable() {}
    virtual String toString() = 0;
    virtual void fromString(const String& src) = 0;
    virtual int getCode() = 0;
    virtual bool read(ConnectionReader& reader) = 0;
    virtual bool write(ConnectionWriter& writer) = 0;
    virtual Storable *create() = 0;

    virtual int asInt() { return 0; }
    virtual double asDouble() { return 0; }
    virtual String asString() { return ""; }
};

class yarp::StoreInt : public Storable {
private:
    int x;
public:
    StoreInt() { x = 0; }
    StoreInt(int x) { this->x = x; }
    virtual String toString();
    virtual void fromString(const String& src);
    virtual int getCode() { return code; }
    virtual bool read(ConnectionReader& reader);
    virtual bool write(ConnectionWriter& writer);
    virtual Storable *create() { return new StoreInt(0); }
    virtual int asInt() { return x; }
    virtual double asDouble() { return x; }
    static const int code;
};

class yarp::StoreString : public Storable {
private:
    String x;
public:
    StoreString() { x = ""; }
    StoreString(const String& x) { this->x = x; }
    virtual String toString();
    virtual void fromString(const String& src);
    virtual int getCode() { return code; }
    virtual bool read(ConnectionReader& reader);
    virtual bool write(ConnectionWriter& writer);
    virtual Storable *create() { return new StoreString(String("")); }
    virtual String asString() { return x; }
    static const int code;
};

class yarp::StoreDouble : public Storable {
private:
    double x;
public:
    StoreDouble() { x = 0; }
    StoreDouble(double x) { this->x = x; }
    virtual String toString();
    virtual void fromString(const String& src);
    virtual int getCode() { return code; }
    virtual bool read(ConnectionReader& reader);
    virtual bool write(ConnectionWriter& writer);
    virtual Storable *create() { return new StoreDouble(0); }
    virtual int asInt() { return (int)x; }
    virtual double asDouble() { return x; }
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
    String getString(int index);
    double getDouble(int index);
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

    ACE_Vector<Storable*> content;
    ACE_Vector<char> data;
    bool dirty;

    void add(Storable *s);
    void smartAdd(const String& str);

    void synch();
};


class yarp::StoreList : public Storable {
private:
    yarp::os::Bottle content;
public:
    StoreList() {}
    yarp::os::Bottle& internal() {
        return content;
    }
    virtual String toString();
    virtual void fromString(const String& src);
    virtual int getCode() { return code; }
    virtual bool read(ConnectionReader& reader);
    virtual bool write(ConnectionWriter& writer);
    virtual Storable *create() { return new StoreList(); }
    static const int code;
};

#endif

