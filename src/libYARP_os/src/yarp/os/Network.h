/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_NETWORK_H
#define YARP_OS_NETWORK_H

#include <yarp/os/Contact.h>
#include <yarp/os/ContactStyle.h>
#include <yarp/os/NameStore.h>
#include <yarp/os/Portable.h>
#include <yarp/os/Property.h>
#include <yarp/os/QosStyle.h>
#include <yarp/os/Time.h>
#include <yarp/os/Value.h>


namespace yarp {
namespace os {
class ContactStyle;
class QosStyle;
} // namespace os
} // namespace yarp

// Make plugins in a library available for use
#define YARP_DECLARE_PLUGINS(name) extern "C" void add_ ## name ## _plugins();
#define YARP_REGISTER_PLUGINS(name) add_ ## name ## _plugins();

namespace yarp {
namespace os {

/**
 * \ingroup comm_class
 *
 * Utilities for manipulating the YARP network, excluding initialization
 * and shutdown.
 */
class YARP_os_API NetworkBase
{
public:
    /**
     * Basic system initialization, not including plugins.
     * Must eventually make a matching call to finiMinimum().
     */
    static void initMinimum();

    /**
     * Basic system initialization, not including plugins,
     * The clock to be initialized can be specified.
     * Must eventually make a matching call to finiMinimum().
     */
    static void initMinimum(yarp::os::yarpClockType clockType,
                            yarp::os::Clock* custom = nullptr);

    /**
     * Basic system initialization, not including plugins.
     * A matching finiMinimum() will be called automatically
     * on program termination.
     */
    static void autoInitMinimum();

    /**
     * Basic system initialization, not including plugins.
     * The clock to be initialized can be specified.
     * A matching finiMinimum() will be called automatically
     * on program termination.
     */
    static void autoInitMinimum(yarp::os::yarpClockType clockType,
                                yarp::os::Clock* custom = nullptr);

    /**
     * This function specifically initialize the clock
     * In case clockType is one of the valid cases:
     *      YARP_CLOCK_SYSTEM,
     *      YARP_CLOCK_NETWORK,
     *      YARP_CLOCK_CUSTOM
     * (see yarp::os::Time for more), the corresponding clock will be initialized.
     *
     * In case the clockType is YARP_CLOCK_DEFAULT, the environment variable
     * YARP_CLOCK will be used to choose between system or network clock.
     *
     * See description of yarp::os::Time::useNetworkClock() for more details about the
     * network clock.
     *
     * This function is called by Network constructor and by Network::init(),
     * and Network::initMinimum().
     *
     * In case of failure calls YARP_FAIL assert.
     **/
    static void yarpClockInit(yarp::os::yarpClockType clockType,
                              Clock* custom = nullptr);

    /**
     * Deinitialization, excluding plugins.
     */
    static void finiMinimum();

    /*
     * Return true if network is initialized.
     * Expect problem if you use things that require a network before
     * initializing it.
     */
    static bool isNetworkInitialized();

    /**
     * Request that an output port connect to an input port.
     * @param src the name of an output port
     * @param dest the name of an input port
     * @param carrier the name of the protocol to use (tcp/udp/mcast)
     * @param quiet suppress messages displayed upon success/failure
     * @return true on success, false on failure
     */
    static bool connect(const std::string& src,
                        const std::string& dest,
                        const std::string& carrier = "",
                        bool quiet = true);

    // Catch old uses of nullptr for carrier
    static bool connect(const char* src,
                        const char* dest,
                        const char* carrier,
                        bool quiet = true)
    {
        return connect(std::string(src),
                       std::string(dest),
                       std::string((carrier == nullptr) ? "" : carrier),
                       quiet);
    }

    /**
     * Request that an output port connect to an input port.
     * @param src the name of an output port
     * @param dest the name of an input port
     * @param style options for connection
     * @return true on success, false on failure
     */
    static bool connect(const std::string& src,
                        const std::string& dest,
                        const ContactStyle& style);

