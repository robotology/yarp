// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_VALUE_
#define _YARP2_VALUE_

#include <yarp/os/ConstString.h>
#include <yarp/os/Searchable.h>
#include <yarp/os/Portable.h>
#include <yarp/os/Bottle.h>
#include <stddef.h> // defines size_t

namespace yarp {
    namespace os {
        class Value;
        class ValueType;
        class ValueInt;
        class ValueDouble;
        class ValueString;
        class ValueVocab;
        class ValueBlob;
        class ValueList;
        class ValueDict;
        class Property;
    }
}

/**
 * A single value (typically within a Bottle). Values can be integers, strings,
 * doubles (floating-point numbers), lists, vocabulary, or blobs
 * (unformatted binary data).  This set is carefully chosen to have
 * good text and binary representations both for network transmission
 * and human viewing/generation.  Lists are represented as a nested
 * Bottle object.  Value objects are Searchable - but you won't
 * find anything in them unless they are actually a list.
 *
 */
class YARP_OS_API yarp::os::Value : public Portable, public Searchable {
private:
    Value *proxy;

    void setProxy(Value *proxy);
    void ok() const;


public:

    /**
     * Construct a list Value
     */
    Value() {
        proxy = 0 /*NULL*/;
    }

    /**
     * Construct an integer Value
     * @param x the integer
     * @param isVocab set this to true if the integer should be interpreted
     * as a vocabulary identifier
     */
    Value(int x, bool isVocab = false) {
        proxy = 0 /*NULL*/;
        if (!isVocab) {
            setProxy(makeInt(x));
        } else {
            setProxy(makeVocab(x));
        }
    }

    /**
     * Construct a floating point Value
     * @param x the number
     */
    Value(double x) {
        proxy = 0 /*NULL*/;
        setProxy(makeDouble(x));
    }

    /**
     * Construct a string Value
     * @param str the string
     * @param isVocab set this to true if the string should be interpreted
     * as a vocabulary identifier
     */
    Value(const char *str, bool isVocab = false) {
        proxy = 0 /*NULL*/;
        if (!isVocab) {
            setProxy(makeString(str));
        } else {
            setProxy(makeVocab(str));
        }
    }

    /**
     * Construct a binary data Value
     * @param data the data to copy
     * @param length the size of the data in bytes
     */
    Value(void *data, int length) {
        proxy = 0 /*NULL*/;
        setProxy(makeBlob(data,length));
    }


    /**
     * Copy constructor.
     * @param alt the value to copy
     */
    Value(const Value& alt);

    /**
     * Assignment operator
     * @param alt the value to copy
     * @return this object itself
     */
    const Value& operator = (const Value& alt);

    /**
     * Destructor
     */
    virtual ~Value() {
        if (proxy!=0 /*NULL*/) { 
            delete proxy;
            proxy = 0 /*NULL*/;
        }
    }

    /**
     * Checks if value is a boolean. If so, asBool() will return that boolean.
     * @return true iff value is a boolean
     */
    virtual bool isBool() const     { ok(); return proxy->isBool(); }

    /**
     * Checks if value is an integer. If so, asInt() will return that integer.
     * @return true iff value is an integer
     */
    virtual bool isInt() const     { ok(); return proxy->isInt(); }

    /**
     * Checks if value is a string. If so, asString() will return that string.
     * @return true iff value is a string
     */
    virtual bool isString() const  { ok(); return proxy->isString(); }

    /**
     * Checks if value is a floating point number. If so, asDouble() will return that number.
     * @return true iff value is a floating point number
     */
    virtual bool isDouble() const  { ok(); return proxy->isDouble(); }

    /**
     * Checks if value is a list. If so, asList() will return that list.
     * @return true iff value is a list
     */
    virtual bool isList() const    { ok(); return proxy->isList(); }

    /**
     * Checks if value is a dictionary. If so, asDict() will return that 
     * dictionary.
     * @return true iff value is a dictionary
     */
    virtual bool isDict() const    { ok(); return proxy->isDict(); }

    /**
     * Checks if value is a vocabulary identifier. If so, asVocab()
     * will return it.  
     * @return true iff value is a vocabulary identifier
     */
    virtual bool isVocab() const   { ok(); return proxy->isVocab(); }


    /**
     * Checks if value is a binary object. If so, asBlob() and asBlobLength() 
     * will return it.
     * @return true iff value is a binary object.
     */
    virtual bool isBlob() const    { ok(); return proxy->isBlob(); }

    /**
     * Get boolean value.
     * @return boolean value if value is indeed a boolean.
     * If it is another type, the appropriate cast value is returned.
     * Otherwise returns false.
     */
    virtual bool asBool() const         { ok(); return proxy->asBool(); }

    /**
     * Get integer value.
     * @return integer value if value is indeed an integer.
     * If it is another numeric type, the appropriate cast value is returned.
     * Otherwise returns 0.
     */
    virtual int asInt() const           { ok(); return proxy->asInt(); }

