// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
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
class yarp::os::Property : public Searchable, public Portable {

public:
    using Searchable::check;

    Property();

    virtual ~Property();

    /**
     * Check if there exists a property of the given name
     * @param key the name to check for
     * @return true iff a property of the given name exists
     */ 
    bool check(const char *key) const;

    void put(const char *key, const char *val);

    void put(const char *key, Value& value);

    void put(const char *key, Value *value);

    void put(const char *key, int v) {
        put(key,Value::makeInt(v));
    }

    void put(const char *key, double v) {
        put(key,Value::makeDouble(v));
    }

    void unput(const char *key);

    // documented in Searchable
    virtual Value& find(const char *key);

    // documented in Searchable
    virtual Bottle& findGroup(const char *key);

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
     */
    void fromString(const char *txt);
    
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
     */
    void fromCommand(int argc, char *argv[], bool skipFirst=true);

    /**
     * Interprets a file as a list of properties. 
     * For example, for a file containing two lines,
     * "width 10" and "height 15", the Property object
     * will be the mapping {width => 10, height => 15}.
     * Lines of the form "[NAME]" will result in nested subproperties.
     * For example, with file content:
     * \code
     * [SIZE]
     * width 10
     * height 15
     * [APPEARANCE]
     * color red
     * \endcode
     * the structure of the Property object will be 
     * "(SIZE (width 10) (height 15)) (APPEARANCE (color red))".
     * So, for example:
     * \code
     *   prop.findGroup("SIZE").find("width").asInt() // gives 10
     *   prop.findGroup("APPEARANCE").find("color").asString() // gives red
     * \endcode
     * @param fname the name of the file to read from
     */
    void fromConfigFile(const char *fname);

    void fromConfig(const char *txt);

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

