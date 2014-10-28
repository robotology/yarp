// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_PROPERTY_
#define _YARP2_PROPERTY_

#include <yarp/os/ConstString.h>
#include <yarp/os/Bottle.h>

namespace yarp {
    namespace os {
        class Property;
    }
}


/**
 * \ingroup key_class
 *
 *
 * A class for storing options and configuration information.
 * Use put() to add keyword/value pairs, and get() or check()
 * to look them up afterwards.
 * It can
 * read from configuration files using the fromConfigFile() method,
 * and from command line options using the fromCommand() method, and
 * from any Searchable object (include Bottle objects) using the
 * fromString() method.
 * Property objects can be searched efficiently.
 *
 */
class YARP_OS_API yarp::os::Property : public Searchable, public Portable {

public:
    using Searchable::check;
    using Searchable::findGroup;

    /**
     * Constructor.
     *
     * @param hash_size a scalar controlling efficiency of the
     * hash map storing the data.  Set to 0 for default size.
     * The bigger this number, the more memory used, but the
     * more efficient the map.
     *
     */
    Property(int hash_size = 0);

    /**
     * Initialize from a string, using fromString().
     */
    Property(const char *str);


    /**
     * Copy constructor
     */
    Property(const Property& prop);

    /**
     * Destructor.
     */
    virtual ~Property();

    /**
     * Assignment.
     */
    const Property& operator = (const Property& prop);

    // documented in Searchable
    bool check(const ConstString& key) const;

    /**
     * Associate the given key with the given string, so that
     * find(key).asString() will give that string.
     * @param key the key
     * @param value the string value
     */
    void put(const ConstString& key, const ConstString& value);

    /**
     * Associate the given key with the given value, so that
     * find(key).asString() will give that value.
     * @param key the key
     * @param value the value
     */
    void put(const ConstString& key, const Value& value);

    /**
     * Associate the given key with the given value, so that
     * find(key) will give that value.  The Property
     * object is responsible for deallocating the value.
     * @param key the key
     * @param value the value
     */
    void put(const ConstString& key, Value *value);

    /**
     * Associate the given key with the given integer, so that
     * find(key).asInt() will give that integer.
     * @param key the key
     * @param v the integer value
     */
    void put(const ConstString& key, int v);

    /**
     * Associate the given key with the given floating point number, so that
     * find(key).asDouble() will give that number.
     * @param key the key
     * @param v the floating point value
     */
    void put(const ConstString& key, double v);

    /**
     *
     * Add a nested group. Careful: the group object returned is
     * valid only until the next read from the Property it was added
     * to.  As soon as there is a request for data from that Property,
     * then it ceases to be usable.
     *
     * @param key the key
     * @return the nested group, represented as a Property
     *
     */
    Property& addGroup(const ConstString& key);

    /**
     * Remove the association from the given key to a value, if present.
     * Guarantees that find(key).isNull() will be true.
     * @param key the key
     */
    void unput(const ConstString& key);

    // documented in Searchable
    virtual Value& find(const ConstString& key) const;

    // documented in Searchable
    virtual Bottle& findGroup(const ConstString& key) const;

    /**
     * Remove all associations.
     * Guarantees that find(key).isNull() will be true for all values of key.
     */
    void clear();

    /**
     * Interprets a string as a list of properties. Uses the the same
     * format as a Bottle.  The first element of every sub-list
     * is interpreted as a key.  For example, with
     * text = "(width 10) (height 15)", the Property object
     * will be the mapping {width => 10, height => 15}.  So:
     * \code
     *   prop.find("width").asInt() // gives 10
     *   prop.find("height").asInt() // gives 15
     * \endcode
     * @param txt the textual form of the Property object
     * @param wipe should Property be emptied first
     */
    void fromString(const ConstString& txt, bool wipe=true);

    /**
     * Interprets a list of command arguments as a list of properties.
     * Keys are named by beginning with "--".  For example, with
     * argv = "program_name --width 10 --height 15", the Property object
     * will be the mapping {width => 10, height => 15}.  So:
     * \code
     *   prop.find("width").asInt() // gives 10
     *   prop.find("height").asInt() // gives 15
     * \endcode
     * @param argc the number of arguments
     * @param argv the list of arguments
     * @param skipFirst set to true if the first argument should be skipped
     * (which is the right thing to do for arguments passed to main())
     * @param wipe should Property be emptied first
     */
    void fromCommand(int argc, char *argv[], bool skipFirst=true,
                     bool wipe=true);

    void fromCommand(int argc, const char *argv[], bool skipFirst=true,
                     bool wipe=true);

    /**
     * Interprets a file as a list of properties.
     * For example, for a file containing:
     * \code
     *   width 10
     *   height 15
     * \endcode
     * the Property object will be the mapping  {width => 10, height => 15}.
     * In other words:
     * \code
     *   prop.find("width").asInt() // gives 10
     *   prop.find("height").asInt() // gives 15
     * \endcode
     * Lines of the form "[NAME]" will result in nested subproperties.
     * For example, for a file containing:
     * \code
     *   [SIZE]
     *   width 10
     *   height 15
     *   [APPEARANCE]
     *   color red
     * \endcode
     * the structure of the Property object will be
     * "(SIZE (width 10) (height 15)) (APPEARANCE (color red))".
     * In other words:
     * \code
     *   prop.findGroup("SIZE").find("width").asInt() // gives 10
     *   prop.findGroup("APPEARANCE").find("color").asString() // gives red
     * \endcode
     *
     * It is possible to nest configuration files.  To include
     * the configuration file "preamble.ini" inside another one,
     * do:
     * \code
     *   [include "preamble.ini"]
     * \endcode
     * This will insert the content of preamble.ini as if cut-and-pasted.
     * If rather you would like the content included within a subsection
     * called FOO, do instead:
     * \code
     *   [include FOO "preamble.ini"]
     * \endcode
     *
     * @param fname the name of the file to read from
     * @param wipe should Property be emptied first
     * @return true if file exists and can be read
     */
    bool fromConfigFile(const ConstString& fname, bool wipe=true);

    /**
     * Variant of fromConfigFile(fname,wipe) that includes extra
     * "environment variables".  These will be expanded, along
     * with any other variables in the environment, if present
     * in the configuration file in $variable or ${variable} form.
     * @param fname the name of the file to read from
     * @param env extra set of environment variables
     * @param wipe should Property be emptied first
     * @return true if file exists and can be read
     */
    bool fromConfigFile(const ConstString& fname,
                        Searchable& env,
                        bool wipe=true);

    /**
     * Parses text in the configuration format described in
     * fromConfigFile().
     * @param txt the configuration text
     * @param wipe should Property be emptied first
     */
    void fromConfig(const char *txt, bool wipe=true);


    /**
     * Variant of fromConfig(txt,wipe) that includes extra
     * "environment variables".  These will be expanded, along
     * with any other variables in the environment, if present
     * in the configuration file in $variable or ${variable} form.
     * @param txt the configuration text
     * @param env extra set of environment variables
     * @param wipe should Property be emptied first
     */
    void fromConfig(const char *txt,
                    Searchable& env,
                    bool wipe=true);


    /**
     * Parses text in a url, adding anything of the form foo=bar
     * as a property
     * @param url the text of the url, of form ...prop1=val1&prop2=val2...
     * @param wipe should Property be emptied first
     */
    void fromQuery(const char *url,
                   bool wipe=true);


    // documented in Searchable
    ConstString toString() const;

    // documented in Portable
    bool read(ConnectionReader& connection);

    // documented in Portable
    bool write(ConnectionWriter& connection);

private:
    void *implementation;
};

#endif