    /**
     * Get vocabulary identifier as an integer.
     * @return integer value of vocabulary identifier.
     */
    virtual int asVocab() const          { ok(); return proxy->asVocab(); }

    /**
     * Get floating point value.
     * @return floating point value if value is indeed a floating point.
     * If it is another numeric type, the appropriate cast value is returned.
     * Otherwise returns 0.
     */
    virtual double asDouble() const      { ok(); return proxy->asDouble(); }

    /**
     * Get string value.
     * @return string value if value is indeed a string.
     * Otherwise returns empty string.
     */
    virtual ConstString asString() const { ok(); return proxy->asString(); }

    /**
     * Get list value.
     * @return pointer to list if value is indeed a list.
     * Otherwise returns NULL
     */
    virtual Bottle *asList() const       { ok(); return proxy->asList(); }

    /**
     * Get dictionary (hash table) value.
     * @return pointer to dictionary if value is indeed of the right type.
     * Otherwise returns NULL
     */
    virtual Property *asDict() const       { ok(); return proxy->asDict(); }

    /**
     * Get dictionary or list value
     * @return pointer to dictionary or list value if present.
     * Otherwise returns NULL
     */
    virtual Searchable *asSearchable() const;

    /**
     * Get binary data value.
     * @return pointer to binary data if value is indeed binary data.
     * Otherwise returns NULL
     */
    virtual const char *asBlob() const   { ok(); return proxy->asBlob(); }

    /**
     * Get binary data length.
     * @return length of binary data if value is indeed binary data.
     * Otherwise returns 0
     */
    virtual size_t asBlobLength() const    { ok(); return proxy->asBlobLength(); }

    // documented in Portable
    virtual bool read(ConnectionReader& connection);

    // documented in Portable
    virtual bool write(ConnectionWriter& connection);

    // documented in Searchable
    virtual bool check(const char *key) {
        ok();
        return proxy->check(key);
    }

    // documented in Searchable
    virtual Value& find(const char *key) {
        ok();
        return proxy->find(key);
    }

    // documented in Searchable
    virtual Bottle& findGroup(const char *key) {
        ok();
        return proxy->findGroup(key);
    }


    /**
     * Equality test.
     * @param alt the value to compare against
     * @result true iff the values are equal
     */
    virtual bool operator == (const Value& alt) const { 
        ok(); return (*proxy)==alt; 
    }

    /**
     * Inequality test.
     * @param alt the value to compare against
     * @result true iff the values are not equal
     */
    virtual bool operator != (const Value& alt) const { 
        return !((*this)==alt);
    }


    /**
     * Set value to correspond to a textual representation.
     * For example if str="10" then the value will be an integer,
     * if str="(1 2 3)" the value will be a list.
     * @param str the textual representation.
     */
    void fromString(const char *str) {
        setProxy(makeValue(str));
    }

    ConstString toString() const {
        ok();
        return proxy->toString();
    }

    /**
     * Create a new value of the same type.
     * @return the new value.
     */
    virtual Value *create() const { ok(); return proxy->create(); }

    /**
     * Create a copy of the value
     * @return the new value, which will equal this.
     */
    virtual Value *clone() const  { ok(); return proxy->clone(); } 

    /**
     * Get standard type code of value.
     * @return the standard type code of the value.
     */
    virtual int getCode() const { ok(); return proxy->getCode(); }

    virtual bool isNull() const  { ok(); return proxy->isNull(); }

    virtual bool isLeaf() const { return false; }

    /**
     * Create an integer Value
     * @param x the value to take on
     * @return an integer Value
     */
    static Value *makeInt(int x);

    /**
     * Create a floating point Value
     * @param x the value to take on
     * @return a floating point Value
     */
    static Value *makeDouble(double x);

    /**
     * Create a string Value
     * @param str the value to take on
     * @return a string Value
     */
    static Value *makeString(const char *str);

    /**
     * Create a vocabulary identifier Value
     * @param v the value to take on
     * @return a vocabulary identifier Value
     */
    static Value *makeVocab(int v);

    /**
     * Create a vocabulary identifier Value
     * @param str the value to take on
     * @return a vocabulary identifier Value
     */
    static Value *makeVocab(const char *str);


    /**
     * Create a Value containing binary data
     * @param data the data (will be copied)
     * @param length the size of the data in bytes
     * @return a Value containing binary data
     */
    static Value *makeBlob(void *data, int length);

    /**
     * Create a list Value
     * @return a list Value
     */
    static Value *makeList();

    /**
     * Create a list Value and initialize it
     * @param txt the list in text form, e.g. "1 2.0 (4 5) xyz"
     * @return a list Value
     */
    static Value *makeList(const char *txt);

    /**
     * Create a Value from a text description.
     * @param txt the description.  E.g. "10" will create an integer,
     * "(5 6 7)" will create a list.
     * @return the Value to which the text description corresponds
     */
    static Value *makeValue(const char *txt);


    /**
     * Return an invalid, "null" Value
     * @return an invalid "null" Value
     */
    static Value& getNullValue();
};


#endif
