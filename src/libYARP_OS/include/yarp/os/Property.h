/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_PROPERTY_H
#define YARP_OS_PROPERTY_H

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
 * \brief A class for storing options and configuration information.
 *
 * Use put() to add keyword/value pairs, and get() or check() to look them up
 * afterwards.
 * It can read from configuration files using the fromConfigFile() method, and
 * from command line options using the fromCommand() method, and from any
 * Searchable object (include Bottle objects) using the fromString() method.
 * Property objects can be searched efficiently.
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
     * Assignment operator.
     */
    const Property& operator=(const Property& prop);

    // documented in Searchable
    bool check(const ConstString& key) const;

    /**
     * \brief Associate the given \c key with the given string.
     *
     * After the association <code>find(key).asString()</code> will return that
     * string.
     * If \c key is already associated, the value will be replaced with the new
     * one.
     *
     * @param key the key
     * @param value the string value
     */
    void put(const ConstString& key, const ConstString& value);

    /**
     * \brief Associate the given \c key with the given value
     *
     * After the association <code>find(key).asString()</code> will return that
     * value.
     * If \c key is already associated, the value will be replaced with the new
     * one.
     *
     * @param key the key
     * @param value the value
     */
    void put(const ConstString& key, const Value& value);

    /**
     * \brief Associate the given \c key with the given value.
     *
     * After the association <code>find(key)</code> will return that value.
     * The Property object is responsible for deallocating the value.
     * If \c key is already associated, the value will be replaced with the new
     * one.
     *
     * @param key the key
     * @param value the value
     */
    void put(const ConstString& key, Value *value);

    /**
     * \brief Associate the given \c key with the given integer.
     *
     * After the association <code>find(key).asInt()</code> will return that
     * integer.
     * If \c key is already associated, the value will be replaced with the new
     * one.
     *
     * @param key the key
     * @param value the integer value
     */
    void put(const ConstString& key, int value);

    /**
     * \brief Associate the given \c key with the given floating point number
     *
     * After the association <code>find(key).asDouble()</code> will return that
     * number.
     * If \c key is already associated, the value will be replaced with the new
     * one.
     *
     * @param key the key
     * @param value the floating point value
     */
    void put(const ConstString& key, double value);

    /**
     * \brief Add a nested group.
     *
     * \warning the group object returned is valid only until the next read from
     * the Property it was added to. As soon as there is a request for data from
     * that Property, then it ceases to be usable.
     *
     * @param key the key
     * @return the nested group, represented as a Property
     */
    Property& addGroup(const ConstString& key);

    /**
     * \brief Remove the association from the given \c key to a value, if
     *        present.
     *
     * Guarantees that <code>find(key).isNull()</code> will be true.
     *
     * @param key the key
     */
    void unput(const ConstString& key);

    // documented in Searchable
    virtual Value& find(const ConstString& key) const;

    // documented in Searchable
    virtual Bottle& findGroup(const ConstString& key) const;

    /**
     * \brief Remove all associations.
     *
     * Guarantees that <code>find(key).isNull()</code> will be true for all
     * values of \c key.
     */
    void clear();

    /**
     * \brief Interprets a string as a list of properties.
     *
     * Uses the the same format as a Bottle.
     * The first element of every sub-list is interpreted as a key.
     * For example, with text = <code>(width 10) (height 15)</code>, the
     * Property object will be the mapping
     * <code>{width => 10, height => 15}</code>.
     * Therefore:
     * \code
     *   prop.find("width").asInt() // gives 10
     *   prop.find("height").asInt() // gives 15
     * \endcode
     *
     * If a key is duplicated, only the latest will be used.
     * For example, with text = <code>(foo bar) (foo baz)</code>, the Property
     * object will be the mapping <code>{foo => baz}</code>.
     *
     * @param txt the textual form of the Property object
     * @param wipe should Property be emptied first
     */
    void fromString(const ConstString& txt, bool wipe=true);

    /**
     * \brief Interprets a list of command arguments as a list of properties.
     *
     * Keys are named by beginning with \c \-\-.
     * For example, with argv =
     * <code>program_name \-\-width 10 \-\-height 15</code>, the Property object
     * will be the mapping <code>{width => 10, height => 15}</code>.
     * Therefore:
     * \code
     *   prop.find("width").asInt() // gives 10
     *   prop.find("height").asInt() // gives 15
     * \endcode
     *
     * If a key is duplicated, only the latest will be used.
     * For example, with argv =
     * <code>program_name \-\-foo bar \-\-foo baz</code>, the Property object
     * will be the mapping <code>{foo => baz}</code>.
     *
     * @param argc the number of arguments
     * @param argv the list of arguments
     * @param skipFirst set to true if the first argument should be skipped
     * (which is the right thing to do for arguments passed to main())
     * @param wipe should Property be emptied first
     */
    void fromCommand(int argc, char *argv[], bool skipFirst=true,
                     bool wipe=true);

    /**
     * \brief Interprets a list of command arguments as a list of properties.
     *
     * Keys are named by beginning with \c \-\-.
     * For example, with argv =
     * <code>program_name \-\-width 10 \-\-height 15</code>, the Property object
     * will be the mapping <code>{width => 10, height => 15}</code>.
     * Therefore:
     * \code
     *   prop.find("width").asInt() // gives 10
     *   prop.find("height").asInt() // gives 15
     * \endcode
     *
     * If a key is duplicated, only the latest will be used.
     * For example, with argv =
     * <code>program_name \-\-foo bar \-\-foo baz</code>, the Property object
     * will be the mapping <code>{foo => baz}</code>.
     *
     * @param argc the number of arguments
     * @param argv the list of arguments
     * @param skipFirst set to true if the first argument should be skipped
     * (which is the right thing to do for arguments passed to main())
     * @param wipe should Property be emptied first
     */
    void fromCommand(int argc, const char *argv[], bool skipFirst=true,
                     bool wipe=true);

    /**
     * \brief Interprets a list of command arguments as a list of properties.
     *
     * Keys are named by beginning with \c \-\-.
     * For example, with argv =
     * <code>program_name \-\-width 10 \-\-height 15</code>, the Property object
     * will be the mapping <code>{width => 10, height => 15}</code>.
     * Therefore:
     * \code
     *   prop.find("width").asInt() // gives 10
     *   prop.find("height").asInt() // gives 15
     * \endcode
     *
     * If a key is duplicated, only the latest will be used.
     * For example, with argv =
     * <code>program_name \-\-foo bar \-\-foo baz</code>, the Property object
     * will be the mapping {foo => baz}.
     *
     * @param arguments the command arguments
     * @param wipe should Property be emptied first
     */
    void fromArguments(const char *arguments, bool wipe=true);

    /**
     * \brief Interprets a file as a list of properties.
     *
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
     *
     * If a key is duplicated, only the latest will be used.
     * For example, for a file containing:
     * \code
     *   foo bar
     *   foo baz
     * \endcode
     *
     * the Property object will be the mapping {foo => baz}.
     *
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
     * \warning If a key in a group is duplicated, they will be both included as
     * the value for the group, since the value will be a Bottle, that can
     * include duplicate values. In this case, the Value returned by find(),
     * will be the first and not the latter.
     * For example, for a file containing:
     * \code
     * [GROUP]
     * foo bar
     * foo baz
     * \endcode
     * the structure of the Property object will be
     * "(GROUP (foo bar) (foo baz))".
     * Therefore
     * \code
     * prop.findGroup("GROUP").toString() // gives "(foo bar) (foo baz)"
     * prop.findGroup("GROUP").find("foo").asString(); // gives "bar", and NOT "baz"
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
     * \brief Variant of fromConfigFile(fname,wipe) that includes extra
     * "environment variables".
     *
     * These will be expanded, along with any other variables in the
     * environment, if present in the configuration file in
     * <code>$variable</code> or <code>${variable}</code> form.
     *
     * @param fname the name of the file to read from
     * @param env extra set of environment variables
     * @param wipe should Property be emptied first
     * @return true if file exists and can be read
     */
    bool fromConfigFile(const ConstString& fname,
                        Searchable& env,
                        bool wipe=true);

    /**
     * \brief Interprets all files in a directory as lists of properties as
     * described in fromConfigFile().
     *
     * @param dirname the name of the file to read from
     * @param section if specified names the subsection for each file,
     * otherwise use the same section for all files
     * @param wipe should Property be emptied first
     * @return true if file exists and can be read
     */
    bool fromConfigDir(const ConstString& dirname,
                       const ConstString& section = ConstString(),
                       bool wipe = true);

    /**
     * \brief Parses text in the configuration format described in
     * fromConfigFile().
     *
     * @param txt the configuration text
     * @param wipe should Property be emptied first
     */
    void fromConfig(const char *txt, bool wipe=true);

    /**
     * Variant of fromConfig(const char*, bool) that includes extra
     * "environment variables".
     *
     * These will be expanded, along with any other variables in the
     * environment, if present in the configuration file in
     * <code>$variable<code> or <code>${variable}</code> form.
     *
     * @param txt the configuration text
     * @param env extra set of environment variables
     * @param wipe should Property be emptied first
     */
    void fromConfig(const char *txt,
                    Searchable& env,
                    bool wipe=true);

    /**
     * \brief Parses text in a url.
     *
     * Anything of the form <code>foo=bar</code> is added as a property.
     *
     * @param url the text of the url, of form
     *            <code>...prop1=val1&prop2=val2...</code>
     * @param wipe should Property be emptied first
     */
    void fromQuery(const char *url,
                   bool wipe=true);


    // documented in Searchable
    ConstString toString() const;

    // documented in Portable
    bool read(ConnectionReader& reader);

    // documented in Portable
    bool write(ConnectionWriter& writer);

private:
    void *implementation;
    int hash_size;

    void summon();
    bool check() const;
};

#endif // YARP_OS_PROPERTY_H
