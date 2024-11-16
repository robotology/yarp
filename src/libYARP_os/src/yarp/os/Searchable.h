/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_SEARCHABLE_H
#define YARP_OS_SEARCHABLE_H

#include <yarp/os/api.h>

#include <cstddef>
#include <string>

namespace yarp::os {
class Value;
class Bottle;
} // namespace yarp::os

namespace yarp::os {

/**
 * A base class for nested structures that can be searched.
 * A Searchable object promises that you can look inside it
 * with the find() and findGroup() methods to get values and
 * lists corresponding to keywords.
 *
 * @see Property Bottle Value
 */
class YARP_os_API Searchable
{
public:
    /**
     * Default constructor.
     */
    explicit Searchable();

    /**
     * Copy constructor
     */
    Searchable(const Searchable& rhs) = default;

    /**
     * Move constructor
     */
    Searchable(Searchable&& rhs) noexcept = default;

    /**
     * Destructor.
     */
    virtual ~Searchable();

    /**
     * Copy assignment operator.
     */
    Searchable& operator=(const Searchable& rhs) = default;

    /**
     * Move assignment operator.
     */
    Searchable& operator=(Searchable&& rhs) noexcept = default;

    /**
     * Check if there exists a property of the given name
     * @param key the name to check for
     * @return true iff a property of the given name exists, even if
     * it doesn't have a value associated with it
     */
    virtual bool check(const std::string& key) const = 0;


    /**
     * Check if there exists a property of the given name
     * @param key the name to check for
     * @param comment Human-readable explanation
     * @return true iff a property of the given name exists, even if
     * it doesn't have a value associated with it
     */
    virtual bool check(const std::string& key,
                       const std::string& comment) const;

    /**
     * Gets a value corresponding to a given keyword
     * @param key The keyword to look for
     * @return A value corresponding to a given keyword.  If there is no
     * such value, then the isNull() method called on the result will be
     * true.  Otherwise, the value can be read by calling result.asInt32(),
     * result.asString(), etc. as appropriate.
     */
    virtual Value& find(const std::string& key) const = 0;

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
    virtual Bottle& findGroup(const std::string& key) const = 0;

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
    Bottle& findGroup(const std::string& key, const std::string& comment) const;

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
     * p.check("help", v)  // this is false, there is no value associated
     * p.check("width", v) // this is true, and v->asInt32() is 10
     * \endcode
     *
     * @param key The keyword to look for
     * @param result A pointer to store the address of the result in
     * @param comment Optional human-readable explanation
     * @return True if there is a value corresponding to a given keyword,
     * false otherwise.  See the find() method for interpreting the
     * value found.
     */
    virtual bool check(const std::string& key,
                       Value*& result,
                       const std::string& comment = "") const;


    /**
     * Gets a value corresponding to a given keyword
     * @param key The keyword to look for
     * @param fallback A default value to return if nothing found
     * @param comment Optional human-readable explanation
     * @return A value corresponding to a given keyword,
     * or the default if nothing is found.  See the find() method for
     * interpreting the value found.
     */
    virtual Value check(const std::string& key,
                        const Value& fallback,
                        const std::string& comment = "") const;

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
    virtual std::string toString() const = 0;
};

} // namespace yarp::os

#endif // YARP_OS_SEARCHABLE_H