    /**
     * Request that an output port disconnect from an input port.
     * @param src the name of an output port
     * @param dest the name of an input port
     * @param quiet suppress messages displayed upon success/failure
     * @return true on success, false on failure
     */
    static bool disconnect(const std::string& src,
                           const std::string& dest,
                           bool quiet);

    /**
     * Request that an output port disconnect from an input port.
     * @param src the name of an output port
     * @param dest the name of an input port
     * @param style options for network communication related to disconnection
     * @return true on success, false on failure
     */
    static bool disconnect(const std::string& src,
                           const std::string& dest,
                           const ContactStyle& style);

    /**
     * Request that an output port disconnect from an input port.
     * @param src the name of an output port
     * @param dest the name of an input port
     * @param carrier the name of the protocol to use (tcp/udp/mcast)
     * @param quiet suppress messages displayed upon success/failure
     * @return true on success, false on failure
     */
    static bool disconnect(const std::string& src,
                        const std::string& dest,
                        const std::string& carrier = "",
                        bool quiet = true);

    // Catch old uses of nullptr for carrier
    static bool disconnect(const char* src,
                        const char* dest,
                        const char* carrier,
                        bool quiet = true)
    {
        return disconnect(std::string(src),
                       std::string(dest),
                       std::string((carrier == nullptr) ? "" : carrier),
                       quiet);
    }

    /**
     * Check if a connection exists between two ports.
     * @param src the name of an output port
     * @param dest the name of an input port
     * @param quiet suppress messages displayed upon success/failure
     * @return true if there is a connection
     */
    static bool isConnected(const std::string& src,
                            const std::string& dest,
                            bool quiet);

    /**
     * Check if a connection exists between two ports.
     * @param src the name of an output port
     * @param dest the name of an input port
     * @param style options for network communication
     * @return true if there is a connection
     */
    static bool isConnected(const std::string& src,
                            const std::string& dest,
                            const ContactStyle& style);

    /**
     * Check if a connection exists between two ports.
     * @param src the name of an output port
     * @param dest the name of an input port
     * @param carrier the name of the protocol to use (tcp/udp/mcast)
     * @param quiet suppress messages displayed upon success/failure
     * @return true if there is a connection
     */
    static bool isConnected(const std::string& src,
                        const std::string& dest,
                        const std::string& carrier = "",
                        bool quiet = true);

    // Catch old uses of nullptr for carrier
    static bool isConnected(const char* src,
                        const char* dest,
                        const char* carrier,
                        bool quiet = true)
    {
        return isConnected(std::string(src),
                       std::string(dest),
                       std::string((carrier == nullptr) ? "" : carrier),
                       quiet);
    }

    /**
     * Check for a port to be ready and responsive.
     * @param port the name of a port
     * @param quiet suppress messages displayed during check
     * @return true on success, false on failure
     */
    static bool exists(const std::string& port,
                       bool quiet = true,
                       bool checkVer = true);

    /**
     * Check for a port to be ready and responsive.
     * @param port the name of a port
     * @param style options for network communication
     * @return true on success, false on failure
     */
    static bool exists(const std::string& port,
                       const ContactStyle& style,
                       bool checkVer = true);

    /**
     * Wait for a port to be ready and responsive.
     * @param port the name of a port
     * @param quiet suppress messages displayed during wait
     * @return true on success, false on failure
     */
    static bool sync(const std::string& port,
                     bool quiet = true);

    /**
     * An assertion.  Should be true.  If false, this will be
     * reported, and YARP will shut down.  In general, this is an
     * OS-specific process.
     * @param shouldBeTrue the asserted truth value.
     */
    static void assertion(bool shouldBeTrue);

    /**
     * Find out information about a registered name.
     * This communicates with the name server to find out
     * what is known about how to contact the entity with the given name
     * (if one exists).
     * @param name the name to query
     * @return full contact information for the name.  If nothing is
     * known about the name, the returned contact is invalid
     * (Contact::isValid returns false)
     */
    static Contact queryName(const std::string& name);

