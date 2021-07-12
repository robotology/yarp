/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_PROPERTY_H
#define YARP_OS_PROPERTY_H

#include <yarp/os/Bottle.h>
#include <yarp/os/Portable.h>
#include <yarp/os/Searchable.h>

#include <string>

namespace yarp {
namespace os {

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
class YARP_os_API Property :
        public Searchable,
        public Portable
{
public:
    using Searchable::check;
    using Searchable::findGroup;

    /**
     * Constructor.
     */
    Property();

#ifndef YARP_NO_DEPRECATED // Since YARP 3.3
    /**
     * Constructor.
     *
     * @param hash_size a scalar controlling efficiency of the
     * hash map storing the data.  Set to 0 for default size.
     * The bigger this number, the more memory used, but the
     * more efficient the map.
     *
     * @deprecated Since YARP 3.3
     */
    YARP_DEPRECATED_MSG("Use default constructor instead")
    Property(int hash_size);
#endif

    /**
     * Initialize from a string, using fromString().
     */
    Property(const char* str);


    /**
     * Copy constructor
     */
    Property(const Property& rhs);

    /**
     * Move constructor
     */
    Property(Property&& rhs) noexcept;

    /**
     * @brief Initializer list constructor.
     * @param[in] values, list of std::pair with which initialize the Property.
     */
    Property(std::initializer_list<std::pair<std::string, yarp::os::Value>> values);
    /**
     * Destructor.
     */
    ~Property() override;

    /**
     * Copy assignment operator.
     */
    Property& operator=(const Property& prop);

    /**
     * Move assignment operator.
     */
    Property& operator=(Property&& prop) noexcept;

    // documented in Searchable
    bool check(const std::string& key) const override;

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
    void put(const std::string& key, const std::string& value);

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
    void put(const std::string& key, const Value& value);

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
    void put(const std::string& key, Value* value);

    /**
     * \brief Associate the given \c key with the given integer.
     *
     * After the association <code>find(key).asInt32()</code> will return that
     * integer.
     * If \c key is already associated, the value will be replaced with the new
     * one.
     *
     * @param key the key
     * @param value the integer value
     */
    void put(const std::string& key, int value);

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
    void put(const std::string& key, double value);

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
    Property& addGroup(const std::string& key);

    /**
     * \brief Remove the association from the given \c key to a value, if
     *        present.
     *
     * Guarantees that <code>find(key).isNull()</code> will be true.
     *
     * @param key the key
     */
    void unput(const std::string& key);

    // documented in Searchable
    Value& find(const std::string& key) const override;

    // documented in Searchable
    Bottle& findGroup(const std::string& key) const override;

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
     *   prop.find("width").asInt32() // gives 10
     *   prop.find("height").asInt32() // gives 15
     * \endcode
     *
     * If a key is duplicated, only the latest will be used.
     * For example, with text = <code>(foo bar) (foo baz)</code>, the Property
     * object will be the mapping <code>{foo => baz}</code>.
     *
     * @param txt the textual form of the Property object
     * @param wipe should Property be emptied first
     */
    void fromString(const std::string& txt, bool wipe = true);

    /**
     * \brief Interprets a list of command arguments as a list of properties.
     *
     * Keys are named by beginning with \c \-\-.
     * For example, with argv =
     * <code>program_name \-\-width 10 \-\-height 15</code>, the Property object
     * will be the mapping <code>{width => 10, height => 15}</code>.
     * Therefore:
     * \code
     *   prop.find("width").asInt32() // gives 10
     *   prop.find("height").asInt32() // gives 15
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
    void fromCommand(int argc, char* argv[], bool skipFirst = true, bool wipe = true);

    /**
     * \brief Interprets a list of command arguments as a list of properties.
     *
     * Keys are named by beginning with \c \-\-.
     * For example, with argv =
     * <code>program_name \-\-width 10 \-\-height 15</code>, the Property object
     * will be the mapping <code>{width => 10, height => 15}</code>.
     * Therefore:
     * \code
     *   prop.find("width").asInt32() // gives 10
     *   prop.find("height").asInt32() // gives 15
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
    void fromCommand(int argc, const char* argv[], bool skipFirst = true, bool wipe = true);

    /**
     * \brief Interprets a list of command arguments as a list of properties.
     *
     * Keys are named by beginning with \c \-\-.
     * For example, with argv =
     * <code>program_name \-\-width 10 \-\-height 15</code>, the Property object
     * will be the mapping <code>{width => 10, height => 15}</code>.
     * Therefore:
     * \code
     *   prop.find("width").asInt32() // gives 10
     *   prop.find("height").asInt32() // gives 15
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
    void fromArguments(const char* arguments, bool wipe = true);

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
     *   prop.find("width").asInt32() // gives 10
     *   prop.find("height").asInt32() // gives 15
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
     *   prop.findGroup("SIZE").find("width").asInt32() // gives 10
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
    bool fromConfigFile(const std::string& fname, bool wipe = true);

    /**
     * \brief Variant of fromConfigFile(fname, wipe) that includes extra
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
    bool fromConfigFile(const std::string& fname,
                        Searchable& env,
                        bool wipe = true);

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
    bool fromConfigDir(const std::string& dirname,
                       const std::string& section = std::string(),
                       bool wipe = true);

    /**
     * \brief Parses text in the configuration format described in
     * fromConfigFile().
     *
     * @param txt the configuration text
     * @param wipe should Property be emptied first
     */
    void fromConfig(const char* txt, bool wipe = true);

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
    void fromConfig(const char* txt,
                    Searchable& env,
                    bool wipe = true);

    /**
     * \brief Parses text in a url.
     *
     * Anything of the form <code>foo=bar</code> is added as a property.
     *
     * @param url the text of the url, of form
     *            <code>...prop1=val1&prop2=val2...</code>
     * @param wipe should Property be emptied first
     */
    void fromQuery(const char* url,
                   bool wipe = true);


    // documented in Searchable
    std::string toString() const override;

    // documented in Portable
    bool read(ConnectionReader& reader) override;

    // documented in Portable
    bool write(ConnectionWriter& writer) const override;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
private:
    class Private;
    Private* mPriv;
#endif // DOXYGEN_SHOULD_SKIP_THIS
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_PROPERTY_H
