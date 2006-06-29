// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef _YARP2_BOTTLEBIT_
#define _YARP2_BOTTLEBIT_

#include <yarp/os/ConstString.h>
#include <yarp/os/Searchable.h>
#include <yarp/os/Portable.h>

namespace yarp {
    namespace os {
        class BottleBit;
    }
}

/**
 *
 * A single value within a Bottle.  Values can be integers, strings,
 * doubles (floating-point numbers), lists, vocabulary, or blobs
 * (unformatted binary data).  This set is carefully chosen to have
 * good text and binary representations both for network transmission
 * and human viewing/generation.  Lists are represented as a nested
 * Bottle object.  BottleBit objects are Searchable - but you won't
 * find anything in them unless they are actually a list.
 *
 */
class yarp::os::BottleBit : public Portable, public Searchable {
public:
    virtual bool isInt()     { return false; }
    virtual bool isString()  { return false; }
    virtual bool isDouble()  { return false; }
    virtual bool isList()    { return false; }
    virtual bool isVocab()   { return false; }
    virtual bool isBlob()    { return false; }

    virtual int asInt()            { return 0; }
    virtual int asVocab()          { return 0; }
    virtual double asDouble()      { return 0; }
    virtual ConstString asString() { return ""; }
    virtual Bottle *asList()       { return (Bottle*)0; }
    virtual const char *asBlob()   { return (const char*)0; }
    virtual int asBlobLength()     { return 0; }

    virtual bool read(ConnectionReader& connection) = 0;
    virtual bool write(ConnectionWriter& connection) = 0;

    virtual bool operator == (const BottleBit& alt);

    virtual bool operator != (const BottleBit& alt) {
        return !((*this)==alt);
    }

    virtual BottleBit *create() = 0;
    virtual BottleBit *clone() = 0;

    virtual int getCode() { return 0; }

};


#endif