    /**
     * Register a name with the name server.
     * The name server will allocate a way to contact that name.
     * It is up to you to make sure that this works.
     * @param name the name to register
     * @return the contact information now associated with that name
     * (in other words, what Contact::queryName would now return)
     */
    static Contact registerName(const std::string& name);

    /**
     * Register contact information with the name server.
     * The name server will fill in any extra information needed
     * to make the contact information complete..
     * @param contact the proposed contact information (may be incomplete)
     * @return the contact information now associated with a name
     * (in other words, what Contact::queryName would now return)
     */
    static Contact registerContact(const Contact& contact);

    /**
     * Removes the registration for a name from the name server.
     * @param name the name to unregister
     * @return the contact information now associated with that name
     * (in other words, what Contact::queryName would now return).
     * This will be the invalid contact (Contact::isValid is false).
     */
    static Contact unregisterName(const std::string& name);

    /**
     * Removes the registration for a contact from the name server.
     * @param contact the contact to unregister
     * @return the contact information now associated with its former name
     * (in other words, what Contact::queryName would now return).
     * This will be the invalid contact (Contact::isValid is false).
     */
    static Contact unregisterContact(const Contact& contact);


    /**
     * Names registered with the nameserver can have arbitrary
     * key->value properties associated with them.
     * This method sets a value associated with a particular key
     * for a named entry (typically a port name).
     * @param name The name registered with the nameserver (typically a port).
     * @param key The key to provide a value for.
     * @param value The value associated with the key for the named entry.
     * @return true on success.
     */
    static bool setProperty(const char* name,
                            const char* key,
                            const Value& value);


    /**
     * Look up the value associated with a particular key for a named
     * entry registered with the nameserver.
     * @param name The name registered with the nameserver (typically a port).
     * @param key The key to provide a value for.
     * @return the value associated with the given key.
     */
    static Value* getProperty(const char* name,
                              const char* key);


    /**
     * Get the name of the port associated with the nameserver (usually
     * "/root", but this can be overwritten by the "yarp namespace"
     * command).
     * @return name of the port associated with the nameserver
     */
    static std::string getNameServerName();

    /**
     * Get the contact information for the port associated with the nameserver
     * (usually "/root", but this can be overwritten by the "yarp namespace"
     * command).
     * @return contact informatoin for the port associated with the nameserver
     */
    static Contact getNameServerContact();


    /**
     * Set the name of the port associated with the nameserver (usually
     * "/root", but this can be overwritten by the "yarp namespace"
     * command).  This method is not thread-safe.  Do not call this command
     * while ports are being registered/unregistered or connections are
     * being made/broken in another thread.
     * @return true on success
     */
    static bool setNameServerName(const std::string& name);


    /**
     * Chooses whether communication is process-local.
     * Call this with flag=true to avoid ever consulting an
     * external nameserver.
     * @param flag true if communication should be local to the calling process.
     * @return prior state of this flag.
     */
    static bool setLocalMode(bool flag);

    /**
     * Get current value of flag "localMode", see setLocalMode function.
     * @return state of the flag.
     */
    static bool getLocalMode();

#ifndef YARP_NO_DEPRECATED // Since YARP 3.0.0
    /**
     * Read a line of arbitrary length from standard input.
     *
     * @param eof If non-null, this is set to true if standard input has
     * closed.  Note that some heuristics are used to guess if someone
     * on windows has hit the equivalent of ctrl-D, the key to close
     * a stream on unix.  These heuristics will lead to false detects
     * in some cases if the user hits strange keys.
     *
     * @return A string from standard input, without newline or
     * linefeed characters.
     *
     * @deprecated since YARP 3.0.0
     */
    YARP_DEPRECATED
    static std::string readString(bool* eof = nullptr);
#endif // YARP_NO_DEPRECATED


