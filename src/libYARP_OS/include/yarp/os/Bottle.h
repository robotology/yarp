// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006, 2008 RobotCub Consortium, Arjan Gijsberts
 * Authors: Paul Fitzpatrick, Arjan Gijsberts
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_BOTTLE_
#define _YARP2_BOTTLE_

#include <yarp/os/ConstString.h>
#include <yarp/os/Portable.h>
#include <yarp/os/Value.h>
#include <yarp/conf/numeric.h>

#define BOTTLE_TAG_INT 1
#define BOTTLE_TAG_VOCAB (1+8)
#define BOTTLE_TAG_DOUBLE (2+8)
#define BOTTLE_TAG_STRING (4)
#define BOTTLE_TAG_BLOB (4+8)
#define BOTTLE_TAG_INT64 (1+16)
#define BOTTLE_TAG_LIST 256
#define BOTTLE_TAG_DICT 512

namespace yarp {
    namespace os {
        class Bottle;
        class Property;
        class NetworkBase;
    }
}


/**
 * \ingroup key_class
 *
 * \brief A simple collection of objects that can be described and transmitted
 * in a portable way.
 *
 * Objects are stored in a list, which you can add to and access.
 * It is handy to use until you feel the need to make your own more
 * efficient formats for transmission.
 *
 * Here's an example of using a Bottle:
 * \include example/os/bottle_add.cpp
 *
 * This class has a well-defined, documented representation in both
 * binary and text form.  The name of this class comes from the idea
 * of throwing a "message in a bottle" into the network and hoping it
 * will eventually wash ashore somewhere else.  In the very early days
 * of YARP, that is what communication felt like.
 */
class YARP_OS_API yarp::os::Bottle : public Portable, public Searchable {
public:
    using Searchable::check;
    using Searchable::findGroup;

    /**
     * Constructor.  The bottle is initially empty.
     */
    Bottle();

    /**
     * Creates a bottle from a string.
     *
     * The string which should contain a textual form of the bottle,
     * e.g. text = "10 -5.3 1.0 \"hello there\"" would give a bottle
     * with 4 elements: an integer, two floating point numbers, and a
     * string.
     *
     * @param text the textual form of the bottle to be interpreted.
     */
    Bottle(const ConstString& text);

    /**
     * Copy constructor.
     *
     * @param bottle The object to copy.
     */
    Bottle(const Bottle& bottle);


    /**
     * Empties the bottle of any objects it contains.
     */
    void clear();

    /**
     * Places an integer in the bottle, at the end of the list.
     *
     * @param x the integer to add.
     */
    void addInt(int x);

    /**
     * Places a 64 integer in the bottle, at the end of the list.
     *
     * @param x the integer to add.
     */
    void addInt64(const YARP_INT64& x);

    /**
     * Places a vocabulary item in the bottle, at the end of the list.
     *
     * @param x the item to add.
     */
    void addVocab(int x);

    /**
     * Places a floating point number in the bottle, at the end of the
     * list.
     *
     * @param x the number to add.
     */
    void addDouble(double x);

    /**
     * Places a string in the bottle, at the end of the list.
     *
     * @param str the string to add.
     */
    void addString(const char *str);

    /**
     * Places a string in the bottle, at the end of the list.
     *
     * @param str the string to add.
     */
    void addString(const ConstString& str);

    /**
     * Add a Value to the bottle, at the end of the list.
     *
     * @param value the Value to add.
     */
    void add(const Value& value);

    // worked "accidentally", user code relies on this
    void add(const char *txt) {
        addString(txt);
    }

    /**
     * Add a Value to the bottle, at the end of the list.
     *
     * The object passed will be placed directly into the list, without
     * copying.  The Bottle will be responsible for deallocating it
     * when appropriate.
     *
     * @param value the Value to add.
     */
    void add(Value *value);

    /**
     * Places an empty nested list in the bottle, at the end of the
     * list.
     *
     * The list itself is represented as a bottle.
     *
     * @return a reference to the newly added list.
     */
    Bottle& addList();

    /**
     * Places an empty key/value object in the bottle, at the end of the
     * list.
     *
     * @return a reference to the newly added property.
     */
    Property& addDict();

    /**
     * Removes a Value v from the end of the list and returns this
     * value.
     *
     * @return the value popped from the end of the bottle.
     */
    Value pop();

    /**
     * Reads a Value v from a certain part of the list.
     *
     * Methods like v.isInt() or v.isString() can be used to check the
     * type of the result.
     * Methods like v.asInt() or v.asString() can be used to access the
     * result as a particular type.
     *
     * @param index the part of the list to read from.
     * @return the Value v; if the index lies outside the range of
     *         elements present, then v.isNull() will be true.
     */
    Value& get(int index) const;

    /**
     * Gets the number of elements in the bottle.
     *
     * @return number of elements in the bottle.
     */
    int size() const;



    /**
     * Initializes bottle from a string.
     *
     * The string should contain a textual form of the bottle,
     * e.g. text = "10 -5.3 1.0 \"hello there\"" would give a bottle
     * with 4 elements: an integer, two floating point numbers, and a
     * string.
     *
     * @param text the textual form of the bottle to be interpreted.
     */
    void fromString(const ConstString& text);

    /**
     * Initializes bottle from a binary representation.
     *
     * @param buf the binary form of the bottle to be interpreted.
     * @param len the length of the binary form.
     */
    void fromBinary(const char *buf, int len);


    /**
     * Returns binary representation of bottle.
     *
     * Lifetime is until next modification of bottle.
     *
     * @param size if non-null, the referenced variable is set to the
     *             buffer size.
     * @return pointer to a buffer holding the binary representation of
     *         the bottle.
     */
    const char *toBinary(size_t *size = 0/*NULL*/);


