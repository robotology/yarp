// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef _YARP2_SEARCHABLE_
#define _YARP2_SEARCHABLE_

#include <yarp/os/ConstString.h>

namespace yarp {
    namespace os {
        class Value;
        class Bottle;
        class Searchable;
    }
}


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
class yarp::os::Searchable {
public:
    /**
     * Destructor.
     */
    virtual ~Searchable() {}

    /**
     * Check if there exists a property of the given name
     * @param key the name to check for
     * @return true iff a property of the given name exists, even if
     * it doesn't have a value associated with it
     */ 
    virtual bool check(const char *key) = 0;

    /**
     * Gets a value corresponding to a given keyword
     * @param key The keyword to look for
     * @return A value corresponding to a given keyword.  If there is no
     * such value, then the isNull() method called on the result will be
     * true.  Otherwise, the value can be read by calling result.asInt(),
     * result.asString(), etc. as appropriate.
     */
    virtual Value& find(const char *key) = 0;

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
    virtual Bottle& findGroup(const char *key) = 0;

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
     * @return True if there is a value corresponding to a given keyword,
     * false otherwise.  See the find() method for interpreting the
     * value found.
     */
    bool check(const char *key, Value *& result);


    /**
     * Gets a value corresponding to a given keyword
     * @param key The keyword to look for
     * @param fallback A default value to return if nothing found
     * @return A value corresponding to a given keyword,
     * or the default if nothing is found.  See the find() method for 
     * interpreting the value found.
     */
    Value check(const char *key, const Value& fallback);

    /**
     * Checks if the object is invalid.
     * @return True if the object is invalid or "null".
     */
    virtual bool isNull() const  { return false; }

    /**
     * Return a standard text representation of the content of the
     * object.  The representation is readable by the Bottle and 
     * Property classes.
     * @return A standard text representation of the content of the
     * object.  
     */
    virtual ConstString toString() const = 0;
};

#endif
