#ifndef _YARP2_PORT_
#define _YARP2_PORT_

#include <yarp/os/Contact.h>
#include <yarp/os/PortReader.h>
#include <yarp/os/PortWriter.h>

/**
 * The main, catch-all namespace for YARP.
 */
namespace yarp {
  /**
   * An interface to the operating system.
   * It includes Port-based communication.
   */
  namespace os {
    class Port;
  }
}

/**
 * A mini-server for network communication.
 * It maintains a dynamic collection of incoming and
 * outgoing connections.  Data coming from any incoming connection can
 * be received by calling Port::read.  Calls to Port::write
 * result in data being sent to all the outgoing connections.
 */
class yarp::os::Port {

public:
  /**
   * Constructor.  The port begins life in a dormant state. Call
   * Port::open to start things happening.
   */
  Port();


  /**
   * Destructor.
   */
  virtual ~Port();

  /**
   * Start port operation, with automatically-chosen network parameters. 
   * The port is registered with the given name, and allocated
   * network resources,
   * by communicating with the YARP name server.
   * @return true iff the port started operation successfully and is now
   * visible on the YARP network
   */
  bool open(const char *name);

  /**
   * Start port operation with user-chosen network parameters.
   * Contact information is supplied by the user rather than
   * the name server.  If the Contact
   * information is incomplete, the name server is used
   * to complete it (set registerName to false if you don't
   * want name server help).
   * @return true iff the port started operation successfully and is now
   * visible on the YARP network
   */
  bool open(const Contact& contact, bool registerName = true);

  /**
   * Add an output connection to the specified port.
   * @param name the name of the target port
   * @return true iff the connection is successfully created
   */
  bool addOutput(const char *name) {
    return addOutput(Contact::byName(name));
  }

  /**
   * Add an output connection to the specified port, using a specified
   * carrier
   * @param name the name of the target port
   * @param carrier the carrier (network protocol) to use, e.g. "tcp", "udp", "mcast", "text", ...
   * @return true iff the connection is successfully created
   */
  bool addOutput(const char *name, const char *carrier) {
    return addOutput(Contact::byName(name).addCarrier(carrier));
  }

  /**
   * Add an output connection to the specified port, using specified
   * network parameters.
   * @param contact information on how to reach the target
   * @return true iff the connection is successfully created
   */
  bool addOutput(const Contact& contact);

  /**
   * Stop port activity.
   */
  void close();

  /**
   * Returns information about how this port can be reached.
   * @return network parameters for this port
   */
  Contact where();

  bool write(PortWriter& writer);

  /**
   * Read an object from the port.
   * @param reader any object that knows how to read itself from a
   * network connection - see for example Bottle
   * @return true iff the object is successfully read
   */
  bool read(PortReader& reader);

  /**
   * set an external writer for port data
   */
  //void setWriter(PortWriter& writer);

  /**
   * set an external reader for port data
   * @param reader the external reader to use
   */
  void setReader(PortReader& reader);

private:
  void *implementation;

};

#endif