    /**
     * Gives a human-readable textual representation of the bottle.
     *
     * This representation is suitable for passing to Bottle::fromString
     * (see that method for examples).
     *
     * @return a textual representation of the bottle.
     */
    ConstString toString() const;

    /**
     * Output a representation of the bottle to a network connection.
     *
     * @param writer the interface to the network connection for writing
     * @result true iff the representation was written successfully.
     */
    bool write(ConnectionWriter& writer);

    /**
     * Set the bottle's value based on input from a network connection.
     *
     * @param reader the interface to the network connection for reading
     * @return true iff the bottle was read successfully.
     */
    bool read(ConnectionReader& reader);

    /**
     * Copy the bottle's value to a object that can read a
     * serialization.
     *
     * Must be serialized in standard Bottle-compatible format.
     *
     * @param reader the serializable object.
     * @param textMode true if text serialization should be used.
     * @return true iff the bottle was written successfully.
     */
    bool write(PortReader& reader, bool textMode = false);

    /**
     * Set the bottle's value based on input from a serializable object.
     *
     * Must be serialized in standard Bottle-compatible format.
     *
     * @param writer the serializable object.
     * @param textMode true if text serialization should be used.
     * @return true iff the bottle was read successfully.
     */
    bool read(PortWriter& writer, bool textMode = false);


    void onCommencement();

    virtual bool check(const ConstString& key) const;

    virtual Value& find(const ConstString& key) const;

    Bottle& findGroup(const ConstString& key) const;

    virtual bool isNull() const;

    /**
     * Assignment operator.
     *
     * @param bottle The object to copy.
     * @return the Bottle itself.
     */
    const Bottle& operator = (const Bottle& bottle);


    /**
     * Destructor.
     */
    virtual ~Bottle();



    /**
     * Copy all or part of another Bottle.
     *
     * @param alt The object to copy.
     * @param first The index of the first element to copy.
     * @param len The number of elements to copy (-1 for all).
     */
    void copy(const Bottle& alt, int first = 0, int len = -1);

    /**
     * A special Bottle with no content.
     *
     * @return the special invalid "null" Bottle.
     */
    static Bottle& getNullBottle();


    /**
     * Equality test.
     *
     * @param alt the value to compare against.
     * @result true iff the values are equal.
     */
    virtual bool operator == (const Bottle& alt);

    /**
     * Inequality test.
     *
     * @param alt the value to compare against.
     * @result true iff the values are not equal.
     */
    virtual bool operator != (const Bottle& alt);

    /**
     * Append the content of the given bottle to the current
     * list.
     *
     * @param alt the content to add.
     */
    void append(const Bottle& alt);

    /**
     * Get all but the first element of a bottle.
     *
     * @return a bottle containing all but the first element of the
     * current bottle.
     */
    Bottle tail() const;




#ifndef DOXYGEN_SHOULD_SKIP_THIS
    // this is needed for implementation reasons -- could go away
    // with judicious use of friend declarations
    void specialize(int subCode);
    int getSpecialization();
    void setNested(bool nested);
#endif /*DOXYGEN_SHOULD_SKIP_THIS*/

    /**
     * Declare that the content of the Bottle has been changed.
     *
     * It is important to call this if you modify an individual element
     * of the Bottle through assignment, so that serialization happens
     * correctly.
     */
    void hasChanged();

    static ConstString toString(int x);

    /**
     *
     * Convert a numeric bottle code to a string.
     * @param the code to convert
     * @return a string representation of the code's meaning
     *
     */
    static ConstString describeBottleCode(int code);


protected:

    virtual void setReadOnly(bool readOnly) { 
        ro = readOnly;
    }

private:

    void edit();

    Value& findGroupBit(const ConstString& key) const;
    Value& findBit(const ConstString& key) const;

    virtual Bottle *create();

    virtual Bottle *clone();

    //Value& find(const char *key);
    void *implementation;
    bool invalid;
    bool ro;


    ///////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////
    // the methods below are no longer used
    ///////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////

    /**
     * Reads an integer from a certain part of the list.
     * @param index the part of the list to read from
     * @return the integer - result is undefined if it is not actually an
     *         integer - see Bottle::isInt
     */
    int getInt(int index);

    /**
     * Reads a string from a certain part of the list.
     * @param index the part of the list to read from
     * @return the string - result is undefined if it is not actually an
     *         string - see Bottle::isString
     */
    ConstString getString(int index);

    /**
     * Reads a floating point number from a certain part of the list.
     * @param index the part of the list to read from
     * @return the floating point number - result is undefined if it is not
     *         actually a floating point number - see Bottle::isDouble
     */
    double getDouble(int index);

    /**
     * Reads a nested list from a certain part of the list.
     * @param index the part of the list to read from
     * @return a pointer to the nested list, or NULL if it isn't a list -
     * see Bottle::isList
     */
    Bottle *getList(int index);

    /**
     * Checks if a certain part of the list is an integer.
     * @param index the part of the list to check
     * @return true iff that part of the list is indeed an integer
     */
    bool isInt(int index);

    /**
     * Checks if a certain part of the list is a floating point number.
     * @param index the part of the list to check
     * @return true iff that part of the list is indeed a floating point number
     */
    bool isDouble(int index);

    /**
     * Checks if a certain part of the list is a string.
     * @param index the part of the list to check
     * @return true iff that part of the list is indeed a string
     */
    bool isString(int index);

    /**
     * Checks if a certain part of the list is a nested list.
     * @param index the part of the list to check
     * @return true iff that part of the list is indeed a nested list
     */
    bool isList(int index);


    virtual bool isList();

    //virtual Bottle *asList() {
    //  return this;
    //}

    static void fini();
    friend class NetworkBase;

};

#endif