    /**
     *
     * Send a single command to a port and await a single response.
     * Similar to the "yarp rpc" command line utility.
     * If you want to send several such commands, you'd be better off
     * making a port and using its methods for writing with replies.
     *
     * @param contact the target to communicate with
     * @param cmd the message to send
     * @param reply the response is read here
     * @param admin true for administrative message, false for regular data
     * @param quiet true to suppress error messages
     * @param timeout activity timeout in seconds
     *
     * @return true on success
     */
    static bool write(const Contact& contact,
                      PortWriter& cmd,
                      PortReader& reply,
                      bool admin = false,
                      bool quiet = false,
                      double timeout = -1);

    /**
     * Variant write method with options bundled into a
     * yarp::os::ContactStyle (there was getting to be too many of
     * them).
     *
     * @param contact the target to communicate with
     * @param cmd the message to send
     * @param reply the response if any is read here
     * @param style options for the connection
     *
     * @return true on success
     */
    static bool write(const Contact& contact,
                      PortWriter& cmd,
                      PortReader& reply,
                      const ContactStyle& style);

    /**
     * Variant write method specialized to name server.
     *
     * @param cmd the message to send
     * @param reply the response if any is read here
     * @param style options for the connection
     *
     * @return true on success
     */
    static bool writeToNameServer(PortWriter& cmd,
                                  PortReader& reply,
                                  const ContactStyle& style);

    /**
     *
     * Variant write method with port name specified directly.
     *
     * @param port_name the target to communicate with
     * @param cmd the message to send
     * @param reply the response is read here
     *
     * @return true on success
     */
    static bool write(const std::string& port_name,
                      PortWriter& cmd,
                      PortReader& reply);


    /**
     *
     * Check if the YARP Network is up and running.  Basically,
     * checks if a yarp name server is running and responding.
     *
     * @return true if the YARP Network is active.
     *
     */
    static bool checkNetwork();


    /**
     *
     * Check if a name server is running and responding.
     *
     * @param timeout time in seconds to wait for a response from a
     * name server.
     *
     * @return true if the name server responds within the given time.
     *
     */
    static bool checkNetwork(double timeout);

    /**
     *
     * Returns true if YARP has been fully initialized.
     *
     * @return true if YARP has been initialized (by creating a
     * yarp::os::Network object or calling yarp::os::Network::init).
     *
     */
    static bool initialized();

#ifndef YARP_NO_DEPRECATED // Since YARP 3.4
    /**
     * Set level of verbosity of YARP messages.
     *
     * @param verbosity -1 inhibits messages, 0 is normal, 1 is verbose
     * @deprecated since YARP 3.4
     */
    YARP_DEPRECATED_MSG("Use LogComponents instead")
    static void setVerbosity(int verbosity);
#endif // YARP_NO_DEPRECATED

    /**
     *
     * Redirect queries to another source.
     *
     */
    static void queryBypass(NameStore* store);

    static NameStore* getQueryBypass();

#ifndef YARP_NO_DEPRECATED // Since YARP 3.4
    /**
     * Read a variable from the environment.
     *
     * @param key the variable to read
     * @param found an optional variable to set to true iff variable is found
     * @return the value of the environment variable, or "" if not found
     *
     * @deprecated Since YARP 3.4. Use yarp::conf::environment::get_string instead
     */
    YARP_DEPRECATED_MSG("Use yarp::conf::environment::get_string instead")
    static std::string getEnvironment(const char* key,
                                      bool* found = nullptr);

    /**
     * Set or change an environment variable.
     *
     * @param key the variable to set or change
     * @param val the target value
     *
     * @deprecated Since YARP 3.4. Use yarp::conf::environment::set_string instead
     */
    YARP_DEPRECATED_MSG("Use yarp::conf::environment::set_string instead")
    static void setEnvironment(const std::string& key,
                               const std::string& val);

