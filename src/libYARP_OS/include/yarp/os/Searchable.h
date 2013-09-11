// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_SEARCHABLE_
#define _YARP2_SEARCHABLE_

#include <cstddef>
#include <yarp/os/ConstString.h>

namespace yarp {
    namespace os {
        class Value;
        class Bottle;
        class Searchable;
        class SearchMonitor;
        class SearchReport;
    }
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS

class YARP_OS_API yarp::os::SearchReport {
public:
    ConstString key;
    ConstString value;
    bool isFound;
    bool isGroup;
    bool isComment;
    bool isDefault;

    explicit SearchReport();
};

class YARP_OS_API yarp::os::SearchMonitor {
public:
    virtual ~SearchMonitor();
    virtual void report(const SearchReport& report, const char *context) = 0;
};

#endif /* DOXYGEN_SHOULD_SKIP_THIS */


/**
 *
 * A base class for nested structures that can be searched.
 * A Searchable object promises that you can look inside it
 * with the find() and findGroup() methods to get values and
 * lists corresponding to keywords.
 *
 * @see Property Bottle Value
 *
 */
class YARP_OS_API yarp::os::Searchable {
private:
    SearchMonitor *monitor;
    ConstString monitorContext;
public:
    /**
     * Default constructor.
     */
    explicit Searchable();

    /**
     * Destructor.
     */
    virtual ~Searchable();

    /**
     * Check if there exists a property of the given name
     * @param key the name to check for
     * @return true iff a property of the given name exists, even if
     * it doesn't have a value associated with it
     */
    virtual bool check(const ConstString& key) = 0;


    /**
     * Check if there exists a property of the given name
     * @param key the name to check for
     * @param comment Human-readable explanation
     * @return true iff a property of the given name exists, even if
     * it doesn't have a value associated with it
     */
    virtual bool check(const ConstString& key,
                       const ConstString& comment);

    /**
     * Gets a value corresponding to a given keyword
     * @param key The keyword to look for
     * @return A value corresponding to a given keyword.  If there is no
     * such value, then the isNull() method called on the result will be
     * true.  Otherwise, the value can be read by calling result.asInt(),
     * result.asString(), etc. as appropriate.
     */
    virtual Value& find(const ConstString& key) = 0;

    /**
     * Gets a list corresponding to a given keyword
     * @param key The keyword to look for
     * @return A list corresponding to a given keyword.  If there is no
     * such list, then the isNull() method called on the result will be
     * true.  Otherwise, the elements of the list can be read through
     * result.get(index) where result.get(0) is the keyword, and
     * result.get(i) for i>=1 are the "real" elements of the list.
     *
     */
    virtual Bottle& findGroup(const ConstString& key) = 0;

    /**
     * Gets a list corresponding to a given keyword
     * @param key The keyword to look for
     * @param comment Human-readable explanation
     * @return A list corresponding to a given keyword.  If there is no
     * such list, then the isNull() method called on the result will be
     * true.  Otherwise, the elements of the list can be read through
     * result.get(index) where result.get(0) is the keyword, and
     * result.get(i) for i>=1 are the "real" elements of the list.
     *
     */
    Bottle& findGroup(const ConstString& key, const ConstString& comment);

    /**
     * Gets a value corresponding to a given keyword.  If a property
     * does not exist, this returns false and does not modify the
     * result pointer.  If a property exists but does not have a
     * value, this again returns false and does not modify the result
     * pointer.
     *
     * \code
     * Property p;
     * p.fromString("(width 10) (height 15) (help)");
     * p.check("help")  // this is true
     * p.check("width") // this is true
     * p.check("foo")   // this is false
     * Value *v;
     * p.check("help",v)  // this is false, there is no value associated
     * p.check("width",v) // this is true, and v->asInt() is 10
     * \endcode
     *
     * @param key The keyword to look for
     * @param result A pointer to store the address of the result in
     * @param comment Optional human-readable explanation
     * @return True if there is a value corresponding to a given keyword,
     * false otherwise.  See the find() method for interpreting the
     * value found.
     */
    virtual bool check(const ConstString& key, Value *& result,
                       const ConstString& comment = "");


    /**
     * Gets a value corresponding to a given keyword
     * @param key The keyword to look for
     * @param fallback A default value to return if nothing found
     * @param comment Optional human-readable explanation
     * @return A value corresponding to a given keyword,
     * or the default if nothing is found.  See the find() method for
     * interpreting the value found.
     */
    virtual Value check(const ConstString& key, const Value& fallback,
                        const ConstString& comment = "");

    /**
     * Checks if the object is invalid.
     * @return True if the object is invalid or "null".
     */
    virtual bool isNull() const;

    /**
     * Return a standard text representation of the content of the
     * object.  The representation is readable by the Bottle and
     * Property classes.
     * @return A standard text representation of the content of the
     * object.
     */
    virtual ConstString toString() const = 0;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    virtual void setMonitor(SearchMonitor *monitor, const char *context="");
    virtual SearchMonitor *getMonitor();
    virtual ConstString getContext();
    virtual void reportToMonitor(const SearchReport& report);
#endif /* DOXYGEN_SHOULD_SKIP_THIS */
};


#endif