    /**
     * Remove an environment variable.
     *
     * @param key the variable to remove
     *
     * @deprecated Since YARP 3.4. Use yarp::conf::environment::unset instead
     */
    YARP_DEPRECATED_MSG("Use yarp::conf::environment::unset instead")
    static void unsetEnvironment(const std::string& key);
#endif // YARP_NO_DEPRECATED


#ifndef YARP_NO_DEPRECATED // Since YARP 3.3.0
    /**
     *  Get an OS-appropriate directory separator (e.g. "/" on linux)
     *
     *  @deprecated since YARP 3.3.0. Use yarp::conf::filesystem::preferred_separator instead
     */
    YARP_DEPRECATED_MSG("Use yarp::conf::filesystem::preferred_separator instead")
    static std::string getDirectorySeparator();

    /**
     * Get an OS-appropriate path separator (e.g. ":" on linux)
     *
     * @deprecated since YARP 3.3.0. Use yarp::conf::environment::path_separator instead
     */
    YARP_DEPRECATED_MSG("Use yarp::conf::environment::path_separator instead")
    static std::string getPathSeparator();
#endif // YARP_NO_DEPRECATED

    /**
     *
     * Register a carrier to make available at runtime.
     * @param name name of carrier
     * @param dll name of shared library carrier is implemented in
     * @return true if carrier was registered
     *
     */
    static bool registerCarrier(const char* name,
                                const char* dll);

    /**
     * Call wait() on a global mutual-exclusion semaphore allocated by YARP.
     */
    static void lock();

    /**
     * Call post() on a global mutual-exclusion semaphore allocated by YARP.
     */
    static void unlock();

    /**
     *
     * Check where the name server in use expects processes to allocate
     * their own network resources.
     *
     * @return true if local network resource allocation is expected.
     *
     */
    static bool localNetworkAllocation();

    /**
     *
     * Scan for an available name server.
     *
     * @param useDetectedServer If a name server is found, use it.
     * @param scanNeeded True if a network scan was done to find server.
     * @param serverUsed True if a server was found and configured for use.
     *
     * @return address of name server.
     *
     */
    static Contact detectNameServer(bool useDetectedServer,
                                    bool& scanNeeded,
                                    bool& serverUsed);

    /**
     * Set explicitly the nameserver information
     *
     * @param nameServerContact the NameServer contact information (e.g. IP, port)
     * @return true if succeed. False otherwise
     */
    static bool setNameServerContact(Contact& nameServerContact);

    /**
     *
     * Search for a configuration file in YARP's standard config
     * file path.  Return full name of file including path.
     * File not guaranteed to exist.
     *
     * @return full name of file including path
     *
     */
    static std::string getConfigFile(const char* fname);

    /**
     *
     * Under normal operation, YARP has a name server that manages a pool
     * of (socket) ports starting at a point specified by the YARP_PORT_RANGE
     * environment variable (or 10000 if that is not set).
     *
     * @return the beginning of YARP's port range, as specified by
     * the environment variable YARP_PORT_RANGE (or 10000 if not set)
     *
     */
    static int getDefaultPortRange();

    /**
     * Adjust the Qos preferences of a connection.
     * @param src the name of an output port
     * @param dest the name of an input port
     * @param srcStyle the Qos preference of the output port
     * @param destStyle the Qos preference of the input port
     * @return true if the Qos preferences are set correctly
     */
    static bool setConnectionQos(const std::string& src,
                                 const std::string& dest,
                                 const QosStyle& srcStyle,
                                 const QosStyle& destStyle,
                                 bool quiet = true);

    /**
     * Adjust the Qos preferences of a connection.
     * @param src the name of an output port
     * @param dest the name of an input port
     * @param style the Qos preference of both input and output ports
     * @return true if the Qos preferences are set correctly
     */
    static bool setConnectionQos(const std::string& src,
                                 const std::string& dest,
                                 const QosStyle& style,
                                 bool quiet = true);

    /**
     * Gets the Qos preferences of a connection.
     * @param src the name of an output port
     * @param dest the name of an input port
     * @param srcStyle the Qos preference of the output port
     * @param destStyle the Qos preference of the input port
     * @return true if the Qos preferences are gotten correctly
     */
    static bool getConnectionQos(const std::string& src,
                                 const std::string& dest,
                                 QosStyle& srcStyle,
                                 QosStyle& destStyle,
                                 bool quiet = true);

    /**
     * Checks that the port has a valid name.
     * @param portName the name of port
     * @return true if portName is valid
     */
    static bool isValidPortName(const std::string& portName);

    /**
     * Delays the system until a specified connection is established.
     * @param source name of the source port of the connection
     * @param destination name of the dest port of the connection
     * @param quiet flag for verbosity
     * @return true when the connection is finally found
     */
    static bool waitConnection(const std::string& source,
                               const std::string& destination,
                               bool quiet = false);

    /**
     * Delays the system until a specified port is open.
     * @param target name of the port to wait for
     * @param quiet flag for verbosity
     * @return true when the port is finally open
     */
    static bool waitPort(const std::string& target, bool quiet = false);

    /**
     * Just a reminder to sendMessage with temporary output parameter that will
     * be discarded.
     */
    static int sendMessage(const std::string& port,
                           yarp::os::PortWriter& writable,
                           bool silent = false);

    /**
     * Sends a message to the specified port.
     * @param port name of destination port
     * @param writable the object to be written to the port
     * @param output storage string for the output message received from port
     * @param quiet flag for verbosity
     * @return 0 on success
     */
    static int sendMessage(const std::string& port,
                           yarp::os::PortWriter& writable,
                           std::string& output,
                           bool quiet);

    /**
     * Sends a disconnection command to the specified port.
     * @param src the port to send the command to
     * @param dest the port that has to be disconnected
     * @param silent flag for verbosity
     * @return 0 on success
     */
    static int disconnectInput(const std::string& src,
                               const std::string& dest,
                               bool silent = false);

    /**
     * Sends a 'describe yourself' message to a specified port, in order to
     * receive information about the port and its connections.
     * @param target the name of the port to be described
     * @param silent flag for verbosity
     * @return 0 on success
     */
    static int poll(const std::string& target, bool silent = false);
};

/**
 * \ingroup comm_class
 *
 * Utilities for manipulating the YARP network, including initialization
 * and shutdown.
 */
class YARP_init_API Network : public NetworkBase
{
public:
    /**
     * Constructor.  Configures process to use the YARP network.
     * Can be more convenient to use than calling Network::init()
     * directly, since it will clean things up with a call to
     * Network::fini() automatically.
     */
    Network();

    /**
     * Initialize the YARP network using the specified clock.
     * This function will take precedence with respect to the environment
     * variable YARP_CLOCK because it is explixitly required by the user.
     *
     * Calling this function with UNDEFINED_CLOCK is equivalent to use the
     * classic yarp::Network() method and the environment variable will be
     * used.
     *
     * In case YARP_CLOCK_CUSTOM is used, the Clock pointer must point to a valid
     * Clock object already initialized.
     */
    Network(yarp::os::yarpClockType clockType,
            yarp::os::Clock* custom = nullptr);

    /**
     * Destructor.  Disconnects from the YARP network.
     */
    virtual ~Network();

    /**
     * Initialization.  On some operating systems, there are certain
     * start-up tasks that need to be performed, and this method does
     * them.  It is a good idea to call this method near the start of
     * your program, and to call Network::fini towards the end.
     */
    static void init();

    /**
     * Initialization.  Same as init(), but let the user configure which
     * clock shall be used right from the initialization phase.
     */
    static void init(yarp::os::yarpClockType clockType,
                     Clock* custom = nullptr);

    /**
     * Deinitialization.  On some operating systems, there are certain
     * shut-down tasks that need to be performed, and this method does
     * them.  It is a good idea to call Network::init near the start of
     * your program, and to call this method towards the end.
     */
    static void fini();
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_NETWORK_H
